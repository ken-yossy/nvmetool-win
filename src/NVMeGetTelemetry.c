#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <nvme.h>

#include "WinFunc.h"
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
    printf("\tbyte [511:384] Reason Identifier:\n\n");
    PrintDataBuffer(_pData->ReasonIdentifier, 128);
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
    printf("\tbyte [511:384] Reason Identifier:\n\n");
    PrintDataBuffer(_pData->ReasonIdentifier, 128);
}

static int s_iNVMeGetTelemetryHostInitiated(HANDLE _hDevice, bool _bCreate)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;
    NVME_CDW10_GET_LOG_PAGE_V13 cdw10;
    NVME_CDW11_GET_LOG_PAGE cdw11;
    NVME_CDW12_GET_LOG_PAGE cdw12;
    NVME_CDW13_GET_LOG_PAGE cdw13;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)
        + NVME_TELEMETRY_DATA_BLOCK_SIZE; // sizeof(NVME_TELEMETRY_HOST_INITIATED_LOG)
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError( GetLastError(), "malloc" );
        goto error_exit;
    }

    ZeroMemory(buffer, bufferLength);

    query               = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr   = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData        = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId   = StorageAdapterProtocolSpecificProperty;
    query->QueryType    = PropertyStandardQuery;

    cdw10.AsUlong   = 0; // clear
    cdw10.LID       = NVME_LOG_PAGE_TELEMETRY_HOST_INITIATED;
    cdw10.LSP       = (_bCreate == true) ? 1 : 0;
    cdw10.NUMDL     = NVME_TELEMETRY_DATA_BLOCK_SIZE >> 2; // 512 / 4 (in NVMe, DWORD is 4 byte)

    cdw11.AsUlong   = 0;
    cdw12.LPOL      = 0;
    cdw13.LPOU      = 0;

    protocolData->ProtocolType  = ProtocolTypeNvme;
    protocolData->DataType      = NVMeDataTypeLogPage;

    protocolData->ProtocolDataRequestValue     = cdw10.AsUlong;
    protocolData->ProtocolDataRequestSubValue  = cdw11.AsUlong;
    protocolData->ProtocolDataRequestSubValue2 = cdw12.LPOL;
    protocolData->ProtocolDataRequestSubValue3 = cdw13.LPOU;

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

    printf("\n");

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        fprintf(stderr, "[E] NVMeGetTelemetryHostInitiated: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < NVME_TELEMETRY_DATA_BLOCK_SIZE)) {
        fprintf(stderr, "[E] NVMeGetTelemetryHostInitiated: ProtocolData Offset/Length not valid.\n");
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

/**
 * from STORAGE_PROTOCOL_DATA_SUBVALUE_GET_LOG_PAGE union (ntddstor.h)
 * <https://docs.microsoft.com/ja-jp/windows-hardware/drivers/ddi/ntddstor/ns-ntddstor-storage_protocol_data_subvalue_get_log_page>
 */
#ifndef STORAGE_PROTOCOL_DATA_SUBVALUE_GET_LOG_PAGE
typedef union _STORAGE_PROTOCOL_DATA_SUBVALUE_GET_LOG_PAGE {
    struct {
        ULONG RetainAsynEvent : 1;
        ULONG LogSpecificField : 4;
        ULONG Reserved : 27;
    } DUMMYSTRUCTNAME;
    ULONG  AsUlong;
} STORAGE_PROTOCOL_DATA_SUBVALUE_GET_LOG_PAGE, * PSTORAGE_PROTOCOL_DATA_SUBVALUE_GET_LOG_PAGE;
#endif

static int s_iNVMeGetTelemetryHostInitiatedTEST(HANDLE _hDevice, bool _bCreate)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT protocolDataDescr = NULL;
    NVME_CDW12_GET_LOG_PAGE cdw12;
    NVME_CDW13_GET_LOG_PAGE cdw13;
    STORAGE_PROTOCOL_DATA_SUBVALUE_GET_LOG_PAGE u32Opt;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA_EXT)
        + NVME_TELEMETRY_DATA_BLOCK_SIZE; // sizeof(NVME_TELEMETRY_HOST_INITIATED_LOG)
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError(GetLastError(), "malloc");
        goto error_exit;
    }

    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT)query->AdditionalParameters;

    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    //    cdw10.NUMDL     = NVME_TELEMETRY_DATA_BLOCK_SIZE >> 2; // 512 / 4 (in NVMe, DWORD is 4 byte)
    //  where is NUMDL...?

    cdw12.LPOL = 0;
    cdw13.LPOU = 0;
    u32Opt.RetainAsynEvent     = 1; //0; // clear asynchronous event
    u32Opt.LogSpecificField    = (_bCreate == true) ? 1 : 0;
    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType     = NVMeDataTypeLogPage;

    protocolData->ProtocolDataValue     = NVME_LOG_PAGE_TELEMETRY_HOST_INITIATED;
    protocolData->ProtocolDataSubValue  = cdw12.LPOL; // This will be passed as the lower 32 bit of log page offset if controller supports
    protocolData->ProtocolDataSubValue2 = cdw13.LPOU; // This will be passed as the higher 32 bit of log page offset if controller supports
    protocolData->ProtocolDataSubValue3 = u32Opt.AsUlong;

    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA_EXT);
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

    printf("\n");

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        fprintf(stderr, "[E] NVMeGetTelemetryHostInitiated: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA_EXT)) ||
        (protocolData->ProtocolDataLength < NVME_TELEMETRY_DATA_BLOCK_SIZE)) {
        fprintf(stderr, "[E] NVMeGetTelemetryHostInitiated: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    {
        PNVME_TELEMETRY_HOST_INITIATED_LOG aLog = (PNVME_TELEMETRY_HOST_INITIATED_LOG)((PCHAR)protocolData + protocolData->ProtocolDataOffset);
        fprintf(stderr, "[I] EXPERIMENTAL\n");
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
        vPrintSystemError(GetLastError(), "malloc");
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

    printf("\n");

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        fprintf(stderr, "[E] NVMeGetTelemetryControllerInitiated: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_TELEMETRY_HOST_INITIATED_LOG))) {
        fprintf(stderr, "[E] NVMeGetTelemetryControllerInitiated: ProtocolData Offset/Length not valid.\n");
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

#include <ntddstor.h>

#define STATUS_DATA_HEADER_LENGTH 512

static void s_vPrintDeviceInternalStatusData(PDEVICE_INTERNAL_STATUS_DATA _pData)
{
    printf("[I] Device Internal Status Data:\n");
    printf("[I]\t          Version = 0x%08X\n", _pData->Version);
    printf("[I]\t             Size = 0x%08X\n", _pData->Size);
    printf("[I]\t      T10VendorId = 0x%016llX\n", _pData->T10VendorId);
    printf("[I]\t   DataSet1Length = 0x%08X\n", _pData->DataSet1Length);
    printf("[I]\t   DataSet2Length = 0x%08X\n", _pData->DataSet2Length);
    printf("[I]\t   DataSet3Length = 0x%08X\n", _pData->DataSet3Length);
    printf("[I]\t   DataSet4Length = 0x%08X\n", _pData->DataSet4Length);
    printf("[I]\tStatusDataVersion = 0x%08X\n", _pData->StatusDataVersion);

    printf("[I]\t ReasonIdentifier:\n\n");
    PrintDataBuffer(_pData->ReasonIdentifier, 128);

    printf("[I]\t StatusDataLength = 0x%08X\n", _pData->StatusDataLength);
    printf("[I]\t StatusData:\n\n");
    PrintDataBuffer(_pData->StatusData, STATUS_DATA_HEADER_LENGTH);
}

static int s_iNVMeGetTelemetryHostInitiatedWithDeviceInternalLog(HANDLE _hDevice)
{
    int     iResult = -1;
    ULONG   returnedLength = 0;

    GET_DEVICE_INTERNAL_STATUS_DATA_REQUEST request;
    PDEVICE_INTERNAL_STATUS_DATA outBuffer = NULL;
    ULONG outBufferLength = 0;

    // Allocate buffer for use.
    outBufferLength = FIELD_OFFSET(DEVICE_INTERNAL_STATUS_DATA, StatusData) + STATUS_DATA_HEADER_LENGTH;
    outBuffer = malloc(outBufferLength);
    if (outBuffer == NULL)
    {
        vPrintSystemError(GetLastError(), "malloc");
        goto error_exit;
    }

    ZeroMemory(&request, sizeof(GET_DEVICE_INTERNAL_STATUS_DATA_REQUEST));
    ZeroMemory(outBuffer, outBufferLength);

    // from https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddstor/ns-ntddstor-get_device_internal_status_data_request
    request.Version = sizeof(GET_DEVICE_INTERNAL_STATUS_DATA_REQUEST);
    request.Size = sizeof(GET_DEVICE_INTERNAL_STATUS_DATA_REQUEST);
    request.RequestDataType = DeviceCurrentInternalStatusDataHeader;

    // Send request down.
    iResult = iIssueDeviceIoControl(_hDevice,
        IOCTL_STORAGE_GET_DEVICE_INTERNAL_LOG,
        &request,
        sizeof(GET_DEVICE_INTERNAL_STATUS_DATA_REQUEST),
        outBuffer,
        outBufferLength,
        &returnedLength,
        NULL
    );

    if (iResult) goto error_exit;

    printf("\n");
    s_vPrintDeviceInternalStatusData(outBuffer);

error_exit:

    if (outBuffer != NULL)
    {
        free(outBuffer);
    }

    return iResult;
}

int iNVMeGetTelemetryHostInitiated(HANDLE _hDevice, bool _bCreate)
{
    int iResult = -1;
    char cOpt = 'n';
    char strCmd[256];

    if (_bCreate && bCanUseGetDeviceInternalLog())
    {
        cOpt = cGetConsoleInput("#\t[EXPERIMENTAL] Using IOCTL_STORAGE_GET_DEVICE_INTERNAL_LOG may be supported; if you want to use it, press 'y'\n", strCmd);
    }

    if (cOpt == 'y')
    {
        iResult = s_iNVMeGetTelemetryHostInitiatedWithDeviceInternalLog(_hDevice);
    }
    else if (cOpt == 'e')
    {
        iResult = s_iNVMeGetTelemetryHostInitiatedTEST(_hDevice, _bCreate);
    }
    else
    {
        iResult = s_iNVMeGetTelemetryHostInitiated(_hDevice, _bCreate);
    }

    return iResult;
}

