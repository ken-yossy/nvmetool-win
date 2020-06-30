#include "WinFunc.h"
#include <iostream>
#include <nvme.h>
#include "NVMeUtils.h"
#include "NVMeIdentifyController.h"
#include "NVMeErrorInformation.h"

static void s_vPrintNVMeErrorInformation(PNVME_ERROR_INFO_LOG13 _pData, int _iLogNo)
{
    printf("[I] Error Information (%d) :\n", _iLogNo);
    if (_pData->ErrorCount == 0)
    {
        printf("\tbyte[  7:  0] Error Count: 0 = invalid entry\n");
        return;
    }

    printf("\tbyte [ 39: 32] 0x%016llX = Command Specific Information.\n", _pData->CommandSpecificInfo);
    if (_pData->VendorInfoAvailable)
    {
        printf("\tbyte [     28] %xh = Log Page ID for additional vendor specific error information\n", _pData->VendorInfoAvailable);
    }
    else
    {
        printf("\tbyte [     28] 0h = No additional information is available.\n");
    }

    if (_pData->NameSpace)
    {
        printf("\tbyte [ 27: 24] %Xh = The namespace id that the error is associated with.\n", _pData->NameSpace);
    }
    else
    {
        printf("\tbyte [ 27: 24] 0h = No namespace information is available.\n");
    }

    printf("\tbyte [ 23: 16] %16llXh = The first LBA that experienced the error condition, if applicable.\n", _pData->Lba);
    printf("\tbyte [ 15: 14] Parameter Error Location:\n");
    if (_pData->ParameterErrorLocation.AsUshort == 0xFFFF)
    {
        printf("\t\tbit [ 15:  0] FFFFh = The error is not specific to a particular command.\n");
    }
    else
    {
        printf("\t\tbit [ 10:  8] %Xh = Bit in command that contained the error.\n", _pData->ParameterErrorLocation.Bit);
        printf("\t\tbit [  7:  0] %Xh = Byte in command that contained the error.\n", _pData->ParameterErrorLocation.Byte);
    }

    PNVME_COMMAND_STATUS pStat = &(_pData->Status);
    printf("\tbyte [ 13: 12] %04Xh = Status Field.\n", pStat->AsUshort);
    printf("\t\tbit [     15] %d = Do Not Retry (DNR)\n", pStat->DNR);
    printf("\t\tbit [     14] %d = More (M)\n", pStat->M);
    printf("\t\tbit [ 11:  9] %Xh = Status Code Type (SCT)\n", pStat->SCT);
    printf("\t\tbit [  8:  1] %Xh = Status Code (SC)\n", pStat->SC);
    printf("\t\tbit [      0] %d = Phase (P)\n", pStat->P);

    printf("\tbyte [ 11: 10] Command ID: ");
    if (_pData->CMDID == 0xFFFF)
    {
        printf("FFFFh = the error is not specific to a particular command\n");
    }
    else
    {
        printf("%04Xh\n", _pData->CMDID);
    }

    printf("\tbyte [  9:  8] Submission Queue ID: ");
    if (_pData->SQID == 0xFFFF)
    {
        printf("FFFFh = the error is not specific to a particular command\n");
    }
    else
    {
        printf("%04Xh\n", _pData->SQID);
    }

    printf("\tbyte[  7:  0] Error Count: %llx\n", _pData->ErrorCount);
}

int iNVMeGetErrorInformation(HANDLE _hDevice)
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
        + sizeof(NVME_ERROR_INFO_LOG13) * (g_stController.ELPE + 1);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vUtilPrintSystemError( GetLastError(), "malloc" );
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
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_ERROR_INFO;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_ERROR_INFO_LOG13) * (g_stController.ELPE + 1);

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

    fprintf(stderr, "\n");

    // Validate the returned data.
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        printf("[E] NVMeGetErrorInformation: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_ERROR_INFO_LOG13) * (g_stController.ELPE + 1))) {
        printf("[E] NVMeGetErrorInformation: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    {
        PNVME_ERROR_INFO_LOG13 aLog = (PNVME_ERROR_INFO_LOG13)((PCHAR)protocolData + protocolData->ProtocolDataOffset);
        for (int i = 0; i < (g_stController.ELPE + 1); i++)
        {
            s_vPrintNVMeErrorInformation(aLog, i);
            aLog++;
        }
        iResult = 0; // succeeded
    }

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
