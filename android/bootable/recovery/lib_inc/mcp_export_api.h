#ifndef __MCP_EXPORT_API_H__
#define __MCP_EXPORT_API_H__


#ifdef __cplusplus
extern "C" {
#endif
#include "osal_type.h"

#define SHA256_SIZE	32
typedef int ion_user_handle_t;

int SHA1_dohash(unsigned int pDataIn, int length, unsigned int pDataOut, unsigned int iv[5]);
int SHA256_dohash(unsigned int pDataIn, int length, unsigned int pDataOut, unsigned int iv[8]);

void* RTKIon_alloc(OSAL_U32 size, OSAL_U8 **noncacheable, OSAL_U32 *phys_addr, ion_user_handle_t *ionhdl);
void RTKIon_free(void *addr, unsigned int size, ion_user_handle_t *ionhdl);
void RTKIon_sync();

#ifdef __cplusplus
}
#endif

#endif
