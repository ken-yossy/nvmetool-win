#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include <nvme.h>
#include "WinFunc.h"

typedef struct {
    uint8_t LSUPP : 1;          // bit [    0]
    uint8_t IOS : 1;            // bit [    1]
    uint32_t Reserved : 14;     // bit [15: 2]
    uint32_t LIDSpecific : 16;  // bit [31:16]
} NVME_LID_SUPPORTED_AND_EFFECTS_DATA, *PNVME_LID_SUPPORTED_AND_EFFECTS_DATA;

#define NUM_LIDS 256

static void s_vPrintNVMeSupportedLogPages(
    PNVME_LID_SUPPORTED_AND_EFFECTS_DATA _pData) {
    printf("[I] Log page information:\n");

    for (int i = 0; i < NUM_LIDS; i++) {
        printf("[I]   Log ID %3d: ", i);
        if (_pData[i].LSUPP) {
            printf(" supported\n");
            printf("[I]     bit [    0] 1  = LID Supported\n");
            if (_pData[i].IOS) {
                printf("[I]     bit [    1] 1 = Index Offset is supported\n");
            } else {
                printf(
                    "[I]     bit [    1] 0 = Index Offset is not supported\n");
            }

            if (i == 14) {  // 0Dh
                if (_pData[i].LIDSpecific & 1) {
                    printf(
                        "[I]     bit [   16] 1 = Establish Context and Read "
                        "512 Bytes of Header action is supported\n");
                } else {
                    printf(
                        "[I]     bit [   16] 0 = Establish Context and Read "
                        "512 Bytes of Header action is not supported\n");
                }
            } else if (0xC0 <= i) {
                printf(
                    "[I]     bit [31:16] 0x%u = LID specific field (vendor "
                    "specific)\n",
                    _pData[i].LIDSpecific);
            }
        } else {
            printf(" not supported\n");
        }
    }
}

#ifndef NVME_LOG_PAGE_SUPPORTED_LOG_PAGES
#define NVME_LOG_PAGE_SUPPORTED_LOG_PAGES 0
#endif

int iNVMeGetSupportedLogPages(HANDLE _hDevice) {
    int iResult = -1;
    PVOID buffer = NULL;
    ULONG bufferLength = 0;
    ULONG returnedLength = 0;
    ULONG XferSize = sizeof(NVME_LID_SUPPORTED_AND_EFFECTS_DATA) * NUM_LIDS;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters) +
                   sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) +
                   XferSize;  // 4 bytes x 256 = 1024 bytes
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
    protocolData->ProtocolDataRequestValue =
        NVME_LOG_PAGE_SUPPORTED_LOG_PAGES;  // 0h
    protocolData->ProtocolDataRequestSubValue =
        0;  // lower 32-bit of the offset
    protocolData->ProtocolDataRequestSubValue2 =
        0;  // higher 32-bit of the offset
    // Subvalue3 and Subvalue4 are zero

    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = XferSize;

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
                "[E] NVMeGetSupportedLogPages: Data descriptor header not "
                "valid.\n");
        iResult = -1;  // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset <
         sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < XferSize)) {
        fprintf(stderr,
                "[E] NVMeGetSupportedLogPages: ProtocolData Offset/Length not "
                "valid.\n");
        iResult = -1;  // error
        goto error_exit;
    }

    s_vPrintNVMeSupportedLogPages(
        (PNVME_LID_SUPPORTED_AND_EFFECTS_DATA)((PCHAR)protocolData +
                                               protocolData
                                                   ->ProtocolDataOffset));
    iResult = 0;  // succeeded;

error_exit:
    if (buffer != NULL) {
        free(buffer);
    }

    return iResult;
}
