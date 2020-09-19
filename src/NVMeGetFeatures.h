#pragma once

#include <windows.h>
#include <stdint.h>
#include <nvme.h>

int iNVMeGetFeatures(HANDLE _hDevice);
int iNVMeGetFeature32(HANDLE _hDevice, DWORD _dwFId, int _iType, DWORD _dwCDW11, uint32_t* _pulData);
void vNVMeGetFeaturesShowCapabilities(NVME_CDW11_FEATURE_SUPPORTED_CAPABILITY _ulCaps);

