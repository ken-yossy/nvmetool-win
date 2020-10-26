#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <nvme.h>

#include "WinFunc.h"

typedef struct {
    uint8_t NIDT;
    uint8_t NIDL;
    uint8_t Reserved[2];
    uint8_t NID[4092];
} NVME_IDENTIFY_NSID_DESCRIPTOR, * PNVME_IDENTIFY_NSID_DESCRIPTOR;

static void printNVMeIdentifyNSIDDescriptor(PNVME_IDENTIFY_NSID_DESCRIPTOR _pNSData, DWORD _dwNSID)
{
    int iExpectedNIDL = 0;

    printf("[O] Namespace Identifier Type (NIDT): ");
    if (_pNSData->NIDT == 0x1)
    {
        printf("01h (IEEE Extended Unique Identifier, 8 byte)\n");
        iExpectedNIDL = 8;
    }
    else if (_pNSData->NIDT == 0x2)
    {
        printf("02h (Namespace Globally Unique Identifier, 16 byte)\n");
        iExpectedNIDL = 16;
    }
    else if (_pNSData->NIDT == 0x3)
    {
        printf("03h (Namespace UUID, 16 byte)\n");
        iExpectedNIDL = 16;
    }
    else
    {
        printf("%02Xh (Reserved value)\n", _pNSData->NIDT);
    }

    if (iExpectedNIDL == 0)
    {
        fprintf(stderr, "[W] NIDT is unknown, skip");
        return;
    }

    printf("[O] Namespace Identifier Length (NIDL): %02Xh\n", _pNSData->NIDL);
    if (_pNSData->NIDL != iExpectedNIDL)
    {
        fprintf(stderr, "[W] NIDL is unmatched to the length indicated by NIDT, skip\n");
        return;
    }

    printf("[O] Namespace Identifier (NID): ");
    for (int i = 0; i < iExpectedNIDL; i++)
    {
        printf("%c", (unsigned char)(_pNSData->NID[i]));
    }
    printf("\n");
}

int iNVMeIdentifyNSIDDescriptor(HANDLE _hDevice, DWORD _dwNSID)
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
        + sizeof(NVME_IDENTIFY_NSID_DESCRIPTOR);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError( GetLastError(), "malloc" );
        goto error_exit;
    }

    // Initialize query data structure to get Identify Active Namespace ID list.
    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageDeviceProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeIdentify;
    protocolData->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_DESCRIPTOR_NAMESPACE;
    protocolData->ProtocolDataRequestSubValue = _dwNSID;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_IDENTIFY_NSID_DESCRIPTOR);

    // Send request down.
    iResult = iIssueDeviceIoControl(
        _hDevice,
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

    //
    // Validate the returned data.
    //
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)))
    {
        fprintf(stderr, "[E] NVMeIdentifyNamespace: data descriptor header not valid, stop.\n");
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset > sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_IDENTIFY_NSID_DESCRIPTOR)))
    {
        fprintf(stderr, "[E] NVMeIdentifyNamespace: ProtocolData Offset/Length not valid, stop.\n");
        goto error_exit;
    }

    // Identify Namespace
    printNVMeIdentifyNSIDDescriptor(
        (PNVME_IDENTIFY_NSID_DESCRIPTOR)((PCHAR)protocolData + protocolData->ProtocolDataOffset),
        _dwNSID);
    iResult = 0; // succeeded

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
