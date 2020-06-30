#include "WinFunc.h"
#include "NVMeUtils.h"

#include "NVMeIdentify.h"
#include "NVMeIdentifyController.h"
#include "NVMeIdentifyNamespace.h"

int iNVMeIdentify(HANDLE _hDevice)
{
    int iResult = -1;

    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    sprintf_s(strPrompt,
        1024,
        "\n# Input Controller or Namespace Structure (CNS) value (in hex):"
        "\n#    Supported CNS = %02xh, %02xh\n",
        IDENTIFY_CNS_NAMESPACE, IDENTIFY_CNS_CONTROLLER);

    int iCNS = iGetConsoleInputHex((const char*)strPrompt, strCmd);
    switch (iCNS)
    {
    case IDENTIFY_CNS_NAMESPACE:
        cCmd = cGetConsoleInput("\n# Identify - Active Namespace (1), Press 'y' to continue\n",	strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeIdentifyNamespace(_hDevice, 1);
        }
        break;

    case IDENTIFY_CNS_CONTROLLER:
        cCmd = cGetConsoleInput("\n# Identify - Controller, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeIdentifyController(_hDevice);
            if (iResult == 0)
            {
                vPrintNVMeIdentifyControllerData();
            }
        }
        break;

    default:
        printf("\n[E] Command not implemented yet.\n");
        break;
    }

    fprintf(stderr, "\n");
    return iResult;
}
