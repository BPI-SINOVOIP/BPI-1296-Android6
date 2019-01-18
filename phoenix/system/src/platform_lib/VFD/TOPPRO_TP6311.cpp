/*
	VFD User-space Access Library:

	*must* be used combined with Venus VFD device driver which exposes

	/dev/venus_vfdo			[w]
	/dev/venus_vfd_wrctl	[rw]
	/dev/venus_vfd_keypad	[r]

	The functions are designed for 
	*) TOPPRO TP6311 VFD Controller

	Author:		Chih-pin Wu <wucp@realtek.com.tw>
	Start Date:	2005/11/17


Remark:
	This driver is for Little-Endian Processor only

Updates:
	2007/02/14: 
	add disableAutoRead() and enableAutoRead() to prevent 
	conflict between write and auto-read transaction

*/

#include <Platform_Lib/FrontControl/VFD_l.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>

// period for force update whole display memory [s]
#define PERIOD_FORCE_UPDATE	300
// period for waiting auto-read completion [us]
#define PERIOD_WAIT_AUTO_READ 100 

unsigned int VFD_Controller::m_displayMode;
unsigned int VFD_Controller::m_dimmingQuality;

VFD_Controller::VFD_Controller(int digit, unsigned char pulseWidth)
	:m_LED(0x1f), 
	bNeedUpdate(false)
{
	unsigned int vfdo_value, wrctl_value;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&mem_mutex, &attr);
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&drv_mutex, &attr);
	pthread_mutexattr_destroy(&attr);

	if(digit >= 8 && digit <= 16)
		m_displayMode = digit-1;
	else
		assert(0);

	fd_vfdo 	= open(VENUS_VFDO, O_RDWR);
	fd_wrctl 	= open(VENUS_VFD_WRCTL, O_RDWR);

	setDimmingQuality(pulseWidth);
	/* Initialize TP6311 VFD Controller */

	/* [Byte1 : Display mode setting command - by parameter digit] 
	   0x00 = 0000-0000
	   [Byte0 : Display control command, turn on display, pulse width = 14/16] 
	   0x8f = 1000-1111 */
	vfdo_value = (m_dimmingQuality << 8) | (m_displayMode);

	/* use WRCTL to send command
	   0x0330 = 1-0011-0011-0000 */
	wrctl_value = 0x00001330;

	pthread_mutex_lock(&drv_mutex);
	disableAutoRead();
	writeAtOnce(vfdo_value, wrctl_value);
	enableAutoRead();
	pthread_mutex_unlock(&drv_mutex);

	pthread_create(&id_updater, NULL, DisplayUpdater, (void *)this);

	/* Initialize TP6311 Display Memory (0x00 - 0x2f) 48 bytes */
	InitDisplayRAM();
}

VFD_Controller::~VFD_Controller() {
	unsigned int vfdo_value, wrctl_value;

	pthread_cancel(id_updater);
	pthread_mutex_destroy(&mem_mutex);
	pthread_mutex_destroy(&drv_mutex);

	if(fd_vfdo == -1 || fd_wrctl == -1)
		return;

	vfdo_value = 0x80;
	wrctl_value = 0x1110;
	disableAutoRead();
	writeAtOnce(vfdo_value, wrctl_value);
	enableAutoRead();

	close(fd_wrctl);
	close(fd_vfdo);
}

// public member functions
void VFD_Controller::InitDisplayRAM(void) {
	int i;

	pthread_mutex_lock(&mem_mutex);

	for(i = 0 ; i < 48 ; i++)
		m_DisplayRAM[i] = 0x00;

	pthread_mutex_unlock(&mem_mutex);

	UpdateDisplayRAM();
}

void VFD_Controller::UpdateDisplayRAM(void) {
	bNeedUpdate = true;
	return;
}

void * VFD_Controller::DisplayUpdater(void *pInstance) {
	VFD_Controller *myself = (VFD_Controller *)pInstance;

	pli_setThreadName("VFD_Updater");
	unsigned char DisplayRAM[48];
	unsigned char origDisplayRAM[48];
	time_t lastUpdateTime;

	memset(DisplayRAM, 0xff, sizeof(unsigned char)*48);
	memset(origDisplayRAM, 0xff, sizeof(unsigned char)*48);

	time(&lastUpdateTime);

	while(1) {
		if(myself->bNeedUpdate == false) {
			usleep(10000);
			continue;
		}
		else if(memcmp(origDisplayRAM, myself->m_DisplayRAM, sizeof(unsigned char)*48) == 0) {
			myself->bNeedUpdate = false;
			usleep(10000);
		}
		else {
			pthread_mutex_lock(&(myself->mem_mutex));
			memcpy(DisplayRAM, myself->m_DisplayRAM, sizeof(unsigned char)*48);
			pthread_mutex_unlock(&(myself->mem_mutex));

			myself->bNeedUpdate = false;

			// this function might cost up to 5-600ms to complete
			unsigned int vfdo_value, wrctl_value;
			int i, j;
			// Total 48 bytes, separated to write in variant times, to optimize writing speed the most
			// done in 2006/09/12

			pthread_mutex_lock(&(myself->drv_mutex));
			myself->disableAutoRead();

			for(i = 0 ; i < 48 ; ) {
				int bytes_to_write = 1;
				int bytes_to_compare = 1;

				while(memcmp(&(origDisplayRAM[i]), &(DisplayRAM[i]), sizeof(unsigned char)*bytes_to_compare) == 0 && (i+bytes_to_compare < 48))
					bytes_to_compare++;

				bytes_to_compare--;

				if(bytes_to_compare > 0) {
					i += bytes_to_compare;
					continue;
				}

				while(memcmp(&(origDisplayRAM[i+bytes_to_write]), &(DisplayRAM[i+bytes_to_write]), sizeof(unsigned char)) != 0 && (i+bytes_to_write < 48))
					bytes_to_write++;

				// [Byte1 : Address setting command, set to 0x10 * i]
				// 0xc0 = 1100-0000
				// [Byte0 : Data setting command, normal operation, incremental, write to display memory]
				// 0x40 = 0100-0000

				vfdo_value = ((0xc0 | (unsigned char)i) << 8) | 0x40;
				wrctl_value = 0x0330;

				myself->writeAtOnce(vfdo_value, wrctl_value);

				for(j = 0 ; j < bytes_to_write ; j+=4) {
					if(bytes_to_write - j > 4) {
						/* (4i + 0) ~ (4i + 3) */
						vfdo_value = ((unsigned int)(DisplayRAM[i+j])) |
							((unsigned int)(DisplayRAM[i+j+1]) << 8) |
							((unsigned int)(DisplayRAM[i+j+2]) << 16) |
							((unsigned int)(DisplayRAM[i+j+3]) << 24);

						wrctl_value = 0xf0;
					}
					else {
						switch(bytes_to_write - j) {
							case 4:
								vfdo_value = ((unsigned int)(DisplayRAM[i+j])) |
									((unsigned int)(DisplayRAM[i+j+1]) << 8) |
									((unsigned int)(DisplayRAM[i+j+2]) << 16) |
									((unsigned int)(DisplayRAM[i+j+3]) << 24);

								wrctl_value = 0x10f0;
								break;
							case 3:
								vfdo_value = ((unsigned int)(DisplayRAM[i+j])) |
									((unsigned int)(DisplayRAM[i+j+1]) << 8) |
									((unsigned int)(DisplayRAM[i+j+2]) << 16);
								wrctl_value = 0x1070;
								break;
							case 2:
								vfdo_value = ((unsigned int)(DisplayRAM[i+j])) |
									((unsigned int)(DisplayRAM[i+j+1]) << 8);
								wrctl_value = 0x1030;
								break;
							case 1:
								vfdo_value = ((unsigned int)(DisplayRAM[i+j]));
								wrctl_value = 0x1010;
								break;
							default:
								assert(0);
						}
					}
					myself->writeAtOnce(vfdo_value, wrctl_value);
				}
				i += bytes_to_write;
			}
			// send command 1 & 4
			vfdo_value = (m_dimmingQuality << 8) | (m_displayMode);
			wrctl_value = 0x00001330;
			myself->writeAtOnce(vfdo_value, wrctl_value);
			memcpy(origDisplayRAM, DisplayRAM, sizeof(unsigned char)*48);
			myself->enableAutoRead();
			pthread_mutex_unlock(&(myself->drv_mutex));
		}
	}
}

void VFD_Controller::UpdateLED(void) {
	unsigned int vfdo_value, wrctl_value;

	// [Byte1 : data] = m_LED;
	// [Byte0 : Data setting command, normal operation, write data to LED port 
	// 0x41 = 0100-0001
	vfdo_value = 0x41 | ((unsigned int)(m_LED) << 8);

	// 0x1130 = 1-0001-0011-0000
	wrctl_value = 0x1130;
	pthread_mutex_lock(&drv_mutex);
	disableAutoRead();
	writeAtOnce(vfdo_value, wrctl_value);
	enableAutoRead();
	pthread_mutex_unlock(&drv_mutex);
}

void VFD_Controller::LEDOff(char ledNum) {
	m_LED = m_LED | (0x1<<ledNum);
}

void VFD_Controller::LEDOn(char ledNum) {
	m_LED = m_LED & (~(0x1<<ledNum));
}

void VFD_Controller::NodeOn(unsigned char digit, unsigned char segment) {
	unsigned int vfdo_value, wrctl_value;
	unsigned char addr = (digit-1) * 3 + (segment-1) / 8;

	if(m_DisplayRAM[addr] == (m_DisplayRAM[addr] | ((0x1) << ((segment-1) % 8))))
		return;

	pthread_mutex_lock(&mem_mutex);
	m_DisplayRAM[addr] = m_DisplayRAM[addr] | ((0x1) << ((segment-1) % 8));
	pthread_mutex_unlock(&mem_mutex);

	// [Byte2 : data]
	// [Byte1 : Address setting command, set to 0x00]
	// addr
	// [Byte0 : Data setting command, normal operation, fixed address, write to display memory]
	// 0x40 = 0100-0100
	vfdo_value = (0x44) | 
			(((unsigned int)(0xc0 | addr)) << 8) |
			(((unsigned int)(m_DisplayRAM[addr])) << 16);

	// 0x1130 = 1-0011-0111-0000
	wrctl_value = 0x1370;
	pthread_mutex_lock(&drv_mutex);
	disableAutoRead();
	writeAtOnce(vfdo_value, wrctl_value);
	enableAutoRead();
	pthread_mutex_unlock(&drv_mutex);
}

void VFD_Controller::NodeOff(unsigned char digit, unsigned char segment) {
	unsigned int vfdo_value, wrctl_value;
	unsigned char addr = (digit-1) * 3 + (segment-1) / 8;

	if(m_DisplayRAM[addr] == (m_DisplayRAM[addr] & (~((0x1) << ((segment-1) % 8)))))
		return;

	pthread_mutex_lock(&mem_mutex);
	m_DisplayRAM[addr] = m_DisplayRAM[addr] & (~((0x1) << ((segment-1) % 8)));
	pthread_mutex_unlock(&mem_mutex);

	// [Byte2 : data]
	// [Byte1 : Address setting command, set to 0x00]
	// addr
	// [Byte0 : Data setting command, normal operation, fixed address, write to display memory]
	// 0x40 = 0100-0100
	vfdo_value = (0x44) | 
			(((unsigned int)(0xc0 | addr)) << 8) |
			(((unsigned int)(m_DisplayRAM[addr])) << 16);

	// 0x1130 = 1-0011-0111-0000
	wrctl_value = 0x1370;
	pthread_mutex_lock(&drv_mutex);
	disableAutoRead();
	writeAtOnce(vfdo_value, wrctl_value);
	enableAutoRead();
	pthread_mutex_unlock(&drv_mutex);
}

void VFD_Controller::sNodeOn(unsigned char digit, unsigned char segment) {
	unsigned char addr = (digit-1) * 3 + (segment-1) / 8;

	if(m_DisplayRAM[addr] == (m_DisplayRAM[addr] | ((0x1) << ((segment-1) % 8))))
		return;

	pthread_mutex_lock(&mem_mutex);
	m_DisplayRAM[addr] = m_DisplayRAM[addr] | ((0x1) << ((segment-1) % 8));
	pthread_mutex_unlock(&mem_mutex);
}

void VFD_Controller::sNodeOff(unsigned char digit, unsigned char segment) {
	unsigned char addr = (digit-1) * 3 + (segment-1) / 8;

	if(m_DisplayRAM[addr] == (m_DisplayRAM[addr] & (~((0x1) << ((segment-1) % 8)))))
		return;

	pthread_mutex_lock(&mem_mutex);
	m_DisplayRAM[addr] = m_DisplayRAM[addr] & (~((0x1) << ((segment-1) % 8)));
	pthread_mutex_unlock(&mem_mutex);
}

// private member functions
void VFD_Controller::writeAtOnce(unsigned int valueVFDO, unsigned int valueWRCTL) {
	unsigned int wrctl_value;

	if(fd_vfdo == -1 || fd_wrctl == -1)
		return;

	write(fd_vfdo, &valueVFDO, 4);
	write(fd_wrctl, &valueWRCTL, 4);

	do { // wait for WriteDone signal
		read(fd_wrctl, &wrctl_value, 4);
	} while((wrctl_value & 0x02) == 0x00);
}

void VFD_Controller::setDimmingQuality(unsigned char pulseWidth) {
	switch(pulseWidth) {
		case 0:
			m_dimmingQuality = 0x80;
			break;
		case 1:
			m_dimmingQuality = 0x88;
			break;
		case 2:
			m_dimmingQuality = 0x89;
			break;
		case 4:
			m_dimmingQuality = 0x8a;
			break;
		case 10:
			m_dimmingQuality = 0x8b;
			break;
		case 11:
			m_dimmingQuality = 0x8c;
			break;
		case 12:
			m_dimmingQuality = 0x8d;
			break;
		case 13:
			m_dimmingQuality = 0x8e;
			break;
		default:
			fprintf(stderr, "VFD: pulse width %d not supported, setting to default value [14]\n", pulseWidth);
		case 14:
			m_dimmingQuality = 0x8f;
			break;
	}
}

void VFD_Controller::disableAutoRead(void) {
	if(fd_wrctl == -1)
		return;

	if(ioctl(fd_wrctl, VENUS_VFD_IOC_DISABLE_AUTOREAD) < 0)
		fprintf(stderr, "Warning: Could not disable auto read for VFD\n");
	else
		usleep(PERIOD_WAIT_AUTO_READ); // workaround - sleep 100us to wait auto read completion
}


void VFD_Controller::enableAutoRead(void) {
	if(fd_wrctl == -1)
		return;

	if(ioctl(fd_wrctl, VENUS_VFD_IOC_ENABLE_AUTOREAD) < 0)
		fprintf(stderr, "Warning: Could not enable auto read for VFD\n");
}
