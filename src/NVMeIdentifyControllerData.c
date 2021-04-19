#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <nvme.h>

#include "NVMeUtils.h"
#include "NVMeIdentifyController.h"

static void s_vPrintNVMeIdentifyControllerDataCMIC(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC):\n");

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        if (_pstController->CMIC.ANAReport)
        {
            printf("\tbit [      3] 1 = Supports Asymmetric Namespace Access Reporting\n");
        }
        else
        {
            printf("\tbit [      3] 0 = Does not support Asymmetric Namespace Access Reporting\n");
        }
    }

    if (_pstController->CMIC.SRIOV)
    {
        printf("\tbit [      2] 1 = Associated with an SR-IOV Virtual Function\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Associated with a PCI Function or a Fabrics connection\n");
    }

    if (_pstController->CMIC.MultiControllers)
    {
        printf("\tbit [      1] 1 = May contain two or more controllers\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Contains only a single controller\n");
    }

    if (_pstController->CMIC.MultiPCIePorts)
    {
        printf("\tbit [      0] 1 = May contain more than one NVM subsystem port\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Contains only a single NVM subsystem port\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataOAES(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Optional Asynchronous Events Supported (OAES):\n");

    if (bIsNVMeV20OrLater())
    { // revision 2.0 or later
        if (_pstController->OAES.DiscoveryLogChanged)
        {
            printf("\tbit [     31] 1 = Supports Discovery Log Change Notification event\n");
        }
        else
        {
            printf("\tbit [     31] 0 = Does not support Discovery Log Change Notification event\n");
        }

        if (_pstController->OAES.ZoneDesciptorChanged)
        {
            printf("\tbit [     27] 1 = Supports Zone Descriptor Changed Notices event\n");
        }
        else
        {
            printf("\tbit [     27] 0 = Does not support Zone Descriptor Changed Notices event\n");
        }

        if (_pstController->OAES.NVMSubsystemShutdown)
        {
            printf("\tbit [     15] 1 = Supports Normal NVM Subsystem Shutdown event\n");
        }
        else
        {
            printf("\tbit [     15] 0 = Does not support Normal NVM Subsystem Shutdown event\n");
        }
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        if (_pstController->OAES.EnduranceGroupEventAggregateLogChange)
        {
            printf("\tbit [     14] 1 = Supports Endurance Group Event Aggregate Log Page Change Notices event\n");
        }
        else
        {
            printf("\tbit [     14] 0 = Does not support Endurance Group Event Aggregate Log Page Change Notices event\n");
        }

        if (_pstController->OAES.LBAStatusInfo)
        {
            printf("\tbit [     13] 1 = Supports LBA Status Information Notices event\n");
        }
        else
        {
            printf("\tbit [     13] 0 = Does not support LBA Status Information Notices event\n");
        }

        if (_pstController->OAES.PredictableLatencyEventAggregateLogChange)
        {
            printf("\tbit [     12] 1 = Supports Predictable Latency Event Aggregate Log Change Notices event\n");
        }
        else
        {
            printf("\tbit [     12] 0 = Does not support Predictable Latency Event Aggregate Log Change Notices event\n");
        }

        if (_pstController->OAES.ANAChange)
        {
            printf("\tbit [     11] 1 = Supports Asymmetric Namespace Access Change Notices event\n");
        }
        else
        {
            printf("\tbit [     11] 0 = Does not support Asymmetric Namespace Access Change Notices event\n");
        }
    }

    if (_pstController->OAES.FirmwareActivation)
    {
        printf("\tbit [      9] 1 = Supports Firmware Activation Notices event\n");
    }
    else
    {
        printf("\tbit [      9] 0 = Does not support Firmware Activation Notices event\n");
    }

    if (_pstController->OAES.NamespaceAttributeChanged)
    {
        printf("\tbit [      8] 1 = Supports Namespace Attribute Notices and associated Changed Namespace List log page\n");
    }
    else
    {
        printf("\tbit [      8] 0 = Does not support Namespace Attribute Notices event nor associated Changed Namespace List log page\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataCTRATT(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Controller Attributes (CTRATT):\n");

    if (bIsNVMeV20OrLater())
    { // revision 2.0 or later
        if (_pstController->CTRATT.ExtLBAFmt)
        {
            printf("\tbit [     15] 1 = Supports extended PI formats\n");
        }
        else
        {
            printf("\tbit [     15] 0 = Does not support extended PI formats\n");
        }
        
        if (_pstController->CTRATT.DelNVMSet)
        {
            printf("\tbit [     14] 1 = Supports Delete NVM Set operation\n");
        }
        else
        {
            printf("\tbit [     14] 0 = Does not support Delete NVM Set operation\n");
        }

        if (_pstController->CTRATT.DelEndGrp)
        {
            printf("\tbit [     13] 1 = Supports Delete Endurance Group operation\n");
        }
        else
        {
            printf("\tbit [     13] 0 = Does not support Delete Endurance Group operation\n");
        }

        if (_pstController->CTRATT.VariableCapMgmt)
        {
            printf("\tbit [     12] 1 = Supports Variable Capacity Management\n");
        }
        else
        {
            printf("\tbit [     12] 0 = Does not support Variable Capacity Management\n");
        }

        if (_pstController->CTRATT.FixedCapMgmt)
        {
            printf("\tbit [     11] 1 = Supports Fixed Capacity Management\n");
        }
        else
        {
            printf("\tbit [     11] 0 = Does not support Fixed Capacity Management\n");
        }

        if (_pstController->CTRATT.MDS)
        {
            printf("\tbit [     10] 1 = Supports multiple domains\n");
        }
        else
        {
            printf("\tbit [     10] 0 = Does not support multiple domains\n");
        }
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        if (_pstController->CTRATT.UUIDList)
        {
            printf("\tbit [      9] 1 = Supports reporting of a UUID List\n");
        }
        else
        {
            printf("\tbit [      9] 0 = Does not support reporting of a UUID List\n");
        }

        if (_pstController->CTRATT.SQAssociation)
        {
            printf("\tbit [      8] 1 = Supports SQ Associations\n");
        }
        else
        {
            printf("\tbit [      8] 0 = Does not support SQ Associations\n");
        }

        if (_pstController->CTRATT.NamespaceGranularity)
        {
            printf("\tbit [      7] 1 = Supports reporting of Namespace Granularity\n");
        }
        else
        {
            printf("\tbit [      7] 0 = Does not support reporting of Namespace Granularity\n");
        }

        if (_pstController->CTRATT.TrafficBasedKeepAlive)
        {
            printf("\tbit [      6] 1 = Restart Keep Alive Timer if an Admin command or an I/O command is processed during Keep Alive Timeout Interval\n");
        }
        else
        {
            printf("\tbit [      6] 0 = Restart Keep Alive Timer only if a Keep Alive command is processed during Keep Alive Timeout Interval\n");
        }

        if (_pstController->CTRATT.PredictableLatencyMode)
        {
            printf("\tbit [      5] 1 = Supports Predictable Latency Mode\n");
        }
        else
        {
            printf("\tbit [      5] 0 = Does not support Predictable Latency Mode\n");
        }

        if (_pstController->CTRATT.EnduranceGroups)
        {
            printf("\tbit [      4] 1 = Supports Endurance Groups\n");
        }
        else
        {
            printf("\tbit [      4] 0 = Does not support Endurance Groups\n");
        }

        if (_pstController->CTRATT.ReadRecoveryLevel)
        {
            printf("\tbit [      3] 1 = Supports Read Recovery Levels\n");
        }
        else
        {
            printf("\tbit [      3] 0 = Does not support Read Recovery Levels\n");
        }

        if (_pstController->CTRATT.NVMSet)
        {
            printf("\tbit [      2] 1 = Supports NVM Sets\n");
        }
        else
        {
            printf("\tbit [      2] 0 = Does not support NVM Sets\n");
        }
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.3 or over
        if (_pstController->CTRATT.NoopPSPermissiveModeEn)
        {
            printf("\tbit [      1] 1 = Supports host control of whether the controller may temporarily exceed the power of a non-operational power state for the purpose of executing controller initiated background operations in a non-operational power state\n");
        }
        else
        {
            printf("\tbit [      1] 0 = Does not support host control of whether the controller may exceed the power of a non-operational state for the purpose of executing controller initiated background operations in a non-operational state\n");
        }
    }

    if (_pstController->CTRATT.HostIdEn)
    {
        printf("\tbit [      0] 1 = Supports a 128-bit Host Identifier\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support a 128-bit Host Identifier\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataRRLS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Read Recovery Levels Supports (RRLS):\n");
    for (int i = 0; i < 16; i++)
    {
        if ((_pstController->RRLS >> i) & 0x1)
        {
            printf("\tbit [     %2d] 1 = Read Recovery Level %2d is supported\n", i, i);
        }
        else
        {
            printf("\tbit [     %2d] 0 = Read Recovery Level %2d is not supported\n", i, i);
        }
    }
}

static void s_vPrintNVMeIdentifyControllerDataCNTRLTYPE(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Controller Type (CNTRLTYPE): ");
    switch ( _pstController->CNTRLTYPE )
    {
    case 0:
        printf("00h (not reported)\n");
        break;

    case 1:
        printf("01h (I/O Controller)\n");
        break;

    case 2:
        printf("02h (Discovery Controller)\n");
        break;

    case 3:
        printf("03h (Administrative Controller)\n");
        break;

    default:
        printf("%02Xh (invalid value)\n", _pstController->CNTRLTYPE);
        break;
    }
}

static void s_vPrintNVMeIdentifyControllerDataCRDT(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Command Retry Delay Time 1 (CDRT1): %d (%d ms)\n",
        _pstController->CRDT1, _pstController->CRDT1 * 100);
    printf("[O] Command Retry Delay Time 2 (CDRT2): %d (%d ms)\n",
        _pstController->CRDT2, _pstController->CRDT2 * 100);
    printf("[O] Command Retry Delay Time 3 (CDRT3): %d (%d ms)\n",
        _pstController->CRDT3, _pstController->CRDT3 * 100);
}

static void s_vPrintNVMeIdentifyControllerDataNVMSR(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] NVM Subsystem Report (NVMSR):\n");
    if (_pstController->NVMSR.NVMEE)
    {
        printf("\tbit [      1] 1 = This NVM subsystem is part of an NVMe Enclosure\n");
    }
    else
    {
        printf("\tbit [      1] 0 = This NVM subsystem is not part of an NVMe Enclosure\n");
    }

    if (_pstController->NVMSR.NVMESD)
    {
        printf("\tbit [      0] 1 = This NVM subsystem is part of an NVMe Storage Device\n");
    }
    else
    {
        printf("\tbit [      0] 0 = This NVM subsystem is not part of an NVMe Storage Device\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataVWCI(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] VPD Write Cycle Information (VWCI):\n");
    if (_pstController->VWCI.VMCRV)
    {
        printf("\tbit [      7] 1 = The VPD Write Cycles Remaining field is valid\n");
        printf("\tbit [  6:  0] %d = the remaining number of times that VPD contents are able to be updated using the VPD Write command\n", _pstController->VWCI.VMCR);
    }
    else
    {
        printf("\tbit [      7] 0 = The VPD Write Cycles Remaining field is valid\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataMEC(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Management Endpoint Capabilities (MEC):\n");
    if (_pstController->MEC.PCIEME)
    {
        printf("\tbit [      1] 1 = This NVM subsystem contains a Management Endpoint on a PCIe port\n");
    }
    else
    {
        printf("\tbit [      1] 0 = This NVM subsystem does not contain a Management Endpoint on a PCIe port\n");
    }

    if (_pstController->MEC.SMBUSME)
    {
        printf("\tbit [      0] 1 = This NVM subsystem contains a Management Endpoint on an SMBus/I2C port\n");
    }
    else
    {
        printf("\tbit [      0] 0 = This NVM subsystem does not contain a Management Endpoint on an SMBus/I2C port\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataOACS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Optional Admin Command Support (OACS):\n");

    if (bIsNVMeV20OrLater())
    { // revision 2.0 or later
        if (_pstController->OACS.Lockdown)
        {
            printf("\tbit [     10] 1 = Supports Lockdown command\n");
        }
        else
        {
            printf("\tbit [     10] 0 = Does not support Lockdown command\n");
        }
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        if (_pstController->OACS.GetLBAStatusCommand)
        {
            printf("\tbit [      9] 1 = Supports Get LBA Status command\n");
        }
        else
        {
            printf("\tbit [      9] 0 = Does not support Get LBA Status command\n");
        }
    }

    if (bIsNVMeV13OrLater())
    { // revision 1.3 or over
        if (_pstController->OACS.DBConfigCommand)
        {
            printf("\tbit [      8] 1 = Supports Doorbell Buffer Config command\n");
        }
        else
        {
            printf("\tbit [      8] 0 = Does not support Doorbell Buffer Config command\n");
        }

        if (_pstController->OACS.VirtMgmtCommands)
        {
            printf("\tbit [      7] 1 = Supports Virtualization Management command\n");
        }
        else
        {
            printf("\tbit [      7] 0 = Does not support Virtualization Management command\n");
        }

        if (_pstController->OACS.NVMeMICommands)
        {
            printf("\tbit [      6] 1 = Supports NVMe-MI Send and NVMe-MI Receive commands\n");
        }
        else
        {
            printf("\tbit [      6] 0 = Does not support NVMe-MI Send and NVMe-MI Receive commands\n");
        }

        if (_pstController->OACS.Directives)
        {
            printf("\tbit [      5] 1 = Supports Directives\n");
        }
        else
        {
            printf("\tbit [      5] 0 = Does not support Directives\n");
        }

        if (_pstController->OACS.DeviceSelfTest)
        {
            printf("\tbit [      4] 1 = Supports Device Self-test command\n");
        }
        else
        {
            printf("\tbit [      4] 0 = Does not support Device Self-test command\n");
        }
    }

    if (_pstController->OACS.NamespaceCommands)
    {
        printf("\tbit [      3] 1 = Supports Namespace Management and Namespace Attachment commands\n");
    }
    else
    {
        printf("\tbit [      3] 0 = Does not support Namespace Management and Namespace Attachment commands\n");
    }

    if (_pstController->OACS.FirmwareCommands)
    {
        printf("\tbit [      2] 1 = Supports Firmware Commit and Firmware Image Download commands\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not support Firmware Commit and Firmware Image Download commands\n");
    }

    if (_pstController->OACS.FormatNVM)
    {
        printf("\tbit [      1] 1 = Supports Format NVM command\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Does not support Format NVM command\n");
    }

    if (_pstController->OACS.SecurityCommands)
    {
        printf("\tbit [      0] 1 = Supports Security Send and Security Receive commands\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Security Send and Security Receive commands\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataFRMW(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Firmware Updates (FRMW):\n");

    if (bIsNVMeV20OrLater())
    {
        if (_pstController->FRMW.MultipleUpdateDetect)
        {
            printf("\tbit [      5] 1 = Supports detecting overlapping firmware / boot partition image update command sequences\n");
        }
        else
        {
            printf("\tbit [      5] 0 = Does not support detecting overlapping firmware / boot partition image update command sequences\n");
        }
    }

    if (_pstController->FRMW.ActivationWithoutReset)
    {
        printf("\tbit [      4] 1 = Supports firmware activation without a reset\n");
    }
    else
    {
        printf("\tbit [      4] 0 = Does not support firmware activation without a reset\n");
    }

    printf("\tbit [  3:  1] %d = The number of firmware slots that the controller supports\n", _pstController->FRMW.SlotCount);

    if (_pstController->FRMW.Slot1ReadOnly)
    {
        printf("\tbit [      0] 1 = The first firmware slot (slot 1) is read only\n");
    }
    else
    {
        printf("\tbit [      0] 0 = The first firmware slot (slot 1) is read/write\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataLPA(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Log Page Attributes (LPA):\n");

    if (bIsNVMeV20OrLater())
    { // revision 2.0 or later
        if (_pstController->LPA.TelemetryArea4)
        {
            printf("\tbit [      6] 1 = Supports Data Area 4 for the Telemetry Log\n");
        }
        else
        {
            printf("\tbit [      6] 0 = Does not support Data Area 4 for the Telemetry Log\n");
        }

        if (_pstController->LPA.CommandScope)
        {
            printf("\tbit [      5] 1 = Supports reporting the scope of commands in Commands Supported and Effects log page\n");
        }
        else
        {
            printf("\tbit [      5] 0 = Does not support returning the scope of commands in Commands Supported and Effects log page\n");
        }
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        if (_pstController->LPA.PersistentEventLog)
        {
            printf("\tbit [      4] 1 = Supports Persistent Event Log\n");
        }
        else
        {
            printf("\tbit [      4] 0 = Does not support Persistent Event Log\n");
        }
    }

    if (bIsNVMeV13OrLater())
    { // revision 1.3 or over
        if (_pstController->LPA.TelemetrySupport)
        {
            printf("\tbit [      3] 1 = Supports Telemetry Host-Initiated and Telemetry Controller-Initiated log pages and sending Telemetry Log Notices\n");
        }
        else
        {
            printf("\tbit [      3] 0 = Does not support Telemetry Host-Initiated and Telemetry Controller-Initiated log pages and sending Telemetry Log Notices\n");
        }
    }

    if (_pstController->LPA.LogPageExtendedData)
    {
        printf("\tbit [      2] 1 = Supports extended data for Get Log Page (including extended Number of Dwords and Log Page Offset fields)\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not support extended data for Get Log Page\n");
    }

    if (_pstController->LPA.CommandEffectsLog)
    {
        printf("\tbit [      1] 1 = Supports Commands Supported and Effects log page\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Does not support Commands Supported and Effects log page\n");
    }

    if (_pstController->LPA.SmartPagePerNamespace)
    {
        printf("\tbit [      0] 1 = Supports SMART / Health information log page on a per namespace basis\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support SMART / Health information log page on a per namespace basis\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataAVSCC(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Admin Vendor Specific Command Configuration (AVSCC):\n");
    if (_pstController->AVSCC.CommandFormatInSpec)
    {
        printf("\tbit [      0] 1 = All Admin Vendor Specific Commands use the format defined in the specification\n");
    }
    else
    {
        printf("\tbit [      0] 0 = The format of all Admin Vendor Specific Commands are vendor specific\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataAPSTA(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Autonomous Power State Transition Attributes (APSTA):\n");
    if (_pstController->APSTA.Supported)
    {
        printf("\tbit [      0] 1 = Supports Autonomous Power State Transitions\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Autonomous Power State Transitions\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataCTEMP(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Warning Composite Temperature Threshold (WCTEMP): ");
    if (_pstController->WCTEMP == 0)
    {
        printf("0 = undefined\n");
    }
    else
    {
        printf("%d = %d (K), %d (C)\n", _pstController->WCTEMP, _pstController->WCTEMP, _pstController->WCTEMP - 273);
    }

    printf("[M] Critical Composite Temperature Threshold (CCTEMP): ");
    if (_pstController->CCTEMP == 0)
    {
        printf("0 = undefined\n");
    }
    else
    {
        printf("%d = %d (K), %d (C)\n", _pstController->CCTEMP, _pstController->CCTEMP, _pstController->CCTEMP - 273);
    }
}

static void s_vPrintNVMeIdentifyControllerDataMTFA(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Maximum Time for Firmware Activation (MTFA): ");
    if (_pstController->MTFA == 0)
    {
        printf("0 = undefined\n");
    }
    else
    {
        printf("%d = %d msec\n", _pstController->MTFA, _pstController->MTFA * 100);
    }
}

static void s_vPrintNVMeIdentifyControllerDataHMB(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Host Memory Buffer Preferred Size (HMPRE): ");
    if (_pstController->HMPRE == 0)
    {
        printf("0 = HMB is not supported\n");
    }
    else
    {
        printf("%d = %d KiB\n", _pstController->HMPRE, _pstController->HMPRE * 4);
    }

    printf("[O] Host Memory Buffer Minimum Size (HMMIN): ");
    if (_pstController->HMMIN == 0)
    {
        printf("0 = HMB is not supported\n");
    }
    else
    {
        printf("%d = %d KiB\n", _pstController->HMMIN, _pstController->HMMIN * 4);
    }
}

static void s_vPrintNVMeIdentifyControllerDataRPMB(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Replay Protected Memory Block Support (RPMBS):\n");

    if (_pstController->RPMBS.RPMBUnitCount != 0)
    {
        printf("\tbit [ 31: 24] %d = %d bytes\n",
            _pstController->RPMBS.AccessSize, (_pstController->RPMBS.AccessSize + 1) * 512);
        printf("\tbit [ 23: 16] %d = %d KiB\n",
            _pstController->RPMBS.TotalSize, (_pstController->RPMBS.TotalSize + 1) * 128);
        if (_pstController->RPMBS.AuthenticationMethod == 0) {
            printf("\tbit [  5:  3] %d = means HMAC SHA-256\n",
                _pstController->RPMBS.AuthenticationMethod);
        }
    }

    if (_pstController->RPMBS.RPMBUnitCount == 0)
    {
        printf("\tbit [  2:  0] 0 = Does not support RPMB\n");
    }
    else
    {
        printf("\tbit [  2:  0] %d = The number of RPMB targets\n", _pstController->RPMBS.RPMBUnitCount);
    }
}

static void s_vPrintNVMeIdentifyControllerDataEDSTT(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Extended Device Self-test Time (EDSTT): ");
    if (_pstController->OACS.DeviceSelfTest != 0)
    {
        printf("%d = %d minutes\n", _pstController->EDSTT, _pstController->EDSTT);
    }
    else
    {
        printf("0 = Does not support Device Self-test\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataDSTO(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Device Self-test Options (DSTO): ");
    if (_pstController->OACS.DeviceSelfTest != 0)
    {
        if (_pstController->DSTO == 1)
        {
            printf("\n\tbit [      0] 1 = Supports only one device self-test operation in progress at a time\n");
        }
        else if (_pstController->DSTO == 0)
        {
            printf("\n\tbit [      0] 0 = Supports one device self-test operation per controller at a time\n");
        }
    }
    else
    {
        printf("0 = Not supported\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataFWUG(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Firmware Update Granularity (FWUG): ");
    if (_pstController->FWUG == 0xFF)
    {
        printf("FFh = No limitation\n");
    }
    else if (_pstController->FWUG == 0)
    {
        printf("00h = no information\n");
    }
    else
    {
        printf("%02Xh = means %d KiB\n", _pstController->FWUG, _pstController->FWUG * 4);
    }
}

static void s_vPrintNVMeIdentifyControllerDataKAS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Keep Alive Support (KAS): ");
    if (_pstController->KAS == 0)
    {
        printf("0 = Not supported\n");
    }
    else
    {
        printf("1 = Supported, The granularity of the Keep Alive Timer is %d msec\n", _pstController->KAS * 100);
    }
}

static void s_vPrintNVMeIdentifyControllerDataHCTMA(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Host Controlled Thermal Management Attributes (HCTMA):\n");
    if (_pstController->HCTMA.Supported)
    {
        printf("\tbit [      0] 1 = Supports Host Controlled Thermal Management\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Host Controlled Thermal Management\n");
    }

    printf("[O] Minimum Thermal Management Temperature (MNTMT): ");
    if (_pstController->HCTMA.Supported == 0)
    {
        printf("N/A = Not suppored\n");
    }
    else if (_pstController->MNTMT == 0)
    {
        printf("0 = Not reported\n");
    }
    else
    {
        printf("%d = %d (K), %d (C)\n", _pstController->MNTMT, _pstController->MNTMT, _pstController->MNTMT - 273);
    }

    printf("[O] Maximum Thermal Management Temperature (MXTMT): ");
    if (_pstController->HCTMA.Supported == 0)
    {
        printf("N/A = Not supported\n");
    }
    else if (_pstController->MNTMT == 0)
    {
        printf("0 = Not reported\n");
    }
    else
    {
        printf("%d = %d (K), %d (C)\n", _pstController->MXTMT, _pstController->MXTMT, _pstController->MXTMT - 273);
    }
}

static void s_vPrintNVMeIdentifyControllerDataSANICAP(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Sanitize Capabilities (SANICAP): ");
    if (_pstController->SANICAP.Overwrite | _pstController->SANICAP.CryptoErase | _pstController->SANICAP.BlockErase)
    {
        printf("\n");
        if (bIsNVMeV14OrLater())
        { // revision 1.4 or over
            switch (_pstController->SANICAP.NODMMAS)
            {
            case 0:
                printf("\tbit [ 31: 30] 0 = Additional media modification after sanitize operation completes successfully is not defined\n");
                break;

            case 1:
                printf("\tbit [ 31: 30] 1 = Media is not additionally modified by the NVMe controller after sanitize operation completes successfully\n");
                break;

            case 2:
                printf("\tbit [ 31: 30] 2 = Media is additionally modified by the NVMe controller after sanitize operation completes successfully\n");
                break;

            default:
                printf("\tbit [ 31: 30] 3 = (reserved value)\n");
                break;
            }
            if (_pstController->SANICAP.NoDeallocateInhibited)
            {
                printf("\tbit [     29] 1 = No-Deallocate Inhibited\n");
            }
            else
            {
                printf("\tbit [     29] 0 = No-Deallocate Inhibited\n");
            }
        }
        if (_pstController->SANICAP.Overwrite)
        {
            printf("\tbit [      2] 1 = Supports Overwrite sanitize operation\n");
        }
        else
        {
            printf("\tbit [      2] 0 = Does not support Overwrite sanitize operation\n");
        }
        if (_pstController->SANICAP.BlockErase)
        {
            printf("\tbit [      1] 1 = Supports Block Erase sanitize operation\n");
        }
        else
        {
            printf("\tbit [      1] 0 = Does not support Block Erase sanitize operation\n");
        }
        if (_pstController->SANICAP.CryptoErase)
        {
            printf("\tbit [      0] 1 = Supports Crypto Erase sanitize operation\n");
        }
        else
        {
            printf("\tbit [      0] 0 = Does not support Crypto Erase sanitize operation\n");
        }
    }
    else
    {
        printf("0 = The Sanitize command is not supported\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataHMMINDS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Host Memory Buffer Minimum Descriptor Entry Size (HMMINDS): ");

    if (_pstController->HMMINDS)
    {
        printf("%d (%d KiB)\n", _pstController->HMMINDS, _pstController->HMMINDS << 2);
    }
    else
    {
        printf("0 (does not indicate any limitations)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataHMMAXD(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Host Memory Maximum Descriptors Entries (HMMAXD): ");

    if (_pstController->HMMAXD)
    {
        printf("%d\n", _pstController->HMMINDS);
    }
    else
    {
        printf("0 (does not indicate a maximum number of Host Memory Buffer Descriptor Entries)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataNSETIDMAX(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] NVM Set Identifier Maximum (NSETIDMAX): %d\n", _pstController->NSETIDMAX);
}

static void s_vPrintNVMeIdentifyControllerDataENDGIDMAX(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Endurance Group Identifier Maximum (ENDGIDMAX): %d\n", _pstController->ENDGIDMAX);
}

static void s_vPrintNVMeIdentifyControllerDataANATT(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] ANA Transition Time (ANATT): ");

    if (_pstController->ANATT)
    {
        printf("%d (%d second(s))\n", _pstController->ANATT, _pstController->ANATT);
    }
    else
    {
        printf("0 (does not support Asymmetric Namespace Access Reporting)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataANACAP(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Asymmetric Namespace Access Capabilities (ANACAP):\n");
    if (_pstController->ANACAP.NonZeroGRPIDSupport)
    {
        printf("\tbit [      7] 1 = Supports a non-zero value in ANAGRPID field of Namespace Management command\n");
    }
    else
    {
        printf("\tbit [      7] 0 = Does not support a non-zero value in ANAGRPID field of Namespace Management command\n");
    }

    if (_pstController->ANACAP.NoGRPIDChangeDuringAttached)
    {
        printf("\tbit [      6] 1 = ANAGRPID field in Identify Namespace data structure does not change while the namespace is attached to any controller\n");
    }
    else
    {
        printf("\tbit [      6] 0 = ANAGRPID field may change while the namespace is attached to any controller\n");
    }

    if (_pstController->ANACAP.ReportChangeState)
    {
        printf("\tbit [      4] 1 = Reports ANA Change state\n");
    }
    else
    {
        printf("\tbit [      4] 0 = Does not report ANA Change state\n");
    }

    if (_pstController->ANACAP.ReportPersistentLossState)
    {
        printf("\tbit [      3] 1 = Reports ANA Persistent Loss state\n");
    }
    else
    {
        printf("\tbit [      3] 0 = Does not report ANA Persistent Loss state\n");
    }

    if (_pstController->ANACAP.ReportInaccessibleState)
    {
        printf("\tbit [      2] 1 = Reports ANA Inaccessible state\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not report ANA Inaccessible state\n");
    }

    if (_pstController->ANACAP.ReportNonOptimizedState)
    {
        printf("\tbit [      1] 1 = Reports ANA Non-Optimized state\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Does not report ANA Non-Optimized state\n");
    }

    if (_pstController->ANACAP.ReportOptimizedState)
    {
        printf("\tbit [      0] 1 = Reports ANA Optimized state\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not report ANA Optimized state\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataANAGRPMAX(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] ANA Group Identifier Maximum (ANAGRPMAX): ");

    if (_pstController->ANAGRPMAX)
    {
        printf("%d\n", _pstController->ANAGRPMAX);
    }
    else
    {
        printf("0 (does not support Asymmetric Namespace Access Reporting)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataNANAGRPID(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Number of ANA Group Identifiers (NANAGRPID): ");

    if (_pstController->NANAGRPID)
    {
        printf("%d\n", _pstController->NANAGRPID);
    }
    else
    {
        printf("0 (does not support Asymmetric Namespace Access Reporting)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataPELS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Persistent Event Log Size (PELS): ");

    if (_pstController->PELS)
    {
        printf("%d (%d KiB)\n", _pstController->PELS, _pstController->PELS * 64);
    }
    else
    {
        printf("0 (does not support Persistent Event Log)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataDomainId(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Domain Identifier: ");

    if (_pstController->CTRATT.MDS)
    {
        printf("%d\n", _pstController->DomainId);
    }
    else
    {
        printf("0 (does not support multiple domains)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataMEGCAP(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Max Endurance Group Capacity: ");

    if (_pstController->CTRATT.EnduranceGroups)
    {
        printf("%llu (byte)\n", (uint64_t)(_pstController->MEGCAP));
    }
    else
    {
        printf("0 (does not support Endurance Group)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataSQES(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Submission Queue Entry Size (SQES):\n");

    printf("\tbit [  7:  4] %d = %d bytes = The maximum Submission Queue Entry Size when using NVM Command Set\n",
        _pstController->SQES.MaxEntrySize, 1 << _pstController->SQES.MaxEntrySize);
    printf("\tbit [  3:  0] %d = %d bytes = The required Submission Queue Entry Size when using NVM Command Set\n",
        _pstController->SQES.RequiredEntrySize, 1 << _pstController->SQES.RequiredEntrySize);
}

static void s_vPrintNVMeIdentifyControllerDataCQES(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Completion Queue Entry Size (SQES):\n");

    printf("\tbit [  7:  4] %d = %d bytes = The maximum Completion Queue Entry Size when using NVM Command Set\n",
        _pstController->CQES.MaxEntrySize, 1 << _pstController->CQES.MaxEntrySize);
    printf("\tbit [  3:  0] %d = %d bytes = The required Completion Queue Entry Size when using NVM Command Set\n",
        _pstController->CQES.RequiredEntrySize, 1 << _pstController->CQES.RequiredEntrySize);
}

static void s_vPrintNVMeIdentifyControllerDataMAXCMD(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Maximum Outstanding Commands (MAXCMD): ");
    if (_pstController->MAXCMD == 0)
    {
        printf("0 = unused\n");
    }
    else
    {
        printf("%d\n", _pstController->MAXCMD);
    }
}

static void s_vPrintNVMeIdentifyControllerDataONCS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Optional NVM Command Support (ONCS):\n");

    if (bIsNVMeV20OrLater())
    {
        if (_pstController->ONCS.Copy)
        {
            printf("\tbit [      8] 1 = Supports Copy command\n");
        }
        else
        {
            printf("\tbit [      8] 0 = Does not support Copy command\n");
        }
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        if (_pstController->ONCS.Verify)
        {
            printf("\tbit [      7] 1 = Supports Verify command\n");
        }
        else
        {
            printf("\tbit [      7] 0 = Does not support Verify command\n");
        }
    }

    if (bIsNVMeV13OrLater())
    { // revision 1.3 or over
        if (_pstController->ONCS.Timestamp)
        {
            printf("\tbit [      6] 1 = Supports Timestamp feature\n");
        }
        else
        {
            printf("\tbit [      6] 0 = Does not support Timestamp feature\n");
        }
    }

    if (_pstController->ONCS.Reservations)
    {
        printf("\tbit [      5] 1 = Supports Reservations\n");
    }
    else
    {
        printf("\tbit [      5] 0 = Does not support Reservations\n");
    }

    if (_pstController->ONCS.FeatureField)
    {
        printf("\tbit [      4] 1 = Supports Save field set to a non-zero value in Set Features command and Select field set to a non-zero value in Get Features command\n");
    }
    else
    {
        printf("\tbit [      4] 0 = Does not support Save field set to a non-zero value in Set Features command and Select field set to a non-zero value in Get Features command\n");
    }

    if (_pstController->ONCS.WriteZeroes)
    {
        printf("\tbit [      3] 1 = Supports Write Zeroes command\n");
    }
    else
    {
        printf("\tbit [      3] 0 = Does not support Write Zeroes command\n");
    }

    if (_pstController->ONCS.DatasetManagement)
    {
        printf("\tbit [      2] 1 = Supports Dataset Management command\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not support Dataset Management command\n");
    }

    if (_pstController->ONCS.WriteUncorrectable)
    {
        printf("\tbit [      1] 1 = Supports Write Uncorrectable command\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Does not support Write Uncorrectable command\n");
    }

    if (_pstController->ONCS.Compare)
    {
        printf("\tbit [      0] 1 = Supports Compare command\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Compare command\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataFUSES(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Fused Operation Support (FUSES):\n");
    if (_pstController->FUSES.CompareAndWrite)
    {
        printf("\tbit [      0] 1 = Supports Compare and Write fused operation\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Compare and Write fused operation\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataFNA(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Format NVM Attributes (FNA):\n");

    if (bIsNVMeV20OrLater())
    {
        if (_pstController->FNA.NotAcceptNAIDIsAllF)
        {
            printf("\tbit [      3] 1 = Does not support Format NVM command with an NSID value set to FFFFFFFFh\n");
        }
        else
        {
            printf("\tbit [      3] 0 = Supports Format NVM command with an NSID value set to FFFFFFFFh\n");
        }
    }

    if (_pstController->FNA.CryptographicEraseSupported)
    {
        printf("\tbit [      2] 1 = Supports cryptographic erase operation\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not support cryptographic erase operation\n");
    }

    if (_pstController->FNA.SecureEraseApplyToAll)
    {
        printf("\tbit [      1] 1 = Cryptographic erase or user data erase as part of a format apply to all namespaces.\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Cryptographic erase or user data erase as part of a format is performed on a per namespace basis.\n");
    }

    if (_pstController->FNA.FormatApplyToAll)
    {
        printf("\tbit [      0] 1 = Format operation applies to all namespaces\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Format operation is specific to a particular namespace.\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataVWC(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Volatile Write Cache (VWC):\n");
    if (_pstController->VWC.Present)
    {
        if (bIsNVMeV14OrLater())
        { // revision 1.4 or over
            switch (_pstController->VWC.FlushBehavior)
            {
            case 1:
                printf("\tbit [  2:  1] 1 = (reserved value)\n");
                break;

            case 2:
                printf("\tbit [  2:  1] 2 = Does not support Flush command with NSID field set to FFFFFFFFh\n");
                break;

            case 3:
                printf("\tbit [  2:  1] 2 = Supports Flush command with NSID field set to FFFFFFFFh\n");
                break;

            case 0:
            default:
                printf("\tbit [  2:  1] 0 = Supports Flush command with NSID field set to FFFFFFFFh is not indicated\n");
                break;
            }
        }
        printf("\tbit [      0] 1 = A volatile write cache is present\n");
    }
    else
    {
        printf("\tbit [      0] 0 = A volatile write cache is not present\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataNVSCC(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] NVM Vendor Specific Command Configuration (NVSCC):\n");
    if (_pstController->NVSCC.CommandFormatInSpec)
    {
        printf("\tbit [      0] 1 = All NVM Vendor Specific Commands use the format defined in the specification\n");
    }
    else
    {
        printf("\tbit [      0] 0 = The format of all NVM Vendor Specific Commands are vendor specific\n");
    }
}


static void s_vPrintNVMeIdentifyControllerDataAWU(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Atomic Write Unit Normal (AWUN): ");
    if (_pstController->AWUN == 0xFFFF)
    {
        printf("FFFFh = all commands are atomic\n");
    }
    else
    {
        printf("%04Xh = %d sector(s)\n", _pstController->AWUN, _pstController->AWUN + 1);
    }

    printf("[M] Atomic Write Unit Power Fail (AWUPF): %04Xh = %d sector(s)\n",
        _pstController->AWUPF, _pstController->AWUPF + 1);
}

static void s_vPrintNVMeIdentifyControllerDataNWPC(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Namespace Write Protection Capabilities (NWPC):\n");
    if (_pstController->NWPC.PermanentWriteProtectSupport)
    {
        printf("\tbit [      2] 1 = Supports Permanent Write Protect state\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not support Permanent Write Protect state\n");
    }

    if (_pstController->NWPC.WriteProtectUntilPowerCycleSupport)
    {
        printf("\tbit [      1] 1 = Supports Write Protect Until Power Cycle state\n");
    }
    else
    {
        printf("\tbit [      1] 0 = Does not support Write Protect Until Power Cycle state\n");
    }

    if (_pstController->NWPC.NoWriteProtectSupport)
    {
        printf("\tbit [      0] 1 = Supports No Write Protect and Write Protect namespace write protection states\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Namespace Write Protection\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataACWU(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Atomic Compare & Write Unit (ACWU): ");
    if (_pstController->ACWU == 0)
    {
        printf("0 = Compare and Write is not supported in a fused command\n");
    }
    else
    {
        printf("%d = means %d sector(s)\n", _pstController->ACWU, _pstController->ACWU + 1);
    }
}

static void s_vPrintNVMeIdentifyControllerDataOCFS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Optional Copy Formats Supported:\n");
    if (_pstController->OCFS.FormatZeroSupp)
    {
        printf("\tbit [      0] 1 = Supports Copy Format 0h\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Copy Format 0h\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataSGLS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] SGL Support (SGLS):\n");

    if (_pstController->SGLS.SGLSupported == 0)
    {
        if (bIsNVMeV13OrLater())
        { // revision 1.3 or over
            printf("\tbit [  1:  0] 0 = SGLs are not supported\n");
        }
        else
        {
            printf("\tbit [      0] 0 = SGLs are not supported\n");
        }
        return;
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        if (_pstController->SGLS.TransactionalSGLDataBlockDescSupported)
        {
            printf("\tbit [     21] 1 = Supports Transport SGL Data Block descriptor\n");
        }
        else
        {
            printf("\tbit [     21] 0 = Does not support Transport SGL Data Block descriptor\n");
        }
    }

    if (_pstController->SGLS.OffsetByAddrFieldSupported)
    {
        printf("\tbit [     20] 1 = Supports Address field in SGL Data Block, SGL Segment, and SGL Last Segment descriptor types specifying an offset\n");
    }
    else
    {
        printf("\tbit [     20] 0 = Does not support Address field specifying an offset\n");
    }

    if (_pstController->SGLS.MPTRContainingSGLDescSupported)
    {
        printf("\tbit [     19] 1 = Supports use of a Metadata Pointer (MPTR) that contains an address of an SGL segment containing exactly one SGL Descriptor that is Qword aligned\n");
    }
    else
    {
        printf("\tbit [     19] 0 = Does not support use of a Metadata Pointer (MPTR) that contains an address of an SGL segment containing exactly one SGL Descriptor that is Qword aligned\n");
    }

    if (_pstController->SGLS.SGLLengthLargerThanDataLength)
    {
        printf("\tbit [     18] 1 = Supports commands that contain a data or metadata SGL of a length larger than the amount of data to be transferred\n");
    }
    else
    {
        printf("\tbit [     18] 0 = Requires the SGL length is equal to the amount of data to be transferred\n");
    }

    if (_pstController->SGLS.ByteAlignedContiguousPhysicalBuffer)
    {
        printf("\tbit [     17] 1 = Supports use of a byte aligned contiguous physical buffer of metadata\n");
    }
    else
    {
        printf("\tbit [     17] 0 = Does not support use of a byte aligned contiguous physical buffer of metadata\n");
    }

    if (_pstController->SGLS.BitBucketDescrSupported)
    {
        printf("\tbit [     16] 1 = Supports SGL Bit Bucket descriptor\n");
    }
    else
    {
        printf("\tbit [     16] 0 = Does not support SGL Bit Bucket descriptor\n");
    }

    if (bIsNVMeV20OrLater())
    {
        if (_pstController->SGLS.SDT)
        {
            printf("\tbit [ 15:  8] %d = Recommended maximum number of SGL descriptors in a command\n", _pstController->SGLS.SDT);
        }
        else
        {
            printf("\tbit [ 15:  8] 0 = No recommended maximum number of SGL descriptors is reported\n");
        }
    }

    if (_pstController->SGLS.KeyedDBDescSupported)
    {
        printf("\tbit [      2] 1 = Supports Keyed SGL Data Block descriptor\n");
    }
    else
    {
        printf("\tbit [      2] 0 = Does not support Keyed SGL Data Block descriptor\n");
    }

    switch (_pstController->SGLS.SGLSupported)
    {
        case 0:
            printf("\tbit [  1:  0] 0 = SGLs are not supported\n");
            break;

        case 1:
            printf("\tbit [  1:  0] 1 = SGLs are supported. There is no alignment nor granularity requirement for Data Blocks\n");
            break;

        case 2:
            printf("\tbit [  1:  0] 2 = SGLs are supported. There is a Dword alignment and granularity requirement for Data Blocks\n");
            break;

        case 3: // reserved
        default:
            break;

    }
}

static void s_vPrintNVMeIdentifyControllerDataMNAN(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Maximum Number of Allowed Namespaces (MNAN): ");

    if (_pstController->MNAN)
    {
        printf("%d\n", _pstController->MNAN);
    }
    else
    {
        printf("\n\t0 (Maximum number of namespaces supported by the NVM subsystem is less than or equal to the value in the NN field)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataMAXDNA(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Maximum Domain Namespace Attachments (MAXDNA): ");
    uint64_t val = (uint64_t)(_pstController->MAXDNA);

    if (val)
    {
        printf("%llu\n", val);
    }
    else
    {
        printf("0 (no maximum is specified)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataMAXCNA(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[O] Maximum I/O Controller Namespace Attachments (MAXCNA): ");
    if (_pstController->MAXCNA)
    {
        printf("%d\n", _pstController->MAXCNA);
    }
    else
    {
        printf("0 (no maximum is specified)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataSUBNQN(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] NVM Subsystem NVMe Qualified Name (SUBNQN): ");
    char* strConv = strConvertUTF8toMultiByte((const char*)&(_pstController->SUBNQN));
    if (strConv != NULL)
    {
        printf("%s\n", strConv);
        free(strConv);
    }
    else
    {
        printf("(null)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataIOCCSZ(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] I/O Queue Command Capsule Supported Size (IOCCSZ): ");
    printf("%d (means %d bytes)\n", _pstController->IOCCSZ, _pstController->IOCCSZ * 16);
}

static void s_vPrintNVMeIdentifyControllerDataIORCSZ(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] I/O Queue Response Capsule Supported Size (IORCSZ): ");
    printf("%d (means %d bytes)\n", _pstController->IORCSZ, _pstController->IORCSZ * 16);
}

static void s_vPrintNVMeIdentifyControllerDataICDOFF(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] In Capsule Data Offset (ICDOFF): ");
    printf("%d (means %d bytes)\n", _pstController->ICDOFF, _pstController->ICDOFF * 16);
}

static void s_vPrintNVMeIdentifyControllerDataFCATT(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Fabrics Controller Attributes (FCATT):\n");
    if (_pstController->FCATT.StaticCtrlModel)
    {
        printf("\tbit [      0] 1 = Uses a static controller model\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Uses a dynamic controller model\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataMSDBD(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Maximum SGL Data Block Descriptors (MSDBD): ");
    if (_pstController->MSDBD)
    {
        printf("%d\n", _pstController->MSDBD);
    }
    else
    {
        printf("0 (means no limit)\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataOFCS(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    printf("[M] Optional Fabric Commands Support (OFCS):\n");
    if (_pstController->OFCS.DisconnectSupp)
    {
        printf("\tbit [      0] 1 = Supports Disconnect command\n");
    }
    else
    {
        printf("\tbit [      0] 0 = Does not support Disconnect command\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataPSD(PNVME_POWER_STATE_DESC pDesc, ULONG ulVersion)
{
    double dCoeffAPS = 0.0;
    printf("\tbit [183:182] %d = Active Power Scale (APS) is ", pDesc->APS);
    switch (pDesc->APS)
    {
    case 0:
        printf("not reported\n");
        break;

    case 1:
        printf("0.0001 W (0.1 mW)\n");
        dCoeffAPS = 0.1;
        break;

    case 2:
        printf("0.01 W (10 mW)\n");
        dCoeffAPS = 10.0;
        break;

    default:
        printf("(illegal value)\n");
        break;
    }

    printf("\tbit [178:176] %d = Active Power Workload (APW) is ", pDesc->APW);
    switch (pDesc->APW)
    {
    case 0:
        printf("not reported / no workload\n");
        break;

    case 1:
        printf("workload #1 (Extended Idle Period with a Burst of Random Writes)\n");
        break;

    case 2:
        printf("workload #2 (Heavy Sequential Writes)\n");
        break;

    default:
        printf("(illegal)\n");
        break;
    }

    printf("\tbit [175:160] %d = Active Power (ACTP) ", pDesc->ACTP);
    if (pDesc->ACTP == 0)
    {
        printf("is not reported\n");
    }
    else
    {
        printf("is %3.2f (mW)\n", (double)(pDesc->ACTP) * dCoeffAPS);
    }

    printf("\tbit [151:150] %d = Idle Power Scale (IPS) is ", pDesc->IPS);
    double dCoeffIPS = 0.0;
    switch (pDesc->IPS)
    {
    case 0:
        printf("not reported\n");
        break;

    case 1:
        printf("0.0001 W (0.1 mW)\n");
        dCoeffIPS = 0.1;
        break;

    case 2:
        printf("0.01 W (10 mW)\n");
        dCoeffIPS = 10.0;
        break;

    default:
        printf("(illegal value)\n");
        break;
    }

    printf("\tbit [143:128] %d = Idle Power (IDLP) is ", pDesc->IDLP);
    if (pDesc->IDLP == 0)
    {
        printf("not reported\n");
    }
    else
    {
        printf("%3.2f (mW)\n", (double)(pDesc->IDLP) * dCoeffIPS);
    }

    printf("\tbit [124:120] %d = Relative Write Latency (RWL)\n", pDesc->RWL);
    printf("\tbit [116:112] %d = Relative Write Throughput (RWT)\n", pDesc->RWT);
    printf("\tbit [108:104] %d = Relative Read Latency (RRL)\n", pDesc->RRL);
    printf("\tbit [100: 96] %d = Relative Read Throughput (RWT)\n", pDesc->RRT);

    printf("\tbit [ 95: 64] %d = Exit Latency (EXLAT) is ", pDesc->EXLAT);
    if (pDesc->EXLAT == 0)
    {
        printf("not reported\n");
    }
    else
    {
        printf("%d (msec)\n", pDesc->EXLAT);
    }

    printf("\tbit [ 63: 32] %d = Entry Latency (ENLAT) is ", pDesc->ENLAT);
    if (pDesc->ENLAT == 0)
    {
        printf("not reported\n");
    }
    else
    {
        printf("%d (msec)\n", pDesc->ENLAT);
    }

    if (pDesc->NOPS == 0)
    {
        printf("\tbit [     25] 0 = Operational state\n");
    }
    else
    {
        printf("\tbit [     25] 1 = Non-operational state\n");
    }

    if (ulVersion < 0x10300)
    {
        printf("\tbit [     24] %d = Max Power Scale (MPS) is ", pDesc->MPS);
    }
    else
    {
        printf("\tbit [     24] %d = Max Power Scale (MXPS) is ", pDesc->MPS);
    }

    double dCoeffMPS = 0.0;
    if (pDesc->MPS == 0)
    {
        printf("0.01 W (10 mW)\n");
        dCoeffMPS = 10.0;
    }
    else
    {
        printf("0.0001 W (0.1 mW)\n");
        dCoeffMPS = 0.1;
    }

    printf("\tbit [ 15:  0] %d = Maximum Power (MP) is ", pDesc->MP);
    if (pDesc->MP == 0)
    {
        printf("not reported\n");
    }
    else
    {
        printf("%3.2f (mW)\n", (double)(pDesc->MP) * dCoeffMPS);
    }
}

static void s_vPrintNVMeIdentifyControllerDataPSDs(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    int iNumPSD = _pstController->NPSS + 1;

    printf("[M] Power State 0 Descriptor (PSD0):\n");
    s_vPrintNVMeIdentifyControllerDataPSD(&(_pstController->PDS[0]), _pstController->VER);

    for (int i = 1; i < iNumPSD; i++)
    {
        printf("[O] Power State %d Descriptor (PSD%d):\n", i, i);
        s_vPrintNVMeIdentifyControllerDataPSD(&(_pstController->PDS[i]), _pstController->VER);
    }
}

static void s_vPrintNVMeIdentifyControllerDataFGUID(PMY_NVME_IDENTIFY_CONTROLLER_DATA _pstController)
{
    int iSum = 0;
    unsigned char* pId = _pstController->FGUID;

    printf("[O] FRU Globally Unique Identifier (FGUID): ");
    for (int i = 0; i < 16; i++)
    {
        iSum = pId[i];
    }
    if (iSum == 0)
    {
        printf("(not implemented)");
    }
    else
    {
        printf("\tbyte [127:123] %02X%02X%02X%02X%02X (Extension identifer assigned by the vendor)\n",
            pId[11], pId[12], pId[13], pId[14], pId[15]); // Bytes [127:123] (5 byte, 40 bit = Extension identifer assigned by the organization
        printf("\tbyte [122:120] %02X%02X%02X (Organizationally Unique Identifier (OUI) value assigned by the IEEE Registration Authority)\n",
            pId[8], pId[9], pId[10]); // Bytes [122:120] (3 byte, 24 bit = Organizationally Unique Identifier (OUI) value assigned by the IEEE Registration Authority
        printf("\tbyte [119:112] %02X%02X%02X%02X%02X%02X%02X%02X (Vendor specific extension identifier)\n",
            pId[0], pId[1], pId[2], pId[3], pId[4], pId[5], pId[6], pId[7]); // Bytes [119:112] (8 byte, 64 bit = Vendor specific extension identifier
    }
    printf("\n");
}

void vPrintNVMeIdentifyControllerData(void)
{
    PMY_NVME_IDENTIFY_CONTROLLER_DATA pstController = (PMY_NVME_IDENTIFY_CONTROLLER_DATA)&(g_stController);

    printf("[M] PCI Vendor ID (VID): 0x%X\n",               pstController->VID);
    printf("[M] PCI Subsystem Vendor ID (SSVID): 0x%X\n",   pstController->SSVID);

    {
        char buf[21];
        ZeroMemory(buf, 21);
        strncpy_s(buf, _countof(buf), (const char*)(pstController->SN), 20);
        buf[20] = '\0';
        printASCII("[M] Serial Number (SN): ", (const char*)buf, true);
    }

    {
        char buf[41];
        ZeroMemory(buf, 41);
        strncpy_s(buf, _countof(buf), (const char*)(pstController->MN), 40);
        buf[40] = '\0';
        printASCII("[M] Model Number (MN): ", (const char*)buf, true);
    }

    {
        char buf[9];
        ZeroMemory(buf, 9);
        strncpy_s(buf, _countof(buf), (const char*)(pstController->FR), 8);
        buf[8] = '\0';
        printASCII("[M] Firmware Revision (FR): ", (const char*)buf, true);
    }

    printf("[M] Recommended Arbitration Burst (RAB): %d (means %d)\n",  pstController->RAB, 1 << pstController->RAB);
    printf("[M] IEEE OUI Identifier (IEEE): %02X-%02X-%02X\n",          pstController->IEEE[2], pstController->IEEE[1], pstController->IEEE[0]);

    s_vPrintNVMeIdentifyControllerDataCMIC(pstController);

    printf("[M] Maximum Data Transfer Size (MDTS): %d (means %d)\n",    pstController->MDTS, 1 << pstController->MDTS);
    printf("[M] Controller ID (CNTLID): 0x%x\n",                        pstController->CNTLID);
    printf("[M] Version (VER): 0x%x (NVMe Revision %d.%d.%d)\n",
        pstController->VER, (pstController->VER >> 16) & 0xFFFF, (pstController->VER >> 8) & 0xFF, pstController->VER & 0xFF);
    printf("[M] RTD3 Resume Latency (RTD3R): %d (usec)\n",              pstController->RTD3R);
    printf("[M] RTD3 Entry Latency (RTD3E): %d (usec)\n",               pstController->RTD3E);

    s_vPrintNVMeIdentifyControllerDataOAES(pstController);
    s_vPrintNVMeIdentifyControllerDataCTRATT(pstController);

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        s_vPrintNVMeIdentifyControllerDataRRLS(pstController);
        s_vPrintNVMeIdentifyControllerDataCNTRLTYPE(pstController);
    }

    if (bIsNVMeV13OrLater())
    { // revision 1.3 or over
        s_vPrintNVMeIdentifyControllerDataFGUID(pstController);
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        s_vPrintNVMeIdentifyControllerDataCRDT(pstController);
    }

    if (bIsNVMeV20OrLater())
    { // revision 2.0 or later
        s_vPrintNVMeIdentifyControllerDataNVMSR(pstController);
        s_vPrintNVMeIdentifyControllerDataVWCI(pstController);
        s_vPrintNVMeIdentifyControllerDataMEC(pstController);
    }

    s_vPrintNVMeIdentifyControllerDataOACS(pstController);

    printf("[M] Abort Command Limit (ACL): %d (means %d)\n",                pstController->ACL, pstController->ACL + 1);
    printf("[M] Asynchronous Event Request Limit (AERL): %d (means %d)\n",  pstController->AERL, pstController->AERL + 1);

    s_vPrintNVMeIdentifyControllerDataFRMW(pstController);
    s_vPrintNVMeIdentifyControllerDataLPA(pstController);

    printf("[M] Error Log Page Entries (ELPE): %d (means %d)\n",            pstController->ELPE, pstController->ELPE + 1);
    printf("[M] Number of Power States Support (NPSS): %d (means %d)\n",    pstController->NPSS, pstController->NPSS + 1);

    s_vPrintNVMeIdentifyControllerDataAVSCC(pstController);
    s_vPrintNVMeIdentifyControllerDataAPSTA(pstController);
    s_vPrintNVMeIdentifyControllerDataCTEMP(pstController);
    s_vPrintNVMeIdentifyControllerDataMTFA(pstController);
    s_vPrintNVMeIdentifyControllerDataHMB(pstController);

    if (pstController->OACS.NamespaceCommands)
    {
        printf("[O] Total NVM Capacity (TNVMCAP): %llu (byte)\n",       (uint64_t)(pstController->TNVMCAP));
        printf("[O] Unallocated NVM Capacity (UNVMCAP): %llu (byte)\n", (uint64_t)(pstController->UNVMCAP));
    }

    s_vPrintNVMeIdentifyControllerDataRPMB(pstController);

    if (bIsNVMeV13OrLater())
    { // revision 1.3 or over
        s_vPrintNVMeIdentifyControllerDataEDSTT(pstController);
        s_vPrintNVMeIdentifyControllerDataDSTO(pstController);
        s_vPrintNVMeIdentifyControllerDataFWUG(pstController);
    }

    s_vPrintNVMeIdentifyControllerDataKAS(pstController);

    if (bIsNVMeV13OrLater())
    { // revision 1.3 or over
        s_vPrintNVMeIdentifyControllerDataHCTMA(pstController);
        s_vPrintNVMeIdentifyControllerDataSANICAP(pstController);
    }

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        s_vPrintNVMeIdentifyControllerDataHMMINDS(pstController);
        s_vPrintNVMeIdentifyControllerDataHMMAXD(pstController);
        s_vPrintNVMeIdentifyControllerDataNSETIDMAX(pstController);
        s_vPrintNVMeIdentifyControllerDataENDGIDMAX(pstController);
        s_vPrintNVMeIdentifyControllerDataANATT(pstController);
        s_vPrintNVMeIdentifyControllerDataANACAP(pstController);
        s_vPrintNVMeIdentifyControllerDataANAGRPMAX(pstController);
        s_vPrintNVMeIdentifyControllerDataNANAGRPID(pstController);
        s_vPrintNVMeIdentifyControllerDataPELS(pstController);
    }

    if (bIsNVMeV20OrLater())
    {
        s_vPrintNVMeIdentifyControllerDataDomainId(pstController);
        s_vPrintNVMeIdentifyControllerDataMEGCAP(pstController);
    }

    s_vPrintNVMeIdentifyControllerDataSQES(pstController);
    s_vPrintNVMeIdentifyControllerDataCQES(pstController);
    s_vPrintNVMeIdentifyControllerDataMAXCMD(pstController);

    printf("[M] Number of Namespaces (NN): %d\n", pstController->NN);

    s_vPrintNVMeIdentifyControllerDataONCS(pstController);
    s_vPrintNVMeIdentifyControllerDataFUSES(pstController);
    s_vPrintNVMeIdentifyControllerDataFNA(pstController);
    s_vPrintNVMeIdentifyControllerDataVWC(pstController);
    s_vPrintNVMeIdentifyControllerDataAWU(pstController);
    s_vPrintNVMeIdentifyControllerDataNVSCC(pstController);

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        s_vPrintNVMeIdentifyControllerDataNWPC(pstController);
    }

    s_vPrintNVMeIdentifyControllerDataACWU(pstController);

    if (bIsNVMeV20OrLater())
    { // revision 2.0 or later
        s_vPrintNVMeIdentifyControllerDataOCFS(pstController);
    }

    s_vPrintNVMeIdentifyControllerDataSGLS(pstController);

    if (bIsNVMeV14OrLater())
    { // revision 1.4 or over
        s_vPrintNVMeIdentifyControllerDataMNAN(pstController);
    }

    if (bIsNVMeV20OrLater())
    { // revision 2.0 or later
        s_vPrintNVMeIdentifyControllerDataMAXDNA(pstController);
        s_vPrintNVMeIdentifyControllerDataMAXCNA(pstController);
    }

    s_vPrintNVMeIdentifyControllerDataSUBNQN(pstController);

    if (bIsNVMeV20OrLater())
    { // revision 2.0 or later
        s_vPrintNVMeIdentifyControllerDataIOCCSZ(pstController);
        s_vPrintNVMeIdentifyControllerDataIORCSZ(pstController);
        s_vPrintNVMeIdentifyControllerDataICDOFF(pstController);
        s_vPrintNVMeIdentifyControllerDataFCATT(pstController);
        s_vPrintNVMeIdentifyControllerDataMSDBD(pstController);
        s_vPrintNVMeIdentifyControllerDataOFCS(pstController);
    }

    s_vPrintNVMeIdentifyControllerDataPSDs(pstController);
}
