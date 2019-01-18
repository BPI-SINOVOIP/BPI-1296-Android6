#ifndef RTK_TEST_H
#define RTK_TEST_H

#define ENDIAN_CHANGE(x)    (((x&0xff000000)>>24)|((x&0x00ff0000)>>8)|((x&0x0000ff00)<<8)|((x&0x000000ff)<<24))
/*======= user modify start =======*/
#define RTK_SYNC 0x11223344
#define RTK_PHY_ADDR_RESERVED 0x06005000        // according to linux-kernel/arch/arm/boot/dts/rtd-119x-qa.dts
#define RTK_PHY_ADDR_RESERVED_SIZE 0x06200000   // 98MB
#define DBG_VO_ADDR 0x06002000  // physical address: communicate with VO
#define RTK_NUMBER_OF_FRAME_BUF 64
/*======= user modify end =======*/
#define RTK_STRUC_ADDR (RTK_PHY_ADDR_RESERVED + RTK_PHY_ADDR_RESERVED_SIZE - (1024 * 4))

typedef struct {
    unsigned long SYNC;
    int frame_number;
    int pic_width, pic_height;
    unsigned long phy_addr_Y[RTK_NUMBER_OF_FRAME_BUF];
    unsigned long phy_addr_Cb[RTK_NUMBER_OF_FRAME_BUF];
    unsigned long phy_addr_Cr[RTK_NUMBER_OF_FRAME_BUF];
    unsigned long vir_addr_Y[RTK_NUMBER_OF_FRAME_BUF];
    unsigned long vir_addr_Cb[RTK_NUMBER_OF_FRAME_BUF];
    unsigned long vir_addr_Cr[RTK_NUMBER_OF_FRAME_BUF];
    // vpu 
    unsigned long vpu_vir_addr_Y[RTK_NUMBER_OF_FRAME_BUF];
    unsigned long vpu_vir_addr_Cb[RTK_NUMBER_OF_FRAME_BUF];
    unsigned long vpu_vir_addr_Cr[RTK_NUMBER_OF_FRAME_BUF];
    long sizeLuma, sizeChroma, cbcrInterleave;
}RTK_VIDEO;

typedef struct{
     unsigned int magic;  //0x11223344 means OK
     unsigned int state;  //0:OFF 1:ON      //要打開V1
     unsigned int Y_addr;
     unsigned int U_addr;
     unsigned int YUVFormat; //0:420 1:422
     unsigned int width;
     unsigned int height;
     unsigned int Y_pitch;
     unsigned int C_pitch;
     unsigned int order;  // 0~2 top:2 //這個不用理他
     unsigned int display;
} YUV_DATA ;

#endif

