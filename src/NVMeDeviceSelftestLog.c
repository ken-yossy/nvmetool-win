#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeIdentifyController.h"

static void s_vPrintNVMeDeviceSelftestLog(PNVME_DEVICE_SELF_TEST_LOG _pData)
{
    printf("[I] Device Self-test Information :\n");

    printf("\n");
    printf("[I] Current Device Self-test Operation :\n");
    switch (_pData->CurrentOperation.Status)
    {
    case 0x0:
        printf("\tbyte [    0] 0x00 = No device self-test operation in progress\n");
        break;

    case 0x1:
        printf("\tbyte [    0] 0x01 = Short device self-test operation in progress\n");
        break;

    case 0x2:
        printf("\tbyte [    0] 0x02 = Extended device self-test operation in progress\n");
        break;

    case 0xE:
        printf("\tbyte [    0] 0x0E = Vendor specific\n");
        break;

    default:
        printf("\tbyte [    0] 0x%02X = Reserved\n", _pData->CurrentOperation.Status);
        break;
    }

    printf("\n");
    printf("[I] Current Device Self-test Completion :\n");
    if (_pData->CurrentOperation.Status == 0)
    {
        printf("\tbyte [    1] ---- = No test is in progress\n");
    }
    else
    {
        printf("\tbyte [    1] 0x%02X = %d%% completed\n", _pData->CurrentCompletion.CompletePercent, _pData->CurrentCompletion.CompletePercent);
    }

    printf("\n");
    for (int i = 0; i < 20; i++)
    {
        printf("[I] Self-test Result Data #%d:\n", i);
        printf("\tbyte [    0] Device Self-test Status\n");
        switch (_pData->ResultData[i].Status.Result)
        {
        case 0x0:
            printf("\t\tbit [ 3: 0] 0x0 = Completed without error\n");
            break;

        case 0x1:
            printf("\t\tbit [ 3: 0] 0x1 = Aborted by a Device Self-test command\n");
            break;

        case 0x2:
            printf("\t\tbit [ 3: 0] 0x2 = Aborted by a Controller Level Reset\n");
            break;

        case 0x3:
            printf("\t\tbit [ 3: 0] 0x3 = Aborted due to a removal of a namespace from the namespace inventory\n");
            break;

        case 0x4:
            printf("\t\tbit [ 3: 0] 0x4 = Aborted due to the processing of a Format NVM command\n");
            break;

        case 0x5:
            printf("\t\tbit [ 3: 0] 0x5 = Did not complete due to fatal error or unknown test error\n");
            break;

        case 0x6:
            printf("\t\tbit [ 3: 0] 0x6 = Completed with a failed segment (failed segment is unknown)\n");
            break;

        case 0x7:
            printf("\t\tbit [ 3: 0] 0x7 = Completed with one or more failed segments (first failed segment is indicated in the Segment Number field)\n");
            break;

        case 0x8:
            printf("\t\tbit [ 3: 0] 0x8 = Aborted for unknown reason\n");
            break;

        case 0xF:
            printf("\t\tbit [ 3: 0] 0xF = Entry not used (does not contain a test result)\n");
            continue; // skip further analysis
            break;

        default:
            printf("\t\tbit [ 3: 0] 0x%X = Reserved\n", _pData->ResultData[i].Status.CodeValue);
            break;
        }

        switch (_pData->ResultData[i].Status.CodeValue)
        {
        case 0x1:
            printf("\t\tbit [ 7: 4] 0x1 = Short device self-test operation\n");
            break;

        case 0x2:
            printf("\t\tbit [ 7: 4] 0x2 = Extended device self-test operation\n");
            break;

        case 0xE:
            printf("\t\tbit [ 7: 4] 0xE = Vendor specific\n");
            break;

        default:
            printf("\t\tbit [ 7: 4] 0x%X = Reserved\n", _pData->ResultData[i].Status.Result);
            break;
        }

        printf("\tbyte [    1] 0x%02X = Segment Number\n", _pData->ResultData[i].SegmentNumber);

        printf("\tbyte [    2] Valid Diagnostic Information\n");
        if (_pData->ResultData[i].ValidDiagnostics.NSIDValid)
        {
            printf("\t\tbit [    0] 1 = Namespace Identifier field is valid\n");
        }
        else
        {
            printf("\t\tbit [    0] 0 = Namespace Identifier field is invalid\n");
        }
        if (_pData->ResultData[i].ValidDiagnostics.FLBAValid)
        {
            printf("\t\tbit [    1] 1 = Failing LBA field is valid\n");
        }
        else
        {
            printf("\t\tbit [    1] 0 = Failing LBA field is invalid\n");
        }
        if (_pData->ResultData[i].ValidDiagnostics.SCTValid)
        {
            printf("\t\tbit [    2] 1 = Status Code Type field is valid\n");
        }
        else
        {
            printf("\t\tbit [    2] 0 = Status Code Type field is invalid\n");
        }
        if (_pData->ResultData[i].ValidDiagnostics.SCValid)
        {
            printf("\t\tbit [    3] 1 = Status Code field is valid\n");
        }
        else
        {
            printf("\t\tbit [    3] 0 = Status Code field is invalid\n");
        }

        printf("\tbyte [11: 4] 0x%016llX = Power On Hours\n", _pData->ResultData[i].POH);

        if (_pData->ResultData[i].ValidDiagnostics.NSIDValid)
        {
            printf("\tbyte [15:12] 0x%08X = Namespace Identifier\n", _pData->ResultData[i].NSID);
        }
        if (_pData->ResultData[i].ValidDiagnostics.FLBAValid)
        {
            printf("\tbyte [23:16] 0x%016llX = Failing LBA\n", _pData->ResultData[i].FailingLBA);
        }
        if (_pData->ResultData[i].ValidDiagnostics.SCTValid)
        {
            printf("\tbyte [   24] 0x%02X = Status Code Type\n", _pData->ResultData[i].StatusCodeType.AdditionalInfo);
        }
        if (_pData->ResultData[i].ValidDiagnostics.SCValid)
        {
            printf("\tbyte [   25] 0x%02X = Status Code\n", _pData->ResultData[i].StatusCode);
        }

        printf("\tbyte [27:26] 0x%04X = Vendor Specific\n", _pData->ResultData[i].VendorSpecific);
    }
}

int iNVMeGetDeviceSelftestLog(HANDLE _hDevice, bool _bPrint, bool *_bInProgress)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    (*_bInProgress) = 0;

    if (!g_stController.OACS.DeviceSelfTest)
    {
        fprintf(stderr, "[W] This controller does not support Device Self-test command, ignore\n");
        return iResult;
    }

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)
        + sizeof(NVME_DEVICE_SELF_TEST_LOG);
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
    protocolData->ProtocolDataRequestValue = NVME_LOG_PAGE_DEVICE_SELF_TEST;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_DEVICE_SELF_TEST_LOG);

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
        fprintf(stderr, "[E] NVMeGetDeviceSelftestLog: Data descriptor header not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_DEVICE_SELF_TEST_RESULT_DATA))) {
        fprintf(stderr, "[E] NVMeGetDeviceSelftestLog: ProtocolData Offset/Length not valid.\n");
        iResult = -1; // error
        goto error_exit;
    }

    {
        PNVME_DEVICE_SELF_TEST_LOG aLog = (PNVME_DEVICE_SELF_TEST_LOG)((PCHAR)protocolData + protocolData->ProtocolDataOffset);
        if ( _bPrint )
        {
            s_vPrintNVMeDeviceSelftestLog(aLog);
        }
        if (aLog->CurrentOperation.Status == 0)
        {
            (*_bInProgress) = false;
        }
        else
        {
            (*_bInProgress) = true;
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
