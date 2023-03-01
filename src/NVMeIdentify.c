#include <stdio.h>
#include <windows.h>

#include "NVMeIdentifyActiveNSIDList.h"
#include "NVMeIdentifyController.h"
#include "NVMeIdentifyControllerData.h"
#include "NVMeIdentifyIOCSNamespace.h"
#include "NVMeIdentifyNSIDDescriptor.h"
#include "NVMeIdentifyNamespace.h"
#include "NVMeUtils.h"

int iNVMeIdentify(HANDLE _hDevice) {
    int iResult = -1;

    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    sprintf_s(
        strPrompt, 1024,
        "\n# Input Controller or Namespace Structure (CNS) value (in hex):"
        "\n#    Supported CNS values are:"
        "\n#     %02Xh = Namespace data structure (NSID = 1)"
        "\n#     %02Xh = Controller data structure (CNTID = 0)"
        "\n#     %02Xh = Active Namespace ID list"
        "\n#     %02Xh = Namespace Identification Descriptor (NSID = 1)"
        "\n#     %02Xh = I/O Command Set Specific Identify Namespace (NSID = 1)"
        "\n",
        NVME_IDENTIFY_CNS_SPECIFIC_NAMESPACE, NVME_IDENTIFY_CNS_CONTROLLER,
        NVME_IDENTIFY_CNS_ACTIVE_NAMESPACES,
        NVME_IDENTIFY_CNS_DESCRIPTOR_NAMESPACE,
        NVME_IDENTIFY_CNS_SPECIFIC_NAMESPACE_IO_COMMAND_SET);

    int iCNS = iGetConsoleInputHex((const char*)strPrompt, strCmd);
    switch (iCNS) {
        case NVME_IDENTIFY_CNS_SPECIFIC_NAMESPACE:
            cCmd = cGetConsoleInput(
                "\n# Identify : Namespace data structure (NSID = 1), Press 'y' "
                "to continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeIdentifyNamespace(_hDevice, 1, true);
            }
            break;

        case NVME_IDENTIFY_CNS_CONTROLLER:
            cCmd = cGetConsoleInput(
                "\n# Identify : Controller data structure (CNTID = 0), Press "
                "'y' to continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeIdentifyController(_hDevice);
                if (iResult == 0) {
                    vPrintNVMeIdentifyControllerData();
                }
            }
            break;

        case NVME_IDENTIFY_CNS_ACTIVE_NAMESPACES:
            cCmd = cGetConsoleInput(
                "\n# Identify : Active Namespace ID list, Press 'y' to "
                "continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeIdentifyActiveNSIDList(_hDevice);
            }
            break;

        case NVME_IDENTIFY_CNS_DESCRIPTOR_NAMESPACE:
            cCmd = cGetConsoleInput(
                "\n# Identify : Namespace Identification Descriptor (NSID = "
                "1), Press 'y' to continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeIdentifyNSIDDescriptor(_hDevice, (DWORD)1);
            }
            break;

        case NVME_IDENTIFY_CNS_SPECIFIC_NAMESPACE_IO_COMMAND_SET:
            cCmd = cGetConsoleInput(
                "\n# Identify : I/O Command Set specific Identify Namespace "
                "(NSID = 5), Press 'y' to continue\n",
                strCmd);
            if (cCmd == 'y') {
                iResult = iNVMeIdentifyIOCSNamespace(_hDevice, (uint32_t)1);
            }
            break;

        default:
            fprintf(stderr, "\n[E] Command not implemented yet.\n");
            break;
    }

    printf("\n");
    return iResult;
}
