#pragma once

#include <windows.h>
#include <cstdint>
#include <nvme.h>

#include "NVMeIdentifyController12.h"
#include "NVMeIdentifyController13.h"
#include "NVMeIdentifyController14.h"

extern NVME_IDENTIFY_CONTROLLER_DATA g_stController;

int iNVMeIdentifyController(HANDLE _hDevice);
uint32_t uiPrintControllerBasicData(void);
void vPrintNVMeIdentifyControllerData(void);
