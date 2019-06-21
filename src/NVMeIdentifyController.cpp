#include "WinFunc.h"
#include <iostream>
#include <nvme.h>
#include "NVMeUtils.h"
#include "NVMeIdentifyController.h"

static void s_vPrintNVMeIdentifyControllerDataCMIC(void)
{
	printf("[O] Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC):\n");

	if (g_stController.CMIC.SRIOV)
	{
		printf("\tbit [      2] 1 = This controller is associated with an SR-IOV Virtual Function\n");
	}
	else
	{
		printf("\tbit [      2] 0 = This controller is associated with a PCI Function or a Fabrics connection\n");
	}

	if (g_stController.CMIC.MultiControllers)
	{
		printf("\tbit [      1] 1 = This NVM subsystem may contain two or more controllers\n");
	}
	else
	{
		printf("\tbit [      1] 0 = This NVM subsystem contains only a single controller\n");
	}

	if (g_stController.CMIC.MultiPCIePorts)
	{
		printf("\tbit [      0] 1 = This NVM subsystem may contain more than one NVM subsystem port\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This NVM subsystem contains only a single NVM subsystem port\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataOAES(void)
{
	printf("[M] Optional Asynchronous Events Supported (OAES):\n");

	if (g_stController.OAES.FirmwareActivation)
	{
		printf("\tbit [      9] 1 = This controller supports sending Firmware Activation Notices\n");
	}
	else
	{
			printf("\tbit [      9] 0 = This controller does not support the Firmware Activation Notices event\n");
	}

	if (g_stController.OAES.NamespaceAttributeChanged)
	{
		printf("\tbit [      8] 1 = This controller supports sending Namespace Attribute Notices and the associated Changed Namespace List log page\n");
	}
	else
	{
		printf("\tbit [      8] 0 = This controller does not support the Namespace Attribute Notices event nor the associated Changed Namespace List log page\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataCTRATT(void)
{
	printf("[M] Controller Attributes (CTRATT):\n");
	if (g_stController.CTRATT.NoopPSPermissiveModeEn)
	{
		printf("\tbit [      1] 1 = This controller supports host control of whether the controller may temporarily exceed the power of a non-operational power state for the purpose of executing controller initiated background operations in a non-operational power state\n");
	}
	else
	{
		printf("\tbit [      1] 0 = This controller does not support host control of whether the controller may exceed the power of a non-operational state for the purpose of executing controller initiated background operations in a non-operational state\n");
	}

	if (g_stController.CTRATT.HostIdEn)
	{
		printf("\tbit [      0] 1 = This controller supports a 128-bit Host Identifier\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This controller does not support a 128-bit Host Identifier\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataOACS(void)
{
	printf("[M] Optional Admin Command Support (OACS):\n");
	if (g_stController.OACS.DBConfigCommand)
	{
		printf("\tbit [      8] 1 = This controller supports the Doorbell Buffer Config command\n");
	}
	else
	{
		printf("\tbit [      8] 0 = This controller does not support the Doorbell Buffer Config command\n");
	}

	if (g_stController.OACS.VirtMgmtCommands)
	{
		printf("\tbit [      7] 1 = This controller supports the Virtualization Management command\n");
	}
	else
	{
		printf("\tbit [      7] 0 = This controller does not support the Virtualization Management command\n");
	}

	if (g_stController.OACS.NVMeMICommands)
	{
		printf("\tbit [      6] 1 = This controller supports the NVMe-MI Send and NVMe-MI Receive commands\n");
	}
	else
	{
		printf("\tbit [      6] 0 = This controller does not support the NVMe-MI Send and NVMe-MI Receive commands\n");
	}

	if (g_stController.OACS.Directives)
	{
		printf("\tbit [      5] 1 = This controller supports Directives\n");
	}
	else
	{
		printf("\tbit [      5] 0 = This controller does not support Directives\n");
	}

	if (g_stController.OACS.DeviceSelfTest)
	{
		printf("\tbit [      4] 1 = This controller supports the Device Self-test command\n");
	}
	else
	{
		printf("\tbit [      4] 0 = This controller does not support the Device Self-test command\n");
	}

	if (g_stController.OACS.NamespaceCommands)
	{
		printf("\tbit [      3] 1 = This controller supports the Namespace Management and Namespace Attachment commands\n");
	}
	else
	{
		printf("\tbit [      3] 0 = This controller does not support the Namespace Management and Namespace Attachment commands\n");
	}

	if (g_stController.OACS.FirmwareCommands)
	{
		printf("\tbit [      2] 1 = This controller supports the Firmware Commit and Firmware Image Download commands\n");
	}
	else
	{
		printf("\tbit [      2] 0 = This controller does not support the Firmware Commit and Firmware Image Download commands\n");
	}

	if (g_stController.OACS.FormatNVM)
	{
		printf("\tbit [      1] 1 = This controller supports the Format NVM command\n");
	}
	else
	{
		printf("\tbit [      1] 0 = This controller does not support the Format NVM command\n");
	}

	if (g_stController.OACS.SecurityCommands)
	{
		printf("\tbit [      0] 1 = This controller supports the Security Send and Security Receive commands\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This controller does not support the Security Send and Security Receive commands\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataFRMW(void)
{
	printf("[M] Firmware Updates (FRMW):\n");

	if (g_stController.FRMW.ActivationWithoutReset)
	{
		printf("\tbit [      4] 1 = This controller supports firmware activation without a reset\n");
	}
	else
	{
		printf("\tbit [      4] 0 = This controller does not support firmware activation without a reset\n");
	}

	printf("\tbit [  3:  1] %d = The number of firmware slots that the controller supports\n", g_stController.FRMW.SlotCount);

	if (g_stController.FRMW.Slot1ReadOnly)
	{
		printf("\tbit [      0] 1 = The first firmware slot (slot 1) is read only\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The first firmware slot (slot 1) is read/write\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataLPA(void)
{
	printf("[M] Log Page Attributes (LPA):\n");
	if (g_stController.LPA.TelemetrySupport)
	{
			printf("\tbit [      3] 1 = This controller supports the Telemetry Host-Initiated and Telemetry Controller-Initiated log pages and sending Telemetry Log Notices\n");
	}
	else
	{
		printf("\tbit [      3] 0 = This controller does not support the Telemetry Host-Initiated and Telemetry Controller-Initiated log pages and sending Telemetry Log Notices\n");
	}

	if (g_stController.LPA.LogPageExtendedData)
	{
		printf("\tbit [      2] 1 = This controller supports extended data for Get Log Page (including extended Number of Dwords and Log Page Offset fields)\n");
	}
	else
	{
		printf("\tbit [      2] 0 = This controller does not support extended data for Get Log Page\n");
	}

	if (g_stController.LPA.CommandEffectsLog)
	{
		printf("\tbit [      1] 1 = This controller supports the Commands Supported and Effects log page\n");
	}
	else
	{
		printf("\tbit [      1] 0 = This controller does not support the Commands Supported and Effects log page\n");
	}

	if (g_stController.LPA.SmartPagePerNamespace)
	{
		printf("\tbit [      0] 1 = This controller supports the SMART / Health information log page on a per namespace basis\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This controller does not support the SMART / Health information log page on a per namespace basis\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataAVSCC(void)
{
	printf("[M] Admin Vendor Specific Command Configuration (AVSCC):\n");
	if (g_stController.AVSCC.CommandFormatInSpec)
	{
		printf("\tbit [      0] 1 = All Admin Vendor Specific Commands use the format defined in the specification\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The format of all Admin Vendor Specific Commands are vendor specific\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataAPSTA(void)
{
	printf("[O] Autonomous Power State Transition Attributes (APSTA):\n");
	if (g_stController.APSTA.Supported)
	{
		printf("\tbit [      0] 1 = This controller supports autonomous power state transitions\n");
	}
	else
	{
		printf("\tbit [      0] 0 = This controller does not support autonomous power state transitions\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataCTEMP(void)
{
	printf("[M] Warning Composite Temperature Threshold (WCTEMP): ");
	if (g_stController.WCTEMP == 0)
	{
		printf("0 = undefined\n");
	}
	else
	{
		printf("%d = %d (K), %d (C)\n", g_stController.WCTEMP, g_stController.WCTEMP, g_stController.WCTEMP - 273);
	}

	printf("[M] Critical Composite Temperature Threshold (CCTEMP): ");
	if (g_stController.CCTEMP == 0)
	{
		printf("0 = undefined\n");
	}
	else
	{
		printf("%d = %d (K), %d (C)\n", g_stController.CCTEMP, g_stController.CCTEMP, g_stController.CCTEMP - 273);
	}
}

static void s_vPrintNVMeIdentifyControllerDataMTFA(void)
{
	printf("[O] Maximum Time for Firmware Activation (MTFA): ");
	if (g_stController.MTFA == 0)
	{
		printf("0 = undefined\n");
	}
	else
	{
		printf("%d = %d msec\n", g_stController.MTFA, g_stController.MTFA * 100);
	}
}

static void s_vPrintNVMeIdentifyControllerDataHMB(void)
{
	printf("[O] Host Memory Buffer Preferred Size (HMPRE): ");
	if (g_stController.HMPRE == 0)
	{
		printf("0 = HMB is not supported\n");
	}
	else
	{
		printf("%d = %d KiB\n", g_stController.HMPRE, g_stController.HMPRE * 4);
	}

	printf("[O] Host Memory Buffer Minimum Size (HMMIN): ");
	if (g_stController.HMMIN == 0)
	{
		printf("0 = HMB is not supported\n");
	}
	else
	{
		printf("%d = %d KiB\n", g_stController.HMMIN, g_stController.HMMIN * 4);
	}
}

static void s_vPrintNVMeIdentifyControllerDataRPMB(void)
{
	printf("[O] Replay Protected Memory Block Support (RPMBS):\n");

	if (g_stController.RPMBS.RPMBUnitCount != 0)
	{
		printf("\tbit [ 31: 24] %d = %d bytes\n",
			g_stController.RPMBS.AccessSize, (g_stController.RPMBS.AccessSize + 1) * 512);
		printf("\tbit [ 23: 16] %d = %d KiB\n",
			g_stController.RPMBS.TotalSize, (g_stController.RPMBS.TotalSize + 1) * 128);
		if (g_stController.RPMBS.AuthenticationMethod == 0) {
			printf("\tbit [  5:  3] %d = means HMAC SHA-256\n",
				g_stController.RPMBS.AuthenticationMethod);
		}
	}

	if (g_stController.RPMBS.RPMBUnitCount == 0)
	{
		printf("\tbit [  2:  0] 0 = This controller does not support RPMB\n");
	}
	else
	{
		printf("\tbit [  2:  0] %d = The number of RPMB targets\n", g_stController.RPMBS.RPMBUnitCount);
	}
}

static void s_vPrintNVMeIdentifyControllerDataEDSTT(void)
{
	printf("[O] Extended Device Self-test Time (EDSTT): ");
	if (g_stController.OACS.DeviceSelfTest != 0)
	{
		printf("%d = %d minutes\n", g_stController.EDSTT, g_stController.EDSTT);
	}
	else
	{
		printf("0 = DST is not supported\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataDSTO(void)
{
	printf("[O] Device Self-test Options (DSTO): ");
	if (g_stController.OACS.DeviceSelfTest != 0)
	{
		if (g_stController.DSTO == 1)
		{
			printf("\n\tbit [      0] 1 = The NVM subsystem supports only one device self-test operation in progress at a time\n");
		}
		else if (g_stController.DSTO == 0)
		{
			printf("\n\tbit [      0] 0 = The NVM subsystem supports one device self-test operation per controller at a time\n");
		}
	}
	else
	{
		printf("0 = DST is not supported\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataFWUG(void)
{
	printf("[M] Firmware Update Granularity (FWUG): ");
	if (g_stController.FWUG == 0xFF)
	{
		printf("FFh = No limitation\n");
	}
	else if (g_stController.FWUG == 0)
	{
		printf("00h = no information\n");
	}
	else
	{
		printf("%02Xh = means %d KiB\n", g_stController.FWUG, g_stController.FWUG * 4);
	}
}

static void s_vPrintNVMeIdentifyControllerDataKAS(void)
{
	printf("[M] Keep Alive Support (KAS): ");
	if (g_stController.KAS == 0)
	{
		printf("0 = Not supported\n");
	}
	else
	{
		printf("1 = Supported, The granularity of the Keep Alive Timer is %d msec\n", g_stController.KAS * 100);
	}
}

static void s_vPrintNVMeIdentifyControllerDataThermalMgmt(void)
{
	printf("[O] Host Controlled Thermal Management Attributes (HCTMA):\n");
	if (g_stController.HCTMA.Supported)
	{
		printf("\tbit [      0] 1 = The controller supports host controlled thermal management\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The controller does not support host controlled thermal management\n");
	}

	printf("[O] Minimum Thermal Management Temperature (MNTMT): ");
	if (g_stController.HCTMA.Supported == 0)
	{
		printf("N/A = The controller does not support host controlled thermal management\n");
	}
	else if (g_stController.MNTMT == 0)
	{
		printf("0 = The controller does not report this field\n");
	}
	else
	{
		printf("%d = %d (K), %d (C)\n", g_stController.MNTMT, g_stController.MNTMT, g_stController.MNTMT - 273);
	}

	printf("[O] Maximum Thermal Management Temperature (MXTMT): ");
	if (g_stController.HCTMA.Supported == 0)
	{
		printf("N/A = The controller does not support host controlled thermal management\n");
	}
	else if (g_stController.MNTMT == 0)
	{
		printf("0 = The controller does not report this field\n");
	}
	else
	{
		printf("%d = %d (K), %d (C)\n", g_stController.MXTMT, g_stController.MXTMT, g_stController.MXTMT - 273);
	}
}

static void s_vPrintNVMeIdentifyControllerDataSANICAP(void)
{
	printf("[O] Sanitize Capabilities (SANICAP): ");
	if (g_stController.SANICAP.Overwrite | g_stController.SANICAP.CryptoErase | g_stController.SANICAP.BlockErase)
	{
		printf("\n");
		if (g_stController.SANICAP.Overwrite)
		{
			printf("\tbit [      2] 1 = The controller supports the Overwrite sanitize operation\n");
		}
		else
		{
			printf("\tbit [      2] 0 = The controller does not support the Overwrite sanitize operation\n");
		}
		if (g_stController.SANICAP.BlockErase)
		{
			printf("\tbit [      1] 1 = The controller supports the Block Erase sanitize operation\n");
		}
		else
		{
			printf("\tbit [      1] 0 = The controller does not support the Block Erase sanitize operation\n");
		}
		if (g_stController.SANICAP.CryptoErase)
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

static void s_vPrintNVMeIdentifyControllerDataSQES(void)
{
	printf("[M] Submission Queue Entry Size (SQES):\n");

	printf("\tbit [  7:  4] %d = %d bytes = The maximum Submission Queue entry size when using the NVM Command Set\n",
		g_stController.SQES.MaxEntrySize, 1 << g_stController.SQES.MaxEntrySize);
	printf("\tbit [  3:  0] %d = %d bytes = The required Submission Queue Entry size when using the NVM Command Set\n",
		g_stController.SQES.RequiredEntrySize, 1 << g_stController.SQES.RequiredEntrySize);
}

static void s_vPrintNVMeIdentifyControllerDataCQES(void)
{
	printf("[M] Completion Queue Entry Size (SQES):\n");

	printf("\tbit [  7:  4] %d = %d bytes = The maximum Completion Queue entry size when using the NVM Command Set\n",
		g_stController.CQES.MaxEntrySize, 1 << g_stController.CQES.MaxEntrySize);
	printf("\tbit [  3:  0] %d = %d bytes = The required Completion Queue Entry size when using the NVM Command Set\n",
		g_stController.CQES.RequiredEntrySize, 1 << g_stController.CQES.RequiredEntrySize);
}

static void s_vPrintNVMeIdentifyControllerDataMAXCMD(void)
{
	printf("[M] Maximum Outstanding Commands (MAXCMD): ");
	if (g_stController.MAXCMD == 0)
	{
		printf("0 = unused\n");
	}
	else
	{
		printf("%d\n", g_stController.MAXCMD);
	}
}

static void s_vPrintNVMeIdentifyControllerDataONCS(void)
{
	printf("[M] Optional NVM Command Support (ONCS):\n");

	if (g_stController.ONCS.Timestamp)
	{
		printf("\tbit [      6] 1 = The controller supports the Timestamp feature\n");
	}
	else
	{
		printf("\tbit [      6] 0 = The controller does not support the Timestamp feature\n");
	}

	if (g_stController.ONCS.Reservations)
	{
		printf("\tbit [      5] 1 = The controller supports reservations\n");
	}
	else
	{
		printf("\tbit [      5] 0 = The controller does not support reservations\n");
	}

	if (g_stController.ONCS.FeatureField)
	{
		printf("\tbit [      4] 1 = The controller supports the Save field set to a non-zero value in the Set Features command and the Select field set to a non-zero value in the Get Features command\n");
	}
	else
	{
		printf("\tbit [      4] 0 = The controller does not support the Save field set to a non-zero value in the Set Features command and the Select field set to a non-zero value in the Get Features command\n");
	}

	if (g_stController.ONCS.WriteZeroes)
	{
		printf("\tbit [      3] 1 = The controller supports the Write Zeroes command\n");
	}
	else
	{
		printf("\tbit [      3] 0 = The controller does not support the Write Zeroes command\n");
	}

	if (g_stController.ONCS.DatasetManagement)
	{
		printf("\tbit [      2] 1 = The controller supports the Dataset Management command\n");
	}
	else
	{
		printf("\tbit [      2] 0 = The controller does not support the Dataset Management command\n");
	}

	if (g_stController.ONCS.WriteUncorrectable)
	{
		printf("\tbit [      1] 1 = The controller supports the Write Uncorrectable command\n");
	}
	else
	{
		printf("\tbit [      1] 0 = The controller does not support the Write Uncorrectable command\n");
	}

	if (g_stController.ONCS.Compare)
	{
		printf("\tbit [      0] 1 = The controller supports the Compare command\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The controller does not support the Compare command\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataFUSES(void)
{
	printf("[M] Fused Operation Support (FUSES):\n");
	if (g_stController.FUSES.CompareAndWrite)
	{
		printf("\tbit [      0] 1 = The controller supports the Compare and Write fused operation\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The controller does not support the Compare and Write fused operation\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataFNA(void)
{
	printf("[M] Format NVM Attributes (FNA):\n");
	if (g_stController.FNA.CryptographicEraseSupported)
	{
		printf("\tbit [      2] 1 = The controller supports cryptographic erase operation\n");
	}
	else
	{
		printf("\tbit [      2] 0 = The controller does not support cryptographic erase operation\n");
	}

	if (g_stController.FNA.SecureEraseApplyToAll)
	{
		printf("\tbit [      1] 1 = Cryptographic erase or user data erase as part of a format apply to all namespaces.\n");
	}
	else
	{
		printf("\tbit [      1] 0 = Cryptographic erase or user data erase as part of a format is performed on a per namespace basis.\n");
	}

	if (g_stController.FNA.FormatApplyToAll)
	{
		printf("\tbit [      0] 1 = Format operation applies to all namespaces\n");
	}
	else
	{
		printf("\tbit [      0] 0 = Format operation is specific to a particular namespace.\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataVWC(void)
{
	printf("[M] Volatile Write Cache (VWC):\n");
	if (g_stController.VWC.Present)
	{
		printf("\tbit [      0] 1 = A volatile write cache is present\n");
	}
	else
	{
		printf("\tbit [      0] 0 = A volatile write cache is not present\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataAWU(void)
{
	printf("[M] Atomic Write Unit Normal (AWUN): ");
	if (g_stController.AWUN == 0xFFFF)
	{
		printf("FFFFh = all commands are atomic\n");
	}
	else
	{
		printf("%04Xh = %d sector(s)\n", g_stController.AWUN, g_stController.AWUN + 1);
	}

	printf("[M] Atomic Write Unit Power Fail (AWUPF): %04Xh = %d sector(s)\n",
		g_stController.AWUPF, g_stController.AWUPF + 1);
}

static void s_vPrintNVMeIdentifyControllerDataNVSCC(void)
{
	printf("[M] NVM Vendor Specific Command Configuration (NVSCC):\n");
	if (g_stController.NVSCC.CommandFormatInSpec)
	{
		printf("\tbit [      0] 1 = All NVM Vendor Specific Commands use the format defined in the specification\n");
	}
	else
	{
		printf("\tbit [      0] 0 = The format of all NVM Vendor Specific Commands are vendor specific\n");
	}
}

static void s_vPrintNVMeIdentifyControllerDataACWU(void)
{
	printf("[O] Atomic Compare & Write Unit (ACWU): ");
	if (g_stController.ACWU == 0)
	{
		printf("0 = Compare and Write is not a supported in fused command\n");
	}
	else
	{
		printf("%d = means %d sector(s)\n", g_stController.ACWU, g_stController.ACWU + 1);
	}
}

static void s_vPrintNVMeIdentifyControllerDataSGLS(void)
{
	printf("[O] SGL Support (SGLS):\n");

	if (g_stController.SGLS.SGLSupported == 0)
	{
		printf("\tbit [  1:  0] 0 = SGLs are not supported\n");
		return;
	}

	if (g_stController.SGLS.OffsetByAddrFieldSupported)
	{
		printf("\tbit [     20] 1 = The controller supports the Address field in SGL Data Block, SGL Segment, and SGL Last Segment descriptor types specifying an offset\n");
	}
	else
	{
		printf("\tbit [     20] 0 = The controller does not support the Address field specifying an offset\n");
	}

	if (g_stController.SGLS.MPTRContainingSGLDescSupported)
	{
		printf("\tbit [     19] 1 = The controller supports the use of a Metadata Pointer (MPTR) that contains an address of an SGL segment containing exactly one SGL Descriptor that is Qword aligned\n");
	}
	else
	{
		printf("\tbit [     19] 0 = The controller does not support the use of a Metadata Pointer (MPTR) that contains an address of an SGL segment containing exactly one SGL Descriptor that is Qword aligned\n");
	}

	if (g_stController.SGLS.SGLLengthLargerThanDataLength)
	{
		printf("\tbit [     18] 1 = The controller supports commands that contain a data or metadata SGL of a length larger than the amount of data to be transferred\n");
	}
	else
	{
		printf("\tbit [     18] 0 = The controller requires the SGL length is equal to the amount of data to be transferred\n");
	}

	if (g_stController.SGLS.ByteAlignedContiguousPhysicalBuffer)
	{
		printf("\tbit [     17] 1 = The controller supports the use of a byte aligned contiguous physical buffer of metadata\n");
	}
	else
	{
		printf("\tbit [     17] 0 = The controller does not supports the use of a byte aligned contiguous physical buffer of metadata\n");
	}

	if (g_stController.SGLS.BitBucketDescrSupported)
	{
		printf("\tbit [     16] 1 = The controller supports the SGL Bit Bucket descriptor\n");
	}
	else
	{
		printf("\tbit [     16] 0 = The controller does not supports the SGL Bit Bucket descriptor\n");
	}

    switch (g_stController.SGLS.SGLSupported)
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

static void s_vPrintNVMeIdentifyControllerDataSUBNQN(void)
{
	printf("[M] NVM Subsystem NVMe Qualified Name (SUBNQN): ");
	char* strConv = strConvertUTF8toMultiByte((const char*)&(g_stController.SUBNQN));
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

static void s_vPrintNVMeIdentifyControllerDataPSDs(void)
{
	int iNumPSD = g_stController.NPSS + 1;

	printf("[M] Power State 0 Descriptor (PSD0):\n");
	s_vPrintNVMeIdentifyControllerDataPSD(&(g_stController.PDS[0]), g_stController.VER);

	for (int i = 1; i < iNumPSD; i++)
	{
		printf("[O] Power State %d Descriptor (PSD%d):\n", i, i);
		s_vPrintNVMeIdentifyControllerDataPSD(&(g_stController.PDS[i]), g_stController.VER);
	}
}

static void s_vPrintNVMeIdentifyControllerDataFGUID(void)
{
	int iSum = 0;
	unsigned char* pId = g_stController.FGUID;

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

void vPrintControllerBasicData(void)
{
    {
        char buf[21];
        ZeroMemory(buf, 21);
        strncpy_s(buf, (const char*)(g_stController.SN), 20);
        buf[20] = '\0';
        printASCII("[M] SerialNumber (SN): ", (const char*)buf);
    }

    {
        char buf[41];
        ZeroMemory(buf, 41);
        strncpy_s(buf, (const char*)(g_stController.MN), 40);
        buf[40] = '\0';
        printASCII("[M] Model Number (MN): ", (const char*)buf);
    }

    {
        char buf[9];
        ZeroMemory(buf, 9);
        strncpy_s(buf, (const char*)(g_stController.FR), 8);
        buf[8] = '\0';
        printASCII("[M] Firmware Revision (FR): ", (const char*)buf);
    }
}


void vPrintNVMeIdentifyControllerData(void)
{
	printf("[M] PCI Vendor ID (VID): 0x%X\n", g_stController.VID);
	printf("[M] PCI Subsystem Vendor ID (SSVID): 0x%X\n", g_stController.SSVID);

    vPrintControllerBasicData();

	printf("[M] Recommended Arbitration Burst (RAB): %d (means %d)\n", g_stController.RAB, 1 << g_stController.RAB);
	printf("[M] IEEE OUI Identifier (IEEE): %02X-%02X-%02X\n", g_stController.IEEE[2], g_stController.IEEE[1], g_stController.IEEE[0]);

	s_vPrintNVMeIdentifyControllerDataCMIC();

	printf("[M] Maximum Data Transfer Size (MDTS): %d (means %d)\n", g_stController.MDTS, 1 << g_stController.MDTS);
	printf("[M] Controller ID (CNTLID): 0x%x\n", g_stController.CNTLID);
	printf("[M] Version (VER): 0x%x\n", g_stController.VER);
	printf("[M] RTD3 Resume Latency (RTD3R): %d (usec)\n", g_stController.RTD3R);
	printf("[M] RTD3 Entry Latency (RTD3E): %d (usec)\n", g_stController.RTD3E);

	s_vPrintNVMeIdentifyControllerDataOAES();
	s_vPrintNVMeIdentifyControllerDataCTRATT();
	s_vPrintNVMeIdentifyControllerDataFGUID();
	s_vPrintNVMeIdentifyControllerDataOACS();

	printf("[M] Abort Command Limit (ACL): %d (means %d)\n", g_stController.ACL, g_stController.ACL + 1);
	printf("[M] Asynchronous Event Request Limit (AERL): %d (means %d)\n", g_stController.AERL, g_stController.AERL + 1);

	s_vPrintNVMeIdentifyControllerDataFRMW();
	s_vPrintNVMeIdentifyControllerDataLPA();

	printf("[M] Error Log Page Entries (ELPE): %d (means %d)\n", g_stController.ELPE, g_stController.ELPE + 1);
	printf("[M] Number of Power States Support (NPSS): %d (means %d)\n", g_stController.NPSS, g_stController.NPSS + 1);
	s_vPrintNVMeIdentifyControllerDataAVSCC();
	s_vPrintNVMeIdentifyControllerDataAPSTA();
	s_vPrintNVMeIdentifyControllerDataCTEMP();
	s_vPrintNVMeIdentifyControllerDataMTFA();
	s_vPrintNVMeIdentifyControllerDataHMB();

	if (g_stController.OACS.NamespaceCommands)
	{
		printf("[O] Total NVM Capacity (TNVMCAP): %llu (byte)\n", (uint64_t)g_stController.TNVMCAP);
		printf("[O] Unallocated NVM Capacity (UNVMCAP): %llu (byte)\n", (uint64_t)g_stController.UNVMCAP);
	}

	s_vPrintNVMeIdentifyControllerDataRPMB();
	s_vPrintNVMeIdentifyControllerDataEDSTT();
	s_vPrintNVMeIdentifyControllerDataDSTO();
	s_vPrintNVMeIdentifyControllerDataFWUG();
	s_vPrintNVMeIdentifyControllerDataKAS();
	s_vPrintNVMeIdentifyControllerDataThermalMgmt();
	s_vPrintNVMeIdentifyControllerDataSANICAP();

	s_vPrintNVMeIdentifyControllerDataSQES();
	s_vPrintNVMeIdentifyControllerDataCQES();
	s_vPrintNVMeIdentifyControllerDataMAXCMD();

	printf("[M] Number of Namespaces (NN): %d\n", g_stController.NN);

	s_vPrintNVMeIdentifyControllerDataONCS();
	s_vPrintNVMeIdentifyControllerDataFUSES();
	s_vPrintNVMeIdentifyControllerDataFNA();
	s_vPrintNVMeIdentifyControllerDataVWC();
	s_vPrintNVMeIdentifyControllerDataAWU();
	s_vPrintNVMeIdentifyControllerDataNVSCC();
	s_vPrintNVMeIdentifyControllerDataACWU();
	s_vPrintNVMeIdentifyControllerDataSGLS();
	s_vPrintNVMeIdentifyControllerDataSUBNQN();
	s_vPrintNVMeIdentifyControllerDataPSDs();
}

int iNVMeIdentifyController(HANDLE _hDevice)
{
	int     iResult = -1;
	PVOID   buffer = NULL;
	ULONG   bufferLength = 0;
	ULONG   returnedLength = 0;

	PSTORAGE_PROPERTY_QUERY query = NULL;
	PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
	PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

	// Allocate buffer for use.
	bufferLength = offsetof(STORAGE_PROPERTY_QUERY, AdditionalParameters)
		+ sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)
		+ sizeof(NVME_IDENTIFY_CONTROLLER_DATA13);
	buffer = malloc(bufferLength);

	if (buffer == NULL)
	{
		vUtilPrintSystemError( GetLastError(), "malloc" );
		goto error_exit;
	}

	//
	// Initialize query data structure to get Identify Controller Data.
	//
	ZeroMemory(buffer, bufferLength);

	query = (PSTORAGE_PROPERTY_QUERY)buffer;
	protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
	protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

	query->PropertyId = StorageAdapterProtocolSpecificProperty;
	query->QueryType = PropertyStandardQuery;

	protocolData->ProtocolType = ProtocolTypeNvme;
	protocolData->DataType = NVMeDataTypeIdentify;
	protocolData->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_CONTROLLER;
	protocolData->ProtocolDataRequestSubValue = 0;
	protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
	protocolData->ProtocolDataLength = sizeof(NVME_IDENTIFY_CONTROLLER_DATA13);

	//
	// Send request down.
	//
	iResult = iIssueDeviceIoControl(
		_hDevice,
		IOCTL_STORAGE_QUERY_PROPERTY,
		buffer,
		bufferLength,
		buffer,
		bufferLength,
		&returnedLength,
		NULL
	);

	if (iResult) goto error_exit;

	fprintf(stderr, "\n");

	// Validate the returned data.
	if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
		(protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)))
	{
		fprintf(stderr, "[E] NVMeIdentifyController: Data Descriptor Header is not valid, stop.\n");
        iResult = -1; // error
		goto error_exit;
	}

	protocolData = &protocolDataDescr->ProtocolSpecificData;

	if ((protocolData->ProtocolDataOffset > sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
		(protocolData->ProtocolDataLength < sizeof(NVME_IDENTIFY_CONTROLLER_DATA13)))
	{
		fprintf(stderr, "[E] NVMeIdentifyController: ProtocolData Offset/Length is not valid, stop.\n");
        iResult = -1; // error
        goto error_exit;
	}

    memcpy_s((void*)(&g_stController), sizeof(NVME_IDENTIFY_CONTROLLER_DATA13), (uint8_t*)protocolData + protocolData->ProtocolDataOffset, sizeof(NVME_IDENTIFY_CONTROLLER_DATA13));
    if ((g_stController.VID == 0) || (g_stController.NN == 0))
	{
		fprintf(stderr, "[E] NVMeIdentifyController: Identify Controller Data is not valid, stop.\n");
        iResult = -1; // error
        goto error_exit;
	}

    printf("[I] Version of NVMe spec. that this controller conforms to is %d.%d.%d\n",
            ((g_stController.VER) >> 16) & 0xFFFF, ((g_stController.VER) >> 8) & 0xFF, (g_stController.VER) & 0xFF);
    iResult = 0; // succeeded

error_exit:

    if (buffer != NULL)
    {
        free(buffer);
    }

	return iResult;
}

