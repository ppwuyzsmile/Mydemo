//_commandEntries
// SerialDlg.cpp : ʵ���ļ�
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
#include "..\..\API����\APIDLL\apidll.h"
using namespace std;

extern BOOL isConnected;
char* baudTypeString[] = {
	"�����ʣ�9600\r\n",
	"�����ʣ�115200\r\n",
	"�����ʣ�128000\r\n"
	"�����ʣ�256000\r\n"
};
char* encryptTypeString[] = {
	"�������ͣ����\r\n",
	"�������ͣ���ϣ\r\n",
	"�������ͣ�AES\r\n",
	"�������ͣ�ECC\r\n"
};
char* fileSysTypeString[] = {
	"�����õ��ļ�ϵͳ���ͣ�δʹ���ļ�ϵͳ\r\n",
	"�����õ��ļ�ϵͳ���ͣ�FAT�ļ�ϵͳ\r\n",
	"�����õ��ļ�ϵͳ���ͣ�GJB�ļ�ϵͳ\r\n"
};
list<int> MsgEventID;  //ȫ����ϢID�б�
CRITICAL_SECTION g_cs; //ȫ�ֹؼ��Σ�������Ϣ�¼��̵߳�ͬ������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CSerialDlg �Ի���


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
		AfxMessageBox("��ʼ������ʧ��");
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
	ON_COMMAND(ID_BTN_TOOLBAR1,popRegisterTag)   //button1Ϊ��ʼ����ǩ��Ϣ������
	ON_COMMAND(ID_BTN_TOOLBAR2,popReaderManager) //button2Ϊ�Ķ���������Ϣ������
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


// CSerialDlg ��Ϣ�������

BOOL CSerialDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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
	printf("����һ��������Ϣ");
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
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

	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->AddString("1_�б�");
	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->AddString("2_��Ч��Ч");
	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->AddString("3_��/����");
	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->AddString("4_����");


	//Ĭ��ѡ����
	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(0);//COM��
	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(1);//������
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(0);//У��
	((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(3);//����λ����
	((CComboBox*)GetDlgItem(IDC_COMBO5))->SetCurSel(0);//ֹͣλ����

	((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->SetCurSel(0);//����ϵͳ����
	

	((CComboBox*)GetDlgItem(IDC_COMBO6))->AddString("Ŀ¼DF");
	((CComboBox*)GetDlgItem(IDC_COMBO6))->AddString("͸��EF");
	((CComboBox*)GetDlgItem(IDC_COMBO6))->AddString("��¼EF");

	((CComboBox*)GetDlgItem(IDC_COMBO7))->AddString("��Ч");
	((CComboBox*)GetDlgItem(IDC_COMBO7))->AddString("��Ч");

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


	m_staticFileMaxLen = "����ļ�����";
	m_staticFileRecords = "����ֵ:1";
	m_staticFileRealLen = "ʵ���ļ�����";
	m_SecurityAttrB2_3 = "��ȡ�Ͳ�ѯ";
	m_SecurityAttrB4_5 = "����";

	m_maxRecordNum = 1;
	m_maxFileLen = 1;
	m_fileRealLen = 0;

	UpdateData(FALSE);//���±༭������

	DWORD dwTagList_Style;
	dwTagList_Style = m_listCtrl.GetExtendedStyle();
	m_listCtrl.SetExtendedStyle(dwTagList_Style|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);//ѡ��ĳ��ʹ���и���&������չ���
	m_listCtrl.InsertColumn(0,"#");
	m_listCtrl.InsertColumn(1,"��ǩID");
	m_listCtrl.InsertColumn(2,"��ص���");
	m_listCtrl.InsertColumn(3,"UII");
	m_listCtrl.SetColumnWidth(0,30);
	m_listCtrl.SetColumnWidth(1,170);
	m_listCtrl.SetColumnWidth(2,60);
	m_listCtrl.SetColumnWidth(3,20);


	DWORD dwFileList_Style;
	dwFileList_Style = m_listCtrlFile.GetExtendedStyle();
	m_listCtrlFile.SetExtendedStyle(dwFileList_Style|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);//ѡ��ĳ��ʹ���и���&������չ���
	m_listCtrlFile.InsertColumn(0,"#");
	m_listCtrlFile.InsertColumn(1,"�ļ�ID");
	m_listCtrlFile.InsertColumn(2,"�ļ�����");
	m_listCtrlFile.InsertColumn(3,"��Ч��Ϣ");
	m_listCtrlFile.InsertColumn(4,"��󳤶�/��¼����");
	m_listCtrlFile.InsertColumn(5,"��������¼��");
	m_listCtrlFile.InsertColumn(6,"ʵ�ʳ���");
	m_listCtrlFile.InsertColumn(7,"[ѡ��][�б�/��ȡ��ѯ][����][��Ч/��Ч]");
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
	m_listCtrlGoods.SetExtendedStyle(dwGoodsList_Info|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);//ѡ��ĳ��ʹ���и���&������չ���
	m_listCtrlGoods.InsertColumn(0,"#");
	m_listCtrlGoods.InsertColumn(1,"����");
	m_listCtrlGoods.InsertColumn(2,"����");
	m_listCtrlGoods.InsertColumn(3,"�ͺ�");
	m_listCtrlGoods.InsertColumn(4,"�����ȼ�");
	m_listCtrlGoods.InsertColumn(5,"����");
	m_listCtrlGoods.InsertColumn(6,"��������");
	m_listCtrlGoods.InsertColumn(7,"������λ");
	m_listCtrlGoods.SetColumnWidth(0,30);
	m_listCtrlGoods.SetColumnWidth(1,70);
	m_listCtrlGoods.SetColumnWidth(2,50);
	m_listCtrlGoods.SetColumnWidth(3,60);
	m_listCtrlGoods.SetColumnWidth(4,60);
	m_listCtrlGoods.SetColumnWidth(5,50);
	m_listCtrlGoods.SetColumnWidth(6,80);
	m_listCtrlGoods.SetColumnWidth(7,60);


	m_curMFDF.fileAttr.fileID = 0x3F00;

	m_ImageList.Create(32,32,ILC_COLOR24|ILC_MASK,1,1);//ͼƬ��С����һ��
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

	m_Toolbar.CreateEx(this);//�����������ؼ�,CreateEx���������Ĺ�������Ĭ���и�����ť����
	m_Toolbar.EnableToolTips();//������Ϣ��ʾ����
	m_Toolbar.SetButtons(nArray,2);//�����������߰�ť������������ID�ţ���ӦnArray�����Ԫ��
	m_Toolbar.SetSizes(CSize(40,38),CSize(32,32));//������һ���ǰ�ť��С���ڶ�����ͼ���С����ť�����ͼ��Ҫ�󣬾����ǰ�ť��Ҫ��ͼ��Ŀ��7�������ϣ���6
	m_Toolbar.GetToolBarCtrl().SetImageList(&m_ImageList);//����ͼ��
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

	hComEvent = CreateEvent(NULL,FALSE,FALSE,"comDevice");
	SetEvent(hComEvent);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSerialDlg::OnPaint()
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	CRect rect;
	GetClientRect(&rect);
	CPaintDC *pDC = &dc;
	if (IsIconic())
	{
		//		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
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
		// 		// ������ͼ;
		// 		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		// 		CRect rect;
		// 		GetClientRect(&rect);
		// 		CDC  dcMem;  
		// 		dcMem.CreateCompatibleDC(&dc);///��������DC 
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

		//GDI+��ͼ


		CDialogEx::OnPaint();
	}
}


HBRUSH CSerialDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if( nCtlColor == CTLCOLOR_STATIC)      
	{    
		pDC->SetBkMode(TRANSPARENT);   //���ñ���͸�� 
		//		return   HBRUSH(GetStockObject(HOLLOW_BRUSH));
		hbr = (HBRUSH)(m_brush.GetSafeHandle());
	}
	//TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSerialDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSerialDlg::OnComm()
{
	// TODO: �ڴ���������������
}


void CSerialDlg::OnTimer(UINT_PTR nIDEvent)
{
	static int count = 0;
	PC_CMD_DATA pcCmdData;
	CSerialDlg::initCmdData(pcCmdData,PC_CMD_TYPE_CONNECT_TAG);
//	memcpy(pcCmdData.uParam.CONNECT_TAG.connectTag,"listen",6);
	switch(nIDEvent)
	{
	case 1:	//��ʱ��1
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
		//GetDlgItem(IDC_STATIC1)->SetWindowText("�̴��ǩ��ʱ");
		m_statusbar = "�̴��ǩ��ʱ";
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		UpdateData(FALSE);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
BEGIN_EVENTSINK_MAP(CSerialDlg, CDialogEx)
	/*ON_EVENT(CSerialDlg, IDC_MSCOMM1, 1, CSerialDlg::OnCommMscomm1, VTS_NONE)*/
END_EVENTSINK_MAP()




/*��������ɶ���Ҫ���ƣ�һ�������ķ����߼��޳�����by ppwu 11-12*/
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

void CSerialDlg::OnBnClickedButton2()		//�̴�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
	CPwdRFDlg pwdRFDlg;							//����Ի�����CPwdRFDlg��ʵ��
	nRes = pwdRFDlg.DoModal();					//�����Ի���
	if (IDCANCEL == nRes)	return;				//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	UpdateData(TRUE);
	m_statusbar = "�̴濪ʼ,���Ժ򡭡�����";
	//GetDlgItem(IDC_STATIC1)->SetWindowText("�̴濪ʼ,���Ժ򡭡�����");
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);//�̴�button
	//"�׵�ַд�ļ�"��������
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
	//"�б��ļ�"�������
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
	//"ƫ����д�ļ�"��������
	GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
	
	//SetTimer(123456,15000,NULL);

	ClearTagList();
	ClearFileAttrList();
	m_curNumOfTag = 0;

	m_recFileContent.Empty(); //��ն��ļ�control����
	m_writeFile.Empty();
	m_listCtrlFile.DeleteAllItems();
	m_listCtrl.DeleteAllItems();		//����б������е�item
	UpdateData(FALSE);

	m_pSelectedTag = NULL;

	/*MsgEventID.push_back(COLLECT_MSG_ID);*/
	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CollectTagThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CSerialDlg::CollectTagThreadFunc(LPVOID lparam)
{
	//�����̴�API
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //���������ָ��
	CPwdRFDlg  *pwdlg = new CPwdRFDlg;
	int nIndex = 0;
	vector<string> TidVector;
	int FlagNum = 1;
	BYTE tmpCollectNum = pdlg->m_CollectNum;
	
	while(tmpCollectNum--)
	{
		if(FlagNum != 1)
		{
			(pdlg->m_statusbar).Format("����ִ�е�%d���̴�......", FlagNum);
			pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			Sleep(5200); //ÿ���̴�֮����5s���˴�������0.2s���ų����
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
							tagInfo[nCount].TID[i] = 0xaf; //��ʱת��
							cmpFlag[i] = 1;
						}
					}

					string FindTid;
					FindTid = (char*)tagInfo[nCount].TID;
					FindTid.resize(8);
					FindTid.reserve(8); //����string���capacity�Ĵ�С�����������Ϊ8�����������Զ����䣬ÿ�η����ֵ���ܲ�һ��������find�������������

					vector<string>::const_iterator result = find(TidVector.begin(), TidVector.end(), FindTid);

					if(result == TidVector.end()) //find����û���ҵ��������ĵڶ���������ʵ��
					{
						TidVector.push_back(FindTid); //��û���ظ���push��������

						//......
						for(int i = 0; i < 8; i++)
						{
							if((tagInfo[nCount].TID[i] == 0xaf) && (cmpFlag[i] == 1))
								tagInfo[nCount].TID[i] = 0x00; //�ָ�ת��
						}

						//ִ����ʾ�������
						CString tidStr;
						nIndex = pdlg->m_listCtrl.GetItemCount();
						strNumber.Format("%d",nIndex);//ȡ�����б�ǩ����
						pdlg->m_listCtrl.InsertItem(nIndex,strNumber);//�����µ�һ��,׼����ʾ

						BYTE TidLen = strlen((char*)tagInfo[nCount].TID);
						for(int i = 0; i < 8; i++)
						{
							CString tmpTid;
							tmpTid.Format("%02x ", tagInfo[nCount].TID[i]);
							tidStr += tmpTid;
						}
						pdlg->m_listCtrl.SetItemText(nIndex, 1, tidStr);

						//��ǩ״̬��ʽ����ʾ
						tagStateStr.Format("%02x", tagInfo[nCount].tagState);
						pdlg->m_listCtrl.SetItemText(nIndex, 2, tagStateStr);

						//������Ӧ�ı�ǩ��Ϣ����
						pTagAttr = (PTAG_ATTR)malloc(sizeof(TAG_ATTR));//�����ڴ�
						memset(pTagAttr, 0, sizeof(TAG_ATTR));//�ڴ�����
						pTagAttr->nIndex = nIndex;
						pTagAttr->pNext = NULL;
						memcpy(pTagAttr->TID, tagInfo[nCount].TID,TID_LEN);
						pdlg->InsertTagList(pTagAttr);
					}
				}
				(pdlg->m_statusbar).Format("��%d��,�̴�ɹ�", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
			break;
		case IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_SEND_FAILED:
			{
				(pdlg->m_statusbar).Format("��%d��,�̴��ǩʧ��", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
			break;
		case IF_TYPE_COLLECT_TAG| COLLECT_TAG_STATE_TIMEOUT:
			{
				(pdlg->m_statusbar).Format("��%d��,�̴��Ķ����ظ�ʧ��", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
			break;
		case IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_EMPTY:
			{
				(pdlg->m_statusbar).Format("��%d��,û���̴浽��ǩ", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
			break;
		default:
			{
				(pdlg->m_statusbar).Format("��%d��,�̴�δ֪����", FlagNum);
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);//���ﶼҪ��sendmessage��������postmessage.
			}
			break;
		}
		FlagNum++; //��¼�̴����
	}
	pdlg->m_SwitchSignal = TRUE; //�ô��������¼���ɿ�ִ��״̬��False��ʾ�����¼����ɴ���
	pdlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
}


void CSerialDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	*pResult = 0;
}


void CSerialDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void CSerialDlg::OnBnClickedButton4()		//ѡ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}

	m_statusbar.Empty();
	m_recFileContent.Empty();
	m_writeFile.Empty();
	UpdateData(FALSE);

	//�ڱ�ǩ�б��л�þ���ĳ�е��к�
	CString str;
	unsigned int nId;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition(); //�õ������λ��
	if (pos == NULL)
	{
		MessageBox("������ѡ��һ���ǩ");
		return;
	}
	nId = (unsigned int)m_listCtrl.GetNextSelectedItem(pos);  //�õ��кţ�ͨ��POSITIONת��
	PTAG_ATTR pTagAttr = GetTagAttr(nId);
	if(pTagAttr == NULL)
	{
		AfxMessageBox("���ұ�ǩ����ʧ��");
		return;
	}

	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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

// 	//����ѡ���ǩAPI
// 	int Ret = rfSelectTag(m_ptmpSelectTag->TID);
// 
// 	switch(Ret)
// 	{
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SUCCESS:
// 		{
// 			m_pSelectedTag = m_ptmpSelectTag;
// 			m_statusbar = "ѡ���ǩ�ɹ�";
// 			ClearFileAttrList();
// 			m_listCtrlFile.DeleteAllItems();
// 			GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
// 			GetDlgItem(IDC_BUTTON16)->EnableWindow(TRUE);
// 			GetDlgItem(IDC_BTN_FILESYS_FORMAT)->EnableWindow(TRUE);
// 		}
// 		break;
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SEND_FAILED:
// 		m_statusbar = "ѡ���ǩ���ʹ���";
// 		break;
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_TIMEOUT:
// 		m_statusbar = "ѡ���ǩ��ʱ";
// 		break;
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_NOT_EXISTING:
// 		m_statusbar = "ѡ���ǩ������";
// 		break;
// 	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_ALREADY_SELECTED:
// 		m_statusbar = "��ǩ�Ѿ���ѡ��";
// 		break;
// 	default:
// 		m_statusbar = "ѡ���ǩδ֪����";
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
			pdlg->m_statusbar = "ѡ���ǩ�ɹ�";
			pdlg->ClearFileAttrList();
			pdlg->m_listCtrlFile.DeleteAllItems();
			pdlg->GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
			pdlg->GetDlgItem(IDC_BUTTON16)->EnableWindow(TRUE);
			pdlg->GetDlgItem(IDC_BTN_FILESYS_FORMAT)->EnableWindow(TRUE);
		}
		break;
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SEND_FAILED:
		pdlg->m_statusbar = "ѡ���ǩ���ʹ���";
		break;
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_TIMEOUT:
		pdlg->m_statusbar = "ѡ���ǩ��ʱ";
		break;
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_NOT_EXISTING:
		pdlg->m_statusbar = "ѡ���ǩ������";
		break;
	case IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_ALREADY_SELECTED:
		pdlg->m_statusbar = "��ǩ�Ѿ���ѡ��";
		break;
	default:
		pdlg->m_statusbar = "ѡ���ǩδ֪����";
		break;
	}

	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
}


void CSerialDlg::OnBnClickedButton6()    //�б��ļ�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON15)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_FILE_DELETE)->EnableWindow(FALSE);
	UpdateData(TRUE);
	ClearFileAttrList();//���ڿ�������Ŀ¼����add 2013-11-25
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
// 				m_statusbar = "�б��ļ��ɹ�";
// 				/******************************************/
// 				DisplayOneFilelistItems(m_listCtrlFile, pListFileAttr);
// 			}
// 		}
// 	    break;
// 	default:
// 		m_statusbar = "�б��ļ�ʧ��";
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
// 				pdlg->m_statusbar = "�б��ļ��ɹ�";
// 				/******************************************/
// 				pdlg->DisplayOneFilelistItems(pdlg->m_listCtrlFile, pListFileAttr);
// 			}
// 		}
// 		break;
// 	default:
// 		pdlg->m_statusbar = "�б��ļ�ʧ��";
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
				opMode = DERECTORY_LSIT_MODE_NEXT; //��������һ��

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

				pdlg->m_statusbar = "�б�δ��ɣ�����......";
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

				break;
			}
		case IF_TYPE_LIST_FILE | LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE:
			{
				pdlg->m_SwitchSignal = TRUE;
				Flag = FALSE;
				pdlg->m_statusbar = "�б�ɹ�";
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
				return;
			}
		default:
			{
				pdlg->m_SwitchSignal = TRUE;
				pdlg->m_statusbar = "�б��ļ�ʧ��";
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
				return;
			}
		}
	}
}


void CSerialDlg::OnBnClickedButton5()		//���ļ�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_recPoint = 0;
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	m_recFileContent.Empty();
	m_writeFile.Empty();
	m_statusbar.Empty();
	UpdateData(FALSE);

	if (m_pSelectedFileAttr == NULL)
	{
		MessageBox("��ѡ���ļ�");
		GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
		return;
	}

	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		MessageBox("���ļ�ΪĿ¼�ļ�����ѡ���б��ļ��鿴������");
		return;
	}

	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
// 						m_statusbar = "��͸���ļ���ûѡ�ļ�";
// 						break;
// 					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_TIMEOUT:
// 						m_statusbar = "��͸���ļ���ʱ";
// 						break;
// 					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NOT_EXISTING:
// 						m_statusbar = "��͸���ļ�������";
// 						break;
// 					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FINISH:
// 						{
// 							m_statusbar = "��ȡ͸���ļ����";
// 							CString tempbuf;
// 							tempbuf.Format("%s\r\n", buff);
// 							m_recFileContent += tempbuf;
// 						}
// 						break;
// 					default:
// 						m_statusbar = "��ȡ͸���ļ�ʧ��";
// 						break;
// 					}
// 					m_curReadState.readFinish = TRUE;
// 				}
// 				break;
// 			case FILE_TYPE_RECORD: //����¼�ļ�
// 				{
// 					ReadRecordFileItems(RECORD_IN_FILE_READ_MODE_FIX);
// 				}
// 				break;
// 			default:
// 				{
// 					m_statusbar = "���ļ�ΪĿ¼�ļ�����ѡ���б��ļ����鿴������";
// 					GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
// 					return;
// 				}
// 			}
// 		}
// 	default:
// 		m_statusbar = "�������ļ�ʧ��";
// 		break;
// 	}
// 
// 	UpdateData(FALSE);
}

void CSerialDlg::ReadFileThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //���������ָ��

	pdlg->m_statusbar = "���ļ����ڽ�����......";
	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

	pdlg->m_curReadState.pFileAttr = &pdlg->m_pSelectedFileAttr->fileAttr;
	switch(pdlg->m_pSelectedFileAttr->fileAttr.securityType & TOKEN_RDSEARCH_MASK )
	{
	case TOKEN_RDSEARCH_STATE_ALLOW:		//ALW
		{
			switch(pdlg->m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK)
			{
			case FILE_TYPE_TRANS: //��͸���ļ�
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
						pdlg->m_statusbar = "��͸���ļ�ʧ��";
						break;
					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_TIMEOUT:
						pdlg->m_statusbar = "��͸���ļ���ʱ";
						break;
					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NOT_EXISTING:
						pdlg->m_statusbar = "��͸���ļ�������";
						break;
					case IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FINISH:
						{
							for(int i = 0; i < pdlg->m_pSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen; i++)
							{
								CString tmpStr;
								tmpStr.Format("%02x", buff[i]);
								pdlg->m_recFileContent += tmpStr;
							}
							pdlg->m_statusbar = "��ȡ͸���ļ����";

							//CString tempbuf;
							//tempbuf.Format("%s\r\n", buff);
							//pdlg->m_recFileContent += tempbuf;
							//pdlg->m_statusbar = "��ȡ͸���ļ����";
						}
						break;
					default:
						pdlg->m_statusbar = "��ȡ͸���ļ�����";
						break;
					}
				}
				break;
			case FILE_TYPE_RECORD: //����¼�ļ�
				{
					//pdlg->ReadRecordFileItems(RECORD_IN_FILE_READ_MODE_FIX);
					if(pdlg->m_pSelectedFileAttr->fileAttr.uFileAttr.recEF.realRecordNums == 0)
					{
						pdlg->m_SwitchSignal = TRUE;
						pdlg->m_statusbar = "�ü�¼�ļ�û������";
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
							(pdlg->m_statusbar).Format("����%d����¼�ļ�ʧ��", RecordNum);
							pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
							break;
						case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_NEED_NEV_VERIFY:
							(pdlg->m_statusbar).Format("����%d����¼�ļ�ʧ��,Ȩ��ΪNEV�����ʱ��ܾ�", RecordNum);
							pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
							break;
						case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_TIMEOUT:
							(pdlg->m_statusbar).Format("����%d����¼�ļ���ʱ", RecordNum);
							pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
							break;
						case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_SUCCESS:
							{
								(pdlg->m_statusbar).Format("����%d����¼�ļ��ɹ�", RecordNum);
								pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

								CString tmpStr;
								for(int i = 0; i < pdlg->m_pSelectedFileAttr->fileAttr.uFileAttr.recEF.recordLen; i++)
								{
									tmpStr.Format("%02x", buff[i]);
									pdlg->m_recFileContent += tmpStr;
								}
								tmpStr.Format(" \r\n");
								pdlg->m_recFileContent += tmpStr;
								//(pdlg->m_statusbar).Format("����%d����¼�ļ��ɹ�", RecordNum);
								//pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
								//CString tempbuf;
								//tempbuf.Format("%s\r\n", buff);
								//pdlg->m_recFileContent += tempbuf;
							}
							break;
						default:
							pdlg->m_statusbar = "����¼�ļ���������";
							break;
						}
					}
				}
				break;
			default:
				{
					pdlg->m_statusbar = "���ļ�ΪĿ¼�ļ�����ѡ���б��ļ����鿴������";
					pdlg->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
					pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
					return;
				}
			}
		}
		break;
	default:
		pdlg->m_statusbar = "�������ļ�ʧ��";
		break;
	}

	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
}


void CSerialDlg::OnBnClickedButton15()		//�����ļ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}



void CSerialDlg::OnBnClickedButton10()		//��ǩ����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
	CSleepTagDlg sleepTagDlg;					//����Ի�����CSleepTagDlg��ʵ��
	nRes = sleepTagDlg.DoModal();					//�����Ի���
	if (IDCANCEL == nRes)						//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��
		return;
}


void CSerialDlg::OnBnClickedButton11()    //ѡ���ļ�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_statusbar.Empty();
	m_recFileContent.Empty();
	m_writeFile.Empty();
	UpdateData(FALSE);

	//���ļ��б��л�þ���ĳ�е��к�
	unsigned int nId;
	POSITION pos = m_listCtrlFile.GetFirstSelectedItemPosition(); //�õ������λ��
	if (pos == NULL)
	{
		MessageBox("������ѡ��һ��");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	nId = (unsigned int)m_listCtrlFile.GetNextSelectedItem(pos);//�õ��кţ�ͨ��POSITIONת��
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
				m_statusbar = "ѡ���ļ��ɹ�";
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
				if(m_bRetToMf)//��һ��ѡ�е�����Ŀ¼�ļ�,������Ŀ¼
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
				//"�׵�ַд�ļ�"��������
				GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
				//"ƫ����д�ļ�"��������
				GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
			}
			break;
		default:
			m_statusbar = "ѡ���ļ�ʧ��";
			break;
		}
	}
	else
	{
		m_isSelected = FALSE;
		AfxMessageBox("�����ļ������б�ʧ��");
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedButton7()	//д�ļ�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}

	m_statusbar.Empty();
	//"�׵�ַд�ļ�"�󰴼����
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
	UpdateData(TRUE);

	m_writeCmdType = 0;
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("����ѡ���ļ�");
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_INVALID_MASK) == FILE_TYPE_INVALID)
	{
		MessageBox("���ļ�Ϊ��Ч�ļ�");
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		MessageBox("���ļ�ΪĿ¼�ļ�");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
// 		if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//���ļ�Ϊ͸���ļ�
// 		{
// 			char dataBuffer[8*1024];
// 			m_pSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen = m_writeFile.GetLength();
// 			memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
// 			int Ret = rfWriteFile(0, 0, m_writeFile.GetLength(), dataBuffer);
// 			switch(Ret)
// 			{
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
// 				m_statusbar = "д͸���ļ���ʱ";
// 				break;
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
// 				m_statusbar = "д͸���ļ�ʧ��";
// 				break;
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
// 				m_statusbar = "д͸���ļ��ɹ�";
// 				break;
// 			default:
// 				m_statusbar = "д͸���ļ�δ֪����";
// 				break;
// 			}
// 		}
// 		else{	//���ļ�Ϊ��¼�ļ�
// 			if (m_writeFile.GetLength() > 109)
// 			{
// 				MessageBox("���Ȳ��ܳ�����󻺳���109������ʵҲ��Ӧ�ó���80");
// 				return;
// 			}
// 			
// 			char dataBuffer[109];
// 			memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
// 			int Ret = rfWriteFile(1, 0, m_writeFile.GetLength(), dataBuffer);
// 			switch(Ret)
// 			{
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
// 				m_statusbar = "д��¼�ļ��ɹ�";
// 				break;
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
// 				m_statusbar = "д������¼�ļ��谲ȫ�Ự";
// 				break;
// 			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
// 				m_statusbar = "д��¼�ļ�ʧ��";
// 				break;
// 			case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
// 				m_statusbar = "д��¼�ļ����ճ�ʱ";
// 				break;
// 			default:
// 				m_statusbar = "д��¼�ļ�δ֪����";
// 				break;
// 			}
// 		}
// 		break;
// 
// 	case TOKEN_UPDATE_STATE_PWD:
// 		{
// 			if (m_pSelectedFileAttr->TokenUpdateState == FALSE)	//FALSE��ʾδ����������֤
// 			{
// 				INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
// 				CPwdRFDlg pwdRFDlg;							//����Ի�����CPwdRFDlg��ʵ��
// 				nRes = pwdRFDlg.DoModal();					//�����Ի���
// 				if (IDCANCEL == nRes)	return;				//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��
// 
// 				m_curVerifyPwdState.pListFileAttr = m_pSelectedFileAttr;
// 
// 				int Ret = rfVerifyPwdFile(TOKEN_TYPE_UPDATE, theApp.m_pwd);
// 				switch(Ret)
// 				{
// 				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED:
// 					m_statusbar = "������֤ʧ��";
// 					break;
// 				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_SUCCESS:
// 					m_statusbar = "������֤��ȷ";
// 					break;
// 				default:
// 					m_statusbar = "������֤δ֪����";
// 					break;
// 				}
// 			}
// 			else
// 			{
// 				if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//���ļ�Ϊ͸���ļ�
// 				{
// 					char dataBuffer[8*1024];
// 					memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
// 					int Ret = rfWriteFile(0, 0, m_writeFile.GetLength(), dataBuffer);
// 					switch(Ret)
// 					{
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
// 						m_statusbar = "д͸���ļ���ʱ";
// 						break;
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
// 						m_statusbar = "д͸���ļ�ʧ��";
// 						break;
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
// 						m_statusbar = "д͸���ļ��ɹ�";
// 						break;
// 					default:
// 						m_statusbar = "д͸���ļ�δ֪����";
// 						break;
// 					}
// 				}
// 				else	//���ļ�Ϊ��¼�ļ�
// 				{
// 					if (m_writeFile.GetLength() > 109)
// 					{
// 						MessageBox("���Ȳ��ܳ�����󻺳���109������ʵҲ��Ӧ�ó���80");
// 						return;
// 					}
// 
// 					char dataBuffer[109];
// 					memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
// 					int Ret = rfWriteFile(1, 0, m_writeFile.GetLength(), dataBuffer);
// 					switch(Ret)
// 					{
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
// 						m_statusbar = "д��¼�ļ��ɹ�";
// 						break;
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
// 						m_statusbar = "д������¼�ļ��谲ȫ�Ự";
// 						break;
// 					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
// 						m_statusbar = "д��¼�ļ�ʧ��";
// 						break;
// 					case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
// 						m_statusbar = "д��¼�ļ����ճ�ʱ";
// 						break;
// 					default:
// 						m_statusbar = "д��¼�ļ�δ֪����";
// 						break;
// 					}
// 				}
// 			}
// 		}
// 		break;
// 	case TOKEN_UPDATE_STATE_NEV:
// 		MessageBox("������д��ǰ�ļ�");
// 		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
// 		break;
// 	}
// 	UpdateData(FALSE);
}

void CSerialDlg::WriteFileThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //���������ָ��

	pdlg->m_statusbar = "д�ļ����ڽ�����......";
	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);

	switch(pdlg->m_pSelectedFileAttr->fileAttr.securityType & TOKEN_UPDATE_MASK)
	{
	case TOKEN_UPDATE_STATE_ALLOW:
		if ((pdlg->m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//���ļ�Ϊ͸���ļ�
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
				pdlg->m_statusbar = "д͸���ļ���ʱ";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
				pdlg->m_statusbar = "д͸���ļ�ʧ��";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
				pdlg->m_statusbar = "д͸���ļ��ɹ�";
				break;
			default:
				pdlg->m_statusbar = "д͸���ļ�δ֪����";
				break;
			}
		}
		else
		{	//���ļ�Ϊ��¼�ļ�
			char dataBuffer[256];
			int DataLen;
			DataLen = pdlg->StrToHex(pdlg->m_writeFile.GetBuffer(), dataBuffer);

			if (DataLen > 108)
			{
				AfxMessageBox("���Ȳ��ܳ�����󻺳���108");
				return;
			}

			int Ret = rfWriteFile(1, pdlg->m_FileOffset, DataLen, dataBuffer);
			pdlg->m_SwitchSignal = TRUE;
			switch(Ret)
			{
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
				pdlg->m_statusbar = "д��¼�ļ��ɹ�";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
				pdlg->m_statusbar = "д������¼�ļ��谲ȫ�Ự";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
				pdlg->m_statusbar = "д��¼�ļ�ʧ��";
				break;
			case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
				pdlg->m_statusbar = "д��¼�ļ����ճ�ʱ";
				break;
			default:
				pdlg->m_statusbar = "д��¼�ļ�δ֪����";
				break;
			}
		}
		break;

	case TOKEN_UPDATE_STATE_PWD:
		{
			if (pdlg->m_pSelectedFileAttr->TokenUpdateState == FALSE)	//FALSE��ʾδ����������֤
			{
				INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
				CPwdRFDlg pwdRFDlg;							//����Ի�����CPwdRFDlg��ʵ��
				nRes = pwdRFDlg.DoModal();					//�����Ի���
				if (IDCANCEL == nRes)	return;				//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��

				pdlg->m_curVerifyPwdState.pListFileAttr = pdlg->m_pSelectedFileAttr;

				int Ret = rfVerifyPwdFile(TOKEN_TYPE_UPDATE, theApp.m_pwd);
				pdlg->m_SwitchSignal = TRUE;
				switch(Ret)
				{
				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED:
					pdlg->m_statusbar = "������֤ʧ��";
					break;
				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_SUCCESS:
					pdlg->m_statusbar = "������֤��ȷ";
					break;
				default:
					pdlg->m_statusbar = "������֤δ֪����";
					break;
				}
			}
			else
			{
				if ((pdlg->m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//���ļ�Ϊ͸���ļ�
				{
					char dataBuffer[16*1024];
					int DataLen;
					DataLen = pdlg->StrToHex(pdlg->m_writeFile.GetBuffer(), dataBuffer);

					int Ret = rfWriteFile(0, pdlg->m_FileOffset, DataLen, dataBuffer);
					pdlg->m_SwitchSignal = TRUE;
					switch(Ret)
					{
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
						pdlg->m_statusbar = "д͸���ļ���ʱ";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
						pdlg->m_statusbar = "д͸���ļ�ʧ��";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
						pdlg->m_statusbar = "д͸���ļ��ɹ�";
						break;
					default:
						pdlg->m_statusbar = "д͸���ļ�δ֪����";
						break;
					}
				}
				else	//���ļ�Ϊ��¼�ļ�
				{
					char dataBuffer[256];
					int DataLen;
					DataLen = pdlg->StrToHex(pdlg->m_writeFile.GetBuffer(), dataBuffer);

					if (DataLen > 108)
					{
						AfxMessageBox("���Ȳ��ܳ�����󻺳���108");
						return;
					}

					int Ret = rfWriteFile(1, pdlg->m_FileOffset, DataLen, dataBuffer);
					pdlg->m_SwitchSignal = TRUE;
					switch(Ret)
					{
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
						pdlg->m_statusbar = "д��¼�ļ��ɹ�";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
						pdlg->m_statusbar = "д������¼�ļ��谲ȫ�Ự";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
						pdlg->m_statusbar = "д��¼�ļ�ʧ��";
						break;
					case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
						pdlg->m_statusbar = "д��¼�ļ����ճ�ʱ";
						break;
					default:
						pdlg->m_statusbar = "д��¼�ļ�δ֪����";
						break;
					}
				}
			}
		}
		break;
	case TOKEN_UPDATE_STATE_NEV:
		pdlg->m_statusbar = "���ļ������Խ���д����";
		pdlg->m_SwitchSignal = TRUE;
		pdlg->GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		break;
	}
	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);//���ﶼҪ��sendmessage��������postmessage.
}


void CSerialDlg::OnBnClickedButton14()		//ƫ����д�ļ�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}

	//"ƫ����д�ļ�"�󰴼����
	GetDlgItem(IDC_BUTTON14)->EnableWindow(FALSE);
	UpdateData(TRUE);

	m_writeCmdType = 1;
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("����ѡ���ļ�");
		GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_INVALID_MASK) == FILE_TYPE_INVALID)
	{
		MessageBox("���ļ�Ϊ��Ч�ļ�");
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		MessageBox("���ļ�ΪĿ¼�ļ�");
		return;
	}
	switch(m_pSelectedFileAttr->fileAttr.securityType & TOKEN_UPDATE_MASK)
	{
	case TOKEN_UPDATE_STATE_ALLOW:
		if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//���ļ�Ϊ͸���ļ�
		{
			char dataBuffer[8*1024];
			memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
			int Ret = rfWriteFile(0, m_FileOffset, m_writeFile.GetLength(), dataBuffer);
			switch(Ret)
			{
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
				m_statusbar = "д͸���ļ���ʱ";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
				m_statusbar = "д͸���ļ�ʧ��";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
				m_statusbar = "д͸���ļ��ɹ�";
				break;
			default:
				m_statusbar = "д͸���ļ�δ֪����";
				break;
			}
		}
		else	//���ļ�Ϊ��¼�ļ�
		{
			if (m_writeFile.GetLength() > 108)
			{
				MessageBox("���Ȳ��ܳ�����󻺳���108");
				return;
			}

			char dataBuffer[109];
			memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
			int Ret = rfWriteFile(1, m_FileOffset, m_writeFile.GetLength(), dataBuffer);
			switch(Ret)
			{
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
				m_statusbar = "д��¼�ļ��ɹ�";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
				m_statusbar = "д������¼�ļ��谲ȫ�Ự";
				break;
			case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
				m_statusbar = "д��¼�ļ�ʧ��";
				break;
			case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
				m_statusbar = "д��¼�ļ����ճ�ʱ";
				break;
			default:
				m_statusbar = "д��¼�ļ�δ֪����";
				break;
			}
		}
		break;

	case TOKEN_UPDATE_STATE_PWD:
		{
			if (m_pSelectedFileAttr->TokenUpdateState == FALSE)	//FALSE��ʾδ����������֤
			{
				INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
				CPwdRFDlg pwdRFDlg;							//����Ի�����CPwdRFDlg��ʵ��
				nRes = pwdRFDlg.DoModal();					//�����Ի���
				if (IDCANCEL == nRes)	return;				//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��

 				m_curVerifyPwdState.pListFileAttr = m_pSelectedFileAttr;

				int Ret = rfVerifyPwdFile(TOKEN_TYPE_UPDATE, theApp.m_pwd);
				switch(Ret)
				{
				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED:
					m_statusbar = "������֤ʧ��";
					break;
				case IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_SUCCESS:
					m_statusbar = "������֤��ȷ";
					break;
				default:
					m_statusbar = "������֤δ֪����";
					break;
				}
			}
			else
			{
				if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS)//���ļ�Ϊ͸���ļ�
				{
					char dataBuffer[8*1024];
					memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
					int Ret = rfWriteFile(0, m_FileOffset, m_writeFile.GetLength(), dataBuffer);
					switch(Ret)
					{
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
						m_statusbar = "д͸���ļ���ʱ";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
						m_statusbar = "д͸���ļ�ʧ��";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
						m_statusbar = "д͸���ļ��ɹ�";
						break;
					default:
						m_statusbar = "д͸���ļ�δ֪����";
						break;
					}
				}
				else	//���ļ�Ϊ��¼�ļ�
				{
					if (m_writeFile.GetLength() > 108)
					{
						MessageBox("���Ȳ��ܳ�����󻺳���108");
						return;
					}

					char dataBuffer[109];
					memcpy(dataBuffer, m_writeFile.GetBuffer(), m_writeFile.GetLength());
					int Ret = rfWriteFile(1, m_FileOffset, m_writeFile.GetLength(), dataBuffer);
					switch(Ret)
					{
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS:
						m_statusbar = "д��¼�ļ��ɹ�";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY:
						m_statusbar = "д������¼�ļ��谲ȫ�Ự";
						break;
					case IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED:
						m_statusbar = "д��¼�ļ�ʧ��";
						break;
					case  IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT:
						m_statusbar = "д��¼�ļ����ճ�ʱ";
						break;
					default:
						m_statusbar = "д��¼�ļ�δ֪����";
						break;
					}
				}
			}
		}break;
	case TOKEN_UPDATE_STATE_NEV:
		MessageBox("������д��ǰ�ļ�");
		GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedButton8() //�����ļ�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	UpdateData(TRUE);
	m_statusbar.Empty();
	UpdateData(FALSE);
	Sleep(200); //��Ϣ200ms��ʹ״̬��̬��ӡ��Ϣ�仯�ɱ�ʶ��
	
	FILE_ATTRIBUTE_W  fileAttr_W;
	unsigned int tmpID = 0;
	char *strFileID = (LPSTR)(LPCTSTR)(m_FileID);
	tmpID = strtol(strFileID, NULL, 16); //ʮ������ת������

	if (tmpID >0xffff )
	{
		AfxMessageBox("�ļ�ID������Χ,���ܳ���65535");
		m_SwitchSignal = TRUE;
		return;
	}
	if (tmpID < 0x3f00)
	{
		AfxMessageBox("�������ļ�ID����С��0x3f00");
		m_SwitchSignal = TRUE;
		return;
	}

	fileAttr_W.fileAttr_R.fileID = tmpID;
	
	CString Type,Valid;
	GetDlgItemText(IDC_COMBO6,Type);
	GetDlgItemText(IDC_COMBO7,Valid);
	char temp_type = NULL,temp_valid = NULL;

	if (Type == "Ŀ¼DF" && Valid == "��Ч")
	{
		MessageBox("���ܴ���Ŀ¼��Ч�ļ�");
		m_SwitchSignal = TRUE;
		return;
	}
	else if(Type == "Ŀ¼DF")
	{
		temp_type = FILE_TYPE_DF;
	}
	else if(Type == "͸��EF")
	{
		temp_type = FILE_TYPE_TRANS;
	}
	else if(Type == "��¼EF")
	{
		temp_type = FILE_TYPE_RECORD;
	}


	if(Valid == "��Ч")
	{
		temp_valid = 0x00;
	}
	else if(Valid == "��Ч")
	{
		temp_valid = FILE_INVALID_MASK;
	}

	fileAttr_W.fileAttr_R.fileTypeValid = (temp_valid | temp_type);

	if(Type == "Ŀ¼DF")
	{
		if (m_maxRecordNum > 65532)
		{
			MessageBox("Ŀ¼�ļ�����ļ������ܴ���65532");
			m_SwitchSignal = TRUE;
			return;
		}
		fileAttr_W.fileAttr_R.uFileAttr.MFDF.constMaxFileLen = 1;
		fileAttr_W.fileAttr_R.uFileAttr.MFDF.numsOfMaxFile = m_maxRecordNum/*+3*/;
		fileAttr_W.fileAttr_R.uFileAttr.MFDF.numsOfRealFile = 0;
		fileAttr_W.access_tokens.MFDF.list_token = m_pwdRdRearch;
		fileAttr_W.access_tokens.MFDF.valid_token = m_pwdValid;
	}

	else if(Type == "͸��EF")
	{
		if (m_fileRealLen > m_maxFileLen)
		{
			MessageBox("ʵ���ļ����Ȳ��ܴ�������ļ�����");
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
	else if(Type == "��¼EF")
	{
		if (m_maxFileLen > 108)
		{
			MessageBox("��¼���Ȳ��ܳ���108");
			m_SwitchSignal = TRUE;
			return;
		}
		if (m_maxRecordNum >255)
		{
			MessageBox("����¼�����ܳ���255");
			m_SwitchSignal = TRUE;
			return;
		}
		if (m_fileRealLen > m_maxRecordNum)
		{
			MessageBox("ʵ�ʼ�¼�����ܴ�������¼��");
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
		m_statusbar = "�����ļ��ɹ�";
		break;
	case IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED:
		m_statusbar = "�����ļ�ʧ��";
		break;
	case IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_TIMEOUT:
		m_statusbar = "�����ļ���ʱ";
		break;
	case IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_EXISTING:	//�ļ��Ѿ�����
		m_statusbar = "�����ļ��Ѵ���";
		break;
	default:
		m_statusbar = "�����ļ�δ֪����";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedButton12()		//ʹ�ļ���Ч
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("����ѡ���ļ�");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
	
	int Ret = rfFileValidEn(1);//0��ʹ�ļ���Ч��1��ʹ�ļ���Ч
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_SUCCESS:
		{
			m_statusbar = "�����ļ�״̬�ɹ�";
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
		m_statusbar = "����ѡ���ļ�";
		break;
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_NOT_ALLOWED:
		m_statusbar = "���ܽ�Ŀ¼�ļ���Ϊ��Ч����Ч";
		break;
	default:
		m_statusbar = "�����ļ�״̬ʧ��";
		break;
	}
	UpdateData(FALSE);
}

void CSerialDlg::OnBnClickedButton13()		//�ļ���Ч����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("����ѡ���ļ�");
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		MessageBox("���ܽ�Ŀ¼�ļ���Ϊ��Ч");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	m_statusbar.Empty();
	UpdateData(FALSE);

	//�����Ӧ�Ĳ���
	m_validCmdType = 0;
	UpdateData(TRUE);

	int Ret = rfFileValidEn(0);//0��ʹ�ļ���Ч��1��ʹ�ļ���Ч
	m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_SUCCESS:
		{
			m_statusbar = "�����ļ�״̬�ɹ�";
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
		m_statusbar = "����ѡ���ļ�";
		break;
	case IF_TYPE_VALID_FILE | VALID_FILE_STATE_NOT_ALLOWED:
		m_statusbar = "���ܽ�Ŀ¼�ļ���Ϊ��Ч����Ч";
		break;
	default:
		m_statusbar = "�����ļ�״̬״̬ʧ��";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


// �����������ļ���
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

// �����������ǩ��
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


//��������(�������Ϣ)
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



// �����ļ�����
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
			//ɾ������
			tmpPlistFileAttr->pNext = tmpPlistFileAttr2->pNext;
			free(tmpPlistFileAttr2);
			return 1;
		}
	}
	return 0;
}

// ���ر�ǩ����
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

//���ػ�����Ϣ
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

// ����ļ���������
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



// ��ձ�ǩ��������
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


//��ջ�����Ϣ����
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


// �����ļ�����ͨ�� �ļ�ID
PLIST_FILE_ATTR CSerialDlg::GetFileAttrByID(unsigned short fileID)
{
	PLIST_FILE_ATTR pFileListAttr = m_pFileList;
	if (pFileListAttr == NULL)
	{
		return NULL;
	}
	while (pFileListAttr)//û�б�������������,����
	{
		if (pFileListAttr->fileAttr.fileID == fileID)
		{
			//���������Ӧ��ID,��ֱ�ӷ���������
			return pFileListAttr;
		}
		//���û���ҵ�,�����������һ���ڵ�
		pFileListAttr = pFileListAttr->pNext;
	}
	return PLIST_FILE_ATTR(NULL);
}


void CSerialDlg::OnCbnSelchangeCombo6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}


void CSerialDlg::OnCbnSelendokCombo6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}


void CSerialDlg::OnCbnDblclkCombo6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString tmpFileType;
	UpdateData(TRUE);
	GetDlgItemText(IDC_COMBO6,tmpFileType);
	if(tmpFileType == "Ŀ¼DF")
	{
		m_staticFileMaxLen = "����ֵ:1";
		m_staticFileRecords = "Ŀ¼:֧���ļ�����";
		m_staticFileRealLen = "Ŀ¼:ʵ���ļ�����";
		m_SecurityAttrB2_3 = "�б�";
		m_SecurityAttrB4_5 = "����";
	}else if (tmpFileType == "͸��EF")
	{
		m_staticFileMaxLen = "����ļ�����";
		m_staticFileRecords = "����ֵ:1";
		m_staticFileRealLen = "Ŀ¼��ʵ���ļ�����";
		m_SecurityAttrB2_3 = "��ȡ�Ͳ�ѯ";
		m_SecurityAttrB4_5 = "����";
	}else if (tmpFileType == "��¼EF")
	{
		m_staticFileMaxLen = "��¼����(255)";
		m_staticFileRecords = "����¼��(255)";
		m_staticFileRealLen = "ʵ�ʼ�¼��";
		m_SecurityAttrB2_3 = "��ȡ�Ͳ�ѯ";
		m_SecurityAttrB4_5 = "����";
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnCbnCloseupCombo6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString tmpFileType;
	UpdateData(TRUE);
	GetDlgItemText(IDC_COMBO6,tmpFileType);
	if(tmpFileType == "Ŀ¼DF")
	{
		m_staticFileMaxLen = "����ֵ:1";
		m_staticFileRecords = "֧���ļ���";
		m_staticFileRealLen = "ʵ���ļ���";
		m_SecurityAttrB2_3 = "�б�";
		m_SecurityAttrB4_5 = "����";
	}else if (tmpFileType == "͸��EF")
	{
		m_staticFileMaxLen = "����ļ�����";
		m_staticFileRecords = "����ֵ:1";
		m_staticFileRealLen = "ʵ���ļ�����";
		m_SecurityAttrB2_3 = "��ȡ�Ͳ�ѯ";
		m_SecurityAttrB4_5 = "����";
	}else if (tmpFileType == "��¼EF")
	{
		m_staticFileMaxLen = "��¼����<=255";
		m_staticFileRecords = "����¼��<256";
		m_staticFileRealLen = "ʵ�ʼ�¼��";
		m_SecurityAttrB2_3 = "��ȡ�Ͳ�ѯ";
		m_SecurityAttrB4_5 = "����";
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnEnChangeEdit4()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


// д�ļ�
//FileType:1͸��,3��¼
void CSerialDlg::WriteFile(int FileOffset, unsigned short WriteLength, char* pData,int FileType)
{
	//׼�������ṹ��,��������ṹ�ռ�
	PWRTIE_FILE_THERAD_PARAM pWriteFileParam = (PWRTIE_FILE_THERAD_PARAM)::malloc(sizeof(WRITE_FILE_THREAD_PARAM));
	//��0
	memset(pWriteFileParam,0,sizeof(WRITE_FILE_THREAD_PARAM));
	//�����ļ����ݻ�����
	pWriteFileParam->pBuf = (byte*)malloc(WriteLength+4);
	//��0
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

	//�����в���ʾ���
	strNumber.Format("%d",pFilelistAttribute->nIndex);
	lstctrl.InsertItem(pFilelistAttribute->nIndex,strNumber);
	//��ʾ�ļ�ID
	strFileID.Format("%x",pFilelistAttribute->fileAttr.fileID);
	lstctrl.SetItemText(pFilelistAttribute->nIndex,1,strFileID);
	xprintf("�ļ�ID:0x%x\t�ļ�����:%d",pFilelistAttribute->fileAttr.fileID,pFilelistAttribute->fileAttr.fileTypeValid);
	//��ʾ�ļ�����,�Ƿ���Ч,����������
	switch(pFilelistAttribute->fileAttr.fileTypeValid)
	{
	case FILE_TYPE_DF:
		{
			lstctrl.SetItemText(nIndex2,2,"Ŀ¼�ļ�");
			lstctrl.SetItemText(nIndex2,3,"��Ч");
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
			lstctrl.SetItemText(nIndex2,2,"Ŀ¼�ļ�");
			lstctrl.SetItemText(nIndex2,3,"��Ч");
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
			lstctrl.SetItemText(nIndex2,2,"͸��EF");
			lstctrl.SetItemText(nIndex2,3,"��Ч");
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
			lstctrl.SetItemText(nIndex2,2,"͸��EF");
			lstctrl.SetItemText(nIndex2,3,"��Ч");
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
			lstctrl.SetItemText(nIndex2,2,"��¼EF");
			lstctrl.SetItemText(nIndex2,3,"��Ч");
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
			lstctrl.SetItemText(nIndex2,2,"��¼EF");
			lstctrl.SetItemText(nIndex2,3,"��Ч");
			formatStr2.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.recordLen);
			lstctrl.SetItemText(nIndex2,4,formatStr2);
			formatStr3.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.maxRecordNums);
			lstctrl.SetItemText(nIndex2,5,formatStr3);
			formatStr4.Format("%d",pFilelistAttribute->fileAttr.uFileAttr.recEF.realRecordNums);
			lstctrl.SetItemText(nIndex2,6,formatStr4);
		}
		break;
	}
	//��ʾ��ȫ����
	GetSecurityTypeStr( pFilelistAttribute->fileAttr.fileTypeValid&FILE_TYPE_MASK,
		pFilelistAttribute->fileAttr.securityType,
		strSecurityType);
	lstctrl.SetItemText(nIndex2,7,strSecurityType);
}


void CSerialDlg::ReadRecordFileItems(unsigned char ReadMode)
{
	if (m_pSelectedFileAttr == NULL)
	{
		AfxMessageBox("��ѡ��һ���ļ�");
		return;
	}
	if ((m_pSelectedFileAttr->fileAttr.fileTypeValid & FILE_TYPE_MASK) != FILE_TYPE_RECORD)
	{
		AfxMessageBox("��ǰ�ļ��Ǽ�¼�ļ�");
		return;
	}

	int dataLen = (unsigned char)m_pSelectedFileAttr->fileAttr.uFileAttr.recEF.recordLen;
	WORD retLen;
	BYTE buff[1024*8] = {0};
	int Ret = rfReadRecFile(0, ReadMode, 0, dataLen, &retLen, buff);
	switch(Ret)
	{
	case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_FAILED:
		m_statusbar = "����¼�ļ���ûѡ�ļ�";
		break;
	case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_NEED_NEV_VERIFY:
		m_statusbar = "����¼�ļ���Ȩ��ΪNEV�����ʱ��ܾ�";
		break;
	case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_TIMEOUT:
		m_statusbar = "����¼�ļ���ʱ";
		break;
	case IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_SUCCESS:
		{
			m_statusbar = "����¼�ļ��ɹ�";
			CString tempbuf;
			tempbuf.Format("%s\r\n", buff);
			m_recFileContent += tempbuf;
		}
		break;
	default:
		m_statusbar = "����¼�ļ�ʧ��";
		break;
	}

	UpdateData(FALSE);
}


void CSerialDlg::OnCbnSetfocusCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CSerialDlg::OnBnClickedButton1()//������Ŀ¼ 
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
		m_statusbar = "������Ŀ¼�ɹ�";
		break;
	default:
		m_statusbar = "������Ŀ¼ʧ��";
		break;
	}

	m_bRetToMf = TRUE;
	UpdateData(FALSE);
}


void CSerialDlg::ReadLargeFile(void)		//�����ļ�
{
	PC_CMD_DATA pcCmdData;
	initCmdData(pcCmdData,PC_CMD_TYPE_READ_LARGE_FILE);
	pcCmdData.uParam.READ_LARGE_FILE_PARAM.fileAttrR.fileTypeValid = FILE_TYPE_TRANS;
	pcCmdData.uParam.READ_LARGE_FILE_PARAM.fileAttrR.uFileAttr.transBinEF.realFileLen = m_pSelectedFileAttr->fileAttr.uFileAttr.transBinEF.realFileLen;
	//	pcCmdData.uParam.READ_LARGE_FILE_PARAM.uReadPram.transBinFile.dataLen = dataLen;
	send_cmd(&pcCmdData,sizeof(pcCmdData));
}


bool CSerialDlg::enum_reg_key_value(HKEY hKey, LPCTSTR lpSubKey)	// ��ע����Ѷ˿ں�
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
	RegQueryInfoKey(hOpenKey, //�Ѵ򿪵ļ��ľ��
		achClass, //��������
		&cchClassNameLen, //�������Ƶĳ���
		NULL, //����
		&cSubKeysNum, // �����Ӽ�����Ŀ
		&cbMaxSubKeyLen, // ��������Ӽ�����
		&cMaxClassLen, // ��������೤��
		&cbValuesNum, // ����ֵ����Ŀ
		&cbMaxValueNameLen, // �������ֵ��,���Ƶĳ���
		&cbMaxValueLen, // �������ֵ�ĳ���
		&cbSecurityDescriptor, //���ذ�ȫ����
		&ftLastWriteTime); // ���ؼ����д���ʱ��
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


void CSerialDlg::OnBnClickedButton16()		//��\�رշ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PC_CMD_DATA pcCmdData;
	initCmdData(pcCmdData,PC_CMD_TYPE_MONITOR_SENSOR_EN);
	pcCmdData.uParam.MONITOR_SENSOREN_PARAM.FileId = 0x00;
	pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorId = 0x01;
	if (m_pSelectedTag == NULL)
	{
		AfxMessageBox("δѡ�б�ǩ");
		return;
	}

	if (m_pSelectedTag->tagSpeakerState)	//��״̬����ť��ʾ���رա�
	{
		pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorState = 0x00;
	}else{	//�ر�״̬����ť��ʾ���򿪡�
		pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorState = 0x01;
	}
	send_cmd(&pcCmdData,sizeof(pcCmdData));
}


void CSerialDlg::OnBnClickedRegisterTag()		//AESע���ǩ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PC_CMD_DATA pcCmdData;
	initCmdData(pcCmdData,PC_CMD_TYPE_REGISTER_TAG);
	UpdateData(TRUE);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.TID,m_TID,8);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.u.AES_KEY.MIK,m_MIK,32);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.u.AES_KEY.MEK,m_MEK,32);
	pcCmdData.uParam.REGISTER_TAG_PARAM.keyType = 0;//0ΪAES��1ΪECC
	send_cmd(&pcCmdData,sizeof(pcCmdData));
}


BOOL CSerialDlg::OnDisplay(unsigned int id, NMHDR* pNMHDR, LRESULT* pResult)	// Ϊ��������ť������ͣ����ʾ��Ϣ
{
	TOOLTIPTEXT *pTTT=(TOOLTIPTEXT *)pNMHDR;
	UINT nID=pNMHDR->idFrom;//��ȡ��������ťID
	UINT nIndex=m_Toolbar.CommandToIndex(nID);//���ݰ�ťID��ȡ����
	switch(nIndex)
	{
	case 0:
		m_toolStr.Format("��ʼ����ǩ");break;
	case 1:
		m_toolStr.Format("�Ķ�������");break;
	}
	pTTT->lpszText=m_toolStr.GetBuffer(m_toolStr.GetLength());//���ð�ť��ʾ��Ϣ
	pTTT->hinst=AfxGetResourceHandle();
	return TRUE;
}


void CSerialDlg::popRegisterTag(void)			//����ע���ǩ����
{
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
	
	nRes = RegisterTagDlg.DoModal();			//�����Ի���
	if (IDCANCEL == nRes)						//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��
		return;
}


void CSerialDlg::OnBnClickedBtnGettaginfo()		//��ȡ��ǩ��Ϣ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CSerialDlg::OnBnClickedBtnCleanalltag()	//������б�ǩ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}

	int Ret = rfCleanAllTag ();
	switch(Ret)
	{
	case IF_TYPE_CLR_ALL_TAG | CLR_ALL_REGISTER_TAG_STATE_SUCCESS:
		m_statusbar = "������б�ǩ��Ϣ�ɹ�";
		break;
	case IF_TYPE_CLR_ALL_TAG | CLR_ALL_REGISTER_TAG_STATE_FAILED:
		m_statusbar = "������б�ǩ��Ϣʧ��";
		break;
	default:
		m_statusbar = "������б�ǩ��Ϣunknown error";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedBtnWakeuponetag()	//���ѵ�����ǩ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
	CPwdRFDlg pwdRFDlg;							//����Ի�����CPwdRFDlg��ʵ��
	nRes = pwdRFDlg.DoModal();					//�����Ի���
	if (IDCANCEL == nRes)	return;				//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��
	UpdateData(TRUE);
	m_statusbar = "���ѵ�����ǩ��ʼ......";

	//"�׵�ַд�ļ�"��������
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
	//"ƫ����д�ļ�"��������
	GetDlgItem(IDC_BUTTON14)->EnableWindow(TRUE);
	ClearTagList();
	ClearFileAttrList();
	m_curNumOfTag = 0;
	m_listCtrlFile.DeleteAllItems();
	m_listCtrl.DeleteAllItems();		//����б������е�item
	m_pSelectedTag = NULL;
	UpdateData(FALSE);

	TAG_INFO tagInfoArray;
	BYTE TID[8];
	memcpy(TID, m_writeFile, 8);

	int Ret = rfWakeupOneTag(TID, pwdRFDlg.m_pwd, &tagInfoArray);
	switch(Ret)
	{
	case IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_NOT_FOUND:
		m_statusbar = "���ѵ�����ǩδ����";
		break;
	case IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT:
		m_statusbar = "���ѵ�����ǩ��ʱ";
		break;
	case IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_SUCCESS:
		{
			GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);//�б����

			int nIndex=0;

			CString strNumber,tagStateStr,tidStr,uiiStr;
			BYTE tid[9];
			BYTE uii[32];
			tid[8] = '\0';
			PTAG_ATTR pTagAttr;

			//��������ʾ���б���
			nIndex = m_listCtrl.GetItemCount();
			strNumber.Format("%d", nIndex);//ȡ�����б�ǩ����

			m_listCtrl.InsertItem(nIndex, strNumber);//�����µ�һ��,׼����ʾ

			memcpy(tid, tagInfoArray.TID, TID_LEN);//ȡ���ظ�֡�е�TID,����ʽ����ʾ
			tidStr.Format("%c%c%c%c%c  [ %c%c%c ]",tid[0],tid[1],tid[2],tid[3],tid[4],tid[5],tid[6],tid[7]);
			m_listCtrl.SetItemText(nIndex, 1, tidStr);

			//��ǩ״̬��ʽ����ʾ
			tagStateStr.Format("%d", tagInfoArray.tagState);
			m_listCtrl.SetItemText(nIndex, 2, tagStateStr);

			//UII
			memcpy(uii, tagInfoArray.UII, UII_LEN);
			uiiStr.Format("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				uii[0],uii[1],uii[2],uii[3],uii[4],uii[5],uii[6],uii[7],uii[8],uii[9],uii[10],
				uii[11],uii[12],uii[13],uii[14],uii[15],uii[16],uii[17],uii[18],uii[19],uii[20],
				uii[21],uii[22],uii[23],uii[24],uii[25],uii[26],uii[27],uii[28],uii[29],uii[30],uii[31]);
			m_listCtrl.SetItemText(nIndex, 3, uiiStr);

			//������Ӧ�ı�ǩ��Ϣ����
			pTagAttr = (PTAG_ATTR)malloc(sizeof(TAG_ATTR));//�����ڴ�
			memset(pTagAttr, 0, sizeof(TAG_ATTR));//�ڴ�����
			pTagAttr->nIndex = nIndex;
			pTagAttr->pNext = NULL;
			memcpy(pTagAttr->TID, tagInfoArray.TID, TID_LEN);
			InsertTagList(pTagAttr);
		}
		m_statusbar = "���ѵ�����ǩ�ɹ�";
		break;
	default:
		m_statusbar = "���ѵ�����ǩʧ��";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::popReaderManager(void)			// �����Ķ�������
{
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
	nRes = ReaderManageDlg.DoModal();			//�����Ի���
	if (IDCANCEL == nRes)						//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��
		return;
}


void CSerialDlg::OnBnClickedBtnFileDelete()		//�ļ�ɾ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	if (m_isSelected == FALSE)	//���ļ�δѡ��ʱ����־λm_isSelectec=0
	{
		MessageBox("����ѡ���ļ�");
		return;
	}
	UpdateData(TRUE);

	if (m_pSelectedFileAttr->fileAttr.fileID == 0xa101 ||
		m_pSelectedFileAttr->fileAttr.fileID == 0xa102 ||
		m_pSelectedFileAttr->fileAttr.fileID == 0xa103 ||
		m_pSelectedFileAttr->fileAttr.fileID == 0x3f00 ||
		m_pSelectedFileAttr->fileAttr.fileID == 0xf000)
	{
		MessageBox("ϵͳ�ļ�����ɾ��");
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
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
				m_statusbar = "�ļ�ɾ���ɹ�";
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
				m_statusbar = "�ļ�ɾ��ʧ��";
				UpdateData(FALSE);
				return;
			}
			DisplayAllFilelistItems(m_listCtrlFile,m_pFileList);
			//m_pSelectedFileAttr = NULL;
			m_statusbar = "�ļ�ɾ���ɹ�";
		}
		break;
	case IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_SYSFILE:
		m_statusbar = "ϵͳ�ļ�����ɾ��";
		break;
	default:
		m_statusbar = "�ļ�ɾ������ʧ��";
		break;
	}

	m_isSelected = FALSE;
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedBtnFilesysFormat()	//�ļ�ϵͳ��ʽ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
		UpdateData(FALSE);
		return;
	}
	else
		m_SwitchSignal = FALSE;

	if (MessageBox("ȷ��Ҫ��ʽ���ļ�ϵͳ��","��ֹ�����",MB_OKCANCEL) == IDCANCEL)
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
		m_statusbar = "�ļ�ϵͳ��ʽ���ɹ�";
		break;
	case IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_TIMEOUT:
		m_statusbar = "�ļ�ϵͳ��ʽ����ʱ";
		break;
	case IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_PWD_ERROR:
		m_statusbar = "�ļ�ϵͳ��ʽ���������";
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnBnClickedBtnKillTag()		//��ǩ���
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}

	INT_PTR nRes;								//���ڱ���DoModal�����ķ���ֵ
	CPwdRFDlg pwdRFDlg;							//����Ի�����CPwdRFDlg��ʵ��
	nRes = pwdRFDlg.DoModal();					//�����Ի���
	if (IDCANCEL == nRes)	return;				//�ж϶Ի����˳��󷵻�ֵ�Ƿ�ΪIDCANCEL,�������return�������������ִ��

	UpdateData(TRUE);
	if(pwdRFDlg.m_pwd == 0)
	{
		AfxMessageBox("������Ϊ0�����ʧ��");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
		m_statusbar = "��ǩ���ɹ�";
		break;
	case IF_TYPE_KILL_TAG | KILL_TAG_FAILED:
		m_statusbar = "��ǩ���ʧ��";
		break;
	case IF_TYPE_KILL_TAG | KILL_TAG_TIMEOUT:
		m_statusbar = "��ǩ��ʱ";
		break;
	case IF_TYPE_KILL_TAG | 3:
		m_statusbar = "��ǩ������Ϊ�㣬�������";
		break;
	default:
		m_statusbar = "��ǩ������δ֪����";
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
		pDlg->m_statusbar = "��ǩ���ɹ�";
		break;
	case IF_TYPE_KILL_TAG | KILL_TAG_FAILED:
		pDlg->m_statusbar = "��ǩ���ʧ��";
		break;
	case IF_TYPE_KILL_TAG | KILL_TAG_TIMEOUT:
		pDlg->m_statusbar = "��ǩ��ʱ";
		break;
	case IF_TYPE_KILL_TAG | 3:
		pDlg->m_statusbar = "��ǩ������Ϊ�㣬�������";
		break;
	default:
		pDlg->m_statusbar = "��ǩ������δ֪����";
		break;
	}
	pDlg->SendMessage(MY_MESSAGE, NULL, NULL);
}

void CSerialDlg::OnBnClickedCancel()
{	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		
		errcode.Format("error code:%d\n",GetLastError());  // 3:·������
		
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


// ͬ��������֡����
int CSerialDlg::GetCmdReplyFrameData(void* pData, int bufLen)
{
	return 0;
}
// ͬ�������ݺ���
int CSerialDlg::GetCmdReplyDataSync(void* pData, int bufLen)
{
	return 0;
}


//����ϵͳ����
void CSerialDlg::OnBnClickedBtnUpdatePwd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}

	UpdateData(TRUE);
	WORD PwdIndex = 0;
	char *strPwdIndex = (LPSTR)(LPCTSTR)(m_PwdIndex);
	PwdIndex = strtol(strPwdIndex, NULL, 16); //ʮ������ת������

	if ((PwdIndex > 4) && (PwdIndex < 16128))
	{
		MessageBox("0x3f00���ڵ��ļ��������޸����룬���������Ϸ�");
		return;
	}

	BYTE mode = ((CComboBox*)GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE))->GetCurSel()+1;
	int Ret = rfUpdatePwd(m_adminPwd, PwdIndex, mode, m_newPwd);
	switch(Ret)
	{
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_SUCCESS:
		m_statusbar = "����ϵͳ����ɹ�";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_FAILED:
		m_statusbar = "����ϵͳ����ʧ��";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_TIMEOUT:
		m_statusbar = "����ϵͳ���ʱ";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_ADMIN_PWD_NOT_RIGHT:
		m_statusbar = "����ϵͳ�������Ա���벻ƥ��";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_INVALID_NEW_PWD:
		m_statusbar = "����ϵͳ�����¿���Ϸ�";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_NOT_AUTHORISED:
		m_statusbar = "����ϵͳ����ڰ�ȫ�Ự";
		break;
	case IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_UNKNOWN:
		m_statusbar = "����ϵͳ����δ֪����";
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}


void CSerialDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CSerialDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 if(IDOK != MessageBox("������˳���,������?","!!!!�˳����򾯸�!!!!",MB_OKCANCEL))
			 return FALSE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSerialDlg::OnBnClickedOpensocket()//������ͨ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
			m_statusbar = "������ͨ��ʧ��";

			ClearTagList();
			ClearFileAttrList();
			m_recFileContent.Empty(); //��ն��ļ�control����
			m_writeFile.Empty();
			m_listCtrlFile.DeleteAllItems();
			m_listCtrl.DeleteAllItems();		//����б������е�item

			UpdateData(FALSE);
			return;
		}

		GetDlgItem(IDC_OpenSocket)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);//�̴�
		m_statusbar = "������ͨ�ųɹ�";
		m_OpenCommunication = TRUE;
		m_SwitchSignal = TRUE; //true��ʾ���Դ�����Ϣ�¼���false��ʾ���ܴ���

		ClearTagList();
		ClearFileAttrList();
		m_recFileContent.Empty(); //��ն��ļ�control����
		m_writeFile.Empty();
		m_listCtrlFile.DeleteAllItems();
		m_listCtrl.DeleteAllItems(); //����б������е�item

		UpdateData(FALSE);
	}

	if(strFlag == "USB")
	{
		UpdateData(TRUE);

		if(0 != SearchHidDevice())
		{
			m_OpenCommunication = TRUE;
			m_SwitchSignal = TRUE; //true��ʾ���Դ�����Ϣ�¼���false��ʾ���ܴ���
			m_statusbar = "��USBͨ�ųɹ���";
			GetDlgItem(IDC_OpenSocket)->EnableWindow(FALSE);

			ClearTagList();
			ClearFileAttrList();
			m_recFileContent.Empty(); //��ն��ļ�control����
			m_writeFile.Empty();
			m_listCtrlFile.DeleteAllItems();
			m_listCtrl.DeleteAllItems();		//����б������е�item

			UpdateData(FALSE);
		}
		else
		{
			m_statusbar = "��USBͨ��ʧ�ܣ�";

			ClearTagList();
			ClearFileAttrList();
			m_recFileContent.Empty(); //��ն��ļ�control����
			m_writeFile.Empty();
			m_listCtrlFile.DeleteAllItems();
			m_listCtrl.DeleteAllItems();		//����б������е�item

			UpdateData(FALSE);
		}
	}

// 	HANDLE startThread;
// 	InitializeCriticalSection(&g_cs); //��ʼ��ȫ�ֹؼ���
// 	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BeginMsgEventThread, this, 0, NULL);
// 	CloseHandle(startThread);
}

void CSerialDlg::SocketConnectThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam;

	char *addressIP = (LPSTR)(LPCTSTR)(pdlg->m_IP);//��CString����ת��Ϊchar*����
	int Ret = StartConnect(pdlg->m_IPPort, addressIP);
	if(Ret == 0)
	{
		Sleep(1500);
	}
}

void CSerialDlg::BeginMsgEventThread(LPVOID lparam)  //������Ϣ�����߳�BY PPWU 11-14
{
	MsgEventID.clear(); //��������ڵ�����Ԫ��
	MsgEventID.push_back(-1);  //��ʼ����Ϣ�¼�ID��-1��ʾû���¼�Ҫ����

	while(1)
	{
		if (MsgEventID.empty() != TRUE)
		{
			//�����ٽ���
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
			//�뿪�ٽ���
			LeaveCriticalSection(&g_cs);
		}

		if (MsgEventID.empty() == TRUE)
		{
			Sleep(500);
		}
	}
}

void CSerialDlg::OnBnClickedClosesocket()//�ر�����ͨ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	int nIndex = m_CommuFlag.GetCurSel();
	CString strFlag;
	m_CommuFlag.GetLBText(nIndex, strFlag);

	if(strFlag == "TCP")
	{
		CloseConnect();//�ر��׽���

		GetDlgItem(IDC_OpenSocket)->EnableWindow(TRUE);
		m_OpenCommunication = FALSE;//�ر�ͨ�Ŷ˿ڱ�־bool������false��ʾ�ر�

		m_statusbar = "�ر�ͨ�ųɹ�";
		UpdateData(FALSE);
	}

	if(strFlag == "USB")
	{
		CloseHidDevice();//�ر�ȫ�־��hCom
		GetDlgItem(IDC_OpenSocket)->EnableWindow(TRUE);
		m_OpenCommunication = FALSE;

		m_statusbar = "�ر�ͨ�ųɹ�";
		UpdateData(FALSE);
	}
}


//������Լ�����Ϣ��Ӧ�������������������������ش��ڵ�Updatedata
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CReaderTestDlg Dlg;
	Dlg.DoModal();
}

void CSerialDlg::OnBnClickedCmdtest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CTestSingleCmd Dlg;
	Dlg.DoModal();
}


void CSerialDlg::OnBnClickedBtnSafeSession()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
		m_statusbar = "��ȫ�Ự�����ʧ��";
		UpdateData(FALSE);
		return;
	}

	int Ret = TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_SECURITY_COMMUNICATION, TIME_OUT*2);
	m_SwitchSignal = TRUE;
	if(Ret != 1)
	{
		m_statusbar = "��ȫ�Ựʧ��";
		UpdateData(FALSE);
		return;
	}
	switch(pcCmdReply.uReplyParam.securityCommunicationParam.securityCommunicationParam)
	{
	case SECURITY_COMMUNICATION_SUCCESS:
		m_statusbar = "��ȫ�Ự�ɹ�";
		break;
	case SECURITY_COMMUNICATION_FAILED:
		m_statusbar = "��ȫ�Ựʧ��";
		break;
	case SECURITY_COMMUNICATION_TIMEOUT:
		m_statusbar = "��ȫ�Ự��ʱ";
		break;
	default:
		m_statusbar = "��ȫ�Ựδ֪����";
		break;
	}
	
	UpdateData(FALSE);
}


int CSerialDlg::StrToHex(CString str, char *data) //�ַ���ת����ʮ�������ֽ�
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

void CSerialDlg::OnBnClickedSearchrecord() //������¼
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}
	if(m_pSelectedFileAttr == NULL)
	{
		MessageBox("����ѡ���ļ�");
		return;
	}

	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //���������ָ��

	WORD rRecordNum;
	SEARCH_ELEMENT searchElement;

	char buffer[74];
	int DataLen;
	DataLen = pdlg->StrToHex(pdlg->m_SearchCondition.GetBuffer(), buffer);
	searchElement.logicalOperator = buffer[0]; //ʮ������ת������
	searchElement.fieldOffset = buffer[1];
	searchElement.relationOperator = buffer[2];
	searchElement.dataLen = buffer[3];
	memcpy(searchElement.dataBuf, buffer+4, DataLen-4);


	int Ret = rfSearchRecords(&searchElement, 1, &rRecordNum);
	pdlg->m_SwitchSignal = TRUE;
	switch(Ret)
	{
	case IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_SUCCESS:
		pdlg->m_statusbar.Format("������¼�ɹ�������������¼����%d", rRecordNum);
		break;
	case IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED:
		pdlg->m_statusbar = "������¼ʧ��";
		break;
	case IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_TIMEOUT:
		pdlg->m_statusbar = "������¼��ʱ";
		break;
	default:
		pdlg->m_statusbar = "������¼δ֪����";
		break;
	}
	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
}

void CSerialDlg::OnBnClickedWakeup() //����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_OpenCommunication == FALSE)
	{
		m_statusbar = "���ȴ�ͨ�Ŷ˿�";
		UpdateData(FALSE);
		return;
	}
	UpdateData(TRUE);
	if(m_WakeupNum > 10000)
	{
		AfxMessageBox("���Ѵ�����Ҫ����10000");
		return;
	}
	if(m_SwitchSignal == FALSE)
	{
		m_statusbar = "��һ���������ڽ����У���ȴ�......";
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
// 		m_statusbar = "���ѳɹ�";
// 		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
// 		break;
// 	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_FAILED:
// 		m_statusbar = "����ʧ��";
// 		break;
// 	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_TIMEOUT:
// 		m_statusbar = "���ѳ�ʱ";
// 		break;
// 	default:
// 		m_statusbar = "����δ֪����";
// 		break;
// 	}
// 
// 	UpdateData(FALSE);
}

void CSerialDlg::WakeUpThreadFunc(LPVOID lparam)
{
	CSerialDlg *pdlg = (CSerialDlg *)lparam; //���������ָ��

	int ret = rfWakeUpTag(pdlg->m_WakeupNum);
	pdlg->m_SwitchSignal = TRUE;
	switch(ret)
	{
	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_SUCCESS:
		pdlg->m_statusbar = "���ѳɹ�";
		pdlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		break;
	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_FAILED:
		pdlg->m_statusbar = "����ʧ��";
		break;
	case IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_TIMEOUT:
		pdlg->m_statusbar = "���ѳ�ʱ";
		break;
	default:
		pdlg->m_statusbar = "����δ֪����";
		break;
	}

	pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
}