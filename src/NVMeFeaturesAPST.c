#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "NVMeGetFeatures.h"
#include "WinFunc.h"

static int siNVMeGetFeaturesAPST(
    HANDLE _hDevice, NVME_CDW10_GET_FEATURES _cdw10,
    PNVME_CDW11_FEATURE_AUTO_POWER_STATE_TRANSITION _result,
    PNVME_AUTO_POWER_STATE_TRANSITION_ENTRY _entry) {
    int iResult = -1;
    PVOID buffer = NULL;
    ULONG bufferLength = 0;
    ULONG returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;
    PNVME_AUTO_POWER_STATE_TRANSITION_ENTRY resultData = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters) +
                   sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) +
                   sizeof(NVME_AUTO_POWER_STATE_TRANSITION_ENTRY);
    buffer = malloc(bufferLength);

    if (buffer == NULL) {
        vPrintSystemError(GetLastError(), "malloc");
        return iResult;
    }

    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeFeature;
    protocolData->ProtocolDataRequestValue = _cdw10.AsUlong;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength =
        sizeof(NVME_AUTO_POWER_STATE_TRANSITION_ENTRY);

    // Send request down.
    iResult = iIssueDeviceIoControl(_hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                                    buffer, bufferLength, buffer, bufferLength,
                                    &returnedLength, NULL);

    if (iResult == 0) {
        // Validate the returned data.
        if ((protocolDataDescr->Version !=
             sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
            (protocolDataDescr->Size !=
             sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
            fprintf(
                stderr,
                "[E] NVMeGetFeaturesAPST: Data descriptor header not valid.\n");
            iResult = -1;  // error
        } else {
            _result->AsUlong = (ULONG)(protocolDataDescr->ProtocolSpecificData
                                           .FixedProtocolReturnData);
            resultData =
                (PNVME_AUTO_POWER_STATE_TRANSITION_ENTRY)((PCHAR)protocolData +
                                                          protocolData
                                                              ->ProtocolDataOffset);
            _entry->IdleTimePriorToTransition =
                resultData->IdleTimePriorToTransition;
            _entry->IdleTransitionPowerState =
                resultData->IdleTransitionPowerState;
        }
    }

    if (buffer != NULL) {
        free(buffer);
    }

    return iResult;
}

int iNVMeGetFeaturesAPST(HANDLE _hDevice) {
    int iResult = -1;
    NVME_CDW10_GET_FEATURES cdw10;
    NVME_CDW11_FEATURE_AUTO_POWER_STATE_TRANSITION CurrentSetting,
        DefaultSetting, SavedSetting;
    NVME_AUTO_POWER_STATE_TRANSITION_ENTRY CurrentEntry, DefaultEntry,
        SavedEntry;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY Cap;

    // 1. Get current value
    cdw10.FID = NVME_FEATURE_AUTONOMOUS_POWER_STATE_TRANSITION;
    cdw10.SEL = NVME_FEATURE_VALUE_CURRENT;
    iResult =
        siNVMeGetFeaturesAPST(_hDevice, cdw10, &CurrentSetting, &CurrentEntry);
    if (iResult != 0) return iResult;

    // 2. Get default value
    cdw10.SEL = NVME_FEATURE_VALUE_DEFAULT;
    iResult =
        siNVMeGetFeaturesAPST(_hDevice, cdw10, &DefaultSetting, &DefaultEntry);
    if (iResult != 0) return iResult;

    // 3. Get saved value
    cdw10.SEL = NVME_FEATURE_VALUE_SAVED;
    iResult =
        siNVMeGetFeaturesAPST(_hDevice, cdw10, &SavedSetting, &SavedEntry);
    if (iResult != 0) return iResult;

    // 4. get supported capabilities
    iResult = iNVMeGetFeature32(
        _hDevice, NVME_FEATURE_AUTONOMOUS_POWER_STATE_TRANSITION,
        NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&Cap);
    if (iResult != 0) return iResult;

    printf("\n[I] Autonomous Power State Transition:\n");

    printf("\tAutonomous Power State Transition Enable (APSTE):\n");
    if (CurrentSetting.APSTE) {
        printf("\t\tCurrent = 1 (enable)\n");
    } else {
        printf("\t\tCurrent = 0 (disable)\n");
    }

    if (DefaultSetting.APSTE) {
        printf("\t\tDefault = 1 (enable)\n");
    } else {
        printf("\t\tDefault = 0 (disable)\n");
    }

    if (SavedSetting.APSTE) {
        printf("\t\tSaved   = 1 (enable)\n");
    } else {
        printf("\t\tSaved   = 0 (disable)\n");
    }

    printf(
        "\tIdle Transition Power State (ITPS):\n"
        "\t\tCurrent = PS%d\n"
        "\t\tDefault = PS%d\n"
        "\t\tSaved   = PS%d\n",
        CurrentEntry.IdleTransitionPowerState,
        DefaultEntry.IdleTransitionPowerState,
        SavedEntry.IdleTransitionPowerState);

    printf(
        "\tIdle Time Prior to Transition (ITPT):\n"
        "\t\tCurrent = %d (ms)\n"
        "\t\tDefault = %d (ms)\n"
        "\t\tSaved   = %d (ms)\n",
        CurrentEntry.IdleTimePriorToTransition,
        DefaultEntry.IdleTimePriorToTransition,
        SavedEntry.IdleTimePriorToTransition);

    vNVMeGetFeaturesShowCapabilities(Cap);

    return iResult;
}
