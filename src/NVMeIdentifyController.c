#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeUtils.h"
#include "NVMeIdentifyController.h"

bool g_WA_bGetControllerSMARTLogWithNSIDZero;

bool bIsNVMeV20OrLater(void)
{
    if (0x00020000 <= ((g_stController.VER) & 0xFFFFFF00))
        return true;

    return false;
}

bool bIsNVMeV14OrLater(void)
{
    if (0x00010400 <= ((g_stController.VER) & 0xFFFFFF00))
        return true;

    return false;
}

bool bIsNVMeV13OrLater(void)
{
    if (0x00010300 <= ((g_stController.VER) & 0xFFFFFF00))
        return true;

    return false;
}

void vPrintControllerBasicData(void)
{
    char strSN[21];
    char strMN[41];
    char strFR[9];
    char strVER[16];

    ZeroMemory(strSN, 21);
    strncpy_s(strSN, _countof(strSN), (const char*)(g_stController.SN), 20);
    strSN[20] = '\0';
    printASCII("[M] SerialNumber (SN): ", (const char*)strSN, true);

    ZeroMemory(strMN, 41);
    strncpy_s(strMN, _countof(strMN), (const char*)(g_stController.MN), 40);
    strMN[40] = '\0';
    printASCII("[M] Model Number (MN): ", (const char*)strMN, true);

    ZeroMemory(strFR, 9);
    strncpy_s(strFR, _countof(strFR), (const char*)(g_stController.FR), 8);
    strFR[8] = '\0';
    printASCII("[M] Firmware Revision (FR): ", (const char*)strFR, true);

    ZeroMemory(strVER, 16);
    snprintf(strVER, _countof(strVER), "0x%08x\0", (g_stController.VER));
    printASCII("[M] Version (VER): ", (const char*)strVER, false);
    printf(" (NVMe Revision %d.%d.%d)\n", (g_stController.VER >> 16) & 0xFFFF, (g_stController.VER >> 8) & 0xFF, g_stController.VER & 0xFF);

    if ((strstr((const char*)strMN, "WDC WDS") == NULL) ||
        (strstr((const char*)strMN, "2B0C") == NULL) ||
        (strstr((const char*)strFR, "211070WD") == NULL))
    {
        g_WA_bGetControllerSMARTLogWithNSIDZero = false;
    }
    else
    {
        /**
        * It is unable to get controller's SMART / Health information log page by Get Log Page command
        * with NSID = FFFFFFFFh. So, NSID = 0 is used for a workaround.
        */
        g_WA_bGetControllerSMARTLogWithNSIDZero = true;
        fprintf(stderr, "[W] Workanound is enabled: WD SN550 : Cannot retrieve controller's SMART/Health information by Get Log Page command with NSID FFFFFFFFh\n");
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

