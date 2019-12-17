# nvmetool-win: Sample program of accessing NVMe device using Windows' inbox NVMe driver

## Abstract
This software demonstrates that issuing some NVMe commands from userland to NVMe device using Windows inbox NVMe driver (stornvme.sys)[1].

Specification that this software refered to is NVMe 1.3d[2] and NVMe 1.4[6] (only Identify Controller Data Structure).

## ChangeLog
See Changelog.md

## Supported commands (Admin Command Set)

### Identify (Opcode = 06h)

| CNS Value | Description               | Note                            |
| --------: | :-------------------------|:--------------------------------|
|       00h | Namespace data structure  | Namespace Identifier (NSID) = 1 |
|       01h | Controller data structure |                                 |

### Get Log Page (Opcode = 02h)

| Log Identifier | Description                   | Note |
| -------------: | :---------------------------- |:-----|
|            01h | Error Information             |      |
|            02h | SMART / Health Information    |      |
|            05h | Command Supported and Effects |      |

### Get Features (Opcode = 0Ah)

| Feature Identifier | Description                        | Note                                                      |
| -----------------: | :--------------------------------- |:----------------------------------------------------------|
|                01h | Arbitration                        |                                                           |
|                02h | Power Management                   |                                                           |
|                04h | Temperature Threashold             |                                                           |
|                05h | Error Recovery                     |                                                           |
|                06h | Volatile Write Cache               |                                                           |
|                07h | Number of Queues                   | DeviceIoControl returns error code 1117 (ERROR_IO_DEVICE) |
|                08h | Interrupt Coalescing               |                                                           |
|                0Ah | Write Atomicity Normal             |                                                           |
|                0Bh | Asynchronous Event Configuration   |                                                           |
|                10h | Host Controlled Thermal Management |                                                           |
|                80h | Software Progress Marker           | DeviceIoControl returns error code 1117 (ERROR_IO_DEVICE) |

## Supported commands (NVM Command Set)
Read and Write command are issued via SCSI Pass-through[3].

Dataset Management command (Deallocate) is issued with `IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES` [4][5].

### Read (Opcode = 02h)

Starting Logical Block Address (SLBA) = 0, Number of Logical Blocks (NLB) = 1

### Write (Opcode = 01h)

SLBA = 0, NLB = 1

### Dataset Management (Opcode = 09h)

Only with Deallocate bit.

LBA region to be deallocated is SLBA = 0, NLB = 1

## Caution
"Write" and "Dataset Management (Deallocate)" command to LBA 0 of devices under management by file systems (e.g. FAT, NTFS) will corrupt your system.

These commands should be issued with highest care.

"Read" command does not have such a side effect... :-) 

## Environment / Requirements
Confirmed on the following software environment:

* Operating system and device driver
  * Windows 10 Professional 64bit (Version 1909, Build 18363.535)
  * stornvme.sys (version 10.0.18362.1, WinBuild 160101.0800)
* Developping environment
  * Microsoft Visual Studio Community 2019 (Version 16.4.1)
    * Microsoft Visual C++ 2019
    * Windows Driver Kit (10.0.18346.1000)

An NVMe drive that supports Dataset Management command is also needed.

Supporting Dataset Management command can be checked by bit 2 of "Optional NVM Command Support (ONCS)" field in Identify Controller Data Structure.

If the bit is set to 1, the controller support Dataset Management command.

For further information, see "5.15.3 Identify Controller data structure (CNS 01h)" in the NVMe specification[2]. 

An NVMe drive that return fixed data pattern for reading deallocated logical block is recommended for checking the result of deallocation.

It can be checked by bit 2:0 of "Deallocate Logical Block Features (DLFEAT)" field in Identify Namespace Data Structure.

If it is 001b, the drive returns all bytes cleared to 00h for reading deallocated logical block.

If it is 010b, the drive returns all bytes set to FFh for reading deallocated logical block.

For further information, see "5.15.2 Identify Namespace data structure (CNS 00h)" in the NVMe specification[2]. 

## To build and run program

1. Double click `NVMeTool.sln` to start Visual Studio
1. Build program with _Build_ -> _Build Solution_ (or Control+Shift+B)
1. Run program (F5)

## Future Works
* Try other commands, LIDs, FIDs, and CNSs in the NVMe specification
* Enable to issue Read / Write / Dataset Management (Deallocate) command with arbitrary SLBA and NLB
* Discovery of storage devices in the system
* Try latest NVMe driver (ex. the one shipped with Windows 10 May 2019 Update (build 18343))

## License
This software is released under the MIT License and Microsoft Public License (MS-PL), see LICENSE.

## Reference
[1] Microsoft, _"Working with NVMe drives"_, https://docs.microsoft.com/en-us/windows/desktop/fileio/working-with-nvme-devices, Retrieved May 14, 2019

[2] NVM Express, _"NVM Express\[TM\] Base Specification"_, Revision 1.3d, March 20, 2019

[3] Microsoft, _"SCSI Pass-Through Interface Tool"_, https://github.com/microsoft/Windows-driver-samples/tree/master/storage/tools/spti, Retrieved May 14, 2019.

[4] Mark E. Russinovich, et al., _"Windows Internals, Part 2"_, 6th Edition, October 2012, Microsoft Press, ISBN: 978-0735665873

[5] Microsoft, _"IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES IOCTL"_, https://docs.microsoft.com/ja-jp/windows/desktop/api/winioctl/ni-winioctl-ioctl_storage_manage_data_set_attributes, Retrieved May 14, 2019

[6] NVM Express, _"NVM Express\[TM\] Base Specification"_, Revision 1.4, June 10, 2019
