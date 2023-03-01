#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

//
// Information of SMART log: 512 bytes
//
typedef struct {
    struct {
        uint8_t AvailableSpaceLow : 1;                 // bit [    0]
        uint8_t TemperatureThreshold : 1;              // bit [    1]
        uint8_t ReliabilityDegraded : 1;               // bit [    2]
        uint8_t ReadOnly : 1;                          // bit [    3]
        uint8_t VolatileMemoryBackupDeviceFailed : 1;  // bit [    4]
        uint8_t PMRDegraded : 1;                       // bit [    5] <rev1.4>
        uint8_t Reserved : 2;                          // bit [ 6: 7]
    } CriticalWarning;  // byte [      0] Critical Warning

    uint8_t Temperature[2];  // byte [  2:  1] Composite Temperature
    uint8_t AvailableSpare;  // byte [      3] Available Spare
    uint8_t
        AvailableSpareThreshold;  // byte [      4] Available Spare Threshold
    uint8_t PercentageUsed;       // byte [      5] Percentage Used

    struct {
        uint8_t AvailableSpareLow : 1;    // bit [    0]
        uint8_t Reserved1 : 1;            // bit [    1]
        uint8_t ReliabilityDegraded : 1;  // bit [    2]
        uint8_t ReadOnly : 1;             // bit [    3]
        uint8_t Reserved2 : 4;            // bit [ 7: 4]
    } EnduranceGroupSummary;  // byte [      6] Endurance Group Critical Warning
                              // Summary <rev1.4>

    uint8_t Reserved0[25];  // byte [ 31:  7]

    uint64_t DataUnitRead_L;  // byte [ 47: 32] Data Units Read
    uint64_t DataUnitRead_H;
    uint64_t DataUnitWritten_L;  // byte [ 63: 48] Data Units Written
    uint64_t DataUnitWritten_H;
    uint64_t HostReadCommands_L;  // byte [ 79: 64] Host Read Commands
    uint64_t HostReadCommands_H;
    uint64_t HostWrittenCommands_L;  // byte [ 95: 80] Host Write Commands
    uint64_t HostWrittenCommands_H;
    uint64_t ControllerBusyTime_L;  // byte [111: 66] Controller Busy Time
    uint64_t ControllerBusyTime_H;
    uint64_t PowerCycle_L;  // byte [127:112] Power Cycles
    uint64_t PowerCycle_H;
    uint64_t PowerOnHours_L;  // byte [143:128] Power On Hours
    uint64_t PowerOnHours_H;
    uint64_t UnsafeShutdowns_L;  // byte [159:144] Unsafe Shutdowns
    uint64_t UnsafeShutdowns_H;
    uint64_t MediaErrors_L;  // byte [175:160] Media and Data Integrity Errors
    uint64_t MediaErrors_H;
    uint64_t ErrorInfoLogEntryNum_L;  // byte [191:176] Number of Error
                                      // Information Log Entries
    uint64_t ErrorInfoLogEntryNum_H;
    uint32_t WCTempTime;  // byte [195:192] Warning Composite Temperature Time
    uint32_t CCTempTime;  // byte [196:196] Critical Composite Temperature Time

    uint16_t TemperatureSensor1;  // byte [201:200] Current temperature reported
                                  // by temperature sensor 1.
    uint16_t TemperatureSensor2;  // byte [203:202] Current temperature reported
                                  // by temperature sensor 2.
    uint16_t TemperatureSensor3;  // byte [205:204] Current temperature reported
                                  // by temperature sensor 3.
    uint16_t TemperatureSensor4;  // byte [207:206] Current temperature reported
                                  // by temperature sensor 4.
    uint16_t TemperatureSensor5;  // byte [209:208] Current temperature reported
                                  // by temperature sensor 5.
    uint16_t TemperatureSensor6;  // byte [211:210] Current temperature reported
                                  // by temperature sensor 6.
    uint16_t TemperatureSensor7;  // byte [213:212] Current temperature reported
                                  // by temperature sensor 7.
    uint16_t TemperatureSensor8;  // byte [215:214] Current temperature reported
                                  // by temperature sensor 8.

    uint32_t TMT1TransitionCount;  // byte [219:216] Thermal Management
                                   // Temperature 1 Transition Count <rev1.3>
    uint32_t TMT2TransitionCount;  // byte [223:220] Thermal Management
                                   // Temperature 2 Transition Count <rev1.3>
    uint32_t TMT1TotalTime;  // byte [227:224] Total Time For Thermal Management
                             // Temperature 1 <rev1.3>
    uint32_t TMT2TotalTime;  // byte [231:228] Total Time For Thermal Management
                             // Temperature 2 <rev1.3>

    uint8_t Reserved1[280];  // byte [511:232]

} NVME_SMART_INFO_LOG, *PNVME_SMART_INFO_LOG;

extern NVME_SMART_INFO_LOG g_stSMARTLog;

int iNVMeGetSMART(HANDLE _hDevice, bool _bPrint, int iNSID);
