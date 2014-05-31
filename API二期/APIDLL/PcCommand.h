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

#define READER_SET_KEY	0			//烧写阅读器
#define TAG_SET_KEY		1			//烧写标签

#define FILE_ID_MF		( 0x3f00 )
#define FILE_ID_TID		( 0xa101 )
#define FILE_ID_UII		( 0xa102 )
#define FILE_ID_CONF	( 0xa103 )


#define CMD_LIST_MODE_NORMAL 1	/*列表操作由PC机来完成具体的逻辑,而阅读器只是按照国军标进行转发,现阶段只支持1*/
#define CMD_LIST_MODE_ALL_IN_ONE 0 /*列表文件由阅读器一次完成后,统一返回给PC端*/

#define		ID_BTN_TOOLBAR1		501
#define		ID_BTN_TOOLBAR2 	502
/************************文件系统操作相关宏定义************************/
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
#define		RECORD_IN_FILE_READ_MODE_NEXT			2  //读取文件的下一条记录
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
#define		MAX_FILE_LENGTH_IN_TAG		65500		//标签文件最大值

/*	NOTE THAT:
 *	根据安全协议帧要求,一个有效的数据帧在读文件时由原来的最长的109字节,减少为现在为安全帧长度
 *
 */
#define		MAX_READ_LARGE_LEN	80//109
#define		MAX_READ_LARGE_LEN_S 80//109

#define		MIN_READ_LARGE_LEN 2000//大文件读的定义最小文件大小

/**************************宏:记录文件搜索相关宏定义************************/
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
	BYTE usbFlag;//USB标识位，必须初始化为0
	BYTE pkgNUM;//分包序号
	BYTE endFlag;//分包结束标志。0表示没有结束，1表示结束
	BYTE dataLen;//每个分包的长度
	char dataBuf[60];
}MULTI_PACKET_SEND_RECV,*pMULTI_PACKET_SEND_RECV;

typedef struct _file_attribute {
	unsigned short	fileID;
	unsigned char	fileTypeValid; //文件类型有效无效变量
	union{
		struct{
			unsigned short constMaxFileLen;
			unsigned short numsOfMaxFile;
			unsigned short numsOfRealFile;
		}MFDF;
		struct{
			unsigned short maxFileLen;//最大文件长度
			unsigned short constMaxRecord;//默认总为0001h
			unsigned short realFileLen;//实际文件长度
		}transBinEF;
		struct{
			unsigned short recordLen;//记录长度
			unsigned short maxRecordNums;//最大文件记录数
			unsigned short realRecordNums;//实际记录数
		}recEF;
	}uFileAttr;
	unsigned char securityType;
	
#if defined(STRUCT_PACK_1)
	//如果定义了结构体成员1字节对齐，请在本文件开头定义STRUCT_PACK_1，
	//以保证token结构体的对齐访问
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
			struct {//透明文件
				unsigned int valid_token;
				unsigned int read_token;
				unsigned int update_token;
			} transBinEF;
			struct {//记录文件
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
	unsigned int memoryVolume;	//存储器容量
	unsigned char inqMaxNum;	//查询元素最大数量
	unsigned short inqMaxMatchNum;	//查询结果最大匹配数量
	unsigned char MIK[32];
	unsigned char MEK[32];
}TAG_ATTRIBUTE;

typedef struct{
	unsigned char TID[8];
	unsigned char UII[32];
	unsigned int memoryVolume;	//存储器容量
	unsigned char inqMaxNum;	//查询元素最大数量
	unsigned short inqMaxMatchNum;	//查询结果最大匹配数量
	int	Channel;	//设置信道
	int Power;		//设置功率
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

/*****************************阅读器参数相关配置*****************************/
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
	unsigned char	RID[4];			//阅读器RID
	unsigned char	ReadName[32];	//阅读器名称
	BAUD_TYPE		baudRate;		//波特率
	ENCRYPT_TYPE	encryptType;	//加密类型
	FILE_SYS_TYPE	fileSysType;	//阅读器所采用的文件系统类型
	unsigned int	storeCapacity;	//存储容量
}READER_INFO;

typedef struct{
	unsigned char	RID[4];			//阅读器RID
	char			ReadName[32];	//阅读器名称
	BAUD_TYPE		baudRate;		//波特率
}READER_INFO_W;

#pragma pack()
/***********************************************
 *命令类型定义:
 *			PC端与阅读器接口命令定义
 *
 ************************************************/
typedef enum{
	PC_CMD_TYPE_COLLECT=1,				//收集,盘存命令1
	PC_CMD_TYPE_SELECT_TAG,				//选择标签2
	PC_CMD_TYPE_SELECT_FILE,			//选择文件3
	PC_CMD_TYPE_READ_FILE,				//读文件4
	PC_CMD_TYPE_READ_LARGE_FILE,		//读大文件测试命令5
	PC_CMD_TYPE_WRITE_FILE,				//写文件6
	PC_CMD_TYPE_CREATE_FILE,			//创建文件7
	PC_CMD_TYPE_VALID_FILE,				//有效,无效文件[文件删除]8
	PC_CMD_TYPE_LIST_FILE,				//列表文件,对目录文件进行枚举9
	PC_CMD_TYPE_VERIFY_PWD_FILE,		//验证文件口令10
	PC_CMD_TYPE_SLEEP_ALL_TAG,			//休眠所有标签11
	PC_CMD_TYPE_MONITOR_CONF,			//检测周期配置12
	PC_CMD_TYPE_MONITOR_START,			//检测周期起始13
	PC_CMD_TYPE_MONITOR_REPORT,			//监测周期上报14
	PC_CMD_TYPE_MONITOR_STOP,			//监测周期停止15
	PC_CMD_TYPE_MONITOR_SENSOR_EN,		//监视传感器状态16
	PC_CMD_TYPE_SAFE_SESSION,			//安全会话17
	PC_CMD_TYPE_REGISTER_TAG,			//注册标签18
	PC_CMD_TYPE_GET_ALL_REGISTER_TAG,	//得到所有的已经注册的标签信息19
	PC_CMD_TYPE_CLR_ALL_REGISTER_TAG,	//清除所有的已经注册的标签信息20
	PC_CMD_TYPE_INIT_TAG,				//初始化标签21
	PC_CMD_TYPE_GET_READER_INFO,		//获取阅读器信息22
	PC_CMD_TYPE_SET_READER_INFO,		//设置阅读器信息23
	PC_CMD_TYPE_WAKEUP_ONE_TAG,			//唤醒单个标签24
	PC_CMD_TYPE_FILE_DELETE,			//文件删除25
	PC_CMD_TYPE_FILESYS_FORMAT,			//文件系统格式化26
	PC_CMD_TYPE_SELLECT_CHANNEL,		//选择阅读器信道27
	PC_CMD_TYPE_CONNECT_TAG,			//建立PC与Tag之间的连接28
	PC_CMD_TYPE_DISCONNECT_TAG,			//断开PC与Tag之间的连接29
	PC_CMD_TYPE_KILL_TAG,				//标签灭活30
	PC_CMD_TYPE_WRITECACHE_SYNC,		//写缓存同步31
	PC_CMD_TYPE_WRITE_CACHE,			//把写大文件（透明文件）的内容存入阅读器Flash中32
	PC_CMD_TYPE_SEND_CACHE,				//把阅读器Flash中的数据发送出去（给标签）33
	PC_CMD_TYPE_CLEAN_CACHE,			//清除缓存34
	PC_CMD_TYPE_SET_POWER,				//设置阅读器功率35
	PC_CMD_TYPE_SEND_PUREWAVE,			//发射空载波36
	PC_CMD_TYPE_STOP_PUREWAVE,			//停止空载波37
	PC_CMD_TYPE_SEND_MODUL_INFO,		//发射调制信息38
	PC_CMD_TYPE_STOP_MODUL_INFO,		//停止调制信息39
	PC_CMD_TYPE_SET_RECV_MODEL,			//设置接收模式40
	PC_CMD_TYPE_STOP_RECV_MODEL,		//停止接收模式41
	PC_CMD_TYPE_SET_KEY,				//发送密钥42
	PC_CMD_TYPE_SLEEP_EX_ONES_TAG,		//休眠除某一类标签外所有标签43
	PC_CMD_TYPE_SLEEP_EX_ONE_TAG,		//休眠除某个标签外所有标签44
	PC_CMD_TYPE_GET_TAG_INFO,			//获取标签信息45
	PC_CMD_TYPE_SET_POWER_TAG,			//设置标签功率46
	PC_CMD_TYPE_SET_CHANNEL_TAG,		//设置标签信道47
	PC_CMD_TYPE_SEARCH_RECORDS,			//检索记录48
	PC_CMD_TYPE_UPDATE_PWD,				//更新系统口令49
	PC_CMD_TYPE_UPDATE_ADMIN_PWD,		//更新管理员口令50
	PC_CMD_TYPE_GET_TAG_SAFE_PARAM,		//获取标签按安全参数51
	PC_CMD_TYPE_REQUIRE_RANDOM,			//请求随机数52
	PC_CMD_TYPE_DATA_BOARDCAST,			//数据广播53
	PC_CMD_TYPE_SEARCH_ALLTAG_RECS,		//检索所有标签记录54
	PC_CMD_TYPE_SEARCH_ALLTAG_TRANS,	//检索所有标签透明55
	PC_CMD_TYPE_COLLECT_SPEC_CONT,		//盘存指定内容标签56
	PC_CMD_TYPE_SEARCH_FILE,			//根据文件id写数据或读取文件数据57
	PC_CMD_TYPE_WAKEUP_TAG,             //唤醒命令58
	PC_CMD_TYPE_SECURITY_COMMUNICATION, //安全会话命令，二期不同与一期安全会话59
	PC_CMD_TYPE_SET_TCPIP,              //设置TCP/IP参数60
	PC_CMD_TYPE_SET_QN,                 //设置Q/N值61
	PC_CMD_TYPE_SET_MAC,                //设置MAC地址62
	PC_CMD_TYPE_TEST_SINGLE_CMD,        //单命令63
	
	// ----------------------------------Test------------------------------------
	PC_CMD_TYPE_TEST_SET_PARAM = 100,	//参数设置
	PC_CMD_TYPE_TEST_FREQ_ACCURACY,		//频率&准确度
	PC_CMD_TYPE_TEST_FREQ_ACCURACY_Q,	//频率&准确度（取消）
	PC_CMD_TYPE_TEST_MIN_RCVOR_BAND,	//最小接收机带宽
	PC_CMD_TYPE_TEST_MIN_RCVOR_BAND_Q,	//最小接收机带宽（取消）
	PC_CMD_TYPE_TEST_READER_SWITCH_TIME,//(Reader)转换时间
	PC_CMD_TYPE_TEST_TAG_SWITCH_TIME,	//(Tag)转换时间
	PC_CMD_TYPE_TEST_SWITCH_TIME_Q,		//转换时间（取消）
	PC_CMD_TYPE_TEST_MODUL_MODE,		//调制方式
	PC_CMD_TYPE_TEST_MODUL_MODE_Q,		//调制方式（取消）
	PC_CMD_TYPE_TEST_FSSS_SEQUENCE,		//扩频序列
	PC_CMD_TYPE_TEST_FSSS_SEQUENCE_Q,	//扩频序列（取消）
	PC_CMD_TYPE_TEST_BIT_SPEED,			//位速率
	PC_CMD_TYPE_TEST_BIT_SPEED_Q,		//位速率（取消）
	PC_CMD_TYPE_TEST_BEFORE_CODE,		//前导码
	PC_CMD_TYPE_TEST_BEFORE_CODE_Q,		//前导码（取消）
	PC_CMD_TYPE_TEST_PREVCOLLI_MANAGE,	//标签防碰撞管理
	PC_CMD_TYPE_TEST_PREVCOLLI_MANAGE_Q,//标签防碰撞管理（取消）
	PC_CMD_TYPE_TEST_RCV_SENSITIVITY,	//接收灵敏度
	PC_CMD_TYPE_TEST_RCV_SENSITIVITY_Q,	//接收灵敏度（取消）
}PC_CMD_TYPE;

typedef enum _TEST_RADIO_STATE{
	TEST_RADIO_GET_REPLY,
	TEST_RADIO_NOT_GET_REPLY
}TEST_RADIO_STATE; //测试单命令返回状态

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
	COLLECT_TAG_STATE_SUCCESS, //盘存成功
	COLLECT_TAG_STATE_FAILED,   //盘存失败
	COLLECT_TAG_STATE_SEND_FAILED,	//发送盘存命令失败
	COLLECT_TAG_STATE_EMPTY,		//没盘存到标签
	COLLECT_TAG_STATE_TIMEOUT		//盘存超时
}COLLECT_TAG_STATE;

typedef enum{
	SELECT_TAG_STATE_SUCCESS,//选择标签成功
	SELECT_TAG_STATE_NOT_EXISTING,//所选择的标签不存在
	SELECT_TAG_STATE_ALREADY_SELECTED,//标签已经被选中
	SELECT_TAG_STATE_SEND_FAILED,	//发送选择标签命令失败
	SELECT_TAG_STATE_TIMEOUT		//选择标签超时
}SELECT_TAG_STATE;

typedef enum{
	SELECT_FILE_STATE_SUCCESS,//选择文件成功
	SELECT_FILE_STATE_NEED_PWD_VERIFY,//选择文件,需要密码验证
	SELECT_FILE_STATE_NEED_ATH_VERIFY,//选择文件,需要实体验证
	SELECT_FILE_STATE_NOT_EXISTING,//所选择的文件不存在
	SELECT_FILE_STATE_DENIED,//选择文件,访问被拒绝,该文件不开放选择权限
	SELECT_FILE_STATE_TIMEOUT		//发送命令失败
}SELECT_FILE_STATE;

typedef enum{
	LIST_FILE_STATE_SUCCESS,
	LIST_FILE_STATE_NEED_PWD_VERIFY,
	LIST_FILE_STATE_NEED_ATH_VERIFY,
	LSIT_FILE_STATE_FILE_NOT_EXISTING,
	LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE,	//next mode时,已经到最后一个文件.
	LIST_FILE_STATE_FAILED,					//列表文件失败
	LIST_FILE_STATE_TIMEOUT,				//列表文件超时
	LIST_FILE_STATE_BUFF_NOT_ENOUGH,		//缓冲区不够
	LIST_FILE_STATE_NUMS_ERROR,				//由于信号不稳定导致列表文件个数不全
	LIST_FILE_STATE_NO_TID,					//不存在的TID
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
	CREATE_FILE_STATE_IN_NULL,	//输入的指针为空
	CREATE_FILE_STATE_TIMEOUT,
	CREATE_FILE_STATE_EXISTING,		//文件已经存在
	CREATE_FILE_NOT_ALLOWED,
	CREATE_FILE_INVALID,//所在目录是无效目录
	CREATE_FILE_NEED_PWD,//需要密码验证
	CREATE_FILE_NOT_AUTHORIZED,
	CREATE_FILE_TOO_DEEP,//文件深度过大
	CREATE_FILE_ATTRIBUTE_ERROR,//传入属性错误
	CREATE_FILE_SPACE_NOT_ENOUPH, //空间不足
	CREATE_FILE_TOO_MUCH_FILES, //目录中文件数目达到上限
}CREATE_FILE_STATE;

typedef enum _VALID_FILE_STATE{
	VALID_FILE_STATE_SUCCESS,
	VALID_FILE_STATE_TIMEOUT,
	VALID_FILE_STATE_FILE_NOT_EXIST,
	VALID_FILE_STATE_NOT_AUTHORISED,
	VALID_FILE_STATE_ALREADY_INVALID,
	VALID_FILE_STATE_UNKNOWN_ERR,
	VALID_FILE_STATE_NOT_ALLOWED,
	VALID_FILE_STATE_FAILED,		//未通过
	VALID_FILE_STATE_PWD_VERIFY,
	VALID_FILE_STATE_ATH_VERIFY,
	VALID_FILE_STATE_NEV_VERIFY
}VALID_FILE_STATE;

typedef enum _DELETE_FILE_STATE{
	FILE_DELETE_STATE_SUCCESS,
	FILE_DELETE_STATE_TIMEOUT,
	FILE_DELETE_STATE_UNKNOWN_ERR,
	FILE_DELETE_STATE_SYSFILE	//系统文件不能删除
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
	MONITOR_SENSOREN_STATE_SEND_FAILED //发送命令失败
}MONITOR_SENSOREN_STATE;

typedef enum _SAFE_SESSION_STATE{
	SAFE_SESSION_STATE_SUCCESS,
	SAFE_SESSION_STATE_UNKNOWN,
	SAFE_SESSION_STATE_TIMEOUT,
	SAFE_SESSION_STATE_FAILED,
	SAFE_SESSION_STATE_ALREADY_IN		//已进入安全会话
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

//单标签唤醒命令执行状态
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

typedef enum _UPDATE_PWD_STATE{//更新系统口令，返回码
	UPDATE_PWD_STATE_SUCCESS,
	UPDATE_PWD_STATE_FAILED,
	UPDATE_PWD_STATE_TIMEOUT,
	UPDATE_PWD_STATE_ADMIN_PWD_NOT_RIGHT,//管理员密码不匹配
	UPDATE_PWD_STATE_INVALID_INDEX_OR_MODE,//口令索引或者模式不合法
	UPDATE_PWD_STATE_INVALID_NEW_PWD,//新口令不合法
	UPDATE_PWD_STATE_NOT_AUTHORISED,//不在安全会话
	UPDATE_PWD_STATE_UNKNOWN,//未知错误
}UPDATE_PWD_STATE;

//更新管理员口令，返回状态
typedef enum _UPDATE_ADMIN_PWD_STATE{
	UPDATE_ADMIN_PWD_STATE_SUCCESS,				//0
	UPDATE_ADMIN_PWD_STATE_FAILED,				//1
	UPDATE_ADMIN_PWD_STATE_TIMEOUT,				//2
	UPDATE_ADMIN_PWD_STATE_NOT_AUTHORISED, //不在安全会话下3
	UPDATE_ADMIN_PWD_STATE_PWD_ERROR	//原密码错误4
}UPDATE_ADMIN_PWD_STATE;

//设置监测周期返回状态
typedef enum _CONF_MONITOR_STATE
{
	CONF_MONITOR_SUCCESS,				//0
	CONF_MONITOR_UNKNOWN,				//未知错误
	CONF_MONITOR_NOT_MATCH,				//传感器标识符不匹配
	CONF_MONITOR_OVERRANGE,				//数据越界
	CONF_MONITOR_FAILED,				//配置失败
	CONF_MONITOR_TIMEOUT				//接收回复超时
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
// 	//阅读器测试项
// 	RD_FREQUENCY_TEST,				//阅读器工作频率及准确度测试
// 	RD_BANDWIDTH_TEST,				//阅读器信道带宽测试
// 	RD_CHANNEL_LEAK_TEST,			//阅读器邻信道功率泄漏比测试
// 	RD_MAXPOWER_TEST,				//阅读器最大发射功率测试
// 	RD_RECV_SENSITIVITY_TEST,		//阅读器接收灵敏度测试
// 
// 	//标签测试项
// 	TAG_FREQUENCY_TEST = 10,				//标签工作频率测试
// 	TAG_RECV_SENSITIVITY_TEST,				//标签接收灵敏度测试
// 	TAG_EQULERAD_PWR_TEST,					//标签等效辐射功率测试
// 	TAG_SEND_BANDWIDTH_TEST					//标签发射占用信道带宽
// }TEST_COMMAND;//测试命令

//----------------Test 返回状态-----------------------
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
//----------------------PC通信命令参数结构体------------------------
#pragma pack(1)
typedef struct{
	unsigned short	sigStart;
	PC_CMD_TYPE		cmdType;
	union{
		//------------指定文件id读或写数据---------
		struct{
			unsigned char readBuffer[10];
			unsigned char writeBuffer[10];
			unsigned short fileId;
		}WRITE_OR_READ_FILE;
		//------------唤醒-------------
		struct{
			int wakeupNum;
		}WAKEUP_TAG_PARAM;
		//------------盘存-------------
		struct{
			unsigned int readyToken;
			unsigned char retryTimes;
		}INVENTORY_TAG_PARAM;
		//--------盘存指定内容标签----------
		struct{
			unsigned int readyToken;
			unsigned char retryTimes;
			unsigned short fileID;
			SEARCH_ELEMENT searchElement;//指定内容
			unsigned char datalen;//指定内容的长度
		}INVENTORY_SPECCONT_TAG_PARAM;
		//--------------选择标签命令参数-------------
		struct{
			unsigned char	TID[8];
		}SELECT_TAG_PRAM;
		/*---------------选择文件参数---------------*/
		struct{
			unsigned char	TID[8];
			unsigned short	fileID;
		}SELECT_FILE_PARAM;/**/
		//----------读文件参数--------------
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
		/*--------------------读大文件参数--------------*/
		struct{
			FILE_ATTRIBUTE_R fileAttrR;
			unsigned short fileOffset;
			unsigned char	dataLen;
		}READ_LARGE_FILE_PARAM;
		//---------------------写文件参数---------------------
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
		//-------------写大文件将数据存入阅读器Flash中--------------
		struct{
			unsigned short fileOffset;//2Byte
			unsigned char  dataLen;//1Byte
			char  dataBuffer[MAX_WRITE_CACHE_LEN];
		}WRITE_CACHE_PARAM;
		//-----------创建文件命令----------------
		struct{
			FILE_ATTRIBUTE_W fileAttr;
		}CREATE_FILE_PARAM;
		//-------------有效无效文件-----------
		struct {
			unsigned char newFileState;//0:新状态为无效,1:新状态为有效.
		}VALID_FILE_PARAM;
		//-------------列表文件-------------
		struct{
			unsigned char lsitMode;/*现阶段只支持模式1*/
			unsigned char dfRecPointer;
			unsigned char opMode;/*文件系统列表文件模式*/
		}LIST_FILE_PARAM;
		//------------验证口令--------------
		struct{
			unsigned char	pwdType;
			unsigned int	passWord;
		}VERIFY_PWD_FILE_PARAM;
		//-----------休眠所有标签------------
		struct{
			unsigned int sleepCode;
			unsigned char sleepTime;
			unsigned char listenTime;
		}SLEEP_ALL_TAG_PARAM;
		//-----------休眠除某一类标签外所有标签------------
		struct{
			unsigned int sleepCode;
			unsigned short fileID;
			unsigned char offset;
			unsigned char dataLen;
			char dataBuffer[80];	//长度可修改，暂定80
			unsigned char sleepTime;
			unsigned char listenTime;
		}SLEEP_EX_ONES_TAG_PARAM;
		//-----------休眠除某个标签外所有标签------------
		struct{
			unsigned char	TID[8];
			unsigned int sleepCode;
			unsigned char sleepTime;
			unsigned char listenTime;
		}SLEEP_EX_ONE_TAG_PARAM;
		//-----------监测配置命令--------------
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
		//-----------监测起始命令--------------
		struct{
			unsigned short CycleCnt;//监测周期次数
			unsigned short unUsed;
		}MONITOR_TRSTART_PARAM;
		//-----------传感器使能命令-----------------
		struct{
			unsigned short FileId;
			unsigned char  SensorId;
			unsigned char  SensorState;
		}MONITOR_SENSOREN_PARAM;
		//------------安全会话命令-----------------
		struct{
			unsigned char	TID[8];
		}SAFE_SESSION_PARAM;
		//--------------注册标签命令---------------
		struct{
			unsigned char keyType; //0为AES，1为ECC
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
		//--------------初始化标签命令参数---------
		struct{
			union{
				TAG_ATTRIBUTE tagAttr;
				struct{
					unsigned char priKeyTag[PKI_PRIVATEKEY_LEN];				//此处修改为了168
					unsigned char pubKeyReader[PKI_PUBLICKEY_LEN];
				}ECC_KEY;
			}ATTR;
			char paraType;//0：基本参数 1：ECC_key参数
		}TAG_ATTR_PARAM;
		//---------------唤醒单个标签----------------
		struct{
			unsigned char	TID[8];
			unsigned int	readyToken;
		}WAKEUP_ONE_TAG_PARAM;
		//----------------设置阅读器---------------
		struct{
			union{
				READER_INFO_W readerInfo_w;
				unsigned char priKeyReader[PKI_PRIVATEKEY_LEN];
			}u;
			char paraType;//0：基本参数 1：ECC_key参数
		}SET_READER_INFO_PARAM;
		//----------------空中烧写密钥----------------
		struct{
			char paraType;	// 0:烧写阅读器密钥，1:烧写标签密钥
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
		//-----------------设置信道------------------
		struct{
			int Channel;
		}SELECTED_CHANNEL;
		//--------------建立PC与Tag之间的连接----------
		struct{
			unsigned char connectTag;//c
		}CONNECT_TAG;
		//--------------断开PC与Tag之间的连接----------
		struct{
			unsigned char disconnectTag;//d
		}DISCONNECT_TAG;
		//--------------设置功率----------------------
		struct{
			int Power;
		}SELECTED_POWER;
		//--------------检索记录-------------------
		struct{
			unsigned short dataLen;
			unsigned char dataBuf[80];		//一个命令帧最大长度80
		}SEARCH_RECORDS;
		//--------------检索透明-------------------
		struct{
			unsigned short dataLen;
			unsigned char dataBuf[80];		//一个命令帧最大长度80
		}SEARCH_TRANS;
		//------------更新系统口令--------------
		struct{
			unsigned char Tid[9];
			unsigned int adminPWD;//提供管理员密码
			unsigned short indexPWD;//口令索引
			unsigned char mode;//index为文件标示符时使用，区分密码类别
			unsigned int newPWD;//新密码
		}UPDATE_PWD;
		//------------更新管理员口令------------
		struct{
			unsigned int oldPWD;//旧密码
			unsigned int newPWD;//新密码
		}UPDATE_ADMIN_PWD;
		//---------------标签灭活-----------------
		struct{
			unsigned int killTagToken;
		}KILL_TAG_PARAM;
		//---------------请求随机数---------------
		struct{
			unsigned char randomLenth;
		}REQUIRE_RANDOM_PARAM;
		//--------------数据广播---------------
		struct{
			unsigned int readyToken;
			unsigned char boardcastData[100];
		}DATA_BOARDCAST_PARAM;


//----------------------Test-----------------------------
		//-------------参数设置----------------
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
	unsigned short	tagState;//电量
	char	UII[32];
}TAG_INFO;

typedef struct{
	char	TID[8];
}TID_INFO;

typedef struct{
	unsigned char	TID[8];		//标签ID
	unsigned short	fileOffset;	//偏移
	unsigned short	fileLength;	//文件长度
	unsigned char	Data[80];	//数据 127-16-2-5=104(安全帧) 16字节对齐
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
		//唤醒
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
			unsigned int allListFileNums;				//返回所有列表文件个数
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
		//--------------传感器使能命令---------------
		struct{
			MONITOR_SENSOREN_STATE sensorEnState;
		}monitorSensorEnParam;
		struct{
			SAFE_SESSION_STATE safeSessionState;
		}safeSessionStateParam;
		struct{
			REGISTER_TAG_STATE registerTagState;
		}registerTagStateParam;
		//------------设置标签基本信息（初始化标签）-----------
		struct{
			INIT_TAG_STATE initTagState;
		}initTagStateParam;
		struct{
			CLR_ALL_TAG_STATE clrAllTagInfoState;
		}clrAllRegsitedTagParam;
		//---------获取所有注册标签信息-------------
		struct{
			GET_ALL_TAG_STATE getAllTagState;
			unsigned int allTagNums;
			unsigned int curTagNums;
			TID_INFO tidArray[8];
		}getAllTagParam;
		//-----------------单标签唤醒---------------------
		struct{
			WAKEUP_ONE_TAG_STATE wakeupOneTagState;
			TAG_INFO		tagInfo;
		}wakeupOneTagParam;
		//------------------获取阅读器信息-----------------
		struct{
			GET_RADER_INFO_STATE getReaderInfoSate;
			READER_INFO readerInfo;
		}getReaderInfoParam;
		//----------------设置阅读器信息--------------------
		struct{
			SET_RADER_INFO_STATE setReaderInfoState;
			//unsigned char  TID[8];
		}setReaderInfoParam;
		//-----------------------文件删除------------------
		struct{
			FILE_DELETE_STATE fileDeleteState;
		}fileDeleteParam;
		//-----------------------文件格式化----------------
		struct{
			FILESYS_FORMAT_STATE filesysFormatState;
		}filesysFormatParam;	
		//---------------设置信道------------------------
		struct{
			SET_CHANNEL setChannelState;
		}setChannelParam;
		//---------------建立PC与Tag之间的连接----------------
		struct{
			unsigned char connectTag;//o
			CONNECT_TAG_STATE connectTagState;
		}connectTagParam;
		//--------------断开PC与Tag之间的连接----------
		struct{
			unsigned char disconnectTag;//k
		}disconnectTagParam;
		//--------------标签灭活------------------
		struct{
			KILL_TAG_STATE killTagstate;
		}killTagParam;
		//-------------写缓存同步--------------
		struct{
			WRITECACHE_SYNC_STATE writecacheSyncState;
		}writecacheSyncParam;
		//--------把写大文件的数据存入阅读区Flash中------
		struct{
			WRITE_CACHE_STATE writeCacheState;
		}writeCacheParam;
		//--------把阅读器Flash中的数据发送给标签--------
		struct{
			SEND_CACHE_STATE sendCacheState;
		}sendCacheParam;
		//------------清除缓存----------------
		struct{
			CLEAN_CACHE_STATE cleanCacheState;
		}cleanCacheParam;
		//----------------休眠所有标签--------------------
		struct{
			SLEEP_TAG_STATE sleepTagState;
		}sleepTagParam;
		//---------------休眠除某一类标签外所有标签---------------
		struct{
			SLEEP_EX_ONES_TAG_STATE sleepExOnesTagState;
		}sleepExOnesTagParam;
		//---------------休眠除某个标签外所有标签---------------
		struct{
			SLEEP_EX_ONE_TAG_STATE sleepExOneTagState;
		}sleepExOneTagParam;
		//------------设置功率-----------------
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
			int CollectTime;	//单位ms
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
		//-------------------空中烧写密钥-------------------
		struct{
			WIRELESS_SET_KEY_STATE wirelessSetKeyState;
		}wirelessSetKeyParam;
		//--------------------获取标签信息-------------------
		struct{
			GET_TAG_INFO_STATE getTagInfoState;
			TAG_ATTRIBUTE_R tagAttribute;
		}getTagInfoParam;
		struct{
			SEARCH_RECORDS_STATE searchRecordsState;
			unsigned short retRecords;
		}searchRecordsParam;
		//---------------更新系统口令-----------------
		struct{
			UPDATE_PWD_STATE upDatePwdState;
		}updatePWDParam;
		//---------------更新管理员口令-----------------
		struct{
			UPDATE_ADMIN_PWD_STATE updateAdminPwdState;
		}updateAdminPwdParam;
		//-----------------监测周期配置---------------------
		struct{
			CONF_MONITOR_STATE confMonState;
		}confMonParam;
		//----------------监测起始回复-----------------------
		struct{
			MONITOR_START_STATE monitorStartState;
		}monitorStartParam;
		//-----------------监测周期标签汇报---------------------
		struct{
			GET_MONITOR_REPORT_STATE State;
			CYCLE_INFO	cycleInfo;
		}MonRepParam;
		//-----------------监测周期停止--------------------
		struct{
			MONITOR_STOP_STATE monitorStopState;
		}monitorStopParam;
		//----------------获取标签安全参数------------------
		struct{
			GET_TAG_SAFE_PARAM_STATE getTagSafeParamState;
			unsigned short keyParam;
			unsigned short safePowerParam;
		}getTagSafeParam;
		//---------------请求随机数-----------------
		struct{
			REQUIRE_RANDOM_STATE requireRandomState;
			unsigned char randomNums[8];
		}requireRandomParam;
		//---------------数据广播-----------------
		struct{
			DATA_BOARDCAST_STATE dataBoardcastState;
		}dataBoardcastParam;

		struct{
			TEST_RADIO_STATE  testRadioState;
			unsigned char Data[128];
		}testSingleCmd;//测试单命令返回
	}uReplyParam;
	unsigned short SIG_END;
}PC_CMD_REPLY,*PPC_CMD_REPLY;
#pragma pack()
#define CMD_LEN sizeof(PC_CMD_DATA)
#define CMD_REPLY_LEN sizeof(PC_CMD_REPLY)
void pcCmdInterfaceInit(void);
#endif
