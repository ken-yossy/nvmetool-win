#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeIdentifyController.h"
#include "NVMeSMART.h"

static void s_vPrintNVMeSMARTLog(PNVME_SMART_INFO_LOG _pData)
{
    printf("[I] Critical Warning:\n");
    if (0x00010400 <= ((g_stController.VER) & 0xFFFFFF00))
    { // revision 1.4 or later
        if (_pData->CriticalWarning.PMRDegraded)
        {
            printf("\tbit [      5] 1 = Persistent Memory Region has become read-only or unreliable.\n");
        }
        else
        {
            printf("\tbit [      5] 0 = Persistent Memory Region has not become read-only or unreliable.\n");
        }
    }

    if (_pData->CriticalWarning.VolatileMemoryBackupDeviceFailed)
    {
        printf("\tbit [      4] 1 = Volatile memory backup device has failed.\n");
    }
    else
    {
        printf("\tbit [      4] 0 = Volatile memory backup device has not failed.\n");
    }

    if (_pData->CriticalWarning.ReadOnly)
    {
        printf("\tbit [      3] 1 = The media has been placed in read only mode.\n");
    }
    else
    {
        printf("\tbit [      3] 0 = The media has not been placed in read only mode.\n");
    }

    if (_pData->CriticalWarning.ReliabilityDegraded)
    {
        printf("\tbit [      2] 1 = The NVM subsystem reliability has been degraded.\n");
    }
    else
    {
        printf("\tbit [      2] 0 = The NVM subsystem reliability has not been degraded.\n");
    }

    if (_pData->CriticalWarning.TemperatureThreshold)
    {
        printf("\tbit [      1] 1 = The temperature threshold has been encountered.\n");
    }
    else
    {
        printf("\tbit [      1] 0 = The temperature threshold has not been encountered.\n");
    }

    if (_pData->CriticalWarning.AvailableSpaceLow)
    {
        printf("\tbit [      0] 1 = The available spare capacity has fallen below the threshold.\n");
    }
    else
    {
        printf("\tbit [      0] 0 = The available spare capacity has not fallen below the threshold.\n");
    }

    int iTemp = 0;
    memcpy_s((void*)(&iTemp), sizeof(int), (const void*)(_pData->Temperature), sizeof(char) * 2);
    printf("[I] Composite Temperature: %d (K), %d (C)\n", iTemp, iTemp-273);

    printf("[I] Available Spare: %d (%%)\n", _pData->AvailableSpare);
    printf("[I] Available Spare Threshold: %d (%%)\n", _pData->AvailableSpareThreshold);
    printf("[I] Percentage Used: %d (%%)\n", _pData->PercentageUsed);

    if (0x00010400 <= ((g_stController.VER) & 0xFFFFFF00))
    { // revision 1.4 or later
        printf("[I] Endurance Group Critical Warning Summary:\n");
        if (_pData->EnduranceGroupSummary.ReadOnly)
        {
            printf("\tbit [      3] 1 = The namespaces in one or more Endurance Groups have been placed in read only mode.\n");
        }
        else
        {
            printf("\tbit [      3] 0 = No namespace in all Endurance Groups has been placed in read only mode.\n");
        }

        if (_pData->EnduranceGroupSummary.ReliabilityDegraded)
        {
            printf("\tbit [      3] 1 = The reliability of one or more Endurance Groups has been degraded.\n");
        }
        else
        {
            printf("\tbit [      3] 0 = The reliability of all Endurance Groups has not been degraded.\n");
        }

        if (_pData->EnduranceGroupSummary.AvailableSpareLow)
        {
            printf("\tbit [      3] 1 = The available spare capacity of one or more Endurance Groups has fallen below the threshold.\n");
        }
        else
        {
            printf("\tbit [      3] 0 = The available spare capacity of all Endurance Groups has not fallen below the threshold.\n");
        }
    }

    uint64_t ullLow = 0;
    uint64_t ullHigh = 0;

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->DataUnitRead), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->DataUnitRead[8])), sizeof(char) * 8);

    printf("[I] Data Units Read (in unit of 512 byte x 1000): 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->DataUnitWritten), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->DataUnitWritten[8])), sizeof(char) * 8);

    printf("[I] Data Units Written (in unit of 512 byte x 1000): 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->HostReadCommands), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->HostReadCommands[8])), sizeof(char) * 8);

    printf("[I] Host Read Commands: 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX (commands)\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->HostWrittenCommands), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->HostWrittenCommands[8])), sizeof(char) * 8);

    printf("[I] Host Write Commands: 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX (commands)\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->ControllerBusyTime), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->ControllerBusyTime[8])), sizeof(char) * 8);

    printf("[I] Controller Busy Time: 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX (minutes)\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->PowerCycle), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->PowerCycle[8])), sizeof(char) * 8);

    printf("[I] Power Cycles: 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX (times)\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->PowerOnHours), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->PowerOnHours[8])), sizeof(char) * 8);

    printf("[I] Power On Hours: 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX (hours)\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->UnsafeShutdowns), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->UnsafeShutdowns[8])), sizeof(char) * 8);

    printf("[I] Unsafe Shutdowns: 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX (times)\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->MediaErrors), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->MediaErrors[8])), sizeof(char) * 8);

    printf("[I] Media and Data Integrity Errors: 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX (times)\n", ullLow);

    memcpy_s((void*)(&ullLow), sizeof(uint64_t), (const void*)(_pData->ErrorInfoLogEntryNum), sizeof(char) * 8);
    memcpy_s((void*)(&ullHigh), sizeof(uint64_t), (const void*)(&(_pData->ErrorInfoLogEntryNum[8])), sizeof(char) * 8);

    printf("[I] Number of Error Information Log Entries: 0x");
    if (ullHigh != 0)
    {
        printf("%llX", ullHigh);
    }
    printf("%08llX\n", ullLow);

    printf("[I] Warning Composite Temperature Time: %lu (minutes) (= %lu hours)\n", _pData->WCTempTime, _pData->WCTempTime / 60);
    printf("[I] Critical Composite Temperature Time: %lu (minutes) (= %lu hours)\n", _pData->CCTempTime, _pData->CCTempTime / 60);

    if (_pData->TemperatureSensor1 != 0)
    {
        printf("[I] Temperature Sensor 1: %d (K), %d (C)\n", _pData->TemperatureSensor1, _pData->TemperatureSensor1 - 273);
    }
    else
    {
        printf("[I] Temperature Sensor 1: (not implemented)\n");
    }
    if (_pData->TemperatureSensor2 != 0)
    {
        printf("[I] Temperature Sensor 2: %d (K), %d (C)\n", _pData->TemperatureSensor2, _pData->TemperatureSensor2 - 273);
    }
    else
    {
        printf("[I] Temperature Sensor 2: (not implemented)\n");
    }
    if (_pData->TemperatureSensor3 != 0)
    {
        printf("[I] Temperature Sensor 3: %d (K), %d (C)\n", _pData->TemperatureSensor3, _pData->TemperatureSensor3 - 273);
    }
    else
    {
        printf("[I] Temperature Sensor 3: (not implemented)\n");
    }
    if (_pData->TemperatureSensor4 != 0)
    {
        printf("[I] Temperature Sensor 4: %d (K), %d (C)\n", _pData->TemperatureSensor4, _pData->TemperatureSensor4 - 273);
    }
    else
    {
        printf("[I] Temperature Sensor 4: (not implemented)\n");
    }
    if (_pData->TemperatureSensor5 != 0)
    {
        printf("[I] Temperature Sensor 5: %d (K), %d (C)\n", _pData->TemperatureSensor5, _pData->TemperatureSensor5 - 273);
    }
    else
    {
        printf("[I] Temperature Sensor 5: (not implemented)\n");
    }
    if (_pData->TemperatureSensor6 != 0)
    {
        printf("[I] Temperature Sensor 6: %d (K), %d (C)\n", _pData->TemperatureSensor6, _pData->TemperatureSensor6 - 273);
    }
    else
    {
        printf("[I] Temperature Sensor 6: (not implemented)\n");
    }
    if (_pData->TemperatureSensor7 != 0)
    {
        printf("[I] Temperature Sensor 7: %d (K), %d (C)\n", _pData->TemperatureSensor7, _pData->TemperatureSensor7 - 273);
    }
    else
    {
        printf("[I] Temperature Sensor 7: (not implemented)\n");
    }
    if (_pData->TemperatureSensor8 != 0)
    {
        printf("[I] Temperature Sensor 8: %d (K), %d (C)\n", _pData->TemperatureSensor8, _pData->TemperatureSensor8 - 273);
    }
    else
    {
        printf("[I] Temperature Sensor 8: (not implemented)\n");
    }

    printf("[I] Thermal Management Temperature 1 Transition Count: %d (times)\n", _pData->TMT1TransitionCount);
    printf("[I] Thermal Management Temperature 2 Transition Count: %d (times)\n", _pData->TMT2TransitionCount);
    printf("[I] Total Time For Thermal Management Temperature 1: %d (seconds)\n", _pData->TMT1TotalTime);
    printf("[I] Total Time For Thermal Management Temperature 2: %d (seconds)\n", _pData->TMT2TotalTime);
}

int iNVMeGetSMART(HANDLE _hDevice, bool _bPrint)
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
        + sizeof(NVME_SMART_INFO_LOG);
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
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_HEALTH_INFO;
    protocolData->ProtocolDataRequestSubValue = NVME_NAMESPACE_ALL;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_SMART_INFO_LOG);

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
        fprintf(stderr, "[E] NVMeGetSMARTLog: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_SMART_INFO_LOG))) {
        fprintf(stderr, "[E] NVMeGetSMARTLog: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    // Command Support and Effect Log Data
    if ( _bPrint ) s_vPrintNVMeSMARTLog((PNVME_SMART_INFO_LOG)((PCHAR)protocolData + protocolData->ProtocolDataOffset));

    memcpy_s((void*)(&g_stSMARTLog), sizeof(NVME_SMART_INFO_LOG), (uint8_t*)protocolData + protocolData->ProtocolDataOffset, sizeof(NVME_SMART_INFO_LOG));
    iResult = 0; // succeeded;

error_exit:
    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
