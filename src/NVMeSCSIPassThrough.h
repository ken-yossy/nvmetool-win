#pragma once

#include <windows.h>

int iReadViaSCSIPassThrough(HANDLE _hDevice);
int iWriteViaSCSIPassThrough(HANDLE _hDevice);
int iFlushViaSCSIPassThrough(HANDLE _hDevice);
int iSecurityReceiveViaSCSIPassThrough(HANDLE _hDevice);
