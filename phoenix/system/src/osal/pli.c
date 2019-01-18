#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

#include "sizes.h"

#define DEBUG
//#define TEST_PLI

#ifdef DEBUG
#define	pr_info		printf
#define pr_warn		printf
#define pr_err		printf
#else
#define	pr_info(...)
#define pr_warn(...)
#define pr_err(...)
#endif

#define MODULE_NAME "/dev/mem"

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n",\
		__LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

#define REG_PHYS_BASE	0x18000000
#define REG_SIZE		0x00060000

static int		pli_fd = 0;
volatile void	*reg_base_addr;

inline unsigned int pli_getRtc32(void)
{
	struct	timeval tp;

	gettimeofday(&tp, NULL);
	return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int64_t pli_getMilliseconds(void)
{
	return (int64_t)pli_getRtc32();
}

// 27 MHZ counter
int64_t pli_getSCR(void)
{
	struct	timeval tv;
	int64_t	PTS;

	/* Obtain the time of day, and convert it to a tm struct */
	gettimeofday(&tv, NULL);
	/* Compute PTS from microseconds */
	PTS = (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
	PTS *= 27;

	return PTS;
}

int pli_print_time(char *pTime, int size)
{
	struct timeval	tv;
	struct tm*		ptm;
	char			time_string[40];
	long			miliseconds;

	/* Obtain the time of day, and convert it to a tm struct */
	gettimeofday(&tv, NULL);
	ptm = (struct tm*)localtime(&tv.tv_sec);
	if (ptm == NULL) FATAL;

	/* Format the date and time, down to a single second. */
	strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", ptm);
	/* Compute miliseconds from microseconds */
	miliseconds = tv.tv_usec / 1000;
	/* Print the formatted time, in seconds, followed by a decimal point and the miliseconds. */
	pr_info("%s.%03ld\n", time_string, miliseconds);

	return 0;
}

int pli_getTime(struct tm* time)
{
	struct timeval	tv;
	struct tm*		ptm;

	/* Obtain the time of day, and convert it to a tm struct. */
	gettimeofday (&tv, NULL);
	ptm = (struct tm*)localtime (&tv.tv_sec);
	if (ptm == NULL) FATAL;

	time->tm_sec	= ptm->tm_sec;
	time->tm_min	= ptm->tm_min;
	time->tm_hour	= ptm->tm_hour;
	time->tm_mday	= ptm->tm_mday;
	time->tm_mon	= ptm->tm_mon;
	time->tm_year	= ptm->tm_year;
	time->tm_wday	= ptm->tm_wday;
	time->tm_yday	= ptm->tm_yday;
	time->tm_isdst	= ptm->tm_isdst;

	return 0;
}


int pli_init(void)
{
	if (pli_fd != 0) {
		pr_warn("Open pli interface twice...\n");
		return 1;
	}

	if ((pli_fd = open(MODULE_NAME, O_RDWR | O_SYNC)) == -1) FATAL;

	reg_base_addr = (volatile void*)mmap(NULL,
						REG_SIZE,
						PROT_READ | PROT_WRITE,
						MAP_SHARED,
						pli_fd,
						REG_PHYS_BASE);
	if (MAP_FAILED == reg_base_addr) {
		close(pli_fd);
		FATAL;
	}

	return 0;
}

int pli_close(void)
{
	int ret;

	munmap((void*) reg_base_addr, (size_t)REG_SIZE);
	ret = close(pli_fd);
	pli_fd = 0;

	return ret;
}

volatile int* pli_getIOAddress(int addr)
{
	return (volatile int*)(reg_base_addr + addr);
}

unsigned int pli_readReg32(unsigned int addr)
{
	return *((volatile int*)(reg_base_addr + addr));
}

void pli_writeReg32(unsigned int addr, unsigned int value)
{
	*((volatile int*)(reg_base_addr + addr)) = value;
}

void *pli_allocContinuousMemory(size_t size, unsigned char** nonCachedAddr, unsigned long *phyAddr)
{
	if (size <= 0) FATAL;

	char *ptr;

	if (size < SZ_64K) {
//		ptr = (char*) ionapi_alloc(size, SZ_4K, ION_HEAP_SYSTEM_CONTIG_MASK, (void**)nonCachedAddr, phyAddr, NULL);
	}
	else {
//		ptr = (char*) ionapi_alloc(size, SZ_16K, RTK_SATURN_ION_HEAD_MEDIA_MASK, (void**)nonCachedAddr, phyAddr, NULL);
	}

	ptr = (char*)(*nonCachedAddr);

	return ptr;
}

void pli_freeContinuousMemory(void *ptr)
{
//	ionapi_free(ptr, 0, NULL);
}

#ifdef TEST_PLI
#define MISC_REG_OFFSET		0x0001B000

int main(int argc, char **argv)
{
	volatile int *misc_reg;
	int i;

	pli_init();

	misc_reg = (volatile int*)(reg_base_addr + MISC_REG_OFFSET);
	for (i=0; i<4; i++) {
		pr_info("[0x%08x]: %08x %08x %08x %08x\n",(unsigned int)misc_reg, *misc_reg, *(misc_reg+1), *(misc_reg+2), *(misc_reg+3));
		misc_reg += 4;
	}

	pli_close();
	return 0;
}
#endif //~TEST_PLI
