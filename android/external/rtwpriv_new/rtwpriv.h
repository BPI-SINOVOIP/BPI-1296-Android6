#ifndef __FEATURE
#define __FEATURE

#include <math.h>
#include <string.h>

#define RTWPRIVE_VERSION "V3.1_20170620"

typedef unsigned char UCHAR;
typedef unsigned char BOOL,*PBOOL;
typedef unsigned short USHORT;
typedef unsigned int	UINT;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define TRUE	1
#define FALSE	0

#define DbgPrint	printf
#define PRINT_DATA(_TitleString, _HexData, _HexDataLen)	\
do {	\
	char *szTitle = _TitleString;	\
	u8 *pbtHexData = _HexData;	\
	u32 u4bHexDataLen = _HexDataLen;	\
	u32 __i;	\
	printf("%s", szTitle);	\
	for (__i = 0 ;__i < u4bHexDataLen; __i++)	\
	{	\
		if ((__i & 15) == 0)	\
		{	\
			printf("\n");	\
		}	\
		printf("%02X%s", pbtHexData[__i], ( ((__i&3)==3) ? "  " : " ") ); \
	}	\
	printf("\n");	\
} while (0);


typedef enum _ANTENNA_PATH{
    ANTENNA_NONE 	= 0,
	ANTENNA_D		= 1,
	ANTENNA_C		= 2,
	ANTENNA_CD	= 3,
	ANTENNA_B		= 4,
	ANTENNA_BD	= 5,
	ANTENNA_BC	= 6,
	ANTENNA_BCD	= 7,
	ANTENNA_A		= 8,
	ANTENNA_AD	= 9,
	ANTENNA_AC	= 10,
	ANTENNA_ACD	= 11,
	ANTENNA_AB	= 12,
	ANTENNA_ABD	= 13,
	ANTENNA_ABC	= 14,
	ANTENNA_ABCD	= 15
} ANTENNA_PATH;


typedef enum _TEST_MODE{
	TEST_NONE                 ,
	PACKETS_TX                ,
	PACKETS_RX                ,
	CONTINUOUS_TX             , 
	OFDM_Single_Tone_TX       ,
	CCK_Carrier_Suppression_TX
}TEST_MODE;

typedef enum _MPT_RATE{
	/* CCK rate. Total: 4*/
	RATE_CCK_1M = 1, 
	RATE_CCK_2M,
	RATE_CCK_55M,
	RATE_CCK_11M,
	
	/* OFDM rate. Total: 8*/
	RATE_OFDM_6M,       //#5
	RATE_OFDM_9M,
	RATE_OFDM_12M,
	RATE_OFDM_18M,
	RATE_OFDM_24M,
	RATE_OFDM_36M,      //#10
	RATE_OFDM_48M,
	RATE_OFDM_54M,
	
	/* HT rate.  Total: 16*/
	RATE_MCS0,
	RATE_MCS1,
	RATE_MCS2,          //#15
	RATE_MCS3,
	RATE_MCS4,
	RATE_MCS5,
	RATE_MCS6,
	RATE_MCS7,          //#20
	RATE_MCS8,
	RATE_MCS9,
	RATE_MCS10,
	RATE_MCS11,
	RATE_MCS12,         //#25
	RATE_MCS13,
	RATE_MCS14,
	RATE_MCS15,
	RATE_MCS16,
	RATE_MCS17,         //#30
	RATE_MCS18,
	RATE_MCS19,
	RATE_MCS20,
	RATE_MCS21,
	RATE_MCS22,         //#35
	RATE_MCS23,
	RATE_MCS24,
	RATE_MCS25,
	RATE_MCS26,
	RATE_MCS27,         //#40
	RATE_MCS28,
	RATE_MCS29,
	RATE_MCS30,
	RATE_MCS31,

	
	/* VHT rate. Total: 20*/
	RATE_VHT1SS_MCS0 = 100, // To reserve MCS16~MCS31, the index starts from #100.
	RATE_VHT1SS_MCS1,   //#101
	RATE_VHT1SS_MCS2,
	RATE_VHT1SS_MCS3,
	RATE_VHT1SS_MCS4,
	RATE_VHT1SS_MCS5,
	RATE_VHT1SS_MCS6,   //#106
	RATE_VHT1SS_MCS7,
	RATE_VHT1SS_MCS8,
	RATE_VHT1SS_MCS9,
	RATE_VHT2SS_MCS0,
	RATE_VHT2SS_MCS1,   //#111
	RATE_VHT2SS_MCS2,
	RATE_VHT2SS_MCS3,
	RATE_VHT2SS_MCS4,
	RATE_VHT2SS_MCS5,
	RATE_VHT2SS_MCS6,   //#116
	RATE_VHT2SS_MCS7,
	RATE_VHT2SS_MCS8,
	RATE_VHT2SS_MCS9,
	RATE_VHT3SS_MCS0,
	RATE_VHT3SS_MCS1,	// #121
	RATE_VHT3SS_MCS2,
	RATE_VHT3SS_MCS3,
	RATE_VHT3SS_MCS4,
	RATE_VHT3SS_MCS5,
	RATE_VHT3SS_MCS6,	// #126
	RATE_VHT3SS_MCS7,
	RATE_VHT3SS_MCS8,
	RATE_VHT3SS_MCS9,
	RATE_VHT4SS_MCS0,
	RATE_VHT4SS_MCS1,	// #131
	RATE_VHT4SS_MCS2,
	RATE_VHT4SS_MCS3,
	RATE_VHT4SS_MCS4,
	RATE_VHT4SS_MCS5,
	RATE_VHT4SS_MCS6,	// #136
	RATE_VHT4SS_MCS7,
	RATE_VHT4SS_MCS8,
	RATE_VHT4SS_MCS9,
	RATE_LAST
	 
}MPT_RATE_E, *PMPT_RATE_E;


#define IS_CCK_RATE(_value)     (RATE_CCK_1M <= _value && _value <= RATE_CCK_11M)
#define IS_OFDM_RATE(_value)    (RATE_OFDM_6M <= _value && _value <= RATE_OFDM_54M)
#define IS_HT_RATE(_value)     (RATE_MCS0 <= _value && _value <= RATE_MCS31)
#define IS_HT_1S_RATE(_value)  (RATE_MCS0 <= _value && _value <= RATE_MCS7)
#define IS_HT_2S_RATE(_value)  (RATE_MCS8 <= _value && _value <= RATE_MCS15)
#define IS_HT_3S_RATE(_value)  (RATE_MCS16 <= _value && _value <= RATE_MCS23)
#define IS_HT_4S_RATE(_value)  (RATE_MCS24 <= _value && _value <= RATE_MCS31)

#define IS_VHT_RATE(_value)     (RATE_VHT1SS_MCS0 <= _value && _value <= RATE_VHT4SS_MCS9)
#define IS_VHT_1S_RATE(_value)  (RATE_VHT1SS_MCS0 <= _value && _value <= RATE_VHT1SS_MCS9)
#define IS_VHT_2S_RATE(_value)  (RATE_VHT2SS_MCS0 <= _value && _value <= RATE_VHT2SS_MCS9)
#define IS_VHT_3S_RATE(_value)  (RATE_VHT3SS_MCS0 <= _value && _value <= RATE_VHT3SS_MCS9)
#define IS_VHT_4S_RATE(_value)  (RATE_VHT4SS_MCS0 <= _value && _value <= RATE_VHT4SS_MCS9)

#define IS_2SS_RATE(_rate) ((RATE_MCS8 <= _rate && _rate <= RATE_MCS15) ||\
							(RATE_VHT2SS_MCS0<= _rate && _rate <= RATE_VHT2SS_MCS9))
#define IS_3SS_RATE(_rate) ((RATE_MCS16 <= _rate && _rate <= RATE_MCS23) ||\
							(RATE_VHT3SS_MCS0<= _rate && _rate <= RATE_VHT3SS_MCS9))
#define IS_4SS_RATE(_rate) ((RATE_MCS24 <= _rate && _rate <= RATE_MCS31) ||\
							(RATE_VHT4SS_MCS0<= _rate && _rate <= RATE_VHT4SS_MCS9))

// Add Declarations here.
typedef struct _RT_PMAC_TX_INFO {
	UCHAR			bEnPMacTx:1;		// 0: Disable PMac 1: Enable PMac
	UCHAR			Mode:3;				// 0: Packet TX 3:Continuous TX
	UCHAR			Ntx:4;				// 0-7
	UCHAR			TX_RATE;			// MPT_RATE_E
	UCHAR			TX_RATE_HEX;
	UCHAR			TX_SC;
	UCHAR			bSGI:1;
	UCHAR			bSPreamble:1;
	UCHAR			bSTBC:1;
	UCHAR			bLDPC:1;
	UCHAR			NDP_sound:1;
	UCHAR			BandWidth:3;		// 0: 20 1:40 2:80Mhz
	UCHAR			m_STBC;				// bSTBC + 1
	USHORT			PacketPeriod;
	UINT			PacketCount;
	UINT			PacketLength;
	UCHAR			PacketPattern;
	USHORT			SFD;
	UCHAR			SignalField;
	UCHAR			ServiceField;
	USHORT			LENGTH;
	UCHAR			CRC16[2];
	UCHAR			LSIG[3];
	UCHAR			HT_SIG[6];
	UCHAR			VHT_SIG_A[6];
	UCHAR			VHT_SIG_B[4];
	UCHAR			VHT_SIG_B_CRC;
	UCHAR			VHT_Delimiter[4];
	UCHAR			MacAddress[6];
} RT_PMAC_TX_INFO, *PRT_PMAC_TX_INFO;


typedef struct _RT_PMAC_PKT_INFO {
	UCHAR			MCS;
	UCHAR			Nss;
	UCHAR			Nsts;
	UINT			N_sym;
	UCHAR			SIGA2B3;
} RT_PMAC_PKT_INFO, *PRT_PMAC_PKT_INFO;


void PMAC_Notify(
	PRT_PMAC_TX_INFO	pPMacTxInfo
	);

#endif
