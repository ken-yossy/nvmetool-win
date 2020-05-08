#pragma once

#include <windows.h>
#include <cstdint>
#include <nvme.h>

typedef struct {
    union {
        struct {
            UCHAR   ActiveSlot : 3;
            UCHAR   Reserved0 : 1;
            UCHAR   PendingActivateSlot : 3;
            UCHAR   Reserved1 : 1;
        } DUMMYSTRUCTNAME;
    
        BYTE AsByte;
    } AFI;

    UCHAR    Reserved0[7];

    UCHAR    FRS[7][8];
    UCHAR    Reserved1[448];

} NVME_FIRMWARE_SLOT_INFO_LOG12, *PNVME_FIRMWARE_SLOT_INFO_LOG12;

int iNVMeGetFwSlotInformation(HANDLE _hDevice);
