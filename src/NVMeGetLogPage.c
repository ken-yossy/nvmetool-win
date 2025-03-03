#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#include "NVMeCommandSupportedAndEffects.h"
#include "NVMeDeviceSelftestLog.h"
#include "NVMeEnduranceGroup.h"
#include "NVMeErrorInformation.h"
#include "NVMeFwSlotInformation.h"
#include "NVMeGetSupportedLogPages.h"
#include "NVMeGetTelemetry.h"
#include "NVMeIdentifyController.h"
#include "NVMeSMART.h"
#include "NVMeUtils.h"
#include "WinFunc.h"

#ifndef NVME_LOG_PAGE_SUPPORTED_LOG_PAGES
#define NVME_LOG_PAGE_SUPPORTED_LOG_PAGES 0
#endif

int iNVMeGetLogPage(HANDLE _hDevice) {
    int iResult = -1;

    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    sprintf_s(strPrompt, 1024,
              "\n# Input Log Identifier (in hex):"
              "\n#    Supported Log are:"
              "\n#     %02Xh = Supported Log Pages (NVMe 2.0 or later)"
              "\n#     %02Xh = Error Information"
              "\n#     %02Xh = SMART / Health Information"
              "\n#     %02Xh = Firmware Slot Information"
              "\n#     %02Xh = Command Supported and Effects"
              "\n#     %02Xh = Device Self-test"
              "\n#     %02Xh = Telemetry Host-Initiated"
              "\n#     %02Xh = Telemetry Controller-Initiated"
              "\n#     %02Xh = Endurance Group Information"
              "\n#     %02Xh = Endurance Group Event Aggregate"
              "\n",
              NVME_LOG_PAGE_SUPPORTED_LOG_PAGES, NVME_LOG_PAGE_ERROR_INFO,
              NVME_LOG_PAGE_HEALTH_INFO, NVME_LOG_PAGE_FIRMWARE_SLOT_INFO,
              NVME_LOG_PAGE_COMMAND_EFFECTS, NVME_LOG_PAGE_DEVICE_SELF_TEST,
              NVME_LOG_PAGE_TELEMETRY_HOST_INITIATED,
              NVME_LOG_PAGE_TELEMETRY_CTLR_INITIATED,
              NVME_LOG_PAGE_ENDURANCE_GROUP_INFORMATION,
              NVME_LOG_PAGE_ENDURANCE_GROUP_EVENT_AGGREGATE);

    int iLId = iGetConsoleInputHex((const char*)strPrompt, strCmd);
    switch (iLId) {
        case NVME_LOG_PAGE_SUPPORTED_LOG_PAGES:
            if (bIsNVMeV20OrLater()) {
                cCmd = cGetConsoleInput(
                    "\n# Get Log Page - Supported Log Pages (Log Identifier "
                    "00h), Press 'y' to continue\n",
                    strCmd);
                if (cCmd == 'y') {
                    iResult = iNVMeGetSupportedLogPages(_hDevice);
                }
            } else {
                printf(
                    "\n# Supported Log Pages (Log Identifier 00h) is for NVMe "
                    "2.0 or later, ignored");
            }
            break;

        case NVME_LOG_PAGE_ERROR_INFO:
            cCmd = cGetConsoleInput(
                "\n# Get Log Page - Error Information (Log Identifier 01h), "
                "Press 'y' to continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeGetErrorInformation(_hDevice);
            }
            break;

        case NVME_LOG_PAGE_HEALTH_INFO:
            cCmd = cGetConsoleInput(
                "\n# Get Log Page - SMART / Health Information (Log Identifier "
                "02h), Press 'y' to continue\n",
                strCmd);
            if (cCmd == 'y') {
                int iNSID = 0;
                if (g_stController.LPA.SmartPagePerNamespace) {
                    iNSID = iGetConsoleInputDec(
                        "\n# To get controller's log page, press 0, to get "
                        "specified namespace's log page, input NSID of the "
                        "namespace.\n",
                        strCmd);
                    if (iNSID == 0) {
                        cCmd = cGetConsoleInput(
                            "\n# Get controller's log page, Press 'y' to "
                            "continue\n",
                            strCmd);
                        iNSID = NVME_NAMESPACE_ALL;
                    } else {
                        printf("\n# Get namespace's log page (NSID = %08Xh),",
                               iNSID);
                        cCmd = cGetConsoleInput(" Press 'y' to continue\n",
                                                strCmd);
                    }
                } else {
                    printf(
                        "\n[W] This controller does not support the SMART / "
                        "Health Information log page on a per namespace basis, "
                        "get controller's log page\n");
                    iNSID = NVME_NAMESPACE_ALL;
                }

                iResult = iNVMeGetSMART(_hDevice, 1, iNSID);
            }
            break;

        case NVME_LOG_PAGE_FIRMWARE_SLOT_INFO:
            cCmd = cGetConsoleInput(
                "\n# Get Log Page - Firmware Slot Information (Log Identifier "
                "03h), Press 'y' to continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeGetFwSlotInformation(_hDevice);
            }
            break;

        case NVME_LOG_PAGE_COMMAND_EFFECTS:
            if (g_stController.LPA.CommandEffectsLog) {
                cCmd = cGetConsoleInput(
                    "\n# Get Log Page - Command Supported and Effects (Log "
                    "Identifier 05h), Press 'y' to continue\n",
                    strCmd);
                if (cCmd == 'y') {
                    iResult = iNVMeGetCommandSupportedAndEffects(_hDevice);
                }
            } else {
                fprintf(stderr,
                        "\n[E] This controller does not support Command "
                        "Supported and Effects log page, ignore\n");
            }
            break;

        case NVME_LOG_PAGE_DEVICE_SELF_TEST:
            if (g_stController.OACS.DeviceSelfTest) {
                cCmd = cGetConsoleInput(
                    "\n# Get Log Page - Device Self-test (Log Identifier 06h), "
                    "Press 'y' to continue\n",
                    strCmd);
                if (cCmd == 'y') {
                    bool bInProgress;
                    iResult =
                        iNVMeGetDeviceSelftestLog(_hDevice, true, &bInProgress);
                }
            } else {
                fprintf(stderr,
                        "\n[E] This controller does not support Drive "
                        "Self-test command, ignore\n");
            }
            break;

        case NVME_LOG_PAGE_TELEMETRY_HOST_INITIATED: {
            bool bCreate = false;

            if (g_stController.LPA.TelemetrySupport == 0) {
                fprintf(stderr,
                        "\n[W] This controller does not support Telemetry "
                        "function, skip\n");
                break;
            }

            printf(
                "\n# Get Log Page - Telemetry Host-Initiated (Log Identifier "
                "07h)\n");
            printf("\tNot create telemetry host-initiated data, Press 0\n");
            cCmd = cGetConsoleInput(
                "\t    Create telemetry host-initiated data, Press 1\n",
                strCmd);
            if (cCmd == '1') {
                cCmd = cGetConsoleInput(
                    "\n# Get Log Page - Telemetry Host-Initiated (Log "
                    "Identifier 07h) with creating new telemetry data, Press "
                    "'y' to continue\n",
                    strCmd);
                bCreate = true;
            } else if (cCmd == '0') {
                cCmd = cGetConsoleInput(
                    "\n# Get Log Page - Telemetry Host-Initiated (Log "
                    "Identifier 07h) without creating new telemetry data, "
                    "Press 'y' to continue\n",
                    strCmd);
            } else {
                fprintf(stderr, "\n[E] Unknown input, abort.\n");
                break;
            }

            if (cCmd == 'y') {
                iResult = iNVMeGetTelemetryHostInitiated(_hDevice, bCreate);
            }
        } break;

        case NVME_LOG_PAGE_TELEMETRY_CTLR_INITIATED: {
            if (g_stController.LPA.TelemetrySupport == 0) {
                fprintf(stderr,
                        "\n[W] This controller does not support Telemetry "
                        "function, skip\n");
                break;
            }

            cCmd = cGetConsoleInput(
                "\n# Get Log Page - Telemetry Controller-Initiated (Log "
                "Identifier 08h), Press 'y' to continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeGetTelemetryControllerInitiated(_hDevice);
            }
        } break;

        case NVME_LOG_PAGE_ENDURANCE_GROUP_INFORMATION:
            if (g_stController.CTRATT.EnduranceGroups == 0) {
                fprintf(stderr,
                        "\n[W] This controller does not support Endurance "
                        "Group, skip\n");
                break;
            }

            cCmd = cGetConsoleInput(
                "\n# Get Log Page - Endurance Group Information (Log "
                "Identifier 09h), Press 'y' to continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeGetEnduranceGroupInformation(_hDevice);
            }
            break;

        case NVME_LOG_PAGE_ENDURANCE_GROUP_EVENT_AGGREGATE:
            if (bIsNVMeV14OrLater()) {
                if (g_stController.CTRATT.EnduranceGroups) {
                    cCmd = cGetConsoleInput(
                        "\n# Get Log Page - Endurance Group Event Aggregate "
                        "(Log Identifier 0Fh), Press 'y' to continue\n",
                        strCmd);
                    if (cCmd == 'y') {
                        iResult = iNVMeGetEnduranceGroupEventAggregateLogPage(
                            _hDevice);
                    }

                } else {
                    fprintf(stderr,
                            "\n[W] This controller does not support Endurance "
                            "Group, skip\n");
                }
            } else {
                printf("\n# Endurance Group is for NVMe 1.4 or later, ignored");
            }
            break;

        default:
            fprintf(stderr, "\n[E] Command not implemented yet.\n");
            break;
    }

    printf("\n");
    return iResult;
}
