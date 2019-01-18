/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fs_mgr.h>
#include "bootloader.h"
#include "common.h"
#include "mtdutils/mtdutils.h"
#include "roots.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <ctype.h>
#include <fcntl.h>
#include <limits.h>

#define RECOVERY_FILE_IN_FACTORY  "/tmp/factory/recovery"

int execFactoryUtil(FACTORY_ACT  _act)
{
    int ret;
    pid_t child_pid;
    int status;
    char prog[]="/sbin/factory";
    //char prog[]="/tmp/factory2";
    char act[6] = {'\0'};
    child_pid = fork();

    if (child_pid < 0)
    {
        return -1;
    }

    if(_act == FAC_LOAD)
    {
        sprintf(act, "%s", "load");
    }
    else
    {
        sprintf(act, "%s", "save");
    }

    if (child_pid== 0)
    {
        ret = execl(prog,"factory",act,NULL);
        if(ret==-1)
        {
            LOGE("Execute Factory Fail! Please Check you have factory utility");
        }
        exit(0);
    }
    if(waitpid(child_pid,NULL,0)<0)
        LOGE("waitpid error!");
    if(child_pid>0)
        LOGE("parent pid=%d\n",child_pid);

    return 0;
}

static int get_bootloader_message_mtd(struct bootloader_message *out, const Volume* v);
static int set_bootloader_message_mtd(const struct bootloader_message *in, const Volume* v);
static int get_bootloader_message_block(struct bootloader_message *out, const Volume* v);
static int set_bootloader_message_block(const struct bootloader_message *in, const Volume* v);

int get_bootloader_message(struct bootloader_message *out)
{
#if 0
    Volume* v = volume_for_path("/misc");
    if (v == NULL)
    {
        LOGE("Cannot load volume /misc!\n");
        return -1;
    }
    if (strcmp(v->fs_type, "mtd") == 0)
    {
        return get_bootloader_message_mtd(out, v);
    }
    else if (strcmp(v->fs_type, "emmc") == 0)
    {
        return get_bootloader_message_block(out, v);
    }
    LOGE("unknown misc partition fs_type \"%s\"\n", v->fs_type);
    return -1;
#else
    // we should get info from factory dir
//int ret;
//ret = system("echo test > /tmp/test");
//printf("** get %d after echo test > /tmp/test\n",ret);

#if 0
    pid_t child_pid;
    int status;
    char prog[]="/usr/sbin/factory";
//char prog[]="/tmp/fac";

    printf("** Execuing program: %s\n\n",prog);

    child_pid = fork();

    if (child_pid < 0)
    {
        return -1;
    }

    if (child_pid== 0)
    {
        ret = execl(prog,"factory","load",NULL);
        if(ret==-1)
        {
            printf("here0\n");
        }
        printf("ret=%d\n",ret);
        exit(0);
    }
    if(waitpid(child_pid,NULL,0)<0)
        perror("waitpid");
    if(child_pid>0)
        printf("pid=%d\n",child_pid);
#endif
//ret =    WEXITSTATUS(system("/usr/sbin/factory load"));
//    wait(NULL);
//printf("** get %d after /usr/sbin/factory load\n",ret);
    execFactoryUtil(FAC_LOAD);

    FILE* f = fopen(RECOVERY_FILE_IN_FACTORY, "rb");
    if (f == NULL)
    {
        LOGE("Can't open recovery file\n(%s)\n", strerror(errno));
        return -1;
    }
    struct bootloader_message temp;
    int count = fread(&temp, sizeof(temp), 1, f);
    if (count != 1)
    {
        LOGE("Failed reading recovery cmd\n(%s)\n", strerror(errno));
        return -1;
    }
    if (fclose(f) != 0)
    {
        LOGE("Failed closing recovery in factory\n(%s)\n", strerror(errno));
        return -1;
    }
    memcpy(out, &temp, sizeof(temp));
    return 0;

#endif
}

int set_bootloader_message(const struct bootloader_message *in)
{
#if 0
    Volume* v = volume_for_path("/misc");
    if (v == NULL)
    {
        LOGE("Cannot load volume /misc!\n");
        return -1;
    }
    if (strcmp(v->fs_type, "mtd") == 0)
    {
        return set_bootloader_message_mtd(in, v);
    }
    else if (strcmp(v->fs_type, "emmc") == 0)
    {
        return set_bootloader_message_block(in, v);
    }
    LOGE("unknown misc partition fs_type \"%s\"\n", v->fs_type);
    return -1;
#else
    printf("set_bootloader_message.\n");

    execFactoryUtil(FAC_LOAD);
#if 0
    FILE* f = fopen(RECOVERY_FILE_IN_FACTORY, "wb");
    if (f == NULL)
    {
        LOGE("Can't open %s\n(%s)\n", RECOVERY_FILE_IN_FACTORY, strerror(errno));
        return -1;
    }
    int count = fwrite(in, sizeof(*in), 1, f);
    if (count != 1)
    {
        LOGE("Failed writing %s\n(%s)\n", RECOVERY_FILE_IN_FACTORY, strerror(errno));
        return -1;
    }
    fflush(f);
    if (fclose(f) != 0)
    {
        LOGE("Failed closing %s\n(%s)\n",RECOVERY_FILE_IN_FACTORY, strerror(errno));
        return -1;
    }
#endif
    system("/sbin/busybox rm -rf /tmp/factory/recovery");
    execFactoryUtil(FAC_SAVE);
    return 0;
#endif
}

// ------------------------------
// for misc partitions on MTD
// ------------------------------

static const int MISC_PAGES = 3;         // number of pages to save
static const int MISC_COMMAND_PAGE = 1;  // bootloader command is this page

static int get_bootloader_message_mtd(struct bootloader_message *out,
                                      const Volume* v)
{
    size_t write_size;
    mtd_scan_partitions();
    const MtdPartition *part = mtd_find_partition_by_name(v->blk_device);
    if (part == NULL || mtd_partition_info(part, NULL, NULL, &write_size))
    {
        LOGE("Can't find %s\n", v->blk_device);
        return -1;
    }

    MtdReadContext *read = mtd_read_partition(part);
    if (read == NULL)
    {
        LOGE("Can't open %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }

    const ssize_t size = write_size * MISC_PAGES;
    char data[size];
    ssize_t r = mtd_read_data(read, data, size);
    if (r != size) LOGE("Can't read %s\n(%s)\n", v->blk_device, strerror(errno));
    mtd_read_close(read);
    if (r != size) return -1;

    memcpy(out, &data[write_size * MISC_COMMAND_PAGE], sizeof(*out));
    return 0;
}
static int set_bootloader_message_mtd(const struct bootloader_message *in,
                                      const Volume* v)
{
    size_t write_size;
    mtd_scan_partitions();
    const MtdPartition *part = mtd_find_partition_by_name(v->blk_device);
    if (part == NULL || mtd_partition_info(part, NULL, NULL, &write_size))
    {
        LOGE("Can't find %s\n", v->blk_device);
        return -1;
    }

    MtdReadContext *read = mtd_read_partition(part);
    if (read == NULL)
    {
        LOGE("Can't open %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }

    ssize_t size = write_size * MISC_PAGES;
    char data[size];
    ssize_t r = mtd_read_data(read, data, size);
    if (r != size) LOGE("Can't read %s\n(%s)\n", v->blk_device, strerror(errno));
    mtd_read_close(read);
    if (r != size) return -1;

    memcpy(&data[write_size * MISC_COMMAND_PAGE], in, sizeof(*in));

    MtdWriteContext *write = mtd_write_partition(part);
    if (write == NULL)
    {
        LOGE("Can't open %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }
    if (mtd_write_data(write, data, size) != size)
    {
        LOGE("Can't write %s\n(%s)\n", v->blk_device, strerror(errno));
        mtd_write_close(write);
        return -1;
    }
    if (mtd_write_close(write))
    {
        LOGE("Can't finish %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }

    LOGI("Set boot command \"%s\"\n", in->command[0] != 255 ? in->command : "");
    return 0;
}


// ------------------------------------
// for misc partitions on block devices
// ------------------------------------

static void wait_for_device(const char* fn)
{
    int tries = 0;
    int ret;
    struct stat buf;
    do
    {
        ++tries;
        ret = stat(fn, &buf);
        if (ret)
        {
            printf("stat %s try %d: %s\n", fn, tries, strerror(errno));
            sleep(1);
        }
    }
    while (ret && tries < 10);
    if (ret)
    {
        printf("failed to stat %s\n", fn);
    }
}

static int get_bootloader_message_block(struct bootloader_message *out,
                                        const Volume* v)
{
    wait_for_device(v->blk_device);
    FILE* f = fopen(v->blk_device, "rb");
    if (f == NULL)
    {
        LOGE("Can't open %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }
    struct bootloader_message temp;
    int count = fread(&temp, sizeof(temp), 1, f);
    if (count != 1)
    {
        LOGE("Failed reading %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }
    if (fclose(f) != 0)
    {
        LOGE("Failed closing %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }
    memcpy(out, &temp, sizeof(temp));
    return 0;
}

static int set_bootloader_message_block(const struct bootloader_message *in,
                                        const Volume* v)
{
    wait_for_device(v->blk_device);
    FILE* f = fopen(v->blk_device, "wb");
    if (f == NULL)
    {
        LOGE("Can't open %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }
    int count = fwrite(in, sizeof(*in), 1, f);
    if (count != 1)
    {
        LOGE("Failed writing %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }
    if (fclose(f) != 0)
    {
        LOGE("Failed closing %s\n(%s)\n", v->blk_device, strerror(errno));
        return -1;
    }
    return 0;
}
