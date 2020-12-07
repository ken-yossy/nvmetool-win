#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeUtils.h"
#include "NVMeSMART.h" // to use g_stSMARTLog
#include "NVMeIdentifyController.h" // to use g_stController

#include "NVMeFeaturesAPST.h"
#include "NVMeFeaturesTimestamp.h"
#include "NVMeFeaturesLBARange.h"
#include "NVMeFeaturesHMB.h"

// NVMeGetFeature32() : used for Get Feature command with 32bit fixed return value
int iNVMeGetFeature32(HANDLE _hDevice, DWORD _dwFId, int _iType, DWORD _dwCDW11, uint32_t* _pulData)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;
    NVME_CDW10_GET_FEATURES cdw10 = { 0 };

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
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
    cdw10.FID = _dwFId;
    cdw10.SEL = _iType;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeFeature;
    // protocolData->ProtocolDataRequestValue = (DWORD)((_iType << 8) | _dwFId);
    protocolData->ProtocolDataRequestValue = (DWORD)(cdw10.AsUlong);
    protocolData->ProtocolDataRequestSubValue = _dwCDW11;
    protocolData->ProtocolDataOffset = 0;
    protocolData->ProtocolDataLength = 0;

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

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        fprintf(stderr, "[E] NVMeGetFeature: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    (*_pulData) = (uint32_t)(protocolDataDescr->ProtocolSpecificData.FixedProtocolReturnData);
    iResult = 0; // succeeded;

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

void vNVMeGetFeaturesShowCapabilities(NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY _ulCaps)
{
    printf("\tCapabilities: this feature is\n");
    if (_ulCaps.MOD)
    {
        printf("\t\tbit [      2] 1 = changable\n");
    }
    else
    {
        printf("\t\tbit [      2] 0 = not changable\n");
    }

    if (_ulCaps.NSS)
    {
        printf("\t\tbit [      1] 1 = namespace specific\n");
    }
    else
    {
        printf("\t\tbit [      1] 0 = for entire controller\n");
    }

    if (_ulCaps.SAVE)
    {
        printf("\t\tbit [      0] 1 = savable\n");
    }
    else
    {
        printf("\t\tbit [      0] 0 = not savable\n");
    }
}

static int NVMeGetFeaturesArbitration(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Arbitration:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ARBITRATION, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ARBITRATION, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ARBITRATION, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ARBITRATION, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [ 31: 24] High Priority Weight (HPW)\n"
        "\t\tCurrent = %d (= %d)\n"
        "\t\tDefault = %d (= %d)\n"
        "\t\tSaved   = %d (= %d)\n",
        (ulCurrentData >> 24) & 0xFF, ((ulCurrentData >> 24) & 0xFF) + 1,
        (ulDefaultData >> 24) & 0xFF, ((ulDefaultData >> 24) & 0xFF) + 1,
        (ulSavedData >> 24) & 0xFF, ((ulSavedData >> 24) & 0xFF) + 1);
    printf("\tbit [ 23: 16] Medium Priority Weight (MPW)\n"
        "\t\tCurrent = %d (= %d)\n"
        "\t\tDefault = %d (= %d)\n"
        "\t\tSaved   = %d (= %d)\n",
        (ulCurrentData >> 16) & 0xFF, ((ulCurrentData >> 16) & 0xFF) + 1,
        (ulDefaultData >> 16) & 0xFF, ((ulDefaultData >> 16) & 0xFF) + 1,
        (ulSavedData >> 16) & 0xFF, ((ulSavedData >> 16) & 0xFF) + 1);
    printf("\tbit [ 15:  8] Low Priority Weight (LPW)\n"
        "\t\tCurrent = %d (= %d)\n"
        "\t\tDefault = %d (= %d)\n"
        "\t\tSaved   = %d (= %d)\n",
        (ulCurrentData >> 8) & 0xFF, ((ulCurrentData >> 8) & 0xFF) + 1,
        (ulDefaultData >> 8) & 0xFF, ((ulDefaultData >> 8) & 0xFF) + 1,
        (ulSavedData >> 8) & 0xFF, ((ulSavedData >> 8) & 0xFF) + 1);
    printf("\tbit [  2:  0] Arbitration Burst (AB), 7 (128) means no limit\n"
        "\t\tCurrent = %d (= %d)\n"
        "\t\tDefault = %d (= %d)\n"
        "\t\tSaved   = %d (= %d)\n",
        ulCurrentData & 0x7, 1 << (ulCurrentData & 0x7),
        ulDefaultData & 0x7, 1 << (ulDefaultData & 0x7),
        ulSavedData & 0x7, 1 << (ulSavedData & 0x7));

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);
        
    return result;
}

static int NVMeGetFeaturesPowerManagement(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Power Management:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_POWER_MANAGEMENT, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_POWER_MANAGEMENT, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_POWER_MANAGEMENT, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_POWER_MANAGEMENT, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [  7:  5] Workload Hint (WH):\n");
    switch ((ulCurrentData >> 5) & 0x7)
    {
    case 0:
        printf("\t\tCurrent = 0 (no workload)\n");
        break;

    case 1:
    case 2:
        printf("\t\tCurrent = %d (workload #%d)\n", (ulCurrentData >> 5) & 0x7, (ulCurrentData >> 5) & 0x7);
        break;

    default:
        printf("\t\tCurrent = %d (unknown)\n", (ulCurrentData >> 5) & 0x7);
        break;
    }

    switch ((ulDefaultData >> 5) & 0x7)
    {
    case 0:
        printf("\t\tDefault = 0 (no workload)\n");
        break;

    case 1:
    case 2:
        printf("\t\tDefault = %d (workload #%d)\n", (ulDefaultData >> 5) & 0x7, (ulDefaultData >> 5) & 0x7);
        break;

    default:
        printf("\t\tDefault = %d (unknown)\n", (ulDefaultData >> 5) & 0x7);
        break;
    }

    switch ((ulSavedData >> 5) & 0x7)
    {
    case 0:
        printf("\t\tSaved   = 0 (no workload)\n");
        break;

    case 1:
    case 2:
        printf("\t\tSaved   = %d (workload #%d)\n", (ulSavedData >> 5) & 0x7, (ulSavedData >> 5) & 0x7);
        break;

    default:
        printf("\t\tSaved   = %d (unknown)\n", (ulSavedData >> 5) & 0x7);
        break;
    }

    printf("\tbit [  4:  0] Power State (PS)\n"
        "\t\tCurrent = PS%d\n"
        "\t\tDefault = PS%d\n"
        "\t\tSaved   = PS%d\n",
        (ulCurrentData) & 0x1F,
        (ulDefaultData) & 0x1F,
        (ulSavedData) & 0x1F);

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesTemperatureThreshold(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };
    NVME_CDW11_FEATURE_TEMPERATURE_THRESHOLD cdw11 = { 0 };

    printf("\n[I] Temperature Threshold:\n");

    // Get configuration of Composite Temperature
    printf("\tComposite Temperature:\n");

    // 1. get current value (over)
    cdw11.THSEL = NVME_TEMPERATURE_OVER_THRESHOLD;
    cdw11.TMPSEL = 0; // composite temperature
    cdw11.TMPTH = 0; // not used
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_CURRENT, (DWORD)cdw11.AsUlong, &ulCurrentData);
    if (result) return result;

    // 2. get default value (over)
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_DEFAULT, (DWORD)cdw11.AsUlong, &ulDefaultData);
    if (result) return result;

    // 3. get saved value (over)
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_SAVED, (DWORD)cdw11.AsUlong, &ulSavedData);
    if (result) return result;

    printf("\tbit [ 15:  0] Over Temperature Threshold (TMPTH)\n"
        "\t\tCurrent = %d\n"
        "\t\tDefault = %d\n"
        "\t\tSaved   = %d\n",
        ulCurrentData & 0xFFFF,
        ulDefaultData & 0xFFFF,
        ulSavedData & 0xFFFF);

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, (DWORD)cdw11.AsUlong, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    // 5. get current value (under)
    cdw11.THSEL = NVME_TEMPERATURE_UNDER_THRESHOLD;
    cdw11.TMPSEL = 0; // composite temperature
    cdw11.TMPTH = 0; // not used
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_CURRENT, (DWORD)cdw11.AsUlong, &ulCurrentData);
    if (result) return result;

    // 6. get default value (over)
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_DEFAULT, (DWORD)cdw11.AsUlong, &ulDefaultData);
    if (result) return result;

    // 7. get saved value (over)
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_SAVED, (DWORD)cdw11.AsUlong, &ulSavedData);
    if (result) return result;

    printf("\tbit [ 15:  0] Under Temperature Threshold (TMPTH)\n"
        "\t\tCurrent = %d\n"
        "\t\tDefault = %d\n"
        "\t\tSaved   = %d\n",
        ulCurrentData & 0xFFFF,
        ulDefaultData & 0xFFFF,
        ulSavedData & 0xFFFF);

    // 8. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    for (int i = 0; i < 8; i++)
    {
        uint16_t* pu16Temperature = &(g_stSMARTLog.TemperatureSensor1) + i;
        if (*pu16Temperature != 0)
        {
            // Get configuration of Temperature Sensor [1..8]
            printf("\n\tTemperature Sensor %d:\n", i + 1);

            // 1. get current value (over)
            cdw11.THSEL = NVME_TEMPERATURE_OVER_THRESHOLD;
            cdw11.TMPSEL = i+1; // Temperature Sensor i+1
            cdw11.TMPTH = 0; // not used
            result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_CURRENT, (DWORD)cdw11.AsUlong, &ulCurrentData);
            if (result) return result;

            // 2. get default value (over)
            result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_DEFAULT, (DWORD)cdw11.AsUlong, &ulDefaultData);
            if (result) return result;

            // 3. get saved value (over)
            result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_SAVED, (DWORD)cdw11.AsUlong, &ulSavedData);
            if (result) return result;

            printf("\tbit [ 15:  0] Over Temperature Threshold (TMPTH): (65535 means default)\n"
                "\t\tCurrent = %d\n"
                "\t\tDefault = %d\n"
                "\t\tSaved   = %d\n",
                ulCurrentData & 0xFFFF,
                ulDefaultData & 0xFFFF,
                ulSavedData & 0xFFFF);

            // 4. get supported capabilities
            result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, (DWORD)cdw11.AsUlong, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
            if (result) return result;

            vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

            // 5. get current value (under)
            cdw11.THSEL = NVME_TEMPERATURE_UNDER_THRESHOLD;
            cdw11.TMPSEL = i+1; // Temperature Sensor i+1
            cdw11.TMPTH = 0; // not used
            result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_CURRENT, (DWORD)cdw11.AsUlong, &ulCurrentData);
            if (result) return result;

            // 6. get default value (over)
            result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_DEFAULT, (DWORD)cdw11.AsUlong, &ulDefaultData);
            if (result) return result;

            // 7. get saved value (over)
            result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_SAVED, (DWORD)cdw11.AsUlong, &ulSavedData);
            if (result) return result;

            printf("\tbit [ 15:  0] Under Temperature Threshold (TMPTH): (0 means default)\n"
                "\t\tCurrent = %d\n"
                "\t\tDefault = %d\n"
                "\t\tSaved   = %d\n",
                ulCurrentData & 0xFFFF,
                ulDefaultData & 0xFFFF,
                ulSavedData & 0xFFFF);

            // 8. get supported capabilities
            result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_TEMPERATURE_THRESHOLD, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, (DWORD)cdw11.AsUlong, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
            if (result) return result;

            vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);
        }
    }
    return result;
}

static int NVMeGetFeaturesErrorRecovery(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities =  { 0 };

    printf("\n[I] Error Recovery:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ERROR_RECOVERY, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ERROR_RECOVERY, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ERROR_RECOVERY, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ERROR_RECOVERY, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [     16] Deallocated or Unwritten Logical Block Error Enable (DULBE):\n");
    if ((ulCurrentData >> 16) & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 16) & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 16) & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    printf("\tbit [ 15:  0] Time Limited Error Recovery (TLER):\n");
    if ((ulCurrentData) & 0xFFFF)
    {
        printf("\t\tCurrent = %d (means %d ms)\n", (ulCurrentData) & 0xFFFF, ((ulCurrentData) & 0xFFFF) * 100);
    }
    else
    {
        printf("\t\tCurrent = 0 (no timeout)\n");
    }

    if ((ulDefaultData) & 0xFFFF)
    {
        printf("\t\tDefault = %d (means %d ms)\n", (ulDefaultData) & 0xFFFF, ((ulDefaultData) & 0xFFFF) * 100);
    }
    else
    {
        printf("\t\tDefault = 0 (no timeout)\n");
    }

    if ((ulSavedData) & 0xFFFF)
    {
        printf("\t\tSaved   = %d (means %d ms)\n", (ulSavedData) & 0xFFFF, ((ulSavedData) & 0xFFFF) * 100);
    }
    else
    {
        printf("\t\tSaved   = 0 (no timeout)\n");
    }

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesVolatileWriteCache(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Volatile Write Cache:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_VOLATILE_WRITE_CACHE, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_VOLATILE_WRITE_CACHE, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_VOLATILE_WRITE_CACHE, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_VOLATILE_WRITE_CACHE, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [      0] Volatile Write Cache Enable (WCE):\n");
    if (ulCurrentData & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if (ulDefaultData & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if (ulSavedData & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesNumberOfQueues(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Number of Queues:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NUMBER_OF_QUEUES, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NUMBER_OF_QUEUES, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NUMBER_OF_QUEUES, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NUMBER_OF_QUEUES, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [ 31: 16] Number of I/O Completion Queues Allocated (NCQA)\n"
        "\t\tCurrent = %d (= %d)\n"
        "\t\tDefault = %d (= %d)\n"
        "\t\tSaved   = %d (= %d)\n",
        (ulCurrentData >> 16) & 0xFFFF, ((ulCurrentData >> 16) & 0xFFFF) + 1,
        (ulDefaultData >> 16) & 0xFFFF, ((ulDefaultData >> 16) & 0xFFFF) + 1,
        (ulSavedData >> 16) & 0xFFFF, ((ulSavedData >> 16) & 0xFFFF) + 1);
    printf("\tbit [ 15:  0] Number of I/O Submission Queues Allocated (NCQA)\n"
        "\t\tCurrent = %d (= %d)\n"
        "\t\tDefault = %d (= %d)\n"
        "\t\tSaved   = %d (= %d)\n",
        ulCurrentData & 0xFFFF, (ulCurrentData & 0xFFFF) + 1,
        ulDefaultData & 0xFFFF, (ulDefaultData & 0xFFFF) + 1,
        ulSavedData & 0xFFFF, (ulSavedData & 0xFFFF) + 1);

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesInterruptCoalescing(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Interrupt Coalescing:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_INTERRUPT_COALESCING, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_INTERRUPT_COALESCING, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_INTERRUPT_COALESCING, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_INTERRUPT_COALESCING, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [ 15:  8] Aggregation Time (TIME):\n");
    if ((ulCurrentData >> 8) & 0xFF)
    {
        printf("\t\tCurrent = %d (means %d us)\n", (ulCurrentData >> 8) & 0xFF, ((ulCurrentData >> 8) & 0xFF) * 100);
    }
    else
    {
        printf("\t\tCurrent = 0 (no delay)\n");
    }

    if ((ulDefaultData >> 8) & 0xFF)
    {
        printf("\t\tDefault = %d (means %d us)\n", (ulDefaultData >> 8) & 0xFF, ((ulDefaultData >> 8) & 0xFF) * 100);
    }
    else
    {
        printf("\t\tDefault = 0 (no delay)\n");
    }

    if ((ulSavedData >> 8) & 0xFF)
    {
        printf("\t\tSaved   = %d (means %d us)\n", (ulSavedData >> 8) & 0xFF, ((ulSavedData >> 8) & 0xFF) * 100);
    }
    else
    {
        printf("\t\tSaved   = 0 (no delay)\n");
    }

    printf("\tbit [  7:  0] Aggregation Threshold (THR); number of queue entries\n"
        "\t\tCurrent = %d\n"
        "\t\tDefault = %d\n"
        "\t\tSaved   = %d\n",
        ulCurrentData & 0xFF,
        ulDefaultData & 0xFF,
        ulSavedData & 0xFF);

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesWriteAtomicityNormal(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Write Atomicity Normal:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_WRITE_ATOMICITY, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_WRITE_ATOMICITY, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_WRITE_ATOMICITY, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_WRITE_ATOMICITY, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [      0] Disable Normal (DN):\n");
    if (ulCurrentData & 0x1)
    {
        printf("\t\tCurrent = 1 (honor only AWUPF and NAWUPF)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (honor AWUN, NAWUN, AWUPF, and NAWUPF)\n");
    }

    if (ulDefaultData & 0x1)
    {
        printf("\t\tDefault = 1 (honor only AWUPF and NAWUPF)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (honor AWUN, NAWUN, AWUPF, and NAWUPF)\n");
    }

    if (ulSavedData & 0x1)
    {
        printf("\t\tSaved   = 1 (honor only AWUPF and NAWUPF)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (honor AWUN, NAWUN, AWUPF, and NAWUPF)\n");
    }

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesAsynchronousEventConfiguration(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Asynchronous Event Configuration:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ASYNC_EVENT_CONFIG, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ASYNC_EVENT_CONFIG, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ASYNC_EVENT_CONFIG, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_ASYNC_EVENT_CONFIG, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    if (0x00010400 <= ((g_stController.VER) & 0xFFFFFF00))
    { // revision 1.4 or later
        printf("\tbit [     14] Endurance Group Event Aggregate Log Change Notices:\n");
        if ((ulCurrentData >> 14) & 0x1)
        {
            printf("\t\tCurrent = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tCurrent = 0 (disabled)\n");
        }
        if ((ulDefaultData >> 14) & 0x1)
        {
            printf("\t\tDefault = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tDefault = 0 (disabled)\n");
        }

        if ((ulSavedData >> 14) & 0x1)
        {
            printf("\t\tSaved   = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tSaved   = 0 (disabled)\n");
        }

        printf("\tbit [     13] LBA Status Information Notices:\n");
        if ((ulCurrentData >> 13) & 0x1)
        {
            printf("\t\tCurrent = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tCurrent = 0 (disabled)\n");
        }
        if ((ulDefaultData >> 13) & 0x1)
        {
            printf("\t\tDefault = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tDefault = 0 (disabled)\n");
        }

        if ((ulSavedData >> 13) & 0x1)
        {
            printf("\t\tSaved   = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tSaved   = 0 (disabled)\n");
        }

        printf("\tbit [     12] Predictable Latency Event Aggregate Log Change Notices:\n");
        if ((ulCurrentData >> 12) & 0x1)
        {
            printf("\t\tCurrent = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tCurrent = 0 (disabled)\n");
        }
        if ((ulDefaultData >> 12) & 0x1)
        {
            printf("\t\tDefault = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tDefault = 0 (disabled)\n");
        }

        if ((ulSavedData >> 12) & 0x1)
        {
            printf("\t\tSaved   = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tSaved   = 0 (disabled)\n");
        }

        printf("\tbit [     11] Asymmetric Namespace Access Change Notices:\n");
        if ((ulCurrentData >> 11) & 0x1)
        {
            printf("\t\tCurrent = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tCurrent = 0 (disabled)\n");
        }
        if ((ulDefaultData >> 11) & 0x1)
        {
            printf("\t\tDefault = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tDefault = 0 (disabled)\n");
        }

        if ((ulSavedData >> 11) & 0x1)
        {
            printf("\t\tSaved   = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tSaved   = 0 (disabled)\n");
        }
    }

    printf("\tbit [     10] Telemetry Log Notices:\n");
    if ((ulCurrentData >> 10) & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 10) & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 10) & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    printf("\tbit [      9] Firmware Activation Notices:\n");
    if ((ulCurrentData >> 9) & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 9) & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 9) & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    printf("\tbit [      8] Namespace Attribute Notices:\n");
    if ((ulCurrentData >> 8) & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 8) & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 8) & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    if (0x00010400 <= ((g_stController.VER) & 0xFFFFFF00))
    { // revision 1.4 or later
        printf("\tbit [      5] SMART / Health Critical Warnings (Persistent Memory Region fail):\n");
        if ((ulCurrentData >> 5) & 0x1)
        {
            printf("\t\tCurrent = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tCurrent = 0 (disabled)\n");
        }
        if ((ulDefaultData >> 5) & 0x1)
        {
            printf("\t\tDefault = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tDefault = 0 (disabled)\n");
        }
        if ((ulSavedData >> 5) & 0x1)
        {
            printf("\t\tSaved   = 1 (enabled)\n");
        }
        else
        {
            printf("\t\tSaved   = 0 (disabled)\n");
        }
    }

    printf("\tbit [      4] SMART / Health Critical Warnings (Volatile memory backup device fail):\n");
    if ((ulCurrentData >> 4) & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 4) & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 4) & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    printf("\tbit [      3] SMART / Health Critical Warnings (Read only mode):\n");
    if ((ulCurrentData >> 3) & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 3) & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 3) & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    printf("\tbit [      2] SMART / Health Critical Warnings (Degraded reliability):\n");
    if ((ulCurrentData >> 2) & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 2) & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 2) & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    printf("\tbit [      1] SMART / Health Critical Warnings (Temperature threshold):\n");
    if ((ulCurrentData >> 1) & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 1) & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 1) & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    printf("\tbit [      0] SMART / Health Critical Warnings (Lower spare capacity):\n");
    if (ulCurrentData & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if (ulDefaultData & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if (ulSavedData & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesHCTM(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Host Controlled Thermal Management:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [ 31: 16] Thermal Management Temperature 1 (TMT1):\n");
    if ((ulCurrentData >> 16) & 0xFFFF)
    {
        printf("\t\tCurrent = %d (K)\n", (ulCurrentData >> 16) & 0xFFFF);
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if ((ulDefaultData >> 16) & 0xFFFF)
    {
        printf("\t\tDefault = %d (K)\n", (ulDefaultData >> 16) & 0xFFFF);
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if ((ulSavedData >> 16) & 0xFFFF)
    {
        printf("\t\tSaved   = %d (K)\n", (ulSavedData >> 16) & 0xFFFF);
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    printf("\tbit [ 15:  0] Thermal Management Temperature 2 (TMT2):\n");
    if (ulCurrentData & 0xFFFF)
    {
        printf("\t\tCurrent = %d (K)\n", ulCurrentData & 0xFFFF);
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if (ulDefaultData & 0xFFFF)
    {
        printf("\t\tDefault = %d (K)\n", ulDefaultData & 0xFFFF);
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if (ulSavedData & 0xFFFF)
    {
        printf("\t\tSaved   = %d (K)\n", ulSavedData & 0xFFFF);
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesSoftwareProgressMarker(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Software Progress Marker:\n");

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NVM_SOFTWARE_PROGRESS_MARKER, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NVM_SOFTWARE_PROGRESS_MARKER, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NVM_SOFTWARE_PROGRESS_MARKER, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NVM_SOFTWARE_PROGRESS_MARKER, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [  7:  0] Pre-boot Software Load Count (PBSLC)\n"
        "\t\tCurrent = %d\n"
        "\t\tDefault = %d\n"
        "\t\tSaved   = %d\n",
        ulCurrentData & 0xFF,
        ulDefaultData & 0xFF,
        ulSavedData & 0xFF);

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

static int NVMeGetFeaturesNOPPME(HANDLE _hDevice)
{
    int result = false;
    uint32_t ulCurrentData = 0;
    uint32_t ulDefaultData = 0;
    uint32_t ulSavedData = 0;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY ulSupportedCapabilities = { 0 };

    printf("\n[I] Non-Operational Power State Config:\n");

    if (g_stController.CTRATT.NoopPSPermissiveModeEn == 0)
    {
        fprintf(stderr, "\n[W] This controller does not support Non-Operational Power State Permissive Mode, skip\n");
        return result;
    }

    // 1. get current value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NONOPERATIONAL_POWER_STATE, NVME_FEATURE_VALUE_CURRENT, 0, &ulCurrentData);
    if (result) return result;

    // 2. get default value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NONOPERATIONAL_POWER_STATE, NVME_FEATURE_VALUE_DEFAULT, 0, &ulDefaultData);
    if (result) return result;

    // 3. get saved value
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NONOPERATIONAL_POWER_STATE, NVME_FEATURE_VALUE_SAVED, 0, &ulSavedData);
    if (result) return result;

    // 4. get supported capabilities
    result = iNVMeGetFeature32(_hDevice, NVME_FEATURE_NONOPERATIONAL_POWER_STATE, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&(ulSupportedCapabilities.AsUlong));
    if (result) return result;

    printf("\tbit [      0] Non-Operational Power State Permissive Mode Enable (NOPPME)\n");
    if (ulCurrentData & 0x1)
    {
        printf("\t\tCurrent = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tCurrent = 0 (disabled)\n");
    }

    if (ulDefaultData & 0x1)
    {
        printf("\t\tDefault = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tDefault = 0 (disabled)\n");
    }

    if (ulSavedData & 0x1)
    {
        printf("\t\tSaved   = 1 (enabled)\n");
    }
    else
    {
        printf("\t\tSaved   = 0 (disabled)\n");
    }

    vNVMeGetFeaturesShowCapabilities(ulSupportedCapabilities);

    return result;
}

int iNVMeGetFeatures(HANDLE _hDevice)
{
    int iResult = -1;
    int iFId = 0;

    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    sprintf_s(strPrompt,
        1024,
        "\n# Input Feature Identifier (in hex):"
        "\n#    Supported Features are:"
        "\n#     %02Xh = Arbitration"
        "\n#     %02Xh = Power Management"
        "\n#     %02Xh = LBA Range Type"
        "\n#     %02Xh = Temperature Threshold"
        "\n#     %02Xh = Error Recovery"
        "\n#     %02Xh = Volatile Write Cache"
        "\n#     %02Xh = Number of Queues"
        "\n#     %02Xh = Interrupt Coalescing"
        "\n#     %02Xh = Write Atomicity Normal"
        "\n#     %02Xh = Asynchronous Event Configuration"
        "\n#     %02Xh = Autonomous Power State Transition"
        "\n#     %02Xh = Host Memory Buffer"
        "\n#     %02Xh = Timestamp"
        "\n#     %02Xh = Host Controlled Thermal Management"
        "\n#     %02Xh = Non-Operational Power State Config"
        "\n#     %02Xh = Software Progress Marker"
        "\n",
        NVME_FEATURE_ARBITRATION,
        NVME_FEATURE_POWER_MANAGEMENT,
        NVME_FEATURE_LBA_RANGE_TYPE,
        NVME_FEATURE_TEMPERATURE_THRESHOLD,
        NVME_FEATURE_ERROR_RECOVERY,
        NVME_FEATURE_VOLATILE_WRITE_CACHE,
        NVME_FEATURE_NUMBER_OF_QUEUES,
        NVME_FEATURE_INTERRUPT_COALESCING,
        NVME_FEATURE_WRITE_ATOMICITY,
        NVME_FEATURE_ASYNC_EVENT_CONFIG,
        NVME_FEATURE_AUTONOMOUS_POWER_STATE_TRANSITION,
        NVME_FEATURE_HOST_MEMORY_BUFFER,
        NVME_FEATURE_TIMESTAMP,
        NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT,
        NVME_FEATURE_NONOPERATIONAL_POWER_STATE,
        NVME_FEATURE_NVM_SOFTWARE_PROGRESS_MARKER);

    iFId = iGetConsoleInputHex((const char*)strPrompt, strCmd);
    switch (iFId)
    {
    case NVME_FEATURE_ARBITRATION:
        cCmd = cGetConsoleInput("\n# Get Feature : Arbitration (Feature Identifier = 01h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesArbitration(_hDevice);
        }
        break;

    case NVME_FEATURE_POWER_MANAGEMENT:
        cCmd = cGetConsoleInput("\n# Get Feature : Power Management (Feature Identifier = 02h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesPowerManagement(_hDevice);
        }
        break;

    case NVME_FEATURE_LBA_RANGE_TYPE:
        cCmd = cGetConsoleInput("\n# Get Feature : LBA Range Type (Feature Identifier = 03h) for NSID = 1, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = iNVMeGetFeaturesLBARange(_hDevice);
        }
        break;

    case NVME_FEATURE_TEMPERATURE_THRESHOLD:
        cCmd = cGetConsoleInput("\n# Get Feature : Temperature Threshold (Feature Identifier = 04h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesTemperatureThreshold(_hDevice);
        }
        break;


    case NVME_FEATURE_ERROR_RECOVERY:
        cCmd = cGetConsoleInput("\n# Get Feature : Error Recovery (Feature Identifier = 05h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesErrorRecovery(_hDevice);
        }
        break;

    case NVME_FEATURE_VOLATILE_WRITE_CACHE:
        cCmd = cGetConsoleInput("\n# Get Feature : Volatile Write Cache (Feature Identifier = 06h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesVolatileWriteCache(_hDevice);
        }
        break;

    case NVME_FEATURE_NUMBER_OF_QUEUES:
        cCmd = cGetConsoleInput("\n# Get Feature : Number of Queues (Feature Identifier = 07h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesNumberOfQueues(_hDevice);
        }
        break;

    case NVME_FEATURE_INTERRUPT_COALESCING:
        cCmd = cGetConsoleInput("\n# Get Feature : Interrupt Coalescing (Feature Identifier = 08h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesInterruptCoalescing(_hDevice);
        }
        break;

    case NVME_FEATURE_WRITE_ATOMICITY:
        cCmd = cGetConsoleInput("\n# Get Feature : Write Atomicity Normal (Feature Identifier = 0Ah), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesWriteAtomicityNormal(_hDevice);
        }
        break;

    case NVME_FEATURE_ASYNC_EVENT_CONFIG:
        cCmd = cGetConsoleInput("\n# Get Feature : Asynchronous Event Configuration (Feature Identifier = 0Bh), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesAsynchronousEventConfiguration(_hDevice);
        }
        break;

    case NVME_FEATURE_AUTONOMOUS_POWER_STATE_TRANSITION:
        cCmd = cGetConsoleInput("\n# Get Feature : Autonomous Power State Transition (Feature Identifier = 0Ch), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            if (g_stController.APSTA.Supported == 0)
            {
                fprintf(stderr, "[W] This SSD controller does not support Autonomous Power State Transition (APST), ignore\n");
            }
            else
            {
                iResult = iNVMeGetFeaturesAPST(_hDevice);
            }
        }
        break;

    case NVME_FEATURE_HOST_MEMORY_BUFFER:
        cCmd = cGetConsoleInput("\n# Get Feature : Host Memory Buffer (Feature Identifier = 0Dh), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            if (g_stController.HMPRE == 0)
            {
                fprintf(stderr, "\n[W] This SSD controller does not support Host Memory Buffer (HMB), ignore\n");
            }
            else
            {
                iResult = iNVMeGetFeaturesHMB(_hDevice);
            }
        }
        break;

    case NVME_FEATURE_TIMESTAMP:
        cCmd = cGetConsoleInput("\n# Get Feature : Timestamp (Feature Identifier = 0Eh), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            if (g_stController.ONCS.Timestamp == 0)
            {
                fprintf(stderr, "\n[W] This SSD controller does not support Timestamp, ignore\n");
            }
            else
            {
                iResult = iNVMeGetFeaturesTimestamp(_hDevice);
            }
        }
        break;

    case NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT:
        cCmd = cGetConsoleInput("\n# Get Feature : Host Controlled Thermal Management (Feature Identifier = 10h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesHCTM(_hDevice);
        }
        break;

    case NVME_FEATURE_NONOPERATIONAL_POWER_STATE:
        cCmd = cGetConsoleInput("\n# Get Feature : Non-Operational Power State Config (Feature Identifier = 11h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesNOPPME(_hDevice);
        }
        break;

    case NVME_FEATURE_NVM_SOFTWARE_PROGRESS_MARKER:
        cCmd = cGetConsoleInput("\n# Get Feature : Software Progress Marker (Feature Identifier = 80h), Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            iResult = NVMeGetFeaturesSoftwareProgressMarker(_hDevice);
        }
        break;

    case NVME_FEATURE_KEEP_ALIVE:
    case NVME_FEATURE_NVM_HOST_IDENTIFIER:
    case NVME_FEATURE_NVM_RESERVATION_NOTIFICATION_MASK:
    case NVME_FEATURE_NVM_RESERVATION_PERSISTANCE:
    default:
        fprintf(stderr, "\n[E] Feature %02Xh is not implemented yet.\n", iFId);
        break;
    }

    printf("\n");
    return iResult;
}
