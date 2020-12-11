#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeIdentifyController.h"

//
// Information of log: NVME_LOG_PAGE_ERROR_INFO. Size: 64 bytes
//
typedef struct
{
    uint64_t            ErrorCount;         // byte [ 7: 0] Error Count
    uint16_t            SQID;               // byte [ 9: 8] Submission Queue ID
    uint16_t            CMDID;              // byte [11:10] Command ID
    NVME_COMMAND_STATUS Status;             // byte [13:12] Status Field

    union {
        struct {
            uint16_t    Byte : 8;       // bit [ 7: 0] Byte in command that contained the error.
            uint16_t    Bit : 3;        // bit [10: 8] Bit in command that contained the error.
            uint16_t    Reserved : 5;   // bit [15:11]
        } DUMMYSTRUCTNAME;

        uint16_t        AsUshort;
    } ParameterErrorLocation;               // byte [15:14] Parameter Error Location

    uint64_t            Lba;                // byte [23:16] LBA
    uint32_t            NameSpace;          // byte [27:24] Namespace
    uint8_t             VendorInfoAvailable;// byte [   28] Vendor Specific Information Available
    uint8_t             TRTYPE;             // byte [   29] Transport Type <v1.4>

    uint8_t             Reserved0[2];       // byte [31:30]
    uint64_t            CommandSpecificInfo;// byte [39:32] Command Specific Information
    uint16_t            TRTypeSpecificInfo; // byte [41:40] Transport Type Specific Information <v1.4>
    uint8_t             Reserved1[22];      // byte [63:42]
} NVME_ERROR_INFO_LOG14, *PNVME_ERROR_INFO_LOG14;

static void s_vPrintNVMeErrorInformation(PNVME_ERROR_INFO_LOG14 _pData, int _iLogNo)
{
    printf("[I] Error Information (%d) :\n", _iLogNo);
    if (_pData->ErrorCount == 0)
    {
        printf("\tbyte[  7:  0] Error Count: 0 = invalid entry\n");
        return;
    }

    if (0x00010400 <= ((g_stController.VER) & 0xFFFFFF00))
    {
        printf("\tbyte [ 41: 40] 0x%04X = Transport Type Specific Information\n ", _pData->TRTypeSpecificInfo);
    }

    printf("\tbyte [ 39: 32] 0x%016llX = Command Specific Information\n", _pData->CommandSpecificInfo);

    if (0x00010400 <= ((g_stController.VER) & 0xFFFFFF00))
    {
        printf("\tbyte [     29] Transport Type (TRTYPE): ");
        if (_pData->TRTYPE == 0)
        {
            printf("00h = The transport type is not indicated or the error is not transport related\n");
        }
        else if (_pData->TRTYPE == 1)
        {
            printf("01h = RDMA Transport (refer to the NVMe over Fabric specification)\n");
        }
        else if (_pData->TRTYPE == 2)
        {
            printf("02h = Fibre Channel Transport (refer to INCITS 540)\n");
        }
        else if (_pData->TRTYPE == 3)
        {
            printf("03h = TCP Transport (refer to the NVMe over Fabrics specification)\n");
        }
        else if (_pData->TRTYPE == 0xFE)
        {
            printf("FEh = Intra-host Transport (i.e., loopback)\n");
        }
        else
        {
            printf("%02Xh = (reserved value)\n", _pData->TRTYPE);
        }
    }

    if (_pData->VendorInfoAvailable)
    {
        printf("\tbyte [     28] %02Xh = Log Page ID for additional vendor specific error information\n", _pData->VendorInfoAvailable);
    }
    else
    {
        printf("\tbyte [     28] 00h = No additional information is available\n");
    }

    if (_pData->NameSpace)
    {
        printf("\tbyte [ 27: 24] %Xh = The namespace id that the error is associated with\n", _pData->NameSpace);
    }
    else
    {
        printf("\tbyte [ 27: 24] 0h = No namespace information is available\n");
    }

    printf("\tbyte [ 23: 16] %llXh = The first LBA that experienced the error condition, if applicable\n", _pData->Lba);
    printf("\tbyte [ 15: 14] Parameter Error Location:\n");
    if (_pData->ParameterErrorLocation.AsUshort == 0xFFFF)
    {
        printf("\t\tbit [ 15:  0] FFFFh = The error is not specific to a particular command\n");
    }
    else
    {
        printf("\t\tbit [ 10:  8] %Xh = Bit in command that contained the error\n", _pData->ParameterErrorLocation.Bit);
        printf("\t\tbit [  7:  0] %Xh = Byte in command that contained the error\n", _pData->ParameterErrorLocation.Byte);
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

    printf("\tbyte [  7:  0] Error Count: %llX\n", _pData->ErrorCount);
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
        + sizeof(NVME_ERROR_INFO_LOG14) * (g_stController.ELPE + 1);
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
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_ERROR_INFO;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_ERROR_INFO_LOG14) * (g_stController.ELPE + 1);

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
        fprintf(stderr, "[E] NVMeGetErrorInformation: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_ERROR_INFO_LOG14) * (g_stController.ELPE + 1))) {
        fprintf(stderr, "[E] NVMeGetErrorInformation: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    {
        PNVME_ERROR_INFO_LOG14 aLog = (PNVME_ERROR_INFO_LOG14)((PCHAR)protocolData + protocolData->ProtocolDataOffset);
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
