// ReaderManageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Serial.h"
#include <stdlib.h>
#include <time.h>
#include "ReaderManageDlg.h"
#include "afxdialogex.h"
#include "PcCommand.h"
#include "SerialDlg.h"
#include "makeECDH.h"
#include "makeKey.h"
#include "..\..\API二期\APIDLL\apidll.h"

//全局变量
BYTE newChannel;
BYTE newPower;

// CReaderManageDlg 对话框

IMPLEMENT_DYNAMIC(CReaderManageDlg, CDialogEx)

CReaderManageDlg::CReaderManageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReaderManageDlg::IDD, pParent)
	, m_readInfo(_T(""))
	, m_readerID_w(_T("303030"))
	, m_readerName_w(_T("0102030405060708"))
	, m_pubLenReader(0)
	, m_priLenReader(0)
	, m_filePathName(_T(""))
	, m_TID(_T(""))
{
	memset(m_pubKeyReader,0,PKI_PUBLICKEY_LEN);
	memset(m_priKeyReader,0,PKI_PRIVATEKEY_LEN);
	memset(m_ecdh,0,0x42);
	memset(m_pubKeyTag,0,PKI_PUBLICKEY_LEN);
	m_IP1 = 192;
	m_IP2 = 168;
	m_IP3 = 1;
	m_IP4 = 100;
	m_MaskIP1 = 255;
	m_MaskIP2 = 255;
	m_MaskIP3 = 255;
	m_MaskIP4 = 0;
	//  m_Gateway = _T("");
	m_Port = 7086;
	m_Q = 0;
	m_N = 1;
	m_Gateway1 = 192;
	m_Gateway2 = 168;
	m_Gateway3 = 1;
	m_Gateway4 = 1;
	m_Mac = _T("a1a2a3a4a5a6");
}

CReaderManageDlg::~CReaderManageDlg()
{
}

void CReaderManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INFO, m_readInfo);
	DDX_Text(pDX, IDC_EDIT1, m_readerID_w);
	DDX_Text(pDX, IDC_EDIT13, m_readerName_w);
	DDX_Control(pDX, IDC_COMBO1, m_cmbBaudRate);
	DDX_Text(pDX, IDC_EDIT2, m_IP1);
	DDX_Text(pDX, IDC_EDIT14, m_IP2);
	DDX_Text(pDX, IDC_EDIT15, m_IP3);
	DDX_Text(pDX, IDC_EDIT4, m_IP4);
	DDX_Text(pDX, IDC_EDIT26, m_MaskIP1);
	DDX_Text(pDX, IDC_EDIT7, m_MaskIP2);
	DDX_Text(pDX, IDC_EDIT6, m_MaskIP3);
	DDX_Text(pDX, IDC_EDIT5, m_MaskIP4);
	//  DDX_Text(pDX, IDC_EDIT8, m_Gateway);
	DDX_Text(pDX, IDC_EDIT10, m_Port);
	DDX_Text(pDX, IDC_EDIT9, m_Q);
	DDX_Text(pDX, IDC_EDIT11, m_N);
	DDX_Text(pDX, IDC_EDIT8, m_Gateway1);
	DDX_Text(pDX, IDC_EDIT24, m_Gateway2);
	DDX_Text(pDX, IDC_EDIT3, m_Gateway3);
	DDX_Text(pDX, IDC_EDIT22, m_Gateway4);
	DDX_Text(pDX, IDC_EDIT12, m_Mac);
}


BEGIN_MESSAGE_MAP(CReaderManageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_GETREADERINFO, &CReaderManageDlg::OnBnClickedBtnGetreaderinfo)
	ON_BN_CLICKED(IDOK, &CReaderManageDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_CHANGE_READER_KEY, &CReaderManageDlg::OnBnClickedBtnChangeReaderKey)
	ON_BN_CLICKED(IDC_BTN_IMPOTAG_PUBKEY, &CReaderManageDlg::OnBnClickedBtnImpotagPubkey)
	ON_BN_CLICKED(IDC_BUTTON1, &CReaderManageDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_SET_POWER, &CReaderManageDlg::OnBnClickedBtnSetPower)
	ON_BN_CLICKED(IDC_BUTTON2, &CReaderManageDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CReaderManageDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CReaderManageDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CReaderManageDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CReaderManageDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CReaderManageDlg::OnBnClickedButton7)
	ON_MESSAGE(MYUPDATEFALSE, &CReaderManageDlg::OnMyupdatefalse)
	ON_BN_CLICKED(IDC_SETTCPIP, &CReaderManageDlg::OnBnClickedSettcpip)
	ON_BN_CLICKED(IDC_SETQN, &CReaderManageDlg::OnBnClickedSetqn)
	ON_BN_CLICKED(IDC_SETMAC, &CReaderManageDlg::OnBnClickedSetmac)
END_MESSAGE_MAP()


// CReaderManageDlg 消息处理程序

BOOL CReaderManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("9600");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("115200");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("128000");

	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("0");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("1");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("2");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("3");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("4");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("5");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("6");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("7");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("8");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("9");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("10");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("11");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("12");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("13");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("14");
	((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString("15");

	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("0___-1dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("1___5dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("2___10dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("3___14dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("4___15dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("5___16dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("6___17dbm");

	//默认选择项
	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(1);	//波特率
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(0);	//阅读器信道
	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(0); //阅读器发射功率

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CReaderManageDlg::OnBnClickedBtnGetreaderinfo()	//查询阅读器信息
{
	// TODO: 在此添加控件通知处理程序代码
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("请先打开通信端口");
		return;
	}
	pdlg->m_recFileContent.Empty();
	m_readInfo.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetReaderInfoThread, this, 0, NULL);
	CloseHandle(startThread);
// 	READER_INFO  readerInfo;
// 	int Ret = rfGetReaderInfo(&readerInfo);
// 
// 	switch(Ret)
// 	{
// 	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_TIMEOUT:
// 		AfxMessageBox("查询阅读器基本信息超时");
// 		break;
// 	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_FAILED:
// 		AfxMessageBox("查询阅读器基本信息失败");
// 		break;
// 	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_SUCCESS:
// 		{
// 			AfxMessageBox("查询阅读器基本信息成功");
// 			CString strShow;
// 
// 			strShow.Format("阅读器RID：%x %x %x \r\n", readerInfo.RID[0], readerInfo.RID[1], readerInfo.RID[2]);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			strShow.Format(("阅读器名：%s \r\n"), readerInfo.ReadName);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			strShow.Format("阅读器加密类型：%d\r\n", readerInfo.encryptType);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			strShow.Format("阅读器文件系统类型：%d\r\n", readerInfo.fileSysType);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			strShow.Format("阅读器存储容量：%d字节 \r\n", readerInfo.storeCapacity);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			pdlg->UpdateData(FALSE);
// 		}
// 		break;
// 	default:
// 		break;
// 	}
}

void CReaderManageDlg::GetReaderInfoThread(LPVOID lparam)
{
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd));
	CReaderManageDlg *pReadDlg = (CReaderManageDlg *)lparam;
	READER_INFO  readerInfo;
	memset(readerInfo.ReadName, 0, 32);
	int Ret = rfGetReaderInfo(&readerInfo);
	int aaa = sizeof(PC_CMD_REPLY);

	switch(Ret)
	{
	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_TIMEOUT:
		pReadDlg->m_readInfo = "查询阅读器基本信息超时";
		pReadDlg->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_FAILED:
		pReadDlg->m_readInfo = "查询阅读器基本信息失败";
		pReadDlg->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_SUCCESS:
		{
			pReadDlg->m_readInfo = "查询阅读器基本信息成功";
			pReadDlg->SendMessage(MYUPDATEFALSE, NULL, NULL);
			
			CString strShow;
			strShow.Format("阅读器RID：%02x%02x%02x \r\n", readerInfo.RID[0], readerInfo.RID[1], readerInfo.RID[2]);
			pdlg->m_recFileContent += strShow;

			pdlg->m_recFileContent += "阅读器名：";
			for(int i = 0; i < strlen((char*)readerInfo.ReadName); i++)
			{
				CString strName;
				strName.Format("%02x", readerInfo.ReadName[i]);
 				pdlg->m_recFileContent += strName;
 			}

			strShow.Format("\r\n阅读器加密类型：%02x\r\n", readerInfo.encryptType);
			pdlg->m_recFileContent += strShow;

			strShow.Format("阅读器文件系统类型：%02x\r\n", readerInfo.fileSysType);
			pdlg->m_recFileContent += strShow;

			strShow.Format("阅读器存储容量：%02x字节 \r\n", readerInfo.storeCapacity);
			pdlg->m_recFileContent += strShow;

			pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
		}
		break;
	default:
		pReadDlg->m_readInfo = "查询阅读器基本信息超时";
		pReadDlg->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	}
}


void CReaderManageDlg::OnBnClickedOk()			//设置阅读器信息
{
	// TODO: 在此添加控件通知处理程序代码
// 	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd));
// 	if (pdlg->m_OpenCommunication == FALSE)
// 	{
// 		AfxMessageBox("请先打开通信端口");
// 		return;
// 	}

	UpdateData(TRUE);
	CSerialDlg newDlg; //初始化一个新的主窗口类，调用其十六进制转换成员函数
	char tmpBuffer1[100];
	char tmpBuffer2[50];
	int DataLen1 = newDlg.StrToHex(m_readerName_w, tmpBuffer1);
	int DataLen2 = newDlg.StrToHex(m_readerID_w, tmpBuffer2);

	if(DataLen2 > 3 || DataLen1 > 32)
	{
		MessageBox("请输入正确的值！阅读器ID小于3字节，阅读器名称小于32字节");
		return;
	}

	READER_INFO_W  *readerInfo_w = new READER_INFO_W; //动态分配堆上的内存空间
	memset(readerInfo_w->ReadName, 0, 32);
	memcpy(readerInfo_w->ReadName, tmpBuffer1, DataLen1);
	memcpy(readerInfo_w->RID, tmpBuffer2, DataLen2);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetReaderInfoThread, readerInfo_w, 0, NULL);
	CloseHandle(startThread);

// 	READER_INFO_W  readerInfo_w;
// 	readerInfo_w.baudRate = (BAUD_TYPE)m_cmbBaudRate.GetCurSel();
// 	memcpy(readerInfo_w.ReadName, m_readerName_w, m_readerName_w.GetLength() + 1);//长度加1是留出最后一位给'\0' by ppwu
// 	memcpy(readerInfo_w.RID, m_readerID_w, 3);
// 
// 	int Ret = rfSetReaderInfo(&readerInfo_w);
// 	switch(Ret)
// 	{
// 	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT:
// 		AfxMessageBox("设置阅读器基本信息超时");
// 		break;
// 	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED:
// 		AfxMessageBox("设置阅读器基本信息失败");
// 		break;
// 	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_SUCCESS:
// 		{
// 			AfxMessageBox("设置阅读器基本信息成功");
// 			CDialogEx::OnOK();
// 		}
// 		break;
// 	default:
// 		break;
// 	}
}

void CReaderManageDlg::SetReaderInfoThread(LPVOID lparam)
{
	READER_INFO_W readerInfo_w;
	memcpy(&readerInfo_w, lparam, sizeof(READER_INFO_W));

	int Ret = rfSetReaderInfo(&readerInfo_w);
	switch(Ret)
	{
	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT:
		AfxMessageBox("设置阅读器基本信息超时");
		break;
	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED:
		AfxMessageBox("设置阅读器基本信息失败");
		break;
	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_SUCCESS:
		AfxMessageBox("设置阅读器基本信息成功");
		break;
	default:
		break;
	}
	delete lparam;	//动态删除堆上分配的空间
}


void CReaderManageDlg::OnBnClickedBtnChangeReaderKey()		//生成或更改阅读器密钥
{
	// TODO: 在此添加控件通知处理程序代码
	unsigned char tmpRand[8];					//随机数变量
//	unsigned char ecdh[0x50] = {0};				//ECDH变量
	FILE *fp;									//用于保存文件
// 	U8 pubKeyReader[128];						//阅读器公钥变量
// 	U8 priKeyReader[128];						//阅读器私钥变量
// 	U32 pubLenReader,priLenReader;				//阅读器公钥和私钥的长度

	srand((unsigned int)(time(NULL)));			//产生随机数种子（基于时间）
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	//makeECDH(tmpRand,m_ecdh);				//生成ECDH（不要打开，会导致生成后直接退出 系统崩溃）
	
	makeKey ((char*)tmpRand,m_pubKeyReader,&m_pubLenReader,m_priKeyReader,&m_priLenReader);		//生成阅读器密钥

// 	if (m_pubLenReader < 60 || m_priLenReader < 60)
// 	{
// 		MessageBox ("密钥长度过短，重新生成.");
// 		return;
// 	}

	fp = fopen ("ReaderPubKey","wb");
	fwrite (m_pubKeyReader,sizeof(U8),m_pubLenReader + sizeof(U32),fp);			//将阅读器公钥写入本地文件
	fclose (fp);

// 	fp = fopen ("ReaderPriKey","wb");
// 	fwrite (m_priKeyReader,sizeof(U8),m_priLenReader + sizeof(U32),fp);			
// 	fclose (fp);

	MessageBox("生成阅读器密钥成功！\n私钥即将发送至阅读器，公钥已保存至本地文件");
}


void CReaderManageDlg::OnBnClickedBtnImpotagPubkey()		//导入阅读器私钥
{
	// TODO: 在此添加控件通知处理程序代码
	PC_CMD_DATA pcCmdData;
	CSerialDlg::initCmdData(pcCmdData,PC_CMD_TYPE_SET_READER_INFO);
	memcpy(pcCmdData.uParam.SET_READER_INFO_PARAM.u.priKeyReader,m_priKeyReader,PKI_PRIVATEKEY_LEN);
	pcCmdData.uParam.SET_READER_INFO_PARAM.paraType = 1;//0：基本参数 1：ECC_key参数
	((CSerialDlg *)(theApp.m_pMainWnd))->send_cmd(&pcCmdData,sizeof(pcCmdData));
   }


void CReaderManageDlg::OnBnClickedButton1()				//更改阅读器信道
{
	// TODO: 在此添加控件通知处理程序代码
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("请先打开通信端口");
		return;
	}
	m_readInfo.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	newChannel = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCurSel();
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetChannelThread, this, 0, NULL);
	CloseHandle(startThread);

// 	BYTE newChannel = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCurSel();
// 	int Ret = rfSetChannel(newChannel, 0);
// 	switch(Ret)
// 	{
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_OVERRANGE:
// 		AfxMessageBox("注意：设置阅读器信道超出0-15范围");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
// 		AfxMessageBox("设置阅读器信道失败&发送设置阅读器信道命令失败");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT:
// 		AfxMessageBox("设置阅读器信道未收到回复，超时");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS:
// 		AfxMessageBox("设置阅读器信道成功");
// 		break;
// 	default:
// 		break;
// 	}
}

void CReaderManageDlg::SetChannelThread(LPVOID lparam)
{

	CReaderManageDlg *pReader = (CReaderManageDlg *)lparam;

	int Ret = rfSetChannel(newChannel, 0);
	switch(Ret)
	{
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_OVERRANGE:
		pReader->m_readInfo = "注意：设置阅读器信道超出0-15范围";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
		pReader->m_readInfo = "设置阅读器信道失败";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT:
		pReader->m_readInfo = "设置阅读器信道未收到回复，超时";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS:
		pReader->m_readInfo = "设置阅读器信道成功";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	default:
		pReader->m_readInfo = "设置阅读器信道超时";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	}
}


void CReaderManageDlg::OnBnClickedBtnSetPower()			//设置阅读器发射功率
{
	// TODO: 在此添加控件通知处理程序代码
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("请先打开通信端口");
		return;
	}
	m_readInfo.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	newPower = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetPowerThread, this, 0, NULL);
	CloseHandle(startThread);

// 	BYTE newPower = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();
// 	int Ret = rfSetPower(newPower, 0);
// 
// 	switch(Ret)
// 	{
// 	case IF_TYPE_SET_POWER | SET_POWER_STATE_OVERRANGE:
// 		AfxMessageBox("注意：设置阅读器功率超出0-6范围");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
// 		AfxMessageBox("设置阅读器功率失败&发送设置阅读器功率命令失败");
// 		break;
// 	case IF_TYPE_SET_POWER | SET_POWER_STATE_TIMEOUT:
// 		AfxMessageBox("设置阅读器功率未收到回复，超时");
// 		break;
// 	case IF_TYPE_SET_POWER | SET_POWER_STATE_SUCCESS:
// 		AfxMessageBox("设置阅读器功率成功");
// 		break;
// 	default:
// 		break;
// 	}
}

void CReaderManageDlg::SetPowerThread(LPVOID lparam)
{
	CReaderManageDlg *pReader = (CReaderManageDlg *)lparam;

	int Ret = rfSetPower(newPower, 0);
	switch(Ret)
	{
	case IF_TYPE_SET_POWER | SET_POWER_STATE_OVERRANGE:
		pReader->m_readInfo = "注意：设置阅读器功率超出0-6范围";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
		pReader->m_readInfo = "设置阅读器功率失败&发送设置阅读器功率命令失败";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SET_POWER | SET_POWER_STATE_TIMEOUT:
		pReader->m_readInfo = "设置阅读器功率未收到回复，超时";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SET_POWER | SET_POWER_STATE_SUCCESS:
		pReader->m_readInfo = "设置阅读器功率成功";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	default:
		pReader->m_readInfo = "设置阅读器功率超时";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	}
}


void CReaderManageDlg::OnBnClickedButton2()				//发射空载波（阅读器）
{
	// TODO: 在此添加控件通知处理程序代码
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfSendPurewave();
	switch(Ret)
	{
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_SUCCESS:
		m_readInfo = "发射纯载波成功";
		break;
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_FAILED:
		m_readInfo = "发射纯载波失败";
		break;
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREVAVE_STATE_TIMEOUT:
		m_readInfo = "发射纯载波超时";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton3()				// 停止空载波（阅读器）
{
	// TODO: 在此添加控件通知处理程序代码
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfStopPurewave();
	switch(Ret)
	{
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_SUCCESS:
		m_readInfo = "停止纯载波成功";
		break;
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_FAILED:
		m_readInfo = "停止纯载波失败";
		break;
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_TIMEOUT:
		m_readInfo = "停止纯载波超时";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton4()				// 发射调制信息（阅读器）
{
	// TODO: 在此添加控件通知处理程序代码
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfSendModulInfo();
	switch(Ret)
	{
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_SUCCESS:
		m_readInfo = "发射调制信息成功";
		break;
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_FAILED:
		m_readInfo = "发射调制信息失败";
		break;
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_TIMEOUT:
		m_readInfo = "发射调制信息超时";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton5()				// 关闭调制信息（阅读器）
{
	// TODO: 在此添加控件通知处理程序代码
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfStopModulInfo();
	switch(Ret)
	{
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_SUCCESS:
		m_readInfo = "发射调制信息成功";
		break;
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_FAILED:
		m_readInfo = "发射调制信息失败";
		break;
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_TIMEOUT:
		m_readInfo = "发射调制信息超时";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton6()				// 设置接收模式（阅读器）
{
	// TODO: 在此添加控件通知处理程序代码
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfSetRecvModel();
	switch(Ret)
	{
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS:
		m_readInfo = "设置接收模式成功";
		break;
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED:
		m_readInfo = "设置接收模式失败";
		break;
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_TIMEOUT:
		m_readInfo = "设置接收模式超时";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton7()				// 停止接收模式（阅读器）
{
	// TODO: 在此添加控件通知处理程序代码
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfStopRecvModel();
	switch(Ret)
	{
	case IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS:
		m_readInfo = "停止接收模式成功";
		break;
	case IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED:
		m_readInfo = "停止接收模式失败";
		break;
	case IF_TYPE_STOP_RECV_MODEL | STOP_RECV_MODEL_STATE_TIMEOUT:
		m_readInfo = "停止接收模式超时";
		break;
	}
	UpdateData(FALSE);
}


afx_msg LRESULT CReaderManageDlg::OnMyupdatefalse(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);

	return 0;
}


void CReaderManageDlg::OnBnClickedSettcpip() //设置TCP/IP
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	memset(&pcCmdData, 0, sizeof(PC_CMD_DATA));
	pcCmdData.cmdType = PC_CMD_TYPE_SET_TCPIP;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
	pcCmdData.uParam.SET_TCPIP_PARAM.ip[0] = m_IP1;
	pcCmdData.uParam.SET_TCPIP_PARAM.ip[1] = m_IP2;
	pcCmdData.uParam.SET_TCPIP_PARAM.ip[2] = m_IP3;
	pcCmdData.uParam.SET_TCPIP_PARAM.ip[3] = m_IP4;
	pcCmdData.uParam.SET_TCPIP_PARAM.maskip[0] = m_MaskIP1;
	pcCmdData.uParam.SET_TCPIP_PARAM.maskip[1] = m_MaskIP2;
	pcCmdData.uParam.SET_TCPIP_PARAM.maskip[2] = m_MaskIP3;
	pcCmdData.uParam.SET_TCPIP_PARAM.maskip[3] = m_MaskIP4;
	pcCmdData.uParam.SET_TCPIP_PARAM.gateway[0] = m_Gateway1;
	pcCmdData.uParam.SET_TCPIP_PARAM.gateway[1] = m_Gateway2;
	pcCmdData.uParam.SET_TCPIP_PARAM.gateway[2] = m_Gateway3;
	pcCmdData.uParam.SET_TCPIP_PARAM.gateway[3] = m_Gateway4;
	pcCmdData.uParam.SET_TCPIP_PARAM.port = m_Port;

	if(TestSendCmd(&pcCmdData) == -1)
	{
		m_readInfo = "设置TCP/IP发送失败";
		UpdateData(FALSE);
		return;
	}

	int Ret = TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_SET_TCPIP, TIME_OUT*2);
	if(Ret != 1)
	{
		m_readInfo = "设置TCP/IP失败";
		UpdateData(FALSE);
		return;
	}
	switch(pcCmdReply.uReplyParam.settcpipqnmacParam.SetRetState)
	{
	case SET_TCPIP_QN_MAC_SUCCESS:
		m_readInfo = "设置TCP/IP成功";
		break;
	case SET_TCPIP_QN_MAC_FAILED:
		m_readInfo = "设置TCP/IP失败";
		break;
	case SET_TCPIP_QN_MAC_TIMEOUT:
		m_readInfo = "设置TCP/IP超时";
		break;
	default:
		m_readInfo = "设置TCP/IP未知错误";
		break;
	}

	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedSetqn() //设置QN值
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	memset(&pcCmdData, 0, sizeof(PC_CMD_DATA));
	pcCmdData.cmdType = PC_CMD_TYPE_SET_QN;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
	pcCmdData.uParam.SET_QN_PARAM.Q = m_Q;
	pcCmdData.uParam.SET_QN_PARAM.N = m_N;

	if(TestSendCmd(&pcCmdData) == -1)
	{
		m_readInfo = "设置Q/N值发送失败";
		UpdateData(FALSE);
		return;
	}

	int Ret = TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_SET_QN, TIME_OUT*2);
	if(Ret != 1)
	{
		m_readInfo = "设置Q/N值失败";
		UpdateData(FALSE);
		return;
	}
	switch(pcCmdReply.uReplyParam.settcpipqnmacParam.SetRetState)
	{
	case SET_TCPIP_QN_MAC_SUCCESS:
		m_readInfo = "设置Q/N值成功";
		break;
	case SET_TCPIP_QN_MAC_FAILED:
		m_readInfo = "设置Q/N值失败";
		break;
	case SET_TCPIP_QN_MAC_TIMEOUT:
		m_readInfo = "设置Q/N值超时";
		break;
	default:
		m_readInfo = "设置Q/N值未知错误";
		break;
	}

	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedSetmac() //设置MAC值
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CSerialDlg newDlg;
	char tmpBuff[6];
	int tmpLen = newDlg.StrToHex(m_Mac, tmpBuff);

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	memset(&pcCmdData, 0, sizeof(PC_CMD_DATA));
	pcCmdData.cmdType = PC_CMD_TYPE_SET_MAC;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
	memcpy(pcCmdData.uParam.SET_MAC_PARAM.mac, tmpBuff, tmpLen);


	if(TestSendCmd(&pcCmdData) == -1)
	{
		m_readInfo = "设置MAC值发送失败";
		UpdateData(FALSE);
		return;
	}

	int Ret = TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_SET_MAC, TIME_OUT*2);
	if(Ret != 1)
	{
		m_readInfo = "设置MAC值失败";
		UpdateData(FALSE);
		return;
	}
	switch(pcCmdReply.uReplyParam.settcpipqnmacParam.SetRetState)
	{
	case SET_TCPIP_QN_MAC_SUCCESS:
		m_readInfo = "设置MAC值成功";
		break;
	case SET_TCPIP_QN_MAC_FAILED:
		m_readInfo = "设置MAC值失败";
		break;
	case SET_TCPIP_QN_MAC_TIMEOUT:
		m_readInfo = "设置MAC值超时";
		break;
	default:
		m_readInfo = "设置MAC值未知错误";
		break;
	}

	UpdateData(FALSE);
}
