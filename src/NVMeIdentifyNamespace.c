#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "NVMeIdentifyController.h"
#include "WinFunc.h"

typedef struct {
    uint64_t NSZE;  // byte [   7:   0] M - Namespace Size (NSZE)
    uint64_t NCAP;  // byte [  15:   8] M - Namespace Capacity (NCAP)
    uint64_t NUSE;  // byte [  23:  16] M - Namespace Utilization (NUSE)

    struct {
        uint8_t ThinProvisioning : 1;             // bit [    0]
        uint8_t NameSpaceAtomicWriteUnit : 1;     // bit [    1]
        uint8_t DeallocatedOrUnwrittenError : 1;  // bit [    2]
        uint8_t SkipReuseUI : 1;                  // bit [    3]
        uint8_t OptPerf : 1;                      // bit [    4] <rev1.4>
        uint8_t Reserved : 3;
    } NSFEAT;  // byte [       24] M - Namespace Features (NSFEAT)

    uint8_t NLBAF;  // byte [       25] M - Number of LBA Formats (NLBAF)

    struct {
        uint8_t LbaFormatIndex : 4;             // bit [ 3: 0]
        uint8_t MetadataInExtendedDataLBA : 1;  // bit [    4]
        uint8_t NlbafMsb : 2;                   // bit [ 6: 5] <rev2.0>
        uint8_t Reserved : 1;                   // bit [    7]
    } FLBAS;  // byte [       26] M - Formatted LBA Size (FLBAS)

    struct {
        uint8_t MetadataInExtendedDataLBA : 1;  // bit [    0]
        uint8_t MetadataInSeparateBuffer : 1;   // bit [    1]
        uint8_t Reserved : 6;                   // bit [ 7: 2]
    } MC;  // byte [       27] M - Metadata Capabilities (MC)

    struct {
        uint8_t ProtectionInfoType1 : 1;        // bit [    0]
        uint8_t ProtectionInfoType2 : 1;        // bit [    1]
        uint8_t ProtectionInfoType3 : 1;        // bit [    2]
        uint8_t InfoAtBeginningOfMetadata : 1;  // bit [    3]
        uint8_t InfoAtEndOfMetadata : 1;        // bit [    4]
        uint8_t Reserved : 3;                   // bit [ 7: 5]
    } DPC;  // byte [       28] M - End-to-end Data Protection Capabilities
            // (DPC)

    struct {
        uint8_t
            ProtectionInfoTypeEnabled : 3;  // bit [ 2: 0] 0 - not enabled; 1 ~
                                            // 3: enabled type; 4 ~ 7: reserved
        uint8_t InfoAtBeginningOfMetadata : 1;  // bit [    3]
        uint8_t Reserved : 4;                   // bit [ 7: 4]
    } DPS;  // byte [       29] M - End-to-end Data Protection Type Settings
            // (DPS)

    struct {
        uint8_t SharedNameSpace : 1;  // bit [    0]
        uint8_t Reserved : 7;         // bit [ 7: 1]
    } NMIC;  // byte [       30] O - Namespace Multi-path I/O and Namespace
             // Sharing Capabilities (NMIC)

    union {
        struct {
            uint8_t PersistThroughPowerLoss : 1;     // bit [    0]
            uint8_t WriteExclusiveReservation : 1;   // bit [    1]
            uint8_t ExclusiveAccessReservation : 1;  // bit [    2]
            uint8_t WriteExclusiveRegistrantsOnlyReservation : 1;   // bit [ 3]
            uint8_t ExclusiveAccessRegistrantsOnlyReservation : 1;  // bit [ 4]
            uint8_t WriteExclusiveAllRegistrantsReservation : 1;  // bit [    5]
            uint8_t ExclusiveAccessAllRegistrantsReservation : 1;  // bit [ 6]
            uint8_t IgnoreExistingKey : 1;  // bit [    7]
        } DUMMYSTRUCTNAME;

        UCHAR AsUchar;
    } RESCAP;  // byte [       31] O - Reservation Capabilities (RESCAP)

    struct {
        uint8_t PercentageRemained : 7;  // bit [ 6: 0]
        uint8_t Supported : 1;           // bit [    7]
    } FPI;  // byte [       32] O - Format Progress Indicator (FPI)

    struct {
        uint8_t ReadBehavior : 3;           // bit [ 2: 0]
        uint8_t DeallocateInWriteZero : 1;  // bit [    3]
        uint8_t IsGuardFieldCRC : 1;        // bit [    4]
        uint8_t Reserved : 3;               // bit [ 7: 5]
    } DLFEAT;  // byte [       33] O - Deallocate Logical Block Features
               // (DLFEAT)

    uint16_t NAWUN;   // byte [  35:  34] O - Namespace Atomic Write Unit Normal
                      // (NAWUN)
    uint16_t NAWUPF;  // byte [  37:  36] O - Namespace Atomic Write Unit Power
                      // Fail (NAWUPF)
    uint16_t NACWU;   // byte [  39:  38] O - Namespace Atomic Compare & Write
                      // Unit (NACWU)
    uint16_t NABSN;   // byte [  41:  40] O - Namespace Atomic Boundary Size
                      // Normal (NABSN)
    uint16_t
        NABO;  // byte [  43:  42] O - Namespace Atomic Boundary Offset (NABO)
    uint16_t NABSPF;  // byte [  45:  44] O - Namespace Atomic Boundary Size
                      // Power Fail (NABSPF)
    uint16_t
        NOIOB;  // byte [  47:  46] O - Namespace Optimal IO Boundary (NOIOB)
    uint64_t NVMCAP_L;  // byte [  63:  48] O - NVM Capacity (NVMCAP)
    uint64_t NVMCAP_H;
    uint16_t NPWG;   // byte [  65:  64] O - Namespace Preferred Write
                     // Granularity (NPWG) <rev1.4>
    uint16_t NPWA;   // byte [  67:  66] O - Namespace Preferred Write Alignment
                     // (NPWA) <rev1.4>
    uint16_t NPDG;   // byte [  69:  68] O - Namespace Preferred Deallocate
                     // Granularity (NPDG) <rev1.4>
    uint16_t NPDA;   // byte [  71:  70] O - Namespace Preferred Deallocate
                     // Alignment (NPDA) <rev1.4>
    uint16_t NOWS;   // byte [  73:  72] O - Namespace Optimal Write Size (NOWS)
                     // <rev1.4>
    uint16_t MSSRL;  // byte [  75:  74] O - Maximum Single Source Range Length
                     // (MSSRL) <rev2.0>
    uint32_t MCL;    // byte [  79:  76] O - Maximum Copy Length (MCL) <rev2.0>
    uint8_t MSRC;    // byte [       80] O - Maximum Source Range Count (MSRC)
                     // >rev2.0>

    uint8_t Reserved2[11];  // byte [  91:  81]

    uint32_t ANAGRPID;  // byte [  95:  92] O - ANA Group Identifier (ANAGRPID)
                        // <rev1.4>

    uint8_t Reserved3[3];  // byte [  98:  96]

    struct {
        uint8_t WriteProtect : 1;  // bit [    0]
        uint8_t Reserved : 7;      // bit [ 7: 1]
    } NSATTR;  // byte [       99] O - Namespace Attributes (NSATTR) <rev1.4>

    uint16_t NVMSETID;  // byte [ 101: 100] O - NVM Set Identifier (NVMSETID)
                        // <rev1.4>
    uint16_t ENDGID;    // byte [ 103: 102] O - Endurance Group Identifier
                        // (ENDGID) <rev1.4>

    uint8_t NGUID[16];  // byte [ 119: 104] O - NAmespace Globally Unique
                        // Identifier (NGUID)
    uint8_t EUI64[8];   // byte [ 127: 120] M - IEEE Extended Unique Identifier
                        // (EUI64)

    NVME_LBA_FORMAT LBAF[64];  // byte [ 131: 128] M - LBA Format 0 Support
                               // (LBAF0) byte [ 383: 132] O - LBA Format
                               // Support; 1 (LBAF1) to 63 (LBAF63)

    uint8_t VS[3712];  // byte [4095: 384] O - Vendor Specific (VS)
} MY_NVME_IDENTIFY_NAMESPACE_DATA, *PMY_NVME_IDENTIFY_NAMESPACE_DATA;

static MY_NVME_IDENTIFY_NAMESPACE_DATA g_stNamespaceData;
static bool g_bNamespaceAlreadyIdentified = false;

bool bNamespaceSupportPI(uint32_t _ulNSID) {
    return (g_stNamespaceData.DPS.ProtectionInfoTypeEnabled == 0) ? false
                                                                  : true;
}

bool bNamespaceAlreadyIdentified(uint32_t _ulNSID) {
    return g_bNamespaceAlreadyIdentified;
}

static void printNVMeIdentifyNamespaceData(
    PMY_NVME_IDENTIFY_NAMESPACE_DATA _pNSData, DWORD _dwNSID) {
    uint32_t uiLBAF = 0;  // index of current format (index for LBAF)

    {
        if (bIsNVMeV20OrLater()) {
            uiLBAF = (_pNSData->FLBAS.NlbafMsb << 4) +
                     (_pNSData->FLBAS.LbaFormatIndex);
        } else {
            uiLBAF = _pNSData->FLBAS.LbaFormatIndex;
        }

        uint32_t uiLBADS =
            _pNSData->LBAF[uiLBAF]
                .LBADS;  // shift value for current sector size in byte

        printf("[M] Namespace Size (NSZE): %llu (sectors), about %llu (GiB)\n",
               _pNSData->NSZE, (_pNSData->NSZE) >> (30 - uiLBADS));
        printf(
            "[M] Namespace Capacity (NCAP): %llu (sectors), about %llu (GiB)\n",
            _pNSData->NCAP, (_pNSData->NCAP) >> (30 - uiLBADS));
        printf(
            "[M] Namespace Utilization (NUSE): %llu (sectors), about %llu "
            "(GiB)\n",
            _pNSData->NUSE, (_pNSData->NUSE) >> (30 - uiLBADS));
    }

    printf("[M] Namespace Features (NSFEAT):\n");
    if (bIsNVMeV14OrLater()) {
        if (_pNSData->NSFEAT.OptPerf) {
            printf(
                "\tbit [      4] 1 = Host should use NPWG, NPWA, NPDG, NPDA, "
                "and NOWS\n");
        } else {
            printf(
                "\tbit [      4] 0 = Does not support NPWG, NPWA, NPDG, NPDA, "
                "and NOWS\n");
        }
    }

    if (_pNSData->NSFEAT.SkipReuseUI) {
        printf("\tbit [      3] 1 = NGUID and EUI64 is never reused\n");
    } else {
        printf("\tbit [      3] 0 = NGUID and EUI64 may be reused\n");
    }

    if (_pNSData->NSFEAT.DeallocatedOrUnwrittenError) {
        printf(
            "\tbit [      2] 1 = Supports Deallocated or Unwritten Logical "
            "Block error\n");
    } else {
        printf(
            "\tbit [      2] 0 = Does not support Deallocated or Unwritten "
            "Logical Block error\n");
    }

    if (_pNSData->NSFEAT.NameSpaceAtomicWriteUnit) {
        printf("\tbit [      1] 1 = Use NAWUN, NAWUPF, and NACWU\n");
    } else {
        printf("\tbit [      1] 0 = Use AWUN, AWUPF, and ACWU\n");
    }

    if (_pNSData->NSFEAT.ThinProvisioning) {
        printf("\tbit [      0] 1 = Supports thin provisioning\n");
    } else {
        printf("\tbit [      0] 0 = Does not support thin provisioning\n");
    }

    printf("[M] Number of LBA Formats (NLBAF): %d = Supports %d format(s)\n",
           _pNSData->NLBAF, _pNSData->NLBAF + 1);

    {
        printf("[M] Formatted LBA Size (FLBAS):\n");
        if (bIsNVMeV20OrLater()) {
            printf(
                "\tbit [  6:  5] %d = Most significant 2 bits of LBA format\n",
                _pNSData->FLBAS.NlbafMsb);
        }

        if (_pNSData->FLBAS.MetadataInExtendedDataLBA) {
            printf(
                "\tbit [      4] 1 = The metadata is transferred at the end of "
                "the data LBA, creating an extended data LBA\n");
        } else {
            printf(
                "\tbit [      4] 0 = All of the metadata is transferred as a "
                "separate contiguous buffer of data\n");
        }

        // we have already parsed FLBAS at the beginning of this function
        if (bIsNVMeV20OrLater()) {
            printf("\tbit [  3:  0] %d = LBA format is no.%d (LBAF%d)\n",
                   _pNSData->FLBAS.LbaFormatIndex, uiLBAF, uiLBAF);
        } else {
            printf("\tbit [  3:  0] %d = LBA format is no.%d (LBAF%d)\n",
                   uiLBAF, uiLBAF, uiLBAF);
        }
    }

    printf("[M] Metadata Capabilities (MC):\n");
    if (_pNSData->MC.MetadataInSeparateBuffer) {
        printf(
            "\tbit [      1] 1 = Supports the metadata being transferred as "
            "part of a separate buffer\n");
    } else {
        printf(
            "\tbit [      1] 0 = Does not support the metadata being "
            "transferred as part of a separate buffer\n");
    }
    if (_pNSData->MC.MetadataInExtendedDataLBA) {
        printf(
            "\tbit [      0] 1 = Supports the metadata being transferred as "
            "part of an extended data LBA\n");
    } else {
        printf(
            "\tbit [      0] 0 = Does not support the metadata being "
            "transferred as part of an extended data LBA\n");
    }

    printf("[M] End-to-end Data Protection Capabilities (DPC):\n");
    if (_pNSData->DPC.InfoAtEndOfMetadata) {
        printf(
            "\tbit [      4] 1 = Supports protection information transferred "
            "as the last eight bytes of metadata\n");
    } else {
        printf(
            "\tbit [      4] 0 = Does not support protection information "
            "transferred as the last eight bytes of metadata\n");
    }
    if (_pNSData->DPC.InfoAtBeginningOfMetadata) {
        printf(
            "\tbit [      3] 1 = Supports protection information transferred "
            "as the first eight bytes of metadata\n");
    } else {
        printf(
            "\tbit [      3] 0 = Does not support protection information "
            "transferred as the first eight bytes of metadata\n");
    }
    if (_pNSData->DPC.ProtectionInfoType3) {
        printf("\tbit [      2] 1 = Supports Protection Information Type 3\n");
    } else {
        printf(
            "\tbit [      2] 0 = Does not support Protection Information Type "
            "3\n");
    }
    if (_pNSData->DPC.ProtectionInfoType2) {
        printf("\tbit [      1] 1 = Supports Protection Information Type 2\n");
    } else {
        printf(
            "\tbit [      1] 0 = Does not support Protection Information Type "
            "2\n");
    }
    if (_pNSData->DPC.ProtectionInfoType1) {
        printf("\tbit [      0] 1 = Supports Protection Information Type 1\n");
    } else {
        printf(
            "\tbit [      0] 0 = Does not support Protection Information Type "
            "1\n");
    }

    printf("[M] End-to-end Data Protection Settings (DPS):\n");
    if (_pNSData->DPS.InfoAtBeginningOfMetadata) {
        printf(
            "\tbit [      3] 1 = The protection information, if enabled, is "
            "transferred as the first eight bytes of metadata\n");
    } else {
        printf(
            "\tbit [      3] 0 = The protection information, if enabled, is "
            "transferred as the last eight bytes of metadata\n");
    }
    if (_pNSData->DPS.ProtectionInfoTypeEnabled == 0) {
        printf("\tbit [  2:  0] 0 = Protection information is not enabled\n");
    } else if (_pNSData->DPS.ProtectionInfoTypeEnabled == 1) {
        printf(
            "\tbit [  2:  0] 1 = Protection information is enabled, Type 1\n");
    } else if (_pNSData->DPS.ProtectionInfoTypeEnabled == 2) {
        printf(
            "\tbit [  2:  0] 2 = Protection information is enabled, Type 2\n");
    } else { // if (_pNSData->DPS.ProtectionInfoTypeEnabled == 3)
        printf(
            "\tbit [  2:  0] 3 = Protection information is enabled, Type 3\n");
    }

    printf(
        "[O] Namespace Multi-path I/O and Namespace Sharing Capabilities "
        "(NMIC):\n");
    if (_pNSData->NMIC.SharedNameSpace) {
        printf(
            "\tbit [      0] 1 = May be attached to two or more controllers in "
            "the NVM subsystem concurrently\n");
    } else {
        printf(
            "\tbit [      0] 0 = Is a private namespace and is able to be "
            "attached to only one controller at a time\n");
    }

    printf("[O] Reservation Capabilities (RESCAP):\n");
    if (g_stController.ONCS.Reservations == 0) {
        if (_pNSData->RESCAP.AsUchar == 0) {
            printf("\tbit [  7:  0] 00h = Does not support Reservations\n");
        } else {
            printf(
                "\tbit [  7:  0] %02Xh = (Conflict with Identify Controller "
                "data)\n",
                _pNSData->RESCAP.AsUchar);
        }
    } else {
        if (_pNSData->RESCAP.IgnoreExistingKey) {
            printf(
                "\tbit [      7] 1 = Ignore Existing Key is used as defined in "
                "revision 1.3 or later of this specification\n");
        } else {
            printf(
                "\tbit [      7] 0 = Ignore Existing Key is used as defined in "
                "revision 1.2.1 or earlier of this specification\n");
        }
        if (_pNSData->RESCAP.ExclusiveAccessAllRegistrantsReservation) {
            printf(
                "\tbit [      6] 1 = Supports Exclusive Access - All "
                "Registrants reservation type\n");
        } else {
            printf(
                "\tbit [      6] 0 = Does not support Exclusive Access - All "
                "Registrants reservation type\n");
        }
        if (_pNSData->RESCAP.WriteExclusiveAllRegistrantsReservation) {
            printf(
                "\tbit [      5] 1 = Supports Write Exclusive - All "
                "Registrants reservation type\n");
        } else {
            printf(
                "\tbit [      5] 0 = Does not support Write Exclusive - All "
                "Registrants reservation type\n");
        }
        if (_pNSData->RESCAP.ExclusiveAccessRegistrantsOnlyReservation) {
            printf(
                "\tbit [      4] 1 = Supports Exclusive Access - Registrants "
                "Only reservation type\n");
        } else {
            printf(
                "\tbit [      4] 0 = Does not support Exclusive Access - "
                "Registrants Only reservation type\n");
        }
        if (_pNSData->RESCAP.WriteExclusiveRegistrantsOnlyReservation) {
            printf(
                "\tbit [      3] 1 = Supports Write Exclusive - Registrants "
                "Only reservation type\n");
        } else {
            printf(
                "\tbit [      3] 0 = Does not support Write Exclusive - "
                "Registrants Only reservation type\n");
        }
        if (_pNSData->RESCAP.ExclusiveAccessReservation) {
            printf(
                "\tbit [      2] 1 = Supports Exclusive Access reservation "
                "type\n");
        } else {
            printf(
                "\tbit [      2] 0 = Does not support Exclusive Access "
                "reservation type\n");
        }
        if (_pNSData->RESCAP.WriteExclusiveReservation) {
            printf(
                "\tbit [      1] 1 = Supports Write Exclusive reservation "
                "type\n");
        } else {
            printf(
                "\tbit [      1] 0 = Does not support Write Exclusive "
                "reservation type\n");
        }
        if (_pNSData->RESCAP.PersistThroughPowerLoss) {
            printf(
                "\tbit [      0] 1 = Supports Persist Through Power Loss "
                "capability\n");
        } else {
            printf(
                "\tbit [      0] 0 = Does not support Persist Through Power "
                "Loss capability\n");
        }
    }

    printf("[O] Format Progress Indicator (FPI):\n");
    if (_pNSData->FPI.Supported) {
        printf(
            "\tbit [      7] 1 = Supports Format Progress Indicator defined by "
            "bits 6:0 in this field\n");
        printf(
            "\tbit [  6:  0] %d = the percentage of Format NVM command that "
            "remains to be completed (%%)\n",
            _pNSData->FPI.PercentageRemained);
    } else {
        printf(
            "\tbit [      7] 0 = Does not support Format Progress Indicator\n");
    }

    printf("[O] Deallocate Logical Block Features (DLFEAT):\n");
    if (_pNSData->DLFEAT.IsGuardFieldCRC) {
        printf(
            "\tbit [      4] 1 = Guard field for deallocated logical blocks "
            "that contain protection information is set to the CRC for the "
            "value read from the deallocated logical block and its metadata "
            "(excluding protection information).\n");
    } else {
        printf(
            "\tbit [      4] 0 = Guard field for the deallocated logical "
            "blocks that contain protection information is set to FFFFh.\n");
    }
    if (_pNSData->DLFEAT.DeallocateInWriteZero) {
        printf(
            "\tbit [      3] 1 = Supports Deallocate bit in Write Zeroes "
            "command for this namespace.\n");
    } else {
        printf(
            "\tbit [      3] 0 = Does not support Deallocate bit in Write "
            "Zeroes command for this namespace.\n");
    }
    switch (_pNSData->DLFEAT.ReadBehavior) {
        case 0:
            printf(
                "\tbit [  2:  0] 0 = Read behavior for a deallocatd logical "
                "block is not reported.\n");
            break;

        case 1:
            printf(
                "\tbit [  2:  0] 1 = A deallocated logical block returns all "
                "bytes cleared to 0h\n");
            break;

        case 2:
            printf(
                "\tbit [  2:  0] 2 = A deallocated logical block returns all "
                "bytes set to FFh.\n");
            break;

        default:
            break;
    }

    printf("[O] Namespace Atomic Write Unit Normal (NAWUN): ");
    if (_pNSData->NAWUN) {
        printf("%d (means %d sector(s))\n", _pNSData->NAWUN,
               _pNSData->NAWUN + 1);
    } else {
        printf("0 (The same size as that reported in the AWUN field)\n");
    }

    printf("[O] Namespace Atomic Write Unit Power Fail (NAWUPF): ");
    if (_pNSData->NAWUPF) {
        printf("%d (means %d sector(s))\n", _pNSData->NAWUPF,
               _pNSData->NAWUPF + 1);
    } else {
        printf("0 (The same size as that reported in the AWUPF field)\n");
    }

    printf("[O] Namespace Atomic Compare & Write Unit (NACWU): ");
    if (_pNSData->NACWU) {
        printf("%d (means %d sector(s))\n", _pNSData->NACWU,
               _pNSData->NACWU + 1);
    } else {
        printf("0 (The same size as that reported in the ACWU field)\n");
    }

    printf("[O] Namespace Atomic Boundary Size Normal (NABSN): ");
    if (_pNSData->NABSN) {
        printf("%d (means %d sector(s))\n", _pNSData->NABSN,
               _pNSData->NABSN + 1);
    } else {
        printf(
            "0 (There are no atomic boundaries for normal write operations)\n");
    }

    printf("[O] Namespace Atomic Boundary Offset (NABO): %d\n", _pNSData->NABO);

    printf("[O] Namespace Atomic Boundary Size Power Fail (NABSPF): ");
    if (_pNSData->NABSPF) {
        printf("%d (means %d sector(s))\n", _pNSData->NABSPF,
               _pNSData->NABSPF + 1);
    } else {
        printf(
            "0 (There are no atomic boundaries for power fail or error "
            "conditions)\n");
    }

    printf("[O] Namespace Optimal IO Boundary (NOIOB): ");
    if (_pNSData->NOIOB) {
        printf("%d (means %d sector(s))\n", _pNSData->NOIOB,
               _pNSData->NOIOB + 1);
    } else {
        printf("0 (No optimal IO boundary is reported)\n");
    }

    if (bIsNVMeV14OrLater()) {
        printf("[O] Namespace Preferred Write Granularity (NPWG): ");
        if (_pNSData->NSFEAT.OptPerf) {
            printf("%d = %d sectors\n", _pNSData->NPWG, _pNSData->NPWG + 1);
        } else {
            printf("n/a = Does not support NPWG, NPWA, NPDG, NPDA, and NOWS\n");
        }

        printf("[O] Namespace Preferred Write Alignment (NPWA): ");
        if (_pNSData->NSFEAT.OptPerf) {
            printf("%d = %d sectors\n", _pNSData->NPWA, _pNSData->NPWA + 1);
        } else {
            printf("n/a = Does not support NPWG, NPWA, NPDG, NPDA, and NOWS\n");
        }

        printf("[O] Namespace Preferred Deallocate Granularity (NPDG): ");
        if (_pNSData->NSFEAT.OptPerf) {
            printf("%d = %d sectors\n", _pNSData->NPDG, _pNSData->NPDG + 1);
        } else {
            printf("n/a = Does not support NPWG, NPWA, NPDG, NPDA, and NOWS\n");
        }

        printf("[O] Namespace Preferred Deallocate Alignment (NPDA): ");
        if (_pNSData->NSFEAT.OptPerf) {
            printf("%d = %d sectors\n", _pNSData->NPDA, _pNSData->NPDA + 1);
        } else {
            printf("n/a = Does not support NPWG, NPWA, NPDG, NPDA, and NOWS\n");
        }

        printf("[O] Namespace Optimal Write Size (NOWS): ");
        if (_pNSData->NSFEAT.OptPerf) {
            printf("%d = %d sectors\n", _pNSData->NOWS, _pNSData->NOWS + 1);
        } else {
            printf("n/a = Does not support NPWG, NPWA, NPDG, NPDA, and NOWS\n");
        }
    }

    if (bIsNVMeV20OrLater()) {
        printf("[O] Maximum Single Source Range Length (MSSRL) = %d\n",
               _pNSData->MSSRL);
        printf("[O] Maximum Copy Length (MCL) = %d (sectors)\n", _pNSData->MCL);
        printf(
            "[O] Maximum Source Range Count (MSRC) = %d (means %d entries)\n",
            _pNSData->MSRC, _pNSData->MSRC + 1);
    }

    {
        uint64_t nvmcap_in_gib =
            (((_pNSData->NVMCAP_H) << 32) | ((_pNSData->NVMCAP_L) >> 32) << 2);
        if (_pNSData->NVMCAP_H == 0) {
            printf(
                "[O] NVM Capacity (NVMCAP): 0x%016llX (bytes), about %llu "
                "GiB\n",
                (_pNSData->NVMCAP_L), nvmcap_in_gib);
        } else {
            printf(
                "[O] NVM Capacity (NVMCAP): 0x%016llX%016llX (bytes), about "
                "%llu GiB\n",
                (_pNSData->NVMCAP_H), (_pNSData->NVMCAP_L), nvmcap_in_gib);
        }
    }

    if (bIsNVMeV14OrLater()) {
        printf("[O] ANA Group Identifier (ANAGRPID): ");
        if (g_stController.CMIC.ANAReport) {
            printf("%d\n", _pNSData->ANAGRPID);
        } else {
            printf(
                "n/a (Does not support Asymmetric Namespace Access "
                "Reporting)\n");
        }

        printf("[O] Namespace Attributes (NSATTR):\n");
        if (_pNSData->NSFEAT.OptPerf) {
            printf("\tbit [      0] 1 = This namespace is write protected\n");
        } else {
            printf(
                "\tbit [      0] 0 = This namespace is not write protected\n");
        }

        printf("[O] NVM Set Identifier (NVMSETID): ");
        if (g_stController.CTRATT.NVMSet) {
            printf("%d\n", _pNSData->NVMSETID);
        } else {
            printf("n/a (Does not support NVM Set)\n");
        }

        printf("[O] Endurance Group Identifier (ENDGID): ");
        if (g_stController.CTRATT.EnduranceGroups) {
            printf("%d\n", _pNSData->ENDGID);
        } else {
            printf("n/a (Does not support Endurance Group)\n");
        }
    }

    printf("[O] Namespace Globally Unique Identifier (NGUID):\n");
    printf(
        "\tbyte [ 15: 11] %02X%02X%02X%02X%02X (Extension identifier assigned "
        "by the vendor)\n",
        _pNSData->NGUID[11], _pNSData->NGUID[12], _pNSData->NGUID[13],
        _pNSData->NGUID[14],
        _pNSData->NGUID[15]);  // Bytes [119:115] (5 byte, 40 bit = Extension
                               // identifer assigned by the organization
    printf(
        "\tbyte [ 10:  8] %02X%02X%02X (Organizationally Unique Identifier "
        "(OUI))\n",
        _pNSData->NGUID[8], _pNSData->NGUID[9],
        _pNSData->NGUID[10]);  // Bytes [114:112] (3 byte, 24 bit =
                               // Organizationally Unique Identifier (OUI) value
                               // assigned by the IEEE Registration Authority
    printf(
        "\tbyte [  7:  0] %02X%02X%02X%02X%02X%02X%02X%02X (Vendor specific "
        "extension identifier)\n",
        _pNSData->NGUID[0], _pNSData->NGUID[1], _pNSData->NGUID[2],
        _pNSData->NGUID[3], _pNSData->NGUID[4], _pNSData->NGUID[5],
        _pNSData->NGUID[6],
        _pNSData->NGUID[7]);  // Bytes [111:104] (8 byte, 64 bit = Vendor
                              // specific extension identifier

    printf("[O] IEEE Extended Unique Identifier (EUI64):\n");
    printf(
        "\tbyte [  7:  3] %02X%02X%02X%02X%02X (Extension identifier assigned "
        "by the vendor)\n",
        _pNSData->EUI64[3], _pNSData->EUI64[4], _pNSData->EUI64[5],
        _pNSData->EUI64[6],
        _pNSData->EUI64[7]);  // Bytes [127:123] (5 byte, 40 bit = Extension
                              // identifer assigned by the organization
    printf(
        "\tbyte [  2:  0] %02X%02X%02X (Organizationally Unique Identifier "
        "(OUI))\n",
        _pNSData->EUI64[0], _pNSData->EUI64[1],
        _pNSData->EUI64[2]);  // Bytes [122:120] (3 byte, 24 bit =
                              // Organizationally Unique Identifier (OUI) value
                              // assigned by the IEEE Registration Authority

    for (int i = 0; i < (_pNSData->NLBAF + 1); i++) {
        if (i == 0) {
            printf("[M] LBA Format 0 Support (LBAF0):\n");
        } else {
            printf("[O] LBA Format %d Support (LBAF%d):\n", i, i);
        }

        switch (_pNSData->LBAF[i].RP) {
            case 0:
                printf(
                    "\tbit [ 25: 24] 0 = Relative Performance (RP): Best "
                    "performance\n");
                break;

            case 1:
                printf(
                    "\tbit [ 25: 24] 1 = Relative Performance (RP): Better "
                    "performance\n");
                break;

            case 2:
                printf(
                    "\tbit [ 25: 24] 2 = Relative Performance (RP): Good "
                    "performance\n");
                break;

            case 3:
                printf(
                    "\tbit [ 25: 24] 3 = Relative Performance (RP): Degraded "
                    "performance\n");
                break;

            default:
                break;  // not reach
        }
        printf(
            "\tbit [ 23: 16] %d = LBA Data Size (LBADS): %d bytes / sector\n",
            _pNSData->LBAF[i].LBADS, 1 << (_pNSData->LBAF[i].LBADS));
        printf("\tbit [ 15:  0] %d = Metadata Size (MS): %d bytes\n",
               _pNSData->LBAF[i].MS, _pNSData->LBAF[i].MS);
    }
}

int iNVMeIdentifyNamespace(HANDLE _hDevice, DWORD _dwNSID, bool _bPrintOrNot) {
    int iResult = -1;
    PVOID buffer = NULL;
    ULONG bufferLength = 0;
    ULONG returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    if (bNamespaceAlreadyIdentified(_dwNSID)) {
        printf("[I] Identify Namespace data has already retrieved.\n");
        goto print_ns_data;
    }

    // Allocate buffer for use.
    bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters) +
                   sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) +
                   sizeof(MY_NVME_IDENTIFY_NAMESPACE_DATA);
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
        NVME_IDENTIFY_CNS_SPECIFIC_NAMESPACE;
    protocolData->ProtocolDataRequestSubValue = _dwNSID;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = sizeof(MY_NVME_IDENTIFY_NAMESPACE_DATA);

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
         sizeof(MY_NVME_IDENTIFY_NAMESPACE_DATA))) {
        fprintf(stderr,
                "[E] NVMeIdentifyNamespace: ProtocolData Offset/Length not "
                "valid, stop.\n");
        goto error_exit;
    }

    memcpy_s((void*)(&g_stNamespaceData),
             sizeof(MY_NVME_IDENTIFY_NAMESPACE_DATA),
             (uint8_t*)protocolData + protocolData->ProtocolDataOffset,
             sizeof(MY_NVME_IDENTIFY_NAMESPACE_DATA));
    g_bNamespaceAlreadyIdentified = true;

    // Identify Namespace
print_ns_data:
    if (_bPrintOrNot)
        printNVMeIdentifyNamespaceData(&g_stNamespaceData, _dwNSID);
    iResult = 0;  // succeeded

error_exit:

    if (buffer != NULL) {
        free(buffer);
    }

    return iResult;
}
