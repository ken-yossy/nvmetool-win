#pragma once

#include <windows.h>
#include <cstdint>

//
// Information of log: NVME_LOG_PAGE_HEALTH_INFO. Size: 512 bytes
//
typedef struct {

    struct {
        uint8_t   AvailableSpaceLow : 1;                // bit [    0]
        uint8_t   TemperatureThreshold : 1;             // bit [    1]
        uint8_t   ReliabilityDegraded : 1;              // bit [    2]
        uint8_t   ReadOnly : 1;                         // bit [    3]
        uint8_t   VolatileMemoryBackupDeviceFailed : 1; // bit [    4]
        uint8_t   Reserved : 3;                         // bit [ 5: 7]
    } CriticalWarning;                  // byte [      0] Critical Warning

    uint8_t   Temperature[2];           // byte [  2:  1] Composite Temperature
    uint8_t   AvailableSpare;           // byte [      3] Available Spare
    uint8_t   AvailableSpareThreshold;  // byte [      4] Available Spare Threshold
    uint8_t   PercentageUsed;           // byte [      5] Percentage Used
    uint8_t   Reserved0[26];            // byte [ 31:  6]

    uint8_t   DataUnitRead[16];         // byte [ 47: 32] Data Units Read
    uint8_t   DataUnitWritten[16];      // byte [ 63: 48] Data Units Written
    uint8_t   HostReadCommands[16];     // byte [ 79: 64] Host Read Commands
    uint8_t   HostWrittenCommands[16];  // byte [ 95: 80] Host Write Commands
    uint8_t   ControllerBusyTime[16];   // byte [111: 66] Controller Busy Time
    uint8_t   PowerCycle[16];           // byte [127:112] Power Cycles
    uint8_t   PowerOnHours[16];         // byte [143:128] Power On Hours
    uint8_t   UnsafeShutdowns[16];      // byte [159:144] Unsafe Shutdowns
    uint8_t   MediaErrors[16];          // byte [175:160] Media and Data Integrity Errors
    uint8_t   ErrorInfoLogEntryNum[16]; // byte [191:176] Number of Error Information Log Entries
    uint32_t  WCTempTime;               // byte [195:192] Warning Composite Temperature Time
    uint32_t  CCTempTime;               // byte [196:196] Critical Composite Temperature Time

    uint16_t  TemperatureSensor1;       // byte [201:200] Current temperature reported by temperature sensor 1.
    uint16_t  TemperatureSensor2;       // byte [203:202] Current temperature reported by temperature sensor 2.
    uint16_t  TemperatureSensor3;       // byte [205:204] Current temperature reported by temperature sensor 3.
    uint16_t  TemperatureSensor4;       // byte [207:206] Current temperature reported by temperature sensor 4.
    uint16_t  TemperatureSensor5;       // byte [209:208] Current temperature reported by temperature sensor 5.
    uint16_t  TemperatureSensor6;       // byte [211:210] Current temperature reported by temperature sensor 6.
    uint16_t  TemperatureSensor7;       // byte [213:212] Current temperature reported by temperature sensor 7.
    uint16_t  TemperatureSensor8;       // byte [215:214] Current temperature reported by temperature sensor 8.

    uint32_t  TMT1TransitionCount;      // byte [219:216] Thermal Management Temperature 1 Transition Count
    uint32_t  TMT2TransitionCount;      // byte [223:220] Thermal Management Temperature 2 Transition Count
    uint32_t  TMT1TotalTime;            // byte [227:224] Total Time For Thermal Management Temperature 1
    uint32_t  TMT2TotalTime;            // byte [231:228] Total Time For Thermal Management Temperature 2

    uint8_t   Reserved1[280];           // byte [511:232]

} NVME_HEALTH_INFO_LOG13, *PNVME_HEALTH_INFO_LOG13;

int iNVMeGetSMART(HANDLE _hDevice);
