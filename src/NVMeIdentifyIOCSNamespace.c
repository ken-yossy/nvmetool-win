#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "NVMeIdentifyController.h"
#include "NVMeIdentifyNamespace.h"
#include "WinFunc.h"

typedef struct {
    uint32_t STS : 7;        // bit [ 6: 0] Storage Tag Size
    uint32_t PIF : 2;        // bit [ 8: 7] Protection Information Format
    uint32_t Reserved : 23;  // bit [21: 9]
} NVME_EXTENDED_LBA_FORMAT_FOR_IOCNTRL, *PNVME_EXTENDED_LBA_FORMAT_FOR_IOCNTRL;

typedef struct {
    uint64_t
        LBSTM;  // byte [   7:   0] O - Logical Block Storage Tag Mask (LBSTM)

    struct {
        uint8_t PISTS16B : 1;  // bit [    0] 16b Guard Protection Information
                               // Storage Tag Support (16BPISTS)
        uint8_t PISTM16B : 1;  // bit [    1] 16b Guard Protection Information
                               // Storage Tag Mask (16BPISTM)
        uint8_t Reserved : 6;  // bit [ 7: 2]
    } PIC;  // byte [        8] Protection Information Capabilities (PIC)

    uint8_t Reserved0[3];  // byte [  11:   9]

    NVME_EXTENDED_LBA_FORMAT_FOR_IOCNTRL
        ELBAF[64];  // byte [ 267:  12] Extended LBA Format 0-63 Support

    uint8_t Reserved1[3828];  // byte [4095: 268]
} MY_NVME_IDENTIFY_IOCSPECIFIC_NAMESPACE_IO_CNTRL,
    *PMY_NVME_IDENTIFY_IOCSPECIFIC_NAMESPACE_IO_CNTRL;

static void printNVMeIdentifyNamespaceData(
    PMY_NVME_IDENTIFY_IOCSPECIFIC_NAMESPACE_IO_CNTRL _pNSData,
    uint32_t _ulNSID) {
    printf(
        "[I] NVM Command Set I/O Command Set Specific Identify Namespace (NSID "
        "= %d):\n",
        _ulNSID);
    printf("\n");
    if (!bNamespaceSupportPI(_ulNSID)) {
        printf(
            "[W] This namespace (NSID = %d) does not support Protection "
            "Information, skip.\n",
            _ulNSID);
        return;
    }

    printf("[O] Logical Block Storage Tag Mask (LBSTM): 0x%016llX\n",
           _pNSData->LBSTM);

    printf("[O] Protection Information Capabilities (PIC):\n");
    if (_pNSData->PIC.PISTS16B) {
        printf(
            "\tbit [      0] 1 = 16b Guard Protection Information format "
            "supports a non-zero value in the STS field\n");
    } else {
        printf(
            "\tbit [      0] 0 = 16b Guard Protection Information format "
            "support requires that the STS field be cleared to 0h\n");
    }

    if (_pNSData->PIC.PISTM16B) {
        printf(
            "\tbit [      1] 1 = LBSTM field shall have all bits set to `1' "
            "for the 16b Guard Protection Information\n");
    } else {
        printf(
            "\tbit [      1] 0 = LBSTM field is allowd to have any bits set to "
            "`1' for the 16b Guard Protection Information\n");
    }

    for (int i = 0; i < 64; i++) {
        printf("[O] Extended LBA Format #%d (ELBAF%d) :\n", i, i);
        printf("\tbit [  6:  0] %2d = Storage Tag Size (STS)\n",
               _pNSData->ELBAF[i].STS);
        switch (_pNSData->ELBAF[i].PIF) {
            case 0:
                printf(
                    "\tbit [  8:  7]  0 = Protection Information Format (PIF) "
                    "is 16b Guard Protection Information\n");
                break;

            case 1:
                printf(
                    "\tbit [  8:  7]  1 = Protection Information Format (PIF) "
                    "is 32b Guard Protection Information\n");
                break;

            case 2:
                printf(
                    "\tbit [  8:  7]  2 = Protection Information Format (PIF) "
                    "is 64b Guard Protection Information\n");
                break;

            case 3:
            default:
                printf(
                    "\tbit [  8:  7] %d = Protection Information Format (PIF) "
                    "is unknown\n",
                    _pNSData->ELBAF[i].PIF);
                break;
        }
    }
}

int siNVMeIdentifyIOControllerNamespace(HANDLE _hDevice, DWORD _dwNSID) {
    int iResult = -1;
    PVOID buffer = NULL;
    ULONG bufferLength = 0;
    ULONG returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters) +
                   sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) +
                   sizeof(MY_NVME_IDENTIFY_IOCSPECIFIC_NAMESPACE_IO_CNTRL);
    buffer = malloc(bufferLength);

    if (buffer == NULL) {
        vPrintSystemError(GetLastError(), "malloc");
        goto error_exit;
    }

    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeIdentify;
    protocolData->ProtocolDataRequestValue =
        NVME_IDENTIFY_CNS_SPECIFIC_NAMESPACE_IO_COMMAND_SET;
    protocolData->ProtocolDataRequestSubValue = _dwNSID;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength =
        sizeof(MY_NVME_IDENTIFY_IOCSPECIFIC_NAMESPACE_IO_CNTRL);

    // Send request down.
    iResult = iIssueDeviceIoControl(_hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                                    buffer, bufferLength, buffer, bufferLength,
                                    &returnedLength, NULL);

    if (iResult) goto error_exit;

    printf("\n");

    //
    // Validate the returned data.
    //
    if ((protocolDataDescr->Version !=
         sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        fprintf(stderr,
                "[E] NVMeIdentifyNamespace: data descriptor header not valid, "
                "stop.\n");
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset >
         sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength <
         sizeof(MY_NVME_IDENTIFY_IOCSPECIFIC_NAMESPACE_IO_CNTRL))) {
        fprintf(stderr,
                "[E] NVMeIdentifyNamespace: ProtocolData Offset/Length not "
                "valid, stop.\n");
        goto error_exit;
    }

    // Identify Namespace
    printNVMeIdentifyNamespaceData(
        (PMY_NVME_IDENTIFY_IOCSPECIFIC_NAMESPACE_IO_CNTRL)((PCHAR)protocolData +
                                                           protocolData
                                                               ->ProtocolDataOffset),
        _dwNSID);
    iResult = 0;  // succeeded

error_exit:

    if (buffer != NULL) {
        free(buffer);
    }

    return iResult;
}

int iNVMeIdentifyIOCSNamespace(HANDLE _hDevice, uint32_t _ulNSID) {
    int iResult = -1;

    if (!bNamespaceAlreadyIdentified(_ulNSID))
        if (iNVMeIdentifyNamespace(_hDevice, (DWORD)_ulNSID, false)) return -1;

    if (bIsNVMeV20OrLater()) {
        switch (g_stController.CNTRLTYPE) {
            case 0:  // Reserved (controller type not reported
                fprintf(stderr,
                        "[W] This controller doesn't report controller type, "
                        "skip.\n");
                break;

            case 1:  // I/O Controller
                iResult =
                    siNVMeIdentifyIOControllerNamespace(_hDevice, _ulNSID);
                break;

            case 2:  // Discovery controller
            case 3:  // Administrative controller
                fprintf(stderr,
                        "[W] This tool doesn't support this type ofr "
                        "controller (%02X), skip.\n",
                        g_stController.CNTRLTYPE);
                break;

            default:  // Reserved
                fprintf(stderr,
                        "[W] This tool doesn't support this type ofr "
                        "controller (%02X), skip.\n",
                        g_stController.CNTRLTYPE);
                break;
        }
    } else {
        fprintf(
            stderr,
            "[W] This data is available NVMe Revision 2.0 or later, skip.\n");
    }

    return iResult;
}
