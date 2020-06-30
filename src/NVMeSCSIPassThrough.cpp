#include "WinFunc.h"
#include <iostream>
#include <ntddscsi.h>
#include <string.h>
#include <intsafe.h>

#define _NTSCSI_USER_MODE_
#include <scsi.h>

#define SPT_CDB_LENGTH 32
#define SPT_SENSE_LENGTH 32
#define SPTWB_DATA_LENGTH 512

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS
{
    SCSI_PASS_THROUGH spt;
    ULONG             Filler;      // realign buffers to double word boundary
    UCHAR             ucSenseBuf[SPT_SENSE_LENGTH];
    UCHAR             ucDataBuf[SPTWB_DATA_LENGTH];
} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;

typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER
{
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG             Filler;      // realign buffer to double word boundary
    UCHAR             ucSenseBuf[SPT_SENSE_LENGTH];
} SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, *PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS_EX
{
    SCSI_PASS_THROUGH_EX spt;
    UCHAR             ucCdbBuf[SPT_CDB_LENGTH - 1];       // cushion for spt.Cdb
    ULONG             Filler;      // realign buffers to double word boundary
    STOR_ADDR_BTL8    StorAddress;
    UCHAR             ucSenseBuf[SPT_SENSE_LENGTH];
    UCHAR             ucDataBuf[SPTWB_DATA_LENGTH];     // buffer for DataIn or DataOut
} SCSI_PASS_THROUGH_WITH_BUFFERS_EX, *PSCSI_PASS_THROUGH_WITH_BUFFERS_EX;

typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX
{
    SCSI_PASS_THROUGH_DIRECT_EX sptd;
    UCHAR             ucCdbBuf[SPT_CDB_LENGTH - 1];       // cushion for sptd.Cdb
    ULONG             Filler;      // realign buffer to double word boundary
    STOR_ADDR_BTL8    StorAddress;
    UCHAR             ucSenseBuf[SPT_SENSE_LENGTH];
} SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX, *PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX;

#define CDB6GENERIC_LENGTH                   6
#define CDB10GENERIC_LENGTH                  10
#define SETBITON                             1
#define SETBITOFF                            0

#define BOOLEAN_TO_STRING(_b_) ((_b_) ? "True" : "False")

#if defined(_X86_)
#define PAGE_SIZE  0x1000
#define PAGE_SHIFT 12L
#elif defined(_AMD64_)
#define PAGE_SIZE  0x1000
#define PAGE_SHIFT 12L
#elif defined(_IA64_)
#define PAGE_SIZE 0x2000
#define PAGE_SHIFT 13L
#else
// undefined platform?
#define PAGE_SIZE  0x1000
#define PAGE_SHIFT 12L
#endif

LPCSTR BusTypeStrings[] = {
    "Unknown",
    "Scsi",
    "Atapi",
    "Ata",
    "1394",
    "Ssa",
    "Fibre",
    "Usb",
    "RAID",
    "Not Defined",
};

#define NUMBER_OF_BUS_TYPE_STRINGS (sizeof(BusTypeStrings)/sizeof(BusTypeStrings[0]))

static VOID PrintSenseInfo(PSCSI_PASS_THROUGH_WITH_BUFFERS psptwb)
{
    UCHAR i;

    fprintf(stderr, "Scsi status: %02Xh\n\n", psptwb->spt.ScsiStatus);
    if (psptwb->spt.SenseInfoLength == 0) return;

    fprintf(stderr, "Sense Info -- consult SCSI spec for details\n");
    fprintf(stderr, "-------------------------------------------------------------\n");
    for (i = 0; i < psptwb->spt.SenseInfoLength; i++)
    {
        fprintf(stderr, "%02X ", psptwb->ucSenseBuf[i]);
    }
    fprintf(stderr, "\n\n");
}

static void PrintDataBuffer(PUCHAR DataBuffer, ULONG BufferLength)
{
    ULONG Cnt;
    UCHAR Str[32] = { 0 };

    fprintf(stdout, "        00  01  02  03  04  05  06  07   08  09  0A  0B  0C  0D  0E  0F\n");
    fprintf(stdout, "        ---------------------------------------------------------------\n");

    int i = 0;
    for (Cnt = 0; Cnt < BufferLength; Cnt++)
    {
        // print address
        if ((Cnt) % 16 == 0)
        {
            fprintf(stdout, " 0x%03X  ", Cnt);
        }

        // print hex data
        fprintf(stdout, "%02X  ", DataBuffer[Cnt]);
        if (isprint(DataBuffer[Cnt]))
        {
            Str[i] = DataBuffer[Cnt];
        }
        else
        {
            Str[i] = '.';
        }
        i++;
        if ((Cnt + 1) % 8 == 0)
        {
            fprintf(stdout, " ");
            Str[i++] = ' ';
        }

        // print ascii character if printable
        if ((Cnt + 1) % 16 == 0)
        {
            Str[i++] = '\0';
            i = 0;
            fprintf(stdout, "%s\n", Str);
        }
    }
    fprintf(stdout, "\n\n");
}

static void PrintSenseInfoEx(PSCSI_PASS_THROUGH_WITH_BUFFERS_EX psptwb_ex)
{
    ULONG i;

    fprintf(stderr, "Scsi status: %02Xh\n\n", psptwb_ex->spt.ScsiStatus);
    if (psptwb_ex->spt.SenseInfoLength == 0)
    {
        return;
    }

    fprintf(stderr, "Sense Info -- consult SCSI spec for details\n");
    fprintf(stderr, "-------------------------------------------------------------\n");
    for (i = 0; i < psptwb_ex->spt.SenseInfoLength; i++)
    {
        fprintf(stderr, "%02X ", psptwb_ex->ucSenseBuf[i]);
    }
    fprintf(stderr, "\n\n");
}

static void PrintStatusResultsEx(int status, DWORD returned, PSCSI_PASS_THROUGH_WITH_BUFFERS_EX psptwb_ex,	ULONG length)
{
    if (status)
    {
        vUtilPrintSystemError( GetLastError(), "(Unknown)" );
        return;
    }

    if (psptwb_ex->spt.ScsiStatus)
    {
        PrintSenseInfoEx(psptwb_ex);
        return;
    }
    else
    {
        fprintf(stdout, "Scsi status: %02Xh, Bytes returned: %Xh, ",	psptwb_ex->spt.ScsiStatus, returned);
        fprintf(stdout, "DataOut buffer length: %Xh\nDataIn buffer length: %Xh\n\n\n", psptwb_ex->spt.DataOutTransferLength, psptwb_ex->spt.DataInTransferLength);
        PrintDataBuffer((PUCHAR)(psptwb_ex->ucDataBuf), length);
    }
}

static void PrintAdapterDescriptor(PSTORAGE_ADAPTER_DESCRIPTOR AdapterDescriptor)
{
    ULONG trueMaximumTransferLength;
    LPCSTR busType;

    if (AdapterDescriptor->BusType < NUMBER_OF_BUS_TYPE_STRINGS) {
        busType = BusTypeStrings[AdapterDescriptor->BusType];
    }
    else
    {
        busType = BusTypeStrings[NUMBER_OF_BUS_TYPE_STRINGS - 1];
    }

    // subtract one page, as transfers do not always start on a page boundary
    if (AdapterDescriptor->MaximumPhysicalPages > 1)
    {
        trueMaximumTransferLength = AdapterDescriptor->MaximumPhysicalPages - 1;
    }
    else
    {
        trueMaximumTransferLength = 1;
    }

    // make it into a byte value
    trueMaximumTransferLength <<= PAGE_SHIFT;

    // take the minimum of the two
    if (trueMaximumTransferLength > AdapterDescriptor->MaximumTransferLength)
    {
        trueMaximumTransferLength = AdapterDescriptor->MaximumTransferLength;
    }

    // always allow at least a single page transfer
    if (trueMaximumTransferLength < PAGE_SIZE)
    {
        trueMaximumTransferLength = PAGE_SIZE;
    }

    fprintf(stdout, "\n            ***** STORAGE ADAPTER DESCRIPTOR DATA *****");
    fprintf(stdout,
        "              Version: %08x\n"
        "            TotalSize: %08x\n"
        "MaximumTransferLength: %08x (bytes)\n"
        " MaximumPhysicalPages: %08x\n"
        "  TrueMaximumTransfer: %08x (bytes)\n"
        "        AlignmentMask: %08x\n"
        "       AdapterUsesPio: %s\n"
        "     AdapterScansDown: %s\n"
        "      CommandQueueing: %s\n"
        "  AcceleratedTransfer: %s\n"
        "             Bus Type: %s\n"
        "    Bus Major Version: %04x\n"
        "    Bus Minor Version: %04x\n",
        AdapterDescriptor->Version,
        AdapterDescriptor->Size,
        AdapterDescriptor->MaximumTransferLength,
        AdapterDescriptor->MaximumPhysicalPages,
        trueMaximumTransferLength,
        AdapterDescriptor->AlignmentMask,
        BOOLEAN_TO_STRING(AdapterDescriptor->AdapterUsesPio),
        BOOLEAN_TO_STRING(AdapterDescriptor->AdapterScansDown),
        BOOLEAN_TO_STRING(AdapterDescriptor->CommandQueueing),
        BOOLEAN_TO_STRING(AdapterDescriptor->AcceleratedTransfer),
        busType,
        AdapterDescriptor->BusMajorVersion,
        AdapterDescriptor->BusMinorVersion);

    fprintf(stdout, "\n\n");
}

static void PrintDeviceDescriptor(PSTORAGE_DEVICE_DESCRIPTOR DeviceDescriptor)
{
    LPCSTR vendorId = "";
    LPCSTR productId = "";
    LPCSTR productRevision = "";
    LPCSTR serialNumber = "";
    LPCSTR busType;

    if ((ULONG)DeviceDescriptor->BusType < NUMBER_OF_BUS_TYPE_STRINGS)
    {
        busType = BusTypeStrings[DeviceDescriptor->BusType];
    }
    else
    {
        busType = BusTypeStrings[NUMBER_OF_BUS_TYPE_STRINGS - 1];
    }

    if ((DeviceDescriptor->ProductIdOffset != 0) &&	(DeviceDescriptor->ProductIdOffset != -1))
    {
        productId = (LPCSTR)(DeviceDescriptor);
        productId += (ULONG_PTR)DeviceDescriptor->ProductIdOffset;
    }

    if ((DeviceDescriptor->VendorIdOffset != 0) && (DeviceDescriptor->VendorIdOffset != -1))
    {
        vendorId = (LPCSTR)(DeviceDescriptor);
        vendorId += (ULONG_PTR)DeviceDescriptor->VendorIdOffset;
    }

    if ((DeviceDescriptor->ProductRevisionOffset != 0) && (DeviceDescriptor->ProductRevisionOffset != -1))
    {
        productRevision = (LPCSTR)(DeviceDescriptor);
        productRevision += (ULONG_PTR)DeviceDescriptor->ProductRevisionOffset;
    }

    if ((DeviceDescriptor->SerialNumberOffset != 0) && (DeviceDescriptor->SerialNumberOffset != -1))
    {
        serialNumber = (LPCSTR)(DeviceDescriptor);
        serialNumber += (ULONG_PTR)DeviceDescriptor->SerialNumberOffset;
    }

    fprintf(stdout, "\n            ***** STORAGE DEVICE DESCRIPTOR DATA *****");
    fprintf(stdout,
        "              Version: %08x\n"
        "            TotalSize: %08x\n"
        "           DeviceType: %08x\n"
        "   DeviceTypeModifier: %08x\n"
        "       RemovableMedia: %s\n"
        "      CommandQueueing: %s\n"
        "            Vendor Id: %s\n"
        "           Product Id: %s\n"
        "     Product Revision: %s\n"
        "        Serial Number: %s\n"
        "             Bus Type: %s\n"
        "       Raw Properties: %s\n",
        DeviceDescriptor->Version,
        DeviceDescriptor->Size,
        DeviceDescriptor->DeviceType,
        DeviceDescriptor->DeviceTypeModifier,
        BOOLEAN_TO_STRING(DeviceDescriptor->RemovableMedia),
        BOOLEAN_TO_STRING(DeviceDescriptor->CommandQueueing),
        vendorId,
        productId,
        productRevision,
        serialNumber,
        busType,
        (DeviceDescriptor->RawPropertiesLength ? "Follows" : "None"));

    if (DeviceDescriptor->RawPropertiesLength != 0)
    {
        PrintDataBuffer(DeviceDescriptor->RawDeviceProperties, DeviceDescriptor->RawPropertiesLength);
    }

    fprintf(stdout, "\n\n");
}

static BOOL QueryPropertyForDevice(HANDLE DeviceHandle, PULONG AlignmentMask, PUCHAR SrbType)
{
    PSTORAGE_ADAPTER_DESCRIPTOR adapterDescriptor = NULL;
    PSTORAGE_DEVICE_DESCRIPTOR deviceDescriptor = NULL;
    STORAGE_DESCRIPTOR_HEADER header = { 0 };
    int ng = 0;
    BOOL status = false;
    ULONG i;

    *AlignmentMask = 0; // default to no alignment
    *SrbType = 0; // default to SCSI_REQUEST_BLOCK

    // Loop twice:
    //  First, get size required for storage adapter descriptor
    //  Second, allocate and retrieve storage adapter descriptor
    //  Third, get size required for storage device descriptor
    //  Fourth, allocate and retrieve storage device descriptor

    for (i = 0; i < 4; i++)
    {
        PVOID buffer = NULL;
        ULONG bufferSize = 0;
        ULONG returnedData;
        STORAGE_PROPERTY_QUERY query;
        ZeroMemory((void*)(&query), sizeof(STORAGE_PROPERTY_QUERY));

        switch (i)
        {
        case 0:
            {
                query.QueryType = PropertyStandardQuery;
                query.PropertyId = StorageAdapterProperty;
                bufferSize = sizeof(STORAGE_DESCRIPTOR_HEADER);
                buffer = &header;
                break;
            }

        case 1:
            {
                query.QueryType = PropertyStandardQuery;
                query.PropertyId = StorageAdapterProperty;
                bufferSize = header.Size;
                if (bufferSize != 0)
                {
                    adapterDescriptor = (PSTORAGE_ADAPTER_DESCRIPTOR)LocalAlloc(LPTR, bufferSize);
                    if (adapterDescriptor == NULL) {
                        goto Cleanup;
                    }
                }
                buffer = adapterDescriptor;
                break;
            }

        case 2:
            {
                query.QueryType = PropertyStandardQuery;
                query.PropertyId = StorageDeviceProperty;
                bufferSize = sizeof(STORAGE_DESCRIPTOR_HEADER);
                buffer = &header;
                break;
            }

        case 3:
            {
                query.QueryType = PropertyStandardQuery;
                query.PropertyId = StorageDeviceProperty;
                bufferSize = header.Size;
                if (bufferSize != 0)
                {
                    deviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)LocalAlloc(LPTR, bufferSize);
                    if (deviceDescriptor == NULL) {
                        goto Cleanup;
                    }
                }
                buffer = deviceDescriptor;
                break;
            }
        }

        // buffer can be NULL if the property queried DNE.
        if (buffer != NULL)
        {
            RtlZeroMemory(buffer, bufferSize);

            // all setup, do the ioctl
            ng = iIssueDeviceIoControl(
                DeviceHandle,
                IOCTL_STORAGE_QUERY_PROPERTY,
                &query,
                sizeof(STORAGE_PROPERTY_QUERY),
                buffer,
                bufferSize,
                &returnedData,
                FALSE);

            if (ng)
            {
                if (GetLastError() == ERROR_MORE_DATA)
                {
                    // this is ok, we'll ignore it here
                }
                else if (GetLastError() == ERROR_INVALID_FUNCTION)
                {
                    // this is also ok, the property DNE
                }
                else if (GetLastError() == ERROR_NOT_SUPPORTED)
                {
                    // this is also ok, the property DNE
                }
                else
                {
                    // some unexpected error -- exit out
                    goto Cleanup;
                }

                // zero it out, just in case it was partially filled in.
                RtlZeroMemory(buffer, bufferSize);
            }
        }
    } // end i loop

    // adapterDescriptor is now allocated and full of data.
    // deviceDescriptor is now allocated and full of data.
    if (adapterDescriptor == NULL)
    {
        fprintf(stderr, "   ***** No adapter descriptor supported on the device *****\n");
    }
    else
    {
//		PrintAdapterDescriptor(adapterDescriptor);
        *AlignmentMask = adapterDescriptor->AlignmentMask;
        *SrbType = adapterDescriptor->SrbType;
        status = true;
    }

    if (deviceDescriptor == NULL)
    {
        fprintf(stderr, "   ***** No device descriptor supported on the device  *****\n");
    }
    else
    {
//		PrintDeviceDescriptor(deviceDescriptor);
        status = true;
    }

Cleanup:

    if (adapterDescriptor != NULL)
    {
        LocalFree(adapterDescriptor);
    }

    if (deviceDescriptor != NULL)
    {
        LocalFree(deviceDescriptor);
    }

    return status;
}

static void PrintStatusResults(int status, DWORD returned, PSCSI_PASS_THROUGH_WITH_BUFFERS psptwb, ULONG length)
{
    if (status)
    {
        vUtilPrintSystemError( GetLastError(), "(Unknown)" );
        return;
    }

    if (psptwb->spt.ScsiStatus)
    {
        PrintSenseInfo(psptwb);
        return;
    }
    else
    {
        fprintf(stderr, "Scsi status: %02Xh, Bytes returned: %Xh, ", psptwb->spt.ScsiStatus, returned);
        fprintf(stdout, "Data buffer length: %Xh\n\n\n", psptwb->spt.DataTransferLength);
        PrintDataBuffer((PUCHAR)(psptwb->ucDataBuf), length);
    }
}

static BOOL TestViaSCSIPassThrough(HANDLE _hDevice, PULONG _alignmentMask, PUCHAR _srbType)
{
    BOOL status = false;
    ULONG errorCode = 0;

    status = QueryPropertyForDevice(_hDevice, _alignmentMask, _srbType);
    if (!status)
    {
        errorCode = GetLastError();
        fprintf(stderr, "[E] Error getting device and/or adapter properties.\n");
        vUtilPrintSystemError( GetLastError(), "(Unknown)" );
    }

    return status;
}

static PUCHAR AllocateAlignedBuffer(ULONG size, ULONG AlignmentMask, PUCHAR *pUnAlignedBuffer)
{
    PUCHAR ptr;

    UINT_PTR align64 = (UINT_PTR)AlignmentMask;

    if (AlignmentMask == 0)
    {
        ptr = (PUCHAR)malloc(size);
        *pUnAlignedBuffer = ptr;
    }
    else
    {
        ULONG totalSize;
        (void)ULongAdd(size, AlignmentMask, &totalSize);
        ptr = (PUCHAR)malloc(totalSize);
        *pUnAlignedBuffer = ptr;
        ptr = (PUCHAR)(((UINT_PTR)ptr + align64) & ~align64);
    }

    if (ptr == NULL)
    {
        vUtilPrintSystemError(GetLastError(), "malloc");
    }
    return ptr;
}

int iWriteViaSCSIPassThrough(HANDLE _hDevice)
{
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX sptdwb_ex;
    int iResult = -1;
    ULONG length = 0, errorCode = 0, returned = 0, sectorSize = 512;

    ULONG alignmentMask = 0; // default == no alignment requirement
    UCHAR srbType = 0; // default == SRB_TYPE_SCSI_REQUEST_BLOCK

    if (TestViaSCSIPassThrough(_hDevice, &alignmentMask, &srbType) == false) return false;

    if (srbType)
    {
        PUCHAR pUnAlignedBuffer = NULL;
        PUCHAR databuffer = (PUCHAR)AllocateAlignedBuffer(sectorSize, alignmentMask, &pUnAlignedBuffer);
        if (databuffer == NULL) return false;

        ZeroMemory(databuffer, 512);

        for (int i = 0; i < 512 / 8; i++)
        {
            databuffer[i * 8] = 'D';
            databuffer[i * 8 + 1] = 'E';
            databuffer[i * 8 + 2] = 'A';
            databuffer[i * 8 + 3] = 'D';
            databuffer[i * 8 + 4] = 'B';
            databuffer[i * 8 + 5] = 'E';
            databuffer[i * 8 + 6] = 'E';
            databuffer[i * 8 + 7] = 'F';
        }

        ZeroMemory(&sptdwb_ex, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX));
        sptdwb_ex.sptd.Version					= 0;
        sptdwb_ex.sptd.Length					= sizeof(SCSI_PASS_THROUGH_DIRECT_EX);
        sptdwb_ex.sptd.ScsiStatus				= 0;
        sptdwb_ex.sptd.CdbLength				= CDB10GENERIC_LENGTH;
        sptdwb_ex.sptd.StorAddressLength		= sizeof(STOR_ADDR_BTL8);
        sptdwb_ex.sptd.SenseInfoLength			= SPT_SENSE_LENGTH;
        sptdwb_ex.sptd.DataOutBuffer			= (PVOID)databuffer;
        sptdwb_ex.sptd.DataOutTransferLength	= sectorSize;
        sptdwb_ex.sptd.DataInTransferLength		= 0;
        sptdwb_ex.sptd.DataDirection			= SCSI_IOCTL_DATA_OUT;
        sptdwb_ex.sptd.TimeOutValue				= 5;
        sptdwb_ex.sptd.StorAddressOffset		= offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX, StorAddress);
        sptdwb_ex.StorAddress.Type				= STOR_ADDRESS_TYPE_BTL8;
        sptdwb_ex.StorAddress.Port				= 0;
        sptdwb_ex.StorAddress.AddressLength		= STOR_ADDR_BTL8_ADDRESS_LENGTH;
        sptdwb_ex.StorAddress.Path				= 0;
        sptdwb_ex.StorAddress.Target			= 0;
        sptdwb_ex.StorAddress.Lun				= 0;
        sptdwb_ex.sptd.SenseInfoOffset			= offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX, ucSenseBuf);
        sptdwb_ex.sptd.Cdb[0]					= SCSIOP_WRITE;
        sptdwb_ex.sptd.Cdb[5]					= 0; // Starting LBA
        sptdwb_ex.sptd.Cdb[8]					= 1; // TRANSFER LENGTH

        length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX);
        iResult = iIssueDeviceIoControl(
            _hDevice,
            IOCTL_SCSI_PASS_THROUGH_DIRECT_EX,
            &sptdwb_ex,
            sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX),
            &sptdwb_ex,
            length,
            &returned,
            FALSE);

        PrintStatusResultsEx(iResult, returned, (PSCSI_PASS_THROUGH_WITH_BUFFERS_EX)&sptdwb_ex, sptdwb_ex.sptd.DataOutTransferLength);
        free(pUnAlignedBuffer);
    }
    else
    {
        UCHAR databuffer[512];
        ZeroMemory(databuffer, 512);

        for (int i = 0; i < 512 / 8; i++)
        {
            databuffer[i * 8] = 'D';
            databuffer[i * 8 + 1] = 'E';
            databuffer[i * 8 + 2] = 'A';
            databuffer[i * 8 + 3] = 'D';
            databuffer[i * 8 + 4] = 'B';
            databuffer[i * 8 + 5] = 'E';
            databuffer[i * 8 + 6] = 'E';
            databuffer[i * 8 + 7] = 'F';
        }

        ZeroMemory(&sptdwb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));
        sptdwb.sptd.Length				= sizeof(SCSI_PASS_THROUGH_DIRECT);
        sptdwb.sptd.PathId				= 0;
        sptdwb.sptd.TargetId			= 0;
        sptdwb.sptd.Lun					= 0;
        sptdwb.sptd.CdbLength			= CDB10GENERIC_LENGTH;
        sptdwb.sptd.SenseInfoLength		= SPT_SENSE_LENGTH;
        sptdwb.sptd.SenseInfoOffset		= offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
        sptdwb.sptd.DataIn				= SCSI_IOCTL_DATA_OUT;
        sptdwb.sptd.DataTransferLength	= sectorSize;
        sptdwb.sptd.TimeOutValue		= 5;
        sptdwb.sptd.DataBuffer			= databuffer;
        sptdwb.sptd.Cdb[0]				= SCSIOP_WRITE;
        sptdwb.sptd.Cdb[5]				= 0; // Starting LBA
        sptdwb.sptd.Cdb[8]				= 1; // TRANSFER LENGTH

        length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
        iResult = iIssueDeviceIoControl(
            _hDevice,
            IOCTL_SCSI_PASS_THROUGH_DIRECT,
            &sptdwb,
            length,
            &sptdwb,
            length,
            &returned,
            FALSE);

        PrintStatusResults(iResult, returned, (PSCSI_PASS_THROUGH_WITH_BUFFERS)&sptdwb, length);
    }

    return iResult;
}

int iReadViaSCSIPassThrough(HANDLE _hDevice)
{
    SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;
    SCSI_PASS_THROUGH_WITH_BUFFERS_EX sptwb_ex;
    int     iResult = -1;
    ULONG length = 0, errorCode = 0, returned = 0, sectorSize = 512;
    ULONG alignmentMask = 0; // default == no alignment requirement
    UCHAR srbType = 0; // default == SRB_TYPE_SCSI_REQUEST_BLOCK

    if (TestViaSCSIPassThrough(_hDevice, &alignmentMask, &srbType) == false) return false;

    if ( srbType )
    {
        ZeroMemory(&sptwb_ex, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS_EX));
        sptwb_ex.spt.Version = 0;
        sptwb_ex.spt.Length = sizeof(SCSI_PASS_THROUGH_EX);
        sptwb_ex.spt.ScsiStatus = 0;
        sptwb_ex.spt.CdbLength = CDB10GENERIC_LENGTH;
        sptwb_ex.spt.StorAddressLength = sizeof(STOR_ADDR_BTL8);
        sptwb_ex.spt.SenseInfoLength = SPT_SENSE_LENGTH;
        sptwb_ex.spt.DataOutTransferLength = 0;
        sptwb_ex.spt.DataInTransferLength = 512;
        sptwb_ex.spt.DataDirection = SCSI_IOCTL_DATA_IN;
        sptwb_ex.spt.TimeOutValue = 2;
        sptwb_ex.spt.StorAddressOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS_EX, StorAddress);
        sptwb_ex.StorAddress.Type = STOR_ADDRESS_TYPE_BTL8;
        sptwb_ex.StorAddress.Port = 0;
        sptwb_ex.StorAddress.AddressLength = STOR_ADDR_BTL8_ADDRESS_LENGTH;
        sptwb_ex.StorAddress.Path = 0;
        sptwb_ex.StorAddress.Target = 0;
        sptwb_ex.StorAddress.Lun = 0;
        sptwb_ex.spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS_EX, ucSenseBuf);
        sptwb_ex.spt.DataOutBufferOffset = 0;
        sptwb_ex.spt.DataInBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS_EX, ucDataBuf);
        sptwb_ex.spt.Cdb[0] = SCSIOP_READ;
        sptwb_ex.spt.Cdb[5] = 0; // Starting LBA
        sptwb_ex.spt.Cdb[8] = 1; // TRANSFER LENGTH
        length = sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS_EX);

        iResult = iIssueDeviceIoControl(
            _hDevice,
            IOCTL_SCSI_PASS_THROUGH_EX,
            &sptwb_ex,
            length,
            &sptwb_ex,
            length,
            &returned,
            FALSE);

        PrintStatusResultsEx(iResult, returned, &sptwb_ex, sptwb_ex.spt.DataInTransferLength);
    }
    else
    {
        ZeroMemory(&sptwb, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));
        sptwb.spt.Length = sizeof(SCSI_PASS_THROUGH);
        sptwb.spt.PathId = 0;
        sptwb.spt.TargetId = 0;
        sptwb.spt.Lun = 0;
        sptwb.spt.CdbLength = CDB10GENERIC_LENGTH;
        sptwb.spt.SenseInfoLength = SPT_SENSE_LENGTH;
        sptwb.spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucSenseBuf);
        sptwb.spt.DataIn = SCSI_IOCTL_DATA_IN;
        sptwb.spt.DataTransferLength = 512;
        sptwb.spt.TimeOutValue = 5;
        sptwb.spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf);
        sptwb.spt.Cdb[0] = SCSIOP_READ;
        sptwb.spt.Cdb[5] = 0; // Starting LBA
        sptwb.spt.Cdb[8] = 1; // TRANSFER LENGTH

        length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf) + sptwb.spt.DataTransferLength;
        iResult = iIssueDeviceIoControl(
            _hDevice,
            IOCTL_SCSI_PASS_THROUGH,
            &sptwb,
            sizeof(SCSI_PASS_THROUGH),
            &sptwb,
            length,
            &returned,
            FALSE);

        PrintStatusResults(iResult, returned, &sptwb, sptwb.spt.DataTransferLength);
    }
    return iResult;
}

int iFlushViaSCSIPassThrough(HANDLE _hDevice)
{
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX sptdwb_ex;
    int iResult = -1;
    ULONG length = 0, errorCode = 0, returned = 0;

    ULONG alignmentMask = 0; // default == no alignment requirement
    UCHAR srbType = 0; // default == SRB_TYPE_SCSI_REQUEST_BLOCK

    if (TestViaSCSIPassThrough(_hDevice, &alignmentMask, &srbType) == false) return false;

    if (srbType)
    {
        ZeroMemory(&sptdwb_ex, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX));

        sptdwb_ex.sptd.Version                  = 0;
        sptdwb_ex.sptd.Length                   = sizeof(SCSI_PASS_THROUGH_DIRECT_EX);
        sptdwb_ex.sptd.ScsiStatus               = 0;
        sptdwb_ex.sptd.CdbLength                = CDB10GENERIC_LENGTH;
        sptdwb_ex.sptd.StorAddressLength        = sizeof(STOR_ADDR_BTL8);
        sptdwb_ex.sptd.SenseInfoLength          = SPT_SENSE_LENGTH;
        sptdwb_ex.sptd.DataOutBuffer            = 0;
        sptdwb_ex.sptd.DataOutTransferLength    = 0;
        sptdwb_ex.sptd.DataInTransferLength     = 0;
        sptdwb_ex.sptd.DataDirection            = SCSI_IOCTL_DATA_UNSPECIFIED; // no data-in, no data-out
        sptdwb_ex.sptd.TimeOutValue             = 5;
        sptdwb_ex.sptd.StorAddressOffset        = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX, StorAddress);
        sptdwb_ex.StorAddress.Type              = STOR_ADDRESS_TYPE_BTL8;
        sptdwb_ex.StorAddress.Port              = 0;
        sptdwb_ex.StorAddress.AddressLength     = STOR_ADDR_BTL8_ADDRESS_LENGTH;
        sptdwb_ex.StorAddress.Path              = 0;
        sptdwb_ex.StorAddress.Target            = 0;
        sptdwb_ex.StorAddress.Lun               = 0;
        sptdwb_ex.sptd.SenseInfoOffset          = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX, ucSenseBuf);

        sptdwb_ex.sptd.Cdb[0] = SCSIOP_SYNCHRONIZE_CACHE;
        sptdwb_ex.sptd.Cdb[1] = 0x6;    // SYNC_NV (bit 2) and IMMED (bit 1) are set
        sptdwb_ex.sptd.Cdb[5] = 0;      // logical block address
        sptdwb_ex.sptd.Cdb[8] = 1;      // number of logical blocks

        length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX);
        iResult = iIssueDeviceIoControl(
            _hDevice,
            IOCTL_SCSI_PASS_THROUGH_DIRECT_EX,
            &sptdwb_ex,
            sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER_EX),
            &sptdwb_ex,
            length,
            &returned,
            FALSE);

        PrintStatusResultsEx(iResult, returned, (PSCSI_PASS_THROUGH_WITH_BUFFERS_EX)&sptdwb_ex, sptdwb_ex.sptd.DataOutTransferLength);
    }
    else
    {
        ZeroMemory(&sptdwb, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

        sptdwb.sptd.Length              = sizeof(SCSI_PASS_THROUGH_DIRECT);
        sptdwb.sptd.PathId              = 0;
        sptdwb.sptd.TargetId            = 0;
        sptdwb.sptd.Lun	                = 0;
        sptdwb.sptd.CdbLength           = CDB10GENERIC_LENGTH;
        sptdwb.sptd.SenseInfoLength     = SPT_SENSE_LENGTH;
        sptdwb.sptd.SenseInfoOffset     = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
        sptdwb.sptd.DataIn              = SCSI_IOCTL_DATA_UNSPECIFIED; // no data-in, no data-out
        sptdwb.sptd.DataTransferLength  = 0;
        sptdwb.sptd.TimeOutValue        = 5;
        sptdwb.sptd.DataBuffer          = 0;

        sptdwb_ex.sptd.Cdb[0] = SCSIOP_SYNCHRONIZE_CACHE;
        sptdwb_ex.sptd.Cdb[1] = 0x6;    // SYNC_NV (bit 2) and IMMED (bit 1) are set
        sptdwb_ex.sptd.Cdb[5] = 0;      // logical block address
        sptdwb_ex.sptd.Cdb[8] = 1;      // number of logical blocks

        length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
        iResult = iIssueDeviceIoControl(
            _hDevice,
            IOCTL_SCSI_PASS_THROUGH_DIRECT,
            &sptdwb,
            length,
            &sptdwb,
            length,
            &returned,
            FALSE);

        PrintStatusResults(iResult, returned, (PSCSI_PASS_THROUGH_WITH_BUFFERS)&sptdwb, length);
    }

    return iResult;
}
