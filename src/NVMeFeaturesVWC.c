#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeUtils.h"

static int siNVMeSetFeaturesVWC(HANDLE _hDevice, DWORD _cdw10, DWORD _cdw11)
{
    int     iResult = -1;
    PVOID   buffer;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_SET query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_SET, AdditionalParameters) + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA_EXT);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError(GetLastError(), "malloc");
        return iResult;
    }

    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_SET)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT)(query->AdditionalParameters);

    query->PropertyId = StorageAdapterProtocolSpecificProperty; // StorageDeviceProtocolSpecificProperty;
    query->SetType                     = PropertyStandardSet;
    protocolData->ProtocolType         = ProtocolTypeNvme;
    protocolData->DataType             = NVMeDataTypeFeature;
    protocolData->ProtocolDataValue    = _cdw10;
    protocolData->ProtocolDataSubValue = _cdw11;
    protocolData->ProtocolDataOffset   = 0;
    protocolData->ProtocolDataLength   = 0;

    // Send request down.  
    iResult = iIssueDeviceIoControl(_hDevice,
        IOCTL_STORAGE_SET_PROPERTY,
        buffer,
        bufferLength,
        buffer,
        bufferLength,
        &returnedLength,
        NULL
    );

    if (iResult == 0)
    {
        // Validate the returned data.
        if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
            (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)))
        {
            fprintf(stderr, "[E] NVMeSetFeature: Data descriptor header not valid.\n");
            iResult = -1; // error
        }
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

int iNVMeSetFeaturesVWC(HANDLE _hDevice)
{
    int iResult = 0;
    int iValue = -1;
    char cCmd;
    char strCmd[256];
    char strPrompt[1024];

    NVME_CDW10_SET_FEATURES cdw10 = { 0 };
    NVME_CDW11_FEATURE_VOLATILE_WRITE_CACHE cdw11 = { 0 };

    sprintf_s(strPrompt,
        1024,
        "\n# Input value to be set:"
        "\n#    0: disable Volatile Write Cache"
        "\n#    1: enable  Volatile Write Cache"
        "\n#    2: disable Volatile Write Cache and save the setting"
        "\n#    3: enable  Volatile Write Cache and save the setting"
        "\n");

    iValue = iGetConsoleInputDec((const char*)strPrompt, strCmd);
    switch (iValue)
    {
    case 0:
        cCmd = cGetConsoleInput("\n# Set Feature - Volatile Write Cache: disable and not save, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            cdw11.WCE = 0;
            cdw10.FID = NVME_FEATURE_VOLATILE_WRITE_CACHE;
            cdw10.SV  = 0;
        }
        break;

    case 1:
        cCmd = cGetConsoleInput("\n# Set Feature - Volatile Write Cache: enable and not save, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            cdw11.WCE = 1;
            cdw10.FID = NVME_FEATURE_VOLATILE_WRITE_CACHE;
            cdw10.SV  = 0;
        }
        break;

    case 2:
        cCmd = cGetConsoleInput("\n# Set Feature - Volatile Write Cache: disable and save, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            cdw11.WCE = 0;
            cdw10.FID = NVME_FEATURE_VOLATILE_WRITE_CACHE;
            cdw10.SV  = 1;
        }
        break;

    case 3:
        cCmd = cGetConsoleInput("\n# Set Feature - Volatile Write Cache: enable and save, Press 'y' to continue\n", strCmd);
        if (cCmd == 'y')
        {
            cdw11.WCE = 1;
            cdw10.FID = NVME_FEATURE_VOLATILE_WRITE_CACHE;
            cdw10.SV  = 1;
        }
        break;

    default:
        fprintf(stderr, "\n[E] Value is not supported, process aborted.\n");
        iResult = -1;
        break;
    }

    if (iResult != -1)
    {
        iResult = siNVMeSetFeaturesVWC(_hDevice, cdw10.AsUlong, cdw11.AsUlong);
    }

    return iResult;
}

