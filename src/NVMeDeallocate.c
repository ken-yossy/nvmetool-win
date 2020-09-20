#include <windows.h>

#include "WinFunc.h"

int iNVMeDeallocate(HANDLE _hDevice)
{
    int     iResult = -1;
    PVOID   buffer = NULL;
    ULONG   bufferLength = 0;
    ULONG   returnedLength = 0;

    PDEVICE_MANAGE_DATA_SET_ATTRIBUTES pAttr = NULL;
    PDEVICE_DSM_RANGE pRange = NULL;

    // Allocate buffer for use.
    bufferLength = sizeof(DEVICE_MANAGE_DATA_SET_ATTRIBUTES) + sizeof(DEVICE_DSM_RANGE);
    buffer = malloc(bufferLength);

    if (buffer == NULL)
    {
        vPrintSystemError( GetLastError(), "malloc" );
        return -1;
    }

    // see also https://docs.microsoft.com/ja-jp/windows/desktop/api/winioctl/ni-winioctl-ioctl_storage_manage_data_set_attributes
    // see also winioctl.h
    ZeroMemory(buffer, bufferLength);
    pAttr = (PDEVICE_MANAGE_DATA_SET_ATTRIBUTES)buffer;

    pAttr->Action = DeviceDsmAction_Trim;
    pAttr->Flags = DEVICE_DSM_FLAG_TRIM_NOT_FS_ALLOCATED; // for native deallocate (not file-level trimming)
    pAttr->ParameterBlockOffset = 0; // TRIM does not need additional parameters
    pAttr->ParameterBlockLength = 0;
    pAttr->DataSetRangesOffset = sizeof(DEVICE_MANAGE_DATA_SET_ATTRIBUTES);
    pAttr->DataSetRangesLength = sizeof(DEVICE_DSM_RANGE); //  only one range

    pRange = (PDEVICE_DSM_RANGE)((ULONGLONG)pAttr + sizeof(DEVICE_MANAGE_DATA_SET_ATTRIBUTES));
    pRange->StartingOffset = 0; // LBA = 0
    pRange->LengthInBytes = 512; // 1 sector

    // Send request down (through WinFunc.cpp)
    iResult = iIssueDeviceIoControl(
        _hDevice,                                   // (HANDLE) hDevice             // handle to device
        IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES,   // dwIoControlCode
        buffer,                                     // (LPVOID) lpInBuffer          // input buffer
        bufferLength,                               // (DWORD) nInBufferSize        // size of the input buffer
        buffer,                                     // (LPVOID) lpOutBuffer         // output buffer
        bufferLength,                               // (DWORD) nOutBufferSize       // size of the output buffer
        &returnedLength,                            // (LPDWORD) lpBytesReturned    // number of bytes returned
        NULL                                        // (LPOVERLAPPED) lpOverlapped  // OVERLAPPED structure
    );

    free(buffer);

    return iResult;
}

