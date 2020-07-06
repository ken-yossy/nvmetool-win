#include "WinFunc.h"
#include <stdio.h>
#include <nvme.h>

#include "NVMeUtils.h"
#include "NVMeGetLogPage.h"
#include "NVMeSMART.h"
#include "NVMeCommandSupportedAndEffects.h"
#include "NVMeErrorInformation.h"
#include "NVMeFwSlotInformation.h"
#include "NVMeDeviceSelftestLog.h"

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
        "\n",
        NVME_LOG_PAGE_ERROR_INFO,
        NVME_LOG_PAGE_HEALTH_INFO,
        NVME_LOG_PAGE_FIRMWARE_SLOT_INFO,
        NVME_LOG_PAGE_COMMAND_EFFECTS,
        NVME_LOG_PAGE_DEVICE_SELF_TEST);

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
            iResult = iNVMeGetSMART(_hDevice, true);
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
        cCmd = cGetConsoleInput("\n# Get Log Page - Command Supported and Effects (Log Identifier 05h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeGetCommandSupportedAndEffects(_hDevice, 1);
        }
        break;

    case NVME_LOG_PAGE_DEVICE_SELF_TEST:
        cCmd = cGetConsoleInput("\n# Get Log Page - Device Self-test (Log Identifier 06h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            bool bInProgress;
            iResult = iNVMeGetDeviceSelftestLog(_hDevice, true, &bInProgress);
        }
        break;

    default:
        printf("\n[E] Command not implemented yet.\n");
        break;
    }

    fprintf(stderr, "\n");
    return iResult;
}
