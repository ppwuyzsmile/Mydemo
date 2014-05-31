// ReaderManageDlg.cpp : ʵ���ļ�
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
#include "..\..\API����\APIDLL\apidll.h"

//ȫ�ֱ���
BYTE newChannel;
BYTE newPower;

// CReaderManageDlg �Ի���

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


// CReaderManageDlg ��Ϣ�������

BOOL CReaderManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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

	//Ĭ��ѡ����
	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(1);	//������
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(0);	//�Ķ����ŵ�
	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(0); //�Ķ������书��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CReaderManageDlg::OnBnClickedBtnGetreaderinfo()	//��ѯ�Ķ�����Ϣ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
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
// 		AfxMessageBox("��ѯ�Ķ���������Ϣ��ʱ");
// 		break;
// 	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_FAILED:
// 		AfxMessageBox("��ѯ�Ķ���������Ϣʧ��");
// 		break;
// 	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_SUCCESS:
// 		{
// 			AfxMessageBox("��ѯ�Ķ���������Ϣ�ɹ�");
// 			CString strShow;
// 
// 			strShow.Format("�Ķ���RID��%x %x %x \r\n", readerInfo.RID[0], readerInfo.RID[1], readerInfo.RID[2]);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			strShow.Format(("�Ķ�������%s \r\n"), readerInfo.ReadName);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			strShow.Format("�Ķ����������ͣ�%d\r\n", readerInfo.encryptType);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			strShow.Format("�Ķ����ļ�ϵͳ���ͣ�%d\r\n", readerInfo.fileSysType);
// 			pdlg->m_recFileContent += strShow;
// 			
// 			strShow.Format("�Ķ����洢������%d�ֽ� \r\n", readerInfo.storeCapacity);
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
		pReadDlg->m_readInfo = "��ѯ�Ķ���������Ϣ��ʱ";
		pReadDlg->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_FAILED:
		pReadDlg->m_readInfo = "��ѯ�Ķ���������Ϣʧ��";
		pReadDlg->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_SUCCESS:
		{
			pReadDlg->m_readInfo = "��ѯ�Ķ���������Ϣ�ɹ�";
			pReadDlg->SendMessage(MYUPDATEFALSE, NULL, NULL);
			
			CString strShow;
			strShow.Format("�Ķ���RID��%02x%02x%02x \r\n", readerInfo.RID[0], readerInfo.RID[1], readerInfo.RID[2]);
			pdlg->m_recFileContent += strShow;

			pdlg->m_recFileContent += "�Ķ�������";
			for(int i = 0; i < strlen((char*)readerInfo.ReadName); i++)
			{
				CString strName;
				strName.Format("%02x", readerInfo.ReadName[i]);
 				pdlg->m_recFileContent += strName;
 			}

			strShow.Format("\r\n�Ķ����������ͣ�%02x\r\n", readerInfo.encryptType);
			pdlg->m_recFileContent += strShow;

			strShow.Format("�Ķ����ļ�ϵͳ���ͣ�%02x\r\n", readerInfo.fileSysType);
			pdlg->m_recFileContent += strShow;

			strShow.Format("�Ķ����洢������%02x�ֽ� \r\n", readerInfo.storeCapacity);
			pdlg->m_recFileContent += strShow;

			pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
		}
		break;
	default:
		pReadDlg->m_readInfo = "��ѯ�Ķ���������Ϣ��ʱ";
		pReadDlg->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	}
}


void CReaderManageDlg::OnBnClickedOk()			//�����Ķ�����Ϣ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
// 	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd));
// 	if (pdlg->m_OpenCommunication == FALSE)
// 	{
// 		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
// 		return;
// 	}

	UpdateData(TRUE);
	CSerialDlg newDlg; //��ʼ��һ���µ��������࣬������ʮ������ת����Ա����
	char tmpBuffer1[100];
	char tmpBuffer2[50];
	int DataLen1 = newDlg.StrToHex(m_readerName_w, tmpBuffer1);
	int DataLen2 = newDlg.StrToHex(m_readerID_w, tmpBuffer2);

	if(DataLen2 > 3 || DataLen1 > 32)
	{
		MessageBox("��������ȷ��ֵ���Ķ���IDС��3�ֽڣ��Ķ�������С��32�ֽ�");
		return;
	}

	READER_INFO_W  *readerInfo_w = new READER_INFO_W; //��̬������ϵ��ڴ�ռ�
	memset(readerInfo_w->ReadName, 0, 32);
	memcpy(readerInfo_w->ReadName, tmpBuffer1, DataLen1);
	memcpy(readerInfo_w->RID, tmpBuffer2, DataLen2);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetReaderInfoThread, readerInfo_w, 0, NULL);
	CloseHandle(startThread);

// 	READER_INFO_W  readerInfo_w;
// 	readerInfo_w.baudRate = (BAUD_TYPE)m_cmbBaudRate.GetCurSel();
// 	memcpy(readerInfo_w.ReadName, m_readerName_w, m_readerName_w.GetLength() + 1);//���ȼ�1���������һλ��'\0' by ppwu
// 	memcpy(readerInfo_w.RID, m_readerID_w, 3);
// 
// 	int Ret = rfSetReaderInfo(&readerInfo_w);
// 	switch(Ret)
// 	{
// 	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT:
// 		AfxMessageBox("�����Ķ���������Ϣ��ʱ");
// 		break;
// 	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED:
// 		AfxMessageBox("�����Ķ���������Ϣʧ��");
// 		break;
// 	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_SUCCESS:
// 		{
// 			AfxMessageBox("�����Ķ���������Ϣ�ɹ�");
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
		AfxMessageBox("�����Ķ���������Ϣ��ʱ");
		break;
	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED:
		AfxMessageBox("�����Ķ���������Ϣʧ��");
		break;
	case IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_SUCCESS:
		AfxMessageBox("�����Ķ���������Ϣ�ɹ�");
		break;
	default:
		break;
	}
	delete lparam;	//��̬ɾ�����Ϸ���Ŀռ�
}


void CReaderManageDlg::OnBnClickedBtnChangeReaderKey()		//���ɻ�����Ķ�����Կ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	unsigned char tmpRand[8];					//���������
//	unsigned char ecdh[0x50] = {0};				//ECDH����
	FILE *fp;									//���ڱ����ļ�
// 	U8 pubKeyReader[128];						//�Ķ�����Կ����
// 	U8 priKeyReader[128];						//�Ķ���˽Կ����
// 	U32 pubLenReader,priLenReader;				//�Ķ�����Կ��˽Կ�ĳ���

	srand((unsigned int)(time(NULL)));			//������������ӣ�����ʱ�䣩
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	//makeECDH(tmpRand,m_ecdh);				//����ECDH����Ҫ�򿪣��ᵼ�����ɺ�ֱ���˳� ϵͳ������
	
	makeKey ((char*)tmpRand,m_pubKeyReader,&m_pubLenReader,m_priKeyReader,&m_priLenReader);		//�����Ķ�����Կ

// 	if (m_pubLenReader < 60 || m_priLenReader < 60)
// 	{
// 		MessageBox ("��Կ���ȹ��̣���������.");
// 		return;
// 	}

	fp = fopen ("ReaderPubKey","wb");
	fwrite (m_pubKeyReader,sizeof(U8),m_pubLenReader + sizeof(U32),fp);			//���Ķ�����Կд�뱾���ļ�
	fclose (fp);

// 	fp = fopen ("ReaderPriKey","wb");
// 	fwrite (m_priKeyReader,sizeof(U8),m_priLenReader + sizeof(U32),fp);			
// 	fclose (fp);

	MessageBox("�����Ķ�����Կ�ɹ���\n˽Կ�����������Ķ�������Կ�ѱ����������ļ�");
}


void CReaderManageDlg::OnBnClickedBtnImpotagPubkey()		//�����Ķ���˽Կ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PC_CMD_DATA pcCmdData;
	CSerialDlg::initCmdData(pcCmdData,PC_CMD_TYPE_SET_READER_INFO);
	memcpy(pcCmdData.uParam.SET_READER_INFO_PARAM.u.priKeyReader,m_priKeyReader,PKI_PRIVATEKEY_LEN);
	pcCmdData.uParam.SET_READER_INFO_PARAM.paraType = 1;//0���������� 1��ECC_key����
	((CSerialDlg *)(theApp.m_pMainWnd))->send_cmd(&pcCmdData,sizeof(pcCmdData));
   }


void CReaderManageDlg::OnBnClickedButton1()				//�����Ķ����ŵ�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
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
// 		AfxMessageBox("ע�⣺�����Ķ����ŵ�����0-15��Χ");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
// 		AfxMessageBox("�����Ķ����ŵ�ʧ��&���������Ķ����ŵ�����ʧ��");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT:
// 		AfxMessageBox("�����Ķ����ŵ�δ�յ��ظ�����ʱ");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS:
// 		AfxMessageBox("�����Ķ����ŵ��ɹ�");
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
		pReader->m_readInfo = "ע�⣺�����Ķ����ŵ�����0-15��Χ";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
		pReader->m_readInfo = "�����Ķ����ŵ�ʧ��";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT:
		pReader->m_readInfo = "�����Ķ����ŵ�δ�յ��ظ�����ʱ";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS:
		pReader->m_readInfo = "�����Ķ����ŵ��ɹ�";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	default:
		pReader->m_readInfo = "�����Ķ����ŵ���ʱ";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	}
}


void CReaderManageDlg::OnBnClickedBtnSetPower()			//�����Ķ������书��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
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
// 		AfxMessageBox("ע�⣺�����Ķ������ʳ���0-6��Χ");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
// 		AfxMessageBox("�����Ķ�������ʧ��&���������Ķ�����������ʧ��");
// 		break;
// 	case IF_TYPE_SET_POWER | SET_POWER_STATE_TIMEOUT:
// 		AfxMessageBox("�����Ķ�������δ�յ��ظ�����ʱ");
// 		break;
// 	case IF_TYPE_SET_POWER | SET_POWER_STATE_SUCCESS:
// 		AfxMessageBox("�����Ķ������ʳɹ�");
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
		pReader->m_readInfo = "ע�⣺�����Ķ������ʳ���0-6��Χ";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
		pReader->m_readInfo = "�����Ķ�������ʧ��&���������Ķ�����������ʧ��";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SET_POWER | SET_POWER_STATE_TIMEOUT:
		pReader->m_readInfo = "�����Ķ�������δ�յ��ظ�����ʱ";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	case IF_TYPE_SET_POWER | SET_POWER_STATE_SUCCESS:
		pReader->m_readInfo = "�����Ķ������ʳɹ�";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	default:
		pReader->m_readInfo = "�����Ķ������ʳ�ʱ";
		pReader->SendMessage(MYUPDATEFALSE, NULL, NULL);
		break;
	}
}


void CReaderManageDlg::OnBnClickedButton2()				//������ز����Ķ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfSendPurewave();
	switch(Ret)
	{
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_SUCCESS:
		m_readInfo = "���䴿�ز��ɹ�";
		break;
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_FAILED:
		m_readInfo = "���䴿�ز�ʧ��";
		break;
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREVAVE_STATE_TIMEOUT:
		m_readInfo = "���䴿�ز���ʱ";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton3()				// ֹͣ���ز����Ķ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfStopPurewave();
	switch(Ret)
	{
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_SUCCESS:
		m_readInfo = "ֹͣ���ز��ɹ�";
		break;
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_FAILED:
		m_readInfo = "ֹͣ���ز�ʧ��";
		break;
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_TIMEOUT:
		m_readInfo = "ֹͣ���ز���ʱ";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton4()				// ���������Ϣ���Ķ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfSendModulInfo();
	switch(Ret)
	{
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_SUCCESS:
		m_readInfo = "���������Ϣ�ɹ�";
		break;
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_FAILED:
		m_readInfo = "���������Ϣʧ��";
		break;
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_TIMEOUT:
		m_readInfo = "���������Ϣ��ʱ";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton5()				// �رյ�����Ϣ���Ķ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfStopModulInfo();
	switch(Ret)
	{
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_SUCCESS:
		m_readInfo = "���������Ϣ�ɹ�";
		break;
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_FAILED:
		m_readInfo = "���������Ϣʧ��";
		break;
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_TIMEOUT:
		m_readInfo = "���������Ϣ��ʱ";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton6()				// ���ý���ģʽ���Ķ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfSetRecvModel();
	switch(Ret)
	{
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS:
		m_readInfo = "���ý���ģʽ�ɹ�";
		break;
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED:
		m_readInfo = "���ý���ģʽʧ��";
		break;
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_TIMEOUT:
		m_readInfo = "���ý���ģʽ��ʱ";
		break;
	}
	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedButton7()				// ֹͣ����ģʽ���Ķ�����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_readInfo.Empty();
	UpdateData(FALSE);

	int Ret = rfStopRecvModel();
	switch(Ret)
	{
	case IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS:
		m_readInfo = "ֹͣ����ģʽ�ɹ�";
		break;
	case IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED:
		m_readInfo = "ֹͣ����ģʽʧ��";
		break;
	case IF_TYPE_STOP_RECV_MODEL | STOP_RECV_MODEL_STATE_TIMEOUT:
		m_readInfo = "ֹͣ����ģʽ��ʱ";
		break;
	}
	UpdateData(FALSE);
}


afx_msg LRESULT CReaderManageDlg::OnMyupdatefalse(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);

	return 0;
}


void CReaderManageDlg::OnBnClickedSettcpip() //����TCP/IP
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		m_readInfo = "����TCP/IP����ʧ��";
		UpdateData(FALSE);
		return;
	}

	int Ret = TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_SET_TCPIP, TIME_OUT*2);
	if(Ret != 1)
	{
		m_readInfo = "����TCP/IPʧ��";
		UpdateData(FALSE);
		return;
	}
	switch(pcCmdReply.uReplyParam.settcpipqnmacParam.SetRetState)
	{
	case SET_TCPIP_QN_MAC_SUCCESS:
		m_readInfo = "����TCP/IP�ɹ�";
		break;
	case SET_TCPIP_QN_MAC_FAILED:
		m_readInfo = "����TCP/IPʧ��";
		break;
	case SET_TCPIP_QN_MAC_TIMEOUT:
		m_readInfo = "����TCP/IP��ʱ";
		break;
	default:
		m_readInfo = "����TCP/IPδ֪����";
		break;
	}

	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedSetqn() //����QNֵ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		m_readInfo = "����Q/Nֵ����ʧ��";
		UpdateData(FALSE);
		return;
	}

	int Ret = TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_SET_QN, TIME_OUT*2);
	if(Ret != 1)
	{
		m_readInfo = "����Q/Nֵʧ��";
		UpdateData(FALSE);
		return;
	}
	switch(pcCmdReply.uReplyParam.settcpipqnmacParam.SetRetState)
	{
	case SET_TCPIP_QN_MAC_SUCCESS:
		m_readInfo = "����Q/Nֵ�ɹ�";
		break;
	case SET_TCPIP_QN_MAC_FAILED:
		m_readInfo = "����Q/Nֵʧ��";
		break;
	case SET_TCPIP_QN_MAC_TIMEOUT:
		m_readInfo = "����Q/Nֵ��ʱ";
		break;
	default:
		m_readInfo = "����Q/Nֵδ֪����";
		break;
	}

	UpdateData(FALSE);
}


void CReaderManageDlg::OnBnClickedSetmac() //����MACֵ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		m_readInfo = "����MACֵ����ʧ��";
		UpdateData(FALSE);
		return;
	}

	int Ret = TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_SET_MAC, TIME_OUT*2);
	if(Ret != 1)
	{
		m_readInfo = "����MACֵʧ��";
		UpdateData(FALSE);
		return;
	}
	switch(pcCmdReply.uReplyParam.settcpipqnmacParam.SetRetState)
	{
	case SET_TCPIP_QN_MAC_SUCCESS:
		m_readInfo = "����MACֵ�ɹ�";
		break;
	case SET_TCPIP_QN_MAC_FAILED:
		m_readInfo = "����MACֵʧ��";
		break;
	case SET_TCPIP_QN_MAC_TIMEOUT:
		m_readInfo = "����MACֵ��ʱ";
		break;
	default:
		m_readInfo = "����MACֵδ֪����";
		break;
	}

	UpdateData(FALSE);
}
