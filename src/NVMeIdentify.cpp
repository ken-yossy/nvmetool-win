#include "WinFunc.h"
#include "NVMeUtils.h"

#include "NVMeIdentify.h"
#include "NVMeIdentifyController.h"
#include "NVMeIdentifyNamespace.h"
#include "NVMeIdentifyActiveNSIDList.h"

int iNVMeIdentify(HANDLE _hDevice)
{
    int iResult = -1;

    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    sprintf_s(strPrompt,
        1024,
        "\n# Input Controller or Namespace Structure (CNS) value (in hex):"
        "\n#    Supported CNS values are:"
        "\n#     %02Xh = Namespace data structure (NSID = 1)"
        "\n#     %02Xh = Controller data structure (CNTID = 0)"
        "\n#     %02Xh = Active Namespace ID list"
        "\n",
        IDENTIFY_CNS_NAMESPACE,
        IDENTIFY_CNS_CONTROLLER,
        IDENTIFY_CNS_ACTIVE_NSID_LIST);

    int iCNS = iGetConsoleInputHex((const char*)strPrompt, strCmd);
    switch (iCNS)
    {
    case IDENTIFY_CNS_NAMESPACE:
        cCmd = cGetConsoleInput("\n# Identify : Namespace data structure (NSID = 1), Press 'y' to continue\n",	strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeIdentifyNamespace(_hDevice, 1);
        }
        break;

    case IDENTIFY_CNS_CONTROLLER:
        cCmd = cGetConsoleInput("\n# Identify : Controller data structure (CNTID = 0), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeIdentifyController(_hDevice);
            if (iResult == 0)
            {
                vPrintNVMeIdentifyControllerData();
            }
        }
        break;

    case IDENTIFY_CNS_ACTIVE_NSID_LIST:
        cCmd = cGetConsoleInput("\n# Identify : Active Namespace ID list, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeIdentifyActiveNSIDList(_hDevice);
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
