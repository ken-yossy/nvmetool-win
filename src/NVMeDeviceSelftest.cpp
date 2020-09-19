#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeUtils.h"
#include "NVMeIdentifyController.h"
#include "NVMeDeviceSelftestLog.h"

enum
{
    DST_CODE_SHORT_TEST     = 0x1,
    DST_CODE_EXTENDED_TEST  = 0x2,
    DST_CODE_ABORT_TEST     = 0xF
};

static int siNVMeDST(HANDLE _hDevice, int _iAction)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROTOCOL_COMMAND pCmd = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROTOCOL_COMMAND, Command)
        + STORAGE_PROTOCOL_COMMAND_LENGTH_NVME;
    buffer = malloc(bufferLength);

    if (buffer != NULL)
    {
        ZeroMemory(buffer, bufferLength);
        pCmd = (PSTORAGE_PROTOCOL_COMMAND)buffer;

        pCmd->Length                        = sizeof(STORAGE_PROTOCOL_COMMAND);
        pCmd->Version                       = STORAGE_PROTOCOL_STRUCTURE_VERSION;
        pCmd->ProtocolType                  = ProtocolTypeNvme;
        pCmd->Flags                         = STORAGE_PROTOCOL_COMMAND_FLAG_ADAPTER_REQUEST;
        pCmd->CommandLength                 = STORAGE_PROTOCOL_COMMAND_LENGTH_NVME;
        pCmd->ErrorInfoLength               = 0;
        pCmd->ErrorInfoOffset               = 0;
        pCmd->DataFromDeviceBufferOffset    = 0;
        pCmd->DataFromDeviceTransferLength  = 0;
        pCmd->TimeOutValue                  = 10;
        pCmd->CommandSpecific               = STORAGE_PROTOCOL_SPECIFIC_NVME_ADMIN_COMMAND;

        PNVME_COMMAND pNvmeCmd      = (PNVME_COMMAND)pCmd->Command;
        pNvmeCmd->CDW0.OPC          = NVME_ADMIN_COMMAND_DEVICE_SELF_TEST;
        pNvmeCmd->NSID              = NVME_NAMESPACE_ALL;
        pNvmeCmd->u.GENERAL.CDW10   = _iAction; // 1h: start short test, 2h: start extended test, Fh: abort test

        // Send request down.
        iResult = iIssueDeviceIoControl(_hDevice,
            IOCTL_STORAGE_PROTOCOL_COMMAND,
            buffer,
            bufferLength,
            buffer,
            bufferLength,
            &returnedLength,
            NULL
        );
        fprintf(stderr, "\n");
    }
    else
    {
        vUtilPrintSystemError(GetLastError(), "malloc");
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

int iNVMeDeviceSelftest(HANDLE _hDevice)
{
    int iResult = -1;

    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    if (!g_stController.OACS.DeviceSelfTest)
    {
        printf("[W] This controller does not support Device Self-test command, ignore\n");
        return iResult;
    }

    sprintf_s(strPrompt,
        1024,
        "\n# Input Self-test Code (in hex):"
        "\n#    Supported code are:"
        "\n#     %02Xh = Start a short device self-test operation"
        "\n#     %02Xh = Start an extended device self-test operation"
        "\n#     %02Xh = Abort device self-test operation"
        "\n",
        DST_CODE_SHORT_TEST,
        DST_CODE_EXTENDED_TEST,
        DST_CODE_ABORT_TEST);

    int iStc = iGetConsoleInputHex((const char*)strPrompt, strCmd);
    int iResDSTLog = -1;
    bool bInProgress = false;

    iResDSTLog = iNVMeGetDeviceSelftestLog(_hDevice, false, &bInProgress);
    if (iResDSTLog == -1)
    {
        printf("[E] Device Self-test - Abort operation\n");
        return iResult;
    }

    switch (iStc)
    {
    case DST_CODE_SHORT_TEST:
        if (bInProgress)
        {
            printf("[E] Device Self-test - Another test is in progress, skip operation\n");
        }
        else
        {
            cCmd = cGetConsoleInput("\n# Device Self-test - Start a short test, Press 'y' to continue\n", strCmd);
            if (cCmd == 'y')
            {
                iResult = siNVMeDST(_hDevice, iStc);
            }
        }
        break;

    case DST_CODE_EXTENDED_TEST:
        if (bInProgress)
        {
            printf("[E] Device Self-test - Another test is in progress, skip operation\n");
        }
        else
        {
            printf("# Device Self-test - Start an extended test\n");
            printf("#\tExtended device self-test takes about %d minutes according to the Identify data\n", g_stController.EDSTT);
            cCmd = cGetConsoleInput("\n#\tIf you want to start an extended test, Press 'y' to continue\n", strCmd);
            if (cCmd == 'y')
            {
                iResult = siNVMeDST(_hDevice, iStc);
            }
        }
        break;

    case DST_CODE_ABORT_TEST:
        if (bInProgress)
        {
            cCmd = cGetConsoleInput("\n# Device Self-test - Abort test, Press 'y' to continue\n", strCmd);
            if (cCmd == 'y')
            {
                iResult = siNVMeDST(_hDevice, iStc);
            }
        }
        else
        {
            printf("[E] Device Self-test - No test is in progress, skip operation\n");
        }
        break;

    default:
        printf("\n[E] Unsupported code, ignore.\n");
        break;
    }

    fprintf(stderr, "\n");
    return iResult;
}

