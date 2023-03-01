#pragma once

#include <windows.h>

#define NVME_LOG_PAGE_SUPPORTED_LOG_PAGES 0

int iNVMeGetSupportedLogPages(HANDLE _hDevice);
