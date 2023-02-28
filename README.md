# nvmetool-win: Sample program of accessing NVMe device using Windows' inbox NVMe driver

![build test](https://github.com/ken-yossy/nvmetool-win/actions/workflows/msbuild.yml/badge.svg)

## Abstract
This software demonstrates that issuing some NVMe commands from userland to NVMe device using Windows inbox NVMe driver (stornvme.sys)[1].

Specification that this software refered to is NVMe 1.3d[2] and NVMe 1.4b[6].

Some fields introduced in NVMe Base Specification revision 2.0[10] are supported.

For StorNVMe on Windows 10 version 1903 and later, Supported NVMe Admin and NVM command sets and associated opcodes are shown in [8], and supported features are shown in [9].

## Note for this software
This software is a SAMPLE and DEMONSTRATION program to show how to access NVMe drive with Windows' inbox NVMe driver.

It is **NOT** intended to provide versatile tool with such functions like accepting arbitrary value for parameters, file input / output, non-interactive mode, support for vendor specific commands.

If you need such functions, you can create your own tool based on this software.

Thank you.

## Command matrix

Table 1. Command matrix (Admin Command Set)

Legend for "Status" column: "x" => can issue, "--" => cannot issue, " "(_blank_) => not confirmed

|     Opcode | Command name                | Status      | Note              |
| ---------: | :---------------------------|:------------|:------------------|
|        00h | Delete I/O Submission Queue |             | 1                 |
|        01h | Create I/O Submission Queue |             | 1                 |
|        02h | Get Log Page                | See Table 4 |                   |
|        04h | Delete I/O Completion Queue |             | 1                 |
|        05h | Create I/O Completion Queue |             | 1                 |
|        06h | Identify                    | See Table 3 |                   |
|        08h | Abort                       |             | 1                 |
|        09h | Set Features                | See Table 5 |                   |
|        0Ah | Get Features                | See Table 5 |                   |
|        0Ch | Asynchronous Event Request  |             | 1                 |
|        0Dh | Namespace Management        |             | 2                 |
|        10h | Firmware Commit             |             |                   |
|        11h | Firmware Image Download     |             |                   |
|        14h | Device Self-test            | x           |                   |
|        15h | Namespace Attachment        |             | 2                 |
|        18h | Keep Alive                  |             | Not listed in [8] |
|        19h | Directive Send              |             | 1                 |
|        1Ah | Directive Receive           |             | 1                 |
|        1Ch | Virtualization Management   |             | Not supported     |
|        1Dh | NVMe-MI Send                |             | 2                 |
|        1Eh | NVMe-MI Receive             |             | 2                 |
|        20h | Capacity Management         |             | _from 2.0_        |
|        24h | Lockdown                    |             | _from 2.0_        |
|        7Ch | Doorbell Buffer Config      |             | Not supported     |
|        80h | Format NVM                  | x           | Only for data drive. Need to support Format NVM command and cryptographic erase is supported along Format NVM (bit 2 of the FNA field in Identify Controller data structure) |
|        81h | Security Send               |             |                   |
|        82h | Security Receive            | x           | Getting TCG Level 0 Discovery Data for example in this tool |
|        84h | Sanitize                    |             | 2                 |
|        86h | Get LBA Status              |             | Not supported     |
| C0h -- FFh | _Vendor Specific_           |             | Sample (Template) code exists |

Note 1: probably cannot issue this command because only "Internal Driver Use" is written in [8]

Note 2: can issue this command only in WinPE mode according to [8]

Note 3: To use sample (template) code for issuing Vendor Specific Commands, you need to edit the codes as you want to do

Table 2. Command matrix (NVM Command Set)

Legend for "Status" column: "x" => can issue, "--" => cannot issue, " "(_blank_) => not confirmed

|     Opcode | Command name         | Status | Note                             |
| ---------: | :--------------------|:-------|:---------------------------------|
|        00h | Flush                | x      |                                  |
|        01h | Write                | x      |                                  |
|        02h | Read                 | x      |                                  |
|        04h | Write Uncorrectable  |        | 1                                |
|        05h | Compare              |        | Can issue only in WinPE mode [8] |
|        08h | Write Zeroes         |        | 1                                |
|        09h | Dataset Management   | x      | Only `Deallocate` is supported   |
|        0Ch | Verify               |        | 1                                |
|        0Dh | Reservation Register |        | 1                                |
|        0Eh | Reservation Report   |        | 1                                |
|        11h | Reservation Acquire  |        | 1                                |
|        15h | Reservation Release  |        | 1                                |
|        19h | Copy                 |        | _from 2.0_                       |
| 80h -- FFh | _Vendor Specific_    |        | Sample (Template) code exists    |

Note 1: these command are stated as "Not supported" in [8]

Note 2: To use sample (template) code for issuing Vendor Specific Commands, you need to edit the codes as you want to do

## Controller or Namespace Structure (CNS) for Identify command

CNS values from 10h to 13h are seemed to be able to issue only in WinPE mode, because these values are mandatory for controllers that support the Namespace Management capability in NVMe spec., but Namespace Management command can only be issued in WinPE mode according to [8].

CNS values 14h and 15h are seemed to be unable to issue, because these values are mandatory for controllers that support Virtualization Enhancements in NVMe spec., but Virtualization Management command is not supported according to [8].

Table 3. Controller or Namespace Structure (CNS)

Legend for "Status" column: "x" => can issue, "--" => cannot issue, " "(_blank_) => not confirmed

|        CNS | Description                                         |Status| Note |
| ---------: | :---------------------------------------------------|:-----|:-----|
|        00h | Namespace                                           | x    | NSID = 1 |
|        01h | Controller                                          | x    | CNTID = 0 |
|        02h | Active Namespace ID list                            |      | we can't specify NSID... and seems no data is returned... |
|        03h | Namespace Identification Descriptor list            | x    | |
|        04h | NVM Set list                                        |      | |
|        05h | I/O Command Set specific Identify Namespace data structure | x | NSID = 1 |
|        06h | I/O Command Set specific Identify Controller data structure || CNTID = 0 |
|        07h | Active Namespace ID list associated with the specified I/O Command Set |||
|        08h | I/O Command Set Independent Identify Namespace data structure || NSID = 1 |
| 09h -- 0Fh | _Reserved_                                          |      | |
|        10h | Allocated Namespace ID list                         |      | Probably can issue only in WinPE mode [8]            |
|        11h | Specified allocated namespace                       |      | Probably can issue only in WinPE mode [8]            |
|        12h | List of controllers attached to specified namespace |      | Probably can issue only in WinPE mode [8]            |
|        13h | List of controllers exist in the NVM subsystem      |      | Probably can issue only in WinPE mode [8]            |
|        14h | Primary Controller Capabilities                     |      | Probably cannot issue [8]                            |
|        15h | Secondary controller list                           |      | Probably cannot issue [8]                            |
|        16h | Namespace Granularity List                          |      | |
|        17h | UUID List                                           |      | |
|        18h | Domain List                                         |      | |
|        19h | Endurance Group List                                |      | |
|        1Ah | I/O Command Set specific Allocated Namespace ID list |     | |
|        1Bh | I/O Command Set specific Identify Namespace data structure |||
|        1Ch | I/O Command Set data structure                      |      | |
| 1Dh -- 1Fh | _Reserved_                                          |      | Spec says CNS "18h to 1Fh" are reserved, I think it's typo. |
| 20h -- FFh | _Reserved_                                          |      | |

## Log Identifier (LID) for Get Log Page command

Table 4. Log Identifier (LID)

Legend for "Status" column: "x" => can issue, "--" => cannot issue, " "(_blank_) => not confirmed

|        LID | Description                    |Status| Note |
| ---------: | :----------------------------- |:-----|:-----|
|        00h | Supported Log Pages            | --   | error code = 1117 (The request could not be performed because of an I/O device error.) |
|        01h | Error Information              | x    | Number of entries to be retrieved from devices depends on the value "ELPE" in Identify Controller data |
|        02h | SMART / Health Information     | x    | CNTID = 0, NSID = 0xFFFFFFFh |
|        03h | Firmware Slot Information      | x    | |
|        04h | Changed Namespace List         |      | |
|        05h | Command Supported and Effects  | x    | |
|        06h | Device Self-test               | x    | |
|        07h | Telemetry Host-initiated       | x    | Getting header only. see also Note 1. |
|        08h | Telemetry Controller-initiated | x    | Getting header only. |
|        09h | Endurance Group Information    | x    | |
|        0Ah | Predictable Latency Per NVM Set |     | |
|        0Bh | Predictable Latency Event Aggregate | | |
|        0Ch | Asymmetric Namespace Access    |      | |
|        0Dh | Persistent Event Log           |      | |
|        0Eh | LBA Status Information         |      | |
|        0Fh | Endurance Group Event Aggregate | --   | error code = 1117 (The request could not be performed because of an I/O device error.) |
|        10h | Media Unit Status              |      | |
|        11h | Supported Capacity Configuration List |||
|        12h | Feature Identifiers Supported and Effects |||
|        13h | NVMe-MI Commands Supported and Effects |||
|        14h | Command and Feature Lockdown   |      | |
|        15h | Boot Partition                 |      | |
|        16h | Rotational Media Information   |      | |
| 17h -- 6Fh | _Reserved_                     |      | |
|        70h | Discovery                      |      | For NVMe over Fabrics (NVMeoF) |
| 71h -- 7Fh | _Reserved for NVMeoF_          |      | |
|        80h | Reservation Notification       |      | Probably cannot issue [8] |
|        81h | Sanitize Status                |      | Probably can issue only in WinPE mode [8] |
| 82h -- BFh | _Reserved_                     |      | |
| C0h -- FFh | _Vendor Specific_              |      | |

Note 1: If you want to set `Create Telemetry Host-Initiated Data` to `1`, you need Windows 10 version 2004 (build 10041) or later. And to do this, this tool uses `IOCTL_STORAGE_GET_DEVICE_INTERNAL_LOG` described in [9]. But I don't know products (SSDs) that can respond this type of request (command)...

## Feature Identifier (FID) for Get/Set Features command

According to [8], only Host Controlled Thermal Management is supported via Set Features command with `IOCTL_STORAGE_SET_PROPERTY`.

Table 5. Feature Identifier (FID)

Legend for "Status" column: "x" => can issue, "--" => cannot issue, " "(_blank_) => not confirmed

|        FID | Description                        | Status (Get/Set) | Note |
| ---------: | :----------------------------------|:-----------------|:-----|
|        01h | Arbitration                        | x /              |      |
|        02h | Power Management                   | x /              |      |
|        03h | LBA Range Type                     | -- /             | error code = 1117 (The request could not be performed because of an I/O device error.) |
|        04h | Temperature Threashold             | x /              |      |
|        05h | Error Recovery                     | x /              |      |
|        06h | Volatile Write Cache               | x / --           | error code = 1117 (The request could not be performed because of an I/O device error.) |
|        07h | Number of Queues                   | -- /             | error code = 1117 (The request could not be performed because of an I/O device error.) |
|        08h | Interrupt Coalescing               | x /              |      |
|        09h | Interrupt Vector Configuration     |  /               |      |
|        0Ah | Write Atomicity Normal             | x /              |      |
|        0Bh | Asynchronous Event Configuration   | x /              |      |
|        0Ch | Autonomous Power State Transition  | -- /             | error code = 1117 (The request could not be performed because of an I/O device error.) |
|        0Dh | Host Memory Buffer                 | -- /             | error code = 1117 (The request could not be performed because of an I/O device error.) |
|        0Eh | Timestamp                          | -- /             | error code = 1117 (The request could not be performed because of an I/O device error.) |
|        0Fh | Keep Alive Timer                   |  /               |      |
|        10h | Host Controlled Thermal Management | x / x            | "SV = 1" is not supported |
|        11h | Non-operational Power State Config | x /              |      |
|        12h | Read Recovery Level Config         |  /               |      |
|        13h | Predictable Latency Mode Config    |  /               |      |
|        14h | Predictable Latency Mode Window    |  /               |      |
|        15h | LBA Status Information Report Interval |  /           |      |
|        16h | Host Behavior Support              |  /               |      |
|        17h | Sanitize Config                    |  /               |      |
|        18h | Endurance Group Event Configuration |  /              |      |
|        19h | I/O Command Set Profile            | /                |      |
|        1Ah | Spinup Control                     | /                |      |
| 1Bh -- 77h | _Reserved_                         |  /               |      |
| 78h -- 7Ch | _Refer to the NVMe-MI spec._       |  /               |      |
|        7Dh | Enhanced Controller Metadata       |  /               |      |
|        7Eh | Controller Metadata                |  /               |      |
|        7Fh | Namespace Metadata                 |  /               |      |
|        80h | Software Progress Marker           | x /              |      |
|        81h | Host Identifier                    |  /               |      |
|        82h | Reservation Notification Mask      |  /               | 1    |
|        83h | Reservation Persistence            |  /               | 1    |
|        84h | Namespace Write Protect Config     |  /               |      |
| 85h -- BFh | _Reserved_                         |  /               |      |
| C0h -- FFh | _Vendor specific_                  |  /               |      |

Note 1: FID 82h and 83h are seemed to be unable to issue, because commands related to Reservation are not supported according to [8].

## Note for some commands

### Read command
Read command is issued via SCSI Pass-through[3].

Issue Read command with Starting Logical Block Address (SLBA) = 0 and Number of Logical Blocks (NLB) = 1.

### Write command
Write command is issued via SCSI Pass-through[3].

Issue Write command with SLBA = 0, NLB = 1, and data is "DEADBEEF".

### Dataset Management command
Dataset Management command is issued via SCSI Pass-through[3] with `IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES` [4][5] (only Deallocate bit is set).

LBA region to be deallocated is SLBA = 0 and NLB = 1 (Sector 0).

An NVMe drive that supports Dataset Management command is also needed.

Supporting Dataset Management command can be checked by bit 2 of "Optional NVM Command Support (ONCS)" field in Identify Controller Data Structure.

If the bit is set to 1, the controller support Dataset Management command.

For further information, see "5.15.3 Identify Controller data structure (CNS 01h)" in the NVMe specification[2]. 

An NVMe drive that return fixed data pattern for reading deallocated logical block is recommended for checking the result of deallocation.

It can be checked by bit 2:0 of "Deallocate Logical Block Features (DLFEAT)" field in Identify Namespace Data Structure.

If it is 001b, the drive returns all bytes cleared to 00h for reading deallocated logical block.

If it is 010b, the drive returns all bytes set to FFh for reading deallocated logical block.

For further information, see "5.15.2 Identify Namespace data structure (CNS 00h)" in the NVMe specification[2]. 

## Caution
Issuing "Write", "Dataset Management (Deallocate)", and "Format NVM" command to devices under management by file systems (e.g. FAT, NTFS) will corrupt your system.

These commands should be issued with highest care.

"Read" command does not have such a side effect... :-) 

## Environment
Confirmed on the following software environment:

* Tested operating system and device driver (latest)
  * Windows 11 Pro 64bit (Version 22H2, Build 22621.963)
    * stornvme.sys (version 10.0.22621.755, WinBuild 160101.0800)
* Developing environment
  * Microsoft Visual Studio Community 2022 (Version 17.4.3)
    * Windows SDK 10.0.22621.0
    * Windows Driver Kit 10.0.22605.1

## Limitations
Only tested with the NVMe drive directly attached to PC via PCIe.

It is ok to access to M.2 drives and M.2 drives that attached to M.2-PCIe converter Add-In-Card (AIC).

Also, it may be ok with U.2 drives.

But it may not work over protocol translations such as usb-nvme.

## To build and run program

1. Double click `NVMeTool.sln` to start Visual Studio
1. Build program with _Build_ -> _Build Solution_ (or Control+Shift+B)
1. Run program (F5)

Note that, when you run (debug) in Visual Studio, confirm that the command line argument is set to appropriate drive ID for your system.

You can find the settings of command line arguments from _Debug_ menu -> _NVMeTool Debug Properties_ -> _Configuration Properties_ -> _Debugging_ -> _Command Arguments_.

## Future Works
* Test commands, LIDs, FIDs, and CNSs in the NVMe specification and listed in [8]
* Test in WinPE mode

## License
This software is released under the MIT License and Microsoft Public License (MS-PL), see LICENSE.

## Reference
[1] Microsoft, _"Working with NVMe drives"_, https://docs.microsoft.com/en-us/windows/desktop/fileio/working-with-nvme-devices, Retrieved May 14, 2019

[2] NVM Express, _"NVM Express\[TM\] Base Specification"_, Revision 1.3d, March 20, 2019

[3] Microsoft, _"SCSI Pass-Through Interface Tool"_, https://github.com/microsoft/Windows-driver-samples/tree/master/storage/tools/spti, Retrieved May 14, 2019.

[4] Mark E. Russinovich, et al., _"Windows Internals, Part 2"_, 6th Edition, October 2012, Microsoft Press, ISBN: 978-0735665873

[5] Microsoft, _"IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES IOCTL"_, https://docs.microsoft.com/ja-jp/windows/desktop/api/winioctl/ni-winioctl-ioctl_storage_manage_data_set_attributes, Retrieved May 14, 2019

[6] NVM Express, _"NVM Express\[TM\] Base Specification"_, Revision 1.4b, Sept. 21, 2020

[7] Microsoft, _"NVMe Features Supported by StorNVMe"_, https://docs.microsoft.com/en-us/windows-hardware/drivers/storage/nvme-features-supported-by-stornvme, Retrieved June 25, 2020

[8] Microsoft, _"StorNVMe Command Set Support"_, https://docs.microsoft.com/en-us/windows-hardware/drivers/storage/stornvme-command-set-support, Retrieved June 25, 2020

[9] Microsoft, _"NVMe Features Supported by StorNVMe"_, https://docs.microsoft.com/en-us/windows-hardware/drivers/storage/stornvme-feature-support, Retrieved June 29, 2020

[10] NVM Express, _"NVM Express\[TM\] Base Specification"_, Revision 2.0, June 2, 2021
