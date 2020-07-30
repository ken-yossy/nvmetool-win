#pragma once

#include <windows.h>
#include <cstdint>

int iNVMeIdentifyNSIDDescriptor(HANDLE _hDevice, DWORD _dwNSID);

typedef struct {
    uint8_t NIDT;
    uint8_t NIDL;
    uint8_t Reserved[2];
    uint8_t NID[4092];
} NVME_IDENTIFY_NSID_DESCRIPTOR, *PNVME_IDENTIFY_NSID_DESCRIPTOR;

