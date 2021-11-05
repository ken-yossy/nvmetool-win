#include <windows.h>
#include <stdio.h>
#include <ntddstor.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeUtils.h"

/**
 *   iNVMeAdminVendorSpecificCommand: FOR REFERENCE ONLY
 *     Sample function for issuing a vendor specific command to NVMe drives
 *
 *   [CAUTION]
 *     To use this function as template, you have to edit some codes as your purpose.
 *
 *     Generally, the specification (behavior, parameters, and outputs) of vendor specific commands are
 *      not disclosed to users, it is highly dangerous to issue the command without complete spec of it.
 *
 *     It is confirmed that CDW[10-15] are successfully passed to drives.
 * 
 *     You should carefully read and need to understand Microsoft's article:
 *       "Working with NVMe drives" <https://docs.microsoft.com/en-us/windows/win32/fileio/working-with-nvme-devices>
 */
int iNVMeAdminVendorSpecificCommand(HANDLE _hDevice, int _opcode)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;
    PSTORAGE_PROTOCOL_COMMAND protocolCommand;
    PNVME_COMMAND command;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROTOCOL_COMMAND, Command)
        + sizeof(NVME_COMMAND)
        + sizeof(NVME_IDENTIFY_CONTROLLER_DATA); // TEST DATA; should be corrected
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError(GetLastError(), "malloc");
        goto error_exit;
    }

    ZeroMemory(buffer, bufferLength);
    protocolCommand = (PSTORAGE_PROTOCOL_COMMAND)buffer;

    protocolCommand->Version = STORAGE_PROTOCOL_STRUCTURE_VERSION;
    protocolCommand->Length = sizeof(STORAGE_PROTOCOL_COMMAND);
    protocolCommand->ProtocolType = ProtocolTypeNvme;
    protocolCommand->Flags = STORAGE_PROTOCOL_COMMAND_FLAG_ADAPTER_REQUEST;
    protocolCommand->CommandLength = STORAGE_PROTOCOL_COMMAND_LENGTH_NVME;
    protocolCommand->ErrorInfoLength = 0;
    protocolCommand->DataFromDeviceTransferLength = sizeof(NVME_IDENTIFY_CONTROLLER_DATA); // TEST DATA; should be corrected
    protocolCommand->TimeOutValue = 10;
    protocolCommand->ErrorInfoOffset = 0;
    protocolCommand->DataFromDeviceBufferOffset = offsetof(STORAGE_PROTOCOL_COMMAND, Command) + sizeof(NVME_COMMAND);
    protocolCommand->CommandSpecific = STORAGE_PROTOCOL_SPECIFIC_NVME_ADMIN_COMMAND; // TEST DATA; should be corrected

    command = (PNVME_COMMAND)protocolCommand->Command;

    command->CDW0.OPC = _opcode;           // TEST DATA; should be corrected
    command->u.GENERAL.CDW10 = 0x1;        // TEST DATA; should be corrected
    command->u.GENERAL.CDW11 = 0x12345678; // TEST DATA; should be corrected
    command->u.GENERAL.CDW12 = 0x90ABCDEF; // TEST DATA; should be corrected
    command->u.GENERAL.CDW13 = 0x23456789; // TEST DATA; should be corrected
    command->u.GENERAL.CDW14 = 0xABCDEF01; // TEST DATA; should be corrected
    command->u.GENERAL.CDW15 = 0x34567890; // TEST DATA; should be corrected

    // Send request down.
    iResult = DeviceIoControl(
        _hDevice,
        IOCTL_STORAGE_PROTOCOL_COMMAND,
        buffer,
        bufferLength,
        buffer,
        bufferLength,
        &returnedLength,
        NULL
    );

    if ( returnedLength == bufferLength )
    {
        fprintf(stderr, "[I] NVMeAdminVendorSpecificCommand: expected data is retrieved.\n");
    }
    else
    {
        fprintf(stderr, "[E] NVMeAdminVendorSpecificCommand: expected data is NOT retrieved; data length returned = %d\n", returnedLength);
        iResult = -1; // error
    }

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
