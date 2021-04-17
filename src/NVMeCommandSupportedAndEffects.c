#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "NVMeIdentifyController.h"
#include "WinFunc.h"

static const char* strAdminCommand[256] =
{
    "Delete I/O Submission Queue",  // 00h
    "Create I/O Submission Queue",  // 01h
    "Get Log Page",                 // 02h
    "(reserved)",                   // 03h
    "Delete I/O Completion Queue",  // 04h
    "Create I/O Completion Queue",  // 05h
    "Identify",                     // 06h
    "(reserved)",                   // 07h
    "Abort",                        // 08h
    "Set Features",                 // 09h
    "Get Features",                 // 0Ah
    "(reserved)",                   // 0Bh
    "Asynchronous Event Request",   // 0Ch
    "Namespace Management",         // 0Dh
    "(reserved)", "(reserved)",     // 0Eh, 0Fh
    "Firmware Commit",              // 10h
    "Firmware Image Download",      // 11h
    "(reserved)", "(reserved)",     // 12h, 13h
    "Device Self-test",             // 14h
    "Namespace Attachment",         // 15h
    "(reserved)", "(reserved)",     // 16h, 17h
    "Keep Alive",                   // 18h
    "Directive Send",               // 19h
    "Directive Receive",            // 1Ah
    "(reserved)",                   // 1Bh
    "Virtualization Management",    // 1Ch
    "NVMe-MI Send",                 // 1Dh
    "NVMe-MI Receive",              // 1Eh
    "(reserved)",                   // 1Fh
    "Capacity Management",          // 20h <rev 2.0>
    "(reserved)", "(reserved)", "(reserved)", // 21h -- 23h
    "Lockdown",                     // 24h <rev 2.0>
    "(reserved)", "(reserved)", "(reserved)", // 25h -- 27h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 28h -- 2Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 2Ch -- 2Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 30h -- 33h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 34h -- 37h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 38h -- 3Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 3Ch -- 3Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 40h -- 43h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 44h -- 47h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 48h -- 4Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 4Ch -- 4Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 50h -- 53h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 54h -- 57h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 58h -- 5Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 5Ch -- 5Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 60h -- 63h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 64h -- 67h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 68h -- 6Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 6Ch -- 6Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 70h -- 73h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 74h -- 77h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 78h -- 7Bh
    "Doorbell Buffer Config",                               // 7Ch
    "(reserved)", "(reserved)",                             // 7Dh, 7Eh
    "(refer to the NVMe over Fabrics specification)",       // 7Fh
    "Format NVM",                                           // 80h
    "Security Send",                                        // 81h
    "Security Receive",                                     // 82h
    "(reserved)",                                           // 83h
    "Sanitize",                                             // 84h
    "(reserved)",                                           // 85h
    "Get LBA Status",                                       // 86h <rev1.4>
    "(reserved)",                                           // 87h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 88h -- 8Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 8Ch -- 8Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 90h -- 93h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 94h -- 97h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 98h -- 9Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 9Ch -- 9Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // A0h -- A3h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // A4h -- A7h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // A8h -- ABh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // ACh -- AFh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // B0h -- B3h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // B4h -- B7h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // B8h -- BBh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // BCh -- BFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // C0h -- C3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // C4h -- C7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // C8h -- CBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // CCh -- CFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // D0h -- D3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // D4h -- D7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // D8h -- DBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // DCh -- DFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // E0h -- E3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // E4h -- E7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // E8h -- EBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // ECh -- EFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // F0h -- F3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // F4h -- F7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // F8h -- FBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // FCh -- FFh
};

static const char* strNVMCommand[256] =
{
    "Flush",                        // 00h
    "Write",                        // 01h
    "Read",                         // 02h
    "(reserved)",                   // 03h
    "Write Uncorrectable",          // 04h
    "Compare",                      // 05h
    "(reserved)", "(reserved)",     // 06h, 07h
    "Write Zeroes",                 // 08h
    "Dataset Management",           // 09h
    "(reserved)", "(reserved)",     // 0A, 0Bh
    "Verify",                       // 0Ch <rev1.4>
    "Reservation Register",         // 0Dh
    "Reservation Report",           // 0Eh
    "(reserved)", "(reserved)",     // 0Fh, 10h
    "Reservation Acquire",          // 11h
    "(reserved)", "(reserved)", "(reserved)", // 12h, 13h, 14h
    "Reservation Release",          // 15h
    "(reserved)", "(reserved)",     // 16h, 17h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 18h -- 1Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 1Ch -- 1Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 20h -- 23h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 24h -- 27h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 28h -- 2Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 2Ch -- 2Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 30h -- 33h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 34h -- 37h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 38h -- 3Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 3Ch -- 3Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 40h -- 43h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 44h -- 47h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 48h -- 4Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 4Ch -- 4Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 50h -- 53h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 54h -- 57h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 58h -- 5Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 5Ch -- 5Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 60h -- 63h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 64h -- 67h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 68h -- 6Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 6Ch -- 6Fh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 70h -- 73h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 74h -- 77h
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 78h -- 7Bh
    "(reserved)", "(reserved)", "(reserved)", "(reserved)", // 7Ch -- 7Fh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // 80h -- 83h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // 84h -- 87h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // 88h -- 8Bh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // 8Ch -- 8Fh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // 90h -- 93h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // 94h -- 97h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // 98h -- 9Bh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // 9Ch -- 9Fh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // A0h -- A3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // A4h -- A7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // A8h -- ABh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // ACh -- AFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // B0h -- B3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // B4h -- B7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // B8h -- BBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // BCh -- BFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // C0h -- C3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // C4h -- C7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // C8h -- CBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // CCh -- CFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // D0h -- D3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // D4h -- D7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // D8h -- DBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // DCh -- DFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // E0h -- E3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // E4h -- E7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // E8h -- EBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // ECh -- EFh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // F0h -- F3h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // F4h -- F7h
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // F8h -- FBh
    "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", "(Vendor specific)", // FCh -- FFh
};

static void vPrintNVMeCSEData(PNVME_COMMAND_EFFECTS_LOG _pData)
{
    printf("[I] Command Supported and Effect Log: Admin Command\n");
    for (int i = 0; i < 256; i++)
    {
        NVME_COMMAND_EFFECTS_DATA Data = _pData->ACS[i];
        if (Data.CSUPP == 0)
        {
            continue;
        }
        else
        {
            printf("[I] Opecode = 0x%02X ", i);
            printf(" [ %s ] Supported (bit 0: 1)\n", strAdminCommand[i]);
        }

        switch (Data.CSE)
        {
        case 0:
            printf("\tbit [ 18: 16] 0 = No command submission or execution restriction\n");
            break;

        case 1:
            printf("\tbit [ 18: 16] 1 = May be submitted when there is no other outstanding command to the same namespace and another command should not be submitted to the same namespace until this command is complete\n");
            break;

        case 2:
            printf("\tbit [ 18: 16] 2 = May be submitted when there is no other outstanding command to any namespace and another command should not be submitted to any namespace until this command is complete\n");
            break;

        default:
            break;
        }

        if (Data.CCC)
        {
            printf("\tbit [      4] 1 = May change controller capabilities\n");
        }
        else
        {
            printf("\tbit [      4] 0 = Does not modify controller capabilities\n");
        }

        if (Data.NIC)
        {
            printf("\tbit [      3] 1 = May change the number of namespaces or capabilities for multiple namespaces\n");
        }
        else
        {
            printf("\tbit [      3] 0 = Does not modify the number of namespaces or capabilities for multiple namespaces\n");
        }

        if (Data.NCC)
        {
            printf("\tbit [      2] 1 = May change the capabilities of a single namespace\n");
        }
        else
        {
            printf("\tbit [      2] 0 = Does not modify any namespace capabilities for the specified namespace\n");
        }

        if (Data.LBCC)
        {
            printf("\tbit [      1] 1 = May modify logical block content in one or more namespaces\n");
        }
        else
        {
            printf("\tbit [      1] 0 = Does not modify logical block content in any namespace\n");
        }
    }

    printf("\n");
    printf("[I] Command Supported and Effect Log: NVM Command\n");
    for (int i = 0; i < 256; i++)
    {
        NVME_COMMAND_EFFECTS_DATA Data = _pData->IOCS[i];
        if (Data.CSUPP == 0)
        {
            continue;
        }
        else
        {
            printf("[I] Opecode = 0x%02X ", i);
            printf(" [ %s ] Supported (bit 0: 1)\n", strNVMCommand[i]);
        }

        switch (Data.CSE)
        {
        case 0:
            printf("\tbit [ 18: 16] 0 = No command submission or execution restriction\n");
            break;

        case 1:
            printf("\tbit [ 18: 16] 1 = May be submitted when there is no other outstanding command to the same namespace and another command should not be submitted to the same namespace until this command is complete\n");
            break;

        case 2:
            printf("\tbit [ 18: 16] 2 = May be submitted when there is no other outstanding command to any namespace and another command should not be submitted to any namespace until this command is complete\n");
            break;

        default:
            break;
        }

        if (Data.CCC)
        {
            printf("\tbit [      4] 1 = May change controller capabilities\n");
        }
        else
        {
            printf("\tbit [      4] 0 = Does not modify controller capabilities\n");
        }

        if (Data.NIC)
        {
            printf("\tbit [      3] 1 = May change the number of namespaces or capabilities for multiple namespaces\n");
        }
        else
        {
            printf("\tbit [      3] 0 = Does not modify the number of namespaces or capabilities for multiple namespaces\n");
        }

        if (Data.NCC)
        {
            printf("\tbit [      2] 1 = May change the capabilities of a single namespace\n");
        }
        else
        {
            printf("\tbit [      2] 0 = Does not modify any namespace capabilities for the specified namespace\n");
        }

        if (Data.LBCC)
        {
            printf("\tbit [      1] 1 = May modify logical block content in one or more namespaces\n");
        }
        else
        {
            printf("\tbit[       1] 0 = Does not modify logical block content in any namespace\n");
        }
    }
}

int iNVMeGetCommandSupportedAndEffects(HANDLE _hDevice)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)
        + sizeof(NVME_COMMAND_EFFECTS_LOG);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError( GetLastError(), "malloc" );
        goto error_exit;
    }

    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageDeviceProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeLogPage;
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_COMMAND_EFFECTS;
    protocolData->ProtocolDataRequestSubValue = NVME_NAMESPACE_ALL;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_COMMAND_EFFECTS_LOG);

    // Send request down.
    iResult = iIssueDeviceIoControl(_hDevice,
        IOCTL_STORAGE_QUERY_PROPERTY,
        buffer,
        bufferLength,
        buffer,
        bufferLength,
        &returnedLength,
        NULL
    );

    if (iResult) goto error_exit;

    printf("\n");

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        fprintf(stderr, "[E] getNVMeCommandSupportedAndEffects: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_COMMAND_EFFECTS_LOG))) {
        fprintf(stderr, "[E] getNVMeCommandSupportedAndEffects: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    vPrintNVMeCSEData((PNVME_COMMAND_EFFECTS_LOG)((PCHAR)protocolData + protocolData->ProtocolDataOffset));
    iResult = 0; // succeeded

error_exit:
    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
