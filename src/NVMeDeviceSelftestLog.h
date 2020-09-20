#pragma once

#include <windows.h>
#include <stdbool.h>

int iNVMeGetDeviceSelftestLog(HANDLE _hDevice, bool _bPrint, bool *_bInProgress);
