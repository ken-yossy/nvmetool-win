#pragma once

#include <windows.h>
#include <stdint.h>

int iNVMeIdentifyIOCSNamespace(HANDLE _hDevice, uint32_t _dwNSID);

