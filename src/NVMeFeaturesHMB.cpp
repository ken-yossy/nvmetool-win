#include "WinFunc.h"
#include <iostream>
#include <nvme.h>

#include "NVMeUtils.h"
#include "NVMeGetFeatures.h"

typedef struct {
    NVME_CDW12_FEATURE_HOST_MEMORY_BUFFER   HSIZE;      // byte [   3: 0]
    NVME_CDW13_FEATURE_HOST_MEMORY_BUFFER   HMDLLA;     // byte [   7: 4]
    NVME_CDW14_FEATURE_HOST_MEMORY_BUFFER   HMDLUA;     // byte [  11: 8]
    NVME_CDW15_FEATURE_HOST_MEMORY_BUFFER   HMDLEC;     // byte [  15:12]
    uint8_t                                 rsvd[4080]; // byte [4095:16]
} NVME_GET_FEATURE_HOST_MEMORY_BUFFER, *PNVME_GET_FEATURE_HOST_MEMORY_BUFFER;

static void s_vNVMePrintHMB(PNVME_GET_FEATURE_HOST_MEMORY_BUFFER _pstData)
{
    printf("\t\tbyte [  3:  0] Buffer Size (HSIZE) = %d (in the unit of memory page size indicated in CC.MPS)\n", _pstData->HSIZE.HSIZE);
    printf("\t\tbyte [  7:  4] Descriptor List Address Lower (HMDLAL) = 0x%08X\n", _pstData->HMDLLA.AsUlong);
    printf("\t\tbyte [ 11:  8] Descriptor List Address Upper (HMDLAU) = 0x%08X\n", _pstData->HMDLUA.HMDLUA);
    printf("\t\tbyte [ 15: 12] Descriptor List Entry Count (HMDLEC)   = %d\n", _pstData->HMDLEC.HMDLEC);
}

static int siNVMeGetFeaturesHMB(HANDLE _hDevice, NVME_CDW10_GET_FEATURES _cdw10)
{
    int     iResult         = -1;
    PVOID   buffer          = NULL;
    ULONG   bufferLength    = 0;
    ULONG   returnedLength  = 0;

    PSTORAGE_PROPERTY_QUERY query                       = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData        = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)
        + sizeof(NVME_GET_FEATURE_HOST_MEMORY_BUFFER);
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
    protocolData->ProtocolDataOffset            = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength            = sizeof(NVME_GET_FEATURE_HOST_MEMORY_BUFFER);

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
            printf("[E] NVMeGetFeaturesHMB: Data descriptor header not valid.\n");
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

            s_vNVMePrintHMB((PNVME_GET_FEATURE_HOST_MEMORY_BUFFER)((PCHAR)protocolData + protocolData->ProtocolDataOffset));
        }
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

int iNVMeGetFeaturesHMB(HANDLE _hDevice)
{
    int iResult = -1;
    NVME_CDW10_GET_FEATURES cdw10;
    NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY Cap;

    printf("\n[I] Host Memory Buffer (HMB):\n");

    // 1. Get current value
    cdw10.FID   = NVME_FEATURE_HOST_MEMORY_BUFFER;
    cdw10.SEL   = NVME_FEATURE_VALUE_CURRENT;
    iResult     = siNVMeGetFeaturesHMB(_hDevice, cdw10);
    if (iResult != 0) return iResult;

    // 2. Get default value
    cdw10.SEL   = NVME_FEATURE_VALUE_DEFAULT;
    iResult     = siNVMeGetFeaturesHMB(_hDevice, cdw10);
    if (iResult != 0) return iResult;

    // 3. Get saved value
    cdw10.SEL   = NVME_FEATURE_VALUE_SAVED;
    iResult     = siNVMeGetFeaturesHMB(_hDevice, cdw10);
    if (iResult != 0) return iResult;

    // 4. get supported capabilities
    iResult = iNVMeGetFeature32(_hDevice, NVME_FEATURE_HOST_MEMORY_BUFFER, NVME_FEATURE_VALUE_SUPPORTED_CAPABILITIES, 0, (uint32_t*)&Cap);
    if (iResult != 0) return iResult;

    vNVMeGetFeaturesShowCapabilities(Cap);

    return iResult;
}
