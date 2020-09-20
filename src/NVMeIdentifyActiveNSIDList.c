#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "WinFunc.h"

int iNVMeIdentifyActiveNSIDList(HANDLE _hDevice)
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
        + sizeof(ULONG) * 1024;
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError( GetLastError(), "malloc" );
        goto error_exit;
    }

    // Initialize query data structure to get Identify Active Namespace ID list.
    ZeroMemory(buffer, bufferLength);

    query               = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr   = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData        = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId   = StorageAdapterProtocolSpecificProperty;
    query->QueryType    = PropertyStandardQuery;

    protocolData->ProtocolType                  = ProtocolTypeNvme;
    protocolData->DataType                      = NVMeDataTypeIdentify;
    protocolData->ProtocolDataRequestValue      = NVME_IDENTIFY_CNS_ACTIVE_NAMESPACES;
    protocolData->ProtocolDataRequestSubValue   = 0; // to retrieve all IDs
    protocolData->ProtocolDataOffset            = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength            = sizeof(ULONG) * 1024;

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
        fprintf(stderr, "[E] NVMeIdentifyActiveNSIDList: data descriptor header not valid, stop.\n");
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset > sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(ULONG) * 1024) )
    {
        fprintf(stderr, "[E] NVMeIdentifyActiveNSIDList: ProtocolData Offset/Length not valid, stop.\n");
        goto error_exit;
    }

    // Dump list
    {
        printf("[I] === Active Namespace ID list start ===\n");
        PULONG aIDList = (PULONG)((PCHAR)protocolData + protocolData->ProtocolDataOffset);
        for (int i = 0; i < 1024; i++)
        {
            if ( aIDList[i] )
            {
                printf("\t%4d\n", aIDList[i]);
            }
        }
        printf("[I] === Active Namespace ID list end ===\n");
    }

    iResult = 0; // succeeded

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
