#include <windows.h>
#include <stdio.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeUtils.h"
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

    PSTORAGE_PROPERTY_SET query                             = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT protocolData        = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_SET, AdditionalParameters)
        + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA_EXT);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError(GetLastError(), "malloc");
        return iResult;
    }

    ZeroMemory(buffer, bufferLength);

    query               = (PSTORAGE_PROPERTY_SET)buffer;
    protocolDataDescr   = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR_EXT)buffer;
    protocolData        = (PSTORAGE_PROTOCOL_SPECIFIC_DATA_EXT)query->AdditionalParameters;

    query->PropertyId   = StorageDeviceProtocolSpecificProperty;
    query->SetType      = PropertyStandardSet;

    protocolData->ProtocolType          = ProtocolTypeNvme;
    protocolData->DataType              = NVMeDataTypeFeature;
    protocolData->ProtocolDataValue     = _cdw10;
    protocolData->ProtocolDataSubValue  = _cdw11;
    protocolData->ProtocolDataOffset    = 0;
    protocolData->ProtocolDataLength    = 0;

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
        printf("\n[I] Command Set Features succeeded.\n");
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}

int iNVMeSetFeaturesHCTM(HANDLE _hDevice)
{
    int iResult = -1;
    int iTMT1, iTMT2;

    if (g_stController.HCTMA.Supported == 0)
    {
        fprintf(stderr, "\n[E] This SSD controller does not support Host Controlled Thermal Management (HCTM), skip\n");
        return iResult;
    }

    printf("\n[I] Minimum Thermal Management Temperature (MNTMT) is %d (K)", g_stController.MNTMT);
    printf("\n[I] Maximum Thermal Management Temperature (MXTMT) is %d (K)", g_stController.MXTMT);
    printf("\n[I] You can speficy TMT1 and TMT2, satisfying TMT1 < TMT2, MNTMT <= TMT1, and TMT2 <= MXTMT\n");

    {
        char cCmd;
        char strCmd[256];
        char strPrompt[1024];

        iTMT1 = iGetConsoleInputDec("\n# Input Thermal Management Temperature 1 (TMT1) in Kelvin (lower temperature)\n", strCmd);
        iTMT2 = iGetConsoleInputDec("\n# Input Thermal Management Temperature 2 (TMT2) in Kelvin (higher temperature)\n", strCmd);

        sprintf_s(strPrompt, 1024, "\n# You specified TMT1 = %d (K) and TMT2 = %d (K), press 'y' to continue\n", iTMT1, iTMT2);
        cCmd = cGetConsoleInput(strPrompt, strCmd);
        if ((cCmd != 'y') && (cCmd != 'Y'))
        {
            printf("\n[I] Process aborted\n");
            return 0;
        }
    }

    if (!(iTMT1 < iTMT2))
    {
        fprintf(stderr, "\n[E] Should be TMT1 < TMT2, you tried TMT1 = %d and TMT2 = %d, process aborted\n", iTMT1, iTMT2);
        return iResult;
    }

    if (!(g_stController.MNTMT <= iTMT1))
    {
        fprintf(stderr, "\n[E] Should be MNTMT <= TMT1, you tried TMT1 = %d against MNTMT = %d, process aborted\n", iTMT1, g_stController.MNTMT);
        return iResult;
    }

    if (!(iTMT2 <= g_stController.MXTMT))
    {
        fprintf(stderr, "\n[E] Should be TMT2 <= MXTMT, you tried TMT2 = %d against MXTMT = %d, process aborted\n", iTMT2, g_stController.MXTMT);
        return iResult;
    }

    {
        NVME_CDW10_SET_FEATURES cdw10 = {0};
        NVME_CDW11_FEATURE_HCTM cdw11;

        cdw10.FID   = NVME_FEATURE_HOST_CONTROLLED_THERMAL_MANAGEMENT;
        cdw10.SV    = 0; // not saved
        cdw11.TMT1  = iTMT1;
        cdw11.TMT2  = iTMT2;

        iResult = siNVMeSetFeaturesHCTM(_hDevice, cdw10.AsUlong, cdw11.AsUlong);
        if (iResult != 0)
        {
            return 0;
        }
    }

    return 1;
}
