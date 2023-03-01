#pragma once

#include <stdbool.h>
#include <windows.h>

int iNVMeGetDeviceSelftestLog(HANDLE _hDevice, bool _bPrint,
                              bool *_bInProgress);
