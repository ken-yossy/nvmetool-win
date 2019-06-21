#pragma once

#include <windows.h>
#include <cstdint>
#include <nvme.h>

// Identify Controller data structure (v1.3d) based on nvme.h
typedef struct {
    //
    // byte 0 : 255, Controller Capabilities and Features
    //
    uint16_t    VID;                // byte [   1:   0] M - PCI Vendor ID (VID)
    uint16_t    SSVID;              // byte [   3:   2] M - PCI Subsystem Vendor ID (SSVID)
    uint8_t     SN[20];             // byte [  23:   4] M - Serial Number (SN)
    uint8_t     MN[40];             // byte [  63:  24] M - Model Number (MN)
    uint8_t     FR[8];              // byte [  71:  64] M - Firmware Revision (FR)
    uint8_t     RAB;                // byte [       72] M - Recommended Arbitration Burst (RAB)
    uint8_t     IEEE[3];            // byte [  75:  73] M - IEEE OUI Identifier (IEEE). Controller Vendor code.

    struct {
        uint8_t MultiPCIePorts : 1;     // bit [  0]
        uint8_t MultiControllers : 1;   // bit [  1]
        uint8_t SRIOV : 1;              // bit [  2]
        uint8_t Reserved : 5;           // bit [7:3]
    } CMIC;                         // byte [       76] O - Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC)

    uint8_t     MDTS;               // byte [       77] M - Maximum Data Transfer Size (MDTS)
    uint16_t    CNTLID;             // byte [  79:  78] M - Controller ID (CNTLID)
    uint32_t    VER;                // byte [  83:  80] M - Version (VER)
    uint32_t    RTD3R;              // byte [  87:  84] M - RTD3 Resume Latency (RTD3R)
    uint32_t    RTD3E;              // byte [  91:  88] M - RTD3 Entry Latency (RTD3E)

    struct {
        uint32_t    Reserved0 : 8;                  // bit [ 7: 0]
        uint32_t    NamespaceAttributeChanged : 1;  // bit [    8]
        uint32_t    FirmwareActivation : 1;         // bit [    9]
        uint32_t    Reserved1 : 22;                 // bit [31:10]
    } OAES;                         // byte [  95:  92] M - Optional Asynchronous Events Supported (OAES)

    struct {
        uint32_t    HostIdEn : 1;               // bit [    0]
        uint32_t    NoopPSPermissiveModeEn : 1; // bit [    1]
        uint32_t    Reserved : 30;              // bit [31: 2]
    } CTRATT;                       // byte [  99:  96] M - Controller Attributes (CTRATT)

    uint8_t Reserved0[12];          // byte [ 111: 100]

    uint8_t FGUID[16];              // byte [ 127: 112] O - FRU Globally Unique Identifier (FGUID)

    uint8_t Reserved1[112];         // byte [ 239: 128]
    uint8_t Reserved2[16];          // byte [ 255: 240] Refer to the NVMe Management Interface Specification for definition

    //
    // byte 256 : 511, Admin Command Set Attributes
    //
    struct {
        uint16_t    SecurityCommands : 1;   // bit [    0]
        uint16_t    FormatNVM : 1;          // bit [    1]
        uint16_t    FirmwareCommands : 1;   // bit [    2]
        uint16_t    NamespaceCommands : 1;  // bit [    3]
        uint16_t    DeviceSelfTest : 1;     // bit [    4]
        uint16_t    Directives : 1;         // bit [    5]
        uint16_t    NVMeMICommands : 1;     // bit [    6]
        uint16_t    VirtMgmtCommands : 1;   // bit [    7]
        uint16_t    DBConfigCommand : 1;    // bit [    8]
        uint16_t    Reserved : 7;           // bit [15: 9]
    } OACS;                         // byte [ 257: 256] M - Optional Admin Command Support (OACS)

    uint8_t ACL;                    // byte [      258] M - Abort Command Limit (ACL)
    uint8_t AERL;                   // byte [      259] M - Asynchronous Event Request Limit (AERL)

    struct {
        uint8_t Slot1ReadOnly : 1;          // bit [    0]
        uint8_t SlotCount : 3;              // bit [ 3: 1]
        uint8_t ActivationWithoutReset : 1; // bit [    4]
        uint8_t Reserved : 3;               // bit [ 7: 5]
    } FRMW;                         // byte [      260] M - Firmware Updates (FRMW)

    struct {
        uint8_t SmartPagePerNamespace : 1;  // bit [    0]
        uint8_t CommandEffectsLog : 1;      // bit [    1]
        uint8_t LogPageExtendedData : 1;    // bit [    2]
        uint8_t TelemetrySupport : 1;       // bit [    3]
        uint8_t Reserved : 4;               // bit [ 7: 4]
    } LPA;                          // byte [      261] M - Log Page Attributes (LPA)

    uint8_t ELPE;                   // byte [      262] M - Error Log Page Entries (ELPE)
    uint8_t NPSS;                   // byte [      263] M - Number of Power States Support (NPSS)

    struct {
        uint8_t CommandFormatInSpec : 1;    // bit [    0]
        uint8_t Reserved : 7;               // bit [ 7: 1]
    } AVSCC;                        // byte [      264] M - Admin Vendor Specific Command Configuration (AVSCC)

    struct {
        uint8_t Supported : 1;  // bit [    0]
        uint8_t Reserved : 7;   // bit [ 7: 1]
    } APSTA;                        // byte [      265] O - Autonomous Power State Transition Attributes (APSTA)

    uint16_t    WCTEMP;             // byte [ 267: 266] M - Warning Composite Temperature Threshold (WCTEMP)
    uint16_t    CCTEMP;             // byte [ 269: 268] M - Critical Composite Temperature Threshold (CCTEMP)
    uint16_t    MTFA;               // byte [ 271: 270] O - Maximum Time for Firmware Activation (MTFA)
    uint32_t    HMPRE;              // byte [ 275: 272] O - Host Memory Buffer Preferred Size (HMPRE)
    uint32_t    HMMIN;              // byte [ 279: 276] O - Host Memory Buffer Minimum Size (HMMIN)
    uint8_t     TNVMCAP[16];        // byte [ 295: 280] O - Total NVM Capacity (TNVMCAP)
    uint8_t     UNVMCAP[16];        // byte [ 311: 296] O - Unallocated NVM Capacity (UNVMCAP)

    struct {
        uint32_t    RPMBUnitCount : 3;          // bit [ 2: 0] Number of RPMB Units
        uint32_t    AuthenticationMethod : 3;   // bit [ 5: 3] Authentication Method
        uint32_t    Reserved0 : 10;             // bit [15: 6]
        uint32_t    TotalSize : 8;              // bit [23:16] Total Size: in 128KB units.
        uint32_t    AccessSize : 8;             // bit [31:24] Access Size: in 512B units.
    } RPMBS;                        // byte [ 315: 312] O - Replay Protected Memory Block Support (RPMBS)

    uint16_t    EDSTT;              // byte [ 317: 316] O - Extended Device Self-test Time (EDSTT)
    uint8_t     DSTO;               // byte [      318] O - Device Self-test Options (DSTO)
    uint8_t     FWUG;               // byte [      319] M - Firmware Update Granularity (FWUG)
    uint16_t    KAS;                // byte [ 321: 320] M - Keep Alive Support (KAS)

    struct {
        uint16_t    Supported : 1;  // bit [    0]
        uint16_t    Reserved : 15;  // bit [15: 0]
    } HCTMA;                        // byte [ 323: 322] O - Host Controlled Thermal Management Attributes (HCTMA)

    uint16_t    MNTMT;              // byte [ 325: 324] O - Minimum Thermal Management Temperature (MNTMT)
    uint16_t    MXTMT;              // byte [ 327: 326] O - Maximum Thermal Management Temperature (MXTMT)

    struct {
        uint32_t    CryptoErase : 1;// bit [    0] Controller supports Crypto Erase Sanitize
        uint32_t    BlockErase : 1; // bit [    1] Controller supports Block Erase Sanitize
        uint32_t    Overwrite : 1;  // bit [    2] Controller supports Overwrite Santize
        uint32_t    Reserved : 29;  // bit [31: 3]
    } SANICAP;                      // byte [ 331: 328] O - Sanitize Capabilities (SANICAP)

    uint8_t Reserved3[180];         // byte [ 511: 332]

    //
    // byte 512 : 703, NVM Command Set Attributes
    //
    struct {
        uint8_t RequiredEntrySize : 4;  // bit [ 3: 0] The value is in bytes and is reported as a power of two (2^n).
        uint8_t MaxEntrySize : 4;       // bit [ 7: 4] This value is larger than or equal to the required SQ entry size.  The value is in bytes and is reported as a power of two (2^n).
    } SQES;                         // byte [      512] M - Submission Queue Entry Size (SQES)

    struct {
        uint8_t RequiredEntrySize : 4;  // bit [ 3: 0] The value is in bytes and is reported as a power of two (2^n).
        uint8_t MaxEntrySize : 4;       // bit [ 7: 4] This value is larger than or equal to the required CQ entry size. The value is in bytes and is reported as a power of two (2^n).
    } CQES;                         // byte [      513] M - Completion Queue Entry Size (CQES)

    uint16_t    MAXCMD;             // byte [ 515: 514] M - Maximum Outstanding Commands (MAXCMD)
    uint32_t    NN;                 // byte [ 519: 516] M - Number of Namespaces (NN)

    struct {
        uint16_t    Compare : 1;            // bit [    0]
        uint16_t    WriteUncorrectable : 1; // bit [    1]
        uint16_t    DatasetManagement : 1;  // bit [    2]
        uint16_t    WriteZeroes : 1;        // bit [    3]
        uint16_t    FeatureField : 1;       // bit [    4]
        uint16_t    Reservations : 1;       // bit [    5]
        uint16_t    Timestamp : 1;          // bit [    6]
        uint16_t    Reserved : 9;           // bit [15: 7]
    } ONCS;                         // byte [ 521: 520] M - Optional NVM Command Support (ONCS)

    struct {
        uint16_t    CompareAndWrite : 1;    // bit [    0]
        uint16_t    Reserved : 15;          // bit [15: 1]
    } FUSES;                        // byte [ 523: 522] M - Fused Operation Support (FUSES)

    struct {
        uint8_t     FormatApplyToAll : 1;           // bit [    0]
        uint8_t     SecureEraseApplyToAll : 1;      // bit [    1]
        uint8_t     CryptographicEraseSupported : 1;// bit [    2]
        uint8_t     Reserved : 5;                   // bit [ 7: 3]
    } FNA;                          // byte [      524] M - Format NVM Attributes (FNA)

    struct {
        uint8_t     Present : 1;    // bit [    0]
        uint8_t     Reserved : 7;   // bit [ 7: 0]
    } VWC;                          // byte [      525] M - Volatile Write Cache (VWC)

    uint16_t    AWUN;               // byte [ 527: 526] M - Atomic Write Unit Normal (AWUN)
    uint16_t    AWUPF;              // byte [ 529: 528] M - Atomic Write Unit Power Fail (AWUPF)

    struct {
        uint8_t CommandFormatInSpec : 1;    // bit [    0]
        uint8_t Reserved : 7;               // bit [ 7: 0]
    } NVSCC;                        // byte [      530] M - NVM Vendor Specific Command Configuration (NVSCC)

    uint8_t Reserved4;              // byte [      531]

    uint16_t    ACWU;               // byte [ 533: 532] O - Atomic Compare & Write Unit (ACWU)

    uint8_t Reserved5[2];           // byte [ 535: 534]

    struct {
        uint32_t    SGLSupported : 2;                       // bit [ 1: 0]
        uint32_t    KeyedBBDescSupported : 1;               // bit [    2]
        uint32_t    Reserved0 : 13;                         // bit [15: 3]
        uint32_t    BitBucketDescrSupported : 1;            // bit [   16]
        uint32_t    ByteAlignedContiguousPhysicalBuffer : 1;// bit [   17]
        uint32_t    SGLLengthLargerThanDataLength : 1;      // bit [   18]
        uint32_t    MPTRContainingSGLDescSupported : 1;     // bit [   19]
        uint32_t    OffsetByAddrFieldSupported : 1;         // bit [   20]
        uint32_t    Reserved1 : 11;                         // bit [31:21]
    } SGLS;                         // byte [ 539: 536] O - SGL Support (SGLS)

    uint8_t Reserved6[228];         // byte [ 767: 540]

    uint8_t SUBNQN[256];            // byte [1023: 768] M - NVM Subsystem NVMe Qualified Name (SUBNQN)

    uint8_t Reserved7[768];         // byte [1791:1024]
    uint8_t Reserved8[256];         // byte [2047:1792] Refer to the NVMe over Fabrics specification

    NVME_POWER_STATE_DESC   PDS[32];// byte [2079:2048] M - Power State 0 Descriptor (PSD0)
                                    // byte [3071:2080] O - Power State Descriptors from PS1 (PSD1) to PS31 (PSD31) 

    uint8_t VS[1024];              // byte [4095:3072] Vendor Specific
} NVME_IDENTIFY_CONTROLLER_DATA13, *PNVME_IDENTIFY_CONTROLLER_DATA13;

extern NVME_IDENTIFY_CONTROLLER_DATA13 g_stController;
    
int iNVMeIdentifyController(HANDLE _hDevice);
void vPrintControllerBasicData(void);
void vPrintNVMeIdentifyControllerData(void);

