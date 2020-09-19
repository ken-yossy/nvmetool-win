#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <nvme.h>

#include "WinFunc.h"
#include "NVMeIdentifyController.h"

typedef struct
{
    uint64_t    NSZE;                   // byte [   7:   0] M - Namespace Size (NSZE)
    uint64_t    NCAP;                   // byte [  15:   8] M - Namespace Capacity (NCAP)
    uint64_t    NUSE;                   // byte [  23:  16] M - Namespace Utilization (NUSE)

    struct {
        uint8_t ThinProvisioning : 1;           // bit [    0]
        uint8_t NameSpaceAtomicWriteUnit : 1;   // bit [    1]
        uint8_t DeallocatedOrUnwrittenError : 1;// bit [    2]
        uint8_t SkipReuseUI : 1;                // bit [    3]
        uint8_t Reserved : 4;
    } NSFEAT;                           // byte [       24] M - Namespace Features (NSFEAT)

    uint8_t   NLBAF;                    // byte [       25] M - Number of LBA Formats (NLBAF)

    struct {
        uint8_t   LbaFormatIndex : 4;           // bit [ 3: 0]
        uint8_t   MetadataInExtendedDataLBA : 1;// bit [    4]
        uint8_t   Reserved : 3;                 // bit [ 7: 5]
    } FLBAS;                            // byte [       26] M - Formatted LBA Size (FLBAS)

    struct {
        uint8_t   MetadataInExtendedDataLBA : 1;// bit [    0]
        uint8_t   MetadataInSeparateBuffer : 1; // bit [    1]
        uint8_t   Reserved : 6;                 // bit [ 7: 2]
    } MC;                               // byte [       27] M - Metadata Capabilities (MC)

    struct {
        uint8_t   ProtectionInfoType1 : 1;      // bit [    0]
        uint8_t   ProtectionInfoType2 : 1;      // bit [    1]
        uint8_t   ProtectionInfoType3 : 1;      // bit [    2]
        uint8_t   InfoAtBeginningOfMetadata : 1;// bit [    3]
        uint8_t   InfoAtEndOfMetadata : 1;      // bit [    4]
        uint8_t   Reserved : 3;                 // bit [ 7: 5]
    } DPC;                              // byte [       28] M - End-to-end Data Protection Capabilities (DPC)

    struct {
        uint8_t   ProtectionInfoTypeEnabled : 3;// bit [ 2: 0] 0 - not enabled; 1 ~ 3: enabled type; 4 ~ 7: reserved
        uint8_t   InfoAtBeginningOfMetadata : 1;// bit [    3]
        uint8_t   Reserved : 4;                 // bit [ 7: 4]
    } DPS;                              // byte [       29] M - End-to-end Data Protection Type Settings (DPS)

    struct {
        uint8_t   SharedNameSpace : 1;  // bit [    0]
        uint8_t   Reserved : 7;         // bit [ 7: 1]
    } NMIC;                             // byte [       30] O - Namespace Multi-path I/O and Namespace Sharing Capabilities (NMIC)

    union {
        struct {
            uint8_t   PersistThroughPowerLoss : 1;                  // bit [    0]
            uint8_t   WriteExclusiveReservation : 1;                // bit [    1]
            uint8_t   ExclusiveAccessReservation : 1;               // bit [    2]
            uint8_t   WriteExclusiveRegistrantsOnlyReservation : 1; // bit [    3]
            uint8_t   ExclusiveAccessRegistrantsOnlyReservation : 1;// bit [    4]
            uint8_t   WriteExclusiveAllRegistrantsReservation : 1;  // bit [    5]
            uint8_t   ExclusiveAccessAllRegistrantsReservation : 1; // bit [    6]
            uint8_t   IgnoreExistingKey : 1;                        // bit [    7]
        } DUMMYSTRUCTNAME;

        UCHAR AsUchar;
    } RESCAP;                           // byte [       31] O - Reservation Capabilities (RESCAP)

    struct {
        uint8_t   PercentageRemained : 7;   // bit [ 6: 0]
        uint8_t   Supported : 1;            // bit [    7]
    } FPI;                              // byte [       32] O - Format Progress Indicator (FPI)

    struct {
        uint8_t   ReadBehavior : 3;         // bit [ 2: 0]
        uint8_t   DeallocateInWriteZero : 1;// bit [    3]
        uint8_t   IsGuardFieldCRC : 1;      // bit [    4]
        uint8_t   Reserved : 3;             // bit [ 7: 5]
    } DLFEAT;                           // byte [       33] O - Deallocate Logical Block Features (DLFEAT)

    uint16_t    NAWUN;                  // byte [  35:  34] O - Namespace Atomic Write Unit Normal (NAWUN)
    uint16_t    NAWUPF;                 // byte [  37:  36] O - Namespace Atomic Write Unit Power Fail (NAWUPF)
    uint16_t    NACWU;                  // byte [  39:  38] O - Namespace Atomic Compare & Write Unit (NACWU)
    uint16_t    NABSN;                  // byte [  41:  40] O - Namespace Atomic Boundary Size Normal (NABSN)
    uint16_t    NABO;                   // byte [  43:  42] O - Namespace Atomic Boundary Offset (NABO)
    uint16_t    NABSPF;                 // byte [  45:  44] O - Namespace Atomic Boundary Size Power Fail (NABSPF)
    uint16_t    NOIOB;                  // byte [  47:  46] O - Namespace Optimal IO Boundary (NOIOB)
    uint8_t     NVMCAP[16];             // byte [  63:  48] O - NVM Capacity (NVMCAP)

    uint8_t     Reserved2[40];          // byte [ 103:  64]

    uint8_t     NGUID[16];              // byte [ 119: 104] O - NAmespace Globally Unique Identifier (NGUID)
    uint8_t     EUI64[8];               // byte [ 127: 120] M - IEEE Extended Unique Identifier (EUI64)

    NVME_LBA_FORMAT LBAF[16];           // byte [ 131: 128] M - LBA Format 0 Support (LBAF0)
                                        // byte [ 192: 132] O - LBA Format Support; 1 (LBAF1) to 15 (LBAF15)

    uint8_t     Reserved3[192];         // byte [ 383: 192]

    uint8_t     VS[3712];               // byte [4095: 384] O - Vendor Specific (VS)
} NVME_IDENTIFY_NAMESPACE_DATA13, * PNVME_IDENTIFY_NAMESPACE_DATA13;

static void printNVMeIdentifyNamespaceData(PNVME_IDENTIFY_NAMESPACE_DATA13 _pNSData, DWORD _dwNSID)
{
    printf("[M] Namespace Size (NSZE): %llu (sectors)\n", (uint64_t)_pNSData->NSZE);
    printf("[M] Namespace Capacity (NCAP): %llu (sectors)\n", (uint64_t)_pNSData->NCAP);
    printf("[M] Namespace Utilization (NUSE): %llu (sectors)\n", (uint64_t)_pNSData->NUSE);

    printf("[M] Namespace Features (NSFEAT):\n");
    if (_pNSData->NSFEAT.SkipReuseUI)
    {
        printf("\tbit [      3] 1 = NGUID and EUI64 is never reused.\n");
    }
    else
    {
        printf("\tbit [      3] 0 = NGUID and EUI64 may be reused.\n");
    }

    if (_pNSData->NSFEAT.DeallocatedOrUnwrittenError)
    {
        printf("\tbit [      2] 1 = Supports the Deallocated or Unwritten Logical Block error.\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not support the Deallocated or Unwritten Logical Block error.\n");
    }

    if (_pNSData->NSFEAT.NameSpaceAtomicWriteUnit)
    {
        printf("\tbit [      1] 1 = Use NAWUN, NAWUPF, and NACWU.\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Use AWUN, AWUPF, and ACWU.\n");
    }

    if (_pNSData->NSFEAT.ThinProvisioning)
    {
        printf("\tbit [      0] 1 = Supports thin provisioning.\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support thin provisioning.\n");
    }

    printf("[M] Number of LBA Formats (NLBAF): %d = Supports %d format(s).\n", _pNSData->NLBAF, _pNSData->NLBAF + 1);

    printf("[M] Formatted LBA Size (FLBAS):\n");
    if (_pNSData->FLBAS.MetadataInExtendedDataLBA)
    {
        printf("\tbit [      4] 1 = The metadata is transferred at the end of the data LBA, creating an extended data LBA.\n");
    }
    else
    {
        printf("\tbit [      4] 0 = All of the metadata is transferred as a separate contiguous buffer of data.\n");
    }
    printf("\tbit [  3:  0] %d = LBA format is no.%d (LBAF%d).\n", _pNSData->FLBAS.LbaFormatIndex, _pNSData->FLBAS.LbaFormatIndex, _pNSData->FLBAS.LbaFormatIndex);

    printf("[M] Metadata Capabilities (MC):\n");
    if (_pNSData->MC.MetadataInSeparateBuffer)
    {
        printf("\tbit [      1] 1 = Supports the metadata being transferred as part of a separate buffer.\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Does not support the metadata being transferred as part of a separate buffer.\n");
    }
    if (_pNSData->MC.MetadataInExtendedDataLBA)
    {
        printf("\tbit [      0] 1 = Supports the metadata being transferred as part of an extended data LBA.\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support the metadata being transferred as part of an extended data LBA.\n");
    }

    printf("[M] End-to-end Data Protection Capabilities (DPC):\n");
    if (_pNSData->DPC.InfoAtEndOfMetadata)
    {
        printf("\tbit [      4] 1 = Supports protection information transferred as the last eight bytes of metadata.\n");
    }
    else
    {
        printf("\tbit [      4] 0 = Does not support protection information transferred as the last eight bytes of metadata.\n");
    }
    if (_pNSData->DPC.InfoAtBeginningOfMetadata)
    {
        printf("\tbit [      3] 1 = Supports protection information transferred as the first eight bytes of metadata.\n");
    }
    else
    {
        printf("\tbit [      3] 0 = Does not support protection information transferred as the first eight bytes of metadata.\n");
    }
    if (_pNSData->DPC.ProtectionInfoType3)
    {
        printf("\tbit [      2] 1 = Supports Protection Information Type 3.\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not support Protection Information Type 3.\n");
    }
    if (_pNSData->DPC.ProtectionInfoType2)
    {
        printf("\tbit [      1] 1 = Supports Protection Information Type 2.\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Does not support Protection Information Type 2.\n");
    }
    if (_pNSData->DPC.ProtectionInfoType1)
    {
        printf("\tbit [      0] 1 = Supports Protection Information Type 1.\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Protection Information Type 1.\n");
    }

    printf("[M] End-to-end Data Protection Settings (DPS):\n");
    if (_pNSData->DPS.InfoAtBeginningOfMetadata)
    {
        printf("\tbit [      3] 1 = The protection information, if enabled, is transferred as the first eight bytes of metadata.\n");
    }
    else
    {
        printf("\tbit [      3] 0 = The protection information, if enabled, is transferred as the last eight bytes of metadata.\n");
    }
    if (_pNSData->DPS.ProtectionInfoTypeEnabled == 0)
    {
        printf("\tbit [  2:  0] 0 = Protection information is not enabled.\n");
    }
    else if (_pNSData->DPS.ProtectionInfoTypeEnabled == 1)
    {
        printf("\tbit [  2:  0] 1 = Protection information is enabled, Type 1.\n");
    }
    else if (_pNSData->DPS.ProtectionInfoTypeEnabled == 2)
    {
        printf("\tbit [  2:  0] 2 = Protection information is enabled, Type 2.\n");
    }
    else // if (_pNSData->DPS.ProtectionInfoTypeEnabled == 3)
    {
        printf("\tbit [  2:  0] 3 = Protection information is enabled, Type 3.\n");
    }

    printf("[O] Namespace Multi-path I/O and Namespace Sharing Capabilities (NMIC):\n");
    if (_pNSData->NMIC.SharedNameSpace)
    {
        printf("\tbit [      0] 1 = May be attached to two or more controllers in the NVM subsystem concurrently.\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Is a private namespace and is able to be attached to only one controller at a time.\n");
    }

    printf("[O] Reservation Capabilities (RESCAP):\n");
    if (g_stController.ONCS.Reservations == 0)
    {
        if (_pNSData->RESCAP.AsUchar == 0)
        {
            printf("\tbit [  7:  0] 00h = Does not support Reservations\n");
        }
        else
        {
            printf("\tbit [  7:  0] %02Xh = (Conflict with Identify Controller data)\n", _pNSData->RESCAP.AsUchar);
        }
    }
    else
    {
        if (_pNSData->RESCAP.IgnoreExistingKey)
        {
            printf("\tbit [      7] 1 = Ignore Existing Key is used as defined in revision 1.3 or later of this specification.\n");
        }
        else
        {
            printf("\tbit [      7] 0 = Ignore Existing Key is used as defined in revision 1.2.1 or earlier of this specification.\n");
        }
        if (_pNSData->RESCAP.ExclusiveAccessAllRegistrantsReservation)
        {
            printf("\tbit [      6] 1 = Supports the Exclusive Access - All Registrants reservation type.\n");
        }
        else
        {
            printf("\tbit [      6] 0 = Does not support the Exclusive Access - All Registrants reservation type.\n");
        }
        if (_pNSData->RESCAP.WriteExclusiveAllRegistrantsReservation)
        {
            printf("\tbit [      5] 1 = Supports the Write Exclusive - All Registrants reservation type.\n");
        }
        else
        {
            printf("\tbit [      5] 0 = Does not support the Write Exclusive - All Registrants reservation type.\n");
        }
        if (_pNSData->RESCAP.ExclusiveAccessRegistrantsOnlyReservation)
        {
            printf("\tbit [      4] 1 = Supports the Exclusive Access - Registrants Only reservation type.\n");
        }
        else
        {
            printf("\tbit [      4] 0 = Does not support the Exclusive Access - Registrants Only reservation type.\n");
        }
        if (_pNSData->RESCAP.WriteExclusiveRegistrantsOnlyReservation)
        {
            printf("\tbit [      3] 1 = Supports the Write Exclusive - Registrants Only reservation type.\n");
        }
        else
        {
            printf("\tbit [      3] 0 = Does not support the Write Exclusive - Registrants Only reservation type.\n");
        }
        if (_pNSData->RESCAP.ExclusiveAccessReservation)
        {
            printf("\tbit [      2] 1 = Supports the Exclusive Access reservation type.\n");
        }
        else
        {
            printf("\tbit [      2] 0 = Does not support the Exclusive Access reservation type.\n");
        }
        if (_pNSData->RESCAP.WriteExclusiveReservation)
        {
            printf("\tbit [      1] 1 = Supports the Write Exclusive reservation type.\n");
        }
        else
        {
            printf("\tbit [      1] 0 = Does not support the Write Exclusive reservation type.\n");
        }
        if (_pNSData->RESCAP.PersistThroughPowerLoss)
        {
            printf("\tbit [      0] 1 = Supports the Persist Through Power Loss capability.\n");
        }
        else
        {
            printf("\tbit [      0] 0 = Does not support the Persist Through Power Loss capability.\n");
        }
    }

    printf("[O] Format Progress Indicator (FPI):\n");
    if (_pNSData->FPI.Supported)
    {
        printf("\tbit [      7] 1 = Supports the Format Progress Indicator defined by bits 6:0 in this field.\n");
        printf("\tbit [  6:  0] %d = the percentage of the Format NVM command that remains to be completed (%%).\n", _pNSData->FPI.PercentageRemained);
    }
    else
    {
        printf("\tbit [      7] 0 = Does not support the Format Progress Indicator.\n");
    }

    printf("[O] Deallocate Logical Block Features (DLFEAT):\n");
    if (_pNSData->DLFEAT.IsGuardFieldCRC)
    {
        printf("\tbit [      4] 1 = The Guard field for deallocated logical blocks that contain protection information is set to the CRC for the value read from the deallocated logical block and its metadata (excluding protection information).\n");
    }
    else
    {
        printf("\tbit [      4] 0 = The Guard field for the deallocated logical blocks that contain protection information is set to FFFFh.\n");
    }
    if (_pNSData->DLFEAT.DeallocateInWriteZero)
    {
        printf("\tbit [      3] 1 = Supports the Deallocate bit in the Write Zeroes command for this namespace.\n");
    }
    else
    {
        printf("\tbit [      3] 0 = Does not support the Deallocate bit in the Write Zeroes command for this namespace.\n");
    }
    switch (_pNSData->DLFEAT.ReadBehavior)
    {
    case 0:
        printf("\tbit [  2:  0] 0 = The read behavior is not reported.\n");
        break;

    case 1:
        printf("\tbit [  2:  0] 1 = A deallocated logical block returns all bytes cleared to 0h\n");
        break;

    case 2:
        printf("\tbit [  2:  0] 2 = A deallocated logical block returns all bytes set to FFh.\n");
        break;

    default:
        break;
    }

    printf("[O] Namespace Atomic Write Unit Normal (NAWUN): ");
    if (_pNSData->NAWUN)
    {
        printf("%d (means %d sector(s))\n", _pNSData->NAWUN, _pNSData->NAWUN + 1);
    }
    else
    {
        printf("0 (The same size as that reported in the AWUN field)\n");
    }

    printf("[O] Namespace Atomic Write Unit Power Fail (NAWUPF): ");
    if (_pNSData->NAWUPF)
    {
        printf("%d (means %d sector(s))\n", _pNSData->NAWUPF, _pNSData->NAWUPF + 1);
    }
    else
    {
        printf("0 (The same size as that reported in the AWUPF field)\n");
    }

    printf("[O] Namespace Atomic Compare & Write Unit (NACWU): ");
    if (_pNSData->NACWU)
    {
        printf("%d (means %d sector(s))\n", _pNSData->NACWU, _pNSData->NACWU + 1);
    }
    else
    {
        printf("0 (The same size as that reported in the ACWU field)\n");
    }

    printf("[O] Namespace Atomic Boundary Size Normal (NABSN): ");
    if (_pNSData->NABSN)
    {
        printf("%d (means %d sector(s))\n", _pNSData->NABSN, _pNSData->NABSN + 1);
    }
    else
    {
        printf("0 (There are no atomic boundaries for normal write operations.)\n");
    }

    printf("[O] Namespace Atomic Boundary Offset (NABO): %d\n", _pNSData->NABO);

    printf("[O] Namespace Atomic Boundary Size Power Fail (NABSPF): ");
    if (_pNSData->NABSPF)
    {
        printf("%d (means %d sector(s))\n", _pNSData->NABSPF, _pNSData->NABSPF + 1);
    }
    else
    {
        printf("0 (There are no atomic boundaries for power fail or error conditions.)\n");
    }

    printf("[O] Namespace Optimal IO Boundary (NOIOB): ");
    if (_pNSData->NOIOB)
    {
        printf("%d (means %d sector(s))\n", _pNSData->NOIOB, _pNSData->NOIOB + 1);
    }
    else
    {
        printf("0 (No optimal IO boundary is reported.)\n");
    }

    // TODO: we ignore upper 8 bytes of _pNSData->NVMCAP[8] ... when it becomes to be needed?
    printf("[O] NVM Capacity (NVMCAP): %lld (bytes)\n", (uint64_t)(_pNSData->NVMCAP));

    printf("[O] Namespace Globally Unique Identifier (NGUID):\n");
    printf("\tbyte [ 15: 11] %02X%02X%02X%02X%02X (Extension identifer assigned by the vendor)\n",
        _pNSData->NGUID[11], _pNSData->NGUID[12], _pNSData->NGUID[13], _pNSData->NGUID[14], _pNSData->NGUID[15]); // Bytes [119:115] (5 byte, 40 bit = Extension identifer assigned by the organization
    printf("\tbyte [ 10:  8] %02X%02X%02X (Organizationally Unique Identifier (OUI))\n",
        _pNSData->NGUID[8], _pNSData->NGUID[9], _pNSData->NGUID[10]); // Bytes [114:112] (3 byte, 24 bit = Organizationally Unique Identifier (OUI) value assigned by the IEEE Registration Authority
    printf("\tbyte [  7:  0] %02X%02X%02X%02X%02X%02X%02X%02X (Vendor specific extension identifier)\n",
        _pNSData->NGUID[0], _pNSData->NGUID[1], _pNSData->NGUID[2], _pNSData->NGUID[3], _pNSData->NGUID[4], _pNSData->NGUID[5], _pNSData->NGUID[6], _pNSData->NGUID[7]); // Bytes [111:104] (8 byte, 64 bit = Vendor specific extension identifier

    printf("[O] IEEE Extended Unique Identifier (EUI64):\n");
    printf("\tbyte [  7:  3] %02X%02X%02X%02X%02X (Extension identifer assigned by the vendor)\n",
        _pNSData->EUI64[3], _pNSData->EUI64[4], _pNSData->EUI64[5], _pNSData->EUI64[6], _pNSData->EUI64[7]); // Bytes [127:123] (5 byte, 40 bit = Extension identifer assigned by the organization
    printf("\tbyte [  2:  0] %02X%02X%02X (Organizationally Unique Identifier (OUI))\n",
        _pNSData->EUI64[0], _pNSData->EUI64[1], _pNSData->EUI64[2]); // Bytes [122:120] (3 byte, 24 bit = Organizationally Unique Identifier (OUI) value assigned by the IEEE Registration Authority

    for (int i = 0; i < ( _pNSData->NLBAF + 1 ); i++)
    {
        if (i == 0)
        {
            printf("[M] LBA Format 0 Support (LBAF0):\n");
        }
        else
        {
            printf("[O] LBA Format %d Support (LBAF%d):\n", i, i);
        }

        switch (_pNSData->LBAF[i].RP)
        {
        case 0:
            printf("\tbit [ 25: 24] 0 = Relative Performance (RP): Best performance\n");
            break;

        case 1:
            printf("\tbit [ 25: 24] 1 = Relative Performance (RP): Better performance\n");
            break;

        case 2:
            printf("\tbit [ 25: 24] 2 = Relative Performance (RP): Good performance\n");
            break;

        case 3:
            printf("\tbit [ 25: 24] 3 = Relative Performance (RP): Degraded performance\n");
            break;

        default:
            break; // not reach
        }
        printf("\tbit [ 23: 16] %d = LBA Data Size (LBADS): %d bytes / sector\n", _pNSData->LBAF[i].LBADS, 1 << (_pNSData->LBAF[i].LBADS));
        printf("\tbit [ 15:  0] %d = Metadata Size (MS): %d bytes\n", _pNSData->LBAF[i].MS, _pNSData->LBAF[i].MS);
    }
}

int iNVMeIdentifyNamespace(HANDLE _hDevice, DWORD _dwNSID)
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
        + sizeof(NVME_IDENTIFY_NAMESPACE_DATA13);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError( GetLastError(), "malloc" );
        goto error_exit;
    }

    // Initialize query data structure to get Identify Active Namespace ID list.
    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeIdentify;
    protocolData->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_SPECIFIC_NAMESPACE;
    protocolData->ProtocolDataRequestSubValue = _dwNSID;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(NVME_IDENTIFY_NAMESPACE_DATA13);

    // Send request down.
    iResult = iIssueDeviceIoControl(
        _hDevice,
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

    //
    // Validate the returned data.
    //
    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)))
    {
        fprintf(stderr, "[E] NVMeIdentifyNamespace: data descriptor header not valid, stop.\n");
        goto error_exit;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset > sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < sizeof(NVME_IDENTIFY_NAMESPACE_DATA13)))
    {
        fprintf(stderr, "[E] NVMeIdentifyNamespace: ProtocolData Offset/Length not valid, stop.\n");
        goto error_exit;
    }

    // Identify Namespace
    printNVMeIdentifyNamespaceData(
        (PNVME_IDENTIFY_NAMESPACE_DATA13)((PCHAR)protocolData + protocolData->ProtocolDataOffset),
        _dwNSID);
    iResult = 0; // succeeded

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

    return iResult;
}
