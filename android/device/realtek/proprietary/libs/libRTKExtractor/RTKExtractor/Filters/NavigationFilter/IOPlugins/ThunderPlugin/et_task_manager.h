#ifndef ET_TASK_MANAGER_H_200909081912
#define ET_TASK_MANAGER_H_200909081912
/*-------------------------------------------------------*/
/*                       IDENTIFICATION                             	*/
/*-------------------------------------------------------*/
/*     Filename  : et_task_manager.h                                	*/
/*     Author    : Zeng Yuqing                                      	*/
/*     Project   : EmbedThunderManager                              	*/
/*     Version   : 1.5 								    	*/
/*-------------------------------------------------------*/
/*                  Shenzhen XunLei Networks			          	*/
/*-------------------------------------------------------*/
/*                  - C (copyright) - www.xunlei.com -		    	*/
/*                                                                          		      	*/
/*      This document is the proprietary of XunLei                  	*/
/*                                                                          			*/
/*      All rights reserved. Integral or partial reproduction         */
/*      prohibited without a written authorization from the           */
/*      permanent of the author rights.                               */
/*                                                                          			*/
/*-------------------------------------------------------*/
/*-------------------------------------------------------*/
/*                  FUNCTIONAL DESCRIPTION                           */
/*-------------------------------------------------------*/
/* This file contains the interfaces of EmbedThunderTaskManager       */
/*-------------------------------------------------------*/

/*-------------------------------------------------------*/
/*                       HISTORY                                     */
/*-------------------------------------------------------*/
/*   Date     |    Author   | Modification                            */
/*-------------------------------------------------------*/
/* 2009.09.08 |ZengYuqing  | Creation                                 */
/* 2010.11.11 |ZengYuqing  | Update to 1.5                                 */
/*-------------------------------------------------------*/

#ifdef __cplusplus
extern "C" 
{
#endif

/************************************************************************/
/*                    TYPE DEFINE                    */
/************************************************************************/
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef char				int8;
typedef short				int16;
typedef int					int32;
#if defined(LINUX) || defined( __SYMBIAN32__)
	typedef unsigned long long	uint64;
	typedef long long			int64;
#else
	#ifdef  NOT_SUPPORT_LARGE_INT_64
		typedef unsigned int	uint64;
		typedef int			__int64;
	#else
        #if defined(WINCE)
		typedef unsigned __int64 uint64;
		//typedef long long			__int64;
         #else
		typedef unsigned long	long uint64;
		typedef long long			__int64;
         #endif
	#endif
#endif

#if defined(LINUX) || defined( __SYMBIAN32__)
#ifndef NULL
#define NULL	((void*)(0))
#endif
#endif

#if defined( __SYMBIAN32__)
#include <e32def.h>
#ifdef ETDLL_API
#undef ETDLL_API
#endif
#define ETDLL_API IMPORT_C
#elif defined( WIN32)
  #ifdef WINCE
	#define ETDLL_API
  #else
	#ifdef ETDLL_EXPORTS
	#define ETDLL_API __declspec(dllexport)
	#else
	#define ETDLL_API __declspec(dllimport)
	#endif
  #endif
#else
	#define ETDLL_API
#endif


#ifndef TRUE
typedef int32 Bool;
typedef int32 BOOL;
#define TRUE	(1)
#define FALSE	(0)
#else
typedef int32 Bool;
#endif


/************************************************************************/
/*                    STRUCT DEFINE                 */
/************************************************************************/

#define ETM_MAX_FILE_NAME_LEN (256)		/* �ļ�����󳤶� */
#define ETM_MAX_FILE_PATH_LEN (256)		/* �ļ�·��(�������ļ���)��󳤶� */
#define ETM_MAX_TD_CFG_SUFFIX_LEN (8)		/* Ѹ��������ʱ�ļ��ĺ�׺��󳤶� */

/************************************************************************/

/* ��������*/
typedef enum t_etm_task_type
{
	ETT_URL = 0, 				/* ��URL ����������,֧��Э��: http://,https://,ftp://,thunder:// */
	ETT_BT, 					/* ��.torrent �ļ����������� */
	ETT_TCID,  					/* ��Ѹ�׵�TCID ����������  */
	ETT_KANKAN, 				/* ��Ѹ�׵�TCID,File_Size,��GCIDһ�𴴽������� ,ע�� �������͵����񲻻��Զ�ֹͣ����Ҫ����etm_stop_vod_task�Ż�ֹͣ*/
	ETT_EMULE, 					/* �õ�¿���Ӵ���������  								 */
	ETT_FILE 
} ETM_TASK_TYPE;

/* ����״̬ */
typedef enum t_etm_task_state
{
	ETS_TASK_WAITING=0, 
	ETS_TASK_RUNNING, 
	ETS_TASK_PAUSED,
	ETS_TASK_SUCCESS, 
	ETS_TASK_FAILED, 
	ETS_TASK_DELETED 
} ETM_TASK_STATE;

/* BT ������ַ��������� */
typedef enum t_etm_encoding_mode
{ 
	EEM_ENCODING_NONE = 0, 				/*  ����ԭʼ�ֶ�*/
	EEM_ENCODING_GBK = 1,					/*  ����GBK��ʽ���� */
	EEM_ENCODING_UTF8 = 2,				/*  ����UTF-8��ʽ���� */
	EEM_ENCODING_BIG5 = 3,				/*  ����BIG5��ʽ����  */
	EEM_ENCODING_UTF8_PROTO_MODE = 4,	/*  ���������ļ��е�utf-8�ֶ�  */
	EEM_ENCODING_DEFAULT = 5				/*  δ���������ʽ(ʹ��etm_set_default_encoding_mode��ȫ���������)  */
}ETM_ENCODING_MODE ;

/* ���������е������ʵʱ״�� */
typedef struct t_etm_running_status
{
	ETM_TASK_TYPE  _type;   
	ETM_TASK_STATE  _state;   
	uint64 _file_size; 					/* �����ļ���С*/  
	uint64 _downloaded_data_size; 	/* �����ص����ݴ�С*/  
	u32 _dl_speed;  					/* ʵʱ��������*/  
	u32 _ul_speed;  					/* ʵʱ�ϴ�����*/  
	u32 _downloading_pipe_num; 			/* �����������ݵ�socket ������ */
	u32 _connecting_pipe_num; 		/* ���������е�socket ������ */
}ETM_RUNNING_STATUS;

/* ������Ϣ */
typedef struct t_etm_task_info
{
	u32  _task_id;		
	ETM_TASK_STATE  _state;   
	ETM_TASK_TYPE  _type;   

	char _file_name[ETM_MAX_FILE_NAME_LEN];
	char _file_path[ETM_MAX_FILE_PATH_LEN]; 
	uint64 _file_size; 
	uint64 _downloaded_data_size; 		
	 
	u32 _start_time;				/* ����ʼ���ص�ʱ�䣬��1970��1��1�տ�ʼ������ */
	u32 _finished_time;			/* �������(�ɹ���ʧ��)��ʱ�䣬��1970��1��1�տ�ʼ������ */

	u32  _failed_code;			/* �������ʧ�ܵĻ�,�������ʧ���� */
	u32  _bt_total_file_num; 		/* ����������_type== ETT_BT, ������torrent�ļ��а��������ļ��ܸ���*/
	Bool _is_deleted;			/* �����񱻳ɹ�����etm_delete_task �ӿں�,����_state����ΪETS_TASK_DELETED�����Ǳ���ԭ����ֵ�Ա������Ա���ȷ��ԭ(etm_recover_task)�������ֵ��ʶ�����ʱ״̬ΪETS_TASK_DELETED */
	Bool _check_data;			/* �㲥ʱ�Ƿ���Ҫ��֤���� */	
	Bool _is_no_disk;			/* �Ƿ�Ϊ���̵㲥  */
}ETM_TASK_INFO;

/* BT �������ļ�������״̬ */
typedef enum t_etm_file_status
{
	EFS_IDLE = 0, 
	EFS_DOWNLOADING, 
	EFS_SUCCESS, 
	EFS_FAILED 
} ETM_FILE_STATUS;

/* BT �������ļ���������Ϣ */
typedef struct  t_etm_bt_file
{
	u32 _file_index;
	Bool _need_download;			/* �Ƿ���Ҫ���� */
	ETM_FILE_STATUS _status;
	uint64 _file_size;	
	uint64 _downloaded_data_size;
	u32 _failed_code;
}ETM_BT_FILE;

/* BT �������ļ�����Ϣ */
typedef struct t_etm_torrent_file_info
{
	u32 _file_index;
	char *_file_name;
	u32 _file_name_len;
	char *_file_path;
	u32 _file_path_len;
	uint64 _file_offset;		/* �����ļ�������BT �ļ��е�ƫ�� */
	uint64 _file_size;
} ETM_TORRENT_FILE_INFO;

/* torrent �ļ�����Ϣ */
typedef struct t_etm_torrent_seed_info
{
	char _title_name[ETM_MAX_FILE_NAME_LEN-ETM_MAX_TD_CFG_SUFFIX_LEN];
	u32 _title_name_len;
	uint64 _file_total_size;						/* �������ļ����ܴ�С*/
	u32 _file_num;								/* �������ļ����ܸ���*/
	u32 _encoding;								/* �����ļ������ʽ��GBK = 0, UTF_8 = 1, BIG5 = 2 */
	unsigned char _info_hash[20];					/* �����ļ�������ֵ */
	ETM_TORRENT_FILE_INFO *file_info_array_ptr;	/* ���ļ���Ϣ�б� */
} ETM_TORRENT_SEED_INFO;


/* E-Mule�����а������ļ���Ϣ */
typedef struct tagETM_ED2K_LINK_INFO
{
	char		_file_name[256];
	uint64		_file_size;
	u8		_file_id[16];
	u8		_root_hash[20];
	char		_http_url[512];
}ETM_ED2K_LINK_INFO;


/************************************************************************/

#define	ETM_INVALID_TASK_ID			0


/* ������������ʱ��������� */
typedef struct t_etm_create_task
{
	ETM_TASK_TYPE _type;				/* �������ͣ�����׼ȷ��д*/
	
	char* _file_path; 						/* �����ļ����·���������Ѵ����ҿ�д�����ΪNULL �����ʾ�����Ĭ��·����(��etm_set_download_path��������) */
	u32 _file_path_len;					/* �����ļ����·���ĳ��ȣ�����С�� ETM_MAX_FILE_PATH_LEN */
	char* _file_name;					/* �����ļ����� */
	u32 _file_name_len; 					/* �����ļ����ֵĳ��ȣ�����С�� ETM_MAX_FILE_NAME_LEN-ETM_MAX_TD_CFG_SUFFIX_LEN */
	
	char* _url;							/* _type== ETT_URL ����_type== ETT_EMULE ʱ��������������URL */
	u32 _url_len;						/* URL �ĳ��ȣ�����С��512 */
	char* _ref_url;						/* _type== ETT_URL ʱ������������������ҳ��URL ,��ΪNULL */
	u32 _ref_url_len;						/* ����ҳ��URL �ĳ��ȣ�����С��512 */
	
	char* _seed_file_full_path; 			/* _type== ETT_BT ʱ�������������������ļ�(*.torrent) ȫ·�� */
	u32 _seed_file_full_path_len; 		/* �����ļ�ȫ·���ĳ��ȣ�����С��ETM_MAX_FILE_PATH_LEN+ETM_MAX_FILE_NAME_LEN */
	u32* _download_file_index_array; 	/* _type== ETT_BT ʱ�����������Ҫ���ص����ļ�����б�ΪNULL ��ʾ����������Ч�����ļ�*/
	u32 _file_num;						/* _type== ETT_BT ʱ�����������Ҫ���ص����ļ�����*/
	
	char *_tcid;							/* _type== ETT_TCID ��_type== ETT_KANKAN  ʱ��������������ļ���Ѹ��TCID ,Ϊ40�ֽڵ�ʮ�����������ַ���*/
	uint64 _file_size;						/* _type== ETT_KANKAN  ʱ��������������ļ��Ĵ�С */
	char *_gcid;							/* _type== ETT_KANKAN  ʱ��������������ļ���Ѹ��GCID ,Ϊ40�ֽڵ�ʮ�����������ַ���*/
	
	Bool _check_data;					/* ������±߲�ʱ�Ƿ���ҪУ������  */
	Bool _manual_start;					/* �Ƿ��ֶ���ʼ����?  TRUE Ϊ�û��ֶ���ʼ����,FALSEΪETM�Զ���ʼ����*/
	
	void * _user_data;					/* ���ڴ�����������ص��û�������Ϣ��������������������cookie�� �������cookie��coockie���ֱ�����"Cookie:"��ͷ������"\r\n" ���� */
	u32  _user_data_len;					/* �û����ݵĳ���,����С��65535 */
} ETM_CREATE_TASK;

/* �����㲥����ʱ��������� */
typedef struct t_etm_create_vod_task
{
	ETM_TASK_TYPE _type;

	char* _url;
	u32 _url_len;
	char* _ref_url;
	u32 _ref_url_len;

	char* _seed_file_full_path; 
	u32 _seed_file_full_path_len; 
	u32* _download_file_index_array; 	
	u32 _file_num;						/* _type== ETT_BT ��_type== ETT_KANKAN  ʱ�����������Ҫ�㲥�����ļ�����										[ �ݲ�֧��ETT_KANKAN ����ļ�] */

	char *_tcid;							/* _type== ETT_KANKAN  ʱ��������һ���������ļ�(һ����Ӱ�п��ܱ��ֳ�n���ļ�) TCID ������		[ �ݲ�֧��ETT_KANKAN ����ļ�] */
	uint64 _file_size;						/* _type== ETT_KANKAN  ʱ������������(һ������)��Ҫ�㲥���ļ����ܴ�С								[ �ݲ�֧��ETT_KANKAN ����ļ�] */
	uint64 * _segment_file_size_array;	/* _type== ETT_KANKAN  ʱ����������Ƕ�����ļ�(һ����Ӱ�п��ܱ��ֳ�n���ļ�) file_size ������		[ �ݲ�֧��ETT_KANKAN ����ļ�] */
	char *_gcid;							/* _type== ETT_KANKAN  ʱ��������һ���������ļ�(һ����Ӱ�п��ܱ��ֳ�n���ļ�) GCID ������		[ �ݲ�֧��ETT_KANKAN ����ļ�] */

	Bool _check_data;					/* �Ƿ���ҪУ������  */
	Bool _is_no_disk;					/* �Ƿ�Ϊ���̵㲥  */

	void * _user_data;					/* ���ڴ�����������ص��û�������Ϣ��������������������cookie�� �������cookie��coockie���ֱ�����"Cookie:"��ͷ������"\r\n" ���� */
	u32  _user_data_len;					/* �û����ݵĳ���,����С��65535 */
} ETM_CREATE_VOD_TASK;

/* ����΢������ʱ��������� (�����ϴ������ؼ�KB ��һ�����ݣ�һ��С�ļ���һ��СͼƬ����ҳ��)*/
typedef struct t_etm_mini_task
{
	char* _url;					/* ֻ֧��"http://" ��ͷ��url  */
	u32 _url_len;
	
	Bool _is_file;					/* �Ƿ�Ϊ�ļ���TRUEΪ_file_path,_file_path_len,_file_name,_file_name_len��Ч,_data��_data_len��Ч��FALSE���෴*/
	
	char _file_path[ETM_MAX_FILE_PATH_LEN]; 				/* _is_file=TRUEʱ, ��Ҫ�ϴ������ص��ļ��洢·����������ʵ���� */
	u32 _file_path_len;			/* _is_file=TRUEʱ, ��Ҫ�ϴ������ص��ļ��洢·���ĳ���*/
	char _file_name[ETM_MAX_FILE_NAME_LEN];			/* _is_file=TRUEʱ, ��Ҫ�ϴ������ص��ļ���*/
	u32 _file_name_len; 			/* _is_file=TRUEʱ, ��Ҫ�ϴ������ص��ļ�������*/
	
	u8 * _send_data;			/* _is_file=FALSEʱ,ָ����Ҫ�ϴ�������*/
	u32  _send_data_len;			/* _is_file=FALSEʱ,��Ҫ�ϴ������ݳ��� */
	
	void * _recv_buffer;			/* ���ڽ��շ��������ص���Ӧ���ݵĻ��棬����ʱ���_is_file=TRUE,�˲�����Ч*/
	u32  _recv_buffer_size;		/* _recv_buffer �����С,����Ҫ16384 byte !����ʱ���_is_file=TRUE,�˲�����Ч */
	
	char * _send_header;			/* ���ڷ���httpͷ�� */
	u32  _send_header_len;		/* _send_header�Ĵ�С	 */
	
	void *_callback_fun;			/* ������ɺ�Ļص����� : typedef int32 ( *ETM_NOTIFY_MINI_TASK_FINISHED)(void * user_data, int32 result,void * recv_buffer,u32  recvd_len,void * p_header,u8 * send_data); */
	void * _user_data;			/* �û����� */
	u32  _timeout;				/* ��ʱ,��λ��,����0ʱ180�볬ʱ*/
	u32  _mini_id;				/* id */
	Bool  _gzip;					/* �Ƿ���ܻ���ѹ���ļ� */
} ETM_MINI_TASK;

/************************************************************************/
/*  Interfaces provided by EmbedThunderTaskManager	*/
/************************************************************************/
/////1 ��ʼ���뷴��ʼ����ؽӿ�

/////1.1 ��ʼ����etm_system_path ΪETM ���ڴ��ϵͳ���ݵ�Ŀ¼�������Ѵ����ҿ�д����СҪ��1M �Ŀռ䣬�����ڿ��ƶ��Ĵ洢�豸�У�path_len ����С��ETM_MAX_FILE_PATH_LEN
ETDLL_API int32 etm_init(const char *etm_system_path,u32  path_len);

/////1.2 ����ʼ��
ETDLL_API int32 etm_uninit(void);

/////1.1 ���ⲿ�豸����ʱ�ɵ��øýӿڳ�ʼ����������etm_system_path ΪETM ���ڴ��ϵͳ���ݵ�Ŀ¼�������Ѵ����ҿ�д����СҪ��1M �Ŀռ䣬�����ڿ��ƶ��Ĵ洢�豸�У�path_len ����С��ETM_MAX_FILE_PATH_LEN
ETDLL_API int32 etm_load_tasks(const char *etm_system_path,u32  path_len);

/////U�̰γ�ʱ���øýӿ�ж�ص��������񣬵��Ǳ������̵㲥����
ETDLL_API int32 etm_unload_tasks(void);


/////1.3 ������ؽӿ�
/*
�����������ͣ�
(��16λ) */
#define UNKNOWN_NET   	0x00000000
#define CHINA_MOBILE  	0x00000001
#define CHINA_UNICOM  	0x00000002
#define CHINA_TELECOM 	0x00000004
#define OTHER_NET     	0x00008000
/* (��16λ) */
#define NT_GPRS_WAP   	0x00010000
#define NT_GPRS_NET   	0x00020000
#define NT_3G         		0x00040000
#define NT_WLAN       		0x00080000   // wifi and lan ...

#define NT_CMWAP 		(NT_GPRS_WAP|CHINA_MOBILE)
#define NT_CMNET 		(NT_GPRS_NET|CHINA_MOBILE)

#define NT_CUIWAP 		(NT_GPRS_WAP|CHINA_UNICOM)
#define NT_CUINET 		(NT_GPRS_NET|CHINA_UNICOM)

typedef int32 ( *ETM_NOTIFY_NET_CONNECT_RESULT)(u32 iap_id,int32 result,u32 net_type);
typedef enum t_etm_net_status
{
	ENS_DISCNT = 0, 
	ENS_CNTING, 
	ENS_CNTED 
} ETM_NET_STATUS;

ETDLL_API int32 etm_set_network_cnt_notify_callback( ETM_NOTIFY_NET_CONNECT_RESULT callback_function_ptr);
ETDLL_API int32 etm_init_network(u32 iap_id);
ETDLL_API int32 etm_uninit_network(void);
ETDLL_API ETM_NET_STATUS etm_get_network_status(void);
ETDLL_API int32 etm_get_network_iap(u32 *iap_id);
ETDLL_API int32 etm_get_network_iap_from_ui(u32 *iap_id);
ETDLL_API const char* etm_get_network_iap_name(void);
ETDLL_API int32 etm_get_network_flow(u32 * download,u32 * upload);
ETDLL_API const char* etm_get_peerid(void);

///////////////////////////////////////////////////////////////
/////2 ϵͳ������ؽӿ�

/////2.1  �����û��Զ���ĵײ�ӿ�,����ӿڱ����ڵ���etm_init ֮ǰ����!
#define ET_FS_IDX_OPEN           (0)
#define ET_FS_IDX_ENLARGE_FILE   (1)
#define ET_FS_IDX_CLOSE          (2)
#define ET_FS_IDX_READ           (3)
#define ET_FS_IDX_WRITE          (4)
#define ET_FS_IDX_PREAD          (5)
#define ET_FS_IDX_PWRITE         (6)
#define ET_FS_IDX_FILEPOS        (7)
#define ET_FS_IDX_SETFILEPOS     (8)
#define ET_FS_IDX_FILESIZE       (9)
#define ET_FS_IDX_FREE_DISK      (10)
#define ET_SOCKET_IDX_SET_SOCKOPT (11)
#define ET_MEM_IDX_GET_MEM           (12)
#define ET_MEM_IDX_FREE_MEM          (13)
#define ET_ZLIB_UNCOMPRESS          (14)

/* �����б�˵����
 * 	int32 fun_idx    �ӿں��������
 * 	void *fun_ptr    �ӿں���ָ��
 *
 *
 *  Ŀǰ֧�ֵĽӿں�������Լ����Ӧ�ĺ�������˵����
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		0 (ET_FS_IDX_OPEN)      
 *  ���Ͷ��壺	typedef int32 (*et_fs_open)(char *filepath, int32 flag, u32 *file_id);
 *  ˵����		���ļ�����Ҫ�Զ�д��ʽ�򿪡��ɹ�ʱ����0�����򷵻ش�����
 *  ����˵����
 *	 filepath����Ҫ���ļ���ȫ·���� 
 *	 flag��    ��(flag & 0x01) == 0x01ʱ���ļ��������򴴽��ļ��������ļ�������ʱ��ʧ��
                                                                       ����ļ��������Զ�дȨ�޴��ļ�
                      ��(flag & 0x02) == 0x02ʱ����ʾ��ֻ���ļ�
                      ��(flag & 0x04) == 0x04ʱ����ʾ��ֻд
                      ��(flag ) = 0x 0ʱ����ʾ��дȨ�޴��ļ�
 *	 file_id�� �ļ��򿪳ɹ��������ļ����
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		1 (ET_FS_IDX_ENLARGE_FILE)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_enlarge_file)(u32 file_id, uint64 expect_filesize, uint64 *cur_filesize);
 *  ˵����		���¸ı��ļ���С��Ŀǰֻ��Ҫ��󣩡�һ�����ڴ��ļ��󣬽���Ԥ�����ļ��� �ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id����Ҫ���Ĵ�С���ļ����
 *   expect_filesize�� ϣ�����ĵ����ļ���С
 *   cur_filesize�� ʵ�ʸ��ĺ���ļ���С��ע�⣺�����ô�С�ɹ���һ��Ҫ��ȷ���ô˲�����ֵ!��
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		2 (ET_FS_IDX_CLOSE)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_close)(u32 file_id);
 *  ˵����		�ر��ļ����ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id����Ҫ�رյ��ļ����
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		3 (ET_FS_IDX_READ)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_read)(u32 file_id, char *buffer, int32 size, u32 *readsize);
 *  ˵����		��ȡ��ǰ�ļ�ָ���ļ����ݡ��ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id�� ��Ҫ��ȡ���ļ����
 *   buffer��  ��Ŷ�ȡ���ݵ�bufferָ��
 *   size��    ��Ҫ��ȡ�����ݴ�С�������߿��Ա�֤���ᳬ��buffer�Ĵ�С��
 *   readsize��ʵ�ʶ�ȡ���ļ���С��ע�⣺�ļ���ȡ�ɹ���һ��Ҫ��ȷ���ô˲�����ֵ!��
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		4 (ET_FS_IDX_WRITE)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_write)(u32 file_id, char *buffer, int32 size, u32 *writesize);
 *  ˵����		�ӵ�ǰ�ļ�ָ�봦д�����ݡ��ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id��  ��Ҫд����ļ����
 *   buffer��   ���д�����ݵ�bufferָ��
 *   size��     ��Ҫд������ݴ�С�������߿��Ա�֤���ᳬ��buffer�Ĵ�С��
 *   writesize��ʵ��д����ļ���С��ע�⣺�ļ�д��ɹ���һ��Ҫ��ȷ���ô˲�����ֵ!��
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		5 (ET_FS_IDX_PREAD)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_pread)(u32 file_id, char *buffer, int32 size, uint64 filepos, u32 *readsize);
 *  ˵����		��ȡָ��ƫ�ƴ����ļ����ݡ��ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id�� ��Ҫ��ȡ���ļ����
 *   buffer��  ��Ŷ�ȡ���ݵ�bufferָ��
 *   size��    ��Ҫ��ȡ�����ݴ�С�������߿��Ա�֤���ᳬ��buffer�Ĵ�С��
 *   filepos�� ��Ҫ��ȡ���ļ�ƫ��
 *   readsize��ʵ�ʶ�ȡ���ļ���С��ע�⣺�ļ���ȡ�ɹ���һ��Ҫ��ȷ���ô˲�����ֵ!��
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		6 (ET_FS_IDX_PWRITE)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_pwrite)(u32 file_id, char *buffer, int32 size, uint64 filepos, u32 *writesize);
 *  ˵����		��ָ��ƫ�ƴ�д�����ݡ��ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id��  ��Ҫд����ļ����
 *   buffer��   ���д�����ݵ�bufferָ��
 *   size��     ��Ҫд������ݴ�С�������߿��Ա�֤���ᳬ��buffer�Ĵ�С��
 *   filepos��  ��Ҫ��ȡ���ļ�ƫ��
 *   writesize��ʵ��д����ļ���С��ע�⣺�ļ�д��ɹ���һ��Ҫ��ȷ���ô˲�����ֵ!��
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		7 (ET_FS_IDX_FILEPOS)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_filepos)(u32 file_id, uint64 *filepos);
 *  ˵����		��õ�ǰ�ļ�ָ��λ�á��ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id�� �ļ����
 *   filepos�� ���ļ�ͷ��ʼ������ļ�ƫ��
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		8 (ET_FS_IDX_SETFILEPOS)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_setfilepos)(u32 file_id, uint64 filepos);
 *  ˵����		���õ�ǰ�ļ�ָ��λ�á��ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id�� �ļ����
 *   filepos�� ���ļ�ͷ��ʼ������ļ�ƫ��
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		9 (ET_FS_IDX_FILESIZE)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_filesize)(u32 file_id, uint64 *filesize);
 *  ˵����		��õ�ǰ�ļ���С���ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   file_id�� �ļ����
 *   filepos�� ��ǰ�ļ���С
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		10 (ET_FS_IDX_FREE_DISK)   
 *  ���Ͷ��壺	typedef int32 (*et_fs_get_free_disk)(const char *path, u32 *free_size);
 *  ˵����		���path·�����ڴ��̵�ʣ��ռ䣬һ�������Ƿ���Դ����ļ����ж����ݡ��ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   path��     ��Ҫ��ȡʣ����̿ռ�����ϵ�����·��
 *   free_size��ָ��·�����ڴ��̵ĵ�ǰʣ����̿ռ䣨ע�⣺�˲���ֵ��λ�� KB(1024 bytes) !��
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		11 (ET_SOCKET_IDX_SET_SOCKOPT)   
 *  ���Ͷ��壺	typedef int32 (*et_socket_set_sockopt)(u32 socket, int32 socket_type); 
 *  ˵����		����socket����ز�����Ŀǰֻ֧��Э���PF_INET���ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   socket��     ��Ҫ���õ�socket���
 *   socket_type����socket�����ͣ�Ŀǰ��Ч��ֵ��2����SOCK_STREAM  SOCK_DGRAM����2�����ȡֵ�����ڵ�OSһ�¡�
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		12 (ET_MEM_IDX_GET_MEM)  
 *  ���Ͷ��壺	typedef int32 (*et_mem_get_mem)(u32 memsize, void **mem);
 *  ˵����		�Ӳ���ϵͳ����̶���С�������ڴ�顣�ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   memsize��     ��Ҫ������ڴ��С
 *   mem�� �ɹ�����֮���ڴ���׵�ַ����*mem�з��ء�
 *------------------------------------------------------------------------------------------------------------------
 *  ��ţ�		13 (ET_MEM_IDX_FREE_MEM)   
 *  ���Ͷ��壺	typedef int32 (*et_mem_free_mem)(void* mem, u32 memsize);
 *  ˵����		�ͷ�ָ���ڴ�������ϵͳ���ɹ�����0�����򷵻ش�����
 *  ����˵����
 *   mem��     ��Ҫ�ͷŵ��ڴ���׵�ַ
 *   memsize����Ҫ�ͷŵ��ڴ��Ĵ�С
 *------------------------------------------------------------------------------------------------------------------ 
 *  ��ţ�		14 (ET_ZLIB_UNCOMPRESS)   
 *  ���Ͷ��壺	typedef _int32 (*et_zlib_uncompress)( unsigned char *p_out_buffer, int *p_out_len, const unsigned char *p_in_buffer, int in_len );
 *  ˵����		ָ��zlib��Ľ�ѹ����������,����kad��������Դ���Ľ�ѹ,���emule��Դ����
 *  ����˵����
 *   p_out_buffer����ѹ�����ݻ�����
 *   p_out_len��   ��ѹ�����ݳ���
 *   p_in_buffer�� ����ѹ���ݻ�����
 *   in_len��      ����ѹ���ݳ���
 *------------------------------------------------------------------------------------------------------------------ */
ETDLL_API int32 etm_set_customed_interface(int32 fun_idx, void *fun_ptr);
	
/////2.2 ��ȡ�汾��,����ǰ����ΪETM �İ汾�ţ���һ����Ϊ���ؿ�ET �İ汾��
ETDLL_API const char* etm_get_version(void);

/////2.3 ע��license
/*Ŀǰ�ṩ�����ڲ��Ե����license:
*				License						    |  ��� |		��Ч��		| ״̬
*	09072400010000000000009y4us41bxk5nsno35569 | 0000000 |2009-07-24~2010-07-24	| 00
*/
ETDLL_API int32 	etm_set_license(const char *license, int32 license_size);
ETDLL_API const char* etm_get_license(void);

/////2.4 ��ȡlicense ����֤���,�ӿڷ�����ͽӿڲ���*result ������0 Ϊ��֤ͨ��,����ֵ��������

/*  ����ýӿڷ���ֵΪ102406����ʾETM������Ѹ�׷����������У���Ҫ���Ժ��ٵ��ô˽ӿڡ�
*    �ڽӿڷ���ֵΪ0��ǰ����,�ӿڲ���*result��ֵ��������:
*    4096   ��ʾͨѶ����ԭ��������������������������ˡ�
*    4118   ��ʾETM�ڳ�����12�Σ�ÿ�μ��1Сʱ��license�ϱ�֮�󣬾���ʧ�ܣ�ԭ��ͬ4096��
*    21000 ��ʾ LICENSE ��֤δͨ����ԭ���� LICENSE �����ᡣ
*    21001 ��ʾ LICENSE ��֤δͨ����ԭ���� LICENSE ���ڡ�
*    21002 ��ʾ LICENSE ��֤δͨ����ԭ���� LICENSE �����ա�
*    21003 ��ʾ LICENSE ��֤δͨЭ��ԭ���� LICENSE ���ظ�ʹ�á�
*    21004 ��ʾ LICENSE ��֤δͨЭ��ԭ���� LICENSE ����ٵġ�
*    21005 ��ʾ ��������æ��ETM ����һСʱ���Զ�����,������򲻱���ᡣ
*/
ETDLL_API int32 	etm_get_license_result(u32 * result);

/////2.5 ���ý���.torrent�ļ�ʱϵͳĬ���ַ������ʽ��encoding_mode ����ΪEEM_ENCODING_DEFAULT
ETDLL_API int32 etm_set_default_encoding_mode( ETM_ENCODING_MODE encoding_mode);
ETDLL_API ETM_ENCODING_MODE etm_get_default_encoding_mode(void);

/////2.6 ����Ĭ�����ش洢·��,�����Ѵ����ҿ�д,path_len ����С��ETM_MAX_FILE_PATH_LEN
ETDLL_API int32 etm_set_download_path(const char *path,u32  path_len);
ETDLL_API const char* etm_get_download_path(void);

/////2.7 ��������״̬ת��֪ͨ�ص�����,�˺��������ڴ�������֮ǰ����
// ע��ص������Ĵ���Ӧ������࣬���򲻿����ڻص������е����κ�ETM�Ľӿں���������ᵼ������!
typedef int32 ( *ETM_NOTIFY_TASK_STATE_CHANGED)(u32 task_id, ETM_TASK_INFO * p_task_info);
ETDLL_API int32 	etm_set_task_state_changed_callback( ETM_NOTIFY_TASK_STATE_CHANGED callback_function_ptr);

/////2.8 �������̵㲥��Ĭ����ʱ�ļ�����·��,�����Ѵ����ҿ�д,path_len ҪС��ETM_MAX_FILE_PATH_LEN,�˺��������ڴ������̵㲥����֮ǰ����
ETDLL_API int32 etm_set_vod_cache_path(const char *path,u32  path_len);
ETDLL_API const char* etm_get_vod_cache_path(void);

/////2.9 �������̵㲥�Ļ�������С����λKB�����ӿ�etm_set_vod_cache_path ���������Ŀ¼������д�ռ䣬����3GB ����,�˺��������ڵ���etm_set_vod_cache_path֮��,�������̵㲥����֮ǰ����
ETDLL_API int32 etm_set_vod_cache_size(u32  cache_size);
ETDLL_API u32  etm_get_vod_cache_size(void);

///// ǿ�����vod ����
ETDLL_API int32  etm_clear_vod_cache(void);

/////���û���ʱ�䣬��λ��,Ĭ��Ϊ30�뻺��,���������ø�ֵ,�Ա�֤��������
ETDLL_API int32 etm_set_vod_buffer_time(u32 buffer_time);

/////2.10 ���õ㲥��ר���ڴ��С,��λKB������2MB ����,�˺��������ڴ����㲥����֮ǰ����
ETDLL_API int32 etm_set_vod_buffer_size(u32  buffer_size);
ETDLL_API u32  etm_get_vod_buffer_size(void);

/////2.11 ��ѯvod ר���ڴ��Ƿ��Ѿ�����
ETDLL_API Bool etm_is_vod_buffer_allocated(void);

/////2.12 �ֹ��ͷ�vod ר���ڴ�,ETM �����ڷ���ʼ��ʱ���Զ��ͷ�����ڴ棬�������������뾡���ڳ�����ڴ�Ļ����ɵ��ô˽ӿڣ�ע�����֮ǰҪȷ���޵㲥����������
ETDLL_API int32  etm_free_vod_buffer(void);

/////2.13 �������¸�ϵͳ����:(10M�ڴ�汾ʱ)max_tasks=5,download_limit_speed=-1,upload_limit_speed=-1,auto_limit_speed=FALSE,max_task_connection=128,task_auto_start=FALSE
ETDLL_API int32 	etm_load_default_settings(void);

/////2.14 �������ͬʱ���е��������,��СΪ1,���Ϊ15
ETDLL_API int32 etm_set_max_tasks(u32 task_num);
ETDLL_API u32 etm_get_max_tasks(void);

/////2.15 ������������,��KB Ϊ��λ,����-1Ϊ������,����Ϊ0
ETDLL_API int32 etm_set_download_limit_speed(u32 max_speed);
ETDLL_API u32 etm_get_download_limit_speed(void);

/////2.16 �����ϴ�����,��KB Ϊ��λ,����-1Ϊ������,����Ϊ0,����max_download_limit_speed:max_upload_limit_speed=3:1
ETDLL_API int32 etm_set_upload_limit_speed(u32 max_speed);
ETDLL_API u32 etm_get_upload_limit_speed(void);

/////2.17 �����Ƿ�������������(auto_limit=TRUEΪ����)��linux�±�����root Ȩ������ETM ��ʱ�������Ч
ETDLL_API int32 etm_set_auto_limit_speed(Bool auto_limit);
ETDLL_API Bool etm_get_auto_limit_speed(void);

/////2.18 �������������,10M �ڴ�汾ȡֵ��ΧΪ[1~200]
ETDLL_API int32 etm_set_max_task_connection(u32 connection_num);
ETDLL_API u32 etm_get_max_task_connection(void);

/////2.19 ����ETM �������Ƿ��Զ���ʼ����δ��ɵ�����(auto_start=TRUEΪ��)
ETDLL_API int32 etm_set_task_auto_start(Bool auto_start);
ETDLL_API Bool etm_get_task_auto_start(void);

/////2.20 ��ȡ��ǰȫ���������ٶ�,��Byte Ϊ��λ
ETDLL_API u32 etm_get_current_download_speed(void);

/////2.21 ��ȡ��ǰȫ���ϴ����ٶ�,��Byte Ϊ��λ
ETDLL_API u32 etm_get_current_upload_speed(void);

/////2.22 �������طֶδ�С,[100~1000]����λKB
ETDLL_API int32 etm_set_download_piece_size(u32 piece_size);
ETDLL_API u32 etm_get_download_piece_size(void);

/////2.23 �������������ʾ����������:0 -�ر�,1-��,3-��,4-�����龰ģʽ
ETDLL_API int32 etm_set_prompt_tone_mode(u32 mode);
ETDLL_API u32 etm_get_prompt_tone_mode(void);

/////2.24 ����p2p ����ģʽ:0 -��,1-��WIFI�����´�,2-�ر�
ETDLL_API int32 etm_set_p2p_mode(u32 mode);
ETDLL_API u32 etm_get_p2p_mode(void);

ETDLL_API int32 etm_set_ui_version(const char * version);
/* ��������ӿ����ڽ��汣��ͻ�ȡ������
	item_name ��item_value�Ϊ63�ֽ�
	����get�ӿ�ʱ,���Ҳ�����item_name ��Ӧ��������item_value Ϊ��ʼֵ�������� 
*/
ETDLL_API int32  etm_settings_set_str_item(char * item_name,char *item_value);
ETDLL_API int32  etm_settings_get_str_item(char * item_name,char *item_value);

ETDLL_API int32  etm_settings_set_int_item(char * item_name,int32 item_value);
ETDLL_API int32  etm_settings_get_int_item(char * item_name,int32 *item_value);

ETDLL_API int32  etm_settings_set_bool_item(char * item_name,Bool item_value);
ETDLL_API int32  etm_settings_get_bool_item(char * item_name,Bool *item_value);
///////////////////////////////////////////////////////////////
/////3��������(���������غͱ��±߲�)��ؽӿ�

/////3.1 ������������
ETDLL_API int32 etm_create_task(ETM_CREATE_TASK * p_param,u32* p_task_id );

/////3.2 ��ͣ����
ETDLL_API int32 etm_pause_task (u32 task_id);

/////3.3 ��ʼ����ͣ������
ETDLL_API int32 etm_resume_task(u32 task_id);

/////3.4 ������������վ
ETDLL_API int32 etm_delete_task(u32 task_id);

/////3.5 ��ԭ��ɾ��������
ETDLL_API int32 etm_recover_task(u32 task_id);

/////3.6 ����ɾ������,��delete_fileΪTRUEʱ��ʾɾ������������Ϣ��ͬʱҲɾ�������Ӧ���ļ�������ע��delete_file ֻ����ͨ����(ETT_URL,ETT_TCID,ETT_KANKAN,ETT_EMULE)��Ч,��bt ���񲻻�ɾ���κ��ļ�
ETDLL_API int32 etm_destroy_task(u32 task_id,Bool delete_file);

/////3.7 ��ȡ�����������ȼ�task_id �б�,����״̬ΪETS_TASK_WAITING,ETS_TASK_RUNNING,ETS_TASK_PAUSED,ETS_TASK_FAILED�Ķ���������б���,*buffer_len�ĵ�λ��sizeof(u32)
ETDLL_API int32 etm_get_task_pri_id_list (u32 * id_array_buffer,u32 *buffer_len);

/////3.8 �������������ȼ�����
ETDLL_API int32 etm_task_pri_level_up(u32 task_id,u32 steps);
	
/////3.9 �������������ȼ�����
ETDLL_API int32 etm_task_pri_level_down (u32 task_id,u32 steps);

/////3.10 �����������ļ�������״̬����ΪETS_TASK_SUCCESS����ֻ����ͨ����(ETT_URL,ETT_TCID,ETT_KANKAN,ETT_EMULE)��Ч����������bt����new_name_len����С��ETM_MAX_FILE_NAME_LEN-ETM_MAX_TD_CFG_SUFFIX_LEN
ETDLL_API int32 etm_rename_task(u32 task_id,const char * new_name,u32 new_name_len);

/////3.11 ��ȡ����Ļ�����Ϣ,��etm_create_vod_task�ӿڴ���������Ҳ���Ե�������ӿ�
ETDLL_API int32 etm_get_task_info(u32 task_id,ETM_TASK_INFO *p_task_info);

/////3.12 ��ȡ�������е� ���������״��,��etm_create_vod_task�ӿڴ���������Ҳ���Ե�������ӿ�
ETDLL_API int32 etm_get_task_running_status(u32 task_id,ETM_RUNNING_STATUS *p_status);

/////3.13 ��ȡ����ΪETT_URL ����ETT_EMULE�� �����URL
ETDLL_API const char* etm_get_task_url(u32 task_id);

/////3.14 ��ȡ����ΪETT_URL �� ���������ҳ��URL(����еĻ�)
ETDLL_API const char* etm_get_task_ref_url(u32 task_id);

/////3.15 ��ȡ����ΪETT_TCID  �� ETT_KANKAN �� �����TCID
ETDLL_API const char* etm_get_task_tcid(u32 task_id);

/////3.16 ��ȡ����ΪETT_KANKAN �� �����GCID
ETDLL_API const char* etm_get_task_gcid(u32 task_id);

/////3.17 ��ȡ����ΪETT_BT �� ����������ļ�ȫ·��
ETDLL_API const char* etm_get_bt_task_seed_file(u32 task_id);

/////3.18 ��ȡBT������ĳ�����ļ�����Ϣ,file_indexȡֵ��ΧΪ>=0����etm_create_vod_task�ӿڴ�����bt ����Ҳ���Ե�������ӿ�
ETDLL_API int32 etm_get_bt_file_info(u32 task_id, u32 file_index, ETM_BT_FILE *file_info);

/////3.19 �޸�BT������Ҫ���ص��ļ�����б�,���֮ǰ��Ҫ�µ��ļ���Ȼ��Ҫ���صĻ���ҲҪ����new_file_index_array��
ETDLL_API int32 etm_set_bt_need_download_file_index(u32 task_id, u32* new_file_index_array,	u32 new_file_num);

/////3.20 ��ȡ������û���������,*buffer_len�ĵ�λ��byte
ETDLL_API int32 etm_get_task_user_data(u32 task_id,void * data_buffer,u32 * buffer_len);

/////3.21 ��������״̬��ȡ����id �б�(����Զ�̿��ƴ���������ͱ��ش���������),*buffer_len�ĵ�λ��sizeof(u32)
ETDLL_API int32 	etm_get_task_id_by_state(ETM_TASK_STATE state,u32 * id_array_buffer,u32 *buffer_len);

/////3.22 ��������״̬��ȡ���ش���������id �б�(������Զ�̿��ƴ���������),*buffer_len�ĵ�λ��sizeof(u32)
ETDLL_API int32 	etm_get_local_task_id_by_state(ETM_TASK_STATE state,u32 * id_array_buffer,u32 *buffer_len);

/////3.23 ��ȡ���е�����id �б�,*buffer_len�ĵ�λ��sizeof(u32)
ETDLL_API int32 	etm_get_all_task_ids(u32 * id_array_buffer,u32 *buffer_len);

///////////////////////////////////////////////////////////////
/////����������ؽӿ�
/////3.23 ����URL ,cid,gicd ��BT���ӵ�info_hash ���Ҷ�Ӧ������id
typedef struct t_etm_eigenvalue
{
	ETM_TASK_TYPE _type;				/* �������ͣ�����׼ȷ��д*/
	
	char* _url;							/* _type== ETT_URL ����_type== ETT_EMULE ʱ��������������URL */
	u32 _url_len;						/* URL �ĳ��ȣ�����С��512 */
	char _eigenvalue[40];					/* _type== ETT_TCID ʱ,�����CID,_type== ETT_KANKAN ʱ,�����GCID, _type== ETT_BT ʱ,�����info_hash   */
}ETM_EIGENVALUE;
ETDLL_API int32 	etm_get_task_id_by_eigenvalue(ETM_EIGENVALUE * p_eigenvalue,u32 * task_id);

/////3.24 ����������Ϊ��������
ETDLL_API int32 etm_set_task_lixian_mode(u32 task_id,Bool is_lixian);

/////3.25 ��ѯ�����Ƿ�Ϊ��������
ETDLL_API Bool etm_is_lixian_task(u32 task_id);

/////3.26 ����server ��Դ,file_indexֻ��BT�������õ�,��ͨ����file_index�ɺ���
typedef struct t_etm_server_res
{
	u32 _file_index;
	u32 _resource_priority;
	char* _url;							
	u32 _url_len;					
	char* _ref_url;				
	u32 _ref_url_len;		
	char* _cookie;				
	u32 _cookie_len;		
}ETM_SERVER_RES;
ETDLL_API int32 etm_add_server_resource( u32 task_id, ETM_SERVER_RES  * p_resource );

/////3.27 ����peer ��Դ,file_indexֻ��BT�������õ�,��ͨ����file_index�ɺ���
#define ETM_PEER_ID_SIZE 16
typedef struct t_etm_peer_res
{
	u32 _file_index;
	char _peer_id[ETM_PEER_ID_SIZE];							
	u32 _peer_capability;					
	u32 _res_level;
	u32 _host_ip;
	u16 _tcp_port;
	u16 _udp_port;
}ETM_PEER_RES;
ETDLL_API int32 etm_add_peer_resource( u32 task_id, ETM_PEER_RES  * p_resource );


///////////////////////////////////////////////////////////////
/////4 torrent �ļ�������E-Mule���ӽ�����ؽӿ�,����torrent�ļ������������ӿڱ���ɶ�ʹ��!

/////4.1 �������ļ�������������Ϣ
ETDLL_API int32 etm_get_torrent_seed_info (const char *seed_path, ETM_ENCODING_MODE encoding_mode,ETM_TORRENT_SEED_INFO **pp_seed_info );

/////4.2 �ͷ�������Ϣ
ETDLL_API int32 etm_release_torrent_seed_info( ETM_TORRENT_SEED_INFO *p_seed_info );

/////4.3 ��E-Mule�����н������ļ���Ϣ
ETDLL_API int32 etm_extract_ed2k_url(char* ed2k, ETM_ED2K_LINK_INFO* info);

///////////////////////////////////////////////////////////////
/////5 VOD�㲥��ؽӿ�

/////5.1 �����㲥����
ETDLL_API int32 etm_create_vod_task(ETM_CREATE_VOD_TASK * p_param,u32* p_task_id );

/////5.2 ֹͣ�㲥,��������������etm_create_vod_task�ӿڴ����ģ���ô������񽫱�ɾ��,���������etm_create_task�ӿڴ�������ֹֻͣ�㲥
ETDLL_API int32 etm_stop_vod_task (u32 task_id);

/////5.3 ��ȡ�ļ����ݣ���etm_create_task�ӿڴ���������Ҳ�ɵ�������ӿ���ʵ�ֱ����ر߲���
///// ����len(һ�λ�ȡ���ݵĳ���,��λByte)Ϊ16*1024 �Դﵽ���Ż�ȡ�ٶȣ�block_time ��λΪ����,  ȡֵ500,1000����
ETDLL_API int32 etm_read_vod_file(u32 task_id, uint64 start_pos, uint64 len, char *buf, u32 block_time );
ETDLL_API int32 etm_read_bt_vod_file(u32 task_id, u32 file_index, uint64 start_pos, uint64 len, char *buf, u32 block_time );

/////5.4 ��ȡ����(���ʳ���30������ݳ���)�ٷֱȣ���etm_create_task�ӿڴ����������ڱ����ر߲���ʱҲ�ɵ�������ӿ�
ETDLL_API int32 etm_vod_get_buffer_percent(u32 task_id,  u32 * percent );

/////5.5 ��ѯ�㲥�����ʣ�������Ƿ��Ѿ�������ɣ�һ�������ж��Ƿ�Ӧ�ÿ�ʼ������һ����Ӱ��ͬһ����Ӱ����һƬ�Ρ���etm_create_task�ӿڴ����������ڱ����ر߲���ʱҲ�ɵ�������ӿ�
ETDLL_API int32 etm_vod_is_download_finished(u32 task_id, Bool* finished );

/////5.6 ��ȡ�㲥�����ļ�������
ETDLL_API int32 etm_vod_get_bitrate(u32 task_id, u32 file_index, u32* bitrate );

/////5.7 ��ȡ��ǰ���ص�������λ��
ETDLL_API int32 etm_vod_get_download_position(u32 task_id, uint64* position );

/////5.8 ���㲥����תΪ��������,file_retain_timeΪ�����ļ��ڴ��̵ı���ʱ��(����0Ϊ���ñ���),��λ��
ETDLL_API int32 etm_vod_set_download_mode(u32 task_id, Bool download,u32 file_retain_time );

/////5.9 ��ѯ�㲥�����Ƿ�תΪ��������
ETDLL_API int32 etm_vod_get_download_mode(u32 task_id, Bool * download,u32 * file_retain_time );

/////5.10 �㲥ͳ���ϱ�
typedef struct t_etm_vod_report
{
       u32	_vod_play_time_len;//�㲥ʱ��,��λ��Ϊ��
       u32  _vod_play_begin_time;//��ʼ�㲥ʱ��
	u32  _vod_first_buffer_time_len; //�׻���ʱ��
	u32  _vod_play_drag_num;//�϶�����
	u32  _vod_play_total_drag_wait_time;//�϶���ȴ���ʱ��
	u32  _vod_play_max_drag_wait_time;//����϶��ȴ�ʱ��
	u32  _vod_play_min_drag_wait_time;//��С�϶��ȴ�ʱ��
	u32  _vod_play_interrupt_times;//�жϴ���
	u32  _vod_play_total_buffer_time_len;//�жϵȴ���ʱ��
	u32  _vod_play_max_buffer_time_len;//����жϵȴ�ʱ��
	u32  _vod_play_min_buffer_time_len;//��С�жϵȵ�ʱ��
	
	u32 _play_interrupt_1;         // 1�������ж�
	u32 _play_interrupt_2;	        // 2�������ж�
	u32 _play_interrupt_3;		    //6�������ж�
	u32 _play_interrupt_4;		    //10�������ж�
	u32 _play_interrupt_5;		    //15�������ж�
	u32 _play_interrupt_6;		    //15���������ж�
}ETM_VOD_REPORT;
ETDLL_API int32 etm_vod_report(u32 task_id, ETM_VOD_REPORT * p_report );


///////////////////////////////////////////////////////////////
/// 6. ����洢����ؽӿ�,ע������������������㲥�������κι�ϵ��һ����������������״�˵���

#define ETM_INVALID_NODE_ID	0

/* whether if create tree while it not exist. */
#define ETM_TF_CREATE		(0x1)
/* read and write (default) */
#define ETM_TF_RDWR		(0x0)
/* read only. */
#define ETM_TF_RDONLY		(0x2)
/* write only. */
#define ETM_TF_WRONLY		(0x4)
#define ETM_TF_MASK       (0xFF)


// �������һ����(flag=ETM_TF_CREATE|ETM_TF_RDWR),����ɹ�*p_tree_id���������ڵ�id
ETDLL_API int32 	etm_open_tree(const char * file_path,int32 flag ,u32 * p_tree_id); 

// �ر�һ����
ETDLL_API int32 	etm_close_tree(u32 tree_id); 

// ɾ��һ����
ETDLL_API int32 	etm_destroy_tree(const char * file_path); 

// �ж����Ƿ����
ETDLL_API Bool 	etm_tree_exist(const char * file_path); 

// ����һ����
ETDLL_API int32 	etm_copy_tree(const char * file_path,const char * new_file_path); 


// ����һ���ڵ�,data_len(��λByte)��С������,�����鲻����256
ETDLL_API int32 	etm_create_node(u32 tree_id,u32 parent_id,const char * name, u32 name_len,void * data, u32 data_len,u32 * p_node_id); 

// ɾ��һ���ڵ㡣ע��:������node_id����֦��������������Ҷ�ڵ�ͬʱɾ��;node_id���ܵ���tree_id,ɾ����Ҫ��etm_destroy_tree
ETDLL_API int32 	etm_delete_node(u32 tree_id,u32 node_id);

// ���ýڵ�����(��������),name_len����С��256 bytes;node_id���ܵ���tree_id
ETDLL_API int32 	etm_set_node_name(u32 tree_id,u32 node_id,const char * name, u32 name_len);

// ��ȡ�ڵ�����;node_id����tree_idʱ���������������
ETDLL_API  const char * etm_get_node_name(u32 tree_id,u32 node_id);

// ���û��޸Ľڵ�����,new_data_len(��λByte)��С������,�����鲻����256;
ETDLL_API int32 	etm_set_node_data(u32 tree_id,u32 node_id,void * new_data, u32 new_data_len);

// ��ȡ�ڵ�����,*buffer_len�ĵ�λ��Byte;
ETDLL_API int32	etm_get_node_data(u32 tree_id,u32 node_id,void * data_buffer,u32 * buffer_len);

// �ƶ��ڵ�
///������ͬ����;node_id���ܵ���tree_id
ETDLL_API int32 	etm_set_node_parent(u32 tree_id,u32 node_id,u32 parent_id);

///ͬһ�������ƶ�;node_id���ܵ���tree_id
ETDLL_API int32 	etm_node_move_up(u32 tree_id,u32 node_id,u32 steps);
ETDLL_API int32 	etm_node_move_down(u32 tree_id,u32 node_id,u32 steps);


// ��ȡ���ڵ�id;node_id���ܵ���tree_id
ETDLL_API u32 	etm_get_node_parent(u32 tree_id,u32 node_id);

// ��ȡ�����ӽڵ�id �б�,*buffer_len�ĵ�λ��sizeof(u32)
ETDLL_API int32	etm_get_node_children(u32 tree_id,u32 node_id,u32 * id_buffer,u32 * buffer_len);

// ��ȡ��һ���ӽڵ�id,�Ҳ�������ETM_INVALID_NODE_ID
ETDLL_API u32	etm_get_first_child(u32 tree_id,u32 parent_id);

// ��ȡ�ֵܽڵ�id;node_id���ܵ���tree_id,�Ҳ�������ETM_INVALID_NODE_ID
ETDLL_API u32	etm_get_next_brother(u32 tree_id,u32 node_id);
ETDLL_API u32	etm_get_prev_brother(u32 tree_id,u32 node_id);

// �������ֲ��ҽڵ�id���������ݲ�֧�ֲַ�ʽ��д������aaa.bbb.ccc��
// ���ص�һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID
ETDLL_API u32	etm_find_first_node_by_name(u32 tree_id,u32 parent_id,const char * name, u32 name_len);

// ������һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID;node_id���ܵ���tree_id
ETDLL_API u32	etm_find_next_node_by_name(u32 tree_id,u32 parent_id,u32 node_id,const char * name, u32 name_len); //ע��node_id��parent_id�Ķ�Ӧ��ϵ
// ������һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID;node_id���ܵ���tree_id
ETDLL_API u32	etm_find_prev_node_by_name(u32 tree_id,u32 parent_id,u32 node_id,const char * name, u32 name_len); //ע��node_id��parent_id�Ķ�Ӧ��ϵ


// ���ݽڵ����ݲ��ҽڵ�,data_len��λByte.

// ���ص�һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID
ETDLL_API u32	etm_find_first_node_by_data(u32 tree_id,u32 parent_id,void * data, u32 data_len);

// ������һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID;node_id���ܵ���tree_id
ETDLL_API u32	etm_find_next_node_by_data(u32 tree_id,u32 parent_id,u32 node_id,void * data, u32 data_len);
// ������һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID;node_id���ܵ���tree_id
ETDLL_API u32	etm_find_prev_node_by_data(u32 tree_id,u32 parent_id,u32 node_id,void * data, u32 data_len);

// ���ݽڵ����ֺ����ݲ��Һ��ʵĽڵ�,data_len��λByte.
// ���ص�һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID
ETDLL_API u32	etm_find_first_node(u32 tree_id,u32 parent_id,const char * name, u32 name_len,void * data, u32 data_len);

// ������һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID;node_id���ܵ���tree_id
ETDLL_API u32	etm_find_next_node(u32 tree_id,u32 parent_id,u32 node_id,const char * name, u32 name_len,void * data, u32 data_len);//ע��node_id��parent_id�Ķ�Ӧ��ϵ
// ������һ��ƥ��Ľڵ�id���Ҳ�������ETM_INVALID_NODE_ID;node_id���ܵ���tree_id
ETDLL_API u32	etm_find_prev_node(u32 tree_id,u32 parent_id,u32 node_id,const char * name, u32 name_len,void * data, u32 data_len);//ע��node_id��parent_id�Ķ�Ӧ��ϵ


//////////////////////////////////////////////////////////////
/////7 ����

/////7.1 ��URL �л���ļ������ļ���С��������ܴ�URL ��ֱ�ӽ����õ���ETM �����ѯѸ�׷��������		[ �ݲ�֧�ֻ�ȡ�ļ���С] 
/// ע��: Ŀǰֻ֧��http://,https://,ftp://��thunder://,ed2k://��ͷ��URL, url_len����С��512
///               ���ֻ��õ��ļ�����file_size��Ϊ NULL �����ֻ��õ��ļ���С��name_buffer��Ϊ NULL
///               ���ڸýӿ�Ϊͬ��(�߳�����)�ӿ�,�����Ҫ����block_time,��λΪ����
ETDLL_API int32 etm_get_file_name_and_size_from_url(const char* url,u32 url_len,char *name_buffer,u32 *name_buffer_len,uint64 *file_size, int32 block_time );

/////7.2 ����΢����������,�ص�������resultΪ0��ʾ�ɹ����˽ӿ��ѱ�etm_http_get ��etm_http_get_file���,�����ʹ��!��
// typedef int32 ( *ETM_NOTIFY_MINI_TASK_FINISHED)(void * user_data, int32 result,void * recv_buffer,u32  recvd_len,void * p_header,u8 * send_data);
ETDLL_API int32 etm_get_mini_file_from_url(ETM_MINI_TASK * p_mini_param );

/////7.3 ����΢���ϴ�����,�ص�������resultΪ0��ʾ�ɹ����˽ӿ��ѱ�etm_http_post���,�����ʹ��!��
// typedef int32 ( *ETM_NOTIFY_MINI_TASK_FINISHED)(void * user_data, int32 result,void * recv_buffer,u32  recvd_len,void * p_header,u8 * send_data);
ETDLL_API int32 etm_post_mini_file_to_url(ETM_MINI_TASK * p_mini_param );

/////7.4 ǿ��ȡ��΢�����񡾴˽ӿ��ѱ�etm_http_cancel���,�����ʹ��!��
ETDLL_API int32 etm_cancel_mini_task(u32 mini_id );

//////////////////////////////////////////////////////////////
/* HTTP�Ự�ӿڵĻص���������*/
typedef enum t_etm_http_cb_type
{
	EHCT_NOTIFY_RESPN=0, 
	EHCT_GET_SEND_DATA, 	// Just for POST
	EHCT_NOTIFY_SENT_DATA, 	// Just for POST
	EHCT_GET_RECV_BUFFER,
	EHCT_PUT_RECVED_DATA,
	EHCT_NOTIFY_FINISHED
} ETM_HTTP_CB_TYPE;
typedef struct t_etm_http_call_back
{
	u32 _http_id;
	void * _user_data;			/* �û����� */
	ETM_HTTP_CB_TYPE _type;
	void * _header;				/* _type==EHCT_NOTIFY_RESPNʱ��Ч,ָ��http��Ӧͷ,����etm_get_http_header_value��ȡ�������ϸ��Ϣ*/
	
	u8 ** _send_data;			/* _type==EHCT_GET_SEND_DATAʱ��Ч, ���ݷֲ��ϴ�ʱ,ָ����Ҫ�ϴ�������*/
	u32  * _send_data_len;		/* ��Ҫ�ϴ������ݳ��� */
	
	u8 * _sent_data;			/* _type==EHCT_NOTIFY_SENT_DATAʱ��Ч, ���ݷֲ��ϴ�ʱ,ָ���Ѿ��ϴ�������,�м�����_sent_data_len�Ƿ�Ϊ��,���涼Ҫ�����ͷŸ��ڴ�*/
	u32   _sent_data_len;		/* �Ѿ��ϴ������ݳ��� */
	
	void ** _recv_buffer;			/* _type==EHCT_GET_RECV_BUFFERʱ��Ч, ���ݷֲ�����ʱ,ָ�����ڽ������ݵĻ���*/
	u32  * _recv_buffer_len;		/* �����С ,����Ҫ16384 byte !*/
	
	u8 * _recved_data;			/* _type==EHCT_PUT_RECVED_DATAʱ��Ч, ���ݷֲ�����ʱ,ָ���Ѿ��յ�������,�м�����_recved_data_len�Ƿ�Ϊ��,���涼Ҫ�����ͷŸ��ڴ�*/
	u32  _recved_data_len;		/* �Ѿ��յ������ݳ��� */

	int32 _result;					/* _type==EHCT_NOTIFY_FINISHEDʱ��Ч, 0Ϊ�ɹ�*/
} ETM_HTTP_CALL_BACK;
typedef int32 ( *ETM_HTTP_CALL_BACK_FUNCTION)(ETM_HTTP_CALL_BACK * p_http_call_back_param); 
/* HTTP�Ự�ӿڵ�������� */
typedef struct t_etm_http_get
{
	char* _url;					/* ֻ֧��"http://" ��ͷ��url  */
	u32 _url_len;
	
	char * _ref_url;				/* ����ҳ��URL*/
	u32  _ref_url_len;		
	
	char * _cookie;			
	u32  _cookie_len;		
	
	uint64  _range_from;			/* RANGE ��ʼλ��,���ݲ�֧�֡�*/
	uint64  _range_to;			/* RANGE ����λ��,���ݲ�֧�֡�*/
	
	Bool  _accept_gzip;			/* �Ƿ����ѹ���ļ� */
	
	void * _recv_buffer;			/* ���ڽ��շ��������ص���Ӧ���ݵĻ���*/
	u32  _recv_buffer_size;		/* _recv_buffer �����С,����Ҫ16384 byte !*/
	
	ETM_HTTP_CALL_BACK_FUNCTION _callback_fun;			/* ������ɺ�Ļص����� : typedef int32 ( *ETM_HTTP_CALL_BACK_FUNCTION)(ETM_HTTP_CALL_BACK * p_http_call_back_param); */
	void * _user_data;			/* �û����� */
	u32  _timeout;				/* ��ʱ,��λ��,����0ʱ180�볬ʱ*/
} ETM_HTTP_GET;
typedef struct t_etm_http_post
{
	char* _url;					/* ֻ֧��"http://" ��ͷ��url  */
	u32 _url_len;
	
	char * _ref_url;				/* ����ҳ��URL*/
	u32  _ref_url_len;		
	
	char * _cookie;			
	u32  _cookie_len;		
	
	uint64  _content_len;			/* Content-Length:*/
	
	Bool  _send_gzip;			/* �Ƿ���ѹ������*/
	Bool  _accept_gzip;			/* �Ƿ����ѹ������*/
	
	u8 * _send_data;				/* ָ����Ҫ�ϴ�������*/
	u32  _send_data_len;			/* ��Ҫ�ϴ������ݴ�С*/
	
	void * _recv_buffer;			/* ���ڽ��շ��������ص���Ӧ���ݵĻ���*/
	u32  _recv_buffer_size;		/* _recv_buffer �����С,����Ҫ16384 byte !*/

	ETM_HTTP_CALL_BACK_FUNCTION _callback_fun;			/* ������ɺ�Ļص����� : typedef int32 ( *ETM_HTTP_CALL_BACK_FUNCTION)(ETM_HTTP_CALL_BACK * p_http_call_back_param); */
	void * _user_data;			/* �û����� */
	u32  _timeout;				/* ��ʱ,��λ��,����0ʱ180�볬ʱ*/
} ETM_HTTP_POST;
typedef struct t_etm_http_get_file
{
	char* _url;					/* ֻ֧��"http://" ��ͷ��url  */
	u32 _url_len;
	
	char * _ref_url;				/* ����ҳ��URL*/
	u32  _ref_url_len;		
	
	char * _cookie;			
	u32  _cookie_len;		
	
	uint64  _range_from;			/* RANGE ��ʼλ��,���ݲ�֧�֡�*/
	uint64  _range_to;			/* RANGE ����λ��,���ݲ�֧�֡�*/
	
	Bool  _accept_gzip;			/* �Ƿ����ѹ���ļ� */
	
	char _file_path[ETM_MAX_FILE_PATH_LEN]; 				/* _is_file=TRUEʱ, ��Ҫ�ϴ������ص��ļ��洢·����������ʵ���� */
	u32 _file_path_len;			/* _is_file=TRUEʱ, ��Ҫ�ϴ������ص��ļ��洢·���ĳ���*/
	char _file_name[ETM_MAX_FILE_NAME_LEN];			/* _is_file=TRUEʱ, ��Ҫ�ϴ������ص��ļ���*/
	u32 _file_name_len; 			/* _is_file=TRUEʱ, ��Ҫ�ϴ������ص��ļ�������*/
	
	ETM_HTTP_CALL_BACK_FUNCTION _callback_fun;			/* ������ɺ�Ļص����� : typedef int32 ( *ETM_HTTP_CALL_BACK_FUNCTION)(ETM_HTTP_CALL_BACK * p_http_call_back_param); */
	void * _user_data;			/* �û����� */
	u32  _timeout;				/* ��ʱ,��λ��,����0ʱ180�볬ʱ*/
} ETM_HTTP_GET_FILE;


/////7.6 ����http get �Ự
// typedef int32 ( *ETM_HTTP_CALL_BACK_FUNCTION)(ETM_HTTP_CALL_BACK * p_http_call_back_param);
ETDLL_API int32 etm_http_get(ETM_HTTP_GET * p_http_get ,u32 * http_id);
ETDLL_API int32 etm_http_get_file(ETM_HTTP_GET_FILE * p_http_get_file ,u32 * http_id);

/////7.7 ����http post �Ự
// typedef int32 ( *ETM_HTTP_CALL_BACK_FUNCTION)(ETM_HTTP_CALL_BACK * p_http_call_back_param);
ETDLL_API int32 etm_http_post(ETM_HTTP_POST * p_http_post ,u32 * http_id );

/////7.8 ǿ��ȡ��http�Ự,����������Ǳ���ģ�ֻ��UI���ڻص�������ִ��֮ǰȡ���ûỰʱ�ŵ���
ETDLL_API int32 etm_http_cancel(u32 http_id );

/////7.9 ��ȡhttpͷ���ֶ�ֵ,�ú�������ֱ���ڻص����������
/* httpͷ�ֶ�*/
typedef enum t_etm_http_header_field
{
	EHHV_STATUS_CODE=0, 
	EHHV_LAST_MODIFY_TIME, 
	EHHV_COOKIE,
	EHHV_CONTENT_ENCODING,
	EHHV_CONTENT_LENGTH
} ETM_HTTP_HEADER_FIELD;
ETDLL_API char * etm_get_http_header_value(void * p_header,ETM_HTTP_HEADER_FIELD header_field );

//////////////////////////////////////////////////////////////
/////8 Զ�̿���

/*��Ϣ�ص�����(etm_remote_control_start���سɹ���,��ͨ������ص�����֪ͨԶ�̿����ڲ���Ϣ)*/
/* ע��:�����ڴ˻ص�������ֱ�ӵ���ETM�Ľӿ�, ���������*/
/* msg_id ��Ϣ
0: ������   //��һ��ʹ��Զ�̿���,����Ѽ�������ʾ���û�,��Ҫ�û�ȥweb�󶨼�����.
1: �ܿسɹ� //�û�ʹ��һ����Ч���Ѿ��󶨼������Զ�̿���.
2: ������Ϣ //�������յ������Ϣ,��Ҫȥ���Լ����߳������etm_remote_control_stop�ӿ�,����ֱ�ӵ���etm_remote_control_start����Զ�̿���.
* msg��Ϣ
* msg _len��Ϣ����
*/
typedef int32 ( *ET_RT_CTRL_MSG_NOTIFY)(u32 msg_id,  char * msg, u32 msg_len);

typedef struct t_etm_rc_path
{
	char _data_path[ETM_MAX_FILE_PATH_LEN]; 
} ETM_RC_PATH;


/*  ��ʼʹ��Զ�̿���
*ע��:����web����չʾΪutf-8,����ʹ��Զ�̿��ƹ�����Ҫ����etm_set_default_encoding_mode( EEM_ENCODING_UTF8);
* data_path_array:Ĭ�ϴ洢·������,·������С��256.(����web����ʾ),��ͬ·�������ڲ�ͬ���̡�
* path_num: �洢·������(С�ڵ���3��)
* 104449 ��ʾ���ô˽ӿ�ʱ������.��ʱ�����ڷ�stop״̬(ע���ڵ���stop�����ϵ���start , ��Ϊ��������δ��freebind�������,���Ժ��ӷ���104450������) 
* 104451 path�����Ƿ�( ��ȷ����: 0<path_num<=3 )
* 104452 path���ȷǷ�( ��ȷ·������: 0<path_length<ETM_MAX_FILE_PATH_LEN )
* 104453 path�Ƿ�,path������,��path����д
* 102406 ��ʾliscense��ͨ��,����ETM������Ѹ�׷�����������,��Ҫ�Ե�liscense�ɹ����ٵ��ô˽ӿ�.����liscense�Ƿ�
*/
ETDLL_API int32 etm_remote_control_start( ETM_RC_PATH data_path_array[], u32 path_num, ET_RT_CTRL_MSG_NOTIFY call_back );


/* ֹͣʹ��Զ�̿��� 
 * 104450 ��ʾ���ô˽ӿ�ʱ������.������stop״̬��connecting״̬,����û�õ���start��ֱ�ӵ���stop ((ע����start�����ϵ���stop ,��Ϊ��������δ�ܿسɹ�,���ӷ���104450������)
*/

ETDLL_API int32 etm_remote_control_stop(void);


//////////////////////////////////////////////////////////////
/////8 drm


/* 
 *  ����drm֤��Ĵ��·��
 *  certificate_path:  drm֤��·��
 * ����ֵ: 0     �ɹ�
           4201  certificate_path�Ƿ�(·�������ڻ�·�����ȴ���255)
 */
ETDLL_API int32 etm_set_certificate_path(const char *certificate_path);


/* 
 *  ��xlmv�ļ�,��ȡ�����ļ���drm_id
 *  p_file_full_path:  xlmv�ļ�ȫ·��
 *  p_drm_id: ���ص�drm_id(һ���ļ���Ӧһ��,�Ҳ����ظ�)
 *  p_origin_file_size: ���ص�drmԭʼ�ļ��Ĵ�С
 * ����ֵ: 0     �ɹ�
           22529 drm�ļ�������.
           22530 drm�ļ���ʧ��.
           22531 drm�ļ���ȡʧ��.
           22532 drm�ļ���ʽ����.
           22537 drm֤���ʽ����,���߷Ƿ�֤���ļ�.
           22538 ��֧��openssl��,�޷������ļ�
 */
ETDLL_API int32 etm_open_drm_file(const char *p_file_full_path, u32 *p_drm_id, uint64 *p_origin_file_size);


/* 
 *  xlmv�ļ���Ӧ֤���ļ��Ƿ�Ϸ�����
 *  drm_id:  xlmv�ļ���Ӧ��drm_id
 *  p_is_ok: �Ƿ�֤���ļ��Ϸ�����
 * ����ֵ: 0     �ɹ�
           22534 ��Ч��drm_id.
           22535 ֤������ʧ��.
           22536 drm�ڲ��߼�����.
           22537 drm֤���ʽ����,���߷Ƿ�֤���ļ�.
           22538 ��֧��openssl��,�޷������ļ�
           
           (���´�������Ҫ���ظ��������û�֪��)
           22628 ���ӵ�pid�󶨵��û���������
           22629 �û�û�й���ָ����ӰƬ
           22630 ��Ʒ�Ѿ��¼ܣ��û��������ٹ�����������Ѿ�����֮���¼ܵģ��û����ǿ��Բ���
           22631 �󶨵ĺ�������������
           22632 ��Կδ�ҵ���������֤�����ɹ����г������쳣
           22633 ������æ��һ����ϵͳѹ�����󣬵��²�ѯ���߰󶨴����쳣�������������쳣���ͻ��˿��Կ������ԡ�
 */

ETDLL_API int32 etm_is_certificate_ok(u32 drm_id, Bool *p_is_ok);


/* 
 *  xlmv�ļ���ȡ����
 *  drm_id:      xlmv�ļ���Ӧ��drm_id
 *  p_buffer:    ��ȡ����buffer
 *  size:        ��ȡ����buffer�Ĵ�С
 *  file_pos:    ��ȡ���ݵ���ʼλ��
 *  p_read_size: ʵ�ʶ�ȡ���ݴ�С
 * ����ֵ: 0     �ɹ�
           22533 ֤����δ�ɹ�
           22534 ��Ч��drm_id.
           22536 drm�ڲ��߼�����.
           22537 drm֤���ʽ����,���߷Ƿ�֤���ļ�.
 */
ETDLL_API int32 etm_read_drm_file(u32 drm_id, char *p_buffer, u32 size, 
    uint64 file_pos, u32 *p_read_size );


/* 
 *  �ر�xlmv�ļ�
 *  drm_id:      xlmv�ļ���Ӧ��drm_id
 * ����ֵ: 0     �ɹ�
           22534 ��Ч��drm_id.
 */
ETDLL_API int32 etm_close_drm_file(u32 drm_id);


#define ETM_OPENSSL_IDX_COUNT			    (7)

/* function index */
#define ETM_OPENSSL_D2I_RSAPUBLICKEY_IDX    (0)
#define ETM_OPENSSL_RSA_SIZE_IDX            (1)
#define ETM_OPENSSL_BIO_NEW_MEM_BUF_IDX     (2)
#define ETM_OPENSSL_D2I_RSA_PUBKEY_BIO_IDX  (3)
#define ETM_OPENSSL_RSA_PUBLIC_DECRYPT_IDX  (4)
#define ETM_OPENSSL_RSA_FREE_IDX            (5)
#define ETM_OPENSSL_BIO_FREE_IDX            (6)

/* 
 *  drm��Ҫ�õ�openssl������rsa������صĺ���,��Ҫ������ö�Ӧ�Ļص���������.(���������ؿ�����ǰ����)
 *  fun_count:    ��Ҫ�Ļص���������,�������ET_OPENSSL_IDX_COUNT
 *  fun_ptr_table:    �ص���������,������һ��void*������
 * ����ֵ: 0     �ɹ�
           3273  ��������,fun_count��ΪET_OPENSSL_IDX_COUNT��fun_ptr_tableΪNULL������Ԫ��ΪNULL

 *  ��ţ�0 (ET_OPENSSL_D2I_RSAPUBLICKEY_IDX)  ����ԭ�ͣ�typedef RSA * (*et_func_d2i_RSAPublicKey)(RSA **a, _u8 **pp, _int32 length);
 *  ˵������Ӧopenssl���d2i_RSAPublicKey����

 *  ��ţ�1 (ET_OPENSSL_RSA_SIZE_IDX)  ����ԭ�ͣ�typedef _int32 (*et_func_openssli_RSA_size)(const RSA *rsa);
 *  ˵������Ӧopenssl���RSA_size����

 *  ��ţ�2 (ET_OPENSSL_BIO_NEW_MEM_BUF_IDX)  ����ԭ�ͣ�typedef BIO *(*et_func_BIO_new_mem_buf)(void *buf, _int32 len);
 *  ˵������Ӧopenssl���BIO_new_mem_buf����

 *  ��ţ�3 (ET_OPENSSL_D2I_RSA_PUBKEY_BIO_IDX)  ����ԭ�ͣ�typedef RSA *(*et_func_d2i_RSA_PUBKEY_bio)(BIO *bp,RSA **rsa);
 *  ˵������Ӧopenssl���d2i_RSA_PUBKEY_bio����

 *  ��ţ�4 (ET_OPENSSL_RSA_PUBLIC_DECRYPT_IDX)  ����ԭ�ͣ�typedef _int32(*et_func_RSA_public_decrypt)(_int32 flen, const _u8 *from, _u8 *to, RSA *rsa, _int32 padding);
 *  ˵������Ӧopenssl���RSA_public_decrypt����

 *  ��ţ�5 (ET_OPENSSL_RSA_FREE_IDX)  ����ԭ�ͣ�typedef void(*et_func_RSA_free)( RSA *r );
 *  ˵������Ӧopenssl���RSA_free����

 *  ��ţ�6 (ET_OPENSSL_BIO_FREE_IDX)  ����ԭ�ͣ�typedef void(*et_func_BIO_free)( BIO *a );
 *  ˵������Ӧopenssl���BIO_free����

 */

ETDLL_API int32 etm_set_openssl_rsa_interface(int32 fun_count, void *fun_ptr_table);


//////////////////////////////////////////////////////////////
/////9 ��ѯ�������Ӧ�ļ�Ҫ������ע��1~1024�������ڲ�ͬ��ƽ̨����������ͬ
ETDLL_API const char * etm_get_error_code_description(int32 error_code);





#ifdef __cplusplus
}
#endif
#endif /* ET_TASK_MANAGER_H_200909081912 */

