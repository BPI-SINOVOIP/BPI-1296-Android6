#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <pthread.h>
#include <termios.h>
#include "Speed-Monitor.h"

static int debug    = 1;
static int warning  = 1;
static int info     = 1;

#define IS_STR(arg,x) (arg != NULL && strcmp(arg,x) == 0)

#define dprintf(msg...) if (debug)   { printf(msg); }
#define eprintf(msg...) if (1)       { printf(msg); }
#define wprintf(msg...) if (warning) { printf(msg); }
#define iprintf(msg...) if (info)    { printf(msg); }
#define REG_MMAP_DEV    "/dev/uio250"

int reg_init(reg_dev * pregDev)
{
    if (pregDev->fd != -1) return 0;

    pregDev->mmap_offset    = 0x18000000;
    pregDev->mmap_size      = 0x60000;

    pregDev->fd = open(REG_MMAP_DEV,O_RDWR|O_SYNC);
    if (pregDev->fd == -1) goto err;
    pregDev->mmap_base = mmap(NULL, pregDev->mmap_size, PROT_READ|PROT_WRITE, MAP_SHARED , pregDev->fd, 0);
    if (pregDev->mmap_base == 0) goto err;
    return 0;
    err:
    eprintf(" %s:%d Error! (fd:%d base:0x%08x offset:0x%08x size:%d) \n",__func__,__LINE__,
            pregDev->fd,(unsigned int)pregDev->mmap_base, pregDev->mmap_offset, pregDev->mmap_size);
    return -1;
}

void reg_uninit(reg_dev * pregDev)
{
    if (pregDev->fd == -1) return;
    munmap(pregDev->mmap_base,pregDev->mmap_size);
    close(pregDev->fd);
}

unsigned int reg_readU32 (reg_dev * pregDev, unsigned int phtAddr)
{
    int offset = phtAddr - pregDev->mmap_offset;
    if (offset < 0 || offset > pregDev->mmap_size) goto err;
    return *(unsigned int *)(pregDev->mmap_base + offset);
    err:
    eprintf(" %s:%d Error!\n",__func__,__LINE__);
    return 0;
}

void reg_writeU32 (reg_dev * pregDev, unsigned int phtAddr, unsigned int data)
{
    int offset = phtAddr - pregDev->mmap_offset;
    if (offset < 0 || offset > pregDev->mmap_size) goto err;
    *(unsigned int *)(pregDev->mmap_base + offset) = data;
    return;
    err:
    eprintf(" %s:%d Error!\n",__func__,__LINE__);
    return;
}

typedef struct {
    unsigned int dss_ctrl_phyaddr;
    unsigned int dss_data_phyaddr;
    unsigned int WireSel;
    unsigned int RoSel;
} dss_config;

unsigned int GetDSSCountOut(reg_dev *pregDev, dss_config * config)
{
    unsigned int ctrlAddr = config->dss_ctrl_phyaddr;
    unsigned int dataAddr = config->dss_data_phyaddr;
    unsigned int WireSel  = config->WireSel;
    unsigned int RoSel    = config->RoSel;
    unsigned int reg;

    reg_writeU32( pregDev, ctrlAddr,
            SET_DSS_INPUT_DATA(0)       |
            SET_DSS_SPEED_EN(0)         |
            SET_DSS_WIRE_SEL(WireSel)  |
            SET_DSS_RO_SEL(RoSel)      |
            SET_DSS_RST_N(0)            );

    reg_writeU32( pregDev, ctrlAddr,
            SET_DSS_INPUT_DATA(0)       |
            SET_DSS_SPEED_EN(0)         |
            SET_DSS_WIRE_SEL(WireSel)  |
            SET_DSS_RO_SEL(RoSel)      |
            SET_DSS_RST_N(1)            );

    reg_writeU32( pregDev, ctrlAddr,
            SET_DSS_INPUT_DATA(0)       |
            SET_DSS_SPEED_EN(1)         |
            SET_DSS_WIRE_SEL(WireSel)  |
            SET_DSS_RO_SEL(RoSel)      |
            SET_DSS_RST_N(1)            );

    do {
        reg = reg_readU32( pregDev, dataAddr);
        usleep(100);
    } while (GET_DSS_READY(reg) == 0);

    reg_writeU32( pregDev, ctrlAddr,
            SET_DSS_INPUT_DATA(0)       |
            SET_DSS_SPEED_EN(0)         |
            SET_DSS_WIRE_SEL(WireSel)  |
            SET_DSS_RO_SEL(RoSel)      |
            SET_DSS_RST_N(0)            );

    //printf("0x%08x\n",GET_DSS_COUNT_OUT(reg));

    return GET_DSS_COUNT_OUT(reg);

}

unsigned int GetTemp(reg_dev *pregDev)
{
    unsigned int temp;
#if 0
    FILE * fptr = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    if (fptr == NULL) return 0;
    fscanf(fptr,"%d",&temp);
    fclose(fptr);
#else
    unsigned int compareDelayUs = 15;
    {
        reg_writeU32(pregDev, THERMAL_BASE,
                SET_THERMAL_PWDB1(0x1)      |
                SET_THERMAL_SBG1(0x4)       |
                SET_THERMAL_SDATA1(0x40)    |
                SET_THERMAL_SINL1(0x1)      |
                SET_THERMAL_SOS1(0x4)       );
        usleep(compareDelayUs);
    }
    {
        int i;
        for (i=6; i>=0; i--) {
            u32 reg = reg_readU32(pregDev,THERMAL_BASE);
            u32 cmp_data = (0x1U << i); // 7'b
            reg_writeU32(pregDev, THERMAL_BASE, reg | SET_THERMAL_SDATA1(cmp_data));
            usleep(compareDelayUs);
            reg = reg_readU32(pregDev,THERMAL_BASE);
            if (GET_THERMAL_CMP_OUT1(reg) == 0)
                reg_writeU32(pregDev, THERMAL_BASE, reg & ~SET_THERMAL_SDATA1(cmp_data));
        }
    }
    {
        u32 reg = reg_readU32(pregDev, THERMAL_BASE);
        double temperature = GET_THERMAL_SDATA1(reg);
        temperature = ((temperature*165)/128)-40-7;
        temp = (unsigned int) (temperature * 1000);
    }
#endif
    return temp;
}

int main(int argc, char **argv)
{
    reg_dev regDev;
    reg_dev *pregDev = &regDev;
    unsigned int gWireSel = 0;
    unsigned int gRoSel = 0;
    unsigned int CountOut[2];
    unsigned int delayMs = 100;
    pregDev->fd = -1;
    reg_init(pregDev);

    {
        char s[10];
        int i;
        for (i=1;i<argc;i++){
            sscanf(argv[i],"%s",s);
            if (!strcmp(s,"-d")) {
                unsigned int w;
                sscanf(argv[i+1],"%d",&w);
                delayMs = w;
            }
        }
    }

    while(1) {
        for (gWireSel=0; gWireSel<=1; gWireSel++) {
            for (gRoSel=0; gRoSel<=5; gRoSel++) {
                dss_config config = {
                    .WireSel = gWireSel,
                    .RoSel   = gRoSel,
                };

                config.dss_ctrl_phyaddr = DSS_C35_CTRL_ADDR;
                config.dss_data_phyaddr = DSS_C35_DATA_ADDR;
                CountOut[0] = GetDSSCountOut(pregDev, &config);

                config.dss_ctrl_phyaddr = DSS_C35D5_CTRL_ADDR;
                config.dss_data_phyaddr = DSS_C35D5_DATA_ADDR;
                CountOut[1] = GetDSSCountOut(pregDev, &config);
                double          temperature = (double) GetTemp(pregDev) /1000;

                //printf("temp:%.1f°C WS:%d RS:%d CountOut C35:0x%x C35D5:0x%x\n", temperature, gWireSel,gRoSel,CountOut[0],CountOut[1]);
                printf("temp:%.1f WS:%d RS:%d CountOut C35:0x%x C35D5:0x%x\n", temperature, gWireSel,gRoSel,CountOut[0],CountOut[1]);

            }
        }
        usleep(delayMs*1000);
    }


exit:
    reg_uninit(&regDev);
    return 0;
}
