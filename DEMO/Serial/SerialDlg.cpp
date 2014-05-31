//_commandEntries
// SerialDlg.cpp : 实现文件
//
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include "stdafx.h"
#include "Serial.h"
#include "SerialDlg.h"
#include "afxdialogex.h"
#include "debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MsgEventID.h"
#include "PcCommand.h"
#include "PwdRFDlg.h"
#include "SleepTagDlg.h"
#include "XPButton.h"
#include <shellapi.h>
#include "RegisterTagDlg.h"
#include "ReaderManageDlg.h"
#include "SetSocketDlg.h"
#include "ReaderTestDlg.h"
#include "TestSingleCmd.h"
#include "makeECDH.h"
#include "makeKey.h"
#include "io.h"
#include "..\..\API二期\APIDLL\apidll.h"
using namespace std;

extern BOOL isConnected;
char* baudTypeString[] = {
	"波特率：9600\r\n",
	"波特率：115200\r\n",
	"波特率：128000\r\n"
	"波特率：256000\r\n"
};
char* encryptTypeString[] = {
	"加密类型：异或\r\n",
	"加密类型：哈希\r\n",
	"加密类型：AES\r\n",
	"加密类型：ECC\r\n"
};
char* fileSysTypeString[] = {
	"所采用的文件系统类型：未使用文件系统\r\n",
	"所采用的文件系统类型：FAT文件系统\r\n",
	"所采用的文件系统类型：GJB文件系统\r\n"
};
list<int> MsgEventID;  //全局消息ID列表
CRITICAL_SECTION g_cs; //全局关键段，用来消息事件线程的同步互斥


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
#pragma pack(1)
typedef struct{
	char name[8];
	unsigned int  age;
	char score;
}STU_INFO,*PSTU_INFO;
#pragma pack(1)

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//{{AFX_MSG_MAP(CAllMaterialDlg)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSerialDlg 对话框


CSerialDlg::CSerialDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSerialDlg::IDD, pParent)
	, m_comboCom(0)
	, m_ReplyHead(0)
	, m_dataInBuf(false)
	, m_statusbar(_T(""))
	, m_maxRecordNum(0)
	, m_fileRealLen(0)
	, m_maxFileLen(0)
	, m_CurrentSelectFileID(0)
	, m_recFileContent(_T(""))
	, m_writeFile(_T(""))
	, m_staticFileMaxLen(_T(""))
	, m_staticFileRecords(_T(""))
	, m_staticFileRealLen(_T(""))
	, ComStateClose(TRUE)
	, m_FileOffset(0)
	, m_pGoodsBuff(NULL)
	, m_SecurityAttrB2_3(_T(""))
	, m_SecurityAttrB4_5(_T(""))
	, m_bMF(TRUE)
	, m_validCmdType(-1)
	, m_writeCmdType(-1)
	, m_pwdRdRearch(0)
	, m_pwdUpdate(0)
	, m_pwdValid(0)
	, m_curNumOfTag(0)
	, m_bRetToMf(false)
	, pszBigFile(NULL)
	, m_curptr(NULL)
	, m_NumOfReadLargeFile(0)
	, m_TID(_T(""))
	, m_toolStr(_T(""))
	, m_MIK(_T(""))
	, m_MEK(_T(""))
	, m_filePathName(_T(""))
	, m_isSelected(false)
	, m_comReciveType(false)
	, m_pSelectedTag(NULL)
	, m_adminPwd(0)
	, m_newPwd(0)
	, m_recPoint(0)
	, m_bListFileAfterSelect(false)
	, m_OpenCommunication(FALSE)
	, m_IPPort(7086)
	, m_IP("192.168.1.200")
	, m_IPMask("255.255.255.0")
	,m_FileID("a105")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strRXData = _T("");
	m_pFileList = NULL;
	m_pTagList = NULL;
	m_pGoodsList = NULL;
	m_pSelectedFileAttr = NULL;
	m_pTmpSelectedFileAttr = NULL;
	pszBigFile = (char*)malloc(1024*1024);
	m_ptmpSelectTag = NULL;
	m_pImage = NULL;
	if (pszBigFile==NULL)
	{
		AfxMessageBox("初始化程序失败");
	}
	memset(pszBigFile,0,1024*1024);
	memset(&m_timeOfReadLargeFile,0,sizeof(m_timeOfReadLargeFile));
	memset(m_pubKeyTag,0,PKI_PUBLICKEY_LEN);
	m_SearchCondition = _T("");
	m_PwdIndex = _T("");
	m_WakeupNum = 5000;
	m_CollectNum = 1;
}

void CSerialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RXDATA, m_strRXData);
	DDX_Control(pDX, IDC_EDIT_RXDATA, m_strRX);
	DDX_CBIndex(pDX, IDC_COMBO1, m_comboCom);
	DDX_Control(pDX, IDC_COMBO1, m_Port);
	DDX_Control(pDX, IDC_COMBO2, m_Baud);
	DDX_Control(pDX, IDC_COMBO3, m_Check);
	DDX_Control(pDX, IDC_COMBO4, m_Data);
	DDX_Control(pDX, IDC_COMBO5, m_Stop);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Control(pDX, IDC_LIST2, m_listCtrlFile);
	DDX_Text(pDX, IDC_STATIC1, m_statusbar);
	DDX_Text(pDX, IDC_EDIT5, m_maxRecordNum);
	DDX_Text(pDX, IDC_EDIT6, m_fileRealLen);
	DDX_Text(pDX, IDC_EDIT4, m_maxFileLen);
	DDX_Text(pDX, IDC_EDIT3, m_recFileContent);
	DDX_Text(pDX, IDC_EDIT10, m_writeFile);
	//  DDX_Text(pDX, IDC_EDIT1, m_FileID);
	//  DDV_MaxChars(pDX, m_FileID, 10);
	DDX_Text(pDX, IDC_STATIC_LEN1, m_staticFileMaxLen);
	DDX_Text(pDX, IDC_STATIC_LEN2, m_staticFileRecords);
	DDX_Text(pDX, IDC_STATIC_LEN3, m_staticFileRealLen);
	DDX_Text(pDX, IDC_EDIT7, m_FileOffset);
	DDX_Control(pDX, IDC_LIST3, m_listCtrlGoods);
	DDX_Control(pDX, IDC_COMBO6, m_cmbFileType);
	DDX_Control(pDX, IDC_COMBO7, m_cmbValidState);
	DDX_Control(pDX, IDC_COMBO8, m_cmbAthState);
	DDX_Text(pDX, IDC_STATIC_SECU1, m_SecurityAttrB2_3);
	DDX_Text(pDX, IDC_STATIC_SECU2, m_SecurityAttrB4_5);
	DDX_Control(pDX, IDC_COMBO9, m_cmbUpdata);
	DDX_Control(pDX, IDC_COMBO10, m_cmbB2_3);
	DDX_Text(pDX, IDC_EDIT11, m_pwdRdRearch);
	DDX_Text(pDX, IDC_EDIT9, m_pwdUpdate);
	DDX_Text(pDX, IDC_EDIT8, m_pwdValid);
	DDX_Control(pDX, IDC_BUTTON16, m_xpbtn_open_sensor);
	DDX_Control(pDX, IDC_BUTTON10, m_btn_sleep);
	DDX_Control(pDX, IDC_BUTTON4, m_btn_select);
	DDX_Control(pDX, IDC_BUTTON2, m_btn_inventory);
	DDX_Control(pDX, IDC_BUTTON3, m_btn_open_com);
	DDX_Control(pDX, IDCANCEL, m_exit_soft);
	DDX_Control(pDX, IDC_BUTTON7, m_btn_zero_offset_write_file);
	DDX_Control(pDX, IDC_BUTTON14, m_btn_any_offset_write);
	DDX_Control(pDX, IDC_BUTTON8, m_btn_create_file);
	DDX_Control(pDX, IDC_BUTTON15, m_btn_read_big_file);
	DDX_Control(pDX, IDC_BUTTON1, m_btn_ret_main_def);
	DDX_Control(pDX, IDC_BUTTON13, m_btn_invaild_file);
	DDX_Control(pDX, IDC_BUTTON5, m_btn_read_file);
	DDX_Control(pDX, IDC_BUTTON11, m_btn_select_file);
	DDX_Control(pDX, IDC_BUTTON6, m_btn_list_file);
	DDX_Control(pDX, IDC_BUTTON12, m_btn_valid_file);
	DDX_Control(pDX, IDC_BTN_SAFE_SESSION, m_btn_safe_session);
	DDX_Control(pDX, IDC_BTN_REGISTER_TAG, m_btn_register_tag);
	DDX_Text(pDX, IDC_EDIT12, m_TID);
	DDV_MaxChars(pDX, m_TID, 8);
	DDX_Text(pDX, IDC_EDIT18, m_MIK);
	DDV_MaxChars(pDX, m_MIK, 32);
	DDX_Text(pDX, IDC_EDIT19, m_MEK);
	DDV_MaxChars(pDX, m_MEK, 32);
	DDX_Control(pDX, IDC_BTN_GETTAGINFO, m_btn_getTagInfo);
	DDX_Control(pDX, IDC_BTN_CLEANALLTAG, m_btn_cleanAllTag);
	DDX_Control(pDX, IDC_BTN_WAKEUPONETAG, m_btn_wakeupOneTag);
	DDX_Control(pDX, IDC_BTN_FILE_DELETE, m_btn_file_delete);
	DDX_Control(pDX, IDC_BTN_FILESYS_FORMAT, m_btn_fileSys_format);
	DDX_Control(pDX, IDC_BTN_IMPOTAG_PUBKEY, m_btn_impoTag_pubKey);
	DDX_Control(pDX, IDC_BTN_KILL_TAG, m_btn_kill_tag);
	DDX_Control(pDX, IDC_BTN_KEY_WIRELESS, m_wirelessSetKey);
	DDX_Control(pDX, IDC_BTN_UPDATE_PWD, m_btn_updatePwd);
	DDX_Text(pDX, IDC_EDIT_ADMIN_PWD, m_adminPwd);
	//  DDX_Text(pDX, IDC_EDIT_PWD_INDEX, m_pwdIndex);
	DDX_Text(pDX, IDC_EDIT_NEW_PWD, m_newPwd);
	DDX_Control(pDX, IDC_BTN_WRITE_LARGE_FILE, m_writeLargeFile);
	DDX_Control(pDX, IDC_COMBO12, m_CommuFlag);
	DDX_Control(pDX, IDC_OpenSocket, m_openSocket);
	DDX_Control(pDX, IDC_CloseSocket, m_closeSocket);
	DDX_Text(pDX, IDC_IPEDIT, m_IP);
	DDX_Text(pDX, IDC_IPMASKEDIT, m_IPMask);
	DDX_Text(pDX, IDC_IPPORTEDIT, m_IPPort);
	DDX_Control(pDX, IDC_READERTEST, m_ReaderTestButton);
	DDX_Control(pDX, IDC_CMDTEST, m_CmdTestButton);
	DDX_Control(pDX, IDC_SEARCHRECORD, m_SerachRecordButton);
	DDX_Text(pDX, IDC_EDIT25, m_SearchCondition);
	DDX_Text(pDX, IDC_EDIT_PWD_INDEX, m_PwdIndex);
	DDX_Text(pDX, IDC_WAKEUPNUM, m_WakeupNum);
	DDX_Control(pDX, IDC_WAKEUP, mWakeUpButton);
	DDX_Text(pDX, IDC_EDIT1, m_FileID);
	DDX_Text(pDX, IDC_EDIT27, m_CollectNum);
}

BEGIN_MESSAGE_MAP(CSerialDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDC_MSCOMM1, &CSerialDlg::OnComm)
	ON_COMMAND(ID_BTN_TOOLBAR1,popRegisterTag)   //button1为初始化标签消息处理函数
	ON_COMMAND(ID_BTN_TOOLBAR2,popReaderManager) //button2为阅读器管理消息处理函数
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, &CSerialDlg::OnBnClickedButton2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CSerialDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON4, &CSerialDlg::OnBnClickedButton4)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CSerialDlg::OnLvnItemchangedList2)
	ON_BN_CLICKED(IDC_BUTTON6, &CSerialDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CSerialDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON10, &CSerialDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CSerialDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON7, &CSerialDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CSerialDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON12, &CSerialDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CSerialDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDOK, &CSerialDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO6, &CSerialDlg::OnCbnSelchangeCombo6)
	ON_CBN_SELENDOK(IDC_COMBO6, &CSerialDlg::OnCbnSelendokCombo6)
	ON_CBN_DBLCLK(IDC_COMBO6, &CSerialDlg::OnCbnDblclkCombo6)
	ON_CBN_CLOSEUP(IDC_COMBO6, &CSerialDlg::OnCbnCloseupCombo6)
	ON_EN_CHANGE(IDC_EDIT4, &CSerialDlg::OnEnChangeEdit4)
	ON_BN_CLICKED(IDC_BUTTON14, &CSerialDlg::OnBnClickedButton14)
	ON_WM_SIZE()
	ON_CBN_SETFOCUS(IDC_COMBO1, &CSerialDlg::OnCbnSetfocusCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, &CSerialDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON15, &CSerialDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CSerialDlg::OnBnClickedButton16)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_REGISTER_TAG, &CSerialDlg::OnBnClickedRegisterTag)
	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnDisplay)
	ON_BN_CLICKED(IDC_BTN_GETTAGINFO, &CSerialDlg::OnBnClickedBtnGettaginfo)
	ON_BN_CLICKED(IDC_BTN_CLEANALLTAG, &CSerialDlg::OnBnClickedBtnCleanalltag)
	ON_BN_CLICKED(IDC_BTN_WAKEUPONETAG, &CSerialDlg::OnBnClickedBtnWakeuponetag)
	ON_BN_CLICKED(IDC_BTN_FILE_DELETE, &CSerialDlg::OnBnClickedBtnFileDelete)
	ON_BN_CLICKED(IDC_BTN_FILESYS_FORMAT, &CSerialDlg::OnBnClickedBtnFilesysFormat)
	ON_BN_CLICKED(IDC_BTN_KILL_TAG, &CSerialDlg::OnBnClickedBtnKillTag)
	ON_BN_CLICKED(IDCANCEL, &CSerialDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_UPDATE_PWD, &CSerialDlg::OnBnClickedBtnUpdatePwd)
	ON_WM_CHAR()
	ON_BN_CLICKED(IDC_OpenSocket, &CSerialDlg::OnBnClickedOpensocket)
	ON_BN_CLICKED(IDC_CloseSocket, &CSerialDlg::OnBnClickedClosesocket)
	ON_MESSAGE(MY_MESSAGE, &CSerialDlg::OnMyMessage)
	ON_MESSAGE(UPDATETRUE, &CSerialDlg::OnUpdatetrue)
	ON_BN_CLICKED(IDC_READERTEST, &CSerialDlg::OnBnClickedReadertest)
	ON_BN_CLICKED(IDC_CMDTEST, &CSerialDlg::OnBnClickedCmdtest)
	ON_BN_CLICKED(IDC_BTN_SAFE_SESSION, &CSerialDlg::OnBnClickedBtnSafeSession)
	ON_BN_CLICKED(IDC_SEARCHRECORD, &CSerialDlg::OnBnClickedSearchrecord)
	ON_BN_CLICKED(IDC_WAKEUP, &CSerialDlg::OnBnClickedWakeup)
END_MESSAGE_MAP()


// CSerialDlg 消息处理程序

BOOL CSerialDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	printf("这是一条测试信息");
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetClientRect(&m_rect);
	GetClientRect(&m_originalRectSize);
	
	enum_reg_key_value(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\SERIALCOMM");

	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("9600");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("115200");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("128000");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("256000");

	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("None");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("Odd");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("Even");

	((CComboBox*)GetDlgItem(IDC_COMBO4))->AddString("5");
	((CComboBox*)GetDlgItem(IDC_COMBO4))->AddString("6");
	((CComboBox*)GetDlgItem(IDC_COMBO4))->AddString("7");
	((CComboBox*)GetDlgItem(IDC_COMBO4))->AddString("8");

	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("1");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("1.5");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("2");

	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->AddString("1_列表");
	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->AddString("2_有效无效");
	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->AddString("3_读/检索");
	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->AddString("4_更新");


	//默认选择项
	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(0);//COM号
	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(1);//波特率
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(0);//校验
	((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(3);//数据位长度
	((CComboBox*)GetDlgItem(IDC_COMBO5))->SetCurSel(0);//停止位长度

	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->SetCurSel(0);//更新系统口令
	

	((CComboBox*)GetDlgItem(IDC_COMBO6))->AddString("目录DF");
	((CComboBox*)GetDlgItem(IDC_COMBO6))->AddString("透明EF");
	((CComboBox*)GetDlgItem(IDC_COMBO6))->AddString("记录EF");

	((CComboBox*)GetDlgItem(IDC_COMBO7))->AddString("有效");
	((CComboBox*)GetDlgItem(IDC_COMBO7))->AddString("无效");

	((CComboBox*)GetDlgItem(IDC_COMBO8))->AddString("ALW");
	((CComboBox*)GetDlgItem(IDC_COMBO8))->AddString("PWD");
	((CComboBox*)GetDlgItem(IDC_COMBO8))->AddString("ATH");
	((CComboBox*)GetDlgItem(IDC_COMBO8))->AddString("NEV");

	((CComboBox*)GetDlgItem(IDC_COMBO9))->AddString("ALW");
	((CComboBox*)GetDlgItem(IDC_COMBO9))->AddString("PWD");
	((CComboBox*)GetDlgItem(IDC_COMBO9))->AddString("ATH");
	((CComboBox*)GetDlgItem(IDC_COMBO9))->AddString("NEV");

	((CComboBox*)GetDlgItem(IDC_COMBO10))->AddString("ALW");
	((CComboBox*)GetDlgItem(IDC_COMBO10))->AddString("PWD");
	((CComboBox*)GetDlgItem(IDC_COMBO10))->AddString("ATH");
	((CComboBox*)GetDlgItem(IDC_COMBO10))->AddString("NEV");

	((CComboBox*)GetDlgItem(IDC_COMBO12))->AddString("TCP");
	((CComboBox*)GetDlgItem(IDC_COMBO12))->AddString("USB");

	m_cmbFileType.SetCurSel(1);
	m_cmbValidState.SetCurSel(0);
	m_cmbAthState.SetCurSel(0);
	m_cmbUpdata.SetCurSel(0);
	m_cmbB2_3.SetCurSel(0);
	m_CommuFlag.SetCurSel(0);


	m_staticFileMaxLen = "最大文件长度";
	m_staticFileRecords = "保留值:1";
	m_staticFileRealLen = "实际文件长度";
	m_SecurityAttrB2_3 = "读取和查询";
	m_SecurityAttrB4_5 = "更新";

	m_maxRecordNum = 1;
	m_maxFileLen = 1;
	m_fileRealLen = 0;

	UpdateData(FALSE);//更新编辑框内容

	DWORD dwTagList_Style;
	dwTagList_Style = m_listCtrl.GetExtendedStyle();
	m_listCtrl.SetExtendedStyle(dwTagList_Style|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);//选中某行使整行高亮&设置扩展风格
	m_listCtrl.InsertColumn(0,"#");
	m_listCtrl.InsertColumn(1,"标签ID");
	m_listCtrl.InsertColumn(2,"电池电量");
	m_listCtrl.InsertColumn(3,"UII");
	m_listCtrl.SetColumnWidth(0,30);
	m_listCtrl.SetColumnWidth(1,170);
	m_listCtrl.SetColumnWidth(2,60);
	m_listCtrl.SetColumnWidth(3,20);


	DWORD dwFileList_Style;
	dwFileList_Style = m_listCtrlFile.GetExtendedStyle();
	m_listCtrlFile.SetExtendedStyle(dwFileList_Style|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);//选中某行使整行高亮&设置扩展风格
	m_listCtrlFile.InsertColumn(0,"#");
	m_listCtrlFile.InsertColumn(1,"文件ID");
	m_listCtrlFile.InsertColumn(2,"文件类型");
	m_listCtrlFile.InsertColumn(3,"有效信息");
	m_listCtrlFile.InsertColumn(4,"最大长度/记录长度");
	m_listCtrlFile.InsertColumn(5,"最大允许记录数");
	m_listCtrlFile.InsertColumn(6,"实际长度");
	m_listCtrlFile.InsertColumn(7,"[选择][列表/读取查询][更新][有效/无效]");
	m_listCtrlFile.SetColumnWidth(0,30);
	m_listCtrlFile.SetColumnWidth(1,50);
	m_listCtrlFile.SetColumnWidth(2,60);
	m_listCtrlFile.SetColumnWidth(3,60);
	m_listCtrlFile.SetColumnWidth(4,115);
	m_listCtrlFile.SetColumnWidth(5,100);
	m_listCtrlFile.SetColumnWidth(6,60);
	m_listCtrlFile.SetColumnWidth(7,240);


	DWORD dwGoodsList_Info;
	dwGoodsList_Info = m_listCtrlGoods.GetExtendedStyle();
	m_listCtrlGoods.SetExtendedStyle(dwGoodsList_Info|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);//选中某行使整行高亮&设置扩展风格
	m_listCtrlGoods.InsertColumn(0,"#");
	m_listCtrlGoods.InsertColumn(1,"编码");
	m_listCtrlGoods.InsertColumn(2,"名称");
	m_listCtrlGoods.InsertColumn(3,"型号");
	m_listCtrlGoods.InsertColumn(4,"质量等级");
	m_listCtrlGoods.InsertColumn(5,"数量");
	m_listCtrlGoods.InsertColumn(6,"生产日期");
	m_listCtrlGoods.InsertColumn(7,"计量单位");
	m_listCtrlGoods.SetColumnWidth(0,30);
	m_listCtrlGoods.SetColumnWidth(1,70);
	m_listCtrlGoods.SetColumnWidth(2,50);
	m_listCtrlGoods.SetColumnWidth(3,60);
	m_listCtrlGoods.SetColumnWidth(4,60);
	m_listCtrlGoods.SetColumnWidth(5,50);
	m_listCtrlGoods.SetColumnWidth(6,80);
	m_listCtrlGoods.SetColumnWidth(7,60);


	m_curMFDF.fileAttr.fileID = 0x3F00;

	m_ImageList.Create(32,32,ILC_COLOR24|ILC_MASK,1,1);//图片大小必须一致
	CBitmap bmp;
	bmp.LoadBitmap(IDB_REGISTER_TAG);
	m_ImageList.Add(&bmp,RGB(255,255,255));
	bmp.DeleteObject();
	bmp.LoadBitmap(IDB_READER_MANAGE);
	m_ImageList.Add(&bmp,RGB(255,255,255));
	bmp.DeleteObject();

	UINT nArray[2];
	for(int i=0;i<2;i++)
	{
		nArray[i] = ID_BTN_TOOLBAR1+i;
	}

	m_Toolbar.CreateEx(this);//创建工具栏控件,CreateEx函数创建的工具栏，默认有浮动按钮属性
	m_Toolbar.EnableToolTips();//激活信息提示功能
	m_Toolbar.SetButtons(nArray,2);//创建六个工具按钮，并依次设置ID号，对应nArray数组的元素
	m_Toolbar.SetSizes(CSize(40,38),CSize(32,32));//函数第一个是按钮大小，第二个是图像大小，按钮必须比图像要大，具体是按钮的要比图像的宽大7或者以上，高6
	m_Toolbar.GetToolBarCtrl().SetImageList(&m_ImageList);//设置图像
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

	hComEvent = CreateEvent(NULL,FALSE,FALSE,"comDevice");
	SetEvent(hComEvent);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSerialDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout; 
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSerialDlg::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rect;
	GetClientRect(&rect);
	CPaintDC *pDC = &dc;
	if (IsIconic())
	{
		//		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);

	}

	else
	{
		if(m_pImage)  
		{  
			Graphics   graphics(pDC->GetSafeHdc());  
			Rect   rt;  
			RECT   rc;  
			GetClientRect(&rc);  
			rt.X   =   rc.left;  
			rt.Y   =   rc.top;  
			rt.Width   =   rc.right   -   rc.left;  
			rt.Height   =   rc.bottom   -   rc.top;  
			int   nX,nY;  
			if((int)m_pImage->GetWidth()   >=   rt.Width)  
			{  
				nX   =   0;  
			}  
			else  
			{  
				nX   =   (rt.Width   -   m_pImage->GetWidth())   /   2;  
			}  
			if((int)m_pImage->GetHeight()   >=   rt.Height)  
			{  
				nY   =   0;  
			}  
			else  
			{  
				nY   =   (rt.Height   -   m_pImage->GetHeight())   /   2;  
			}  
			graphics.DrawImage(m_pImage   ,   nX   ,   nY   );  

		}
		// 		// 背景贴图;
		// 		CPaintDC dc(this); // 用于绘制的设备上下文
		// 		CRect rect;
		// 		GetClientRect(&rect);
		// 		CDC  dcMem;  
		// 		dcMem.CreateCompatibleDC(&dc);///建立关联DC 
		// 		CBitmap  bmpBackground;  
		// 		bmpBackground.LoadBitmap(IDB_GND_BACKGROUND6);
		// // 		bmpBackground.LoadBitmap(IDB_GND_MIDDLE);
		// // 		bmpBackground.LoadBitmap(IDB_GND_BOTTOM);
		// 		BITMAP  bitmap;  
		// 		bmpBackground.GetBitmap(&bitmap);  
		// 		CBitmap  *pbmpOld=dcMem.SelectObject(&bmpBackground);  
		// 		dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,  
		// 			bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);
		// 		// Do not call CDialog::OnPaint() for painting messages

		//GDI+贴图


		CDialogEx::OnPaint();
	}
}


HBRUSH CSerialDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if( nCtlColor == CTLCOLOR_STATIC)      
	{    
		pDC->SetBkMode(TRANSPARENT);   //设置背景透明 
		//		return   HBRUSH(GetStockObject(HOLLOW_BRUSH));
		hbr = (HBRUSH)(m_brush.GetSafeHandle());
	}
	//TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSerialDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSerialDlg::OnComm()
{
	// TODO: 在此添加命令处理程序代码
}


void CSerialDlg::OnTimer(UINT_PTR nIDEvent)
{
	static int count = 0;
	PC_CMD_DATA pcCmdData;
	CSerialDlg::initCmdData(pcCmdData,PC_CMD_TYPE_CONNECT_TAG);
//	memcpy(pcCmdData.uParam.CONNECT_TAG.connectTag,"listen",6);
	switch(nIDEvent)
	{
	case 1:	//计时器1
// 		if (!isConnected)
// 		{
// 			PC_CMD_DATA pcCmdData;
// 			int i;
// 			((CSerialDlg *)(theApp.m_pMainWnd))->send_cmd("12",2);
// 			count++;
// 			m_statusbar.Format("send :%d ",count);
// 			if (count == 1200)
// 			{
// 				count = 0;
// 				KillTimer(1);
// 			}
// 		}
		break;
	case 123456:
		//GetDlgItem(IDC_STATIC1)->SetWindowText("盘存标签超时");
		m_statusbar = "盘存标签超时";
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		UpdateData(FALSE);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
BEGIN_EVENTSINK_MAP(CSerialDlg, CDialogEx)
	/*ON_EVENT(CSerialDlg, IDC_MSCOMM1, 1, CSerialDlg::OnCommMscomm1, VTS_NONE)*/
END_EVENTSINK_MAP()




/*后续代码可读性要完善，一期遗留的废弃逻辑剔除掉。by ppwu 11-12*/
static const int cmdReplyLen = sizeof(PC_CMD_REPLY);
#define lenArrayLen 1024
static int lenArrayTest[lenArrayLen],lenIndex = 0 ,lenloop = 0;
int RxCounter=0;



int CSerialDlg::send_cmd(void* pData, unsigned int Len)
{
	return 0;
}

unsigned int CSerialDlg::receive()
{
	unsigned int time=1000000;
	while(time){
		if (m_dataInBuf)
		{
			break;
		}

		time--;
	}
	if (time)
	{
		m_dataInBuf = FALSE;
		return 1;
	}else
	{
		return 0;
	}
}

void CSerialDlg::OnBnClickedButton2()		//盘存
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//用于保存DoModal函数的返回值
	CPwdRFDlg pwdRFDlg;							//构造对话框类CPwdRFDlg的实例
	nRes = pwdRFDlg.DoModal();					//弹出对话框
	if (IDCANCEL == nRes)	return;				//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	UpdateData(TRUE);
	m_statusbar = "盘存开始,请稍候…………";
	//GetDlgItem(IDC_STATIC1)->SetWindowText("盘存开始,请稍候…………");
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);//盘存button
	//"首地址写文件"按键变亮
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
	//"列表文件"按键变灰
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
	//"偏移量写文件"按键变亮
	GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
	
	//SetTimer(123456,15000,NULL);

	ClearTagList();
	ClearFileAttrList();
	m_curNumOfTag = 0;

	m_recFileContent.Empty(); //清空读文件control数据
	m_writeFile.Empty();
	m_listCtrlFile.DeleteAllItems();
	m_listCtrl.DeleteAllItems();		//清空列表中现有的item
	UpdateData(FALSE);

	m_pSelectedTag = NULL;

	/*MsgEventID.push_back(COLLECT_MSG_ID);*/
	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CollectTagThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CSerialDlg::CollectTagThreadFunc(LPVOID lparam)
{
	//调用盘存API
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //获得主窗口指针
	CPwdRFDlg  *pwdlg = new CPwdRFDlg;
	int nIndex = 0;
	vector<string> TidVector;
	int FlagNum = 1;
	BYTE tmpCollectNum = pdlg->m_CollectNum;
	
	while(tmpCollectNum--)
	{
		if(FlagNum != 1)
		{
			(pdlg->m_statusbar).Format("继续执行第%d次盘存......", FlagNum);
			pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			Sleep(5200); //每次盘存之间间隔5s，此处多留出0.2s，排除误差
		}

		WORD  allTagNum;
		TAG_INFO  tagInfoTemp[100];

		int Ret = rfCollectTag(tagInfoTemp, 100, &allTagNum, 1, pwdlg->m_pwd);
		switch(Ret)
		{
		case IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_SUCCESS:
			{
				CString strNumber, tagStateStr, uiiStr;
				BYTE tid[8] = {0};
				BYTE uii[32] = {0};
				PTAG_ATTR pTagAttr;
				TAG_INFO  *tagInfo;

				tagInfo = (TAG_INFO*)malloc(sizeof(TAG_INFO)*allTagNum);
				memcpy(tagInfo, &tagInfoTemp, sizeof(TAG_INFO)*allTagNum);
				
				for(int nCount=0; nCount < allTagNum; nCount++)
				{
					//......
					int cmpFlag[8] = {0};
					for(int i = 0; i < 8; i++)
					{
						if(tagInfo[nCount].TID[i] == 0x00)
						{
							tagInfo[nCount].TID[i] = 0xaf; //临时转换
							cmpFlag[i] = 1;
						}
					}

					string FindTid;
					FindTid = (char*)tagInfo[nCount].TID;
					FindTid.resize(8);
					FindTid.reserve(8); //设置string类的capacity的大小，如果不设置为8，编译器会自动分配，每次分配的值可能不一样，导致find函数查找有误差

					vector<string>::const_iterator result = find(TidVector.begin(), TidVector.end(), FindTid);

					if(result == TidVector.end()) //find函数没有找到返回它的第二个迭代器实参
					{
						TidVector.push_back(FindTid); //把没有重复的push到容器中

						//......
						for(int i = 0; i < 8; i++)
						{
							if((tagInfo[nCount].TID[i] == 0xaf) && (cmpFlag[i] == 1))
								tagInfo[nCount].TID[i] = 0x00; //恢复转换
						}

						//执行显示插入过程
						CString tidStr;
						nIndex = pdlg->m_listCtrl.GetItemCount();
						strNumber.Format("%d",nIndex);//取得已有标签个数
						pdlg->m_listCtrl.InsertItem(nIndex,strNumber);//插入新的一行,准备显示

						BYTE TidLen = strlen((char*)tagInfo[nCount].TID);
						for(int i = 0; i < 8; i++)
						{
							CString tmpTid;
							tmpTid.Format("%02x ", tagInfo[nCount].TID[i]);
							tidStr += tmpTid;
						}
						pdlg->m_listCtrl.SetItemText(nIndex, 1, tidStr);

						//标签状态格式化显示
						tagStateStr.Format("%02x", tagInfo[nCount].tagState);
						pdlg->m_listCtrl.SetItemText(nIndex, 2, tagStateStr);

						//保存相应的标签信息数据
						pTagAttr = (PTAG_ATTR)malloc(sizeof(TAG_ATTR));//申请内存
						memset(pTagAttr, 0, sizeof(TAG_ATTR));//内存清零
						pTagAttr->nIndex = nIndex;
						pTagAttr->pNext = NULL;
						memcpy(pTagAttr->TID, tagInfo[nCount].TID,TID_LEN);
						pdlg->InsertTagList(pTagAttr);
					}
				}
				(pdlg->m_statusbar).Format("第%d次,盘存成功", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
			break;
		case IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_SEND_FAILED:
			{
				(pdlg->m_statusbar).Format("第%d次,盘存标签失败", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
			break;
		case IF_TYPE_COLLECT_TAG| COLLECT_TAG_STATE_TIMEOUT:
			{
				(pdlg->m_statusbar).Format("第%d次,盘存阅读器回复失败", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
			break;
		case IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_EMPTY:
			{
				(pdlg->m_statusbar).Format("第%d次,没有盘存到标签", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
			break;
		default:
			{
				(pdlg->m_statusbar).Format("第%d次,盘存未知错误", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);//这里都要用sendmessage，不能用postmessage.
			}
			break;
		}
		FlagNum++; //记录盘存次数
	}
	pdlg->m_SwitchSignal = TRUE; //让触发其他事件变成可执行状态，False表示其他事件不可触发
	pdlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
}


void CSerialDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	*pResult = 0;
}


void CSerialDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CSerialDlg::OnBnClickedButton4()		//选卡
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}

	m_statusbar.Empty();
	m_recFileContent.Empty();
	m_writeFile.Empty();
	UpdateData(FALSE);

	//在标签列表中获得具体某行的行号
	CString str;
	unsigned int nId;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition(); //得到点击的位置
	if (pos == NULL)
	{
		MessageBox("请至少选择一项标签");
		return;
	}
	nId = (unsigned int)m_listCtrl.GetNextSelectedItem(pos);  //得到行号，通过POSITION转化
	PTAG_ATTR pTagAttr = GetTagAttr(nId);
	if(pTagAttr == NULL)
	{
		AfxMessageBox("查找标签数据失败");
		return;
	}

	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	m_ptmpSelectTag = pTagAttr;
	m_bMF = TRUE;

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SelectTagThreadFunc, this, 0, NULL);
	CloseHandle(startThread);

// 	//调用选择标签API
// 	int Ret = rfSelectTag(m_ptmpSelectTag->TID);
// 
// 	switch(Ret)
// 	{
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SUCCESS:
// 		{
// 			m_pSelectedTag = m_ptmpSelectTag;
// 			m_statusbar = "选择标签成功";
// 			ClearFileAttrList();
// 			m_listCtrlFile.DeleteAllItems();
// 			GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
// 			GetDlgItem(IDC_BUTTON16)->EnableWindow(TRUE);
// 			GetDlgItem(IDC_BTN_FILESYS_FORMAT)->EnableWindow(TRUE);
// 		}
// 		break;
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SEND_FAILED:
// 		m_statusbar = "选择标签发送错误";
// 		break;
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_TIMEOUT:
// 		m_statusbar = "选择标签超时";
// 		break;
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_NOT_EXISTING:
// 		m_statusbar = "选择标签不存在";
// 		break;
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_ALREADY_SELECTED:
// 		m_statusbar = "标签已经被选中";
// 		break;
// 	default:
// 		m_statusbar = "选择标签未知错误";
// 		break;
// 	}
// 	UpdateData(FALSE);
}


void CSerialDlg::SelectTagThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam;

	int Ret = rfSelectTag(pdlg->m_ptmpSelectTag->TID);
	pdlg->m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SUCCESS:
		{
			pdlg->m_pSelectedTag = pdlg->m_ptmpSelectTag;
			pdlg->m_statusbar = "选择标签成功";
			pdlg->ClearFileAttrList();
			pdlg->m_listCtrlFile.DeleteAllItems();
			pdlg->GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
			pdlg->GetDlgItem(IDC_BUTTON16)->EnableWindow(TRUE);
			pdlg->GetDlgItem(IDC_BTN_FILESYS_FORMAT)->EnableWindow(TRUE);
		}
		break;
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SEND_FAILED:
		pdlg->m_statusbar = "选择标签发送错误";
		break;
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_TIMEOUT:
		pdlg->m_statusbar = "选择标签超时";
		break;
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_NOT_EXISTING:
		pdlg->m_statusbar = "选择标签不存在";
		break;
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_ALREADY_SELECTED:
		pdlg->m_statusbar = "标签已经被选中";
		break;
	default:
		pdlg->m_statusbar = "选择标签未知错误";
		break;
	}

	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
}


void CSerialDlg::OnBnClickedButton6()    //列表文件
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON15)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_FILE_DELETE)->EnableWindow(FALSE);
	UpdateData(TRUE);
	ClearFileAttrList();//后期考虑主从目录问题add 2013-11-25
	m_listCtrlFile.DeleteAllItems();
	m_statusbar.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListFileThreadFunc, this, 0, NULL);
	CloseHandle(startThread);

// 	FILE_ATTRIBUTE_R fileAttr_R[20];
// 	DWORD fileNums;
// 	int Ret = rfListFile(CMD_LIST_MODE_NORMAL, 0, 100, fileAttr_R, &fileNums);
// 	switch(Ret)
// 	{
// 	case IF_TYPE_LIST_FILE | LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE:
// 		{
// 			for (int i = 0; i < fileNums; i++)
// 			{
// 				if ( (m_pSelectedFileAttr != NULL)&&(m_pSelectedFileAttr->fileAttr.fileTypeValid&FILE_TYPE_MASK) != FILE_TYPE_DF)
// 				{
// 					m_pSelectedFileAttr = NULL;
// 				}
// 
// 				/******************************************/
// 				int nIndex2 = GetFileListItemsNum();
// 				PLIST_FILE_ATTR pListFileAttr = (PLIST_FILE_ATTR)malloc(sizeof(LIST_FILE_ATTR));
// 				memset(pListFileAttr, 0, sizeof(LIST_FILE_ATTR));
// 				pListFileAttr->nIndex = nIndex2;
// 				memcpy(&pListFileAttr->fileAttr, &fileAttr_R[i], sizeof(FILE_ATTRIBUTE_R));
// 				InsertFileList(pListFileAttr);
// 				m_statusbar = "列表文件成功";
// 				/******************************************/
// 				DisplayOneFilelistItems(m_listCtrlFile, pListFileAttr);
// 			}
// 		}
// 	    break;
// 	default:
// 		m_statusbar = "列表文件失败";
// 		break;
// 	}
// 	UpdateData(FALSE);
}

void CSerialDlg::ListFileThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam;

// 	FILE_ATTRIBUTE_R fileAttr_R[20];
// 	DWORD fileNums;
// 	int Ret = rfListFile(CMD_LIST_MODE_NORMAL, 0, 100, fileAttr_R, &fileNums);
// 	switch(Ret)
// 	{
// 	case IF_TYPE_LIST_FILE | LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE:
// 		{
// 			for (int i = 0; i < fileNums; i++)
// 			{
// 				if ((pdlg->m_pSelectedFileAttr != NULL)&&(pdlg->m_pSelectedFileAttr->fileAttr.fileTypeValid&FILE_TYPE_MASK) != FILE_TYPE_DF)
// 				{
// 					pdlg->m_pSelectedFileAttr = NULL;
// 				}
// 
// 				/******************************************/
// 				int nIndex2 = pdlg->GetFileListItemsNum();
// 				PLIST_FILE_ATTR pListFileAttr = (PLIST_FILE_ATTR)malloc(sizeof(LIST_FILE_ATTR));
// 				memset(pListFileAttr, 0, sizeof(LIST_FILE_ATTR));
// 				pListFileAttr->nIndex = nIndex2;
// 				memcpy(&pListFileAttr->fileAttr, &fileAttr_R[i], sizeof(FILE_ATTRIBUTE_R));
// 				pdlg->InsertFileList(pListFileAttr);
// 				pdlg->m_statusbar = "列表文件成功";
// 				/******************************************/
// 				pdlg->DisplayOneFilelistItems(pdlg->m_listCtrlFile, pListFileAttr);
// 			}
// 		}
// 		break;
// 	default:
// 		pdlg->m_statusbar = "列表文件失败";
// 		break;
// 	}
// 	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

	BOOL Flag = TRUE;
	BYTE opMode = DERECTORY_LIST_MODE_CURRENT;

	while(Flag)
	{ 
		FILE_ATTRIBUTE_R  fileAttr_R = {0};

		int Ret = rfListFileNew(CMD_LIST_MODE_NORMAL, 0, opMode, &fileAttr_R);
		switch(Ret)
		{
		case IF_TYPE_LIST_FILE | LIST_FILE_STATE_SUCCESS:
			{
				opMode = DERECTORY_LSIT_MODE_NEXT; //继续列下一个

				if ((pdlg->m_pSelectedFileAttr != NULL)&&(pdlg->m_pSelectedFileAttr->fileAttr.fileTypeValid&FILE_TYPE_MASK) != FILE_TYPE_DF)
				{
					pdlg->m_pSelectedFileAttr = NULL;
				}
			 
				int nIndex2 = pdlg->GetFileListItemsNum();
				PLIST_FILE_ATTR pListFileAttr = (PLIST_FILE_ATTR)malloc(sizeof(LIST_FILE_ATTR));
				memset(pListFileAttr, 0, sizeof(LIST_FILE_ATTR));
				pListFileAttr->nIndex = nIndex2;
				memcpy(&pListFileAttr->fileAttr, &fileAttr_R, sizeof(FILE_ATTRIBUTE_R));
				pdlg->InsertFileList(pListFileAttr);
				pdlg->DisplayOneFilelistItems(pdlg->m_listCtrlFile, pListFileAttr);

				pdlg->m_statusbar = "列表未完成，继续......";
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

				break;
			}
		case IF_TYPE_LIST_FILE | LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE:
			{
				pdlg->m_SwitchSignal = TRUE;
				Flag = FALSE;
				pdlg->m_statusbar = "列表成功";
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
				return;
			}
		default:
			{
				pdlg->m_SwitchSignal = TRUE;
				pdlg->m_statusbar = "列表文件失败";
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
				return;
			}
		}
	}
}


void CSerialDlg::OnBnClickedButton5()		//读文件
{
	// TODO: 在此添加控件通知处理程序代码
	m_recPoint = 0;
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	m_recFileContent.Empty();
	m_writeFile.Empty();
	m_statusbar.Empty();
	UpdateData(FALSE);

	if (m_pSelectedFileAttr == NULL)
	{
		MessageBox("请选择文件");
		GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
		return;
	}

	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		MessageBox("该文件为目录文件，请选择列表文件查看其内容");
		return;
	}

	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	if (m_pSelectedFileAttr->fileAttr.fileID == 0x400D)
	{
		ClearGoodsList();
		m_listCtrlGoods.DeleteAllItems();
		if (m_pGoodsBuff != NULL)
		{
			free(m_pGoodsBuff);
		}
		m_pGoodsBuff = (char*)malloc(m_pSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen+2);
		memset(m_pGoodsBuff,0,m_pSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen+2);
	}

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadFileThreadFunc, this, 0, NULL);
	CloseHandle(startThread);

// 	m_curReadState.pFileAttr = &m_pSelectedFileAttr->fileAttr;
// 	switch(m_pSelectedFileAttr->fileAttr.securityType & TOKEN_RDSEARCH_MASK )
// 	{
// 	case TOKEN_RDSEARCH_STATE_ALLOW:		//ALW
// 		{
// 			switch(m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK)
// 			{
// 			case FILE_TYPE_TRANS:
// 				{
// 					UpdateData(FALSE);
// 					m_curReadState.FileOffset = 0;
// 					WORD retLen;
// 					WORD NumToRead = m_pTmpSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen;
// 					BYTE buff[1024*8] = {0};
// 
// 					int Ret = rfReadTransFile(0, NumToRead, &retLen, buff);
// 					switch(Ret)
// 					{
// 					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FAILED:
// 						m_statusbar = "读透明文件，没选文件";
// 						break;
// 					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_TIMEOUT:
// 						m_statusbar = "读透明文件超时";
// 						break;
// 					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NOT_EXISTING:
// 						m_statusbar = "读透明文件不存在";
// 						break;
// 					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FINISH:
// 						{
// 							m_statusbar = "读取透明文件完成";
// 							CString tempbuf;
// 							tempbuf.Format("%s\r\n", buff);
// 							m_recFileContent += tempbuf;
// 						}
// 						break;
// 					default:
// 						m_statusbar = "读取透明文件失败";
// 						break;
// 					}
// 					m_curReadState.readFinish = TRUE;
// 				}
// 				break;
// 			case FILE_TYPE_RECORD: //读记录文件
// 				{
// 					ReadRecordFileItems(RECORD_IN_FILE_READ_MODE_FIX);
// 				}
// 				break;
// 			default:
// 				{
// 					m_statusbar = "该文件为目录文件，请选择“列表文件”查看其内容";
// 					GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
// 					return;
// 				}
// 			}
// 		}
// 	default:
// 		m_statusbar = "操作读文件失败";
// 		break;
// 	}
// 
// 	UpdateData(FALSE);
}

void CSerialDlg::ReadFileThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //获得主窗口指针

	pdlg->m_statusbar = "读文件正在进行中......";
	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

	pdlg->m_curReadState.pFileAttr = &pdlg->m_pSelectedFileAttr->fileAttr;
	switch(pdlg->m_pSelectedFileAttr->fileAttr.securityType & TOKEN_RDSEARCH_MASK )
	{
	case TOKEN_RDSEARCH_STATE_ALLOW:		//ALW
		{
			switch(pdlg->m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK)
			{
			case FILE_TYPE_TRANS: //读透明文件
				{
					pdlg->SendMessage(UPDATETRUE, NULL, NULL);
					pdlg->m_curReadState.FileOffset = 0;
					WORD retLen;
					WORD NumToRead = pdlg->m_pTmpSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen;
					BYTE buff[1024*16] = {0};

					int Ret = rfReadTransFile(pdlg->m_FileOffset, NumToRead, &retLen, buff);
					pdlg->m_SwitchSignal = TRUE;
					switch(Ret)
					{
					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FAILED:
						pdlg->m_statusbar = "读透明文件失败";
						break;
					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_TIMEOUT:
						pdlg->m_statusbar = "读透明文件超时";
						break;
					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NOT_EXISTING:
						pdlg->m_statusbar = "读透明文件不存在";
						break;
					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FINISH:
						{
							for(int i = 0; i < pdlg->m_pSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen; i++)
							{
								CString tmpStr;
								tmpStr.Format("%02x", buff[i]);
								pdlg->m_recFileContent += tmpStr;
							}
							pdlg->m_statusbar = "读取透明文件完成";

							//CString tempbuf;
							//tempbuf.Format("%s\r\n", buff);
							//pdlg->m_recFileContent += tempbuf;
							//pdlg->m_statusbar = "读取透明文件完成";
						}
						break;
					default:
						pdlg->m_statusbar = "读取透明文件错误";
						break;
					}
				}
				break;
			case FILE_TYPE_RECORD: //读记录文件
				{
					//pdlg->ReadRecordFileItems(RECORD_IN_FILE_READ_MODE_FIX);
					if(pdlg->m_pSelectedFileAttr->fileAttr.uFileAttr.recEF.realRecordNums == 0)
					{
						pdlg->m_SwitchSignal = TRUE;
						pdlg->m_statusbar = "该记录文件没有内容";
						pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
						return;
					}
					int RecordNum = 0;
					while((pdlg->m_pSelectedFileAttr->fileAttr.uFileAttr.recEF.realRecordNums)--)
					{
						RecordNum++;

						int Ret;
						int dataLen = pdlg->m_pSelectedFileAttr->fileAttr.uFileAttr.recEF.recordLen;
						WORD retLen;
						BYTE buff[255] = {0};

// 						if(RecordNum == 1)
// 							Ret = rfReadRecFile(0, RECORD_IN_FILE_READ_MODE_FIX, 0, dataLen, &retLen, buff);
// 						else
// 							Ret = rfReadRecFile(0, RECORD_IN_FILE_READ_MODE_NEXT, 0, dataLen, &retLen, buff);
						Ret = rfReadRecFile(RecordNum - 1, RECORD_IN_FILE_READ_MODE_FIX, 0, dataLen, &retLen, buff);
						pdlg->m_SwitchSignal = TRUE;
						switch(Ret)
						{
						case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_FAILED:
							(pdlg->m_statusbar).Format("读第%d条记录文件失败", RecordNum);
							pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
							break;
						case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_NEED_NEV_VERIFY:
							(pdlg->m_statusbar).Format("读第%d条记录文件失败,权限为NEV，访问被拒绝", RecordNum);
							pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
							break;
						case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_TIMEOUT:
							(pdlg->m_statusbar).Format("读第%d条记录文件超时", RecordNum);
							pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
							break;
						case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_SUCCESS:
							{
								(pdlg->m_statusbar).Format("读第%d条记录文件成功", RecordNum);
								pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

								CString tmpStr;
								for(int i = 0; i < pdlg->m_pSelectedFileAttr->fileAttr.uFileAttr.recEF.recordLen; i++)
								{
									tmpStr.Format("%02x", buff[i]);
									pdlg->m_recFileContent += tmpStr;
								}
								tmpStr.Format(" \r\n");
								pdlg->m_recFileContent += tmpStr;
								//(pdlg->m_statusbar).Format("读第%d条记录文件成功", RecordNum);
								//pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
								//CString tempbuf;
								//tempbuf.Format("%s\r\n", buff);
								//pdlg->m_recFileContent += tempbuf;
							}
							break;
						default:
							pdlg->m_statusbar = "读记录文件其他错误";
							break;
						}
					}
				}
				break;
			default:
				{
					pdlg->m_statusbar = "该文件为目录文件，请选择“列表文件”查看其内容";
					pdlg->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
					pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
					return;
				}
			}
		}
		break;
	default:
		pdlg->m_statusbar = "操作读文件失败";
		break;
	}

	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
}


void CSerialDlg::OnBnClickedButton15()		//读大文件测试
{
	// TODO: 在此添加控件通知处理程序代码
}



void CSerialDlg::OnBnClickedButton10()		//标签休眠
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//用于保存DoModal函数的返回值
	CSleepTagDlg sleepTagDlg;					//构造对话框类CSleepTagDlg的实例
	nRes = sleepTagDlg.DoModal();					//弹出对话框
	if (IDCANCEL == nRes)						//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行
		return;
}


void CSerialDlg::OnBnClickedButton11()    //选择文件
{
	// TODO: 在此添加控件通知处理程序代码
	m_statusbar.Empty();
	m_recFileContent.Empty();
	m_writeFile.Empty();
	UpdateData(FALSE);

	//在文件列表中获得具体某行的行号
	unsigned int nId;
	POSITION pos = m_listCtrlFile.GetFirstSelectedItemPosition(); //得到点击的位置
	if (pos == NULL)
	{
		MessageBox("请至少选择一项");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	nId = (unsigned int)m_listCtrlFile.GetNextSelectedItem(pos);//得到行号，通过POSITION转化
	PLIST_FILE_ATTR pFileAttr = GetFileAttrByIndex(nId);

	if(pFileAttr !=	NULL)
	{
		m_isSelected = TRUE;
		m_pTmpSelectedFileAttr = pFileAttr;
		m_CurrentSelectFileID = pFileAttr->fileAttr.fileID;

		int Ret = rfSelectFile(m_CurrentSelectFileID, &m_pTmpSelectedFileAttr->fileAttr);
		m_SwitchSignal = TRUE;
		switch(Ret)
		{
		case IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_SUCCESS:
			{
				m_statusbar = "选择文件成功";
				if (m_pSelectedFileAttr != m_pTmpSelectedFileAttr)
				{
					m_pSelectedFileAttr = m_pTmpSelectedFileAttr;
					m_pSelectedFileAttr->TokenValidState = false;
					m_pSelectedFileAttr->TokenListState = false;
					m_pSelectedFileAttr->TokenRdSearchState = false;
					m_pSelectedFileAttr->TokenUpdateState = false;
				}

				if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
				{
					m_bMF = FALSE;
					if (&m_curMFDF != m_pSelectedFileAttr)
					{
						memcpy(&m_curMFDF, m_pTmpSelectedFileAttr, sizeof(m_curMFDF));
						m_pSelectedFileAttr = &m_curMFDF;
					}	
				}
				if(m_bRetToMf)//上一次选中的是主目录文件,返回主目录
				{
					m_bMF = TRUE;
					m_bRetToMf = FALSE;
				}
				if (m_bListFileAfterSelect)
				{
					m_bListFileAfterSelect = false;
					m_curMFDF.TokenValidState = false;
					OnBnClickedButton6();
					return;
				}
				GetDlgItem(IDC_BTN_FILE_DELETE)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON15)->EnableWindow(TRUE);
				//"首地址写文件"按键变亮
				GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
				//"偏移量写文件"按键变亮
				GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
			}
			break;
		default:
			m_statusbar = "选择文件失败";
			break;
		}
	}
	else
	{
		m_isSelected = FALSE;
		AfxMessageBox("查找文件属性列表失败");
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedButton7()	//写文件
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}

	m_statusbar.Empty();
	//"首地址写文件"后按键变灰
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
	UpdateData(TRUE);

	m_writeCmdType = 0;
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("请先选择文件");
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_INVALID_MASK) == FILE_TYPE_INVALID)
	{
		MessageBox("该文件为无效文件");
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		MessageBox("该文件为目录文件");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WriteFileThreadFunc, this, 0, NULL);
	CloseHandle(startThread);

// 	switch(m_pSelectedFileAttr->fileAttr.securityType & TOKEN_UPDATE_MASK)
// 	{
// 	case TOKEN_UPDATE_STATE_ALLOW:
// 		if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//该文件为透明文件
// 		{
// 			char dataBuffer[8*1024];
// 			m_pSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen = m_writeFile.GetLength();
// 			memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
// 			int Ret = rfWriteFile(0, 0, m_writeFile.GetLength(), dataBuffer);
// 			switch(Ret)
// 			{
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
// 				m_statusbar = "写透明文件超时";
// 				break;
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
// 				m_statusbar = "写透明文件失败";
// 				break;
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
// 				m_statusbar = "写透明文件成功";
// 				break;
// 			default:
// 				m_statusbar = "写透明文件未知错误";
// 				break;
// 			}
// 		}
// 		else{	//该文件为记录文件
// 			if (m_writeFile.GetLength() > 109)
// 			{
// 				MessageBox("长度不能超过最大缓冲区109，但其实也不应该超过80");
// 				return;
// 			}
// 			
// 			char dataBuffer[109];
// 			memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
// 			int Ret = rfWriteFile(1, 0, m_writeFile.GetLength(), dataBuffer);
// 			switch(Ret)
// 			{
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
// 				m_statusbar = "写记录文件成功";
// 				break;
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
// 				m_statusbar = "写本条记录文件需安全会话";
// 				break;
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
// 				m_statusbar = "写记录文件失败";
// 				break;
// 			case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
// 				m_statusbar = "写记录文件接收超时";
// 				break;
// 			default:
// 				m_statusbar = "写记录文件未知错误";
// 				break;
// 			}
// 		}
// 		break;
// 
// 	case TOKEN_UPDATE_STATE_PWD:
// 		{
// 			if (m_pSelectedFileAttr->TokenUpdateState == FALSE)	//FALSE表示未进行密码验证
// 			{
// 				INT_PTR nRes;								//用于保存DoModal函数的返回值
// 				CPwdRFDlg pwdRFDlg;							//构造对话框类CPwdRFDlg的实例
// 				nRes = pwdRFDlg.DoModal();					//弹出对话框
// 				if (IDCANCEL == nRes)	return;				//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行
// 
// 				m_curVerifyPwdState.pListFileAttr = m_pSelectedFileAttr;
// 
// 				int Ret = rfVerifyPwdFile(TOKEN_TYPE_UPDATE, theApp.m_pwd);
// 				switch(Ret)
// 				{
// 				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED:
// 					m_statusbar = "密码验证失败";
// 					break;
// 				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_SUCCESS:
// 					m_statusbar = "密码验证正确";
// 					break;
// 				default:
// 					m_statusbar = "密码验证未知错误";
// 					break;
// 				}
// 			}
// 			else
// 			{
// 				if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//该文件为透明文件
// 				{
// 					char dataBuffer[8*1024];
// 					memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
// 					int Ret = rfWriteFile(0, 0, m_writeFile.GetLength(), dataBuffer);
// 					switch(Ret)
// 					{
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
// 						m_statusbar = "写透明文件超时";
// 						break;
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
// 						m_statusbar = "写透明文件失败";
// 						break;
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
// 						m_statusbar = "写透明文件成功";
// 						break;
// 					default:
// 						m_statusbar = "写透明文件未知错误";
// 						break;
// 					}
// 				}
// 				else	//该文件为记录文件
// 				{
// 					if (m_writeFile.GetLength() > 109)
// 					{
// 						MessageBox("长度不能超过最大缓冲区109，但其实也不应该超过80");
// 						return;
// 					}
// 
// 					char dataBuffer[109];
// 					memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
// 					int Ret = rfWriteFile(1, 0, m_writeFile.GetLength(), dataBuffer);
// 					switch(Ret)
// 					{
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
// 						m_statusbar = "写记录文件成功";
// 						break;
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
// 						m_statusbar = "写本条记录文件需安全会话";
// 						break;
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
// 						m_statusbar = "写记录文件失败";
// 						break;
// 					case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
// 						m_statusbar = "写记录文件接收超时";
// 						break;
// 					default:
// 						m_statusbar = "写记录文件未知错误";
// 						break;
// 					}
// 				}
// 			}
// 		}
// 		break;
// 	case TOKEN_UPDATE_STATE_NEV:
// 		MessageBox("不允许写当前文件");
// 		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
// 		break;
// 	}
// 	UpdateData(FALSE);
}

void CSerialDlg::WriteFileThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //获得主窗口指针

	pdlg->m_statusbar = "写文件正在进行中......";
	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

	switch(pdlg->m_pSelectedFileAttr->fileAttr.securityType & TOKEN_UPDATE_MASK)
	{
	case TOKEN_UPDATE_STATE_ALLOW:
		if ((pdlg->m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//该文件为透明文件
		{
			char dataBuffer[16*1024];
			int DataLen;
			DataLen = pdlg->StrToHex(pdlg->m_writeFile.GetBuffer(), dataBuffer);
			//DataLen = pdlg->m_writeFile.GetLength();
			//memcpy(dataBuffer, pdlg->m_writeFile.GetBuffer(), pdlg->m_writeFile.GetLength());

			int Ret = rfWriteFile(0, pdlg->m_FileOffset, DataLen, dataBuffer);
			pdlg->m_SwitchSignal = TRUE;
			switch(Ret)
			{
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
				pdlg->m_statusbar = "写透明文件超时";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
				pdlg->m_statusbar = "写透明文件失败";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
				pdlg->m_statusbar = "写透明文件成功";
				break;
			default:
				pdlg->m_statusbar = "写透明文件未知错误";
				break;
			}
		}
		else
		{	//该文件为记录文件
			char dataBuffer[256];
			int DataLen;
			DataLen = pdlg->StrToHex(pdlg->m_writeFile.GetBuffer(), dataBuffer);

			if (DataLen > 108)
			{
				AfxMessageBox("长度不能超过最大缓冲区108");
				return;
			}

			int Ret = rfWriteFile(1, pdlg->m_FileOffset, DataLen, dataBuffer);
			pdlg->m_SwitchSignal = TRUE;
			switch(Ret)
			{
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
				pdlg->m_statusbar = "写记录文件成功";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
				pdlg->m_statusbar = "写本条记录文件需安全会话";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
				pdlg->m_statusbar = "写记录文件失败";
				break;
			case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
				pdlg->m_statusbar = "写记录文件接收超时";
				break;
			default:
				pdlg->m_statusbar = "写记录文件未知错误";
				break;
			}
		}
		break;

	case TOKEN_UPDATE_STATE_PWD:
		{
			if (pdlg->m_pSelectedFileAttr->TokenUpdateState == FALSE)	//FALSE表示未进行密码验证
			{
				INT_PTR nRes;								//用于保存DoModal函数的返回值
				CPwdRFDlg pwdRFDlg;							//构造对话框类CPwdRFDlg的实例
				nRes = pwdRFDlg.DoModal();					//弹出对话框
				if (IDCANCEL == nRes)	return;				//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行

				pdlg->m_curVerifyPwdState.pListFileAttr = pdlg->m_pSelectedFileAttr;

				int Ret = rfVerifyPwdFile(TOKEN_TYPE_UPDATE, theApp.m_pwd);
				pdlg->m_SwitchSignal = TRUE;
				switch(Ret)
				{
				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED:
					pdlg->m_statusbar = "密码验证失败";
					break;
				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_SUCCESS:
					pdlg->m_statusbar = "密码验证正确";
					break;
				default:
					pdlg->m_statusbar = "密码验证未知错误";
					break;
				}
			}
			else
			{
				if ((pdlg->m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//该文件为透明文件
				{
					char dataBuffer[16*1024];
					int DataLen;
					DataLen = pdlg->StrToHex(pdlg->m_writeFile.GetBuffer(), dataBuffer);

					int Ret = rfWriteFile(0, pdlg->m_FileOffset, DataLen, dataBuffer);
					pdlg->m_SwitchSignal = TRUE;
					switch(Ret)
					{
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
						pdlg->m_statusbar = "写透明文件超时";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
						pdlg->m_statusbar = "写透明文件失败";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
						pdlg->m_statusbar = "写透明文件成功";
						break;
					default:
						pdlg->m_statusbar = "写透明文件未知错误";
						break;
					}
				}
				else	//该文件为记录文件
				{
					char dataBuffer[256];
					int DataLen;
					DataLen = pdlg->StrToHex(pdlg->m_writeFile.GetBuffer(), dataBuffer);

					if (DataLen > 108)
					{
						AfxMessageBox("长度不能超过最大缓冲区108");
						return;
					}

					int Ret = rfWriteFile(1, pdlg->m_FileOffset, DataLen, dataBuffer);
					pdlg->m_SwitchSignal = TRUE;
					switch(Ret)
					{
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
						pdlg->m_statusbar = "写记录文件成功";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
						pdlg->m_statusbar = "写本条记录文件需安全会话";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
						pdlg->m_statusbar = "写记录文件失败";
						break;
					case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
						pdlg->m_statusbar = "写记录文件接收超时";
						break;
					default:
						pdlg->m_statusbar = "写记录文件未知错误";
						break;
					}
				}
			}
		}
		break;
	case TOKEN_UPDATE_STATE_NEV:
		pdlg->m_statusbar = "此文件不可以进行写操作";
		pdlg->m_SwitchSignal = TRUE;
		pdlg->GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		break;
	}
	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);//这里都要用sendmessage，不能用postmessage.
}


void CSerialDlg::OnBnClickedButton14()		//偏移量写文件
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}

	//"偏移量写文件"后按键变灰
	GetDlgItem(IDC_BUTTON14)->EnableWindow(FALSE);
	UpdateData(TRUE);

	m_writeCmdType = 1;
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("请先选择文件");
		GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_INVALID_MASK) == FILE_TYPE_INVALID)
	{
		MessageBox("该文件为无效文件");
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		MessageBox("该文件为目录文件");
		return;
	}
	switch(m_pSelectedFileAttr->fileAttr.securityType & TOKEN_UPDATE_MASK)
	{
	case TOKEN_UPDATE_STATE_ALLOW:
		if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//该文件为透明文件
		{
			char dataBuffer[8*1024];
			memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
			int Ret = rfWriteFile(0, m_FileOffset, m_writeFile.GetLength(), dataBuffer);
			switch(Ret)
			{
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
				m_statusbar = "写透明文件超时";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
				m_statusbar = "写透明文件失败";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
				m_statusbar = "写透明文件成功";
				break;
			default:
				m_statusbar = "写透明文件未知错误";
				break;
			}
		}
		else	//该文件为记录文件
		{
			if (m_writeFile.GetLength() > 108)
			{
				MessageBox("长度不能超过最大缓冲区108");
				return;
			}

			char dataBuffer[109];
			memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
			int Ret = rfWriteFile(1, m_FileOffset, m_writeFile.GetLength(), dataBuffer);
			switch(Ret)
			{
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
				m_statusbar = "写记录文件成功";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
				m_statusbar = "写本条记录文件需安全会话";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
				m_statusbar = "写记录文件失败";
				break;
			case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
				m_statusbar = "写记录文件接收超时";
				break;
			default:
				m_statusbar = "写记录文件未知错误";
				break;
			}
		}
		break;

	case TOKEN_UPDATE_STATE_PWD:
		{
			if (m_pSelectedFileAttr->TokenUpdateState == FALSE)	//FALSE表示未进行密码验证
			{
				INT_PTR nRes;								//用于保存DoModal函数的返回值
				CPwdRFDlg pwdRFDlg;							//构造对话框类CPwdRFDlg的实例
				nRes = pwdRFDlg.DoModal();					//弹出对话框
				if (IDCANCEL == nRes)	return;				//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行

 				m_curVerifyPwdState.pListFileAttr = m_pSelectedFileAttr;

				int Ret = rfVerifyPwdFile(TOKEN_TYPE_UPDATE, theApp.m_pwd);
				switch(Ret)
				{
				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED:
					m_statusbar = "密码验证失败";
					break;
				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_SUCCESS:
					m_statusbar = "密码验证正确";
					break;
				default:
					m_statusbar = "密码验证未知错误";
					break;
				}
			}
			else
			{
				if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//该文件为透明文件
				{
					char dataBuffer[8*1024];
					memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
					int Ret = rfWriteFile(0, m_FileOffset, m_writeFile.GetLength(), dataBuffer);
					switch(Ret)
					{
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
						m_statusbar = "写透明文件超时";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
						m_statusbar = "写透明文件失败";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
						m_statusbar = "写透明文件成功";
						break;
					default:
						m_statusbar = "写透明文件未知错误";
						break;
					}
				}
				else	//该文件为记录文件
				{
					if (m_writeFile.GetLength() > 108)
					{
						MessageBox("长度不能超过最大缓冲区108");
						return;
					}

					char dataBuffer[109];
					memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
					int Ret = rfWriteFile(1, m_FileOffset, m_writeFile.GetLength(), dataBuffer);
					switch(Ret)
					{
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
						m_statusbar = "写记录文件成功";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
						m_statusbar = "写本条记录文件需安全会话";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
						m_statusbar = "写记录文件失败";
						break;
					case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
						m_statusbar = "写记录文件接收超时";
						break;
					default:
						m_statusbar = "写记录文件未知错误";
						break;
					}
				}
			}
		}break;
	case TOKEN_UPDATE_STATE_NEV:
		MessageBox("不允许写当前文件");
		GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedButton8() //创建文件
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	UpdateData(TRUE);
	m_statusbar.Empty();
	UpdateData(FALSE);
	Sleep(200); //休息200ms，使状态框动态打印信息变化可辨识。
	
	FILE_ATTRIBUTE_W  fileAttr_W;
	unsigned int tmpID = 0;
	char *strFileID = (LPSTR)(LPCTSTR)(m_FileID);
	tmpID = strtol(strFileID, NULL, 16); //十六进制转换函数

	if (tmpID >0xffff )
	{
		AfxMessageBox("文件ID超出范围,不能超过65535");
		m_SwitchSignal = TRUE;
		return;
	}
	if (tmpID < 0x3f00)
	{
		AfxMessageBox("创建的文件ID不能小于0x3f00");
		m_SwitchSignal = TRUE;
		return;
	}

	fileAttr_W.fileAttr_R.fileID = tmpID;
	
	CString Type,Valid;
	GetDlgItemText(IDC_COMBO6,Type);
	GetDlgItemText(IDC_COMBO7,Valid);
	char temp_type = NULL,temp_valid = NULL;

	if (Type == "目录DF" && Valid == "无效")
	{
		MessageBox("不能创建目录无效文件");
		m_SwitchSignal = TRUE;
		return;
	}
	else if(Type == "目录DF")
	{
		temp_type = FILE_TYPE_DF;
	}
	else if(Type == "透明EF")
	{
		temp_type = FILE_TYPE_TRANS;
	}
	else if(Type == "记录EF")
	{
		temp_type = FILE_TYPE_RECORD;
	}


	if(Valid == "有效")
	{
		temp_valid = 0x00;
	}
	else if(Valid == "无效")
	{
		temp_valid = FILE_INVALID_MASK;
	}

	fileAttr_W.fileAttr_R.fileTypeValid = (temp_valid | temp_type);

	if(Type == "目录DF")
	{
		if (m_maxRecordNum > 65532)
		{
			MessageBox("目录文件最大文件数不能大于65532");
			m_SwitchSignal = TRUE;
			return;
		}
		fileAttr_W.fileAttr_R.uFileAttr.MFDF.constMaxFileLen = 1;
		fileAttr_W.fileAttr_R.uFileAttr.MFDF.numsOfMaxFile = m_maxRecordNum/*+3*/;
		fileAttr_W.fileAttr_R.uFileAttr.MFDF.numsOfRealFile = 0;
		fileAttr_W.access_tokens.MFDF.list_token = m_pwdRdRearch;
		fileAttr_W.access_tokens.MFDF.valid_token = m_pwdValid;
	}

	else if(Type == "透明EF")
	{
		if (m_fileRealLen > m_maxFileLen)
		{
			MessageBox("实际文件长度不能大于最大文件长度");
			m_SwitchSignal = TRUE;
			return;
		}
		fileAttr_W.fileAttr_R.uFileAttr.transBinEF.maxFileLen = m_maxFileLen;
		fileAttr_W.fileAttr_R.uFileAttr.transBinEF.constMaxRecord = 1;
		fileAttr_W.fileAttr_R.uFileAttr.transBinEF.realFileLen = m_fileRealLen;
		fileAttr_W.access_tokens.transBinEF.read_token = m_pwdRdRearch;
		fileAttr_W.access_tokens.transBinEF.update_token = m_pwdUpdate;
		fileAttr_W.access_tokens.transBinEF.valid_token = m_pwdValid;
	}
	else if(Type == "记录EF")
	{
		if (m_maxFileLen > 108)
		{
			MessageBox("记录长度不能超过108");
			m_SwitchSignal = TRUE;
			return;
		}
		if (m_maxRecordNum >255)
		{
			MessageBox("最大记录数不能超过255");
			m_SwitchSignal = TRUE;
			return;
		}
		if (m_fileRealLen > m_maxRecordNum)
		{
			MessageBox("实际记录数不能大于最大记录数");
			m_SwitchSignal = TRUE;
			return;
		}
		fileAttr_W.fileAttr_R.uFileAttr.recEF.recordLen = m_maxFileLen;
		fileAttr_W.fileAttr_R.uFileAttr.recEF.maxRecordNums = m_maxRecordNum;
		fileAttr_W.fileAttr_R.uFileAttr.recEF.realRecordNums = m_fileRealLen;
		fileAttr_W.access_tokens.recEF.read_search_token = m_pwdRdRearch;
		fileAttr_W.access_tokens.recEF.update_token = m_pwdUpdate;
		fileAttr_W.access_tokens.recEF.valid_token = m_pwdValid;
	}
	// 	GetDlgItemText(IDC_COMBO8,SecurityType1);
	// 	GetDlgItemText(IDC_COMBO9,SecurityType2);
	// 	GetDlgItemText(IDC_COMBO10,SecurityType3);
	char SecurityMask = 0;
	SecurityMask = (m_cmbB2_3.GetCurSel() << 2) + (m_cmbUpdata.GetCurSel() << 4) + 
		(m_cmbAthState.GetCurSel() << 6);
	fileAttr_W.fileAttr_R.securityType = SecurityMask;

	int Ret = rfCreateFile(&fileAttr_W);
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_SUCCESS:
		m_statusbar = "创建文件成功";
		break;
	case IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED:
		m_statusbar = "创建文件失败";
		break;
	case IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_TIMEOUT:
		m_statusbar = "创建文件超时";
		break;
	case IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_EXISTING:	//文件已经存在
		m_statusbar = "创建文件已存在";
		break;
	default:
		m_statusbar = "创建文件未知错误";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedButton12()		//使文件有效
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("请先选择文件");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	m_statusbar.Empty();
	UpdateData(FALSE);

	BYTE newFileState = 1;
	m_validCmdType = 1;

	UpdateData(TRUE);
	
	int Ret = rfFileValidEn(1);//0是使文件无效，1是使文件有效
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_SUCCESS:
		{
			m_statusbar = "操作文件状态成功";
			if (m_validCmdType != -1)
			{
				if (m_validCmdType == 1)
				{
					m_pSelectedFileAttr->fileAttr.fileTypeValid &= (~FILE_INVALID_MASK);
				}
				else
					m_pSelectedFileAttr->fileAttr.fileTypeValid |= FILE_INVALID_MASK;

			}
			DisplayAllFilelistItems(m_listCtrlFile, m_pFileList);
		}
		break;
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_FILE_NOT_EXIST:
		m_statusbar = "请先选择文件";
		break;
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_NOT_ALLOWED:
		m_statusbar = "不能将目录文件置为无效或有效";
		break;
	default:
		m_statusbar = "操作文件状态失败";
		break;
	}
	UpdateData(FALSE);
}

void CSerialDlg::OnBnClickedButton13()		//文件无效置零
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("请先选择文件");
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		MessageBox("不能将目录文件置为无效");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	m_statusbar.Empty();
	UpdateData(FALSE);

	//填充相应的参数
	m_validCmdType = 0;
	UpdateData(TRUE);

	int Ret = rfFileValidEn(0);//0是使文件无效，1是使文件有效
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_SUCCESS:
		{
			m_statusbar = "操作文件状态成功";
			if (m_validCmdType != -1)
			{
				if (m_validCmdType == 1)
				{
					m_pSelectedFileAttr->fileAttr.fileTypeValid &= (~FILE_INVALID_MASK);
				}
				else
					m_pSelectedFileAttr->fileAttr.fileTypeValid |= FILE_INVALID_MASK;

			}
			DisplayAllFilelistItems(m_listCtrlFile,m_pFileList);
		}
		break;
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_FILE_NOT_EXIST:
		m_statusbar = "请先选择文件";
		break;
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_NOT_ALLOWED:
		m_statusbar = "不能将目录文件置为无效或有效";
		break;
	default:
		m_statusbar = "操作文件状态状态失败";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
}


// 插入链表（存文件）
void CSerialDlg::InsertFileList(PLIST_FILE_ATTR pFileAttr)
{
	PLIST_FILE_ATTR tmpPlist;
	if (m_pFileList==NULL)
	{
		m_pFileList = pFileAttr;
		m_pFileList->pNext = NULL;
		return;
	}
	tmpPlist = m_pFileList;
	while (tmpPlist->pNext)
	{
		tmpPlist = tmpPlist->pNext;
	}
	tmpPlist->pNext = pFileAttr;
	pFileAttr->pNext = NULL;
}
int CSerialDlg::GetFileListItemsNum(void)
{
	int nums = 0;
	PLIST_FILE_ATTR pListFileAttr = m_pFileList;
	if (m_pFileList == NULL)
	{
		return 0;
	}
	while (pListFileAttr)
	{
		nums++;
		pListFileAttr = pListFileAttr->pNext;
	}
	return  nums;
}

// 插入链表（存标签）
void CSerialDlg::InsertTagList(PTAG_ATTR pTagAttr)
{
	PTAG_ATTR tmpPlist;
	char TIDS[9] = {0};
	if (pTagAttr)
	{
		memcpy(TIDS,pTagAttr->TID,8);
		xprintf("TID:%s\n",TIDS);
	}
	if (m_pTagList == NULL)
	{
		m_pTagList = pTagAttr;
		m_pTagList->pNext = NULL;
		return;
	}
	tmpPlist = m_pTagList;
	while (tmpPlist->pNext)
	{
		tmpPlist = tmpPlist->pNext;
	}
	tmpPlist->pNext = pTagAttr;
	pTagAttr->pNext = NULL;
}


//插入链表(存货物信息)
void CSerialDlg::InsertGoodsList(PGOODS_INFO pGoodsInfo)
{
	PGOODS_INFO tmplist;
	if (m_pGoodsList == NULL)
	{
		m_pGoodsList = pGoodsInfo;
		m_pGoodsList->pNext = NULL;
		return;
	}
	tmplist = m_pGoodsList;
	while(tmplist->pNext)
	{
		tmplist = tmplist->pNext;
	}
	tmplist->pNext = pGoodsInfo;
	pGoodsInfo->pNext = NULL;
}



// 返回文件属性
PLIST_FILE_ATTR CSerialDlg::GetFileAttrByIndex(unsigned int nIndex)
{
	PLIST_FILE_ATTR tmpPlistFileAttr = m_pFileList;
	if (m_pFileList == NULL)
	{
		return NULL;
	}
	while(tmpPlistFileAttr)
	{
		if (tmpPlistFileAttr->nIndex == nIndex)
		{
			return tmpPlistFileAttr;
		}
		tmpPlistFileAttr = tmpPlistFileAttr->pNext;
	}
	return NULL;
}

PLIST_FILE_ATTR CSerialDlg::GetFileAttrByFileID(unsigned int FileID)
{
	PLIST_FILE_ATTR tmpPlistFileAttr = m_pFileList;
	if (m_pFileList == NULL)
	{
		return NULL;
	}
	while(tmpPlistFileAttr)
	{
		if (tmpPlistFileAttr->fileAttr.fileID == FileID)
		{
			return tmpPlistFileAttr;
		}
		tmpPlistFileAttr = tmpPlistFileAttr->pNext;
	}
	return NULL;
}

BOOL CSerialDlg::DeleteFileByFileID(unsigned int FileID)
{
	PLIST_FILE_ATTR tmpPlistFileAttr = m_pFileList;
	PLIST_FILE_ATTR tmpPlistFileAttr2 = tmpPlistFileAttr->pNext;
	if (m_pFileList == NULL)
	{
		return 0;
	}
	if (m_pFileList->fileAttr.fileID == FileID)
	{
		tmpPlistFileAttr= m_pFileList;
		m_pFileList = m_pFileList->pNext;
		free(tmpPlistFileAttr);
		return 1;
	}
	while(tmpPlistFileAttr2)
	{
		if (tmpPlistFileAttr2->fileAttr.fileID != FileID)
		{
			tmpPlistFileAttr2 = tmpPlistFileAttr2->pNext;
			tmpPlistFileAttr = tmpPlistFileAttr->pNext;
		}
		else
		{
			//删除工作
			tmpPlistFileAttr->pNext = tmpPlistFileAttr2->pNext;
			free(tmpPlistFileAttr2);
			return 1;
		}
	}
	return 0;
}

// 返回标签属性
PTAG_ATTR CSerialDlg::GetTagAttr(unsigned char nIndex)
{
	PTAG_ATTR tmpPTagAttr = m_pTagList;
	if (m_pTagList == NULL)
	{
		return NULL;
	}
	while(tmpPTagAttr)
	{
		if (tmpPTagAttr->nIndex == nIndex)
		{
			return tmpPTagAttr;
		}
		tmpPTagAttr = tmpPTagAttr->pNext;
	}
	return NULL;
}

//返回货物信息
PGOODS_INFO CSerialDlg::GetGoodsInfo(unsigned char nIndex)
{
	PGOODS_INFO tmpPGoodsInfo = m_pGoodsList;
	if (m_pGoodsList == NULL)
	{
		return NULL;
	}
	while(tmpPGoodsInfo)
	{
		if (tmpPGoodsInfo->nIndex == nIndex)
		{
			return tmpPGoodsInfo;
		}
		tmpPGoodsInfo = tmpPGoodsInfo->pNext;
	}
	return NULL;
}

// 清空文件属性链表
void CSerialDlg::ClearFileAttrList(void)
{
	PLIST_FILE_ATTR tmpPFileList,tmp1;
	tmpPFileList = m_pFileList;
	while (tmpPFileList)
	{
		tmp1 = tmpPFileList;
		tmpPFileList = tmpPFileList->pNext;
		free(tmp1);
	}
	m_pFileList = NULL;
	return;
}



// 清空标签属性链表
void CSerialDlg::ClearTagList(void)
{
	PTAG_ATTR tmpPTagList,tmp1;
	tmpPTagList = m_pTagList;
	while (tmpPTagList)
	{
		tmp1 = tmpPTagList;
		tmpPTagList = tmpPTagList->pNext;
		free(tmp1);
	}
	m_pTagList = NULL;
	return;
}


//清空货物信息链表
void CSerialDlg::ClearGoodsList(void)
{
	PGOODS_INFO tmpPGoodsList,tmp1;
	tmpPGoodsList = m_pGoodsList;
	while(tmpPGoodsList)
	{
		tmp1 = tmpPGoodsList;
		tmpPGoodsList = tmpPGoodsList->pNext;
		free(tmp1);
	}
	m_pGoodsList = NULL;
	return;
}


// 查找文件属性通过 文件ID
PLIST_FILE_ATTR CSerialDlg::GetFileAttrByID(unsigned short fileID)
{
	PLIST_FILE_ATTR pFileListAttr = m_pFileList;
	if (pFileListAttr == NULL)
	{
		return NULL;
	}
	while (pFileListAttr)//没有遍历到链表的最后,继续
	{
		if (pFileListAttr->fileAttr.fileID == fileID)
		{
			//如果找了相应的ID,则直接返回其属性
			return pFileListAttr;
		}
		//如果没有找到,则继续遍历下一个节点
		pFileListAttr = pFileListAttr->pNext;
	}
	return PLIST_FILE_ATTR(NULL);
}


void CSerialDlg::OnCbnSelchangeCombo6()
{
	// TODO: 在此添加控件通知处理程序代码

}


void CSerialDlg::OnCbnSelendokCombo6()
{
	// TODO: 在此添加控件通知处理程序代码

}


void CSerialDlg::OnCbnDblclkCombo6()
{
	// TODO: 在此添加控件通知处理程序代码
	CString tmpFileType;
	UpdateData(TRUE);
	GetDlgItemText(IDC_COMBO6,tmpFileType);
	if(tmpFileType == "目录DF")
	{
		m_staticFileMaxLen = "保留值:1";
		m_staticFileRecords = "目录:支持文件个数";
		m_staticFileRealLen = "目录:实际文件个数";
		m_SecurityAttrB2_3 = "列表";
		m_SecurityAttrB4_5 = "保留";
	}else if (tmpFileType == "透明EF")
	{
		m_staticFileMaxLen = "最大文件长度";
		m_staticFileRecords = "保留值:1";
		m_staticFileRealLen = "目录中实际文件个数";
		m_SecurityAttrB2_3 = "读取和查询";
		m_SecurityAttrB4_5 = "更新";
	}else if (tmpFileType == "记录EF")
	{
		m_staticFileMaxLen = "记录长度(255)";
		m_staticFileRecords = "最大记录数(255)";
		m_staticFileRealLen = "实际记录数";
		m_SecurityAttrB2_3 = "读取和查询";
		m_SecurityAttrB4_5 = "更新";
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnCbnCloseupCombo6()
{
	// TODO: 在此添加控件通知处理程序代码
	CString tmpFileType;
	UpdateData(TRUE);
	GetDlgItemText(IDC_COMBO6,tmpFileType);
	if(tmpFileType == "目录DF")
	{
		m_staticFileMaxLen = "保留值:1";
		m_staticFileRecords = "支持文件数";
		m_staticFileRealLen = "实际文件数";
		m_SecurityAttrB2_3 = "列表";
		m_SecurityAttrB4_5 = "保留";
	}else if (tmpFileType == "透明EF")
	{
		m_staticFileMaxLen = "最大文件长度";
		m_staticFileRecords = "保留值:1";
		m_staticFileRealLen = "实际文件长度";
		m_SecurityAttrB2_3 = "读取和查询";
		m_SecurityAttrB4_5 = "更新";
	}else if (tmpFileType == "记录EF")
	{
		m_staticFileMaxLen = "记录长度<=255";
		m_staticFileRecords = "最大记录数<256";
		m_staticFileRealLen = "实际记录数";
		m_SecurityAttrB2_3 = "读取和查询";
		m_SecurityAttrB4_5 = "更新";
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnEnChangeEdit4()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


// 写文件
//FileType:1透明,3记录
void CSerialDlg::WriteFile(int FileOffset, unsigned short WriteLength, char* pData,int FileType)
{
	//准备参数结构体,申请参数结构空间
	PWRTIE_FILE_THERAD_PARAM pWriteFileParam = (PWRTIE_FILE_THERAD_PARAM)::malloc(sizeof(WRITE_FILE_THREAD_PARAM));
	//置0
	memset(pWriteFileParam,0,sizeof(WRITE_FILE_THREAD_PARAM));
	//申请文件内容缓冲区
	pWriteFileParam->pBuf = (byte*)malloc(WriteLength+4);
	//置0
	memset(pWriteFileParam->pBuf,0,WriteLength+4);

	pWriteFileParam->FileLen = WriteLength;
	pWriteFileParam->FileOffset = FileOffset;
	memcpy(pWriteFileParam->pBuf,pData,WriteLength);
	pWriteFileParam->FileType = FileType;

	/*AfxBeginThread(&writeFileThread,pWriteFileParam);*/
}


char* CSerialDlg::FindRecord(char* p)
{
	if (p ==NULL)
	{
		return NULL;
	}
	while(*p)
	{
		if ( (*p == '\r') && (*(p+1)== '\n') )
		{
			*p = '\0';
			p += 2;
			return p;
		}
		p++;
	}
	return NULL;
}


void CSerialDlg::DisplayGoodsItem(CListCtrl &lstCtrl,int index,char* content)
{
	char idxBuff[10]={0};
	char *ptoAt = NULL;
	int itemCount = lstCtrl.GetItemCount();
	//m_listCtrlFile.InsertItem(nIndex2,strNumber);
	_itoa_s(itemCount,(char*)&idxBuff, 10,10);
	lstCtrl.InsertItem( itemCount ,idxBuff);
	ptoAt = content;
	for (int i= 0;i<6;i++)
	{
		while(*ptoAt != '@')ptoAt++;
		*ptoAt = 0;
		lstCtrl.SetItemText(itemCount,i+1,content);
		content = ptoAt+1;
		ptoAt = content;
	}
	lstCtrl.SetItemText(itemCount,7,content);	
	UpdateData(FALSE);
}


void CSerialDlg::GetSecurityTypeStr(char FileType,char Security,CString& OutputStr )
{

	OutputStr.Empty();
	if (FileType == FILE_TYPE_DF)
	{
		OutputStr += "ALW";
		switch(Security & TOKEN_LIST_MASK)
		{
		case TOKEN_LIST_STATE_ALLOW:
			OutputStr += "__ALW";break;
		case TOKEN_LIST_STATE_ATH:
			OutputStr += "__ATH";break;
		case TOKEN_LIST_STATE_PWD:
			OutputStr += "__PWD";break;
		case TOKEN_LIST_STATE_NEV:
			OutputStr += "__NEV";break;
		}
		OutputStr += "__***";
		switch(Security & TOKEN_VALID_MASK)
		{
		case TOKEN_VALID_STATE_ALLOW:
			OutputStr += "__ALW";break;
		case TOKEN_VALID_STATE_ATH:
			OutputStr += "__ATH";break;
		case TOKEN_VALID_STATE_PWD:
			OutputStr += "__PWD";break;
		case TOKEN_VALID_STATE_NEV:
			OutputStr += "__NEV";break;
		}
	}
	else
	{
		OutputStr += "ALW";
		switch(Security & TOKEN_RDSEARCH_MASK)
		{
		case TOKEN_RDSEARCH_STATE_ALLOW:
			OutputStr += "__ALW";break;
		case TOKEN_RDSEARCH_STATE_ATH:
			OutputStr += "__ATH";break;
		case TOKEN_RDSEARCH_STATE_PWD:
			OutputStr += "__PWD";break;
		case TOKEN_RDSEARCH_STATE_NEV:
			OutputStr += "__NEV";break;
		}
		switch(Security & TOKEN_UPDATE_MASK)
		{
		case TOKEN_UPDATE_STATE_ALLOW:
			OutputStr += "__ALW";break;
		case TOKEN_UPDATE_STATE_ATH:
			OutputStr += "__ATH";break;
		case TOKEN_UPDATE_STATE_PWD:
			OutputStr += "__PWD";break;
		case TOKEN_UPDATE_STATE_NEV:
			OutputStr += "__NEV";break;
		}
		switch(Security & TOKEN_VALID_MASK)
		{
		case TOKEN_VALID_STATE_ALLOW:
			OutputStr += "__ALW";break;
		case TOKEN_VALID_STATE_ATH:
			OutputStr += "__ATH";break;
		case TOKEN_VALID_STATE_PWD:
			OutputStr += "__PWD";break;
		case TOKEN_VALID_STATE_NEV:
			OutputStr += "__NEV";break;
		}
	}
}


void CSerialDlg::initCmdData(PC_CMD_DATA& pcCmdData,PC_CMD_TYPE cmdType)
{
	memset(&pcCmdData,0,sizeof(PC_CMD_DATA));
	pcCmdData.cmdType = cmdType;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
	return;
}


void CSerialDlg::DisplayAllFilelistItems(CListCtrl& lstctrl,PLIST_FILE_ATTR pFilelistAttribute)
{
	lstctrl.DeleteAllItems();
	if (pFilelistAttribute != NULL)
	{
		while(pFilelistAttribute)
		{
			DisplayOneFilelistItems(lstctrl,pFilelistAttribute);
			pFilelistAttribute = pFilelistAttribute->pNext;
		}
	}
}


void CSerialDlg::DisplayOneFilelistItems(CListCtrl& lstctrl,PLIST_FILE_ATTR pFilelistAttribute)
{
	CString strFileID("0");
	CString formatStr2("0");
	CString formatStr3("0");
	CString formatStr4("0");
	CString strSecurityType("0");
	CString strNumber("0");
	char temp_Type = NULL,temp_Valid = NULL;
	int nIndex2 = pFilelistAttribute->nIndex;

	//插入行并显示序号
	strNumber.Format("%d",pFilelistAttribute->nIndex);
	lstctrl.InsertItem(pFilelistAttribute->nIndex,strNumber);
	//显示文件ID
	strFileID.Format("%x",pFilelistAttribute->fileAttr.fileID);
	lstctrl.SetItemText(pFilelistAttribute->nIndex,1,strFileID);
	xprintf("文件ID:0x%x\t文件类型:%d",pFilelistAttribute->fileAttr.fileID,pFilelistAttribute->fileAttr.fileTypeValid);
	//显示文件类型,是否有效,三个长度域
	switch(pFilelistAttribute->fileAttr.fileTypeValid)
	{
	case FILE_TYPE_DF:
		{
			lstctrl.SetItemText(nIndex2,2,"目录文件");
			lstctrl.SetItemText(nIndex2,3,"有效");
			formatStr2.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.MFDF.constMaxFileLen);
			lstctrl.SetItemText(nIndex2,4,formatStr2);
			formatStr3.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.MFDF.numsOfMaxFile);
			lstctrl.SetItemText(nIndex2,5,formatStr3);
			formatStr4.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.MFDF.numsOfRealFile);
			lstctrl.SetItemText(nIndex2,6,formatStr4);
		}
		break;
	case (FILE_TYPE_DF|FILE_INVALID_MASK):
		{
			lstctrl.SetItemText(nIndex2,2,"目录文件");
			lstctrl.SetItemText(nIndex2,3,"无效");
			formatStr2.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.MFDF.constMaxFileLen);
			lstctrl.SetItemText(nIndex2,4,formatStr2);
			formatStr3.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.MFDF.numsOfMaxFile);
			lstctrl.SetItemText(nIndex2,5,formatStr3);
			formatStr4.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.MFDF.numsOfRealFile);
			lstctrl.SetItemText(nIndex2,6,formatStr4);
		}
		break;
	case FILE_TYPE_TRANS:
		{
			lstctrl.SetItemText(nIndex2,2,"透明EF");
			lstctrl.SetItemText(nIndex2,3,"有效");
			formatStr2.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.transBinEF.maxFileLen);
			lstctrl.SetItemText(nIndex2,4,formatStr2);
			formatStr3.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.transBinEF.constMaxRecord);
			lstctrl.SetItemText(nIndex2,5,formatStr3);
			formatStr4.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.transBinEF.realFileLen);
			lstctrl.SetItemText(nIndex2,6,formatStr4);
		}
		break;
	case (FILE_TYPE_TRANS+FILE_INVALID_MASK):
		{
			lstctrl.SetItemText(nIndex2,2,"透明EF");
			lstctrl.SetItemText(nIndex2,3,"无效");
			formatStr2.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.transBinEF.maxFileLen);
			lstctrl.SetItemText(nIndex2,4,formatStr2);
			formatStr3.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.transBinEF.constMaxRecord);
			lstctrl.SetItemText(nIndex2,5,formatStr3);
			formatStr4.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.transBinEF.realFileLen);
			lstctrl.SetItemText(nIndex2,6,formatStr4);
		}
		break;
	case FILE_TYPE_RECORD:
		{
			lstctrl.SetItemText(nIndex2,2,"记录EF");
			lstctrl.SetItemText(nIndex2,3,"有效");
			formatStr2.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.recordLen);
			lstctrl.SetItemText(nIndex2,4,formatStr2);
			formatStr3.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.maxRecordNums);
			lstctrl.SetItemText(nIndex2,5,formatStr3);
			formatStr4.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.realRecordNums);
			lstctrl.SetItemText(nIndex2,6,formatStr4);
		}
		break;
	case FILE_TYPE_RECORD+FILE_INVALID_MASK:
		{
			lstctrl.SetItemText(nIndex2,2,"记录EF");
			lstctrl.SetItemText(nIndex2,3,"无效");
			formatStr2.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.recordLen);
			lstctrl.SetItemText(nIndex2,4,formatStr2);
			formatStr3.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.maxRecordNums);
			lstctrl.SetItemText(nIndex2,5,formatStr3);
			formatStr4.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.realRecordNums);
			lstctrl.SetItemText(nIndex2,6,formatStr4);
		}
		break;
	}
	//显示安全属性
	GetSecurityTypeStr( pFilelistAttribute->fileAttr.fileTypeValid&FILE_TYPE_MASK,
		pFilelistAttribute->fileAttr.securityType,
		strSecurityType);
	lstctrl.SetItemText(nIndex2,7,strSecurityType);
}


void CSerialDlg::ReadRecordFileItems(unsigned char ReadMode)
{
	if (m_pSelectedFileAttr == NULL)
	{
		AfxMessageBox("请选择一个文件");
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) != FILE_TYPE_RECORD)
	{
		AfxMessageBox("当前文件非记录文件");
		return;
	}

	int dataLen = (unsigned char)m_pSelectedFileAttr->fileAttr.uFileAttr.recEF.recordLen;
	WORD retLen;
	BYTE buff[1024*8] = {0};
	int Ret = rfReadRecFile(0, ReadMode, 0, dataLen, &retLen, buff);
	switch(Ret)
	{
	case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_FAILED:
		m_statusbar = "读记录文件，没选文件";
		break;
	case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_NEED_NEV_VERIFY:
		m_statusbar = "读记录文件，权限为NEV，访问被拒绝";
		break;
	case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_TIMEOUT:
		m_statusbar = "读记录文件超时";
		break;
	case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_SUCCESS:
		{
			m_statusbar = "读记录文件成功";
			CString tempbuf;
			tempbuf.Format("%s\r\n", buff);
			m_recFileContent += tempbuf;
		}
		break;
	default:
		m_statusbar = "读记录文件失败";
		break;
	}

	UpdateData(FALSE);
}


void CSerialDlg::OnCbnSetfocusCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CSerialDlg::OnBnClickedButton1()//返回主目录 
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	m_statusbar.Empty();
	UpdateData(FALSE);

	m_pTmpSelectedFileAttr =  &m_curMFDF;

	FILE_ATTRIBUTE_R  fileAtr_R;
	WORD fileID = 0x3F00;

	int Ret = rfSelectFile(fileID, &fileAtr_R);
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_SUCCESS:
		m_statusbar = "返回主目录成功";
		break;
	default:
		m_statusbar = "返回主目录失败";
		break;
	}

	m_bRetToMf = TRUE;
	UpdateData(FALSE);
}


void CSerialDlg::ReadLargeFile(void)		//读大文件
{
	PC_CMD_DATA pcCmdData;
	initCmdData(pcCmdData,PC_CMD_TYPE_READ_LARGE_FILE);
	pcCmdData.uParam.READ_LARGE_FILE_PARAM.fileAttrR.fileTypeValid = FILE_TYPE_TRANS;
	pcCmdData.uParam.READ_LARGE_FILE_PARAM.fileAttrR.uFileAttr.transBinEF.realFileLen = m_pSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen;
	//	pcCmdData.uParam.READ_LARGE_FILE_PARAM.uReadPram.transBinFile.dataLen = dataLen;
	send_cmd(&pcCmdData,sizeof(pcCmdData));
}


bool CSerialDlg::enum_reg_key_value(HKEY hKey, LPCTSTR lpSubKey)	// 由注册表搜端口号
{
#define MAX_VALUE_NAME 16383
	HKEY    hOpenKey;
	long    lResult = 0;
	CHAR    achClass[MAX_PATH];
	DWORD   cchClassNameLen;
	DWORD   cMaxClassLen;
	DWORD   cSubKeysNum;
	DWORD   cbMaxSubKeyLen;
	DWORD   cbValuesNum;
	DWORD   cbMaxValueNameLen;
	DWORD   cbMaxValueLen;
	DWORD   cbSecurityDescriptor;
	FILETIME ftLastWriteTime;
	CHAR  achValue[MAX_VALUE_NAME];
	DWORD cbValue = MAX_VALUE_NAME;
	DWORD DataType;
	BYTE  Data[255];
	DWORD DataLen=255;
	CString StrData = "";
	lResult = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hOpenKey);
	if(ERROR_SUCCESS != lResult)
	{
		return FALSE;
	}
	RegQueryInfoKey(hOpenKey, //已打开的键的句柄
		achClass, //类型名称
		&cchClassNameLen, //类型名称的长度
		NULL, //保留
		&cSubKeysNum, // 返回子键的数目
		&cbMaxSubKeyLen, // 返回最长的子键长度
		&cMaxClassLen, // 返回最长的类长度
		&cbValuesNum, // 返回值的数目
		&cbMaxValueNameLen, // 返回最长的值项,名称的长度
		&cbMaxValueLen, // 返回最长的值的长度
		&cbSecurityDescriptor, //返回安全描述
		&ftLastWriteTime); // 返回键最后被写入的时间
	for (DWORD j = 0, retValue = ERROR_SUCCESS; j < cbValuesNum; j++) 
	{
		cbValue = MAX_VALUE_NAME; 
		memset(achValue, 0, sizeof(achValue));
		memset(Data, 0, sizeof(Data));
		StrData = "";
		retValue = RegEnumValue(
			hOpenKey,
			j,
			achValue,
			&cbValue,
			NULL,
			&DataType,
			Data,
			&DataLen);

		if (retValue == (DWORD) ERROR_SUCCESS )
		{
			StrData.Format("%s", Data);
			m_Port.AddString(StrData);
		}
	}
	RegCloseKey(hOpenKey);
	return true;
#undef MAX_VALUE_NAME
}


void CSerialDlg::OnBnClickedButton16()		//打开\关闭蜂鸣器
{
	// TODO: 在此添加控件通知处理程序代码
	PC_CMD_DATA pcCmdData;
	initCmdData(pcCmdData,PC_CMD_TYPE_MONITOR_SENSOR_EN);
	pcCmdData.uParam.MONITOR_SENSOREN_PARAM.FileId = 0x00;
	pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorId = 0x01;
	if (m_pSelectedTag == NULL)
	{
		AfxMessageBox("未选中标签");
		return;
	}

	if (m_pSelectedTag->tagSpeakerState)	//打开状态，按钮显示“关闭”
	{
		pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorState = 0x00;
	}else{	//关闭状态，按钮显示“打开”
		pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorState = 0x01;
	}
	send_cmd(&pcCmdData,sizeof(pcCmdData));
}


void CSerialDlg::OnBnClickedRegisterTag()		//AES注册标签
{
	// TODO: 在此添加控件通知处理程序代码
	PC_CMD_DATA pcCmdData;
	initCmdData(pcCmdData,PC_CMD_TYPE_REGISTER_TAG);
	UpdateData(TRUE);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.TID,m_TID,8);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.u.AES_KEY.MIK,m_MIK,32);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.u.AES_KEY.MEK,m_MEK,32);
	pcCmdData.uParam.REGISTER_TAG_PARAM.keyType = 0;//0为AES，1为ECC
	send_cmd(&pcCmdData,sizeof(pcCmdData));
}


BOOL CSerialDlg::OnDisplay(unsigned int id, NMHDR* pNMHDR, LRESULT* pResult)	// 为工具栏按钮添加鼠标停留提示信息
{
	TOOLTIPTEXT *pTTT=(TOOLTIPTEXT *)pNMHDR;
	UINT nID=pNMHDR->idFrom;//获取工具栏按钮ID
	UINT nIndex=m_Toolbar.CommandToIndex(nID);//根据按钮ID获取索引
	switch(nIndex)
	{
	case 0:
		m_toolStr.Format("初始化标签");break;
	case 1:
		m_toolStr.Format("阅读器管理");break;
	}
	pTTT->lpszText=m_toolStr.GetBuffer(m_toolStr.GetLength());//设置按钮提示信息
	pTTT->hinst=AfxGetResourceHandle();
	return TRUE;
}


void CSerialDlg::popRegisterTag(void)			//弹出注册标签窗口
{
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//用于保存DoModal函数的返回值
	
	nRes = RegisterTagDlg.DoModal();			//弹出对话框
	if (IDCANCEL == nRes)						//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行
		return;
}


void CSerialDlg::OnBnClickedBtnGettaginfo()		//获取标签信息
{
	// TODO: 在此添加控件通知处理程序代码
}


void CSerialDlg::OnBnClickedBtnCleanalltag()	//清除所有标签
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}

	int Ret = rfCleanAllTag ();
	switch(Ret)
	{
	case IF_TYPE_CLR_ALL_TAG | CLR_ALL_REGISTER_TAG_STATE_SUCCESS:
		m_statusbar = "清除所有标签信息成功";
		break;
	case IF_TYPE_CLR_ALL_TAG | CLR_ALL_REGISTER_TAG_STATE_FAILED:
		m_statusbar = "清除所有标签信息失败";
		break;
	default:
		m_statusbar = "清楚所有标签信息unknown error";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedBtnWakeuponetag()	//唤醒单个标签
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//用于保存DoModal函数的返回值
	CPwdRFDlg pwdRFDlg;							//构造对话框类CPwdRFDlg的实例
	nRes = pwdRFDlg.DoModal();					//弹出对话框
	if (IDCANCEL == nRes)	return;				//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行
	UpdateData(TRUE);
	m_statusbar = "唤醒单个标签开始......";

	//"首地址写文件"按键变亮
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
	//"偏移量写文件"按键变亮
	GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
	ClearTagList();
	ClearFileAttrList();
	m_curNumOfTag = 0;
	m_listCtrlFile.DeleteAllItems();
	m_listCtrl.DeleteAllItems();		//清空列表中现有的item
	m_pSelectedTag = NULL;
	UpdateData(FALSE);

	TAG_INFO tagInfoArray;
	BYTE TID[8];
	memcpy(TID, m_writeFile, 8);

	int Ret = rfWakeupOneTag(TID, pwdRFDlg.m_pwd, &tagInfoArray);
	switch(Ret)
	{
	case IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_NOT_FOUND:
		m_statusbar = "唤醒单个标签未找着";
		break;
	case IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT:
		m_statusbar = "唤醒单个标签超时";
		break;
	case IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_SUCCESS:
		{
			GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);//列表点亮

			int nIndex=0;

			CString strNumber,tagStateStr,tidStr,uiiStr;
			BYTE tid[9];
			BYTE uii[32];
			tid[8] = '\0';
			PTAG_ATTR pTagAttr;

			//把数据显示到列表上
			nIndex = m_listCtrl.GetItemCount();
			strNumber.Format("%d", nIndex);//取得已有标签个数

			m_listCtrl.InsertItem(nIndex, strNumber);//插入新的一行,准备显示

			memcpy(tid, tagInfoArray.TID, TID_LEN);//取出回复帧中的TID,并格式化显示
			tidStr.Format("%c%c%c%c%c  [ %c%c%c ]",tid[0],tid[1],tid[2],tid[3],tid[4],tid[5],tid[6],tid[7]);
			m_listCtrl.SetItemText(nIndex, 1, tidStr);

			//标签状态格式化显示
			tagStateStr.Format("%d", tagInfoArray.tagState);
			m_listCtrl.SetItemText(nIndex, 2, tagStateStr);

			//UII
			memcpy(uii, tagInfoArray.UII, UII_LEN);
			uiiStr.Format("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				uii[0],uii[1],uii[2],uii[3],uii[4],uii[5],uii[6],uii[7],uii[8],uii[9],uii[10],
				uii[11],uii[12],uii[13],uii[14],uii[15],uii[16],uii[17],uii[18],uii[19],uii[20],
				uii[21],uii[22],uii[23],uii[24],uii[25],uii[26],uii[27],uii[28],uii[29],uii[30],uii[31]);
			m_listCtrl.SetItemText(nIndex, 3, uiiStr);

			//保存相应的标签信息数据
			pTagAttr = (PTAG_ATTR)malloc(sizeof(TAG_ATTR));//申请内存
			memset(pTagAttr, 0, sizeof(TAG_ATTR));//内存清零
			pTagAttr->nIndex = nIndex;
			pTagAttr->pNext = NULL;
			memcpy(pTagAttr->TID, tagInfoArray.TID, TID_LEN);
			InsertTagList(pTagAttr);
		}
		m_statusbar = "唤醒单个标签成功";
		break;
	default:
		m_statusbar = "唤醒单个标签失败";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::popReaderManager(void)			// 弹出阅读器管理
{
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//用于保存DoModal函数的返回值
	nRes = ReaderManageDlg.DoModal();			//弹出对话框
	if (IDCANCEL == nRes)						//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行
		return;
}


void CSerialDlg::OnBnClickedBtnFileDelete()		//文件删除
{
	// TODO: 在此添加控件通知处理程序代码	
	if (m_isSelected == FALSE)	//当文件未选中时，标志位m_isSelectec=0
	{
		MessageBox("请先选择文件");
		return;
	}
	UpdateData(TRUE);

	if (m_pSelectedFileAttr->fileAttr.fileID == 0xa101 ||
		m_pSelectedFileAttr->fileAttr.fileID == 0xa102 ||
		m_pSelectedFileAttr->fileAttr.fileID == 0xa103 ||
		m_pSelectedFileAttr->fileAttr.fileID == 0x3f00 ||
		m_pSelectedFileAttr->fileAttr.fileID == 0xf000)
	{
		MessageBox("系统文件不能删除");
		return;
	}
	if (m_curMFDF.fileAttr.fileID == m_CurrentSelectFileID)
	{
		m_bRetToMf = true;
	}

	m_statusbar.Empty();
	UpdateData(FALSE);

	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	int Ret = rfFileDelete();
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_SUCCESS:
		{
			if ((m_pSelectedFileAttr->fileAttr.fileTypeValid&FILE_TYPE_MASK) == FILE_TYPE_DF)
			{
				ClearFileAttrList();
				m_pSelectedFileAttr = NULL;
				m_listCtrlFile.DeleteAllItems();
				m_statusbar = "文件删除成功";
				if (m_bRetToMf)
				{
					m_bRetToMf = false;
					m_curMFDF.fileAttr.fileID = 0x3F00;
					m_bMF = true;
				}
				UpdateData(FALSE);
				return;
			}
			if (!DeleteFileByFileID(m_pSelectedFileAttr->fileAttr.fileID))
			{
				m_statusbar = "文件删除失败";
				UpdateData(FALSE);
				return;
			}
			DisplayAllFilelistItems(m_listCtrlFile,m_pFileList);
			//m_pSelectedFileAttr = NULL;
			m_statusbar = "文件删除成功";
		}
		break;
	case IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_SYSFILE:
		m_statusbar = "系统文件不能删除";
		break;
	default:
		m_statusbar = "文件删除操作失败";
		break;
	}

	m_isSelected = FALSE;
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedBtnFilesysFormat()	//文件系统格式化
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	if (MessageBox("确定要格式化文件系统吗？","防止误操作",MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}
	m_statusbar.Empty();
	UpdateData(FALSE);

	int Ret = rfFileSysFormat(0);
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_SUCCESS:
		m_statusbar = "文件系统格式化成功";
		break;
	case IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_TIMEOUT:
		m_statusbar = "文件系统格式化超时";
		break;
	case IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_PWD_ERROR:
		m_statusbar = "文件系统格式化密码错误";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedBtnKillTag()		//标签灭活
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//用于保存DoModal函数的返回值
	CPwdRFDlg pwdRFDlg;							//构造对话框类CPwdRFDlg的实例
	nRes = pwdRFDlg.DoModal();					//弹出对话框
	if (IDCANCEL == nRes)	return;				//判断对话框退出后返回值是否为IDCANCEL,如果是则return，否则继续向下执行

	UpdateData(TRUE);
	if(pwdRFDlg.m_pwd == 0)
	{
		AfxMessageBox("灭活口令为0，灭活失败");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	m_statusbar.Empty();
	UpdateData(FALSE);

// 	HANDLE startThread;
// 	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)KillTagThreadFunc, this, 0, NULL);
// 	CloseHandle(startThread);

	int Ret = rfKillTag(pwdRFDlg.m_pwd);
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_KILL_TAG | KILL_TAG_SUCCESS:
		m_statusbar = "标签灭活成功";
		break;
	case IF_TYPE_KILL_TAG | KILL_TAG_FAILED:
		m_statusbar = "标签灭活失败";
		break;
	case IF_TYPE_KILL_TAG | KILL_TAG_TIMEOUT:
		m_statusbar = "标签灭活超时";
		break;
	case IF_TYPE_KILL_TAG | 3:
		m_statusbar = "标签灭活口令为零，不能灭活";
		break;
	default:
		m_statusbar = "标签灭活动口令未知错误";
		break;
	}
	UpdateData(FALSE);
}

void CSerialDlg::KillTagThreadFunc(LPVOID lparam)
{
	CSerialDlg *pDlg = (CSerialDlg*)lparam;

	int Ret = rfKillTag(0);
	pDlg->m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_KILL_TAG | KILL_TAG_SUCCESS:
		pDlg->m_statusbar = "标签灭活成功";
		break;
	case IF_TYPE_KILL_TAG | KILL_TAG_FAILED:
		pDlg->m_statusbar = "标签灭活失败";
		break;
	case IF_TYPE_KILL_TAG | KILL_TAG_TIMEOUT:
		pDlg->m_statusbar = "标签灭活超时";
		break;
	case IF_TYPE_KILL_TAG | 3:
		pDlg->m_statusbar = "标签灭活口令为零，不能灭活";
		break;
	default:
		pDlg->m_statusbar = "标签灭活动口令未知错误";
		break;
	}
	pDlg->SendMessage(MY_MESSAGE, NULL, NULL);
}

void CSerialDlg::OnBnClickedCancel()
{	
	// TODO: 在此添加控件通知处理程序代码
	HANDLE hFile = NULL;
	hFile =  
		::CreateFile("..\\lentestfile.txt",
					GENERIC_WRITE|GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{

		CString errcode;
		
		errcode.Format("error code:%d\n",GetLastError());  // 3:路径不对
		
		AfxMessageBox(errcode);
		CDialogEx::OnCancel();
		return;
	}
	CString as;
	DWORD dwWrite;
	CString tmp;
	as.Format("lendIndex = %d\tlenloop = %d\n\r",lenIndex,lenloop);
	::WriteFile(hFile,as.GetBuffer(),as.GetLength(),&dwWrite,NULL);
	for (int i = 0; i<lenArrayLen; i++)
	{
		
		tmp.Format("len[%d]: %d\r\n",i,lenArrayTest[i]);
		
		::WriteFile(hFile, tmp.GetBuffer(), tmp.GetLength(), &dwWrite, NULL);
	}
	CloseHandle(hFile);
	CDialogEx::OnCancel();
}


// 同步收数据帧函数
int CSerialDlg::GetCmdReplyFrameData(void* pData, int bufLen)
{
	return 0;
}
// 同步收数据函数
int CSerialDlg::GetCmdReplyDataSync(void* pData, int bufLen)
{
	return 0;
}


//更新系统口令
void CSerialDlg::OnBnClickedBtnUpdatePwd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}

	UpdateData(TRUE);
	WORD PwdIndex = 0;
	char *strPwdIndex = (LPSTR)(LPCTSTR)(m_PwdIndex);
	PwdIndex = strtol(strPwdIndex, NULL, 16); //十六进制转换函数

	if ((PwdIndex > 4) && (PwdIndex < 16128))
	{
		MessageBox("0x3f00以内的文件不允许修改密码，故索引不合法");
		return;
	}

	BYTE mode = ((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->GetCurSel()+1;
	int Ret = rfUpdatePwd(m_adminPwd, PwdIndex, mode, m_newPwd);
	switch(Ret)
	{
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_SUCCESS:
		m_statusbar = "更新系统口令成功";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_FAILED:
		m_statusbar = "更新系统口令失败";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_TIMEOUT:
		m_statusbar = "更新系统口令超时";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_ADMIN_PWD_NOT_RIGHT:
		m_statusbar = "更新系统口令管理员密码不匹配";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_INVALID_NEW_PWD:
		m_statusbar = "更新系统口令新口令不合法";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_NOT_AUTHORISED:
		m_statusbar = "更新系统口令不在安全会话";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_UNKNOWN:
		m_statusbar = "更新系统口令未知错误";
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CSerialDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 if(IDOK != MessageBox("真的想退出吗,再想想?","!!!!退出程序警告!!!!",MB_OKCANCEL))
			 return FALSE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSerialDlg::OnBnClickedOpensocket()//打开网口通信
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_CommuFlag.GetCurSel();
	CString strFlag;
	m_CommuFlag.GetLBText(nIndex, strFlag);

	if(strFlag == "TCP")
	{
// 		CSetSocketDlg Dlg;
// 		Dlg.DoModal();

		UpdateData(TRUE);

		HANDLE startThread;
		startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SocketConnectThreadFunc, this, 0, NULL);

		DWORD Ret = WaitForSingleObject(startThread, 1000);
		if (Ret == WAIT_TIMEOUT)
		{
			m_statusbar = "打开网口通信失败";

			ClearTagList();
			ClearFileAttrList();
			m_recFileContent.Empty(); //清空读文件control数据
			m_writeFile.Empty();
			m_listCtrlFile.DeleteAllItems();
			m_listCtrl.DeleteAllItems();		//清空列表中现有的item

			UpdateData(FALSE);
			return;
		}

		GetDlgItem(IDC_OpenSocket)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);//盘存
		m_statusbar = "打开网口通信成功";
		m_OpenCommunication = TRUE;
		m_SwitchSignal = TRUE; //true表示可以触发消息事件，false表示不能触发

		ClearTagList();
		ClearFileAttrList();
		m_recFileContent.Empty(); //清空读文件control数据
		m_writeFile.Empty();
		m_listCtrlFile.DeleteAllItems();
		m_listCtrl.DeleteAllItems(); //清空列表中现有的item

		UpdateData(FALSE);
	}

	if(strFlag == "USB")
	{
		UpdateData(TRUE);

		if(0 != SearchHidDevice())
		{
			m_OpenCommunication = TRUE;
			m_SwitchSignal = TRUE; //true表示可以触发消息事件，false表示不能触发
			m_statusbar = "打开USB通信成功！";
			GetDlgItem(IDC_OpenSocket)->EnableWindow(FALSE);

			ClearTagList();
			ClearFileAttrList();
			m_recFileContent.Empty(); //清空读文件control数据
			m_writeFile.Empty();
			m_listCtrlFile.DeleteAllItems();
			m_listCtrl.DeleteAllItems();		//清空列表中现有的item

			UpdateData(FALSE);
		}
		else
		{
			m_statusbar = "打开USB通信失败！";

			ClearTagList();
			ClearFileAttrList();
			m_recFileContent.Empty(); //清空读文件control数据
			m_writeFile.Empty();
			m_listCtrlFile.DeleteAllItems();
			m_listCtrl.DeleteAllItems();		//清空列表中现有的item

			UpdateData(FALSE);
		}
	}

// 	HANDLE startThread;
// 	InitializeCriticalSection(&g_cs); //初始化全局关键段
// 	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BeginMsgEventThread, this, 0, NULL);
// 	CloseHandle(startThread);
}

void CSerialDlg::SocketConnectThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam;

	char *addressIP = (LPSTR)(LPCTSTR)(pdlg->m_IP);//将CString类型转换为char*类型
	int Ret = StartConnect(pdlg->m_IPPort, addressIP);
	if(Ret == 0)
	{
		Sleep(1500);
	}
}

void CSerialDlg::BeginMsgEventThread(LPVOID lparam)  //辅助消息处理线程BY PPWU 11-14
{
	MsgEventID.clear(); //清空容器内的所有元素
	MsgEventID.push_back(-1);  //初始化消息事件ID，-1表示没有事件要处理

	while(1)
	{
		if (MsgEventID.empty() != TRUE)
		{
			//进入临界区
			EnterCriticalSection(&g_cs);
			HANDLE startThread;

			switch(MsgEventID.front())
			{
			case COLLECT_MSG_ID:
				{
					startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CollectTagThreadFunc, lparam, 0, NULL);
					CloseHandle(startThread);
				}
				break;
			case SELECTTAG_MSG_ID:
				{
					startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SelectTagThreadFunc, lparam, 0, NULL);
					CloseHandle(startThread);
				}
				break;
			default:
				break;
			}

			MsgEventID.pop_front();
			//离开临界区
			LeaveCriticalSection(&g_cs);
		}

		if (MsgEventID.empty() == TRUE)
		{
			Sleep(500);
		}
	}
}

void CSerialDlg::OnBnClickedClosesocket()//关闭网口通信
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	int nIndex = m_CommuFlag.GetCurSel();
	CString strFlag;
	m_CommuFlag.GetLBText(nIndex, strFlag);

	if(strFlag == "TCP")
	{
		CloseConnect();//关闭套接字

		GetDlgItem(IDC_OpenSocket)->EnableWindow(TRUE);
		m_OpenCommunication = FALSE;//关闭通信端口标志bool变量，false表示关闭

		m_statusbar = "关闭通信成功";
		UpdateData(FALSE);
	}

	if(strFlag == "USB")
	{
		CloseHidDevice();//关闭全局句柄hCom
		GetDlgItem(IDC_OpenSocket)->EnableWindow(TRUE);
		m_OpenCommunication = FALSE;

		m_statusbar = "关闭通信成功";
		UpdateData(FALSE);
	}
}


//定义的自己的消息响应函数，函数功能用来调用主控窗口的Updatedata
afx_msg LRESULT CSerialDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return 0;
}

afx_msg LRESULT CSerialDlg::OnUpdatetrue(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	return 0;
}


void CSerialDlg::OnBnClickedReadertest()
{
	// TODO: 在此添加控件通知处理程序代码
	CReaderTestDlg Dlg;
	Dlg.DoModal();
}

void CSerialDlg::OnBnClickedCmdtest()
{
	// TODO: 在此添加控件通知处理程序代码
	CTestSingleCmd Dlg;
	Dlg.DoModal();
}


void CSerialDlg::OnBnClickedBtnSafeSession()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	m_statusbar.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	memset(&pcCmdData,0,sizeof(PC_CMD_DATA));
	pcCmdData.cmdType = PC_CMD_TYPE_SECURITY_COMMUNICATION;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;

	if(TestSendCmd(&pcCmdData) == -1)
	{
		m_statusbar = "安全会话命令发送失败";
		UpdateData(FALSE);
		return;
	}

	int Ret = TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_SECURITY_COMMUNICATION, TIME_OUT*2);
	m_SwitchSignal = TRUE;
	if(Ret != 1)
	{
		m_statusbar = "安全会话失败";
		UpdateData(FALSE);
		return;
	}
	switch(pcCmdReply.uReplyParam.securityCommunicationParam.securityCommunicationParam)
	{
	case SECURITY_COMMUNICATION_SUCCESS:
		m_statusbar = "安全会话成功";
		break;
	case SECURITY_COMMUNICATION_FAILED:
		m_statusbar = "安全会话失败";
		break;
	case SECURITY_COMMUNICATION_TIMEOUT:
		m_statusbar = "安全会话超时";
		break;
	default:
		m_statusbar = "安全会话未知错误";
		break;
	}
	
	UpdateData(FALSE);
}


int CSerialDlg::StrToHex(CString str, char *data) //字符串转换成十六进制字节
{
	int t, t1;
	int rlen = 0, len = str.GetLength();

	for(int i = 0; i < len; )
	{
		char l, h = str[i];
		if(h == ' ')
		{
			i++;
			continue;
		}
		i++;
		if(i >= len)
			break;
		l = str[i];
		t = HexChar(h);
		t1 = HexChar(l);
		if((t == 16) || (t1 == 16))
			break;
		else
			t = t * 16 + t1;
		i++;
		data[rlen] = (char)t;
		rlen++;
	}

	return rlen;
}


char CSerialDlg::HexChar(char c)
{
	if((c >= '0') && (c <= '9'))
		return c - 0x30;
	else if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	else if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	else
		return 0x10;
}

void CSerialDlg::OnBnClickedSearchrecord() //检索记录
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("请先选择文件");
		return;
	}

	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	UpdateData(TRUE);
	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SearchRecordThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CSerialDlg::SearchRecordThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //获得主窗口指针

	WORD rRecordNum;
	SEARCH_ELEMENT searchElement;

	char buffer[74];
	int DataLen;
	DataLen = pdlg->StrToHex(pdlg->m_SearchCondition.GetBuffer(), buffer);
	searchElement.logicalOperator = buffer[0]; //十六进制转换函数
	searchElement.fieldOffset = buffer[1];
	searchElement.relationOperator = buffer[2];
	searchElement.dataLen = buffer[3];
	memcpy(searchElement.dataBuf, buffer+4, DataLen-4);


	int Ret = rfSearchRecords(&searchElement, 1, &rRecordNum);
	pdlg->m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_SUCCESS:
		pdlg->m_statusbar.Format("检索记录成功，符合条件记录数：%d", rRecordNum);
		break;
	case IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED:
		pdlg->m_statusbar = "检索记录失败";
		break;
	case IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_TIMEOUT:
		pdlg->m_statusbar = "检索记录超时";
		break;
	default:
		pdlg->m_statusbar = "检索记录未知错误";
		break;
	}
	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
}

void CSerialDlg::OnBnClickedWakeup() //唤醒
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "请先打开通信端口";
		UpdateData(FALSE);
		return;
	}
	UpdateData(TRUE);
	if(m_WakeupNum > 10000)
	{
		AfxMessageBox("唤醒次数不要大于10000");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "上一个操作还在进行中，请等待......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	m_statusbar.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WakeUpThreadFunc, this, 0, NULL);
	CloseHandle(startThread);

// 	int ret = rfWakeUpTag(m_WakeupNum);
// 	m_SwitchSignal = TRUE;
// 	switch(ret)
// 	{
// 	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_SUCCESS:
// 		m_statusbar = "唤醒成功";
// 		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
// 		break;
// 	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_FAILED:
// 		m_statusbar = "唤醒失败";
// 		break;
// 	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_TIMEOUT:
// 		m_statusbar = "唤醒超时";
// 		break;
// 	default:
// 		m_statusbar = "唤醒未知错误";
// 		break;
// 	}
// 
// 	UpdateData(FALSE);
}

void CSerialDlg::WakeUpThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //获得主窗口指针

	int ret = rfWakeUpTag(pdlg->m_WakeupNum);
	pdlg->m_SwitchSignal = TRUE;
	switch(ret)
	{
	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_SUCCESS:
		pdlg->m_statusbar = "唤醒成功";
		pdlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		break;
	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_FAILED:
		pdlg->m_statusbar = "唤醒失败";
		break;
	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_TIMEOUT:
		pdlg->m_statusbar = "唤醒超时";
		break;
	default:
		pdlg->m_statusbar = "唤醒未知错误";
		break;
	}

	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
}