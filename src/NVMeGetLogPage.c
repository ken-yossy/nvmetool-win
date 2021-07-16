#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeUtils.h"
#include "NVMeSMART.h"
#include "NVMeCommandSupportedAndEffects.h"
#include "NVMeErrorInformation.h"
#include "NVMeFwSlotInformation.h"
#include "NVMeDeviceSelftestLog.h"
#include "NVMeGetTelemetry.h"
#include "NVMeEnduranceGroup.h"
#include "NVMeIdentifyController.h"

int iNVMeGetLogPage(HANDLE _hDevice)
{
    int iResult = -1;

    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    sprintf_s(strPrompt,
        1024,
        "\n# Input Log Identifier (in hex):"
        "\n#    Supported Log are:"
        "\n#     %02Xh = Error Information"
        "\n#     %02Xh = SMART / Health Information"
        "\n#     %02Xh = Firmware Slot Information"
        "\n#     %02Xh = Command Supported and Effects"
        "\n#     %02Xh = Device Self-test"
        "\n#     %02Xh = Telemetry Host-Initiated"
        "\n#     %02Xh = Telemetry Controller-Initiated"
        "\n#     %02Xh = Endurance Group Information"
        "\n",
        NVME_LOG_PAGE_ERROR_INFO,
        NVME_LOG_PAGE_HEALTH_INFO,
        NVME_LOG_PAGE_FIRMWARE_SLOT_INFO,
        NVME_LOG_PAGE_COMMAND_EFFECTS,
        NVME_LOG_PAGE_DEVICE_SELF_TEST,
        NVME_LOG_PAGE_TELEMETRY_HOST_INITIATED,
        NVME_LOG_PAGE_TELEMETRY_CTLR_INITIATED,
        NVME_LOG_PAGE_ENDURANCE_GROUP_INFORMATION);

    int iLId = iGetConsoleInputHex((const char*)strPrompt, strCmd);
    switch (iLId)
    {
    case NVME_LOG_PAGE_ERROR_INFO:
        cCmd = cGetConsoleInput("\n# Get Log Page - Error Information (Log Identifier 01h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeGetErrorInformation(_hDevice);
        }
        break;

    case NVME_LOG_PAGE_HEALTH_INFO:
        cCmd = cGetConsoleInput("\n# Get Log Page - SMART / Health Information (Log Identifier 02h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            int iNSID = 0;
            if (g_stController.LPA.SmartPagePerNamespace)
            {
                iNSID = iGetConsoleInputDec("\n# To get controller's log page, press 0, to get specified namespace's log page, input NSID of the namespace.\n", strCmd);
                if (iNSID == 0)
                {
                    cCmd = cGetConsoleInput("\n# Get controller's log page, Press 'y' to continue\n", strCmd);
                    iNSID = NVME_NAMESPACE_ALL;
                }
                else
                {
                    printf("\n# Get namespace's log page (NSID = %08Xh),", iNSID);
                    cCmd = cGetConsoleInput(" Press 'y' to continue\n", strCmd);
                }
            }
            else
            {
                printf("\n[W] This controller does not support the SMART / Health Information log page on a per namespace basis, get controller's log page\n");
                iNSID = NVME_NAMESPACE_ALL;
            }

            iResult = iNVMeGetSMART(_hDevice, 1, iNSID);
        }
        break;

    case NVME_LOG_PAGE_FIRMWARE_SLOT_INFO:
        cCmd = cGetConsoleInput("\n# Get Log Page - Firmware Slot Information (Log Identifier 03h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeGetFwSlotInformation(_hDevice);
        }
        break;

    case NVME_LOG_PAGE_COMMAND_EFFECTS:
        if (g_stController.LPA.CommandEffectsLog)
        {
            cCmd = cGetConsoleInput("\n# Get Log Page - Command Supported and Effects (Log Identifier 05h), Press 'y' to continue\n", strCmd);
            if (cCmd == 'y')
            {
                iResult = iNVMeGetCommandSupportedAndEffects(_hDevice);
            }
        }
        else
        {
            fprintf(stderr, "\n[E] This controller does not support Command Supported and Effects log page, ignore\n");
        }
        break;

    case NVME_LOG_PAGE_DEVICE_SELF_TEST:
        if (g_stController.OACS.DeviceSelfTest)
        {
            cCmd = cGetConsoleInput("\n# Get Log Page - Device Self-test (Log Identifier 06h), Press 'y' to continue\n", strCmd);
            if (cCmd == 'y')
            {
                bool bInProgress;
                iResult = iNVMeGetDeviceSelftestLog(_hDevice, true, &bInProgress);
            }
        }
        else
        {
            fprintf(stderr, "\n[E] This controller does not support Drive Self-test command, ignore\n");
        }
        break;

    case NVME_LOG_PAGE_TELEMETRY_HOST_INITIATED:
        {
            bool bCreate = false;

            if (g_stController.LPA.TelemetrySupport == 0)
            {
                fprintf(stderr, "\n[W] This controller does not support Telemetry function, skip\n");
                break;
            }

            printf("\n# Get Log Page - Telemetry Host-Initiated (Log Identifier 07h)\n");
            printf("\tNot create telemetry host-initiated data, Press 0\n");
            cCmd = cGetConsoleInput("\t    Create telemetry host-initiated data, Press 1\n", strCmd);
            if (cCmd == '1')
            {
                cCmd = cGetConsoleInput("\n# Get Log Page - Telemetry Host-Initiated (Log Identifier 07h) with creating new telemetry data, Press 'y' to continue\n", strCmd);
                bCreate = true;
            }
            else if (cCmd == '0')
            {
                cCmd = cGetConsoleInput("\n# Get Log Page - Telemetry Host-Initiated (Log Identifier 07h) without creating new telemetry data, Press 'y' to continue\n", strCmd);
            }
            else
            {
                fprintf(stderr, "\n[E] Unknown input, abort.\n");
                break;
            }

            if (cCmd == 'y')
            {
                    iResult = iNVMeGetTelemetryHostInitiated(_hDevice, bCreate);
            }
        }
        break;

    case NVME_LOG_PAGE_TELEMETRY_CTLR_INITIATED:
    {
        if (g_stController.LPA.TelemetrySupport == 0)
        {
            fprintf(stderr, "\n[W] This controller does not support Telemetry function, skip\n");
            break;
        }

        cCmd = cGetConsoleInput("\n# Get Log Page - Telemetry Controller-Initiated (Log Identifier 08h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeGetTelemetryControllerInitiated(_hDevice);
        }
    }
    break;

    case NVME_LOG_PAGE_ENDURANCE_GROUP_INFORMATION:
        if (g_stController.CTRATT.EnduranceGroups == 0)
        {
            fprintf(stderr, "\n[W] This controller does not support Endurance Group, skip\n");
            break;
        }

        cCmd = cGetConsoleInput("\n# Get Log Page - Endurance Group Information (Log Identifier 09h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeGetEnduranceGroupInformation(_hDevice);
        }
        break;

    default:
        fprintf(stderr, "\n[E] Command not implemented yet.\n");
        break;
    }

    printf("\n");
    return iResult;
}
