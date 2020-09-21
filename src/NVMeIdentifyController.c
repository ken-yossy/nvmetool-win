#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeUtils.h"
#include "NVMeIdentifyController.h"

void vPrintControllerBasicData(void)
{
    // These fields are common among revisions...
    {
        char buf[21];
        ZeroMemory(buf, 21);
        strncpy_s(buf, _countof(buf), (const char*)(g_stController.SN), 20);
        buf[20] = '\0';
        printASCII("[M] SerialNumber (SN): ", (const char*)buf, true);
    }

    {
        char buf[41];
        ZeroMemory(buf, 41);
        strncpy_s(buf, _countof(buf), (const char*)(g_stController.MN), 40);
        buf[40] = '\0';
        printASCII("[M] Model Number (MN): ", (const char*)buf, true);
    }

    {
        char buf[9];
        ZeroMemory(buf, 9);
        strncpy_s(buf, _countof(buf), (const char*)(g_stController.FR), 8);
        buf[8] = '\0';
        printASCII("[M] Firmware Revision (FR): ", (const char*)buf, true);
    }

    {
        char buf[16];
        ZeroMemory(buf, 16);
        snprintf(buf, _countof(buf), "0x%08x\0", (g_stController.VER));
        printASCII("[M] Version (VER): ", (const char*)buf, false);
        printf(" (NVMe Revision %d.%d.%d)\n", (g_stController.VER >> 16) & 0xFFFF, (g_stController.VER >> 8) & 0xFF, g_stController.VER & 0xFF);
    }
}

int iNVMeIdentifyController(HANDLE _hDevice)
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
        + sizeof(NVME_IDENTIFY_CONTROLLER_DATA);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError( GetLastError(), "malloc" );
        goto error_exit;
    }

    //
    // Initialize query data structure to get Identify Controller Data.
    //
    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeIdentify;
    protocolData->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_CONTROLLER;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_IDENTIFY_CONTROLLER_DATA);

    //
    // Send request down.
    //
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

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)))
    {
        fprintf(stderr, "[E] NVMeIdentifyController: Data Descriptor Header is not valid, stop.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset > sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_IDENTIFY_CONTROLLER_DATA)))
    {
        fprintf(stderr, "[E] NVMeIdentifyController: ProtocolData Offset/Length is not valid, stop.\n");
        iResult = -1; // error
        goto error_exit;
    }

    memcpy_s((void*)(&g_stController), sizeof(NVME_IDENTIFY_CONTROLLER_DATA), (uint8_t*)protocolData + protocolData->ProtocolDataOffset, sizeof(NVME_IDENTIFY_CONTROLLER_DATA));
    if ((g_stController.VID == 0) || (g_stController.NN == 0))
    {
        fprintf(stderr, "[E] NVMeIdentifyController: Identify Controller Data is not valid, stop.\n");
        iResult = -1; // error
        goto error_exit;
    }
    iResult = 0; // succeeded

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

