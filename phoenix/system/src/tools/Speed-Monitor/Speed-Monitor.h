
#define u32 unsigned int
#define SETMASK(bits, pos)          (((-1U) >> (32-bits))  << (pos))
#define CLRMASK(bits, pos)          (~(SETMASK(bits, pos)))
#define SET_VAL(val,bits,pos)       ((val << pos) & SETMASK(bits, pos))
#define GET_VAL(reg,bits,pos)       ((reg & SETMASK(bits, pos)) >> pos)

#define DSS_C35_CTRL_ADDR           0x1801D120
#define DSS_C35_DATA_ADDR           0x1801D124
#define DSS_C35D5_CTRL_ADDR         0x1801D130
#define DSS_C35D5_DATA_ADDR         0x1801D134

#define SET_DSS_INPUT_DATA(val)     SET_VAL(val, 20,  8)
#define SET_DSS_SPEED_EN(val)       SET_VAL(val,  1,  5)
#define SET_DSS_WIRE_SEL(val)       SET_VAL(val,  1,  4)
#define SET_DSS_RO_SEL(val)         SET_VAL(val,  3,  1)
#define SET_DSS_RST_N(val)          SET_VAL(val,  1,  0)

#define GET_DSS_COUNT_OUT(reg)      GET_VAL(reg, 20,  4)
#define GET_DSS_WSORT_GO(reg)       GET_VAL(reg,  1,  1)
#define GET_DSS_READY(reg)          GET_VAL(reg,  1,  0)

/* Temperature Detect */
#define THERMAL_BASE                    0x18000314
#define SET_THERMAL_PWDB1(en)           ((en &  0x1U) << 15)
#define SET_THERMAL_SBG1(vt)            ((vt &  0x7U) << 12)
#define SET_THERMAL_SDATA1(sw)          ((sw & 0x7fU) <<  5)
#define SET_THERMAL_SINL1(sl)           ((sl &  0x3U) <<  3)
#define SET_THERMAL_SOS1(sl)            ((sl &  0x7U) <<  0)

#define GET_THERMAL_CMP_OUT1(reg)       ((reg & ( 0x1U << 16)) >> 16)
#define GET_THERMAL_PWDB1(reg)          ((reg & ( 0x1U << 15)) >> 15)
#define GET_THERMAL_SBG1(reg)           ((reg & ( 0x7U << 12)) >> 12)
#define GET_THERMAL_SDATA1(reg)         ((reg & (0x7fU <<  5)) >>  5)
#define GET_THERMAL_SINL1(reg)          ((reg & ( 0x3U <<  3)) >>  3)
#define GET_THERMAL_SOS1(reg)           ((reg & ( 0x7U <<  0)) >>  0)

typedef struct {
	u32             reg;
	u32             value;
} reg_proc;

typedef struct {
    int             fd;
    char            *mmap_base;
    u32             mmap_offset;
    u32             mmap_size;
} reg_dev;
