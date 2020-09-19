#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "NVMeUtils.h"
#include "NVMeFeaturesHCTM.h"
#include "NVMeFeaturesVWC.h"

int iNVMeSetFeatures(HANDLE _hDevice)
{
    int iResult = -1;
    int iFId = 0;

    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    sprintf_s(strPrompt,
        1024,
        "\n# Input Feature Identifier (in hex):"
        "\n#    Supported Features are:"
        "\n#     %02Xh = Volatile Write Cache"
        "\n#     %02Xh = Host Controlled Thermal Management"
        "\n",
        NVME_FEATURE_VOLATILE_WRITE_CACHE,
        NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT);

    iFId = iGetConsoleInputHex((const char*)strPrompt, strCmd);
    switch (iFId)
    {
    case NVME_FEATURE_VOLATILE_WRITE_CACHE:
        cCmd = cGetConsoleInput("\n# Set Feature - Volatile Write Cache (Feature Identifier = 06h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeSetFeaturesVWC(_hDevice);
        }
        break;

    case NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT:
        cCmd = cGetConsoleInput("\n# Set Feature - Host Controlled Thermal Management (Feature Identifier = 10h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeSetFeaturesHCTM(_hDevice);
        }
        break;

    default:
        printf("\n[E] Feature is not implemented yet.\n");
        break;
    }

    fprintf(stderr, "\n");
    return iResult;
}
