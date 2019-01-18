#ifndef __PLI_H__
#define __PLI_H__

#include <time.h>
#include <sys/time.h>

int64_t pli_getMilliseconds(void);
int64_t pli_getSCR(void);
int pli_print_time(char *, int);
int pli_getTime(struct tm*);

int pli_init(void);
int pli_close(void);

volatile int* pli_getIOAddress(int);
unsigned int pli_readReg32(unsigned int);
void pli_writeReg32(unsigned int, unsigned int);

#endif //__PLI_H__
