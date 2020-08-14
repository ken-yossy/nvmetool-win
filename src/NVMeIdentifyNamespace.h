#pragma once

#include <windows.h>
#include <cstdint>
#include <nvme.h>

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
} NVME_IDENTIFY_NAMESPACE_DATA13, *PNVME_IDENTIFY_NAMESPACE_DATA13;

int iNVMeIdentifyNamespace(HANDLE _hDevice, DWORD _dwNSID);

