#pragma once

#include <windows.h>

int iNVMeIdentifyNamespace(HANDLE _hDevice, DWORD _dwNSID, bool _bPrintOrNot);
bool bNamespaceSupportPI(uint32_t _ulNSID);
bool bNamespaceAlreadyIdentified(uint32_t _ulNSID);
