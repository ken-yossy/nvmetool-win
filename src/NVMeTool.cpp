#include "WinFunc.h"
#include <iostream>

#include <string.h>
#include <stdlib.h>

#include "NVMeSCSIPassThrough.h"
#include "NVMeDeallocate.h"
#include "NVMeIdentify.h"
#include "NVMeIdentifyController.h"
#include "NVMeGetLogPage.h"
#include "NVMeGetFeatures.h"
#include "NVMeUtils.h"

NVME_IDENTIFY_CONTROLLER_DATA13 g_stController;

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
        return iResult;
    }

    fprintf(stderr, "[I] Getting controller identify data succeeded.\n\n");
    vPrintControllerBasicData();

    while (!bFinished)
    {
        int iMajorCmd = eGetCommandFromConsole();
        switch (iMajorCmd)
        {
        case CMD_TYPE_DEALLOCATE:
            iResult = iNVMeDeallocate(hDevice);
            break;

        case CMD_TYPE_READ:
            iResult = iReadViaSCSIPassThrough(hDevice);
            break;

        case CMD_TYPE_WRITE:
            iResult = iWriteViaSCSIPassThrough(hDevice);
            break;

        case CMD_TYPE_IDENTIFY:
            iResult = iNVMeIdentify(hDevice);
            break;

        case CMD_TYPE_GET_LOG_PAGE:
            iResult = iNVMeGetLogPage(hDevice);
            break;

        case CMD_TYPE_GET_FEATURE:
            iResult = iNVMeGetFeatures(hDevice);
            break;

        case CMD_TYPE_QUIT:
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
