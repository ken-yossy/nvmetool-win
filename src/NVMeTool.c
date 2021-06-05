#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "WinFunc.h"
#include "NVMeSCSIPassThrough.h"
#include "NVMeDeallocate.h"
#include "NVMeIdentify.h"
#include "NVMeIdentifyController.h"
#include "NVMeGetLogPage.h"
#include "NVMeGetFeatures.h"
#include "NVMeSetFeatures.h"
#include "NVMeDeviceSelftest.h"
#include "NVMeFormatNVM.h"
#include "NVMeSMART.h"
#include "NVMeUtils.h"

static const char strVersion[] = "2.0";

#pragma warning(disable:6301)

MY_NVME_IDENTIFY_CONTROLLER_DATA g_stController;
NVME_SMART_INFO_LOG g_stSMARTLog;

static void s_vPrintUsage(char* _strProgName)
{
    char strShortProgName[1024];

    errno_t err = strncpy_s(strShortProgName, 1024, _strProgName, strlen(_strProgName));
    if (err)
    {
        fprintf(stderr, "[E] Error occurred in strncpy_s(); errno = %d\n", err);
        return;
    }

    char* cPtr = strShortProgName;
    char* tmpPtr = strShortProgName;
    while (tmpPtr != NULL)
    {
        tmpPtr += 1;
        cPtr = tmpPtr;
        tmpPtr = strchr(cPtr, '\\');
    }

    // cPtr holds the last occurrence of '\'
    printf("%s: Sample Utility tool for NVMe drive (version %s)\n\n", cPtr, strVersion);
    printf("Usage:\n");
    printf("  > %s <physical-drive-no>\n\n", cPtr);
    printf("Argument:\n");
    printf("  <physical-drive-no>: physical number of drive to be accessed\n");
    printf("    (you can be confirm it with \"Disk Management\" in Control Panel)\n\n");
}

int main(int _argc, char* _argv[])
{
    int     iResult = -1;
    bool    bFinished = false;
    HANDLE  hDevice = NULL;

    if (_argc != 2)
    {
        s_vPrintUsage(_argv[0]);
        return 0;
    }

    printf("[I] nvmetool-win version %s\n\n", strVersion);
    vGetOSVersion();

    // Get HANDLE object for NVMe SSD
    // We need change UAC setting of this application, to access device with right of administrator
    // see also https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/ac055958-d438-4593-a2a4-6b130b563747/createfile-on-c-with-read-only-returns-quotaccess-deniedquot-on-win7-it-is-ok-on-xp
    hDevice = hIssueCreateFile((const char*)(_argv[1]));
    if ( hDevice == INVALID_HANDLE_VALUE )
    {
        return iResult;
    }

    // retrieve fundamental controller data
    iResult = iNVMeIdentifyController(hDevice);
    if (iResult)
    {
        fprintf(stderr, "[E] Getting controller identify data failed, stop.\n\n");
        return iResult;
    }

    printf("[I] Getting controller identify data succeeded.\n\n");
    vPrintControllerBasicData();

    // retrieve controller's SMART data
    iResult = iNVMeGetSMART(hDevice, false, NVME_NAMESPACE_ALL);
    if (iResult)
    {
        fprintf(stderr, "[E] Getting controller's SMART / Health information log page failed, stop.\n\n");
        return iResult;
    }

    printf("[I] Getting controller's SMART / Health information log page succeeded.\n\n");

    while (!bFinished)
    {
        int iMajorCmd = eGetCommandFromConsole();
        switch (iMajorCmd)
        {
        case NVME_COMMAND_DSM:
            if (g_stController.ONCS.DatasetManagement)
            {
                iResult = iNVMeDeallocate(hDevice);
                if (iResult == 0)
                {
                    printf("[I] Deallocation succeeded.\n\n");
                }
            }
            else
            {
                fprintf(stderr, "[E] This SSD does not support Dataset Management command, ignore.\n\n");
            }
            break;

        case NVME_COMMAND_FLUSH:
            iResult = iFlushViaSCSIPassThrough(hDevice);
            break;

        case NVME_COMMAND_READ:
            iResult = iReadViaSCSIPassThrough(hDevice);
            break;

        case NVME_COMMAND_WRITE:
            iResult = iWriteViaSCSIPassThrough(hDevice);
            break;

        case NVME_COMMAND_IDENTIFY:
            iResult = iNVMeIdentify(hDevice);
            break;

        case NVME_COMMAND_GET_LOG_PAGE:
            iResult = iNVMeGetLogPage(hDevice);
            break;

        case NVME_COMMAND_SET_FEATURES:
            iResult = iNVMeSetFeatures(hDevice);
            break;

        case NVME_COMMAND_GET_FEATURES:
            iResult = iNVMeGetFeatures(hDevice);
            break;

        case NVME_COMMAND_DST:
            iResult = iNVMeDeviceSelftest(hDevice);
            break;

        case NVME_COMMAND_FORMAT_NVM:
            iResult = iNVMeFormatNVM(hDevice);
            break;

        case NVME_COMMAND_SECURITY_RECV:
            if (g_stController.OACS.SecurityCommands)
            {
                iResult = iSecurityReceiveViaSCSIPassThrough(hDevice);
            }
            else
            {
                fprintf(stderr, "[E] This SSD does not support Security Send / Receive commands, ignore.\n\n");
            }
            break;

        case NVME_TOOL_COMMAND_QUIT:
            iResult = 0;
            bFinished = true;
            break;

        default:
            fprintf(stderr, "\n[E] Command not implemented yet.\n");
            break;
        }
    }

    CloseHandle(hDevice);

    printf("Press any key to terminate program... ");
    getc(stdin); // wait...

    return iResult;
}
