#pragma once

#include <windows.h>

#define NVME_LOG_PAGE_SUPPORTED_LOG_PAGES 0

int iNVMeGetLogPage(HANDLE _hDevice);
