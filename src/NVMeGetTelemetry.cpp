#include "WinFunc.h"
#include <iostream>
#include <nvme.h>
#include "NVMeUtils.h"

static void s_vPrintNVMeTelemetryHostInitiated(PNVME_TELEMETRY_HOST_INITIATED_LOG _pData)
{
    printf("[I] Telemetry Host Initiated :\n");
    if (_pData->LogIdentifier != 7)
    {
        printf("\tbyte [      0] %d = Log Identifier (invalid)\n", _pData->LogIdentifier);
        return;
    }
    else
    {
        printf("\tbyte [      0] %d = Log Identifier\n", _pData->LogIdentifier);
    }

    printf("\tbyte [  7:  5] %02X%02X%02X = IEEE OUI Identifier\n", _pData->OrganizationID[0], _pData->OrganizationID[1], _pData->OrganizationID[2]);
    printf("\tbyte [  9:  8] %u = Telemetry Host-Initiated Data Area 1 Last Block\n", _pData->Area1LastBlock);
    printf("\tbyte [ 11: 10] %u = Telemetry Host-Initiated Data Area 2 Last Block\n", _pData->Area2LastBlock);
    printf("\tbyte [ 13: 12] %u = Telemetry Host-Initiated Data Area 3 Last Block\n", _pData->Area3LastBlock);

    if (_pData->ControllerInitiatedDataAvailable)
    {
        printf("\tbyte [    382] %u = Telemetry Controller-Initiated Data is available\n", _pData->ControllerInitiatedDataAvailable);
    }
    else
    {
        printf("\tbyte [    382] %u = Telemetry Controller-Initiated Data is not available\n", _pData->ControllerInitiatedDataAvailable);
    }

    printf("\tbyte [    383] %u = Telemetry Controller-Initiated Data Generation Number\n", _pData->ControllerInitiatedDataGenerationNumber);
}

static void s_vPrintNVMeTelemetryControllerInitiated(PNVME_TELEMETRY_HOST_INITIATED_LOG _pData)
{
    printf("[I] Telemetry Controller Initiated :\n");
    if (_pData->LogIdentifier != 8)
    {
        printf("\tbyte [      0] %d = Log Identifier (invalid)\n", _pData->LogIdentifier);
        return;
    }
    else
    {
        printf("\tbyte [      0] %d = Log Identifier\n", _pData->LogIdentifier);
    }

    printf("\tbyte [  7:  5] %02X%02X%02X = IEEE OUI Identifier\n", _pData->OrganizationID[0], _pData->OrganizationID[1], _pData->OrganizationID[2]);
    printf("\tbyte [  9:  8] %u = Telemetry Controller-Initiated Data Area 1 Last Block\n", _pData->Area1LastBlock);
    printf("\tbyte [ 11: 10] %u = Telemetry Controller-Initiated Data Area 2 Last Block\n", _pData->Area2LastBlock);
    printf("\tbyte [ 13: 12] %u = Telemetry Controller-Initiated Data Area 3 Last Block\n", _pData->Area3LastBlock);

    if (_pData->ControllerInitiatedDataAvailable)
    {
        printf("\tbyte [    382] %u = Telemetry Controller-Initiated Data is available\n", _pData->ControllerInitiatedDataAvailable);
    }
    else
    {
        printf("\tbyte [    382] %u = Telemetry Controller-Initiated Data is not available\n", _pData->ControllerInitiatedDataAvailable);
    }

    printf("\tbyte [    383] %u = Telemetry Controller-Initiated Data Generation Number\n", _pData->ControllerInitiatedDataGenerationNumber);
}

int iNVMeGetTelemetryHostInitiated(HANDLE _hDevice, bool _bCreate)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT protocolDataDescr = NULL;
    NVME_CDW10_GET_LOG_PAGE_V13 cdw10;
    NVME_CDW11_GET_LOG_PAGE cdw11;
    NVME_CDW12_GET_LOG_PAGE cdw12;
    NVME_CDW13_GET_LOG_PAGE cdw13;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA_EXT)
        + NVME_TELEMETRY_DATA_BLOCK_SIZE; // sizeof(NVME_TELEMETRY_HOST_INITIATED_LOG)
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vUtilPrintSystemError( GetLastError(), "malloc" );
        goto error_exit;
    }

    ZeroMemory(buffer, bufferLength);

    query               = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr   = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT)buffer;
    protocolData        = (PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT)query->AdditionalParameters;

    query->PropertyId   = StorageAdapterProtocolSpecificProperty;
    query->QueryType    = PropertyStandardQuery;

    cdw10.AsUlong   = 0; // clear
    cdw10.LID       = NVME_LOG_PAGE_TELEMETRY_HOST_INITIATED;
    cdw10.LSP       = ( _bCreate == true ) ? 1 : 0;
    cdw10.NUMDL     = NVME_TELEMETRY_DATA_BLOCK_SIZE >> 2; // 512 / 4 (in NVMe, DWORD is 4 byte)

    cdw11.AsUlong   = 0;
    cdw12.LPOL      = 0;
    cdw13.LPOU      = 0;

    protocolData->ProtocolType  = ProtocolTypeNvme;
    protocolData->DataType      = NVMeDataTypeLogPage;

    protocolData->ProtocolDataValue     = cdw10.AsUlong;
    protocolData->ProtocolDataSubValue  = cdw11.AsUlong;
    protocolData->ProtocolDataSubValue  = cdw12.LPOL;
    protocolData->ProtocolDataSubValue  = cdw13.LPOU;

    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = NVME_TELEMETRY_DATA_BLOCK_SIZE; // sizeof(NVME_TELEMETRY_HOST_INITIATED_LOG)

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

    fprintf(stderr, "\n");

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        printf("[E] NVMeGetTelemetryHostInitiated: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < NVME_TELEMETRY_DATA_BLOCK_SIZE)) {
        printf("[E] NVMeGetTelemetryHostInitiated: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    {
        PNVME_TELEMETRY_HOST_INITIATED_LOG aLog = (PNVME_TELEMETRY_HOST_INITIATED_LOG)((PCHAR)protocolData + protocolData->ProtocolDataOffset);
        s_vPrintNVMeTelemetryHostInitiated(aLog);
        iResult = 0; // succeeded
    }

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

int iNVMeGetTelemetryControllerInitiated(HANDLE _hDevice)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT protocolDataDescr = NULL;
    NVME_CDW10_GET_LOG_PAGE_V13 cdw10;
    NVME_CDW11_GET_LOG_PAGE cdw11;
    NVME_CDW12_GET_LOG_PAGE cdw12;
    NVME_CDW13_GET_LOG_PAGE cdw13;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA_EXT)
        + NVME_TELEMETRY_DATA_BLOCK_SIZE; // sizeof(NVME_TELEMETRY_HOST_INITIATED_LOG)
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vUtilPrintSystemError(GetLastError(), "malloc");
        goto error_exit;
    }

    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT)query->AdditionalParameters;

    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    cdw10.AsUlong   = 0; // clear
    cdw10.LID       = NVME_LOG_PAGE_TELEMETRY_CTLR_INITIATED;
    cdw10.NUMDL     = NVME_TELEMETRY_DATA_BLOCK_SIZE >> 2; // 512 / 4 (in NVMe, DWORD is 4 byte)

    cdw11.AsUlong   = 0;
    cdw12.LPOL      = 0;
    cdw13.LPOU      = 0;

    protocolData->ProtocolType  = ProtocolTypeNvme;
    protocolData->DataType      = NVMeDataTypeLogPage;

    protocolData->ProtocolDataValue     = cdw10.AsUlong;
    protocolData->ProtocolDataSubValue  = cdw11.AsUlong;
    protocolData->ProtocolDataSubValue  = cdw12.LPOL;
    protocolData->ProtocolDataSubValue  = cdw13.LPOU;

    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = NVME_TELEMETRY_DATA_BLOCK_SIZE; // sizeof(NVME_TELEMETRY_HOST_INITIATED_LOG)

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

    fprintf(stderr, "\n");

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        printf("[E] NVMeGetTelemetryControllerInitiated: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_TELEMETRY_HOST_INITIATED_LOG))) {
        printf("[E] NVMeGetTelemetryControllerInitiated: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    {
        PNVME_TELEMETRY_HOST_INITIATED_LOG aLog = (PNVME_TELEMETRY_HOST_INITIATED_LOG)((PCHAR)protocolData + protocolData->ProtocolDataOffset);
        s_vPrintNVMeTelemetryControllerInitiated(aLog);
        iResult = 0; // succeeded
    }

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
