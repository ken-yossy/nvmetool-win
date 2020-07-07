#pragma once

#include <windows.h>
#include <cstdint>
#include <nvme.h>

extern NVME_IDENTIFY_CONTROLLER_DATA g_stController;

int iNVMeIdentifyController(HANDLE _hDevice);
uint32_t uiPrintControllerBasicData(void);
