#pragma once

#include <windows.h>
#include <nvme.h>

extern NVME_IDENTIFY_CONTROLLER_DATA g_stController;

int iNVMeIdentifyController(HANDLE _hDevice);
void vPrintControllerBasicData(void);
