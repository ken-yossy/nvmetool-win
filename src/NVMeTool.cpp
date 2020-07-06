#include "WinFunc.h"
#include <iostream>
#include <nvme.h>

#include <string.h>
#include <stdlib.h>

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

#pragma warning(disable:6301)

NVME_IDENTIFY_CONTROLLER_DATA g_stController;
NVME_HEALTH_INFO_LOG13 g_stSMARTLog;

uint32_t g_uiRevision; // revision that this controller conforms to; format is same as VER field

void vPrintUsage(char* _strProgName)
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
    fprintf(stderr, "%s: Sample Utility tool for NVMe drive.\n\n", cPtr);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  > %s <physical-drive-no>\n\n", cPtr);
    fprintf(stderr, "Argument:\n");
    fprintf(stderr, "  <physical-drive-no>: physical number of drive to be accessed\n");
    fprintf(stderr, "    (you can be confirm it with \"Disk Management\" in Control Panel)\n\n");
}

int main(int _argc, char* _argv[])
{
    int     iResult = -1;
    bool    bFinished = false;
    HANDLE  hDevice = NULL;

    if (_argc != 2)
    {
        vPrintUsage(_argv[0]);
        return 0;
    }

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

    fprintf(stderr, "[I] Getting controller identify data succeeded.\n\n");
    g_uiRevision = uiPrintControllerBasicData();

    // retrieve fundamental SMART data
    iResult = iNVMeGetSMART(hDevice, false);
    if (iResult)
    {
        fprintf(stderr, "[E] Getting SMART/Health information data failed, stop.\n\n");
        return iResult;
    }

    fprintf(stderr, "[I] Getting SMART information succeeded.\n\n");

    while (!bFinished)
    {
        int iMajorCmd = eGetCommandFromConsole();
        switch (iMajorCmd)
        {
        case NVME_COMMAND_DSM:
            iResult = iNVMeDeallocate(hDevice);
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

        case NVME_TOOL_COMMAND_QUIT:
            iResult = 0;
            bFinished = true;
            break;

        default:
            printf("\n[E] Command not implemented yet.\n");
            break;
        }
    }

    CloseHandle(hDevice);

    fprintf(stderr, "Press any key to terminate program... ");
    getc(stdin); // wait...

    return iResult;
}
