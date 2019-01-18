/*
   For FUTABA 11-BT-236GNK
*/
#include <Platform_Lib/FrontControl/VFD_h.h>
#include <Platform_Lib/FrontControl/VFD_l.h>
#include <Util.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "vfd_char.h"

// FIXME
char * VenusVFD::STATUS_message;
char * VenusVFD::FORMAT_message;
char * VenusVFD::MEDIA_message;
char * VenusVFD::TV_message;
unsigned char VenusVFD::LED_BITS = 0;

/*
   Local Utility Functions
*/

static void modifyDisplayRAM(unsigned char *base, unsigned char offset, char letter) {
	int i;
	int tableIndex;

	if(letter>= '0' && letter <= '9')
		tableIndex = letter - '0';
	else if(letter >= 'A' && letter <= 'Z')
		tableIndex = letter - 'A' + 10;
	else if(letter >= 'a' && letter <= 'z')
		tableIndex = letter - 'a' + 10;
	else if(letter == ' ')
		tableIndex = 36;
	else if(letter == '-')
		tableIndex = 37;
	else
		tableIndex = 38;

	for(i=2;i<=8;i++) {
		if(letterTable[tableIndex][15-i] == 1)
			base[offset] |= (0x1 << (i-1));
		else
			base[offset] &= (~(0x1 << (i-1)));
	}

	for(i=9;i<=15;i++) {
		if(letterTable[tableIndex][15-i] == 1)
			base[offset+1] |= (0x1 << (i-9));
		else
			base[offset+1] &= (~(0x1 << (i-9)));
	}
}


/* VenusVFD */

VenusVFD* VenusVFD::ptr = NULL;
VFD_Controller* VenusVFD::controller = NULL;

VenusVFD::VenusVFD() {
	controller = new VFD_Controller(11);	// 11x17 
}

VenusVFD::~VenusVFD() {
	delete controller;
}

bool VenusVFD::LED(unsigned char bits) {
	if(ptr == NULL)
		ptr = new VenusVFD;

	for(int i=0;i<5;i++)
		if((bits >> i) & 0x1)
			controller->LEDOn(i);
		else
			controller->LEDOff(i);

	controller->UpdateLED();

	return true;
}

bool VenusVFD::AUDIO_FORMAT(NAVAUDIOATTR attr) {
	if(ptr == NULL)
		ptr = new VenusVFD;
	
	controller->sNodeOff(11, 1);
	controller->sNodeOff(11, 2);
	controller->sNodeOff(11, 9);

	switch(attr.type) {
		case MEDIASUBTYPE_PCM:
		case MEDIASUBTYPE_DVD_LPCM_AUDIO:
			break;
		case MEDIASUBTYPE_MPEG1Packet:
		case MEDIASUBTYPE_MPEG1ES:
		case MEDIASUBTYPE_MPEG2_AUDIO:
		case MEDIASUBTYPE_MPEG_AUDIO:
			//controller->sNodeOn(11, 9);
			break;
		case MEDIASUBTYPE_DOLBY_AC3:
		case MEDIASUBTYPE_DOLBY_AC3_SPDIF:
			controller->sNodeOn(11, 1);
			break;
		case MEDIASUBTYPE_DTS:
			controller->sNodeOn(11, 2);
			break;
		default:
			break;
	}

	controller->sNodeOff(1, 6);
	controller->sNodeOff(1, 7);
	controller->sNodeOff(1, 8);
	controller->sNodeOff(1, 9);
	controller->sNodeOff(1, 10);
	controller->sNodeOff(1, 11);
	controller->sNodeOff(1, 12);

	// showing channels 
	if(attr.numChannels == 2) {
		controller->sNodeOn(1, 6);
		controller->sNodeOn(1, 8);
	}
	else if(attr.numChannels == 3) {
		controller->sNodeOn(1, 6);
		controller->sNodeOn(1, 7);
		controller->sNodeOn(1, 8);
	}
	else if(attr.numChannels == 4) {
		controller->sNodeOn(1, 6);
		controller->sNodeOn(1, 7);
		controller->sNodeOn(1, 8);
		controller->sNodeOn(1, 11);
	}
	else if(attr.numChannels == 5) {
		controller->sNodeOn(1, 6);
		controller->sNodeOn(1, 7);
		controller->sNodeOn(1, 8);
		controller->sNodeOn(1, 10);
		controller->sNodeOn(1, 12);
	}
	else if(attr.numChannels == 6) {
		controller->sNodeOn(1, 6);
		controller->sNodeOn(1, 7);
		controller->sNodeOn(1, 8);
		controller->sNodeOn(1, 9);
		controller->sNodeOn(1, 10);
		controller->sNodeOn(1, 12);
	}
	controller->UpdateDisplayRAM();

	return true;
}

bool VenusVFD::MEDIA_TYPE(DATA_MEDIA_TYPE type, PLAYING_SOURCE source) {
	if(ptr == NULL)
		ptr = new VenusVFD;
	
	controller->sNodeOff(11, 6);
	controller->sNodeOff(11, 7);
	controller->sNodeOff(11, 8);

	switch(type.subtype) {
   		case MEDIASUBTYPE_DVD_RTRW_PLUS:	// DVD +RW 
			if(source == SOURCE_DVD)
				controller->sNodeOn(11, 6);
			break;
		case MEDIASUBTYPE_DVD_RTRW_MINUS:	// DVD -VR
		case MEDIASUBTYPE_DVD_RTRW_STILL:
			if(source == SOURCE_DVD)
				controller->sNodeOn(11, 6);
			break;
		case MEDIASUBTYPE_DVD_VIDEO:		// DVD -VIDEO
		case MEDIASUBTYPE_DVD_VIDEO_ROM:
			if(source == SOURCE_DVD)
				controller->sNodeOn(11, 6);
			break;
		case MEDIASUBTYPE_VCD_ROM:
			if(source == SOURCE_DVD) {
				controller->sNodeOn(11, 7);
				controller->sNodeOn(11, 8);
			}
			break;
		case MEDIASUBTYPE_SVCD_ROM:
			if(source == SOURCE_DVD) {
				controller->sNodeOn(11, 7);
				controller->sNodeOn(11, 8);
			}
			break;
		case MEDIASUBTYPE_CDDA_ROM:			// CDDA
			if(source == SOURCE_DVD)
				controller->sNodeOn(11, 8);
			break;
		case MEDIASUBTYPE_HTTP:
			break;
		case MEDIATYPE_FILE:
			break;
		default:
			break;
	}
	controller->UpdateDisplayRAM();
	return true;
}

bool VenusVFD::show_msg(char *msg) {
	int length, i;
	unsigned char startAddr;

	if(ptr == NULL)
		ptr = new VenusVFD;

	length = strlen(msg);
	if(length > 9)
		length = 9;

	for(i=0;i<length;i++) {
		startAddr = (9-i)*3;
		pthread_mutex_lock(&(controller->mem_mutex));
		modifyDisplayRAM(controller->m_DisplayRAM, startAddr, msg[i]);
		pthread_mutex_unlock(&(controller->mem_mutex));
	}
	for(;i<9;i++) {
		startAddr = (9-i)*3;
		pthread_mutex_lock(&(controller->mem_mutex));
		modifyDisplayRAM(controller->m_DisplayRAM, startAddr, ' ');
		pthread_mutex_unlock(&(controller->mem_mutex));
	}

	controller->UpdateDisplayRAM();
	return true;
}

bool VenusVFD::PlaybackStatus(NAVPLAYBACKSTATUS pbStatus) {
	char message[10];

	controller->sNodeOff(11, 10);
	controller->sNodeOff(11, 11);
	controller->sNodeOff(6, 16);
	controller->sNodeOff(4, 16);
   	controller->sNodeOff(10, 1);
   	controller->sNodeOff(8, 1);
   	controller->sNodeOff(9, 1);
	if(ptr == NULL)
		ptr = new VenusVFD;

	if(pbStatus.domain == NAV_DOMAIN_STOP) {
		show_msg("  STOP   ");
		return true;
	}
	else if(pbStatus.domain == NAV_DOMAIN_MENU 
					&& pbStatus.mediaType != MEDIATYPE_VCD 
					&& pbStatus.mediaType != MEDIATYPE_SVCD) {
		show_msg("  MENU   ");
		return true;
	}

	// other domains are all "playing" status
	if(pbStatus.bPaused == false)
		controller->sNodeOn(11, 10);
	else // pause
		controller->sNodeOn(11, 11);
	controller->sNodeOn(6, 16);
	controller->sNodeOn(4, 16);

	switch(pbStatus.mediaType){
		case MEDIATYPE_SVCD:
		case MEDIATYPE_VCD:
		case MEDIATYPE_CDDA:
		case MEDIASUBTYPE_SVCD:
		case MEDIASUBTYPE_SVCD_ROM:
		case MEDIASUBTYPE_VCD:
		case MEDIASUBTYPE_VCD_ROM:
		case MEDIASUBTYPE_CDDA:
		case MEDIASUBTYPE_CDDA_ROM:
			controller->sNodeOn(9, 1);
				snprintf(message, 
				MAX_STRING, 
				"%02d  %d%02d%02d", 
				pbStatus.currentChapter % 100, 
				((pbStatus.elapsedTime.seconds)/60/60) % 10,
				(pbStatus.elapsedTime.seconds)/60%60,
				(pbStatus.elapsedTime.seconds)%60 );
			break;
		case MEDIATYPE_DVD:
		case MEDIASUBTYPE_DVD_RTRW_PLUS:
		case MEDIASUBTYPE_DVD_RTRW_MINUS:
		case MEDIASUBTYPE_DVD_RTRW_STILL:
		case MEDIASUBTYPE_DVD_VIDEO:
		case MEDIASUBTYPE_DVD_VIDEO_ROM:
			controller->sNodeOn(10, 1);
			controller->sNodeOn(8, 1);
        	snprintf(message, 
				MAX_STRING, 
				"%02d%02d%d%02d%02d", 
				pbStatus.currentTitle % 100, 
				pbStatus.currentChapter % 100, 
				((pbStatus.elapsedTime.seconds)/60/60) % 10,
				(pbStatus.elapsedTime.seconds)/60%60,
				(pbStatus.elapsedTime.seconds)%60);
			break;
        default:
			snprintf(message,
				MAX_STRING,
				"    %d%02d%02d",
				((pbStatus.elapsedTime.seconds)/60/60) % 10,
				(pbStatus.elapsedTime.seconds)/60%60,
				(pbStatus.elapsedTime.seconds)%60 );
			break;
	}
        
	show_msg(message);
	return true;
}

bool VenusVFD::RecordStatus(int hour, int minute, int second, int frame, FILTER_STATE state) {
	char message[10];
	if(ptr == NULL)
		ptr = new VenusVFD;

	controller->sNodeOn(11, 12);
	controller->sNodeOn(6, 16);
	controller->sNodeOn(4, 16);

	if(state == State_Running) {
		controller->sNodeOff(11, 11);
	}
	else if(state == State_Paused) {
		controller->sNodeOn(11, 11);
	}
	else {
		controller->sNodeOff(11, 11);
	}

	if(hour < 10) // 0-9
		snprintf(message, MAX_STRING, "    %d%02d%02d", hour, minute, second);
	else if (hour < 100) // 10 - 99
		snprintf(message, MAX_STRING, "   %02d%02d%02d", hour, minute, second);
	else if (hour < 1000) // 100 - 999
		snprintf(message, MAX_STRING, "  %03d%02d%02d", hour, minute, second);

	show_msg(message);
	return true;
}

bool VenusVFD::showTime(time_t timestamp) {
	struct tm TM;
	char message[10];

	if(ptr == NULL)
		ptr = new VenusVFD;

	controller->sNodeOn(6, 16);

	localtime_r(&timestamp, &TM);
	sprintf(message, "   %02d%02d  ", TM.tm_hour, TM.tm_min);

	show_msg(message);
	return true;
}

bool VenusVFD::RepeatMode(NAV_REPEAT_MODE mode) {
	controller->sNodeOff(3, 1);
	controller->sNodeOff(2, 1);
	controller->sNodeOff(2, 16);
	controller->sNodeOff(1, 1);
	controller->sNodeOff(1, 2);

	switch(mode) {
		case NAV_REPEAT_NONE:
		case NAV_REPEAT_TITLE_ONCE:
			break;
		case NAV_REPEAT_TITLE:
			controller->sNodeOn(3, 1);
			controller->sNodeOn(2, 16);
			break;
		case NAV_REPEAT_CHAPTER:
			controller->sNodeOn(3, 1);
			controller->sNodeOn(2, 1);
			break;
		case NAV_REPEAT_AB:
			controller->sNodeOn(3, 1);
			controller->sNodeOn(1, 1);
			controller->sNodeOn(1, 2);
			break;
		default:
			break;
	}
	controller->UpdateDisplayRAM();
	return true;
}

void VenusVFD::clearVFD(void) {
	if(ptr == NULL)
		ptr = new VenusVFD;

	controller->InitDisplayRAM();
}

void VenusVFD::sClearVFD(void) {
	if(ptr == NULL)
		ptr = new VenusVFD;
	
	pthread_mutex_lock(&(controller->mem_mutex));

	for(int i = 0 ; i < (int)sizeof(controller->m_DisplayRAM) ; i++)
		controller->m_DisplayRAM[i] = 0;

	pthread_mutex_unlock(&(controller->mem_mutex));
}

void VenusVFD::deleteInstance(void) {
	if(ptr) {
		delete ptr;
		ptr = NULL;
	}
}
bool VenusVFD::DVPlayStatus(int hour, int minute, int second, int frame) {
	char message[10];
	if(ptr == NULL)
		ptr = new VenusVFD;

	//controller->sNodeOn(11, 12);
	controller->sNodeOn(6, 16);
	controller->sNodeOn(4, 16);

	controller->sNodeOn(11, 10);
	controller->sNodeOff(11, 11);

	if(hour < 10) // 0-9
		snprintf(message, MAX_STRING, "    %d%02d%02d", hour, minute, second);
	else if (hour < 100) // 10 - 99
		snprintf(message, MAX_STRING, "   %02d%02d%02d", hour, minute, second);
	else if (hour < 1000) // 100 - 999
		snprintf(message, MAX_STRING, "  %03d%02d%02d", hour, minute, second);

	show_msg(message);
	return true;
}

bool VenusVFD::playSign(bool playing)
{
    if(ptr == NULL)
        ptr = new VenusVFD;
    
    if(playing)
    {
        controller->sNodeOn(11, 10);
        controller->sNodeOff(11, 11);
    }
    else // pause
    {
        controller->sNodeOff(11, 10);
        controller->sNodeOn(11, 11);
    }
    controller->UpdateDisplayRAM();    
    return true;    
}

bool VenusVFD::PlaybackTime(int chapter, int second)
{
	char message[10];
	
	if(ptr == NULL)
		ptr = new VenusVFD;
		
    snprintf(message, MAX_STRING, "%02d  %d%02d%02d", 
    chapter%100, (second/60/60) % 10, second/60%60, second%60);
				
	show_msg(message);		
    return true;    
}
