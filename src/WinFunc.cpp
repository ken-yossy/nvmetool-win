#include <windows.h>
#include <stdio.h>
#include <tchar.h>

void vUtilPrintSystemError(unsigned long _ulErrorCode, const char* _strFunc)
{
    WCHAR lpMsgBuf[1024];
    ULONG count;

    count = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        _ulErrorCode,
        0x0409, // default
        (LPTSTR)&lpMsgBuf,
        sizeof(lpMsgBuf),
        NULL);

    if (count != 0)
    {
        fprintf(stderr, "[E] %s: (error code = %d) %ls\n", _strFunc, _ulErrorCode, lpMsgBuf);
    }
    else
    {
        fprintf(stderr, "[E] Format message failed.  Error: %d\n", GetLastError());
    }
}

// wrapper for DeviceIoControl()
int iIssueDeviceIoControl(
    HANDLE          _hDevice,
    DWORD           _dwControlCode,
    LPVOID          _lpInBuffer,
    DWORD           _nInBufferSize,
    LPVOID          _lpOutBuffer,
    DWORD           _nOutBufferSize,
    LPDWORD         _lpBytesReturned,
    LPOVERLAPPED    _lpOverlapped)
{
    int iResult = -1;
    BOOL iRetIoControl = 0;

    // bResult is nonzero if succeeded, zero if failed
    iRetIoControl = DeviceIoControl(
        _hDevice,
        _dwControlCode,
        _lpInBuffer,
        _nInBufferSize,
        _lpOutBuffer,
        _nOutBufferSize,
        _lpBytesReturned,
        _lpOverlapped);

    if (iRetIoControl == 0)
    {
        vUtilPrintSystemError(GetLastError(), "DeviceIoControl");
    }
    else
    {
        iResult = 0; // succeeded
    }

    return iResult;
}

HANDLE hIssueCreateFile(const char* _strDeviceNo)
{
    HANDLE hDevice;
    TCHAR strDeviceName[256];

    _stprintf_s(strDeviceName, 256, _T("\\\\.\\PhysicalDrive%hs"), _strDeviceNo);
    hDevice = CreateFile(
        (LPCWSTR)strDeviceName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_DEVICE,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        vUtilPrintSystemError(GetLastError(), "CreateFile");
    }
    return hDevice;
}

