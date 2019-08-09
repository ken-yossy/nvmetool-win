#include "WinFunc.h"
#include <iostream>
#include <nvme.h>
#include "NVMeUtils.h"
#include "NVMeIdentifyController.h"

static void s_vPrintNVMeIdentifyControllerDataCMIC(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[O] Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC):\n");

    if (_pstController->CMIC.ANAReport)
    {
        printf("\tbit [      3] 1 = This controller supports Asymmetric Namespace Access Reporting\n");
    }
    else
    {
        printf("\tbit [      3] 0 = This controller does not support Asymmetric Namespace Access Reporting\n");
    }

    if (_pstController->CMIC.SRIOV)
	{
		printf("\tbit [      2] 1 = This controller is associated with an SR-IOV Virtual Function\n");
	}
	else
	{
		printf("\tbit [      2] 0 = This controller is associated with a PCI Function or a Fabrics connection\n");
	}

	if (_pstController->CMIC.MultiControllers)
	{
		printf("\tbit [      1] 1 = This NVM subsystem may contain two or more controllers\n");
	}
	else
	{
		printf("\tbit [      1] 0 = This NVM subsystem contains only a single controller\n");
	}

	if (_pstController->CMIC.MultiPCIePorts)
	{
		printf("\tbit [      0] 1 = This NVM subsystem may contain more than one NVM subsystem port\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This NVM subsystem contains only a single NVM subsystem port\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataOAES(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Optional Asynchronous Events Supported (OAES):\n");

    if (_pstController->OAES.EnduranceGroupEventAggregateLogChange)
    {
        printf("\tbit [     14] 1 = This controller supports the Endurance Group Event Aggregate Log Page Change Notices event\n");
    }
    else
    {
        printf("\tbit [     14] 0 = This controller does not support the Endurance Group Event Aggregate Log Page Change Notices event\n");
    }

    if (_pstController->OAES.LBAStatusInfo)
    {
        printf("\tbit [     13] 1 = This controller supports LBA Status Information Notices event\n");
    }
    else
    {
        printf("\tbit [     13] 0 = This controller does not support LBA Status Information Notices event\n");
    }

    if (_pstController->OAES.PredictableLatencyEventAggregateLogChange)
    {
        printf("\tbit [     12] 1 = This controller supports Predictable Latency Event Aggregate Log Change Notices event\n");
    }
    else
    {
        printf("\tbit [     12] 0 = This controller does not support Predictable Latency Event Aggregate Log Change Notices event\n");
    }

    if (_pstController->OAES.ANAChange)
    {
        printf("\tbit [     11] 1 = This controller supports Asymmetric Namespace Access Change Notices event\n");
    }
    else
    {
        printf("\tbit [     11] 0 = This controller does not support Asymmetric Namespace Access Change Notices event\n");
    }

    if (_pstController->OAES.FirmwareActivation)
	{
		printf("\tbit [      9] 1 = This controller supports sending Firmware Activation Notices\n");
	}
	else
	{
		printf("\tbit [      9] 0 = This controller does not support the Firmware Activation Notices event\n");
	}

	if (_pstController->OAES.NamespaceAttributeChanged)
	{
		printf("\tbit [      8] 1 = This controller supports sending Namespace Attribute Notices and the associated Changed Namespace List log page\n");
	}
	else
	{
		printf("\tbit [      8] 0 = This controller does not support the Namespace Attribute Notices event nor the associated Changed Namespace List log page\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataCTRATT(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Controller Attributes (CTRATT):\n");
    if (_pstController->CTRATT.UUIDList)
    {
        printf("\tbit [      9] 1 = This controller supports reporting of a UUID List\n");
    }
    else
    {
        printf("\tbit [      9] 0 = This controller does not support reporting of a UUID List\n");
    }

    if (_pstController->CTRATT.SQAssociation)
    {
        printf("\tbit [      8] 1 = This controller supports SQ Associations\n");
    }
    else
    {
        printf("\tbit [      8] 0 = This controller does not support SQ Associations\n");
    }

    if (_pstController->CTRATT.NamespaceGranularity)
    {
        printf("\tbit [      7] 1 = This controller supports reporting of Namespace Granularity\n");
    }
    else
    {
        printf("\tbit [      7] 0 = This controller does not support reporting of Namespace Granularity\n");
    }

    if (_pstController->CTRATT.TrafficBasedKeepAlive)
    {
        printf("\tbit [      6] 1 = This controller supports restarting the Keep Alive Timer if an Admin command or an I/O command is processed during the Keep Alive Timeout Interval\n");
    }
    else
    {
        printf("\tbit [      6] 0 = This controller supports restarting the Keep Alive Timer only if a Keep Alive command is processed during the Keep Alive Timeout Interval\n");
    }

    if (_pstController->CTRATT.PredictableLatencyMode)
    {
        printf("\tbit [      5] 1 = This controller supports Predictable Latency Mode\n");
    }
    else
    {
        printf("\tbit [      5] 0 = This controller does not support Predictable Latency Mode\n");
    }

    if (_pstController->CTRATT.EnduranceGroups)
    {
        printf("\tbit [      4] 1 = This controller supports Endurance Groups\n");
    }
    else
    {
        printf("\tbit [      4] 0 = This controller does not support Endurance Groups\n");
    }

    if (_pstController->CTRATT.ReadRecoveryLevel)
    {
        printf("\tbit [      3] 1 = This controller supports Read Recovery Levels\n");
    }
    else
    {
        printf("\tbit [      3] 0 = This controller does not support Read Recovery Levels\n");
    }

    if (_pstController->CTRATT.NVMSet)
    {
        printf("\tbit [      2] 1 = This controller supports NVM Sets\n");
    }
    else
    {
        printf("\tbit [      2] 0 = This controller does not support NVM Sets\n");
    }

    if (_pstController->CTRATT.NoopPSPermissiveModeEn)
	{
		printf("\tbit [      1] 1 = This controller supports host control of whether the controller may temporarily exceed the power of a non-operational power state for the purpose of executing controller initiated background operations in a non-operational power state\n");
	}
	else
	{
		printf("\tbit [      1] 0 = This controller does not support host control of whether the controller may exceed the power of a non-operational state for the purpose of executing controller initiated background operations in a non-operational state\n");
	}

	if (_pstController->CTRATT.HostIdEn)
	{
		printf("\tbit [      0] 1 = This controller supports a 128-bit Host Identifier\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This controller does not support a 128-bit Host Identifier\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataRRLS(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
    printf("[O] Read Recovery Levels Supports (RRLS):\n");
    for (int i = 0; i < 16; i++)
    {
        if ((_pstController->RRLS >> i) & 0x1)
        {
            printf("\tbit [      %d] 1 = Read Recovery Level %d is supported\n", i, i);
        }
        else
        {
            printf("\tbit [      %d] 0 = Read Recovery Level %d is not supported\n", i, i);
        }
    }
}

static void s_vPrintNVMeIdentifyControllerDataCNTRLTYPE(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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
        printf("%02x (invalid value)\n", _pstController->CNTRLTYPE);
        break;
    }
}

static void s_vPrintNVMeIdentifyControllerDataCRDT(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
    printf("[O] Command Retry Delay Time 1 (CDRT1): %d (%d ms)\n",
        _pstController->CRDT1, _pstController->CRDT1 * 100);
    printf("[O] Command Retry Delay Time 2 (CDRT2): %d (%d ms)\n",
        _pstController->CRDT2, _pstController->CRDT2 * 100);
    printf("[O] Command Retry Delay Time 3 (CDRT3): %d (%d ms)\n",
        _pstController->CRDT3, _pstController->CRDT3 * 100);
}

static void s_vPrintNVMeIdentifyControllerDataOACS(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Optional Admin Command Support (OACS):\n");
    if (_pstController->OACS.GetLBAStatusCommand)
    {
        printf("\tbit [      9] 1 = This controller supports the Get LBA Status command\n");
    }
    else
    {
        printf("\tbit [      9] 0 = This controller does not support the Get LBA Status command\n");
    }

    if (_pstController->OACS.DBConfigCommand)
    {
        printf("\tbit [      8] 1 = This controller supports the Doorbell Buffer Config command\n");
    }
    else
    {
        printf("\tbit [      8] 0 = This controller does not support the Doorbell Buffer Config command\n");
    }

    if (_pstController->OACS.VirtMgmtCommands)
	{
		printf("\tbit [      7] 1 = This controller supports the Virtualization Management command\n");
	}
	else
	{
		printf("\tbit [      7] 0 = This controller does not support the Virtualization Management command\n");
	}

	if (_pstController->OACS.NVMeMICommands)
	{
		printf("\tbit [      6] 1 = This controller supports the NVMe-MI Send and NVMe-MI Receive commands\n");
	}
	else
	{
		printf("\tbit [      6] 0 = This controller does not support the NVMe-MI Send and NVMe-MI Receive commands\n");
	}

	if (_pstController->OACS.Directives)
	{
		printf("\tbit [      5] 1 = This controller supports Directives\n");
	}
	else
	{
		printf("\tbit [      5] 0 = This controller does not support Directives\n");
	}

	if (_pstController->OACS.DeviceSelfTest)
	{
		printf("\tbit [      4] 1 = This controller supports the Device Self-test command\n");
	}
	else
	{
		printf("\tbit [      4] 0 = This controller does not support the Device Self-test command\n");
	}

	if (_pstController->OACS.NamespaceCommands)
	{
		printf("\tbit [      3] 1 = This controller supports the Namespace Management and Namespace Attachment commands\n");
	}
	else
	{
		printf("\tbit [      3] 0 = This controller does not support the Namespace Management and Namespace Attachment commands\n");
	}

	if (_pstController->OACS.FirmwareCommands)
	{
		printf("\tbit [      2] 1 = This controller supports the Firmware Commit and Firmware Image Download commands\n");
	}
	else
	{
		printf("\tbit [      2] 0 = This controller does not support the Firmware Commit and Firmware Image Download commands\n");
	}

	if (_pstController->OACS.FormatNVM)
	{
		printf("\tbit [      1] 1 = This controller supports the Format NVM command\n");
	}
	else
	{
		printf("\tbit [      1] 0 = This controller does not support the Format NVM command\n");
	}

	if (_pstController->OACS.SecurityCommands)
	{
		printf("\tbit [      0] 1 = This controller supports the Security Send and Security Receive commands\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This controller does not support the Security Send and Security Receive commands\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataFRMW(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Firmware Updates (FRMW):\n");

	if (_pstController->FRMW.ActivationWithoutReset)
	{
		printf("\tbit [      4] 1 = This controller supports firmware activation without a reset\n");
	}
	else
	{
		printf("\tbit [      4] 0 = This controller does not support firmware activation without a reset\n");
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

static void s_vPrintNVMeIdentifyControllerDataLPA(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Log Page Attributes (LPA):\n");
    if (_pstController->LPA.PersistentEventLog)
    {
        printf("\tbit [      4] 1 = This controller supports the Persistent Event Log\n");
    }
    else
    {
        printf("\tbit [      4] 0 = This controller does not support the Persistent Event Log\n");
    }

    if (_pstController->LPA.TelemetrySupport)
    {
        printf("\tbit [      3] 1 = This controller supports the Telemetry Host-Initiated and Telemetry Controller-Initiated log pages and sending Telemetry Log Notices\n");
    }
    else
    {
        printf("\tbit [      3] 0 = This controller does not support the Telemetry Host-Initiated and Telemetry Controller-Initiated log pages and sending Telemetry Log Notices\n");
    }

    if (_pstController->LPA.LogPageExtendedData)
	{
		printf("\tbit [      2] 1 = This controller supports extended data for Get Log Page (including extended Number of Dwords and Log Page Offset fields)\n");
	}
	else
	{
		printf("\tbit [      2] 0 = This controller does not support extended data for Get Log Page\n");
	}

	if (_pstController->LPA.CommandEffectsLog)
	{
		printf("\tbit [      1] 1 = This controller supports the Commands Supported and Effects log page\n");
	}
	else
	{
		printf("\tbit [      1] 0 = This controller does not support the Commands Supported and Effects log page\n");
	}

	if (_pstController->LPA.SmartPagePerNamespace)
	{
		printf("\tbit [      0] 1 = This controller supports the SMART / Health information log page on a per namespace basis\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This controller does not support the SMART / Health information log page on a per namespace basis\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataAVSCC(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataAPSTA(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[O] Autonomous Power State Transition Attributes (APSTA):\n");
	if (_pstController->APSTA.Supported)
	{
		printf("\tbit [      0] 1 = This controller supports autonomous power state transitions\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This controller does not support autonomous power state transitions\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataCTEMP(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataMTFA(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataHMB(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataRPMB(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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
		printf("\tbit [  2:  0] 0 = This controller does not support RPMB\n");
	}
	else
	{
		printf("\tbit [  2:  0] %d = The number of RPMB targets\n", _pstController->RPMBS.RPMBUnitCount);
	}
}

static void s_vPrintNVMeIdentifyControllerDataEDSTT(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[O] Extended Device Self-test Time (EDSTT): ");
	if (_pstController->OACS.DeviceSelfTest != 0)
	{
		printf("%d = %d minutes\n", _pstController->EDSTT, _pstController->EDSTT);
	}
	else
	{
		printf("0 = DST is not supported\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataDSTO(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[O] Device Self-test Options (DSTO): ");
	if (_pstController->OACS.DeviceSelfTest != 0)
	{
		if (_pstController->DSTO == 1)
		{
			printf("\n\tbit [      0] 1 = The NVM subsystem supports only one device self-test operation in progress at a time\n");
		}
		else if (_pstController->DSTO == 0)
		{
			printf("\n\tbit [      0] 0 = The NVM subsystem supports one device self-test operation per controller at a time\n");
		}
	}
	else
	{
		printf("0 = DST is not supported\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataFWUG(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataKAS(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataThermalMgmt(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[O] Host Controlled Thermal Management Attributes (HCTMA):\n");
	if (_pstController->HCTMA.Supported)
	{
		printf("\tbit [      0] 1 = The controller supports host controlled thermal management\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The controller does not support host controlled thermal management\n");
	}

	printf("[O] Minimum Thermal Management Temperature (MNTMT): ");
	if (_pstController->HCTMA.Supported == 0)
	{
		printf("N/A = The controller does not support host controlled thermal management\n");
	}
	else if (_pstController->MNTMT == 0)
	{
		printf("0 = The controller does not report this field\n");
	}
	else
	{
		printf("%d = %d (K), %d (C)\n", _pstController->MNTMT, _pstController->MNTMT, _pstController->MNTMT - 273);
	}

	printf("[O] Maximum Thermal Management Temperature (MXTMT): ");
	if (_pstController->HCTMA.Supported == 0)
	{
		printf("N/A = The controller does not support host controlled thermal management\n");
	}
	else if (_pstController->MNTMT == 0)
	{
		printf("0 = The controller does not report this field\n");
	}
	else
	{
		printf("%d = %d (K), %d (C)\n", _pstController->MXTMT, _pstController->MXTMT, _pstController->MXTMT - 273);
	}
}

static void s_vPrintNVMeIdentifyControllerDataSANICAP(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[O] Sanitize Capabilities (SANICAP): ");
	if (_pstController->SANICAP.Overwrite | _pstController->SANICAP.CryptoErase | _pstController->SANICAP.BlockErase)
	{
		printf("\n");
        switch (_pstController->SANICAP.NoDeallocateInhibited)
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

		if (_pstController->SANICAP.Overwrite)
		{
			printf("\tbit [      2] 1 = The controller supports the Overwrite sanitize operation\n");
		}
		else
		{
			printf("\tbit [      2] 0 = The controller does not support the Overwrite sanitize operation\n");
		}
		if (_pstController->SANICAP.BlockErase)
		{
			printf("\tbit [      1] 1 = The controller supports the Block Erase sanitize operation\n");
		}
		else
		{
			printf("\tbit [      1] 0 = The controller does not support the Block Erase sanitize operation\n");
		}
		if (_pstController->SANICAP.CryptoErase)
		{
			printf("\tbit [      0] 1 = The controller supports the Crypto Erase sanitize operation\n");
		}
		else
		{
			printf("\tbit [      0] 0 = The controller does not support the Crypto Erase sanitize operation\n");
		}
	}
	else
	{
		printf("0 = The Sanitize command is not supported\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataHMMINDS(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataHMMAXD(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataNSETIDMAX(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
    printf("[O] NVM Set Identifier Maximum (NSETIDMAX): %d\n", _pstController->NSETIDMAX);
}

static void s_vPrintNVMeIdentifyControllerDataENDGIDMAX(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
    printf("[O] Endurance Group Identifier Maximum (ENDGIDMAX): %d\n", _pstController->ENDGIDMAX);
}

static void s_vPrintNVMeIdentifyControllerDataANATT(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataANACAP(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
    printf("[O] Asymmetric Namespace Access Capabilities (ANACAP):\n");
    if (_pstController->ANACAP.NonZeroGRPIDSupport)
    {
        printf("\tbit [      7] 1 = This controller supports a non-zero value in the ANAGRPID field of the Namespace Management command\n");
    }
    else
    {
        printf("\tbit [      7] 0 = This controller does not support a non-zero value in the ANAGRPID field of the Namespace Management command\n");
    }

    if (_pstController->ANACAP.NoGRPIDChangeDuringAttached)
    {
        printf("\tbit [      6] 1 = ANAGRPID field in the Identify Namespace data structure does not change while the namespace is attached to any controller\n");
    }
    else
    {
        printf("\tbit [      6] 0 = ANAGRPID field may change while the namespace is attached to any controller\n");
    }

    if (_pstController->ANACAP.ReportChangeState)
    {
        printf("\tbit [      4] 1 = This controller reports ANA Change state\n");
    }
    else
    {
        printf("\tbit [      4] 0 = This controller does not report ANA Change state\n");
    }

    if (_pstController->ANACAP.ReportPersistentLossState)
    {
        printf("\tbit [      3] 1 = This controller reports ANA Persistent Loss state\n");
    }
    else
    {
        printf("\tbit [      3] 0 = This controller does not report ANA Persistent Loss state\n");
    }

    if (_pstController->ANACAP.ReportInaccessibleState)
    {
        printf("\tbit [      2] 1 = This controller reports ANA Inaccessible state\n");
    }
    else
    {
        printf("\tbit [      2] 0 = This controller does not report ANA Inaccessible state\n");
    }

    if (_pstController->ANACAP.ReportNonOptimizedState)
    {
        printf("\tbit [      1] 1 = This controller reports ANA Non-Optimized state\n");
    }
    else
    {
        printf("\tbit [      1] 0 = This controller does not report ANA Non-Optimized state\n");
    }

    if (_pstController->ANACAP.ReportOptimizedState)
    {
        printf("\tbit [      0] 1 = This controller reports ANA Optimized state\n");
    }
    else
    {
        printf("\tbit [      0] 0 = This controller does not report ANA Optimized state\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataANAGRPMAX(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataNANAGRPID(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataPELS(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataSQES(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Submission Queue Entry Size (SQES):\n");

	printf("\tbit [  7:  4] %d = %d bytes = The maximum Submission Queue entry size when using the NVM Command Set\n",
		_pstController->SQES.MaxEntrySize, 1 << _pstController->SQES.MaxEntrySize);
	printf("\tbit [  3:  0] %d = %d bytes = The required Submission Queue Entry size when using the NVM Command Set\n",
		_pstController->SQES.RequiredEntrySize, 1 << _pstController->SQES.RequiredEntrySize);
}

static void s_vPrintNVMeIdentifyControllerDataCQES(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Completion Queue Entry Size (SQES):\n");

	printf("\tbit [  7:  4] %d = %d bytes = The maximum Completion Queue entry size when using the NVM Command Set\n",
		_pstController->CQES.MaxEntrySize, 1 << _pstController->CQES.MaxEntrySize);
	printf("\tbit [  3:  0] %d = %d bytes = The required Completion Queue Entry size when using the NVM Command Set\n",
		_pstController->CQES.RequiredEntrySize, 1 << _pstController->CQES.RequiredEntrySize);
}

static void s_vPrintNVMeIdentifyControllerDataMAXCMD(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataONCS(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Optional NVM Command Support (ONCS):\n");

    if (_pstController->ONCS.Verify)
    {
        printf("\tbit [      7] 1 = The controller supports the Verify command\n");
    }
    else
    {
        printf("\tbit [      7] 0 = The controller does not support the Verify command\n");
    }

    if (_pstController->ONCS.Timestamp)
    {
        printf("\tbit [      6] 1 = The controller supports the Timestamp feature\n");
    }
    else
    {
        printf("\tbit [      6] 0 = The controller does not support the Timestamp feature\n");
    }

    if (_pstController->ONCS.Reservations)
	{
		printf("\tbit [      5] 1 = The controller supports reservations\n");
	}
	else
	{
		printf("\tbit [      5] 0 = The controller does not support reservations\n");
	}

	if (_pstController->ONCS.FeatureField)
	{
		printf("\tbit [      4] 1 = The controller supports the Save field set to a non-zero value in the Set Features command and the Select field set to a non-zero value in the Get Features command\n");
	}
	else
	{
		printf("\tbit [      4] 0 = The controller does not support the Save field set to a non-zero value in the Set Features command and the Select field set to a non-zero value in the Get Features command\n");
	}

	if (_pstController->ONCS.WriteZeroes)
	{
		printf("\tbit [      3] 1 = The controller supports the Write Zeroes command\n");
	}
	else
	{
		printf("\tbit [      3] 0 = The controller does not support the Write Zeroes command\n");
	}

	if (_pstController->ONCS.DatasetManagement)
	{
		printf("\tbit [      2] 1 = The controller supports the Dataset Management command\n");
	}
	else
	{
		printf("\tbit [      2] 0 = The controller does not support the Dataset Management command\n");
	}

	if (_pstController->ONCS.WriteUncorrectable)
	{
		printf("\tbit [      1] 1 = The controller supports the Write Uncorrectable command\n");
	}
	else
	{
		printf("\tbit [      1] 0 = The controller does not support the Write Uncorrectable command\n");
	}

	if (_pstController->ONCS.Compare)
	{
		printf("\tbit [      0] 1 = The controller supports the Compare command\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The controller does not support the Compare command\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataFUSES(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Fused Operation Support (FUSES):\n");
	if (_pstController->FUSES.CompareAndWrite)
	{
		printf("\tbit [      0] 1 = The controller supports the Compare and Write fused operation\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The controller does not support the Compare and Write fused operation\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataFNA(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[M] Format NVM Attributes (FNA):\n");
	if (_pstController->FNA.CryptographicEraseSupported)
	{
		printf("\tbit [      2] 1 = The controller supports cryptographic erase operation\n");
	}
	else
	{
		printf("\tbit [      2] 0 = The controller does not support cryptographic erase operation\n");
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

static void s_vPrintNVMeIdentifyControllerDataVWC(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
    printf("[M] Volatile Write Cache (VWC):\n");
    if (_pstController->VWC.Present)
    {
        switch (_pstController->VWC.FlushBehavior)
        {
        case 1:
            printf("\tbit [  2:  1] 1 = (reserved value)\n");
            break;

        case 2:
            printf("\tbit [  2:  1] 2 = does not support Flush command with NSID field set to FFFFFFFFh\n");
            break;

        case 3:
            printf("\tbit [  2:  1] 2 = support Flush command with NSID field set to FFFFFFFFh\n");
            break;

        case 0:
        default:
            printf("\tbit [  2:  1] 0 = support Flush command with NSID field set to FFFFFFFFh is not indicated\n");
            break;
        }
        printf("\tbit [      0] 1 = A volatile write cache is present\n");
    }
    else
    {
        printf("\tbit [      0] 0 = A volatile write cache is not present\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataNVSCC(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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


static void s_vPrintNVMeIdentifyControllerDataAWU(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataNWPC(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
    printf("[M] Namespace Write Protection Capabilities (NWPC):\n");
    if (_pstController->NWPC.PermanentWriteProtectSupport)
    {
        printf("\tbit [      2] 1 = This controller supports the Permanent Write Protect state\n");
    }
    else
    {
        printf("\tbit [      2] 0 = This controller does not support the Permanent Write Protect state\n");
    }

    if (_pstController->NWPC.WriteProtectUntilPowerCycleSupport)
    {
        printf("\tbit [      1] 1 = This controller supports the Write Protect Until Power Cycle state\n");
    }
    else
    {
        printf("\tbit [      1] 0 = This controller does not support the Write Protect Until Power Cycle state\n");
    }

    if (_pstController->NWPC.NoWriteProtectSupport)
    {
        printf("\tbit [      0] 1 = This controller supports No Write Protect and Write Protect namespace write protection states\n");
    }
    else
    {
        printf("\tbit [      0] 0 = This controller does not support the Namespace Write Protection\n");
    }
}

static void s_vPrintNVMeIdentifyControllerDataACWU(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[O] Atomic Compare & Write Unit (ACWU): ");
	if (_pstController->ACWU == 0)
	{
		printf("0 = Compare and Write is not a supported in fused command\n");
	}
	else
	{
		printf("%d = means %d sector(s)\n", _pstController->ACWU, _pstController->ACWU + 1);
	}
}

static void s_vPrintNVMeIdentifyControllerDataSGLS(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
{
	printf("[O] SGL Support (SGLS):\n");

	if (_pstController->SGLS.SGLSupported == 0)
	{
		printf("\tbit [  1:  0] 0 = SGLs are not supported\n");
		return;
	}

    if (_pstController->SGLS.TransactionalSGLDataBlockDescSupported)
    {
        printf("\tbit [     21] 1 = The controller supports Transport SGL Data Block descriptor\n");
    }
    else
    {
        printf("\tbit [     21] 0 = The controller does not support Transport SGL Data Block descriptor\n");
    }

    if (_pstController->SGLS.OffsetByAddrFieldSupported)
	{
		printf("\tbit [     20] 1 = The controller supports the Address field in SGL Data Block, SGL Segment, and SGL Last Segment descriptor types specifying an offset\n");
	}
	else
	{
		printf("\tbit [     20] 0 = The controller does not support the Address field specifying an offset\n");
	}

	if (_pstController->SGLS.MPTRContainingSGLDescSupported)
	{
		printf("\tbit [     19] 1 = The controller supports the use of a Metadata Pointer (MPTR) that contains an address of an SGL segment containing exactly one SGL Descriptor that is Qword aligned\n");
	}
	else
	{
		printf("\tbit [     19] 0 = The controller does not support the use of a Metadata Pointer (MPTR) that contains an address of an SGL segment containing exactly one SGL Descriptor that is Qword aligned\n");
	}

	if (_pstController->SGLS.SGLLengthLargerThanDataLength)
	{
		printf("\tbit [     18] 1 = The controller supports commands that contain a data or metadata SGL of a length larger than the amount of data to be transferred\n");
	}
	else
	{
		printf("\tbit [     18] 0 = The controller requires the SGL length is equal to the amount of data to be transferred\n");
	}

	if (_pstController->SGLS.ByteAlignedContiguousPhysicalBuffer)
	{
		printf("\tbit [     17] 1 = The controller supports the use of a byte aligned contiguous physical buffer of metadata\n");
	}
	else
	{
		printf("\tbit [     17] 0 = The controller does not supports the use of a byte aligned contiguous physical buffer of metadata\n");
	}

	if (_pstController->SGLS.BitBucketDescrSupported)
	{
		printf("\tbit [     16] 1 = The controller supports the SGL Bit Bucket descriptor\n");
	}
	else
	{
		printf("\tbit [     16] 0 = The controller does not supports the SGL Bit Bucket descriptor\n");
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

static void s_vPrintNVMeIdentifyControllerDataMNAN(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataSUBNQN(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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
		printf("no workload\n");
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

	printf("\tbit [     25] %d (Non-Operational State (NOPS)) = ", pDesc->NOPS);
	if (pDesc->NOPS == 0)
	{
		printf("This power state is operational.\n");
	}
	else
	{
		printf("This power state is non-operational.\n");
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

static void s_vPrintNVMeIdentifyControllerDataPSDs(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

static void s_vPrintNVMeIdentifyControllerDataFGUID(PNVME_IDENTIFY_CONTROLLER_DATA14 _pstController)
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

void vPrintNVMeIdentifyControllerData14(void)
{
    PNVME_IDENTIFY_CONTROLLER_DATA14 pstController = (PNVME_IDENTIFY_CONTROLLER_DATA14)&(g_stController);

	printf("[M] PCI Vendor ID (VID): 0x%X\n", pstController->VID);
	printf("[M] PCI Subsystem Vendor ID (SSVID): 0x%X\n", pstController->SSVID);

    {
        char buf[21];
        ZeroMemory(buf, 21);
        strncpy_s(buf, (const char*)(pstController->SN), 20);
        buf[20] = '\0';
        printASCII("[M] SerialNumber (SN): ", (const char*)buf, true);
    }

    {
        char buf[41];
        ZeroMemory(buf, 41);
        strncpy_s(buf, (const char*)(pstController->MN), 40);
        buf[40] = '\0';
        printASCII("[M] Model Number (MN): ", (const char*)buf, true);
    }

    {
        char buf[9];
        ZeroMemory(buf, 9);
        strncpy_s(buf, (const char*)(pstController->FR), 8);
        buf[8] = '\0';
        printASCII("[M] Firmware Revision (FR): ", (const char*)buf, true);
    }

	printf("[M] Recommended Arbitration Burst (RAB): %d (means %d)\n", pstController->RAB, 1 << pstController->RAB);
	printf("[M] IEEE OUI Identifier (IEEE): %02X-%02X-%02X\n", pstController->IEEE[2], pstController->IEEE[1], pstController->IEEE[0]);

	s_vPrintNVMeIdentifyControllerDataCMIC(pstController);

	printf("[M] Maximum Data Transfer Size (MDTS): %d (means %d)\n", pstController->MDTS, 1 << pstController->MDTS);
	printf("[M] Controller ID (CNTLID): 0x%x\n", pstController->CNTLID);
	printf("[M] Version (VER): 0x%x (NVMe Revision %d.%d.%d)\n",
        pstController->VER, (pstController->VER >> 16) & 0xFFFF, (pstController->VER >> 8) & 0xFF, pstController->VER & 0xFF);
    printf("[M] RTD3 Resume Latency (RTD3R): %d (usec)\n", pstController->RTD3R);
	printf("[M] RTD3 Entry Latency (RTD3E): %d (usec)\n", pstController->RTD3E);

	s_vPrintNVMeIdentifyControllerDataOAES(pstController);
    s_vPrintNVMeIdentifyControllerDataCTRATT(pstController);
    s_vPrintNVMeIdentifyControllerDataRRLS(pstController);
    s_vPrintNVMeIdentifyControllerDataCNTRLTYPE(pstController);
    s_vPrintNVMeIdentifyControllerDataFGUID(pstController);
    s_vPrintNVMeIdentifyControllerDataCRDT(pstController);
    s_vPrintNVMeIdentifyControllerDataOACS(pstController);

	printf("[M] Abort Command Limit (ACL): %d (means %d)\n", pstController->ACL, pstController->ACL + 1);
	printf("[M] Asynchronous Event Request Limit (AERL): %d (means %d)\n", pstController->AERL, pstController->AERL + 1);

	s_vPrintNVMeIdentifyControllerDataFRMW(pstController);
	s_vPrintNVMeIdentifyControllerDataLPA(pstController);

	printf("[M] Error Log Page Entries (ELPE): %d (means %d)\n", pstController->ELPE, pstController->ELPE + 1);
	printf("[M] Number of Power States Support (NPSS): %d (means %d)\n", pstController->NPSS, pstController->NPSS + 1);
	s_vPrintNVMeIdentifyControllerDataAVSCC(pstController);
	s_vPrintNVMeIdentifyControllerDataAPSTA(pstController);
	s_vPrintNVMeIdentifyControllerDataCTEMP(pstController);
	s_vPrintNVMeIdentifyControllerDataMTFA(pstController);
	s_vPrintNVMeIdentifyControllerDataHMB(pstController);

	if (pstController->OACS.NamespaceCommands)
	{
		printf("[O] Total NVM Capacity (TNVMCAP): %llu (byte)\n", (uint64_t)(pstController->TNVMCAP));
		printf("[O] Unallocated NVM Capacity (UNVMCAP): %llu (byte)\n", (uint64_t)(pstController->UNVMCAP));
	}

	s_vPrintNVMeIdentifyControllerDataRPMB(pstController);
	s_vPrintNVMeIdentifyControllerDataEDSTT(pstController);
	s_vPrintNVMeIdentifyControllerDataDSTO(pstController);
	s_vPrintNVMeIdentifyControllerDataFWUG(pstController);
	s_vPrintNVMeIdentifyControllerDataKAS(pstController);
	s_vPrintNVMeIdentifyControllerDataThermalMgmt(pstController);
	s_vPrintNVMeIdentifyControllerDataSANICAP(pstController);
    s_vPrintNVMeIdentifyControllerDataHMMINDS(pstController);
    s_vPrintNVMeIdentifyControllerDataHMMAXD(pstController);
    s_vPrintNVMeIdentifyControllerDataNSETIDMAX(pstController);
    s_vPrintNVMeIdentifyControllerDataENDGIDMAX(pstController);
    s_vPrintNVMeIdentifyControllerDataANATT(pstController);
    s_vPrintNVMeIdentifyControllerDataANACAP(pstController);
    s_vPrintNVMeIdentifyControllerDataANAGRPMAX(pstController);
    s_vPrintNVMeIdentifyControllerDataNANAGRPID(pstController);
    s_vPrintNVMeIdentifyControllerDataPELS(pstController);

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
    s_vPrintNVMeIdentifyControllerDataNWPC(pstController);
    s_vPrintNVMeIdentifyControllerDataACWU(pstController);
	s_vPrintNVMeIdentifyControllerDataSGLS(pstController);
    s_vPrintNVMeIdentifyControllerDataMNAN(pstController);
    s_vPrintNVMeIdentifyControllerDataSUBNQN(pstController);
	s_vPrintNVMeIdentifyControllerDataPSDs(pstController);
}
