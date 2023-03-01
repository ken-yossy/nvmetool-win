#pragma once

#include <stdint.h>
#include <windows.h>

int iNVMeIdentifyIOCSNamespace(HANDLE _hDevice, uint32_t _dwNSID);
