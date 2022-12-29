#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <nvme.h>

#include "NVMeUtils.h"
#include "WinFunc.h"
#include "NVMeIdentifyController.h"

typedef struct {
    union {
        struct {
            uint8_t SpareCapacityIsBelowThreshold : 1;
            uint8_t Reserved0 : 1;
            uint8_t ReliabilityDegraded : 1;
            uint8_t NamespaceIsReadOnly : 1;
            uint8_t Reserved1 : 4;
        } DUMMYSTRUCTNAME;

        uint8_t AsByte;
    } CriticalWarning;

    uint8_t Reserved0[2];

    uint8_t AvailableSpare;
    uint8_t AvailableSpareThreshold;
    uint8_t PercentageUsed;

    uint8_t Reserved1[26];

    uint64_t    EnduranceEstimateL;
    uint64_t    EnduranceEstimateH;
    uint64_t    DataUnitsReadL;
    uint64_t    DataUnitsReadH;
    uint64_t    DataUnitsWrittenL;
    uint64_t    DataUnitsWrittenH;
    uint64_t    MediaUnitsWrittenL;
    uint64_t    MediaUnitsWrittenH;
    uint64_t    HostReadCommandsL;
    uint64_t    HostReadCommandsH;
    uint64_t    HostWriteCommandsL;
    uint64_t    HostWriteCommandsH;
    uint64_t    MediaAndDataIntegrityErrorsL;
    uint64_t    MediaAndDataIntegrityErrorsH;
    uint64_t    NumberOfErrorInformationLogEntriesL;
    uint64_t    NumferOfErrorInformationLogEntriesH;

    uint8_t Reserved2[352];
} NVME_ENDURANCE_GROUP_INFO, *PNVME_ENDURANCE_GROUP_INFO;

static void s_vPrintNVMeEnduranceGroupInformation(PNVME_ENDURANCE_GROUP_INFO _pData)
{
    printf("[I] Endurance Group Information :\n");

    printf("\n");
    printf("byte [      0] 0x%02X = Critical Warning\n", _pData->CriticalWarning.AsByte);
    if (_pData->CriticalWarning.NamespaceIsReadOnly == 1)
    {
        printf("\tbit [    3] 0x%02X = All namespaces in this endurance group are placed in read only mode\n", _pData->CriticalWarning.NamespaceIsReadOnly);
    }
    else
    {
        printf("\tbit [    3] 0x%02X = No namespace in this endurance group is placed in read only mode\n", _pData->CriticalWarning.NamespaceIsReadOnly);
    }

    if (_pData->CriticalWarning.ReliabilityDegraded == 1)
    {
        printf("\tbit [    2] 0x%02X = Reliability of this endurance group is degraded\n", _pData->CriticalWarning.ReliabilityDegraded);
    }
    else
    {
        printf("\tbit [    2] 0x%02X = Reliability of this endurance group is not degraded\n", _pData->CriticalWarning.ReliabilityDegraded);
    }

    if (_pData->CriticalWarning.SpareCapacityIsBelowThreshold == 1)
    {
        printf("\tbit [    0] 0x%02X = Available spare capacity of this endurance group is fallen below the threshold\n", _pData->CriticalWarning.SpareCapacityIsBelowThreshold);
    }
    else
    {
        printf("\tbit [    0] 0x%02X = Available spare capacity of this endurance group is above the threshold\n", _pData->CriticalWarning.SpareCapacityIsBelowThreshold);
    }
    printf("\n");

    printf("byte [      3] 0x%02X = Available Spare\n", _pData->AvailableSpare);
    printf("byte [      4] 0x%02X = Available Spare Threshold\n", _pData->AvailableSpareThreshold);
    printf("byte [      5] 0x%02X = Percentage Used\n", _pData->PercentageUsed);

    printf("byte [ 47: 32] 0x%08llX%08llX = Endurance Estimate\n", _pData->EnduranceEstimateH, _pData->EnduranceEstimateL);
    printf("byte [ 63: 48] 0x%08llX%08llX = Data Units Read\n", _pData->DataUnitsReadH, _pData->DataUnitsReadL);
    printf("byte [ 79: 64] 0x%08llX%08llX = Data Units Written\n", _pData->DataUnitsWrittenH, _pData->DataUnitsWrittenL);
    printf("byte [ 95: 80] 0x%08llX%08llX = Media Units Written\n", _pData->MediaUnitsWrittenH, _pData->MediaAndDataIntegrityErrorsL);
    printf("byte [111: 96] 0x%08llX%08llX = Host Read Commands\n", _pData->HostReadCommandsH, _pData->HostReadCommandsL);
    printf("byte [127:112] 0x%08llX%08llX = Host Write Commands\n", _pData->HostWriteCommandsH, _pData->HostWriteCommandsL);
    printf("byte [143:128] 0x%08llX%08llX = Media and Data Integrity Errors\n", _pData->MediaAndDataIntegrityErrorsH, _pData->MediaAndDataIntegrityErrorsL);
    printf("byte [159:144] 0x%08llX%08llX = Number of Error Information Entries\n", _pData->NumferOfErrorInformationLogEntriesH, _pData->NumberOfErrorInformationLogEntriesL);
}

int iNVMeGetEnduranceGroupInformation(HANDLE _hDevice)
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
        + sizeof(NVME_ENDURANCE_GROUP_INFO);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError( GetLastError(), "malloc" );
        goto error_exit;
    }

    uint32_t u32EGID = 0;
    {
        char strCmd[256];
        char cCmd;
        u32EGID = iGetConsoleInputHex("\n# Input Endurance Group Identifier (as Hex) ", strCmd);
        printf("# Endurance Group Identifier = 0x%04X\n", u32EGID);
        cCmd = cGetConsoleInput("# Press 'y' to continue\n", strCmd);
        if ((cCmd != 'y') && (cCmd != 'Y')) return iResult;
    }

    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageDeviceProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeLogPage;
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_ENDURANCE_GROUP_INFORMATION;
    protocolData->ProtocolDataRequestSubValue3 = u32EGID;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_ENDURANCE_GROUP_INFO);

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
        fprintf(stderr, "[E] NVMeGetEnduranceGroupInformation: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_ENDURANCE_GROUP_INFO))) {
        fprintf(stderr, "[E] NVMeGetEnduranceGroupInformation: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    {
        PNVME_ENDURANCE_GROUP_INFO aLog = (PNVME_ENDURANCE_GROUP_INFO)((uint8_t *)protocolData + protocolData->ProtocolDataOffset);
        s_vPrintNVMeEnduranceGroupInformation(aLog);
        iResult = 0; // succeeded
    }

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

#define NUM_MAX_ENDURANCE_GROUP_ID  (252) // FIXME!!! to make the size of structure below 512 bytes

typedef struct
{
    uint64_t    m_ullNumEntries;
    uint16_t    m_ausEntry[NUM_MAX_ENDURANCE_GROUP_ID];
} NVME_ENDURANCE_GROUP_EVENT_AGGREGATE_LOG, *PNVME_ENDURANCE_GROUP_EVENT_AGGREGATE_LOG;

int iNVMeGetEnduranceGroupEventAggregateLogPage(HANDLE _hDevice)
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
        + sizeof(NVME_ENDURANCE_GROUP_EVENT_AGGREGATE_LOG);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError(GetLastError(), "malloc");
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

    // Check the following page for appropriate values for "RequestValue"s.
    // STORAGE_PROTOCOL_NVME_DATA_TYPE enumeration (ntddstor.h)
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddstor/ne-ntddstor-_storage_protocol_nvme_data_type
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_ENDURANCE_GROUP_EVENT_AGGREGATE; // 0Fh
    protocolData->ProtocolDataRequestSubValue = 0; // lower 32-bit of the offset
    protocolData->ProtocolDataRequestSubValue2 = 0; // higher 32-bit of the offset
    // Subvalue3 and Subvalue4 are zero

    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_ENDURANCE_GROUP_EVENT_AGGREGATE_LOG);

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
        fprintf(stderr, "[E] NVMeGetSupportedLogPages: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_ENDURANCE_GROUP_EVENT_AGGREGATE_LOG))) {
        fprintf(stderr, "[E] NVMeGetSupportedLogPages: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    PNVME_ENDURANCE_GROUP_EVENT_AGGREGATE_LOG pData = (PNVME_ENDURANCE_GROUP_EVENT_AGGREGATE_LOG)((PCHAR)protocolData + protocolData->ProtocolDataOffset);

    printf("[I] Endurance Group Event Aggregate Log Page:\n");
    printf("    byte [  7:  0] %llu = Number of Entries\n", pData->m_ullNumEntries);

    if (pData->m_ullNumEntries != 0)
    {
        if (NUM_MAX_ENDURANCE_GROUP_ID < pData->m_ullNumEntries)
        {
            printf("[W] This drive has %llu valid entries, but this tool only show %d entries\n", pData->m_ullNumEntries, NUM_MAX_ENDURANCE_GROUP_ID);
        }

        for (int i = 0; ( i < pData->m_ullNumEntries ) && (i < NUM_MAX_ENDURANCE_GROUP_ID); i++)
        {
            printf("    byte [%3u:%3u] Entry %3u: %3u = Endurance Group ID\n", i*2+8+1, i*2+8, i, pData->m_ausEntry[ i ]);
        }
    }

    iResult = 0; // succeeded;

error_exit:
    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

