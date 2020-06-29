# nvmetool-win: Sample program of accessing NVMe device using Windows' inbox NVMe driver

## Abstract
This software demonstrates that issuing some NVMe commands from userland to NVMe device using Windows inbox NVMe driver (stornvme.sys)[1].

Specification that this software refered to is NVMe 1.3d[2] and NVMe 1.4[6] (only Identify Controller Data Structure).

For StorNVMe on Windows 10 version 1903 and later, Supported NVMe Admin and NVM command sets and associated opcodes are shown in [8], and supported features are shown in [9].

## Command matrix

Table 1. Command matrix (Admin Command Set)

|     Opcode | Command name                | Confirmed in this tool          | Note              |
| ---------: | :---------------------------|:--------------------------------|:------------------|
|        00h | Delete I/O Submission Queue | --                              | 1                 |
|        01h | Create I/O Submission Queue | --                              | 1                 |
|        02h | Get Log Page                | See Table 4                     |                   |
|        04h | Delete I/O Completion Queue | --                              | 1                 |
|        05h | Create I/O Completion Queue | --                              | 1                 |
|        06h | Identify                    | See Table 3                     |                   |
|        08h | Abort                       | --                              | 1                 |
|        09h | Set Features                | See Table 5                     |                   |
|        0Ah | Get Features                | See Table 5                     |                   |
|        0Ch | Asynchronous Event Request  | --                              | 1                 |
|        0Dh | Namespace Management        |                                 | 2                 |
|        10h | Firmware Commit             |                                 |                   |
|        11h | Firmware Image Download     |                                 |                   |
|        14h | Device Self-test            |                                 |                   |
|        15h | Namespace Attachment        |                                 | 2                 |
|        18h | Keep Alive                  | --                              | Not listed in [8] |
|        19h | Directive Send              | --                              | 1                 |
|        1Ah | Directive Receive           | --                              | 1                 |
|        1Ch | Virtualization Management   | --                              | Not supported     |
|        1Dh | NVMe-MI Send                |                                 | 2                 |
|        1Eh | NVMe-MI Receive             |                                 | 2                 |
|        7Ch | Doorbell Buffer Config      | --                              | Not supported     |
|        80h | Format NVM                  |                                 |                   |
|        81h | Security Send               |                                 |                   |
|        82h | Security Receive            |                                 |                   |
|        84h | Sanitize                    |                                 | 2                 |
|        86h | Get LBA Status              | --                              | Not supported     |
| C0h -- FFh | _Vendor Specific_           | --                              |                   |

Note 1: probably cannot issue this command because only "Internal Driver Use" is written in [8]
Note 2: can issue this command only in WinPE mode according to [8]

Table 2. Command matrix (NVM Command Set)

|     Opcode | Command name         | Confirmed in this tool          | Note (according to [8])        |
| ---------: | :--------------------|:--------------------------------|:-------------------------------|
|        00h | Flush                |                                 |                                |
|        01h | Write                | x                               |                                |
|        02h | Read                 | x                               |                                |
|        04h | Write Uncorrectable  | --                              | Not supported                  |
|        05h | Compare              |                                 | Can issue only in WinPE mode   |
|        08h | Write Zeroes         | --                              | Not supported                  |
|        09h | Dataset Management   | x                               | Only `Deallocate` is supported |
|        0Ch | Verify               | --                              | Not supported                  |
|        0Dh | Reservation Register | --                              | Not supported                  |
|        0Eh | Reservation Report   | --                              | Not supported                  |
|        11h | Reservation Acquire  | --                              | Not supported                  |
|        15h | Reservation Release  | --                              | Not supported                  |
| 80h -- FFh | _Vendor Specific_    | --                              |                                |

## Controller or Namespace Structure (CNS) for Identify command

CNS values from 10h to 13h are seemed to be able to issue only in WinPE mode, because these values are mandatory for controllers that support the Namespace Management capability in NVMe spec., but Namespace Management command can only be issued in WinPE mode according to [8].

CNS values 14h and 15h are seemed to be unable to issue, because these values are mandatory for controllers that support Virtualization Enhancements in NVMe spec., but Virtualization Management command is not supported according to [8].

Table 3. Controller or Namespace Structure (CNS)

|        CNS | Description                                         | Confirmied in this tool | Note                                                 |
| ---------: | :---------------------------------------------------|:------------------------|:-----------------------------------------------------|
|        00h | Namespace                                           | x                       | Target Namespace Identifier (NSID) is hardcoded as 1 |
|        01h | Controller                                          | x                       |                                                      |
|        02h | Active Namespace ID list                            |                         |                                                      |
|        03h | Namespace Identification Descriptor list            |                         |                                                      |
| 04h -- 0Fh | _Reserved_                                          | --                      |                                                      |
|        10h | Allocated Namespace ID list                         |                         | Probably can issue only in WinPE mode [8]            |
|        11h | Specified allocated namespace                       |                         | Probably can issue only in WinPE mode [8]            |
|        12h | List of controllers attached to specified namespace |                         | Probably can issue only in WinPE mode [8]            |
|        13h | List of controllers exist in the NVM subsystem      |                         | Probably can issue only in WinPE mode [8]            |
|        14h | Primary Controller Capabilities                     | --                      | Probably cannot issue [8]                            |
|        15h | Secondary controller list                           | --                      | Probably cannot issue [8]                            |
| 16h -- 1Fh | _Reserved_                                          | --                      |                                                      |
| 20h -- FFh | _Reserved_                                          | --                      |                                                      |

## Log Identifier (LID) for Get Log Page command

Table 4. Log Identifier (LID)

|        LID | Description                    | Confirmed in this tool | Note |
| ---------: | :----------------------------- |:-----------------------|:-----|
|        01h | Error Information              | x                      | Number of entries to be retrieved from devices depends on the value "ELPE" in Identify Controller data |
|        02h | SMART / Health Information     | x                      | CNTID = 0, NSID = 0xFFFFFFFh |
|        03h | Firmware Slot Information      | x                      |      |
|        05h | Command Supported and Effects  | x                      |      |
|        06h | Device Self-test               |                        |      |
|        07h | Telemetry Host-initiated       |                        |      |
|        08h | Telemetry Controller-initiated |                        |      |
| 09h -- 6Fh | _Reserved_                     | --                     |      |
|        70h | Discovery                      | --                     | For NVMe over Fabrics (NVMeoF) |
| 71h -- 7Fh | _Reserved for NVMeoF_          | --                     |      |
|        80h | Reservation Notification       | --                     | For NVMe over Fabrics (NVMeoF) |
|        81h | Sanitize Status                | --                     | For NVMe over Fabrics (NVMeoF) |
| 82h -- BFh | _Reserved_                     | --                     |      |
| C0h -- FFh | _Vendor Specific_              | --                     |      |

## Feature Identifier (FID) for Get/Set Features command

According to [8], only Host Controlled Thermal Management is supported via Set Features command with `IOCTL_STORAGE_SET_PROPERTY`.

Table 5. Feature Identifier (FID)

|        FID | Description                        | Confirmed in this tool (Get/Set) | Note |
| ---------: | :----------------------------------|:---------------------------------|:-----|
|        01h | Arbitration                        | x /                              |      |
|        02h | Power Management                   | x /                              |      |
|        03h | LBA Range Type                     |  /                               |      |
|        04h | Temperature Threashold             | x /                              |      |
|        05h | Error Recovery                     | x /                              |      |
|        06h | Volatile Write Cache               | x /                              |      |
|        07h | Number of Queues                   |  /                               |      |
|        08h | Interrupt Coalescing               | x /                              |      |
|        09h | Interrupt Vector Configuration     |  /                               |      |
|        0Ah | Write Atomicity Normal             | x /                              |      |
|        0Bh | Asynchronous Event Configuration   | x /                              |      |
|        0Ch | Autonomous Power State Transition  |  /                               |      |
|        0Dh | Host Memory Buffer                 |  /                               |      |
|        0Eh | Timestamp                          |  /                               |      |
|        0Fh | Keep Alive Timer                   |  /                               |      |
|        10h | Host Controlled Thermal Management | x /                              |      |
|        11h | Non-operational Power State Config |  /                               |      |
| 12h -- 77h | _Reserved_                         | -- / --                          |      |
| 78h -- 7Fh | _Refer to the NVMe-MI spec._       | -- / --                          |      |
|        80h | Software Progress Marker           | x /                              |      |
|        81h | Host Identifier                    |  /                               |      |
|        82h | Reservation Notification Mask      | -- / --                          |      |
|        83h | Reservation Persistence            | -- / --                          |      |
| 84h -- BFh | _Reserved_                         | -- / --                          |      |
| C0h -- FFh | _Vendor specific_                  | -- / --                          |      |

FID 82h and 83h are seemed to be unable to issue, because commands related to Reservation are not supported according to [8].

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
"Write" and "Dataset Management (Deallocate)" command to LBA 0 of devices under management by file systems (e.g. FAT, NTFS) will corrupt your system.

These commands should be issued with highest care.

"Read" command does not have such a side effect... :-) 

## Environment / Requirements
Confirmed on the following software environment:

* Operating system and device driver
  * Windows 10 Professional 64bit (Version 1909, Build 18363.900)
  * stornvme.sys (version 10.0.18362.693, WinBuild 160101.0800)
* Developping environment
  * Microsoft Visual Studio Community 2019 (Version 16.6.2)
    * Microsoft Visual C++ 2019
    * Windows Driver Kit (10.0.19030.1000)

## To build and run program

1. Double click `NVMeTool.sln` to start Visual Studio
1. Build program with _Build_ -> _Build Solution_ (or Control+Shift+B)
1. Run program (F5)

## Future Works
* Test commands, LIDs, FIDs, and CNSs in the NVMe specification and listed in [8]

## License
This software is released under the MIT License and Microsoft Public License (MS-PL), see LICENSE.

## Reference
[1] Microsoft, _"Working with NVMe drives"_, https://docs.microsoft.com/en-us/windows/desktop/fileio/working-with-nvme-devices, Retrieved May 14, 2019

[2] NVM Express, _"NVM Express\[TM\] Base Specification"_, Revision 1.3d, March 20, 2019

[3] Microsoft, _"SCSI Pass-Through Interface Tool"_, https://github.com/microsoft/Windows-driver-samples/tree/master/storage/tools/spti, Retrieved May 14, 2019.

[4] Mark E. Russinovich, et al., _"Windows Internals, Part 2"_, 6th Edition, October 2012, Microsoft Press, ISBN: 978-0735665873

[5] Microsoft, _"IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES IOCTL"_, https://docs.microsoft.com/ja-jp/windows/desktop/api/winioctl/ni-winioctl-ioctl_storage_manage_data_set_attributes, Retrieved May 14, 2019

[6] NVM Express, _"NVM Express\[TM\] Base Specification"_, Revision 1.4, June 10, 2019

[7] Microsoft, _"NVMe Features Supported by StorNVMe"_, https://docs.microsoft.com/en-us/windows-hardware/drivers/storage/nvme-features-supported-by-stornvme, Retrieved June 25, 2020

[8] Microsoft, _"StorNVMe Command Set Support"_, https://docs.microsoft.com/en-us/windows-hardware/drivers/storage/stornvme-command-set-support, Retrieved June 25, 2020

[9] Microsoft, _"NVMe Features Supported by StorNVMe"_, https://docs.microsoft.com/en-us/windows-hardware/drivers/storage/stornvme-feature-support, Retrieved June 29, 2020
