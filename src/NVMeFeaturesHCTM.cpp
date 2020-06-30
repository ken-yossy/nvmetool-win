#include "WinFunc.h"
#include <iostream>
#include <nvme.h>

#include "NVMeUtils.h"
#include "NVMeGetFeatures.h"
#include "NVMeSCSIPassThrough.h"
#include "NVMeIdentifyController.h"

typedef union {

    struct {
        ULONG   TMT2 : 16; // Thermal Management Temperature 2 (TMT2); in Kelvin
        ULONG   TMT1 : 16; // Thermal Management Temperature 1 (TMT1); in Kelvin
    } DUMMYSTRUCTNAME;

    ULONG   AsUlong;

} NVME_CDW11_FEATURE_HCTM, *PNVME_CDW11_FEATURE_HCTM;

static int siNVMeSetFeaturesHCTM(HANDLE _hDevice, DWORD _cdw10, DWORD _cdw11)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PSTORAGE_PROPERTY_SET query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vUtilPrintSystemError(GetLastError(), "malloc");
        return iResult;
    }

    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_SET)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageDeviceProtocolSpecificProperty;
    query->SetType = PropertyStandardSet;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeFeature;
    protocolData->ProtocolDataRequestValue    = _cdw10;
    protocolData->ProtocolDataRequestSubValue = _cdw11;
    protocolData->ProtocolDataOffset = 0;
    protocolData->ProtocolDataLength = 0;

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
            printf("[E] NVMeSetFeature: Data descriptor header not valid.\n");
            iResult = -1; // error
        }
        else
        {
            printf("[I] Command Set Features succeeded.\n");
        }
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

int iNVMeSetFeaturesHCTM(HANDLE _hDevice)
{
    int result  = false;
    int iResult = -1;
    int iTMT1, iTMT2;

    {
        char cCmd;
        char strCmd[256];
        char strPrompt[1024];

        iTMT1 = iGetConsoleInputDec("\n# Input Thermal Management Temperature 1 (TMT1) in Kelvin (lower temperature)", strCmd);
        iTMT2 = iGetConsoleInputDec("\n# Input Thermal Management Temperature 1 (TMT2) in Kelvin (higher temperature)", strCmd);

        sprintf_s(strPrompt, 1024, "\n# You specified TMT1 = %d (K) and TMT2 = %d (K), press 'y' to continue\n", iTMT1, iTMT2);
        cCmd = cGetConsoleInput(strPrompt, strCmd);
        if ((cCmd != 'y') && (cCmd != 'Y'))
        {
            printf("\n[I] Process aborted\n");
            return false;
        }
    }

    if (!(iTMT1 < iTMT2))
    {
        printf("\n[E] Should be TMT1 < TMT2, you tried TMT1 = %d and TMT2 = %d, process aborted\n", iTMT1, iTMT2);
        return false;
    }

    if (!(g_stController.MNTMT <= iTMT1))
    {
        printf("\n[E] Should be MNTMT <= TMT1, you tried TMT1 = %d against MNTMT = %d, process aborted\n", iTMT1, g_stController.MNTMT);
        return false;
    }

    if (!(iTMT2 <= g_stController.MXTMT))
    {
        printf("\n[E] Should be TMT2 <= MXTMT, you tried TMT2 = %d against MXTMT = %d, process aborted\n", iTMT2, g_stController.MXTMT);
        return false;
    }

    {
        NVME_CDW10_SET_FEATURES cdw10;
        NVME_CDW11_FEATURE_HCTM cdw11;

        cdw10.FID  = FEATURE_ID_HOST_CONTROLLED_THERMAL_MANAGEMENT;
        cdw10.SV = 0; // temporary
        cdw11.TMT1 = iTMT1;
        cdw11.TMT2 = iTMT2;

        iResult = siNVMeSetFeaturesHCTM(_hDevice, cdw10.AsUlong, cdw11.AsUlong);
        if (iResult != 0)
        {
            return false;
        }
    }

    return true;
}
