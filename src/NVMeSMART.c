#include "NVMeSMART.h"

//#include <nvme.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#include "NVMeIdentifyController.h"
#include "WinFunc.h"

static void s_vPrintNVMeSMARTLog(PNVME_SMART_INFO_LOG _pData) {
    printf("[I] Critical Warning:\n");
    if (bIsNVMeV14OrLater()) {  // revision 1.4 or later
        if (_pData->CriticalWarning.PMRDegraded) {
            printf(
                "\tbit [      5] 1 = Persistent Memory Region has become "
                "read-only or unreliable.\n");
        } else {
            printf(
                "\tbit [      5] 0 = Persistent Memory Region has not become "
                "read-only or unreliable.\n");
        }
    }

    if (_pData->CriticalWarning.VolatileMemoryBackupDeviceFailed) {
        printf(
            "\tbit [      4] 1 = Volatile memory backup device has failed.\n");
    } else {
        printf(
            "\tbit [      4] 0 = Volatile memory backup device has not "
            "failed.\n");
    }

    if (_pData->CriticalWarning.ReadOnly) {
        printf(
            "\tbit [      3] 1 = The media has been placed in read only "
            "mode.\n");
    } else {
        printf(
            "\tbit [      3] 0 = The media has not been placed in read only "
            "mode.\n");
    }

    if (_pData->CriticalWarning.ReliabilityDegraded) {
        printf(
            "\tbit [      2] 1 = The NVM subsystem reliability has been "
            "degraded.\n");
    } else {
        printf(
            "\tbit [      2] 0 = The NVM subsystem reliability has not been "
            "degraded.\n");
    }

    if (_pData->CriticalWarning.TemperatureThreshold) {
        printf(
            "\tbit [      1] 1 = The temperature threshold has been "
            "encountered.\n");
    } else {
        printf(
            "\tbit [      1] 0 = The temperature threshold has not been "
            "encountered.\n");
    }

    if (_pData->CriticalWarning.AvailableSpaceLow) {
        printf(
            "\tbit [      0] 1 = The available spare capacity has fallen below "
            "the threshold.\n");
    } else {
        printf(
            "\tbit [      0] 0 = The available spare capacity has not fallen "
            "below the threshold.\n");
    }

    int iTemp = 0;
    memcpy_s((void*)(&iTemp), sizeof(int), (const void*)(_pData->Temperature),
             sizeof(char) * 2);
    printf("[I] Composite Temperature: %d (K), %d (C)\n", iTemp, iTemp - 273);

    printf("[I] Available Spare: %d (%%)\n", _pData->AvailableSpare);
    printf("[I] Available Spare Threshold: %d (%%)\n",
           _pData->AvailableSpareThreshold);
    printf("[I] Percentage Used: %d (%%)\n", _pData->PercentageUsed);

    if (bIsNVMeV14OrLater()) {  // revision 1.4 or later
        printf("[I] Endurance Group Critical Warning Summary:\n");
        if (_pData->EnduranceGroupSummary.ReadOnly) {
            printf(
                "\tbit [      3] 1 = The namespaces in one or more Endurance "
                "Groups have been placed in read only mode.\n");
        } else {
            printf(
                "\tbit [      3] 0 = No namespace in all Endurance Groups has "
                "been placed in read only mode.\n");
        }

        if (_pData->EnduranceGroupSummary.ReliabilityDegraded) {
            printf(
                "\tbit [      2] 1 = The reliability of one or more Endurance "
                "Groups has been degraded.\n");
        } else {
            printf(
                "\tbit [      2] 0 = The reliability of all Endurance Groups "
                "has not been degraded.\n");
        }

        if (_pData->EnduranceGroupSummary.AvailableSpareLow) {
            printf(
                "\tbit [      0] 1 = The available spare capacity of one or "
                "more Endurance Groups has fallen below the threshold.\n");
        } else {
            printf(
                "\tbit [      0] 0 = The available spare capacity of all "
                "Endurance Groups has not fallen below the threshold.\n");
        }
    }

    printf("[I] Data Units Read: 0x%016llX%016llX (x512 byte x1000)\n",
           _pData->DataUnitRead_H, _pData->DataUnitRead_L);
    printf("[I] Data Units Written: 0x%016llX%016llX (x512 byte x1000)\n",
           _pData->DataUnitWritten_H, _pData->DataUnitWritten_L);
    printf("[I] Host Read Commands: 0x%016llX%016llX\n",
           _pData->HostReadCommands_H, _pData->HostReadCommands_L);
    printf("[I] Host Write Commands: 0x%016llX%016llX\n",
           _pData->HostWrittenCommands_H, _pData->HostWrittenCommands_L);
    printf("[I] Controller Busy Time: 0x%016llX%016llX (minutes)\n",
           _pData->ControllerBusyTime_H, _pData->ControllerBusyTime_L);
    printf("[I] Power Cycles: 0x%016llX%016llX (times)\n", _pData->PowerCycle_H,
           _pData->PowerCycle_L);
    printf("[I] Power On Hours: 0x%016llX%016llX (hours)\n",
           _pData->PowerOnHours_H, _pData->PowerOnHours_L);
    printf("[I] Unsafe Shutdowns: 0x%016llX%016llX (times)\n",
           _pData->UnsafeShutdowns_H, _pData->UnsafeShutdowns_L);
    printf("[I] Media and Data Integrity Errors: 0x%016llX%016llX (counts)\n",
           _pData->MediaErrors_H, _pData->MediaErrors_L);
    printf("[I] Number of Error Information Log Entries: 0x%016llX%016llX\n",
           _pData->ErrorInfoLogEntryNum_H, _pData->ErrorInfoLogEntryNum_L);

    printf(
        "[I] Warning Composite Temperature Time: %lu (minutes) (= %lu hours)\n",
        _pData->WCTempTime, _pData->WCTempTime / 60);
    printf(
        "[I] Critical Composite Temperature Time: %lu (minutes) (= %lu "
        "hours)\n",
        _pData->CCTempTime, _pData->CCTempTime / 60);

    if (_pData->TemperatureSensor1 != 0) {
        printf("[I] Temperature Sensor 1: %d (K), %d (C)\n",
               _pData->TemperatureSensor1, _pData->TemperatureSensor1 - 273);
    } else {
        printf("[I] Temperature Sensor 1: (not implemented)\n");
    }
    if (_pData->TemperatureSensor2 != 0) {
        printf("[I] Temperature Sensor 2: %d (K), %d (C)\n",
               _pData->TemperatureSensor2, _pData->TemperatureSensor2 - 273);
    } else {
        printf("[I] Temperature Sensor 2: (not implemented)\n");
    }
    if (_pData->TemperatureSensor3 != 0) {
        printf("[I] Temperature Sensor 3: %d (K), %d (C)\n",
               _pData->TemperatureSensor3, _pData->TemperatureSensor3 - 273);
    } else {
        printf("[I] Temperature Sensor 3: (not implemented)\n");
    }
    if (_pData->TemperatureSensor4 != 0) {
        printf("[I] Temperature Sensor 4: %d (K), %d (C)\n",
               _pData->TemperatureSensor4, _pData->TemperatureSensor4 - 273);
    } else {
        printf("[I] Temperature Sensor 4: (not implemented)\n");
    }
    if (_pData->TemperatureSensor5 != 0) {
        printf("[I] Temperature Sensor 5: %d (K), %d (C)\n",
               _pData->TemperatureSensor5, _pData->TemperatureSensor5 - 273);
    } else {
        printf("[I] Temperature Sensor 5: (not implemented)\n");
    }
    if (_pData->TemperatureSensor6 != 0) {
        printf("[I] Temperature Sensor 6: %d (K), %d (C)\n",
               _pData->TemperatureSensor6, _pData->TemperatureSensor6 - 273);
    } else {
        printf("[I] Temperature Sensor 6: (not implemented)\n");
    }
    if (_pData->TemperatureSensor7 != 0) {
        printf("[I] Temperature Sensor 7: %d (K), %d (C)\n",
               _pData->TemperatureSensor7, _pData->TemperatureSensor7 - 273);
    } else {
        printf("[I] Temperature Sensor 7: (not implemented)\n");
    }
    if (_pData->TemperatureSensor8 != 0) {
        printf("[I] Temperature Sensor 8: %d (K), %d (C)\n",
               _pData->TemperatureSensor8, _pData->TemperatureSensor8 - 273);
    } else {
        printf("[I] Temperature Sensor 8: (not implemented)\n");
    }

    printf(
        "[I] Thermal Management Temperature 1 Transition Count: %d (times)\n",
        _pData->TMT1TransitionCount);
    printf(
        "[I] Thermal Management Temperature 2 Transition Count: %d (times)\n",
        _pData->TMT2TransitionCount);
    printf(
        "[I] Total Time For Thermal Management Temperature 1: %d (seconds)\n",
        _pData->TMT1TotalTime);
    printf(
        "[I] Total Time For Thermal Management Temperature 2: %d (seconds)\n",
        _pData->TMT2TotalTime);
}

int iNVMeGetSMART(HANDLE _hDevice, bool _bPrint, int iNSID) {
    int iResult = -1;
    PVOID buffer = NULL;
    ULONG bufferLength = 0;
    ULONG returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters) +
                   sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) +
                   sizeof(NVME_SMART_INFO_LOG);
    buffer = malloc(bufferLength);

    if (buffer == NULL) {
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
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_HEALTH_INFO;
    protocolData->ProtocolDataRequestSubValue =
        0;  // lower 32-bit of the offset
    protocolData->ProtocolDataRequestSubValue2 =
        0;  // higher 32-bit of the offset
    // Subvalue3 and Subvalue4 are zero

    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_SMART_INFO_LOG);

    // Send request down.
    iResult = iIssueDeviceIoControl(_hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                                    buffer, bufferLength, buffer, bufferLength,
                                    &returnedLength, NULL);

    if (iResult) goto error_exit;

    printf("\n");

    // Validate the returned data.
    if ((protocolDataDescr->Version !=
         sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        fprintf(stderr,
                "[E] NVMeGetSMARTLog: Data descriptor header not valid.\n");
        iResult = -1;  // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset <
         sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_SMART_INFO_LOG))) {
        fprintf(stderr,
                "[E] NVMeGetSMARTLog: ProtocolData Offset/Length not valid.\n");
        iResult = -1;  // error
        goto error_exit;
    }

    // Command Support and Effect Log Data
    if (_bPrint)
        s_vPrintNVMeSMARTLog(
            (PNVME_SMART_INFO_LOG)((PCHAR)protocolData +
                                   protocolData->ProtocolDataOffset));

    memcpy_s((void*)(&g_stSMARTLog), sizeof(NVME_SMART_INFO_LOG),
             (uint8_t*)protocolData + protocolData->ProtocolDataOffset,
             sizeof(NVME_SMART_INFO_LOG));
    iResult = 0;  // succeeded;

error_exit:
    if (buffer != NULL) {
        free(buffer);
    }

    return iResult;
}
