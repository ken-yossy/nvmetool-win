#pragma once

#include <windows.h>
#include <cstdint>
#include <nvme.h>

//
// Information of log: NVME_LOG_PAGE_ERROR_INFO. Size: 64 bytes
//
typedef struct
{
    uint64_t            ErrorCount;         // byte [ 7: 0] Error Count
    uint16_t            SQID;               // byte [ 9: 8] Submission Queue ID
    uint16_t            CMDID;              // byte [11:10] Command ID
    NVME_COMMAND_STATUS Status;             // byte [13:12] Status Field

    union {
        struct {
            uint16_t    Byte : 8;       // bit [ 7: 0] Byte in command that contained the error.
            uint16_t    Bit : 3;        // bit [10: 8] Bit in command that contained the error.
            uint16_t    Reserved : 5;   // bit [15:11]
        } DUMMYSTRUCTNAME;

        uint16_t        AsUshort;
    } ParameterErrorLocation;               // byte [15:14] Parameter Error Location

    uint64_t            Lba;                // byte [23:16] LBA
    uint32_t            NameSpace;          // byte [27:24] Namespace
    uint8_t             VendorInfoAvailable;// byte [   28] Vendor Specific Information Available
    uint8_t             Reserved0[3];       // byte [31:29]
    uint64_t            CommandSpecificInfo;// byte [39:32] Command Specific Information
    uint8_t             Reserved1[24];      // byte [63:40]
} NVME_ERROR_INFO_LOG13, *PNVME_ERROR_INFO_LOG13;

int iNVMeGetErrorInformation(HANDLE _hDevice);
