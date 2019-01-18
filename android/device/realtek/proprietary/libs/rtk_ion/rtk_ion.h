#ifndef RTK_ION_API_H
#define RTK_ION_API_H

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <ion/ion.h>

typedef struct  {
    int mmap_fd;
    /* usually size >= want_size, ex: if want_size == 32B, size = 4KB */
    unsigned int size;  // memroy size got from ION
    unsigned char* ion_virt;
    unsigned char* ion_phy;
    int flag;
    ion_user_handle_t ion_handle;
    unsigned int want_size; // memory size which want to get
} ION_DATA;

#ifdef __cplusplus
extern "C" {
#endif

int rtk_ion_malloc(ION_DATA *p, size_t size, size_t align, unsigned int heap, unsigned int flag);
int rtk_ion_free(ION_DATA *p);
void rtk_ion_close();
int rtk_ion_flush_handle_fd(int handle_fd);
int rtk_ion_invalidate_fd(int handle_fd);

#ifdef __cplusplus
}
#endif

#endif
