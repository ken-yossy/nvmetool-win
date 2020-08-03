#include "WinFunc.h"
#include <iostream>
#include <nvme.h>

#include "NVMeUtils.h"
#include "NVMeGetFeatures.h"

static void s_vNVMePrintLBARange(PNVME_LBA_RANGET_TYPE_ENTRY _pstData, int _iNumValid)
{
    for (int i = 0; i < _iNumValid; i++)
    {
        printf("\t\tbyte [      0] (Type) %02Xh = ", _pstData[i].Type );
        switch(_pstData[i].Type)
        {
        case 0:
            printf("General Purpose\n");
            break;

        case NVME_LBA_RANGE_TYPE_FILESYSTEM:
            printf("Filesystem\n");
            break;

        case NVME_LBA_RANGE_TYPE_RAID:
            printf("RAID\n");
            break;

        case NVME_LBA_RANGE_TYPE_CACHE:
            printf("Cache\n");
            break;

        case NVME_LBA_RANGE_TYPE_PAGE_SWAP_FILE:
            printf("Page / swap file\n");
            break;

        default:
            if (_pstData[i].Type < 0x80)
            {
                printf("Reserved\n");
            }
            else
            {
                printf("Vendor Specific\n");
            }
        }

        if (_pstData[i].Attributes.MayOverwritten)
        {
            printf("\t\tbyte [      1] bit [      0] 1 = This LBA range may be overwritten\n");
        }
        else
        {
            printf("\t\tbyte [      1] bit [      0] 0 = This LBA range should not be overwritten\n");
        }

        if (_pstData[i].Attributes.Hidden)
        {
            printf("\t\tbyte [      1] bit [      1] 1 = This LBA range should be hidden\n");
        }
        else
        {
            printf("\t\tbyte [      1] bit [      1] 0 = This LBA range should be visible\n");
        }

        printf("\t\tbyte [ 23: 16] SLBA = 0x%016llX\n", _pstData[i].SLBA);
        printf("\t\tbyte [ 31: 24]  NLB = 0x%016llX\n", _pstData[i].NLB);
        printf("\t\tbyte [ 47: 32] GUID = ");
        {
            int iSum = 0;
            for (int j = 0; j< 16; j++)
            {
                iSum = _pstData[i].GUID[j];
            }
            if (iSum == 0)
            {
                printf("(not implemented)\n");
            }
            else
            {
                // 5 byte = Extension identifer assigned by the organization
                printf("%02X%02X%02X%02X%02X-", _pstData[i].GUID[11], _pstData[i].GUID[12], _pstData[i].GUID[13], _pstData[i].GUID[14], _pstData[i].GUID[15]);

                // 3 byte = Organizationally Unique Identifier (OUI) value assigned by the IEEE Registration Authority
                printf("%02X%02X%02X-", _pstData[i].GUID[8], _pstData[i].GUID[9], _pstData[i].GUID[10]);

                // 8 byte = Vendor specific extension identifier
                printf("%02X%02X%02X%02X%02X%02X%02X%02X\n",
                    _pstData[i].GUID[0], _pstData[i].GUID[1], _pstData[i].GUID[2], _pstData[i].GUID[3], _pstData[i].GUID[4], _pstData[i].GUID[5], _pstData[i].GUID[6], _pstData[i].GUID[7]);
            }
        }
    }
}

static int siNVMeGetFeaturesLBARange(HANDLE _hDevice, NVME_CDW10_GET_FEATURES _cdw10)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    uint64_t ullTimestamp = 0ULL;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)
        + sizeof(NVME_LBA_RANGET_TYPE_ENTRY) * 64; // the max number of entries is 64 in 1.3d
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vUtilPrintSystemError(GetLastError(), "malloc");
        return iResult;
    }

    ZeroMemory(buffer, bufferLength);

    query               = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr   = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData        = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId   = StorageAdapterProtocolSpecificProperty;
    query->QueryType    = PropertyStandardQuery;

    protocolData->ProtocolType                  = ProtocolTypeNvme;
    protocolData->DataType                      = NVMeDataTypeFeature;
    protocolData->ProtocolDataRequestValue      = _cdw10.AsUlong;
    protocolData->ProtocolDataRequestSubValue   = 1; // NSID = 1 ... is location correct?
    protocolData->ProtocolDataOffset            = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength            = sizeof(NVME_LBA_RANGET_TYPE_ENTRY) * 64;

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

    if (iResult == 0)
    {
        // Validate the returned data.
        if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
            (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)))
        {
            printf("[E] NVMeGetFeaturesTimestamp: Data descriptor header not valid.\n");
            iResult = -1; // error
        }
        else
        {
            if (_cdw10.SEL == NVME_FEATURE_VALUE_CURRENT)
            {
                printf("\tCurrent Setting:\n");
            }
            else if (_cdw10.SEL == NVME_FEATURE_VALUE_DEFAULT)
            {
                printf("\tDefault Setting:\n");
            }
            else
            {
                printf("\tSaved Setting:\n");
            }

            if ((protocolData->FixedProtocolReturnData & 0x3F) == 0)
            {
                printf("\t\t[W] No LBA Range Type data is registered, ignore.\n");
                // not error
            }
            else
            {
                s_vNVMePrintLBARange((PNVME_LBA_RANGET_TYPE_ENTRY)((PCHAR)protocolData + protocolData->ProtocolDataOffset),
                    (protocolData->FixedProtocolReturnData & 0x3F));
            }
        }
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

int iNVMeGetFeaturesLBARange(HANDLE _hDevice)
{
    int iResult = -1;
    NVME_CDW10_GET_FEATURES cdw10;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY Cap;

    printf("\n[I] LBA Range Type:\n");

    // 1. Get current value
    cdw10.FID   = NVME_FEATURE_LBA_RANGE_TYPE;
    cdw10.SEL   = NVME_FEATURE_VALUE_CURRENT;
    iResult     = siNVMeGetFeaturesLBARange(_hDevice, cdw10);
    if (iResult != 0) return iResult;

    // 2. Get default value
    cdw10.SEL   = NVME_FEATURE_VALUE_DEFAULT;
    iResult     = siNVMeGetFeaturesLBARange(_hDevice, cdw10);
    if (iResult != 0) return iResult;

    // 3. Get saved value
    cdw10.SEL   = NVME_FEATURE_VALUE_SAVED;
    iResult     = siNVMeGetFeaturesLBARange(_hDevice, cdw10);
    if (iResult != 0) return iResult;

    // 4. get supported capabilities
    iResult = iNVMeGetFeature32(_hDevice, NVME_FEATURE_LBA_RANGE_TYPE, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&Cap);
    if (iResult != 0) return iResult;

    printf("\tCapabilities: this feature is\n"
        "\t\tbit [      2] %d = (1) changable, (0) not changable\n"
        "\t\tbit [      1] %d = (1) namespace specific, (0) for entire controller\n"
        "\t\tbit [      0] %d = (1) savable, (0) not savable\n",
        Cap.MOD, Cap.NSS, Cap.SAVE);

    return iResult;
}
