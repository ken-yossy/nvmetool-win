#pragma once

#include <stdbool.h>
#include <windows.h>

int iNVMeGetTelemetryHostInitiated(HANDLE _hDevice, bool _bCreate);
int iNVMeGetTelemetryControllerInitiated(HANDLE _hDevice);

int iNVMeGetTelemetryHostInitiatedWithDeviceInternalLog(HANDLE _hDevice,
                                                        bool _bCreate);
