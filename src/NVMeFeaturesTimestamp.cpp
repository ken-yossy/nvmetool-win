#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeGetFeatures.h"

static int siNVMeGetFeaturesTimestamp(HANDLE _hDevice, NVME_CDW10_GET_FEATURES _cdw10, uint64_t *_pullResult)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    uint64_t ullTimestamp = 0ULL;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)
        + sizeof(uint64_t);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError(GetLastError(), "malloc");
        return iResult;
    }

    ZeroMemory(buffer, bufferLength);

    query               = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr   = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData        = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId   = StorageAdapterProtocolSpecificProperty;
    query->QueryType    = PropertyStandardQuery;

    protocolData->ProtocolType                  = ProtocolTypeNvme;
    protocolData->DataType                      = NVMeDataTypeFeature;
    protocolData->ProtocolDataRequestValue      = _cdw10.AsUlong;
    protocolData->ProtocolDataRequestSubValue   = 0;
    protocolData->ProtocolDataOffset            = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength            = sizeof(uint64_t);

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

    if (iResult == 0)
    {
        // Validate the returned data.
        if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
            (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)))
        {
            printf("[E] NVMeGetFeaturesTimestamp: Data descriptor header not valid.\n");
            iResult = -1; // error
        }
        else
        {
            (*_pullResult) = ullTimestamp; // copy result
        }
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

static void s_vNVMePrintTimestamp(uint64_t _ullData)
{
    uint64_t    ullTimestamp;
    uint8_t     uAttributes;
    uint8_t     uOrigin;

    ullTimestamp    = _ullData & 0x0000FFFFFFFFFFFF; // byte [5:0]
    uAttributes     = (_ullData >> 48) & 0xFF; // byte 6

    printf("\t\tbyte [  5:  0] 0x%llX = Timestamp\n", ullTimestamp);
    if (uAttributes & 0x1)
    {
        printf("\t\tbyte [      6] bit [      0] 1 = The controller may have stopped counting\n");
    }
    else
    {
        printf("\t\tbyte [      6] bit [      0] 0 = The controller counted time continuously\n");
    }

    uOrigin     = (uAttributes >> 1) & 0x5;
    if (uOrigin == 0)
    {
        printf("\t\tbyte [      6] bit [  3:  1] 0 = The Timestamp field was initialzed to `0' by a Controller Level Reset\n");
    }
    else if (uOrigin == 1)
    {
        printf("\t\tbyte [      6] bit [  3:  1] 1 = The Timestamp field was initialzed with a Timestamp value using a Set Features command\n");
    }
    else
    {
        printf("\t\tbyte [      6] bit [  3:  1] %d = (unknown)\n", uOrigin);
    }
}

int iNVMeGetFeaturesTimestamp(HANDLE _hDevice)
{
    int iResult = -1;
    NVME_CDW10_GET_FEATURES cdw10;
    uint64_t ullCurrentSetting, ullDefaultSetting, ullSavedSetting;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY Cap;

    // 1. Get current value
    cdw10.FID   = NVME_FEATURE_TIMESTAMP;
    cdw10.SEL   = NVME_FEATURE_VALUE_CURRENT;
    iResult     = siNVMeGetFeaturesTimestamp(_hDevice, cdw10, &ullCurrentSetting);
    if (iResult != 0) return iResult;

    // 2. Get default value
    cdw10.SEL   = NVME_FEATURE_VALUE_DEFAULT;
    iResult     = siNVMeGetFeaturesTimestamp(_hDevice, cdw10, &ullDefaultSetting);
    if (iResult != 0) return iResult;

    // 3. Get saved value
    cdw10.SEL   = NVME_FEATURE_VALUE_SAVED;
    iResult     = siNVMeGetFeaturesTimestamp(_hDevice, cdw10, &ullSavedSetting);
    if (iResult != 0) return iResult;

    // 4. get supported capabilities
    iResult = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TIMESTAMP, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&Cap);
    if (iResult != 0) return iResult;

    printf("\n[I] Timestamp:\n");

    printf("\tCurrent Setting:\n");
    s_vNVMePrintTimestamp(ullCurrentSetting);

    printf("\tDefault Setting:\n");
    s_vNVMePrintTimestamp(ullDefaultSetting);

    printf("\tSaved Setting:\n");
    s_vNVMePrintTimestamp(ullSavedSetting);

    vNVMeGetFeaturesShowCapabilities(Cap);

    return iResult;
}
