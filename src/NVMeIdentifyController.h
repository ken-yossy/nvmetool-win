#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include <nvme.h>

// Identify Controller data structure (upto v2.0) based on nvme.h
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
        uint8_t ANAReport : 1;          // bit [  3] <rev1.4>
        uint8_t Reserved : 4;           // bit [7:4]
    } CMIC;                         // byte [       76] O - Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC)

    uint8_t     MDTS;               // byte [       77] M - Maximum Data Transfer Size (MDTS)
    uint16_t    CNTLID;             // byte [  79:  78] M - Controller ID (CNTLID)
    uint32_t    VER;                // byte [  83:  80] M - Version (VER)
    uint32_t    RTD3R;              // byte [  87:  84] M - RTD3 Resume Latency (RTD3R)
    uint32_t    RTD3E;              // byte [  91:  88] M - RTD3 Entry Latency (RTD3E)

    struct {
        uint32_t    Reserved0 : 8;                                  // bit [ 7: 0]
        uint32_t    NamespaceAttributeChanged : 1;                  // bit [    8]
        uint32_t    FirmwareActivation : 1;                         // bit [    9]
        uint32_t    Reserved1 : 1;                                  // bit [   10]
        uint32_t    ANAChange : 1;                                  // bit [   11] <rev1.4>
        uint32_t    PredictableLatencyEventAggregateLogChange : 1;  // bit [   12] <rev1.4>
        uint32_t    LBAStatusInfo : 1;                              // bit [   13] <rev1.4>
        uint32_t    EnduranceGroupEventAggregateLogChange : 1;      // bit [   14] <rev1.4>
        uint32_t    NVMSubsystemShutdown : 1;                       // bit [   15] <rev2.0>
        uint32_t    Reserved2 : 11;                                 // bit [26:16]
        uint32_t    ZoneDesciptorChanged : 1;                       // bit [   27] <rev2.0>
        uint32_t    Reserved3 : 3;                                  // bit [30:28]
        uint32_t    DiscoveryLogChanged : 1;                        // bit [   31] <rev2.0>
    } OAES;                         // byte [  95:  92] M - Optional Asynchronous Events Supported (OAES)

    struct {
        uint32_t    HostIdEn : 1;               // bit [    0]
        uint32_t    NoopPSPermissiveModeEn : 1; // bit [    1] <rev1.3>
        uint32_t    NVMSet : 1;                 // bit [    2] <rev1.4>
        uint32_t    ReadRecoveryLevel : 1;      // bit [    3] <rev1.4>
        uint32_t    EnduranceGroups : 1;        // bit [    4] <rev1.4>
        uint32_t    PredictableLatencyMode : 1; // bit [    5] <rev1.4>
        uint32_t    TrafficBasedKeepAlive : 1;  // bit [    6] <rev1.4>
        uint32_t    NamespaceGranularity : 1;   // bit [    7] <rev1.4>
        uint32_t    SQAssociation : 1;          // bit [    8] <rev1.4>
        uint32_t    UUIDList : 1;               // bit [    9] <rev1.4>
        uint32_t    MDS : 1;                    // bit [   10] <rev2.0>
        uint32_t    FixedCapMgmt : 1;           // bit [   11] <rev2.0>
        uint32_t    VariableCapMgmt : 1;        // bit [   12] <rev2.0>
        uint32_t    DelEndGrp : 1;              // bit [   13] <rev2.0>
        uint32_t    DelNVMSet : 1;              // bit [   14] <rev2.0>
        uint32_t    ExtLBAFmt : 1;              // bit [   15] <rev2.0>
        uint32_t    Reserved : 16;              // bit [31:16]
    } CTRATT;                       // byte [  99:  96] M - Controller Attributes (CTRATT)

    uint16_t    RRLS;               // byte [ 101: 100] O - Read Recovery Levels Supported (RRLS) <rev1.4>
    uint8_t     Reserved0[9];       // byte [ 110: 102]

    uint8_t     CNTRLTYPE;          // byte [      111] M - Controller Type (CNTRLTYPE)             <rev1.4>
    uint8_t     FGUID[16];          // byte [ 127: 112] O - FRU Globally Unique Identifier (FGUID)  <rev1.3>
    uint16_t    CRDT1;              // byte [ 129: 128] O - Command Retry Delay Time 1 (CRDT1)      <rev1.4>
    uint16_t    CRDT2;              // byte [ 131: 130] O - Command Retry Delay Time 2 (CRDT2)      <rev1.4>
    uint16_t    CRDT3;              // byte [ 133: 132] O - Command Retry Delay Time 3 (CRDT3)      <rev1.4>

    uint8_t     Reserved1[106];     // byte [ 239: 134]
    uint8_t     Reserved2[13];      // byte [ 252: 240] Refer to the NVMe Management Interface Specification for definition

    struct {
        uint32_t    NVMESD : 1;     // bit [    0]
        uint32_t    NVMEE : 1;      // bit [    1]
        uint32_t    Reserved : 6;   // bit [ 7: 2]
    } NVMSR;                        // byte [      253] M - NVM Subsystem Report (NVMSR) <rev2.0>

    struct {
        uint32_t    VMCR : 7;       // bit [ 6: 0]
        uint32_t    VMCRV : 1;      // bit [    7]
    } VWCI;                         // byte [      254] M - VPD Write Cycle Information (VMCI) <rev2.0>

    struct {
        uint32_t    SMBUSME : 1;    // bit [    0]
        uint32_t    PCIEME : 1;     // bit [    1]
        uint32_t    Reserved : 6;   // bit [ 7: 2]
    } MEC;                          // byte [      255] M - Management Endpoint Capabilities (MEC) <rev2.0>

    //
    // byte 256 : 511, Admin Command Set Attributes
    //
    struct {
        uint16_t    SecurityCommands : 1;   // bit [    0]
        uint16_t    FormatNVM : 1;          // bit [    1]
        uint16_t    FirmwareCommands : 1;   // bit [    2]
        uint16_t    NamespaceCommands : 1;  // bit [    3]
        uint16_t    DeviceSelfTest : 1;     // bit [    4] <rev1.3>
        uint16_t    Directives : 1;         // bit [    5] <rev1.3>
        uint16_t    NVMeMICommands : 1;     // bit [    6] <rev1.3>
        uint16_t    VirtMgmtCommands : 1;   // bit [    7] <rev1.3>
        uint16_t    DBConfigCommand : 1;    // bit [    8] <rev1.3>
        uint16_t    GetLBAStatusCommand : 1;// bit [    9] <rev1.4>
        uint16_t    Lockdown : 1;           // bit [   10] <rev2.0>
        uint16_t    Reserved : 5;           // bit [15:11]
    } OACS;                         // byte [ 257: 256] M - Optional Admin Command Support (OACS)

    uint8_t ACL;                    // byte [      258] M - Abort Command Limit (ACL)
    uint8_t AERL;                   // byte [      259] M - Asynchronous Event Request Limit (AERL)

    struct {
        uint8_t Slot1ReadOnly : 1;          // bit [    0]
        uint8_t SlotCount : 3;              // bit [ 3: 1]
        uint8_t ActivationWithoutReset : 1; // bit [    4]
        uint8_t MultipleUpdateDetect : 1;   // bit [    5] <rev2.0>
        uint8_t Reserved : 2;               // bit [ 7: 6]
    } FRMW;                         // byte [      260] M - Firmware Updates (FRMW)

    struct {
        uint8_t SmartPagePerNamespace : 1;  // bit [    0]
        uint8_t CommandEffectsLog : 1;      // bit [    1]
        uint8_t LogPageExtendedData : 1;    // bit [    2]
        uint8_t TelemetrySupport : 1;       // bit [    3] <rev1.3>
        uint8_t PersistentEventLog : 1;     // bit [    4] <rev1.4>
        uint8_t CommandScope : 1;           // bit [    5] <rev2.0>
        uint8_t TelemetryArea4 : 1;         // bit [    6] <rev2.0>
        uint8_t Reserved : 1;               // bit [    7]
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

    uint16_t    EDSTT;              // byte [ 317: 316] O - Extended Device Self-test Time (EDSTT)  <rev1.3>
    uint8_t     DSTO;               // byte [      318] O - Device Self-test Options (DSTO)         <rev1.3>
    uint8_t     FWUG;               // byte [      319] M - Firmware Update Granularity (FWUG)      <rev1.3>
    uint16_t    KAS;                // byte [ 321: 320] M - Keep Alive Support (KAS)

    struct {
        uint16_t    Supported : 1;  // bit [    0]
        uint16_t    Reserved : 15;  // bit [15: 1]
    } HCTMA;                        // byte [ 323: 322] O - Host Controlled Thermal Management Attributes (HCTMA)  <rev1.3>

    uint16_t    MNTMT;              // byte [ 325: 324] O - Minimum Thermal Management Temperature (MNTMT) <rev1.3>
    uint16_t    MXTMT;              // byte [ 327: 326] O - Maximum Thermal Management Temperature (MXTMT) <rev1.3>

    struct {
        uint32_t    CryptoErase : 1;            // bit [    0] Controller supports Crypto Erase Sanitize
        uint32_t    BlockErase : 1;             // bit [    1] Controller supports Block Erase Sanitize
        uint32_t    Overwrite : 1;              // bit [    2] Controller supports Overwrite Santize
        uint32_t    Reserved : 26;              // bit [28: 3]
        uint32_t    NoDeallocateInhibited : 1;  // bit [   29] No-Deallocate Inhibited (NDI)                            <rev1.4>
        uint32_t    NODMMAS : 2;                // bit [31:30] No-Deallocate Modifies Media After Sanitize (NODMMAS)    <rev1.4>
    } SANICAP;                      // byte [ 331: 328] O - Sanitize Capabilities (SANICAP)                             <rev1.3>

    uint32_t    HMMINDS;            // byte [ 335: 332] O - Host Memory Buffer Minimum Descriptor Entry Size (HMMINDS)  <rev1.4>
    uint16_t    HMMAXD;             // byte [ 337: 336] O - Host Memory Maximum Descriptors Entries (HMMAXD)            <rev1.4>
    uint16_t    NSETIDMAX;          // byte [ 339: 338] O - NVM Set Identifier Maximum (NSETIDMAX)                      <rev1.4>
    uint16_t    ENDGIDMAX;          // byte [ 341: 340] O - Endurance Group Identifier Maximum (ENDGIDMAX)              <rev1.4>
    uint8_t     ANATT;              // byte [      342] O - ANA Transition Time (ANATT)                                 <rev1.4>

    struct {
        uint8_t     ReportOptimizedState : 1;       // bit [    0] able to report ANA Optimized state
        uint8_t     ReportNonOptimizedState : 1;    // bit [    1] able to report ANA Non-Optimized state
        uint8_t     ReportInaccessibleState : 1;    // bit [    2] able to report ANA Inaccessible state
        uint8_t     ReportPersistentLossState : 1;  // bit [    3] able to report ANA Persistent Loss state
        uint8_t     ReportChangeState : 1;          // bit [    4] able to report ANA Change state
        uint8_t     Reserved0 : 1;                  // bit [    5]
        uint8_t     NoGRPIDChangeDuringAttached : 1;// bit [    6]
        uint8_t     NonZeroGRPIDSupport : 1;        // bit [    7]
    } ANACAP;                       // byte [      343] O - Asymmetric Namespace Access Capabilities (ANACAP) <rev1.4>

    uint32_t    ANAGRPMAX;          // byte [ 347: 344] O - ANA Group Identifier Maximum (ANAGRPMAX)    <rev1.4>
    uint32_t    NANAGRPID;          // byte [ 351: 348] O - Number of ANA Group Identifiers (NANAGRPID) <rev1.4>
    uint32_t    PELS;               // byte [ 355: 352] O - Persistent Event Log Size (PELS)            <rev1.4>

    uint16_t    DomainId;           // byte [ 357: 356] O - Domain Identifier <rev2.0>
    uint8_t     Reserved3[10];      // byte [ 367: 358]
    uint8_t     MEGCAP[16];         // byte [ 383: 368] O - Max Endurance Group Capacity <rev2.0>

    uint8_t     Reserved4[128];     // byte [ 511: 384]

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
        uint16_t    Timestamp : 1;          // bit [    6] <rev1.3>
        uint16_t    Verify : 1;             // bit [    7] <rev1.4>
        uint16_t    Copy : 1;               // bit [    8] <rev2.0>
        uint16_t    Reserved : 7;           // bit [15: 9]
    } ONCS;                         // byte [ 521: 520] M - Optional NVM Command Support (ONCS)

    struct {
        uint16_t    CompareAndWrite : 1;    // bit [    0]
        uint16_t    Reserved : 15;          // bit [15: 1]
    } FUSES;                        // byte [ 523: 522] M - Fused Operation Support (FUSES)

    struct {
        uint8_t     FormatApplyToAll : 1;           // bit [    0]
        uint8_t     SecureEraseApplyToAll : 1;      // bit [    1]
        uint8_t     CryptographicEraseSupported : 1;// bit [    2]
        uint8_t     NotAcceptNAIDIsAllF : 1;        // bit [    3] <rev2.0>
        uint8_t     Reserved : 4;                   // bit [ 7: 4]
    } FNA;                          // byte [      524] M - Format NVM Attributes (FNA)

    struct {
        uint8_t     Present : 1;        // bit [    0]
        uint8_t     FlushBehavior : 2;  // bit [ 2: 1] <rev1.4>
        uint8_t     Reserved : 5;       // bit [ 7: 3]
    } VWC;                          // byte [      525] M - Volatile Write Cache (VWC)

    uint16_t    AWUN;               // byte [ 527: 526] M - Atomic Write Unit Normal (AWUN)
    uint16_t    AWUPF;              // byte [ 529: 528] M - Atomic Write Unit Power Fail (AWUPF)

    struct {
        uint8_t CommandFormatInSpec : 1;    // bit [    0]
        uint8_t Reserved : 7;               // bit [ 7: 0]
    } NVSCC;                        // byte [      530] M - NVM Vendor Specific Command Configuration (NVSCC)

    struct {
        uint8_t NoWriteProtectSupport : 1;              // bit [    0]
        uint8_t WriteProtectUntilPowerCycleSupport : 1; // bit [    1]
        uint8_t PermanentWriteProtectSupport : 1;       // bit [    2]
        uint8_t Reserved : 5;                           // bit [ 7: 3]
    } NWPC;                         // byte [      531] M - Namespace Write Protection Capabilities (NWPC) <rev1.4>

    uint16_t    ACWU;               // byte [ 533: 532] O - Atomic Compare & Write Unit (ACWU)

    struct {
        uint16_t FormatZeroSupp : 1;    // bit [    0]
        uint16_t Reserved : 15;         // bit [15: 1]
    } OCFS;                        // byte [ 535: 534] M - Optional Copy Formats Supported <rev2.0>

    struct {
        uint32_t    SGLSupported : 2;                           // bit [ 1: 0] <rev1.3>
        uint32_t    KeyedDBDescSupported : 1;                   // bit [    2]
        uint32_t    Reserved0 : 5;                              // bit [ 7: 3]
        uint32_t    SDT : 8;                                    // bit [15: 8] <rev2.0>
        uint32_t    BitBucketDescrSupported : 1;                // bit [   16]
        uint32_t    ByteAlignedContiguousPhysicalBuffer : 1;    // bit [   17]
        uint32_t    SGLLengthLargerThanDataLength : 1;          // bit [   18]
        uint32_t    MPTRContainingSGLDescSupported : 1;         // bit [   19]
        uint32_t    OffsetByAddrFieldSupported : 1;             // bit [   20]
        uint32_t    TransactionalSGLDataBlockDescSupported : 1; // bit [   21] <rev1.4>
        uint32_t    Reserved1 : 10;                             // bit [31:22]
    } SGLS;                         // byte [ 539: 536] O - SGL Support (SGLS)

    uint32_t MNAN;                  // byte [ 543: 540] O - Maximum Number of Allowed Namespaces (MNAN) <rev1.4>
    uint8_t MAXDNA[16];             // byte [ 559: 544] O - Maximum Domain Namespace Attachments (MAXDNA) <rev2.0>
    uint32_t MAXCNA;                // byte [ 563: 560] O - Maximum I/O Controller Namespace Attachments (MAXCNA) <rev2.0>

    uint8_t Reserved6[204];         // byte [ 767: 564]

    uint8_t SUBNQN[256];            // byte [1023: 768] M - NVM Subsystem NVMe Qualified Name (SUBNQN)

    uint8_t Reserved7[768];         // byte [1791:1024]

    uint32_t IOCCSZ;                // byte [1795:1792] M - I/O Queue Command Capsule Supported Size (IOCCSZ) <rev2.0>
    uint32_t IORCSZ;                // byte [1799:1796] M - I/O Queue Response Capsule Supported Size (IORCSZ) <rev2.0>
    uint16_t ICDOFF;                // byte [1801:1800] M - In Capsule Data Offset <rev2.0>

    struct {
        uint8_t StaticCtrlModel : 1;    // bit [    0]
        uint8_t Reserved : 7;           // bit [ 7: 1]
    } FCATT;                        // byte [     1802] M - Fabrics Controller Attributes (FCATT) <rev2.0>

    uint8_t MSDBD;                  // byte [     1803] M - Maximum SGL Data lock Descriptors (MSDBD) <rev2.0>

    struct {
        uint16_t DisconnectSupp : 1;    // bit [    0]
        uint16_t Reserved : 15;         // bit [15: 1]
    } OFCS;                         // byte [1805:1804] M - Optional Fabric Commands Support (OFCS) <rev2.0>

    uint8_t Reserved8[242];         // byte [2047:1806]

    NVME_POWER_STATE_DESC   PDS[32];// byte [2079:2048] M - Power State 0 Descriptor (PSD0)
                                    // byte [3071:2080] O - Power State Descriptors from PS1 (PSD1) to PS31 (PSD31) 

    uint8_t VS[1024];              // byte [4095:3072] Vendor Specific
} MY_NVME_IDENTIFY_CONTROLLER_DATA, * PMY_NVME_IDENTIFY_CONTROLLER_DATA;

extern MY_NVME_IDENTIFY_CONTROLLER_DATA g_stController;
extern bool g_WA_bGetControllerSMARTLogWithNSIDZero;

int iNVMeIdentifyController(HANDLE _hDevice);
void vPrintControllerBasicData(void);
bool bIsNVMeV14OrLater(void);
bool bIsNVMeV13OrLater(void);
bool bIsNVMeV20OrLater(void);

