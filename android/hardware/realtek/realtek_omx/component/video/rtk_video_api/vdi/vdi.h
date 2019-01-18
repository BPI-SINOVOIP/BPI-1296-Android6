//------------------------------------------------------------------------------
// File: vdi.h
//
// Copyright (c) 2006, Realtek.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef _VDI_H_
#define _VDI_H_

#include "mm.h"
#include "vpuconfig.h"

/************************************************************************/
/* COMMON REGISTERS                                                     */
/************************************************************************/
#define VPU_PRODUCT_NAME_REGISTER                 0x1040
#define VPU_PRODUCT_CODE_REGISTER                 0x1044

#define SUPPORT_MULTI_CORE_IN_ONE_DRIVER
#define MAX_VPU_CORE_NUM MAX_NUM_VPU_CORE
#define MAX_VPU_BUFFER_POOL (64*MAX_NUM_INSTANCE)

#define VpuWriteReg( CORE, ADDR, DATA )                 vdi_write_register( CORE, ADDR, DATA )					// system register write
#define VpuReadReg( CORE, ADDR )                        vdi_read_register( CORE, ADDR )							// system register write
#define VpuWriteMem( CORE, ADDR, DATA, LEN, ENDIAN )    vdi_write_memory( CORE, ADDR, DATA, LEN, ENDIAN )		// system memory write
#define VpuReadMem( CORE, ADDR, DATA, LEN, ENDIAN )     vdi_read_memory( CORE, ADDR, DATA, LEN, ENDIAN )		// system memory write

#define SUPPORT_INTERRUPT

typedef struct vpu_buffer_t {
    int size;
    unsigned long phys_addr;
    unsigned long base;
    unsigned long virt_addr;
} vpu_buffer_t;

typedef enum {
    VDI_LITTLE_ENDIAN = 0,      /* 64bit LE */
    VDI_BIG_ENDIAN,             /* 64bit BE */
    VDI_32BIT_LITTLE_ENDIAN,
    VDI_32BIT_BIG_ENDIAN,
    /* WAVE410 ONLY */
    VDI_128BIT_BIG_ENDIAN        = 16,
    VDI_128BIT_BE_BYTE_SWAP,
    VDI_128BIT_BE_WORD_SWAP,
    VDI_128BIT_BE_WORD_BYTE_SWAP,
    VDI_128BIT_BE_DWORD_SWAP,
    VDI_128BIT_BE_DWORD_BYTE_SWAP,
    VDI_128BIT_BE_DWORD_WORD_SWAP,
    VDI_128BIT_BE_DWORD_WORD_BYTE_SWAP,
    VDI_128BIT_LE_DWORD_WORD_BYTE_SWAP,
    VDI_128BIT_LE_DWORD_WORD_SWAP,
    VDI_128BIT_LE_DWORD_BYTE_SWAP,
    VDI_128BIT_LE_DWORD_SWAP,
    VDI_128BIT_LE_WORD_BYTE_SWAP,
    VDI_128BIT_LE_WORD_SWAP,
    VDI_128BIT_LE_BYTE_SWAP,
    VDI_128BIT_LITTLE_ENDIAN    = 31,
    VDI_ENDIAN_MAX
} EndianMode;


#define VDI_128BIT_ENDIAN_MASK      0xf



typedef enum {
    VDI_LINEAR_FRAME_MAP  = 0,
    VDI_TILED_FRAME_V_MAP = 1,
    VDI_TILED_FRAME_H_MAP = 2,
    VDI_TILED_FIELD_V_MAP = 3,
    VDI_TILED_MIXED_V_MAP = 4,
    VDI_TILED_FRAME_MB_RASTER_MAP = 5,
    VDI_TILED_FIELD_MB_RASTER_MAP = 6,
    VDI_TILED_FRAME_NO_BANK_MAP = 7,
    VDI_TILED_FIELD_NO_BANK_MAP = 8,
    VDI_LINEAR_FIELD_MAP  = 9,
    VDI_TILED_MAP_TYPE_MAX
} vdi_gdi_tiled_map;

typedef struct vpu_instance_pool_t {
    unsigned char   codecInstPool[MAX_NUM_INSTANCE][MAX_INST_HANDLE_SIZE];  // Since VDI don't know the size of CodecInst structure, VDI should have the enough space not to overflow.
    void*           pendingInst;
    int             pendingInstIdxPlus1;
    void*           vpu_mutex;                                              // the size to be allocated would be assigned from each VDI.c according to mutex structure of OS
    void*           vpu_disp_mutex;
    vpu_buffer_t    vpu_common_buffer;
    int             vpu_instance_num;
    int             instance_pool_inited;
    video_mm_t      vmem;
} vpu_instance_pool_t;


#if defined (__cplusplus)
extern "C" {
#endif
    int vdi_probe(unsigned long core_idx);
    int vdi_init(unsigned long core_idx);
    int vdi_release(unsigned long core_idx);	//this function may be called only at system off.

    vpu_instance_pool_t * vdi_get_instance_pool(unsigned long core_idx);
    int vdi_allocate_common_memory(unsigned long core_idx, Uint32 size);
    int vdi_get_common_memory(unsigned long core_idx, vpu_buffer_t *vb);
    int vdi_allocate_dma_memory(unsigned long core_idx, vpu_buffer_t *vb);
    int vdi_attach_dma_memory(unsigned long core_idx, vpu_buffer_t *vb);
    void vdi_free_dma_memory(unsigned long core_idx, vpu_buffer_t *vb);
    int vdi_get_sram_memory(unsigned long core_idx, vpu_buffer_t *vb);
    int vdi_dettach_dma_memory(unsigned long core_idx, vpu_buffer_t *vb);
    unsigned int vdi_get_realchip_memory_size(unsigned long coreIdx);

    int vdi_wait_interrupt(unsigned long core_idx, int timeout, unsigned int addr_bit_int_reason);
    int vdi_wait_vpu_busy(unsigned long core_idx, int timeout, unsigned int addr_bit_busy_flag);
    int vdi_wait_bus_busy(unsigned long coreIdx, int timeout, unsigned int gdi_busy_flag);
    int vdi_enable_interrupt(unsigned long coreIdx);
    int vdi_hw_reset(unsigned long core_idx);

    int vdi_set_clock_gate(unsigned long core_idx, int enable);
    int vdi_get_clock_gate(unsigned long core_idx);
    /**
     * @brief       make clock stable before changing clock frequency
     * @detail      Before inoking vdi_set_clock_freg() caller MUST invoke vdi_ready_change_clock() function. 
     *              after changing clock frequency caller also invoke vdi_done_change_clock() function.
     * @return  0   failure
     *          1   success
     */
    int vdi_ready_change_clock(unsigned long coreIdx);
    int vdi_done_change_clock(unsigned long coreIdx);

    int  vdi_get_instance_num(unsigned long core_idx);
    int  vdi_get_acture_instance_num(unsigned long coreIdx);

    void vdi_set_fourK_information(unsigned long coreIdx, unsigned long instIdx, int value);
    void vdi_set_fourK_instance_idx(unsigned long coreIdx, unsigned long instIdx, int value);
    int  vdi_get_fourK_information(unsigned long coreIdx, unsigned long instIdx);
    int  vdi_get_fourK_acture_total_instance_num(unsigned long coreIdx);
    unsigned int vdi_get_fourK_instIdx(unsigned long coreIdx, unsigned long instIdx);

    void vdi_write_register(unsigned long core_idx, unsigned int addr, unsigned int data);
    unsigned int vdi_read_register(unsigned long core_idx, unsigned int addr);
    void vdi_fio_write_register(unsigned long coreIdx, unsigned int addr, unsigned int data);
    unsigned int vdi_fio_read_register(unsigned long coreIdx, unsigned int addr);
    int vdi_clear_memory(unsigned long core_idx, unsigned int addr, int len, int endian);
    int vdi_write_memory(unsigned long core_idx, unsigned int addr, unsigned char *data, int len, int endian);
    int vdi_read_memory(unsigned long core_idx, unsigned int addr, unsigned char *data, int len, int endian);

    int vdi_lock(unsigned long core_idx);
    void vdi_unlock(unsigned long core_idx);
    int vdi_disp_lock(unsigned long core_idx);
    void vdi_disp_unlock(unsigned long core_idx);
    void vdi_set_sdram(unsigned long core_idx, unsigned int addr, int len, unsigned char data, int endian);
    void vdi_log(unsigned long coreIdx, int cmd, int step);
    int vdi_open_instance(unsigned long coreIdx, unsigned long instIdx);
    int vdi_close_instance(unsigned long coreIdx, unsigned long instIdx);
    int vdi_set_bit_firmware_to_pm(unsigned long coreIdx, const unsigned short *code);
    int vdi_get_system_endian(unsigned long coreIdx);
    int vdi_convert_endian(Uint32 coreIdx, Uint32 endian);
    void vdi_set_pll_clock(Uint32 coreIdx, Uint32 value);
    unsigned int vdi_get_pll_clock(Uint32 coreIdx);
    unsigned int vdi_get_support_vtype(Uint32 coreIdx);
    void vdi_set_clk_status(Uint32 coreIdx, Uint32 value, BOOL clk_en);

#if defined (__cplusplus)
}
#endif

#endif //#ifndef _VDI_H_ 
