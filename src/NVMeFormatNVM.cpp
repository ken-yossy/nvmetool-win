#include "WinFunc.h"
#include <stdio.h>
#include <nvme.h>

#include "NVMeUtils.h"
#include "NVMeIdentifyController.h"

static int siNVMeDoFormat(HANDLE _hDevice)
{
    int     iResult = -1;
    ULONG   returnedLength = 0;

    // Send request down.
    iResult = iIssueDeviceIoControl(_hDevice,
        IOCTL_STORAGE_REINITIALIZE_MEDIA,
        NULL,
        0,
        NULL,
        0,
        &returnedLength,
        NULL
    );

    if (iResult)
    {
        fprintf(stderr, "[E] Failed in issuing Format NVM command by DeviceIoControl(IOCTL_STORAGE_REINITIALIZE_MEDIA)\n");
    }
    else
    {
        fprintf(stderr, "[I] Issuing Format NVM command by DeviceIoControl(IOCTL_STORAGE_REINITIALIZE_MEDIA) is succeeded\n");
    }

    return iResult;
}

static int siLockDrive(HANDLE _hDevice)
{
    int     iResult = -1;
    ULONG   returnedLength = 0;

    // try to lock the drive
    // see https://docs.microsoft.com/en-us/windows/win32/api/winioctl/ni-winioctl-fsctl_lock_volume
    iResult = iIssueDeviceIoControl(_hDevice,
        FSCTL_LOCK_VOLUME,
        NULL,
        0,
        NULL,
        0,
        &returnedLength,
        NULL
    );

    if (iResult)
    {
        fprintf(stderr, "[E] Failed in locing target drive by DeviceIoControl(FSCTL_LOCK_VOLUME), skip\n");
    }
    return iResult;
}

static void svUnlockDrive(HANDLE _hDevice)
{
    int     iResult = -1;
    ULONG   returnedLength = 0;

    // try to unlock the drive
    iResult = iIssueDeviceIoControl(_hDevice,
        FSCTL_UNLOCK_VOLUME,
        NULL,
        0,
        NULL,
        0,
        &returnedLength,
        NULL
    );

    if (iResult)
    {
        fprintf(stderr, "[W] Failed in unlocing target drive by DeviceIoControl(FSCTL_LOCK_VOLUME), ignore\n");
    }
}

int iNVMeFormatNVM(HANDLE _hDevice)
{
    int iResult = -1;

    if (!g_stController.OACS.FormatNVM)
    {
        printf("[W] This controller does not support Format NVM command, ignore\n");
        return iResult;
    }

    if (!g_stController.FNA.CryptographicEraseSupported)
    {
        printf("[W] This controller does not support cryptographic erase function for Format NVM command, ignore\n");
        return iResult;
    }

    // before format, need to obtain lock for the designated drive
    // see comments for definition of IOCTL_STORAGE_REINITIALIZE_MEDIA in nvme.h
    iResult = siLockDrive(_hDevice);
    if (iResult == 0)
    {
        iResult = siNVMeDoFormat(_hDevice);
        svUnlockDrive(_hDevice);
    }

    return iResult;
}

