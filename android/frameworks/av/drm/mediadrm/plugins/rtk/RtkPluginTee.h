#ifndef RTK_PLUGIN_H
#define RTK_PLUGIN_H

/* This UUID is generated with uuidgen
   the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html */
#define UUID { 0x35284d83, 0x340d, 0x416b, \
                { 0xb7, 0x85, 0x85, 0xd0, 0x2e, 0x1d, 0x52, 0x16} }

#define TA_SUCCESS 1
#define TA_FAILED  0

enum CMD_FOR_TA
{
	TA_TEE_RTK_PLUGIN_ENCRYPT			= 0,
	TA_TEE_RTK_PLUGIN_DECRYPT			= 1,
};


int Rtk_Plugin_Initialize(void);
void Rtk_Plugin_Finalize(void);
int Rtk_Plugin_Opensession(void);
void Rtk_Plugin_Closesession(void);

void Rtk_DrmFactory_Initialize(void);
void Rtk_DrmFactory_Finalize(void);

int Rtk_plugin_Encrypt(unsigned char *src, unsigned char *dst, unsigned int data_length);

int Rtk_plugin_Decrypt(unsigned char *src, unsigned char *dst, unsigned int data_length);

#endif /*RTK_PLUGIN_H*/