#include <rtk_factory.h>
#include <rtk_common.h>
#include <rtk_mtd.h>

#include <rtk_parameter.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

char factory_dir[32];
static int is_init = 0;
static int is_ro_init = 0;
static int gdisable = 0;
static BOOTTYPE gboot_type;
static int enable_layout_check = 1;
#define DEFACULT_FACTORY_DIR "/tmp/factory"
#define DEFACULT_FACTORY_RO_DIR "/tmp/factory_ro"
#define FACTORY_FILE_PATH "/tmp/factory.tar"
#define DEFACULT_FACTORY_LAYOUT "/tmp/factory/layout.txt"
#define FACTORY_ENV_FILE "/tmp/factory/env.txt"
#define ENV_SIZE 131068
#define SPI_FLASH_MAX_LIFE_CYCLE		(75000 * 2)
//#define ENABLE_TAR_VERB
//#define DUMP_BADFAC_WHEN_RECOVERY

typedef struct posix_header_t
{
    /* byte offset */
    char name[100]; /* 0 */
    char mode[8]; /* 100 */
    char uid[8]; /* 108 */
    char gid[8]; /* 116 */
    char size[12]; /* 124 */
    char mtime[12]; /* 136 */
    char chksum[8]; /* 148 */
    char typeflag; /* 156 */
    char linkname[100]; /* 157 */
    char magic[6]; /* 257 */
    char version[2]; /* 263 */
    char uname[32]; /* 265 */
    char gname[32]; /* 297 */
    char devmajor[8]; /* 329 */
    char devminor[8]; /* 337 */
    char prefix[155]; /* 345 */
    unsigned int rtk_seqnum; /*500 */
    unsigned int rtk_tarsize; /*504 */
    char rtk_signature[4]; /*508 */
} posix_header;

typedef struct environment_s
{
    uint32_t  crc; /* CRC32 over data bytes      */
    unsigned char data[ENV_SIZE]; /* Environment data          */
} env_t;

static char gdev[32];
static char gchardev[32];
static unsigned int gfactory_start;
static unsigned int gfactory_size;
static unsigned int gfactory_ro_start;
static unsigned int gfactory_ro_size;
static unsigned int gfactory_block_size;
static unsigned int gerasesize;
static unsigned int pin_pon_index;
static unsigned int seq_num = 0;
static unsigned int tarsize;
static posix_header gheader;
static char current_pp = -1;



unsigned int factory_tar_checksum(char *header)
{

    unsigned char *data = (unsigned char *) header;
    unsigned int sum;
    int i;
    // check checksum;
    sum = 0;
    for(i = 0; i < 148; i++) sum += data[i];

    for(i = 156; i < 512; i++) sum += data[i];

    for(i = 0; i < 8; i++) sum += 32;

    //install_debug("check_sum = %06o\n", sum);
    return sum;
}


void factory_tar_fill_checksum(char *cheader)
{

    posix_header* header = (posix_header*) cheader;
    sprintf(header->chksum, "%06o", factory_tar_checksum(cheader));

    header->chksum[6] = 0;
    header->chksum[7] = 0x20;
    //install_debug("fill checksun  = %s\n", header->chksum);
}

void factory_build_header(char *path)
{

    posix_header_t header;
    char tmpBuffer[2*sizeof(header)];
    rtk_file_to_ptr(path, 0, &header, sizeof(header));
    header.rtk_signature[0] = 'R';
    header.rtk_signature[1] = 'T';
    header.rtk_signature[2] = 'K';
    header.rtk_signature[3] = 0;
    header.rtk_tarsize = rtk_get_size_of_file(FACTORY_FILE_PATH);
    seq_num ++;
    header.rtk_seqnum = seq_num;
    //install_info("header (%d bytes) %d (%s)\n", sizeof(header), header.rtk_tarsize, header.magic);
    //install_info("%s \n", header.rtk_signature);
    factory_tar_fill_checksum((char *) &header);
    rtk_ptr_to_file(path, 0, &header, sizeof(header));
    rtk_ptr_to_file(path, header.rtk_tarsize, &header, sizeof(header));
    memset(tmpBuffer, 0, sizeof(tmpBuffer));
    rtk_ptr_to_file(path, header.rtk_tarsize + sizeof(header), tmpBuffer, 2*sizeof(header));

}


int factory_tar_check_header(char *cheader)
{

    posix_header* header = (posix_header *)cheader;
    if(isascii(header->name[0]) == 0)
    {
        return -1;
    }

    if(factory_tar_checksum((char *) header) != strtol(header->chksum, 0, 8))
    {
        printf("check sum error %s %o\n", header->chksum, factory_tar_checksum((char *) header));
        return -1;
    }
    return 0;

}


int factory_find_latest_update(void)
{

    posix_header p0_start, p0_end;
    posix_header p1_start, p1_end;
    int pp_ok = 0;

    // get factory header from pp0 and pp1
    // get factory header from pp0 and pp1
    rtk_file_to_ptr(gdev, gfactory_start, &p0_start, sizeof(p0_start));
    rtk_file_to_ptr(gdev, gfactory_start + gfactory_block_size, &p1_start, sizeof(p1_start));

    //install_info("p0_start.name: [%s]. \n", p0_start.name);

    if(!strncmp(p0_start.name, "tmp/factory/", strlen("tmp/factory/")))
    {
        rtk_file_to_ptr(gdev, gfactory_start + p0_start.rtk_tarsize, &p0_end, sizeof(p0_end));
        if(!memcmp(&p0_start, &p0_end, sizeof(p0_start)))
        {
            pp_ok |= 1;
        }
    }
    if(!strncmp(p1_start.name, "tmp/factory/", strlen("tmp/factory/")))
    {
        rtk_file_to_ptr(gdev, gfactory_start + gfactory_block_size + p1_start.rtk_tarsize, &p1_end, sizeof(p1_end));
        if(!memcmp(&p1_start, &p1_end, sizeof(p1_start)))
        {
            pp_ok |= 2;
        }
    }

    switch(pp_ok)
    {
        case 0:
            current_pp = -1;
            break;
        case 1:
            //install_info("pp_ok = 1. \n");
            current_pp = 0;
            break;
        case 2:
            //install_info("pp_ok = 2. \n");
            current_pp = 1;
            break;
            break;
        case 3:
            //install_info("pp_ok = 3. \n");
            if(p0_start.rtk_seqnum < p1_start.rtk_seqnum)
            {
                if((p1_start.rtk_seqnum - p0_start.rtk_seqnum) > 0xFFFFFFF0)
                {
                    current_pp = 0;
                }
                else
                {
                    current_pp = 1;
                }
            }
            else
            {
                if((p0_start.rtk_seqnum - p1_start.rtk_seqnum) > 0xFFFFFFF0)
                {
                    current_pp = 1;
                }
                else
                {
                    current_pp = 0;
                }
            }
            break;

    }


    if(current_pp == 0)
    {
        seq_num = p0_start.rtk_seqnum;
        tarsize = p0_start.rtk_tarsize;
    }
    else if(current_pp == 1)
    {
        seq_num = p1_start.rtk_seqnum;
        tarsize = p1_start.rtk_tarsize;
    }

    //install_info("pp_ok = %x current_pp = %d seq_num = %d tarsize =%d\n", pp_ok, current_pp, seq_num, tarsize);
    return 0;
}

int factory_find_next_load(int _current_pp)
{

    posix_header p0_start, p0_end;
    posix_header p1_start, p1_end;
    int pp_ok = 0;
    // get factory header from pp0 and pp1
    rtk_file_to_ptr(gdev, gfactory_start, &p0_start, sizeof(p0_start));
    rtk_file_to_ptr(gdev, gfactory_start + gfactory_block_size, &p1_start, sizeof(p1_start));

    if(!strncmp(p0_start.name, "tmp/factory/", strlen("tmp/factory/")))
    {
        rtk_file_to_ptr(gdev, gfactory_start + p0_start.rtk_tarsize, &p0_end, sizeof(p0_end));

    }
    if(!strncmp(p1_start.name, "tmp/factory/", strlen("tmp/factory/")))
    {
        rtk_file_to_ptr(gdev, gfactory_start + gfactory_block_size + p1_start.rtk_tarsize, &p1_end, sizeof(p1_end));
    }

    // reload factory from another part.
    if(_current_pp == 0)
    {
        current_pp = 1;
        seq_num = p1_start.rtk_seqnum;
        tarsize = p1_start.rtk_tarsize;

        // resave broken parts seq_num
        p0_start.rtk_seqnum -= 2;
        rtk_ptr_to_flash(&p0_start, sizeof(posix_header), gdev, gfactory_start);
        rtk_ptr_to_flash(&p0_start, sizeof(posix_header), gdev, gfactory_start + p0_start.rtk_tarsize);
    }
    else if(_current_pp == 1)
    {
        current_pp = 0;
        seq_num = p0_start.rtk_seqnum;
        tarsize = p0_start.rtk_tarsize;

        // resave broken parts seq_num
        p1_start.rtk_seqnum -= 2;
        rtk_ptr_to_flash(&p1_start, sizeof(posix_header), gdev, gfactory_start + gfactory_block_size);
        rtk_ptr_to_flash(&p1_start, sizeof(posix_header), gdev, gfactory_start + gfactory_block_size + p1_start.rtk_tarsize);
    }

    //install_info("%s:current_pp = %d seq_num = %d tarsize =%d\n", __FUNCTION__, current_pp, seq_num, tarsize);
    return 0;
}


int factory_init(const char* dir)
{
    char cmd[128];
    char tmp[128];
    int ret, ret1, ret2;
    unsigned int seq1, seq2;
    char *ptr;

    ret = get_flsh_info(&gboot_type, &gfactory_start, &gfactory_size, &gerasesize);
    if(ret < 0)
    {
        install_fail("factory utility tool init failed. \n");
        return ret;
    }

    if((gboot_type == BOOT_NAND) || (gboot_type == BOOT_EMMC))
    {
        gfactory_size /= 2;
        enable_layout_check = 0;
    }

    // gdev
    ptr = gdev;
    get_mtd_block_name(&ptr);
    strcpy(gdev, ptr);

    if(gboot_type == BOOT_SPI)
    {
        strcpy(gchardev, get_mtd_char_name());
        strcpy(gdev, gchardev);
    }else if(gboot_type == BOOT_NAND){
        //install_info("enter.nand type\n");
        ptr=NULL;
        ret=rtk_open_mtd_char(&ptr);
        if(ret>0){
          strcpy(gdev, ptr);
        }else if(ret<0){
          install_fail("get mtd block fail\n");
          return ret;
        }
    }

    //install_info("gdev:%s\n",gdev);
    sprintf(factory_dir, "%s", DEFACULT_FACTORY_DIR);

    sprintf(cmd, "/sbin/busybox mkdir -p %s", factory_dir);
    ret = rtk_command(cmd, __LINE__, __FILE__);

    gfactory_block_size = ((gfactory_size + gerasesize - 1) / gerasesize) * gerasesize;
    //install_info("gfactory_start = %x\n", gfactory_start);
    //install_info("gfactory_block_size = %x\n", gfactory_block_size);
    //install_info("gfactory_size = %x\n", gfactory_size);
    //install_info("gerasesize  = %x\n", gerasesize);

    if(gerasesize == 0) return -1;

    if(!strcmp(gdev, "/dev/mtdblock4") || !strcmp(gdev, "/dev/mtd4"))
    {
        gfactory_start = 0;
    }
    factory_find_latest_update();
    if(seq_num > SPI_FLASH_MAX_LIFE_CYCLE && gboot_type == BOOT_SPI)
    {
        install_fail("end of factory life cycle\n");
        return -1;
    }

    is_init = 1;

    return 0;
}

#ifdef FACTORY_RO_SUPPORT
int factory_init_ro(void)
{
    char cmd[128], tmp[128];
    char *ptr;
    int ret, pp_ok = 0;
    posix_header p0_start, p0_end;

    // gfactory_start
    ret = get_parameter_value("factory_ro_start", &gfactory_ro_start);
    if(ret < 0)
        return -1;

    // gfactory_size
    ret = get_parameter_value("factory_ro_size", &gfactory_ro_size);
    if(ret < 0)
        return -1;

    ret = get_parameter_string("boot_flash", tmp, 128);

    if(ret < 0)
    {
        install_fail("can't get boot type\n");
        return -1;
    }

    // so far, we only support EMMC
    if(!strcmp(tmp, "emmc"))
    {
        gboot_type = BOOT_EMMC;
    }
    else
    {
        printf("we cannot support %s boot type\n", tmp);
        return -1;
    }

    gerasesize = 512;

    // gdev
    ptr = gdev;
    get_mtd_block_name(&ptr);
    strcpy(gdev, ptr);

    sprintf(factory_dir, "%s", DEFACULT_FACTORY_RO_DIR);

    //install_info("gfactory_start = %x\n", gfactory_ro_start);
    //install_info("gfactory_size = %x\n", gfactory_ro_size);

    // get factory header and verify it
    rtk_file_to_ptr(gdev, gfactory_ro_start, &p0_start, sizeof(p0_start));
    if((!strncmp(p0_start.name, "tmp/factory_ro", strlen("tmp/factory_ro")))  &&
            (p0_start.rtk_tarsize >= sizeof(posix_header)) &&
            (factory_tar_check_header((char *) &p0_start) == 0) &&
            !strcmp(p0_start.rtk_signature, "RTK") &&
            (p0_start.rtk_tarsize < (gfactory_ro_size - sizeof(posix_header))))
    {
        rtk_file_to_ptr(gdev, gfactory_ro_start + p0_start.rtk_tarsize, &p0_end, sizeof(p0_end));
        if(!memcmp(&p0_start, &p0_end, sizeof(p0_start)))
        {
            pp_ok = 1;
        }
    }

    if(pp_ok != 1)
        return -1;

    tarsize = p0_start.rtk_tarsize;
    is_ro_init = 1;

    return 0;
}

int factory_load_ro(const char *dir)
{
    char cmd[128];
    int ret;

    if(is_ro_init == 0)
        if(factory_init_ro() < 0) return -1;

    // read factory tarfile
    sprintf(cmd, "cd /;/sbin/busybox dd if=%s bs=%u count=%u skip=%u | /sbin/busybox tar x", gdev, gerasesize,
            (tarsize + gerasesize - 1) / gerasesize, gfactory_ro_start / gerasesize);

    ret = rtk_command(cmd, __LINE__, __FILE__);
    //printf("cmd = %s\n", cmd);
    if(ret < 0)
    {
        install_fail("factory_load_ro - to extract file from flash factory section fail\r\n");
        return -1;
    }

    return 0;
}
#endif	//FACTORY_RO_SUPPORT

int factory_load(const char *dir)
{
    char cmd[128];
    int ret, retry = 0;
    if(is_init == 0)
    {
        if(factory_init() < 0) return -1;
	install_log("re-init\r\n");
    }

    if((current_pp != 0) && (current_pp != 1)){
        printf("current_pp = -1\r\n");
        return -1;
    }

redo:
    // read factory tarfile
    sprintf(cmd, "cd /;/sbin/busybox dd if=%s bs=%u count=%u skip=%u | /sbin/busybox tar x", gdev, gerasesize, gfactory_size / gerasesize,
            (gfactory_start + current_pp * gfactory_block_size) / gerasesize);

    ret = rtk_command(cmd, __LINE__, __FILE__);
    //printf("cmd = %s\n", cmd);
    if(ret < 0)
    {
        install_fail("factory_load - to extract file from mtdblock's factory section[%d]fail\r\n", current_pp);
#ifdef DUMP_BADFAC_WHEN_RECOVERY
        sprintf(cmd, "cd /;/sbin/busybox dd if=%s bs=%u count=%u skip=%u > /cache/bad_fac.tar", gdev, 512, (tarsize + 511) / 512,
                (gfactory_start + current_pp * gfactory_block_size) / 512);
        ret = rtk_command(cmd, __LINE__, __FILE__);
#endif
        if(retry == 0)
        {
            retry = 1;
            //factory_find_next_load(current_pp);
            if (current_pp == 0)
                current_pp = 1;
            else
                current_pp = 0;
            install_fail("go to retry from mtdblock's factory section[%d]\r\n", current_pp);
            goto redo;
        }
        return -1;
    }
    else
    {
        //sprintf(cmd, "dd if=/tmp/factory/layout.txt");
        //ret = rtk_command(cmd, __LINE__, __FILE__);
        if (retry)
        {
            int tmp_pp = (current_pp == 1) ? 0 : 1;
            sprintf(cmd, "cd /;/sbin/busybox dd if=/dev/zero of=/tmp/zero.bin bs=%u count=%u seek=%u conv=notrunc,sync",
                    gerasesize, gfactory_size / gerasesize,
                    (gfactory_start + tmp_pp * gfactory_block_size) / gerasesize);

            ret = rtk_command(cmd, __LINE__, __FILE__);

            factory_flush(0, 0);
        }
    }

    sprintf(cmd, "chmod 775 /tmp/factory");
    ret = rtk_command(cmd, __LINE__, __FILE__);

    if(enable_layout_check)
    {
        // Back up
        if(access("/tmp/layout.txt", F_OK))    // if /tmp/layout.txt is absent
        {
            if(access("/tmp/factory/layout.txt", F_OK))
            {
                install_fail("there is no /tmp/factory/layout.txt !!!!!\n");
            }
            else
            {
                sprintf(cmd, "/sbin/busybox cp /tmp/factory/layout.txt /tmp/layout.txt");
                ret = rtk_command(cmd, __LINE__, __FILE__);
            }

        }
    }
    return 0;
}


int spi_factory_flush(unsigned int factory_start, unsigned int factory_size)
{

    unsigned int zero = 0;
    int file_size, file_fd, data_size;
    int count;
    unsigned char data[512];
    file_size = rtk_get_size_of_file(FACTORY_FILE_PATH);
    //printf("file_size = %d\n", file_size);
    //printf("factory_start = %x\n", factory_start);
    printf("factory_size = %x\n", factory_size);
    if(file_size <= 0 || file_size > factory_size)
    {
        install_fail("%s size = %d\n", FACTORY_FILE_PATH , file_size);
        return -1;
    }
    file_fd = open(FACTORY_FILE_PATH, O_RDONLY | O_SYNC);

    if(file_fd < 0)
    {
        install_fail("open %s fail\n", FACTORY_FILE_PATH);
        return -1;
    }

    if(rtk_mtd_char_program((unsigned char *) &zero, factory_start, 4) < 0)
    {
        install_fail("header program as zero fail\n");
        return -1;
    };

    //system("echo 3 > /sys/realtek_boards/rtice_enable");
    if(rtk_mtd_char_erase(factory_start, factory_size) < 0)
    {
        install_fail("erase fail\n");
        return -1;
    };
    //system("echo 0 > /sys/realtek_boards/rtice_enable");

    for(count = 0; count < file_size;)
    {

        if((data_size = read(file_fd, data, sizeof(data))) <= 0)
        {
            install_fail("read file error %x\n", count);
            return -1;
        };

        if(count == 0 || (count + data_size >= file_size))
        {

            if(rtk_mtd_char_program(data, factory_start + count, data_size - 4) < 0)
            {
                install_fail("program fail %x %x\n", count, data_size - 1);
                return -1;
            }
            if(rtk_mtd_char_verify(data, factory_start + count, data_size - 4) < 0)
            {
                install_fail("verify fail %x %x\n", count, data_size - 1);
                return -1;
            }

            if(rtk_mtd_char_program(&data[data_size - 4], factory_start + count + data_size - 4, 4) < 0)
            {
                install_fail("program fail %x %x\n", count + data_size - 1, 1);
                return -1;
            }

            if(rtk_mtd_char_verify(&data[data_size - 4], factory_start + count + data_size - 4, 4) < 0)
            {
                install_fail("verify fail %x %x\n", count + data_size - 1, 1);
                return -1;
            }
        }
        else
        {
            if(rtk_mtd_char_program(data, factory_start + count, data_size) < 0)
            {
                install_fail("program fail %x %x\n", count, data_size);
                return -1;
            }
            if(rtk_mtd_char_verify(data, factory_start + count, data_size) < 0)
            {
                install_fail("verify fail %x %x\n", count, data_size);
                return -1;
            }
        }
        count += data_size;

    }
    return 0;
}



int factory_flush(unsigned int factory_start, unsigned int factory_size)
{
    int ret;
    char cmd[256];
    int size;
    unsigned int checksum;
    unsigned factory_block_size;
    struct stat st;
    FILE* fEnv = NULL;
    env_t tmp_env;
    uint32_t  crc;
    int ENV_Len = 131068;
    int count;

    if(is_init == 0)
    {
        if(factory_init() < 0) return -1;
	install_log("re-init\r\n");
    }

    if(factory_start == 0 || factory_size == 0)
    {
        factory_start = gfactory_start;
        factory_size = gfactory_size;
        factory_block_size = gfactory_block_size;
    }
    else
    {
        factory_block_size = ((gfactory_size + gerasesize - 1) / gerasesize) * gerasesize;
    }
    if(enable_layout_check)
    {
        if(access("/tmp/factory/layout.txt", F_OK))
        {
            install_fail("tmp/factory/layout.txt is miss!!!!!!!! stop factory save");
            return -1;
        }
        if(rtk_file_compare("/tmp/factory/layout.txt", "/tmp/layout.txt") < 0)
        {
            install_fail("tmp/factory/layout.txt content change !!!!! stop factory save");
            return -1;
        }
    }

    if (stat(FACTORY_ENV_FILE, &st) == 0)
    {
        fEnv = fopen(FACTORY_ENV_FILE, "rb+");
        if (fEnv != NULL)
        {
            count = fread(&tmp_env, sizeof(tmp_env), 1, fEnv);
            if (count )
            {
                crc32(&tmp_env.data[0], ENV_Len, &crc);
                if (crc != tmp_env.crc)
                {
                    tmp_env.crc = crc;
                    fseek (fEnv, 0, SEEK_SET);
                    count = fwrite(&tmp_env, sizeof(tmp_env), 1, fEnv);
                    if (count != 1)
                    {
                        printf("Failed writing %s\n(%s)\n", FACTORY_ENV_FILE, strerror(errno));
                        return -1;
                    }
                }
            }
        }

    }
#ifdef ENABLE_TAR_VERB
    sprintf(cmd, "/sbin/busybox tar cvf %s %s", FACTORY_FILE_PATH, factory_dir);
#else
    sprintf(cmd, "/sbin/busybox tar cf %s %s", FACTORY_FILE_PATH, factory_dir);
#endif
    ret = rtk_command(cmd, __LINE__, __FILE__);
    if(ret < 0)
    {
        install_fail("rtk_command fail\r\n");
        return -1;
    }
    usleep(200000);
    //install_info("*****************\n");
    factory_build_header(FACTORY_FILE_PATH);
    system("/sbin/busybox sync");
    //install_info("*****************\n");
    size = rtk_get_size_of_file(FACTORY_FILE_PATH);


    if(size > factory_size || size < (sizeof(posix_header)) * 2)
    {
        install_fail("factory.tar(%d) large then default FACTORY_SIZE(%d)\n", size, factory_size);
        return -1;
    }

    install_log("%s filesize:%u bytes\r\n", FACTORY_FILE_PATH, rtk_get_size_of_file(FACTORY_FILE_PATH));

    if (current_pp !=0 && current_pp!=1)
	return -1;
    else
    {
        current_pp++;
        current_pp &= 0x01;
    }
    //install_info("save to current_pp = %d seq_num = %d\n", current_pp, seq_num);

    if(gboot_type == BOOT_SPI)
    {
        spi_factory_flush(factory_start + current_pp * factory_block_size, factory_size);
        sprintf(cmd, "/sbin/busybox rm %s", FACTORY_FILE_PATH);
        ret = rtk_command(cmd, __LINE__, __FILE__);

        if(ret < 0)
        {
            install_fail("rtk_command fail\r\n");
            return -1;
        }

    }
    else if(gboot_type == BOOT_NAND)
    {
#if 0
        sprintf(cmd, "/sbin/busybox dd if=%s of=%s bs=%u count=%u seek=%u conv=notrunc,sync; /sbin/busybox rm %s", FACTORY_FILE_PATH, gdev, gerasesize, gfactory_size / gerasesize, (factory_start + current_pp * factory_block_size) / gerasesize, FACTORY_FILE_PATH);
#else
        install_info("-----nandflash factory burn------ \n\
                      nand mtd path:%s\n\
                      factory_offset:0x%08x\n\
                      factory_size:0x%08xbytes\n\
                      (%u sector)\r\n"
                    , gdev
                    , (factory_start + current_pp * factory_size)
                    , factory_size
                    , factory_size/gerasesize);
        memset(cmd, 0, sizeof(cmd));
        sprintf(cmd, "flash_erase %s %u %u"
                 , gdev
                 , (factory_start + current_pp * factory_size)
                 , factory_size/gerasesize);
        ret = rtk_command(cmd, __LINE__, __FILE__);
        if(ret < 0){
            install_debug("Exec command fail\r\n");
            return -1;
        }
        memset(cmd, 0, sizeof(cmd));
        sprintf(cmd,"nandwrite -m -p -s %u %s %s",
                    (factory_start + current_pp * factory_size),
                    gdev,
                    FACTORY_FILE_PATH);
#endif
        ret = rtk_command(cmd, __LINE__, __FILE__);
        if(ret < 0)
        {
            install_fail("rtk_command fail\r\n");
        }
    }
    else if(gboot_type == BOOT_EMMC)
    {
        sprintf(cmd, "cd /;/sbin/busybox dd if=/dev/zero of=/tmp/zero.bin bs=%u count=%u seek=%u conv=notrunc,sync",
                gerasesize, gfactory_size / gerasesize,
                (gfactory_start + current_pp * factory_block_size) / gerasesize);

        ret = rtk_command(cmd, __LINE__, __FILE__);
        ret = rtk_file_to_flash(FACTORY_FILE_PATH, 0, gdev, (factory_start + current_pp * factory_block_size), size, NULL);
        if(ret < 0)
        {
            install_fail("write factory to flash fail!");
        }
        sprintf(cmd, "/sbin/busybox rm %s", FACTORY_FILE_PATH);
        ret = rtk_command(cmd, __LINE__, __FILE__);
        if(ret < 0)
        {
            install_fail("rtk_command fail\r\n");
        }
    }

    return 0;
}

