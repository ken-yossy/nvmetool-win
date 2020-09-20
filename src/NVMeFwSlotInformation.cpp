#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "WinFunc.h"

typedef struct {
    union {
        struct {
            UCHAR   ActiveSlot : 3;
            UCHAR   Reserved0 : 1;
            UCHAR   PendingActivateSlot : 3;
            UCHAR   Reserved1 : 1;
        } DUMMYSTRUCTNAME;

        BYTE AsByte;
    } AFI;

    UCHAR    Reserved0[7];

    UCHAR    FRS[7][8];
    UCHAR    Reserved1[448];

} NVME_FIRMWARE_SLOT_INFO_LOG12, * PNVME_FIRMWARE_SLOT_INFO_LOG12;

static void s_vPrintNVMeFwSlotInformation(PNVME_FIRMWARE_SLOT_INFO_LOG12 _pData)
{
    printf("[I] Firmware Slot Information :\n");

    printf("\n");
    printf("byte [    0] 0x%02X = Active Firmware Info (AFI)\n", _pData->AFI.AsByte);
    printf("\tbit [ 6: 4] 0x%02X = Pending Activate Slot\n", _pData->AFI.PendingActivateSlot);
    printf("\tbit [ 2: 0] 0x%02X = Active Slot\n", _pData->AFI.ActiveSlot);
    printf("\n");

    for (int i = 1; i <= 7; i++)
    {
        if (_pData->FRS[i - 1][0] == '\0')
        {
            printf("byte [%2d:%2d] (not valid) = Firmware Revision for Slot %d\n", 8 * (i + 1) - 1, 8 * i, i);
        }
        else
        {
            printf("byte [%2d:%2d] %s = Firmware Revision for Slot %d\n", 8 * (i + 1) - 1, 8 * i, _pData->FRS[i - 1], i);
        }
    }
}

int iNVMeGetFwSlotInformation(HANDLE _hDevice)
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
        + sizeof(NVME_FIRMWARE_SLOT_INFO_LOG12);
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
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_FIRMWARE_SLOT_INFO;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_FIRMWARE_SLOT_INFO_LOG12);

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
        fprintf(stderr, "[E] NVMeGetFwSlotInformation: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_FIRMWARE_SLOT_INFO_LOG12))) {
        fprintf(stderr, "[E] NVMeGetFwSlotInformation: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    {
        PNVME_FIRMWARE_SLOT_INFO_LOG12 aLog = (PNVME_FIRMWARE_SLOT_INFO_LOG12)((PCHAR)protocolData + protocolData->ProtocolDataOffset);
        s_vPrintNVMeFwSlotInformation(aLog);
        iResult = 0; // succeeded
    }

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
