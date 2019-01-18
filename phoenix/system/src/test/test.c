#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "osal/pli.h"

#define MISC_REG_OFFSET		0x0001B000

extern int pthread_mutex_test(void);

void dump_reg(unsigned int base, unsigned int val)
{
	volatile int *misc_reg;
	unsigned int reg_offset;
	int i;

	misc_reg = pli_getIOAddress(MISC_REG_OFFSET);
	for (i=0; i<4; i++) {
		printf("[0x%08x]: %08x %08x %08x %08x\n",(unsigned int)misc_reg, *misc_reg, *(misc_reg+1), *(misc_reg+2), *(misc_reg+3));
		misc_reg += 4;
	}
	reg_offset = MISC_REG_OFFSET;
	for (i=0; i<4; i++) {
		printf("%08x ", pli_readReg32(reg_offset));
		reg_offset += 4;
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	int c;

	pli_init();

	while ((c = getopt(argc, argv, "abcq:")) != -1) {
		switch(c)
		{
			case 'a':
				dump_reg(MISC_REG_OFFSET, 16);
				break;
			case 'b':
				pthread_mutex_test();
				break;
			case 'q':
			default:
				goto out;
				break;
		}
	}

out:
	pli_close();
	return 0;
}
