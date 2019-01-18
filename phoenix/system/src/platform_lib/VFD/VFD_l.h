/*
	VFD	Low-Level Library := VFD Controller

	for TOPPRO TP6311
*/

#ifndef __VFD_LOW_LEVEL_H__
#define __VFD_LOW_LEVEL_H__
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <StreamClass/EType.h>
#define VENUS_VFDO			"/dev/venus_vfdo"
#define VENUS_VFD_WRCTL		"/dev/venus_vfd_wrctl"
#define VENUS_VFD_KEYPAD	"/dev/venus_vfd_keypad"
#define TP6311_DIG_NUM 16

#define VENUS_VFD_IOC_MAGIC			'r'
#define VENUS_VFD_IOC_DISABLE_AUTOREAD	_IO(VENUS_VFD_IOC_MAGIC, 1)
#define VENUS_VFD_IOC_ENABLE_AUTOREAD	_IO(VENUS_VFD_IOC_MAGIC, 2)
#define VENUS_VFD_IOC_MAXNR			8

class VFD_Controller {
public:
	VFD_Controller(int digit, unsigned char pulseWidth = 14);
	~VFD_Controller();

	void InitDisplayRAM(void);
	void UpdateDisplayRAM(void);

	void UpdateLED(void);
	void LEDOn(char ledNum);
	void LEDOff(char ledNum);

	void NodeOn(unsigned char digit, unsigned char segment);	// 1-based
	void NodeOff(unsigned char digit, unsigned char segment);	// 1-based

	// modify DisplayRAM only, without real spi transaction (silent)
	void sNodeOn(unsigned char digit, unsigned char segment);	// 1-based
	void sNodeOff(unsigned char digit, unsigned char segment);	// 1-based

	static void setDimmingQuality(unsigned char pulseWidth);

	pthread_mutex_t mem_mutex;
	unsigned char m_DisplayRAM[48];
	inline bool IsUpdateComplete(void) {
		if(bNeedUpdate == false)
			return true;
		else
			return false;
	}

	// AutoRead control
	void disableAutoRead(void);
	void enableAutoRead(void);

private:
	unsigned char m_LED;
	
	int fd_wrctl;
	int fd_vfdo;

	pthread_t id_updater;
	pthread_mutex_t drv_mutex;

	bool bNeedUpdate;

	static unsigned int m_displayMode;
	static unsigned int m_dimmingQuality;

private:
	static void * DisplayUpdater(void *pInstance);
	void writeAtOnce(unsigned int valueVFDO, unsigned int valueWRCTL);
};

#endif

