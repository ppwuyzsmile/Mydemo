#pragma once
#ifndef _PC_COMMAND_H_
#define _PC_COMMAND_H_


//#include "datatype.h"
/*#include "integer.h"*/
//#define API_LIB_NO_DEBUG
#define INTERFACE_SPEED 115200
#define SIG_START_NUM	0xabcd
#define SIG_END_NUM		0x1314
#define CMD_P2P_PARA_MAXLEN 112
#define CMD_WRITE_TRANS_MAX_LEN 109
#define TID_LEN 8
#define UII_LEN 32
#define RID_LEN 3
//#define SEND_PKG_INDEX sizeof(unsigned short)

#define PKI_PRIVATEKEY_LEN 99
#define PKI_PUBLICKEY_LEN 69

#define READER_SET_KEY	0			//��д�Ķ���
#define TAG_SET_KEY		1			//��д��ǩ

#define FILE_ID_MF		( 0x3f00 )
#define FILE_ID_TID		( 0xa101 )
#define FILE_ID_UII		( 0xa102 )
#define FILE_ID_CONF	( 0xa103 )


#define CMD_LIST_MODE_NORMAL 1	/*�б������PC������ɾ�����߼�,���Ķ���ֻ�ǰ��չ��������ת��,�ֽ׶�ֻ֧��1*/
#define CMD_LIST_MODE_ALL_IN_ONE 0 /*�б��ļ����Ķ���һ����ɺ�,ͳһ���ظ�PC��*/

#define		ID_BTN_TOOLBAR1		501
#define		ID_BTN_TOOLBAR2 	502
/************************�ļ�ϵͳ������غ궨��************************/
#define		FILE_TYPE_INVALID			1 << 4
#define		FILE_TYPE_MASK				0x0F
#define		FILE_INVALID_MASK			0X10

#define		FILE_TYPE_DF				0
#define		FILE_TYPE_NORMAL			1
#define		FILE_TYPE_TRANS				FILE_TYPE_NORMAL
#define		FILE_TYPE_RECORD			3

#define		TOKEN_TYPE_LIST			0X01
#define		TOKEN_TYPE_VALID		0X02
#define		TOKEN_TYPE_RDSEARCH		0X03
#define		TOKEN_TYPE_UPDATE		0X04
#define		TOKEN_TYPE_SELECT		0X05
#define		TOKEN_TYPE_KILLTAG		0X06

#define		TOKEN_VALID_MASK		0XC0
#define		TOKEN_UPDATE_MASK		0X30
#define		TOKEN_RDSEARCH_MASK		0X0C
#define		TOKEN_LIST_MASK			0X0C
#define		TOKEN_SELECT_MASK		0X03

#define		TOKEN_VALID_STATE_ALLOW	0X00
#define		TOKEN_VALID_STATE_ATH	0X80
#define		TOKEN_VALID_STATE_PWD	0X40
#define		TOKEN_VALID_STATE_NEV	0XC0

#define		TOKEN_UPDATE_STATE_ALLOW	0X00
#define		TOKEN_UPDATE_STATE_ATH		0X20
#define		TOKEN_UPDATE_STATE_PWD		0x10
#define		TOKEN_UPDATE_STATE_NEV		0X30

#define		TOKEN_RDSEARCH_STATE_ALLOW		0X00
#define		TOKEN_RDSEARCH_STATE_ATH			0X08
#define		TOKEN_RDSEARCH_STATE_PWD			0X04
#define		TOKEN_RDSEARCH_STATE_NEV			0X0C

#define		TOKEN_LIST_STATE_ALLOW			0X00
#define		TOKEN_LIST_STATE_ATH			0X08
#define		TOKEN_LIST_STATE_PWD			0X04
#define		TOKEN_LIST_STATE_NEV			0X0C

#define		TOKEN_SELECT_STATE_ALLOW		0X00
#define		TOKEN_SELECT_STATE_ATH		0X02
#define		TOKEN_SELECT_STATE_PWD		0X01
#define		TOKEN_SELECT_STATE_NEV		0X03

//----------------------------------------------------------
#define		FILE_MODE_NEXT_FILE			2
#define		FILE_MODE_PREV_FILE			3
#define		FILE_MODE_FIXED_OFFSET		4


#define		RECORD_UPDATE_MODE_PREV			3
#define		RECORD_UPDATE_MODE_NEXT			2
#define		RECORD_UPDATE_MODE_FIX			4

#define		RECORD_IN_FILE_READ_MODE_PREV			3
#define		RECORD_IN_FILE_READ_MODE_NEXT			2  //��ȡ�ļ�����һ����¼
#define		RECORD_IN_FILE_READ_MODE_FIX			4
#define		RECORD_IN_RESULT_READ_MODE_PREV			0x13
#define		RECORD_IN_RESULT_READ_MODE_NEXT			0x12
#define		RECORD_IN_RESULT_READ_MODE_FIX			0x14

#define		DERECTORY_LIST_MODE_CURRENT		0x04
#define		DERECTORY_LSIT_MODE_NEXT		0x02
#define		DERECTORY_LIST_MODE_PREV		0x03

#define		FILE_DESCRIPTOR_MF				0x3F00
#define		FILE_DESCRIPTOR_TID				0xA101
#define		FILE_DESCRIPTOR_UII				0XA102	
#define		FILE_DESCRIPTOR_CONF			0xA103


#define		MAX_READ_LEN	80		
#define		MAX_WRITE_LEN 	109		
#define		MAX_WRITE_CACHE_LEN		160
#define		MAX_FILE_LENGTH_IN_TAG		65500		//��ǩ�ļ����ֵ

/*	NOTE THAT:
 *	���ݰ�ȫЭ��֡Ҫ��,һ����Ч������֡�ڶ��ļ�ʱ��ԭ�������109�ֽ�,����Ϊ����Ϊ��ȫ֡����
 *
 */
#define		MAX_READ_LARGE_LEN	80//109
#define		MAX_READ_LARGE_LEN_S 80//109

#define		MIN_READ_LARGE_LEN 2000//���ļ����Ķ�����С�ļ���С

/**************************��:��¼�ļ�������غ궨��************************/
#ifndef NULL
#define NULL 0
#endif
#define MAX_QUERY_ELEM_NUMS 16
#define MAX_QUERY_RESULT_NUMS 255
#define LOGICAL_C 'C'
#define LOGICAL_A 'A'
#define LOGICAL_O 'O'

#define RELATION_EQAUL			'='
#define RELATION_EQUAL_GREATER	'G'
#define RELATION_EQUAL_LESS		'L'
#define RELATION_LESS			'<'
#define RELATION_GREATER		'>'
#define RELATION_NOT_EQUAL		'!'

#define ECC_TYPE	1
#define AES_TYPE	0							//using in PC_CMD_TYPE_REGISTER_TAG

#define PUBLIC_KEY	0
#define PRIVATE_KEY	1

/*
#pragma pack(1)
typedef struct{
	unsigned char logicalOperator;//1
	unsigned char itemOffset;//2
	unsigned char relationOperator;//3
	unsigned char dataLen;//4
	unsigned char buffer[1];//0.....
}QUERY_ELEMENT,*PQUERY_ELEMENT;
#pragma pack()
*/

#define FILE_SELECTED_REC 0X04
#define QUERY_SELECTED_REC 0X14
#define FILE_ATTR_LEN	10


#pragma pack(1)
typedef struct
{
	BYTE usbFlag;//USB��ʶλ�������ʼ��Ϊ0
	BYTE pkgNUM;//�ְ����
	BYTE endFlag;//�ְ�������־��0��ʾû�н�����1��ʾ����
	BYTE dataLen;//ÿ���ְ��ĳ���
	char dataBuf[60];
}MULTI_PACKET_SEND_RECV,*pMULTI_PACKET_SEND_RECV;

typedef struct _file_attribute {
	unsigned short	fileID;
	unsigned char	fileTypeValid; //�ļ�������Ч��Ч����
	union{
		struct{
			unsigned short constMaxFileLen;
			unsigned short numsOfMaxFile;
			unsigned short numsOfRealFile;
		}MFDF;
		struct{
			unsigned short maxFileLen;//����ļ�����
			unsigned short constMaxRecord;//Ĭ����Ϊ0001h
			unsigned short realFileLen;//ʵ���ļ�����
		}transBinEF;
		struct{
			unsigned short recordLen;//��¼����
			unsigned short maxRecordNums;//����ļ���¼��
			unsigned short realRecordNums;//ʵ�ʼ�¼��
		}recEF;
	}uFileAttr;
	unsigned char securityType;
	
#if defined(STRUCT_PACK_1)
	//��������˽ṹ���Ա1�ֽڶ��룬���ڱ��ļ���ͷ����STRUCT_PACK_1��
	//�Ա�֤token�ṹ��Ķ������
	uint16 nothing;
#endif
}FILE_ATTRIBUTE_R;

//-----------------------------------------------------------------------------
typedef struct{
	FILE_ATTRIBUTE_R fileAttr_R;
	union {
			struct {//MFDF
				unsigned int valid_token;
				unsigned int list_token;
			} MFDF;
			struct {//͸���ļ�
				unsigned int valid_token;
				unsigned int read_token;
				unsigned int update_token;
			} transBinEF;
			struct {//��¼�ļ�
				unsigned int valid_token;
				unsigned int read_search_token;
				unsigned int update_token;
			} recEF;
	}access_tokens;
}FILE_ATTRIBUTE_W;

//--------------------------------------------------------------------------------
typedef struct{
	unsigned char TID[8];
	unsigned char UII[32];
	unsigned int memoryVolume;	//�洢������
	unsigned char inqMaxNum;	//��ѯԪ���������
	unsigned short inqMaxMatchNum;	//��ѯ������ƥ������
	unsigned char MIK[32];
	unsigned char MEK[32];
}TAG_ATTRIBUTE;

typedef struct{
	unsigned char TID[8];
	unsigned char UII[32];
	unsigned int memoryVolume;	//�洢������
	unsigned char inqMaxNum;	//��ѯԪ���������
	unsigned short inqMaxMatchNum;	//��ѯ������ƥ������
	int	Channel;	//�����ŵ�
	int Power;		//���ù���
}TAG_ATTRIBUTE_R;

#pragma pack(1)
typedef struct{
	unsigned char logicalOperator;
	unsigned char fieldOffset;
	unsigned char relationOperator;
	unsigned char dataLen;
	unsigned char dataBuf[70];			
}SEARCH_ELEMENT,*PSEARCH_ELEMENT;
#pragma pack()

/*****************************�Ķ��������������*****************************/
typedef enum _BAUD_TYPE{
	BAUD_TYPE_9600=0,
	BAUD_TYPE_115200,
	BAUD_TYPE_128000,
}BAUD_TYPE;
typedef enum _ENCRYPT_TYPE{
	ENCRYPT_TYPE_NOR = 0,
	ENCRYPT_TYPE_HASH,
	ENCRYPT_TYPE_AES,
	ENCRYPT_TYPE_ECC
}ENCRYPT_TYPE;
typedef enum _FILE_SYS_TYPE{
	FILE_SYS_TYPE_NO_USED = 1,
	FILE_SYS_TYPE_FATFS,
	FILE_SYS_TYPE_GBJFS
}FILE_SYS_TYPE;

typedef struct{
	unsigned char	RID[4];			//�Ķ���RID
	unsigned char	ReadName[32];	//�Ķ�������
	BAUD_TYPE		baudRate;		//������
	ENCRYPT_TYPE	encryptType;	//��������
	FILE_SYS_TYPE	fileSysType;	//�Ķ��������õ��ļ�ϵͳ����
	unsigned int	storeCapacity;	//�洢����
}READER_INFO;

typedef struct{
	unsigned char	RID[4];			//�Ķ���RID
	char			ReadName[32];	//�Ķ�������
	BAUD_TYPE		baudRate;		//������
}READER_INFO_W;

#pragma pack()
/***********************************************
 *�������Ͷ���:
 *			PC�����Ķ����ӿ������
 *
 ************************************************/
typedef enum{
	PC_CMD_TYPE_COLLECT=1,				//�ռ�,�̴�����1
	PC_CMD_TYPE_SELECT_TAG,				//ѡ���ǩ2
	PC_CMD_TYPE_SELECT_FILE,			//ѡ���ļ�3
	PC_CMD_TYPE_READ_FILE,				//���ļ�4
	PC_CMD_TYPE_READ_LARGE_FILE,		//�����ļ���������5
	PC_CMD_TYPE_WRITE_FILE,				//д�ļ�6
	PC_CMD_TYPE_CREATE_FILE,			//�����ļ�7
	PC_CMD_TYPE_VALID_FILE,				//��Ч,��Ч�ļ�[�ļ�ɾ��]8
	PC_CMD_TYPE_LIST_FILE,				//�б��ļ�,��Ŀ¼�ļ�����ö��9
	PC_CMD_TYPE_VERIFY_PWD_FILE,		//��֤�ļ�����10
	PC_CMD_TYPE_SLEEP_ALL_TAG,			//�������б�ǩ11
	PC_CMD_TYPE_MONITOR_CONF,			//�����������12
	PC_CMD_TYPE_MONITOR_START,			//���������ʼ13
	PC_CMD_TYPE_MONITOR_REPORT,			//��������ϱ�14
	PC_CMD_TYPE_MONITOR_STOP,			//�������ֹͣ15
	PC_CMD_TYPE_MONITOR_SENSOR_EN,		//���Ӵ�����״̬16
	PC_CMD_TYPE_SAFE_SESSION,			//��ȫ�Ự17
	PC_CMD_TYPE_REGISTER_TAG,			//ע���ǩ18
	PC_CMD_TYPE_GET_ALL_REGISTER_TAG,	//�õ����е��Ѿ�ע��ı�ǩ��Ϣ19
	PC_CMD_TYPE_CLR_ALL_REGISTER_TAG,	//������е��Ѿ�ע��ı�ǩ��Ϣ20
	PC_CMD_TYPE_INIT_TAG,				//��ʼ����ǩ21
	PC_CMD_TYPE_GET_READER_INFO,		//��ȡ�Ķ�����Ϣ22
	PC_CMD_TYPE_SET_READER_INFO,		//�����Ķ�����Ϣ23
	PC_CMD_TYPE_WAKEUP_ONE_TAG,			//���ѵ�����ǩ24
	PC_CMD_TYPE_FILE_DELETE,			//�ļ�ɾ��25
	PC_CMD_TYPE_FILESYS_FORMAT,			//�ļ�ϵͳ��ʽ��26
	PC_CMD_TYPE_SELLECT_CHANNEL,		//ѡ���Ķ����ŵ�27
	PC_CMD_TYPE_CONNECT_TAG,			//����PC��Tag֮�������28
	PC_CMD_TYPE_DISCONNECT_TAG,			//�Ͽ�PC��Tag֮�������29
	PC_CMD_TYPE_KILL_TAG,				//��ǩ���30
	PC_CMD_TYPE_WRITECACHE_SYNC,		//д����ͬ��31
	PC_CMD_TYPE_WRITE_CACHE,			//��д���ļ���͸���ļ��������ݴ����Ķ���Flash��32
	PC_CMD_TYPE_SEND_CACHE,				//���Ķ���Flash�е����ݷ��ͳ�ȥ������ǩ��33
	PC_CMD_TYPE_CLEAN_CACHE,			//�������34
	PC_CMD_TYPE_SET_POWER,				//�����Ķ�������35
	PC_CMD_TYPE_SEND_PUREWAVE,			//������ز�36
	PC_CMD_TYPE_STOP_PUREWAVE,			//ֹͣ���ز�37
	PC_CMD_TYPE_SEND_MODUL_INFO,		//���������Ϣ38
	PC_CMD_TYPE_STOP_MODUL_INFO,		//ֹͣ������Ϣ39
	PC_CMD_TYPE_SET_RECV_MODEL,			//���ý���ģʽ40
	PC_CMD_TYPE_STOP_RECV_MODEL,		//ֹͣ����ģʽ41
	PC_CMD_TYPE_SET_KEY,				//������Կ42
	PC_CMD_TYPE_SLEEP_EX_ONES_TAG,		//���߳�ĳһ���ǩ�����б�ǩ43
	PC_CMD_TYPE_SLEEP_EX_ONE_TAG,		//���߳�ĳ����ǩ�����б�ǩ44
	PC_CMD_TYPE_GET_TAG_INFO,			//��ȡ��ǩ��Ϣ45
	PC_CMD_TYPE_SET_POWER_TAG,			//���ñ�ǩ����46
	PC_CMD_TYPE_SET_CHANNEL_TAG,		//���ñ�ǩ�ŵ�47
	PC_CMD_TYPE_SEARCH_RECORDS,			//������¼48
	PC_CMD_TYPE_UPDATE_PWD,				//����ϵͳ����49
	PC_CMD_TYPE_UPDATE_ADMIN_PWD,		//���¹���Ա����50
	PC_CMD_TYPE_GET_TAG_SAFE_PARAM,		//��ȡ��ǩ����ȫ����51
	PC_CMD_TYPE_REQUIRE_RANDOM,			//���������52
	PC_CMD_TYPE_DATA_BOARDCAST,			//���ݹ㲥53
	PC_CMD_TYPE_SEARCH_ALLTAG_RECS,		//�������б�ǩ��¼54
	PC_CMD_TYPE_SEARCH_ALLTAG_TRANS,	//�������б�ǩ͸��55
	PC_CMD_TYPE_COLLECT_SPEC_CONT,		//�̴�ָ�����ݱ�ǩ56
	PC_CMD_TYPE_SEARCH_FILE,			//�����ļ�idд���ݻ��ȡ�ļ�����57
	PC_CMD_TYPE_WAKEUP_TAG,             //��������58
	PC_CMD_TYPE_SECURITY_COMMUNICATION, //��ȫ�Ự������ڲ�ͬ��һ�ڰ�ȫ�Ự59
	PC_CMD_TYPE_SET_TCPIP,              //����TCP/IP����60
	PC_CMD_TYPE_SET_QN,                 //����Q/Nֵ61
	PC_CMD_TYPE_SET_MAC,                //����MAC��ַ62
	PC_CMD_TYPE_TEST_SINGLE_CMD,        //������63
	
	// ----------------------------------Test------------------------------------
	PC_CMD_TYPE_TEST_SET_PARAM = 100,	//��������
	PC_CMD_TYPE_TEST_FREQ_ACCURACY,		//Ƶ��&׼ȷ��
	PC_CMD_TYPE_TEST_FREQ_ACCURACY_Q,	//Ƶ��&׼ȷ�ȣ�ȡ����
	PC_CMD_TYPE_TEST_MIN_RCVOR_BAND,	//��С���ջ�����
	PC_CMD_TYPE_TEST_MIN_RCVOR_BAND_Q,	//��С���ջ�����ȡ����
	PC_CMD_TYPE_TEST_READER_SWITCH_TIME,//(Reader)ת��ʱ��
	PC_CMD_TYPE_TEST_TAG_SWITCH_TIME,	//(Tag)ת��ʱ��
	PC_CMD_TYPE_TEST_SWITCH_TIME_Q,		//ת��ʱ�䣨ȡ����
	PC_CMD_TYPE_TEST_MODUL_MODE,		//���Ʒ�ʽ
	PC_CMD_TYPE_TEST_MODUL_MODE_Q,		//���Ʒ�ʽ��ȡ����
	PC_CMD_TYPE_TEST_FSSS_SEQUENCE,		//��Ƶ����
	PC_CMD_TYPE_TEST_FSSS_SEQUENCE_Q,	//��Ƶ���У�ȡ����
	PC_CMD_TYPE_TEST_BIT_SPEED,			//λ����
	PC_CMD_TYPE_TEST_BIT_SPEED_Q,		//λ���ʣ�ȡ����
	PC_CMD_TYPE_TEST_BEFORE_CODE,		//ǰ����
	PC_CMD_TYPE_TEST_BEFORE_CODE_Q,		//ǰ���루ȡ����
	PC_CMD_TYPE_TEST_PREVCOLLI_MANAGE,	//��ǩ����ײ����
	PC_CMD_TYPE_TEST_PREVCOLLI_MANAGE_Q,//��ǩ����ײ����ȡ����
	PC_CMD_TYPE_TEST_RCV_SENSITIVITY,	//����������
	PC_CMD_TYPE_TEST_RCV_SENSITIVITY_Q,	//���������ȣ�ȡ����
}PC_CMD_TYPE;

typedef enum _TEST_RADIO_STATE{
	TEST_RADIO_GET_REPLY,
	TEST_RADIO_NOT_GET_REPLY
}TEST_RADIO_STATE; //���Ե������״̬

typedef enum{
	SET_TCPIP_QN_MAC_SUCCESS,
	SET_TCPIP_QN_MAC_FAILED,
	SET_TCPIP_QN_MAC_TIMEOUT
}SET_TCPIP_QN_MAC_STATE;

typedef enum{
	SEARCH_FILE_STATE_SUCESS,
	SEARCH_FILE_STATE_FAILD,
	SEARCH_FILE_STATE_TIMEOUT
}SEARCH_FILE_STATE;

typedef enum{
	WAKEUP_TAG_STATE_SUCCESS,
	WAKEUP_TAG_STATE_FAILED,
	WAKEUP_TAG_STATE_TIMEOUT
}WAKEUP_TAG_STATE;

typedef enum{
	COLLECT_TAG_STATE_SUCCESS, //�̴�ɹ�
	COLLECT_TAG_STATE_FAILED,   //�̴�ʧ��
	COLLECT_TAG_STATE_SEND_FAILED,	//�����̴�����ʧ��
	COLLECT_TAG_STATE_EMPTY,		//û�̴浽��ǩ
	COLLECT_TAG_STATE_TIMEOUT		//�̴泬ʱ
}COLLECT_TAG_STATE;

typedef enum{
	SELECT_TAG_STATE_SUCCESS,//ѡ���ǩ�ɹ�
	SELECT_TAG_STATE_NOT_EXISTING,//��ѡ��ı�ǩ������
	SELECT_TAG_STATE_ALREADY_SELECTED,//��ǩ�Ѿ���ѡ��
	SELECT_TAG_STATE_SEND_FAILED,	//����ѡ���ǩ����ʧ��
	SELECT_TAG_STATE_TIMEOUT		//ѡ���ǩ��ʱ
}SELECT_TAG_STATE;

typedef enum{
	SELECT_FILE_STATE_SUCCESS,//ѡ���ļ��ɹ�
	SELECT_FILE_STATE_NEED_PWD_VERIFY,//ѡ���ļ�,��Ҫ������֤
	SELECT_FILE_STATE_NEED_ATH_VERIFY,//ѡ���ļ�,��Ҫʵ����֤
	SELECT_FILE_STATE_NOT_EXISTING,//��ѡ����ļ�������
	SELECT_FILE_STATE_DENIED,//ѡ���ļ�,���ʱ��ܾ�,���ļ�������ѡ��Ȩ��
	SELECT_FILE_STATE_TIMEOUT		//��������ʧ��
}SELECT_FILE_STATE;

typedef enum{
	LIST_FILE_STATE_SUCCESS,
	LIST_FILE_STATE_NEED_PWD_VERIFY,
	LIST_FILE_STATE_NEED_ATH_VERIFY,
	LSIT_FILE_STATE_FILE_NOT_EXISTING,
	LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE,	//next modeʱ,�Ѿ������һ���ļ�.
	LIST_FILE_STATE_FAILED,					//�б��ļ�ʧ��
	LIST_FILE_STATE_TIMEOUT,				//�б��ļ���ʱ
	LIST_FILE_STATE_BUFF_NOT_ENOUGH,		//����������
	LIST_FILE_STATE_NUMS_ERROR,				//�����źŲ��ȶ������б��ļ�������ȫ
	LIST_FILE_STATE_NO_TID,					//�����ڵ�TID
	LIST_FILE_NOT_ALLOWED
}LIST_FILE_STATE;

typedef enum{
	READ_FILE_STATE_SUCCESS,
	READ_FILE_STATE_NOT_EXISTING,
	READ_FILE_STATE_FAILED,
	READ_FILE_STATE_LAST_RECORD,
	READ_FILE_STATE_FINISH,
	READ_FILE_STATE_TIMEOUT,
	READ_FILE_STATE_NEED_PWD_VERIFY,
	READ_FILE_STATE_NEED_ATH_VERIFY,
	READ_FILE_STATE_NEED_NEV_VERIFY
}READ_FILE_STATE;

typedef enum _WRITE_FILE_STATE{
	WRITE_FILE_STATE_SUCCESS,
	WRITE_FILE_STATE_FAILED,
	WRITE_FILE_STATE_TIMEOUT,
	WRITE_FILE_STATE_PWD_VERIFY,
	WRITE_FILE_STATE_ATH_VERIFY,
	WRITE_FILE_STATE_NEV_VERIFY
}WRITE_FILE_STATE;

typedef enum _WRITE_FILE_STATE_SIMPLE{
	WRITE_FILE_STATE_SUCCESS_S = 'A',
	WRITE_FILE_STATE_FAILED_S,
	WRITE_FILE_STATE_TIMEOUT_S,
	WRITE_FILE_STATE_PWD_VERIFY_S,
	WRITE_FILE_STATE_ATH_VERIFY_S,
	WRITE_FILE_STATE_NEV_VERIFY_S
}WRITE_FILE_STATE_SIMPLE;

typedef enum _CREATE_FILE_STATE{
	CREATE_FILE_STATE_SUCCESS,
	CREATE_FILE_STATE_FAILED,
	CREATE_FILE_STATE_IN_NULL,	//�����ָ��Ϊ��
	CREATE_FILE_STATE_TIMEOUT,
	CREATE_FILE_STATE_EXISTING,		//�ļ��Ѿ�����
	CREATE_FILE_NOT_ALLOWED,
	CREATE_FILE_INVALID,//����Ŀ¼����ЧĿ¼
	CREATE_FILE_NEED_PWD,//��Ҫ������֤
	CREATE_FILE_NOT_AUTHORIZED,
	CREATE_FILE_TOO_DEEP,//�ļ���ȹ���
	CREATE_FILE_ATTRIBUTE_ERROR,//�������Դ���
	CREATE_FILE_SPACE_NOT_ENOUPH, //�ռ䲻��
	CREATE_FILE_TOO_MUCH_FILES, //Ŀ¼���ļ���Ŀ�ﵽ����
}CREATE_FILE_STATE;

typedef enum _VALID_FILE_STATE{
	VALID_FILE_STATE_SUCCESS,
	VALID_FILE_STATE_TIMEOUT,
	VALID_FILE_STATE_FILE_NOT_EXIST,
	VALID_FILE_STATE_NOT_AUTHORISED,
	VALID_FILE_STATE_ALREADY_INVALID,
	VALID_FILE_STATE_UNKNOWN_ERR,
	VALID_FILE_STATE_NOT_ALLOWED,
	VALID_FILE_STATE_FAILED,		//δͨ��
	VALID_FILE_STATE_PWD_VERIFY,
	VALID_FILE_STATE_ATH_VERIFY,
	VALID_FILE_STATE_NEV_VERIFY
}VALID_FILE_STATE;

typedef enum _DELETE_FILE_STATE{
	FILE_DELETE_STATE_SUCCESS,
	FILE_DELETE_STATE_TIMEOUT,
	FILE_DELETE_STATE_UNKNOWN_ERR,
	FILE_DELETE_STATE_SYSFILE	//ϵͳ�ļ�����ɾ��
}FILE_DELETE_STATE;


typedef enum _FILESYS_FORMAT_STATE{
	FILESYS_FORMAT_STATE_SUCCESS,
	FILESYS_FORMAT_STATE_TIMEOUT,
	FILESYS_FORMAT_STATE_PWD_ERROR
}FILESYS_FORMAT_STATE;

typedef enum _VERIFY_PWD_FILE_STATE{
	VERIFY_PWD_FILE_STATE_SUCCESS,
	VERIFY_PWD_FILE_STATE_FAILED,
	VERIFY_PWD_FILE_STATE_TIMEOUT,
}VERIFY_PWD_FILE_STATE;

typedef enum _MONITOR_SENSOREN_STATE{
	MONITOR_SENSOREN_STATE_SUCCESS,
	MONITOR_SENSOREN_STATE_UNKNOWN,
	MONITOR_SENSOREN_STATE_SENSOR_IDMISMATCH,
	MONITOR_SENSOREN_STATE_TIMEOUT,
	MONITOR_SENSOREN_STATE_FAILED,
	MONITOR_SENSOREN_STATE_SEND_FAILED //��������ʧ��
}MONITOR_SENSOREN_STATE;

typedef enum _SAFE_SESSION_STATE{
	SAFE_SESSION_STATE_SUCCESS,
	SAFE_SESSION_STATE_UNKNOWN,
	SAFE_SESSION_STATE_TIMEOUT,
	SAFE_SESSION_STATE_FAILED,
	SAFE_SESSION_STATE_ALREADY_IN		//�ѽ��밲ȫ�Ự
}SAFE_SESSION_STATE;

typedef enum _INTERFACE_TYPE{
	IF_TYPE_INIT 							= (1 << 8),
	IF_TYPE_DEINIT 							= (2 << 8),
	IF_TYPE_COLLECT_TAG 					= (3 << 8),
	IF_TYPE_SELECT_TAG 						= (4 << 8),
	IF_TYPE_OPEN_SENSOR 					= (5 << 8),
	IF_TYPE_SLEEP_ALL_TAG 					= (6 << 8),
	IF_TYPE_LIST_FILE 						= (7 << 8),
	IF_TYPE_CREATE_FILE 					= (8 << 8),
	IF_TYPE_SELECT_FILE 					= (9 << 8),
	IF_TYPE_READ_RECORD_FILE 				= (10 << 8),
	IF_TYPE_READ_TRANS_FILE 				= (11 << 8),
	IF_TYPE_WRITE_FILE 						= (12 << 8),
	IF_TYPE_ENTITY_VERIFY 					= (13 << 8), 
	IF_TYPE_VERIFY_PWD 						= (14 << 8),
	IF_TYPE_VALID_FILE 						= (15 << 8),
	IF_TYPE_READ_LARGE_FILE 				= (16 << 8),
	IF_TYPE_WAKEUP_ONE_TAG 					= (17 << 8),
	IF_TYPE_SAFE_SESSION 					= (18 << 8),
	IF_TYPE_REGISTER_TAG 					= (19 << 8),
	IF_TYPE_GET_ALL_TAG 					= (20 << 8),
	IF_TYPE_CLR_ALL_TAG 					= (21 << 8),
	IF_TYPE_INIT_TAG 						= (22 << 8),
	IF_TYPE_GET_RADER_INFO 					= (23 << 8),
	IF_TYPE_SET_RADER_INFO 					= (24 << 8),
	IF_TYPE_rfReadLargeTransFile 			= (25 << 8),
	IF_TYPE_rfWriteLargeTransFile 			= (26 << 8),
	IF_TYPE_FILE_DELETE						= (27 << 8),
	IF_TYPE_FILESYS_FORMAT					= (28 << 8),
	IF_TYPE_SELLECT_CHANNEL					= (29 << 8),
	IF_TYPE_CONNECT_TAG						= (30 << 8),
	IF_TYPE_DISCONNECT_TAG					= (31 << 8),
	IF_TYPE_KILL_TAG						= (32 << 8),
	IF_TYPE_WRITECACHE_SYNC					= (33 << 8),
	IF_TYPE_WRITE_CACHE						= (34 << 8),
	IF_TYPE_SEND_CACHE						= (35 << 8),
	IF_TYPE_CLEAN_CACHE						= (36 << 8),
	IF_TYPE_SET_POWER						= (37 << 8),
	IF_TYPE_SEND_PUREWAVE					= (38 << 8),
	IF_TYPE_STOP_PUREWAVE					= (39 << 8),
	IF_TYPE_SEND_MODUL_INFO					= (40 << 8),
	IF_TYPE_STOP_MODUL_INFO					= (41 << 8),
	IF_TYPE_SET_RECV_MODEL					= (42 << 8),
	IF_TYPE_STOP_RECV_MODEL					= (43 << 8),
	IF_TYPE_WIRELESS_SET_KEY				= (44 << 8),
	IF_TYPE_GET_TAG_INFO					= (45 << 8),
	IF_TYPE_SLEEP_EX_ONES_TAG				= (46 << 8),
	IF_TYPE_SLEEP_EX_ONE_TAG				= (47 << 8),
	IF_TYPE_SET_CHANNEL_TAG					= (48 << 8),
	IF_TYPE_SET_POWER_TAG					= (49 << 8),
	IF_TYPE_SEARCH_RECORDS					= (50 << 8),
	IF_TYPE_UPDATE_PWD						= (51 << 8),
	IF_TYPE_MONITOR_CONF					= (52 << 8),
	IF_TYPE_MONITOR_START					= (53 << 8),
	IF_TYPE_MONITOR_STOP					= (54 << 8),
	IF_TYPE_MONITOR_REPORT					= (55 << 8),
	IF_TYPE_UPDATE_ADMIN_PWD				= (56 << 8),
	IF_TYPE_GET_TAGE_SAFE_PARAM				= (57 << 8),
	IF_TYPE_REQUIRE_RANDOM					= (58 << 8),
	IF_TYPE_DATA_BOARDCAST					= (59 << 8),
	IF_TYPE_COLLECT_SPEC_CONT				= (60 << 8),
	IF_TYPE_SEARCH_FILE						= (61 << 8),
	IF_TYPE_WAKEUP_TAG                      = (62 << 8),
}INTERFACE_TYPE;

typedef enum _CLR_TAG_STATE{
	CLR_ALL_REGISTER_TAG_STATE_SUCCESS,
	CLR_ALL_REGISTER_TAG_STATE_FAILED,
	CLR_ALL_REGISTER_TAG_STATE_TIMEOUT
}CLR_ALL_TAG_STATE;

typedef enum _GET_ALL_TAG_STATE{
	GET_ALL_TAG_STATE_SUCCESS,
	GET_ALL_TAG_STATE_FINISH,
	GET_ALL_TAG_STATE_FAILD,
	GET_ALL_TAG_STATE_TIMEOUT
}GET_ALL_TAG_STATE;

typedef enum _REGISTER_TAG_STATE{
	REGISTER_TAG_STATE_SUCCESS,
	REGISTER_TAG_STATE_FAILED,
	REGISTER_TAG_STATE_TIMEOUT
}REGISTER_TAG_STATE;

typedef enum _INIT_TAG_STATE{
	INIT_TAG_STATE_SUCCESS,
	INIT_TAG_STATE_FAILED,
	INIT_TAG_STATE_TIMEOUT
}INIT_TAG_STATE;

//����ǩ��������ִ��״̬
typedef enum _WAKEUP_ONE_TAG_STATE{
	WAKEUP_ONE_TAG_STATE_SUCCESS,
	WAKEUP_ONE_TAG_STATE_NOT_FOUND,
	WAKEUP_ONE_TAG_STATE_TIMEOUT
}WAKEUP_ONE_TAG_STATE;

typedef enum _SET_RADER_INFO_STATE{
	SET_READER_INFO_STATE_SUCCESS,
	SET_READER_INFO_STATE_FAILED,
	SET_READER_INFO_TIMEOUT
}SET_RADER_INFO_STATE;

typedef enum _GET_RADER_INFO_STATE{
	GET_READER_INFO_STATE_SUCCESS,
	GET_READER_INFO_STATE_FAILED,
	GET_READER_INFO_TIMEOUT
}GET_RADER_INFO_STATE;

typedef enum _SET_CHANNEL{
	SET_CHANNEL_SUCCESS,
	SET_CHANNEL_FAILED,
	SET_CHANNEL_TIMEOUT,
	SET_CHANNEL_OVERRANGE
}SET_CHANNEL;

typedef enum _KILL_TAG_STATE{
	KILL_TAG_SUCCESS,
	KILL_TAG_FAILED,
	KILL_TAG_TIMEOUT
}KILL_TAG_STATE;

typedef enum _WRITECACHE_SYNC_STATE{
	WRITECACHE_SYNC_SUCCESS,
	WRITECACHE_SYNC_FAILED,
	WRITECACHE_SYNC_TIMEOUT
}WRITECACHE_SYNC_STATE;

typedef enum _WRITE_CACHE_STATE{
	WRITE_CACHE_SUCCESS,
	WRITE_CACHE_FAILED,
	WRITE_CACHE_TIMEOUT,
	WRITE_CACHE_OVERRANGE
}WRITE_CACHE_STATE;

typedef enum _SEND_CACHE_STATE{
	SEND_CACHE_SUCCESS,
	SEND_CACHE_FAILED,
	SEND_CACHE_TIMEOUT
}SEND_CACHE_STATE;

typedef enum _CLEAN_CACHE_STATE{
	CLEAN_CACHE_SUCCESS,
	CLEAN_CACHE_FAILED,
	CLEAN_CACHE_TIMEOUT
}CLEAN_CACHE_STATE;

typedef enum _SLEEP_TAG_STATE{
	SLEEP_TAG_SUCCESS,
	SLEEP_TAG_FAILED,
	SLEEP_TAG_TIMEOUT
}SLEEP_TAG_STATE;

typedef enum _CONNECT_TAG_STATE{
	CONNECT_TAG_SUCCESS,
	CONNECT_TAG_FAILED,
	CONNECT_TAG_TIMEOUT
}CONNECT_TAG_STATE;

typedef enum _SET_POWER_STATE{
	SET_POWER_STATE_SUCCESS,
	SET_POWER_STATE_FAILED,
	SET_POWER_STATE_TIMEOUT,
	SET_POWER_STATE_OVERRANGE
}SET_POWER_STATE;

typedef enum _SEND_PUREWAVE_STATE{
	SEND_PUREWAVE_STATE_SUCCESS,
	SEND_PUREWAVE_STATE_FAILED,
	SEND_PUREVAVE_STATE_TIMEOUT
}SEND_PUREWAVE_STATE;

typedef enum _STOP_PUREWAVE_STATE{
	STOP_PUREWAVE_STATE_SUCCESS,
	STOP_PUREWAVE_STATE_FAILED,
	STOP_PUREWAVE_STATE_TIMEOUT
}STOP_PUREWAVE_STATE;

typedef enum _SEND_MODUL_INFO_STATE{
	SEND_MODUL_INFO_STATE_SUCCESS,
	SEND_MODUL_INFO_STATE_FAILED,
	SEND_MODUL_INFO_STATE_TIMEOUT
}SEND_MODUL_INFO_STATE;

typedef enum _STOP_MODUL_INFO_STATE{
	STOP_MODUL_INFO_STATE_SUCCESS,
	STOP_MODUL_INFO_STATE_FAILED,
	STOP_MODUL_INFO_STATE_TIMEOUT
}STOP_MODUL_INFO_STATE;

typedef enum _SET_RECV_MODEL_STATE{
	SET_RECV_MODEL_STATE_SUCCESS,
	SET_RECV_MODEL_STATE_FAILED,
	SET_RECV_MODEL_STATE_TIMEOUT
}SET_RECV_MODEL_STATE;

typedef enum _STOP_RECV_MODEL_STATE{
	STOP_RECV_MODEL_STATE_SUCCESS,
	STOP_RECV_MODEL_STATE_FAILED,
	STOP_RECV_MODEL_STATE_TIMEOUT
}STOP_RECV_MODEL_STATE;

typedef enum _SET_KEY_STATE{
	SET_KEY_STATE_SUCCESS,
	SET_KEY_STATE_FAILED,
	SET_KEY_STATE_TIMEOUT
}SET_KEY_STATE;

typedef enum _SLEEP_EX_ONES_TAG_STATE{
	SLEEP_EX_ONES_TAG_STATE_SUCCESS,
	SLEEP_EX_ONES_TAG_STATE_FAILED,
	SLEEP_EX_ONES_TAG_STATE_TIMEOUT
}SLEEP_EX_ONES_TAG_STATE;

typedef enum _SLEEP_EX_ONE_TAG_STATE{
	SLEEP_EX_ONE_TAG_STATE_SUCCESS,
	SLEEP_EX_ONE_TAG_STATE_FAILED,
	SLEEP_EX_ONE_TAG_STATE_TIMEOUT
}SLEEP_EX_ONE_TAG_STATE;

typedef enum _WIRELESS_SET_KEY_STATE{
	WIRELESS_SET_KEY_STATE_SUCCESS,
	WIRELESS_SET_KEY_STATE_FAILED,
	WIRELESS_SET_KEY_STATE_TIMEOUT
}WIRELESS_SET_KEY_STATE;

typedef enum _GET_TAG_INFO_STATE{
	GET_TAG_INFO_STATE_SUCCESS,
	GET_TAG_INFO_STATE_FAILED,
	GET_TAG_INFO_STATE_TIMEOUT
}GET_TAG_INFO_STATE;

typedef enum _SEARCH_RECORDS_STATE{
	SEARCH_RECORDS_STATE_SUCCESS,
	SEARCH_RECORDS_STATE_FAILED,
	SEARCH_RECORDS_STATE_TIMEOUT
}SEARCH_RECORDS_STATE;

typedef enum _UPDATE_PWD_STATE{//����ϵͳ���������
	UPDATE_PWD_STATE_SUCCESS,
	UPDATE_PWD_STATE_FAILED,
	UPDATE_PWD_STATE_TIMEOUT,
	UPDATE_PWD_STATE_ADMIN_PWD_NOT_RIGHT,//����Ա���벻ƥ��
	UPDATE_PWD_STATE_INVALID_INDEX_OR_MODE,//������������ģʽ���Ϸ�
	UPDATE_PWD_STATE_INVALID_NEW_PWD,//�¿���Ϸ�
	UPDATE_PWD_STATE_NOT_AUTHORISED,//���ڰ�ȫ�Ự
	UPDATE_PWD_STATE_UNKNOWN,//δ֪����
}UPDATE_PWD_STATE;

//���¹���Ա�������״̬
typedef enum _UPDATE_ADMIN_PWD_STATE{
	UPDATE_ADMIN_PWD_STATE_SUCCESS,				//0
	UPDATE_ADMIN_PWD_STATE_FAILED,				//1
	UPDATE_ADMIN_PWD_STATE_TIMEOUT,				//2
	UPDATE_ADMIN_PWD_STATE_NOT_AUTHORISED, //���ڰ�ȫ�Ự��3
	UPDATE_ADMIN_PWD_STATE_PWD_ERROR	//ԭ�������4
}UPDATE_ADMIN_PWD_STATE;

//���ü�����ڷ���״̬
typedef enum _CONF_MONITOR_STATE
{
	CONF_MONITOR_SUCCESS,				//0
	CONF_MONITOR_UNKNOWN,				//δ֪����
	CONF_MONITOR_NOT_MATCH,				//��������ʶ����ƥ��
	CONF_MONITOR_OVERRANGE,				//����Խ��
	CONF_MONITOR_FAILED,				//����ʧ��
	CONF_MONITOR_TIMEOUT				//���ջظ���ʱ
}CONF_MONITOR_STATE;

typedef enum _GET_MONITOR_REPORT_STATE
{
	GET_MONITOR_REPORT_SUCCESS,
	GET_MONITOR_REPORT_TIMEOUT,
	GET_MONITOR_REPORT_PARAM_ERROR
}GET_MONITOR_REPORT_STATE;

typedef enum
{
	MONITOR_START_STATE_SUCCESS,
	MONITOR_START_STATE_TIMEOUT
}MONITOR_START_STATE;

typedef enum 
{
	MONITOR_STOP_STATE_SUCCESS,
	MONITOR_STOP_STATE_TIMEOUT
}MONITOR_STOP_STATE;

typedef enum _GET_TAG_SAFE_PARAM_STATE{
	GET_TAG_SAFE_PARAM_STATE_SUCCESS,
	GET_TAG_SAFE_PARAM_STATE_FAILED,
	GET_TAG_SAFE_PARAM_STATE_TIMEOUT
}GET_TAG_SAFE_PARAM_STATE;

typedef enum _REQUIRE_RANDOM_STATE{
	REQUIRE_RANDOM_STATE_SUCCESS,
	REQUIRE_RANDOM_STATE_FAILED,
	REQUIRE_RANDOM_STATE_TIMEOUT
}REQUIRE_RANDOM_STATE;

typedef enum _DATA_BOARDCAST_STATE{
	DATA_BOARDCAST_STATE_SUCCESS,
	DATA_BOARDCAST_STATE_TIMEOUT
}DATA_BOARDCAST_STATE;

// typedef enum _TEST_COMMAND{
// 	//�Ķ���������
// 	RD_FREQUENCY_TEST,				//�Ķ�������Ƶ�ʼ�׼ȷ�Ȳ���
// 	RD_BANDWIDTH_TEST,				//�Ķ����ŵ��������
// 	RD_CHANNEL_LEAK_TEST,			//�Ķ������ŵ�����й©�Ȳ���
// 	RD_MAXPOWER_TEST,				//�Ķ�������书�ʲ���
// 	RD_RECV_SENSITIVITY_TEST,		//�Ķ������������Ȳ���
// 
// 	//��ǩ������
// 	TAG_FREQUENCY_TEST = 10,				//��ǩ����Ƶ�ʲ���
// 	TAG_RECV_SENSITIVITY_TEST,				//��ǩ���������Ȳ���
// 	TAG_EQULERAD_PWR_TEST,					//��ǩ��Ч���书�ʲ���
// 	TAG_SEND_BANDWIDTH_TEST					//��ǩ����ռ���ŵ�����
// }TEST_COMMAND;//��������

//----------------Test ����״̬-----------------------
typedef enum _TEST_SET_PARAM_STATE{
	TEST_SET_PARAM_STATE_SUCCESS,
	TEST_SET_PARAM_STATE_FAILED,
	TEST_SET_PARAM_STATE_TIMEOUT
}TEST_SET_PARAM_STATE;

typedef enum _TEST_FREQ_ACCURACY_STATE{
	TEST_FREQ_ACCURACY_STATE_SUCCESS,
	TEST_FREQ_ACCURACY_STATE_FAILED,
	TEST_FREQ_ACCURACY_STATE_TIMEOUT
}TEST_FREQ_ACCURACY_STATE;

typedef enum _TEST_MIN_RCVOR_BAND_STATE{
	TEST_MIN_RCVOR_BAND_STATE_SUCCESS,
	TEST_MIN_RCVOR_BAND_STATE_FAILED,
	TEST_MIN_RCVOR_BAND_STATE_TIMEOUT
}TEST_MIN_RCVOR_BAND_STATE;

typedef enum _TEST_SWITCH_TIME_STATE{
	TEST_SWITCH_TIME_STATE_SUCCESS,
	TEST_SWITCH_TIME_STATE_FAILED,
	TEST_SWITCH_TIME_STATE_TIMEOUT
}TEST_SWITCH_TIME_STATE;

typedef enum _TEST_MODUL_MODE_STATE{
	TEST_MODUL_MODE_STATE_SUCCESS,
	TEST_MODUL_MODE_STATE_FAILED,
	TEST_MODUL_MODE_STATE_TIMEOUT
}TEST_MODUL_MODE_STATE;

typedef enum _TEST_FSSS_SEQUENCE_STATE{
	TEST_FSSS_SEQUENCE_STATE_SUCCESS,
	TEST_FSSS_SEQUENCE_STATE_FAILED,
	TEST_FSSS_SEQUENCE_STATE_TIMEOUT
}TEST_FSSS_SEQUENCE_STATE;

typedef enum _TEST_BIT_SPEED_STATE{
	TEST_BIT_SPEED_STATE_SUCCESS,
	TEST_BIT_SPEED_STATE_FAILED,
	TEST_BIT_SPEED_STATE_TIMEOUT
}TEST_BIT_SPEED_STATE;

typedef enum _TEST_BEFORE_CODE_STATE{
	TEST_BEFORE_CODE_STATE_SUCCESS,
	TEST_BEFORE_CODE_STATE_FAILED,
	TEST_BEFORE_CODE_STATE_TIMEOUT
}TEST_BEFORE_CODE_STATE;

typedef enum _TEST_PREVCOLLI_MANAGE_STATE{
	TEST_PREVCOLLI_MANAGE_STATE_SUCCESS,
	TEST_PREVCOLLI_MANAGE_STATE_FAILED,
	TEST_PREVCOLLI_MANAGE_STATE_TIMEOUT
}TEST_PREVCOLLI_MANAGE_STATE;

typedef enum _TEST_RCV_SENSITIVITY_STATE{
	TEST_RCV_SENSITIVITY_STATE_SUCCESS,
	TEST_RCV_SENSITIVITY_STATE_FAILED,
	TEST_RCV_SENSITIVITY_STATE_TIMEOUT
}TEST_RCV_SENSITIVITY_STATE;

typedef enum _TEST_TAG_SWITCH_TIME_STATE{
	TEST_TAG_SWITCH_TIME_STATE_SUCCESS,
	TEST_TAG_SWITCH_TIME_STATE_FAILED,
	TEST_TAG_SWITCH_TIME_STATE_TIMEOUT
}TEST_TAG_SWITCH_TIME_STATE;

//typedef enum _CREATE_FILE_STATE
//----------------------PCͨ����������ṹ��------------------------
#pragma pack(1)
typedef struct{
	unsigned short	sigStart;
	PC_CMD_TYPE		cmdType;
	union{
		//------------ָ���ļ�id����д����---------
		struct{
			unsigned char readBuffer[10];
			unsigned char writeBuffer[10];
			unsigned short fileId;
		}WRITE_OR_READ_FILE;
		//------------����-------------
		struct{
			int wakeupNum;
		}WAKEUP_TAG_PARAM;
		//------------�̴�-------------
		struct{
			unsigned int readyToken;
			unsigned char retryTimes;
		}INVENTORY_TAG_PARAM;
		//--------�̴�ָ�����ݱ�ǩ----------
		struct{
			unsigned int readyToken;
			unsigned char retryTimes;
			unsigned short fileID;
			SEARCH_ELEMENT searchElement;//ָ������
			unsigned char datalen;//ָ�����ݵĳ���
		}INVENTORY_SPECCONT_TAG_PARAM;
		//--------------ѡ���ǩ�������-------------
		struct{
			unsigned char	TID[8];
		}SELECT_TAG_PRAM;
		/*---------------ѡ���ļ�����---------------*/
		struct{
			unsigned char	TID[8];
			unsigned short	fileID;
		}SELECT_FILE_PARAM;/**/
		//----------���ļ�����--------------
		struct{
			unsigned char fileType;
			union{
				struct{
					unsigned short fileOffset;
					unsigned char  dataLen;
				}transBinFile;
				struct{
					unsigned char recPointer;
					unsigned char opMode;
					unsigned char recOffset;
					unsigned char datLen;
				}recEF;
			}uReadPram;
		}READ_FILE_PARAM;
		/*--------------------�����ļ�����--------------*/
		struct{
			FILE_ATTRIBUTE_R fileAttrR;
			unsigned short fileOffset;
			unsigned char	dataLen;
		}READ_LARGE_FILE_PARAM;
		//---------------------д�ļ�����---------------------
		struct{
			unsigned char fileType;
			union{
				struct{
					unsigned short fileOffset;//2Byte
					unsigned short pkgIndex;
					unsigned char  dataLen;//1Byte
					char  dataBuffer[CMD_WRITE_TRANS_MAX_LEN];//CMD_P2P_PARA_MAXLEN-2-1
				}transBinEF;
				struct{
					unsigned char recPointer;//1Byte
					unsigned char opMode;//1Byte
					unsigned char dataLen;//1Byte
					char dataBuffer[CMD_P2P_PARA_MAXLEN-1-1-1];
				}recEF;
			}uWriteParam;
		}WRITE_FILE_PARAM;
		//-------------д���ļ������ݴ����Ķ���Flash��--------------
		struct{
			unsigned short fileOffset;//2Byte
			unsigned char  dataLen;//1Byte
			char  dataBuffer[MAX_WRITE_CACHE_LEN];
		}WRITE_CACHE_PARAM;
		//-----------�����ļ�����----------------
		struct{
			FILE_ATTRIBUTE_W fileAttr;
		}CREATE_FILE_PARAM;
		//-------------��Ч��Ч�ļ�-----------
		struct {
			unsigned char newFileState;//0:��״̬Ϊ��Ч,1:��״̬Ϊ��Ч.
		}VALID_FILE_PARAM;
		//-------------�б��ļ�-------------
		struct{
			unsigned char lsitMode;/*�ֽ׶�ֻ֧��ģʽ1*/
			unsigned char dfRecPointer;
			unsigned char opMode;/*�ļ�ϵͳ�б��ļ�ģʽ*/
		}LIST_FILE_PARAM;
		//------------��֤����--------------
		struct{
			unsigned char	pwdType;
			unsigned int	passWord;
		}VERIFY_PWD_FILE_PARAM;
		//-----------�������б�ǩ------------
		struct{
			unsigned int sleepCode;
			unsigned char sleepTime;
			unsigned char listenTime;
		}SLEEP_ALL_TAG_PARAM;
		//-----------���߳�ĳһ���ǩ�����б�ǩ------------
		struct{
			unsigned int sleepCode;
			unsigned short fileID;
			unsigned char offset;
			unsigned char dataLen;
			char dataBuffer[80];	//���ȿ��޸ģ��ݶ�80
			unsigned char sleepTime;
			unsigned char listenTime;
		}SLEEP_EX_ONES_TAG_PARAM;
		//-----------���߳�ĳ����ǩ�����б�ǩ------------
		struct{
			unsigned char	TID[8];
			unsigned int sleepCode;
			unsigned char sleepTime;
			unsigned char listenTime;
		}SLEEP_EX_ONE_TAG_PARAM;
		//-----------�����������--------------
		struct{
			unsigned short FileId;
			unsigned char FileOffset;
			unsigned char DataLen;
			unsigned short TimeSlotNum;
			unsigned short ReportedCycle;
			unsigned short MonitorCycle;
			unsigned short TimeSlotWidth;
			unsigned short TimeSlotSum;
			unsigned char ReportIndication;
			unsigned char Tid[8];
		}MONITOR_TRCONF_PARAM;
		//-----------�����ʼ����--------------
		struct{
			unsigned short CycleCnt;//������ڴ���
			unsigned short unUsed;
		}MONITOR_TRSTART_PARAM;
		//-----------������ʹ������-----------------
		struct{
			unsigned short FileId;
			unsigned char  SensorId;
			unsigned char  SensorState;
		}MONITOR_SENSOREN_PARAM;
		//------------��ȫ�Ự����-----------------
		struct{
			unsigned char	TID[8];
		}SAFE_SESSION_PARAM;
		//--------------ע���ǩ����---------------
		struct{
			unsigned char keyType; //0ΪAES��1ΪECC
			unsigned char  TID[8];
			union{
				struct{
					unsigned char  MEK[32];
					unsigned char  MIK[32];
				}AES_KEY;
				struct{
					unsigned char pubKeyTag[PKI_PUBLICKEY_LEN];
				}ECC_KEY;
			}u;
		}REGISTER_TAG_PARAM;
		//--------------��ʼ����ǩ�������---------
		struct{
			union{
				TAG_ATTRIBUTE tagAttr;
				struct{
					unsigned char priKeyTag[PKI_PRIVATEKEY_LEN];				//�˴��޸�Ϊ��168
					unsigned char pubKeyReader[PKI_PUBLICKEY_LEN];
				}ECC_KEY;
			}ATTR;
			char paraType;//0���������� 1��ECC_key����
		}TAG_ATTR_PARAM;
		//---------------���ѵ�����ǩ----------------
		struct{
			unsigned char	TID[8];
			unsigned int	readyToken;
		}WAKEUP_ONE_TAG_PARAM;
		//----------------�����Ķ���---------------
		struct{
			union{
				READER_INFO_W readerInfo_w;
				unsigned char priKeyReader[PKI_PRIVATEKEY_LEN];
			}u;
			char paraType;//0���������� 1��ECC_key����
		}SET_READER_INFO_PARAM;
		//----------------������д��Կ----------------
		struct{
			char paraType;	// 0:��д�Ķ�����Կ��1:��д��ǩ��Կ
			struct{
				unsigned char priKey[PKI_PRIVATEKEY_LEN];
				unsigned char pubKey[PKI_PUBLICKEY_LEN];
			}ECC_KEY;
		}WIRELESS_SET_KEY;
		struct{
			unsigned short	fileID;
		}FILE_DELETE_PARAM;
		struct{
			unsigned char  TID[8];
			unsigned int formatPwd;
		}FILESYS_FORMAT_PARAM;
		//-----------------�����ŵ�------------------
		struct{
			int Channel;
		}SELECTED_CHANNEL;
		//--------------����PC��Tag֮�������----------
		struct{
			unsigned char connectTag;//c
		}CONNECT_TAG;
		//--------------�Ͽ�PC��Tag֮�������----------
		struct{
			unsigned char disconnectTag;//d
		}DISCONNECT_TAG;
		//--------------���ù���----------------------
		struct{
			int Power;
		}SELECTED_POWER;
		//--------------������¼-------------------
		struct{
			unsigned short dataLen;
			unsigned char dataBuf[80];		//һ������֡��󳤶�80
		}SEARCH_RECORDS;
		//--------------����͸��-------------------
		struct{
			unsigned short dataLen;
			unsigned char dataBuf[80];		//һ������֡��󳤶�80
		}SEARCH_TRANS;
		//------------����ϵͳ����--------------
		struct{
			unsigned char Tid[9];
			unsigned int adminPWD;//�ṩ����Ա����
			unsigned short indexPWD;//��������
			unsigned char mode;//indexΪ�ļ���ʾ��ʱʹ�ã������������
			unsigned int newPWD;//������
		}UPDATE_PWD;
		//------------���¹���Ա����------------
		struct{
			unsigned int oldPWD;//������
			unsigned int newPWD;//������
		}UPDATE_ADMIN_PWD;
		//---------------��ǩ���-----------------
		struct{
			unsigned int killTagToken;
		}KILL_TAG_PARAM;
		//---------------���������---------------
		struct{
			unsigned char randomLenth;
		}REQUIRE_RANDOM_PARAM;
		//--------------���ݹ㲥---------------
		struct{
			unsigned int readyToken;
			unsigned char boardcastData[100];
		}DATA_BOARDCAST_PARAM;


//----------------------Test-----------------------------
		//-------------��������----------------
		struct{
			int channel;
			int power;
			int Q;
			int N;
		}TEST_SET_PARAM;
	}uParam;
	unsigned short	sigEnd;
}PC_CMD_DATA,*PPC_CMD_DATA;
/*********************************************************************************************/
typedef struct{
	unsigned char	TID[8];
	unsigned short	tagState;//����
	char	UII[32];
}TAG_INFO;

typedef struct{
	char	TID[8];
}TID_INFO;

typedef struct{
	unsigned char	TID[8];		//��ǩID
	unsigned short	fileOffset;	//ƫ��
	unsigned short	fileLength;	//�ļ�����
	unsigned char	Data[80];	//���� 127-16-2-5=104(��ȫ֡) 16�ֽڶ���
}CYCLE_INFO;

typedef struct{
	unsigned short  SIG_START;
	unsigned short  packageIndex;
	unsigned short  packageNums;
	PC_CMD_TYPE		cmdType;
	union{
		//-------------------------------------
		struct{
			BYTE readBuffer[10];
			SEARCH_FILE_STATE RepState;
		}searchFileParam;
		//����
		struct{
			WAKEUP_TAG_STATE WakeupState;
		}wakeupParam;
		struct{
			unsigned short	numsOfAllTag;
			unsigned short  curNumOfTag;
			TAG_INFO		tagInfo[2];
			COLLECT_TAG_STATE collectTagState;
		}collectParam;
		//--------------------------------------
		struct{
			SELECT_TAG_STATE selectTagState;
		}selectTagParam;
		//--------------------------------------
		struct{
			SELECT_FILE_STATE selectFileState;
			FILE_ATTRIBUTE_R fileAttr_R;
		}selectFileParam;
		//--------------------------------------
		struct{
			LIST_FILE_STATE listState;
			FILE_ATTRIBUTE_R  fileAttr_R;
			unsigned int allListFileNums;				//���������б��ļ�����
		}listFileParam;
		//--------------------------------------
		struct{
			READ_FILE_STATE readFileState;
			unsigned short retLen;
			char buff[109];
		}readFileParam;
		struct{
			READ_FILE_STATE readFileState;
			unsigned short retLen;
			char buff[80];
		}readLargeFileParam;
		//---------------------------------------
		struct{
			WRITE_FILE_STATE writeFileState;
		}writeFileParam;
		struct{
			CREATE_FILE_STATE createFileState;
		}createFileParam;
		struct{
			VALID_FILE_STATE validFileState;
		}validFileParam;
		struct  
		{
			unsigned char  pwdType;
			VERIFY_PWD_FILE_STATE  verifyState;
		}verifyPwdFileParam;
		//--------------������ʹ������---------------
		struct{
			MONITOR_SENSOREN_STATE sensorEnState;
		}monitorSensorEnParam;
		struct{
			SAFE_SESSION_STATE safeSessionState;
		}safeSessionStateParam;
		struct{
			REGISTER_TAG_STATE registerTagState;
		}registerTagStateParam;
		//------------���ñ�ǩ������Ϣ����ʼ����ǩ��-----------
		struct{
			INIT_TAG_STATE initTagState;
		}initTagStateParam;
		struct{
			CLR_ALL_TAG_STATE clrAllTagInfoState;
		}clrAllRegsitedTagParam;
		//---------��ȡ����ע���ǩ��Ϣ-------------
		struct{
			GET_ALL_TAG_STATE getAllTagState;
			unsigned int allTagNums;
			unsigned int curTagNums;
			TID_INFO tidArray[8];
		}getAllTagParam;
		//-----------------����ǩ����---------------------
		struct{
			WAKEUP_ONE_TAG_STATE wakeupOneTagState;
			TAG_INFO		tagInfo;
		}wakeupOneTagParam;
		//------------------��ȡ�Ķ�����Ϣ-----------------
		struct{
			GET_RADER_INFO_STATE getReaderInfoSate;
			READER_INFO readerInfo;
		}getReaderInfoParam;
		//----------------�����Ķ�����Ϣ--------------------
		struct{
			SET_RADER_INFO_STATE setReaderInfoState;
			//unsigned char  TID[8];
		}setReaderInfoParam;
		//-----------------------�ļ�ɾ��------------------
		struct{
			FILE_DELETE_STATE fileDeleteState;
		}fileDeleteParam;
		//-----------------------�ļ���ʽ��----------------
		struct{
			FILESYS_FORMAT_STATE filesysFormatState;
		}filesysFormatParam;	
		//---------------�����ŵ�------------------------
		struct{
			SET_CHANNEL setChannelState;
		}setChannelParam;
		//---------------����PC��Tag֮�������----------------
		struct{
			unsigned char connectTag;//o
			CONNECT_TAG_STATE connectTagState;
		}connectTagParam;
		//--------------�Ͽ�PC��Tag֮�������----------
		struct{
			unsigned char disconnectTag;//k
		}disconnectTagParam;
		//--------------��ǩ���------------------
		struct{
			KILL_TAG_STATE killTagstate;
		}killTagParam;
		//-------------д����ͬ��--------------
		struct{
			WRITECACHE_SYNC_STATE writecacheSyncState;
		}writecacheSyncParam;
		//--------��д���ļ������ݴ����Ķ���Flash��------
		struct{
			WRITE_CACHE_STATE writeCacheState;
		}writeCacheParam;
		//--------���Ķ���Flash�е����ݷ��͸���ǩ--------
		struct{
			SEND_CACHE_STATE sendCacheState;
		}sendCacheParam;
		//------------�������----------------
		struct{
			CLEAN_CACHE_STATE cleanCacheState;
		}cleanCacheParam;
		//----------------�������б�ǩ--------------------
		struct{
			SLEEP_TAG_STATE sleepTagState;
		}sleepTagParam;
		//---------------���߳�ĳһ���ǩ�����б�ǩ---------------
		struct{
			SLEEP_EX_ONES_TAG_STATE sleepExOnesTagState;
		}sleepExOnesTagParam;
		//---------------���߳�ĳ����ǩ�����б�ǩ---------------
		struct{
			SLEEP_EX_ONE_TAG_STATE sleepExOneTagState;
		}sleepExOneTagParam;
		//------------���ù���-----------------
		struct{
			SET_POWER_STATE setPowerState;
		}setPowerParam;
		struct{
			SEND_PUREWAVE_STATE sendPurewaveState;
		}sendPurewaveParam;
		struct{
			STOP_PUREWAVE_STATE stopPurewaveState;
		}stopPurewaveParam;
		struct{
			SEND_MODUL_INFO_STATE sendModulInfoState;
		}sendModulInfoParam;
		struct{
			STOP_MODUL_INFO_STATE stopModulInfoState;
		}stopModulInfoParam;
		struct{
			char clk_pin, code_pin;
			SET_RECV_MODEL_STATE setRecvModelState;
		}setRecvModelParam;
		struct{
			STOP_RECV_MODEL_STATE stopRecvModelState;
		}stopRecvModelParam;
		
		//-------------TEST--------------
		struct{
			TEST_SET_PARAM_STATE testSetParamState;
		}testSetParam;
		struct{
			TEST_FREQ_ACCURACY_STATE testFreqAccuacyState;
		}testFreqAccuracyParam;
		struct{
			TEST_MIN_RCVOR_BAND_STATE testMinRcvorBandState;
		}testMinRcvorBandParam;
		struct{
			TEST_SWITCH_TIME_STATE testSwitchTimeState;
		}testSwitchTimeParam;
		struct{
			TEST_MODUL_MODE_STATE testModulModeState;
		}testModulModeParam;
		struct{
			TEST_FSSS_SEQUENCE_STATE testFsssSequencyState;
		}testFsssSequencyParam;
		struct{
			TEST_BIT_SPEED_STATE testBitSpeedState;
		}testBitSpeedParam;
		struct{
			TEST_BEFORE_CODE_STATE testBeforeCodeState;
		}testBeforeCodeParam;
		struct{
			TEST_PREVCOLLI_MANAGE_STATE testPreColliManageState;
			int CollectTime;	//��λms
			int CollectedNum;
		}testPreColliManageParam;
		struct{
			TEST_RCV_SENSITIVITY_STATE testRcvSensitivityState;
		}testRcvSensitivityParam;
		struct{
			TEST_TAG_SWITCH_TIME_STATE testTagSwitchTimeState;
		}testTagSwitchTimeParam;
		struct{
			SET_KEY_STATE setKeyState;
		}setKeyParam;
		//-------------------������д��Կ-------------------
		struct{
			WIRELESS_SET_KEY_STATE wirelessSetKeyState;
		}wirelessSetKeyParam;
		//--------------------��ȡ��ǩ��Ϣ-------------------
		struct{
			GET_TAG_INFO_STATE getTagInfoState;
			TAG_ATTRIBUTE_R tagAttribute;
		}getTagInfoParam;
		struct{
			SEARCH_RECORDS_STATE searchRecordsState;
			unsigned short retRecords;
		}searchRecordsParam;
		//---------------����ϵͳ����-----------------
		struct{
			UPDATE_PWD_STATE upDatePwdState;
		}updatePWDParam;
		//---------------���¹���Ա����-----------------
		struct{
			UPDATE_ADMIN_PWD_STATE updateAdminPwdState;
		}updateAdminPwdParam;
		//-----------------�����������---------------------
		struct{
			CONF_MONITOR_STATE confMonState;
		}confMonParam;
		//----------------�����ʼ�ظ�-----------------------
		struct{
			MONITOR_START_STATE monitorStartState;
		}monitorStartParam;
		//-----------------������ڱ�ǩ�㱨---------------------
		struct{
			GET_MONITOR_REPORT_STATE State;
			CYCLE_INFO	cycleInfo;
		}MonRepParam;
		//-----------------�������ֹͣ--------------------
		struct{
			MONITOR_STOP_STATE monitorStopState;
		}monitorStopParam;
		//----------------��ȡ��ǩ��ȫ����------------------
		struct{
			GET_TAG_SAFE_PARAM_STATE getTagSafeParamState;
			unsigned short keyParam;
			unsigned short safePowerParam;
		}getTagSafeParam;
		//---------------���������-----------------
		struct{
			REQUIRE_RANDOM_STATE requireRandomState;
			unsigned char randomNums[8];
		}requireRandomParam;
		//---------------���ݹ㲥-----------------
		struct{
			DATA_BOARDCAST_STATE dataBoardcastState;
		}dataBoardcastParam;

		struct{
			TEST_RADIO_STATE  testRadioState;
			unsigned char Data[128];
		}testSingleCmd;//���Ե������
	}uReplyParam;
	unsigned short SIG_END;
}PC_CMD_REPLY,*PPC_CMD_REPLY;
#pragma pack()
#define CMD_LEN sizeof(PC_CMD_DATA)
#define CMD_REPLY_LEN sizeof(PC_CMD_REPLY)
void pcCmdInterfaceInit(void);
#endif
