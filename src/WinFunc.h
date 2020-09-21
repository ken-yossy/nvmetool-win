#pragma once

#include <windows.h>
#include <stdbool.h>

void vGetOSVersion(void);
bool bCanUseGetDeviceInternalLog(void);
void vPrintSystemError(unsigned long _ulErrorCode, const char* _strFunc);

int iIssueDeviceIoControl(HANDLE _hDevice,
    DWORD _dwControlCode,
    LPVOID _lpInBuffer,
    DWORD _nInBufferSize,
    LPVOID _lpOutBuffer,
    DWORD _nOutBufferSize,
    LPDWORD _lpBytesReturned,
    LPOVERLAPPED _lpOverlapped);

HANDLE hIssueCreateFile(const char* _strDeviceNo);
