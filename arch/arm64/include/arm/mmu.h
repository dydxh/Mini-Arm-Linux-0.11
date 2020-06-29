#ifndef _MMU_H
#define _MMU_H

#define MM_TYPE_PAGE_TABLE      0x3
#define MM_TYPE_PAGE            0x3
#define MM_TYPE_BLOCK           0x1
#define MM_ACCESS               (0x1 << 10)
#define MM_ACCESS_PERMISSION    (0b01 << 6)
#define MM_ACCESS_RWN           (0b00 << 6)
#define MM_ACCESS_RWRW          (0b01 << 6)
#define MM_ACCESS_RON           (0b10 << 6)
#define MM_ACCESS_RORO          (0b11 << 6)

#define MM_ACCESS_UXN1   (1 << 54)
#define MM_ACCESS_UXN0   (0 << 54)
#define MM_ACCESS_PXN1   (1 << 53)
#define MM_ACCESS_PXN0   (0 << 53)

#define MM_ACCESS_AP00   (0b00 << 6)
#define MM_ACCESS_AP01   (0b01 << 6)
#define MM_ACCESS_AP10   (0b10 << 6)
#define MM_ACCESS_AP11   (0b11 << 6)

//////////////// RWX prop for EL1&EL0 ///////////////////
#define MM_ACCESS_RWX__X (MM_ACCESS_UXN0 | MM_ACCESS_PXN0 | MM_ACCESS_AP00)
#define MM_ACCESS_RW_RWX (MM_ACCESS_UXN0 | MM_ACCESS_PXN0 | MM_ACCESS_AP01)
#define MM_ACCESS_R_X__X (MM_ACCESS_UXN0 | MM_ACCESS_PXN0 | MM_ACCESS_AP10)
#define MM_ACCESS_R_XR_X (MM_ACCESS_UXN0 | MM_ACCESS_PXN0 | MM_ACCESS_AP11)
#define MM_ACCESS_RW___X (MM_ACCESS_UXN0 | MM_ACCESS_PXN1 | MM_ACCESS_AP00)
//#define MM_ACCESS_RW_RWX (MM_ACCESS_UXN0 | MM_ACCESS_PXN1 | MM_ACCESS_AP01)
#define MM_ACCESS_R____X (MM_ACCESS_UXN0 | MM_ACCESS_PXN1 | MM_ACCESS_AP10)
#define MM_ACCESS_R__R_X (MM_ACCESS_UXN0 | MM_ACCESS_PXN1 | MM_ACCESS_AP11)
#define MM_ACCESS_RWX___ (MM_ACCESS_UXN1 | MM_ACCESS_PXN0 | MM_ACCESS_AP00)
#define MM_ACCESS_RW_RW_ (MM_ACCESS_UXN1 | MM_ACCESS_PXN0 | MM_ACCESS_AP01)
#define MM_ACCESS_R_X___ (MM_ACCESS_UXN1 | MM_ACCESS_PXN0 | MM_ACCESS_AP10)
#define MM_ACCESS_R_XR__ (MM_ACCESS_UXN1 | MM_ACCESS_PXN0 | MM_ACCESS_AP11)
#define MM_ACCESS_RW____ (MM_ACCESS_UXN1 | MM_ACCESS_PXN1 | MM_ACCESS_AP00)
//#define MM_ACCESS_RW_RW_ (MM_ACCESS_UXN1 | MM_ACCESS_PXN1 | MM_ACCESS_AP01)
#define MM_ACCESS_R_____ (MM_ACCESS_UXN1 | MM_ACCESS_PXN1 | MM_ACCESS_AP10)
#define MM_ACCESS_R__R__ (MM_ACCESS_UXN1 | MM_ACCESS_PXN1 | MM_ACCESS_AP11)

/*
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *                    n      MAIR
 *   DEVICE_nGnRnE    000    00000000
 *   NORMAL_NC        001    01000100
 */
#define MT_DEVICE_nGnRnE        0x0
#define MT_NORMAL_NC            0x1
#define MT_DEVICE_nGnRnE_FLAGS  0x00
#define MT_NORMAL_NC_FLAGS      0x44
#define MAIR_VALUE              (MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))

#define MMU_FLAGS               (MM_TYPE_BLOCK | (MT_NORMAL_NC << 2) | MM_ACCESS)    
#define MMU_DEVICE_FLAGS        (MM_TYPE_BLOCK | (MT_DEVICE_nGnRnE << 2) | MM_ACCESS)
#define MMU_PTE_FLAGS_BASE      (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS)
#define MMU_PTE_FLAGS           (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_PERMISSION)
#define MMU_PTE_RWN             (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_RWN)
#define MMU_PTE_RWRW            (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_RWRW)
#define MMU_PTE_RON             (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_RON)
#define MMU_PTE_RORO            (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_RORO)

#define MMU_PTE_TEXT            (MMU_PTE_FLAGS_BASE | MM_ACCESS_R_X___)
#define MMU_PTE_RODATA          (MMU_PTE_FLAGS_BASE | MM_ACCESS_R_____)
#define MMU_PTE_DATA            (MMU_PTE_FLAGS_BASE | MM_ACCESS_RW____)
#define MMU_PTE_BSS             (MMU_PTE_FLAGS_BASE | MM_ACCESS_RW____)
#define MMU_PTE_PGD             (MMU_PTE_FLAGS_BASE | MM_ACCESS_RW____)
#define MMU_PTE_STACK           (MMU_PTE_FLAGS_BASE | MM_ACCESS_RW____)

#define TCR_T0SZ                (64 - 48) 
#define TCR_T1SZ                ((64 - 48) << 16)
#define TCR_TG0_4K              (0 << 14)
#define TCR_TG1_4K              (2 << 30)
#define TCR_VALUE               (TCR_T0SZ | TCR_T1SZ | TCR_TG0_4K | TCR_TG1_4K)

#endif
