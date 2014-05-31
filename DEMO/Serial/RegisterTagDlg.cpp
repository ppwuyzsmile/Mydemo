// RegisterTagDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Serial.h"
#include "RegisterTagDlg.h"
#include "afxdialogex.h"
#include "PcCommand.h"
#include "SerialDlg.h"
#include "makeKey.h"
#include "..\..\API����\APIDLL\apidll.h"

BOOL  isConnected = FALSE;
WORD  CycleNum = 0;
WORD  UpcycleNum = 0;

// CRegisterTagDlg �Ի���

IMPLEMENT_DYNAMIC(CRegisterTagDlg, CDialogEx)

CRegisterTagDlg::CRegisterTagDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRegisterTagDlg::IDD, pParent)
	, m_TID(_T("a1a2a3a4a5a6a7a8"))
	, m_UII(_T(""))
	, m_memoryVolume(0)
	, m_inqMaxNum(0)
	, m_inqMaxMatchNum(0)
	, m_MIK(_T(""))
	, m_MEK(_T(""))
	, m_pubLenTag(0)
	, m_priLenTag(0)
	, m_filePathName(_T(""))
	, m_status(_T(""))
{
	memset(m_pubKeyTag,0,PKI_PUBLICKEY_LEN);
	memset(m_priKeyTag,0,PKI_PRIVATEKEY_LEN);
	memset(m_pubKeyReader,0,PKI_PUBLICKEY_LEN);
	m_FileID = _T("a101");
	m_Offset = _T("00");
	m_DataLen = _T("08");
	m_SlotNum = _T("0000");
	m_UpCycle = _T("0004");
	m_JianceCycle = _T("0004");
	m_SlotWidth = _T("0004");
	m_SlotSum = _T("0005");
	m_SendRecvIndicate = _T("00");
	m_JianceTid = _T("6a69757a68303050");
}

CRegisterTagDlg::~CRegisterTagDlg()
{
}

void CRegisterTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_UII);
	DDV_MaxChars(pDX, m_UII, 32);
	DDX_Text(pDX, IDC_EDIT13, m_memoryVolume);
	DDX_Text(pDX, IDC_EDIT14, m_inqMaxNum);
	DDX_Text(pDX, IDC_EDIT15, m_inqMaxMatchNum);
	DDX_Text(pDX, IDC_EDIT16, m_MIK);
	DDV_MaxChars(pDX, m_MIK, 32);
	DDX_Text(pDX, IDC_EDIT17, m_MEK);
	DDV_MaxChars(pDX, m_MEK, 32);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_status);
	DDX_Text(pDX, IDC_EDIT1, m_TID);
	DDX_Text(pDX, IDC_EDIT3, m_FileID);
	DDX_Text(pDX, IDC_EDIT9, m_Offset);
	DDX_Text(pDX, IDC_EDIT6, m_DataLen);
	DDX_Text(pDX, IDC_EDIT10, m_SlotNum);
	DDX_Text(pDX, IDC_EDIT7, m_UpCycle);
	DDX_Text(pDX, IDC_EDIT11, m_JianceCycle);
	DDX_Text(pDX, IDC_EDIT8, m_SlotWidth);
	DDX_Text(pDX, IDC_EDIT12, m_SlotSum);
	DDX_Text(pDX, IDC_EDIT18, m_SendRecvIndicate);
	DDX_Text(pDX, IDC_EDIT19, m_JianceTid);
}


BEGIN_MESSAGE_MAP(CRegisterTagDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRegisterTagDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_CHANGE_TAG_KEY, &CRegisterTagDlg::OnBnClickedBtnChangeTagKey)
	ON_BN_CLICKED(IDC_BTN_IMPOREAD_PUBKEY, &CRegisterTagDlg::OnBnClickedBtnImporeadPubkey)
	ON_BN_CLICKED(IDC_BUTTON1, &CRegisterTagDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRegisterTagDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CRegisterTagDlg::OnBnClickedButton3)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON4, &CRegisterTagDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CRegisterTagDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CRegisterTagDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON9, &CRegisterTagDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON17, &CRegisterTagDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON7, &CRegisterTagDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CRegisterTagDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BTN_GET_TAG_INFO, &CRegisterTagDlg::OnBnClickedBtnGetTagInfo)
	ON_BN_CLICKED(IDC_BTN_GET_CHANNEL, &CRegisterTagDlg::OnBnClickedBtnGetChannel)
	ON_BN_CLICKED(IDC_BTN_GET_POWER, &CRegisterTagDlg::OnBnClickedBtnGetPower)
	ON_BN_CLICKED(IDC_BUTTON10, &CRegisterTagDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON12, &CRegisterTagDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CRegisterTagDlg::OnBnClickedButton13)
END_MESSAGE_MAP()


// CRegisterTagDlg ��Ϣ�������


BOOL CRegisterTagDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	isConnected = FALSE;

	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("0");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("1");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("2");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("3");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("4");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("5");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("6");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("7");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("8");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("9");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("10");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("11");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("12");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("13");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("14");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("15");


	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("0___-18dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("1___-7dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("2___-4dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("3___-2dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("4___0dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("5___1dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("6___2dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("7___3dbm");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString("8___5dbm");

	//Ĭ��ѡ����
	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(0);	//��ǩ�ŵ�
	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(0); //�Ķ������书��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CRegisterTagDlg::OnBnClickedOk()			//���ó�ʼ����ǩ��Ϣ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

 	UpdateData(TRUE);

// 	HANDLE startThread;
// 	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetTagInfoThread, NULL, 0, NULL);
// 	CloseHandle(startThread);

	CSerialDlg newDlg;
	char tmpBuffer[32];
	int DataLen = newDlg.StrToHex(m_TID, tmpBuffer);
	if(DataLen != 8)
	{
		AfxMessageBox("����TID�ĳ���Ӧ����8�ֽ�");
		return;
	}

	TAG_ATTRIBUTE  tagAttr;
	memset(tagAttr.TID, 0, 8);
	memcpy(tagAttr.TID, tmpBuffer, DataLen);
	memcpy(tagAttr.UII, m_UII, 32);
	tagAttr.memoryVolume = m_memoryVolume;
	tagAttr.inqMaxNum = m_inqMaxNum;
	tagAttr.inqMaxMatchNum = (unsigned short)m_inqMaxMatchNum;
	memcpy(tagAttr.MIK, m_MIK, 32);
	memcpy(tagAttr.MEK, m_MEK, 32);

	int Ret = rfInitTag(&tagAttr);
	switch(Ret)
	{
	case IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT:
		AfxMessageBox("���ñ�ǩ������Ϣ��ʱ");
		break;
	case IF_TYPE_INIT_TAG | INIT_TAG_STATE_FAILED:
		AfxMessageBox("���ñ�ǩ������Ϣʧ��");
		break;
	case IF_TYPE_INIT_TAG | INIT_TAG_STATE_SUCCESS:
		AfxMessageBox("���ñ�ǩ������Ϣ�ɹ�");
		break;
	default:
		break;
	}
}


void CRegisterTagDlg::SetTagInfoThread(LPVOID lparam)
{
	CRegisterTagDlg *pdlg = new CRegisterTagDlg();

	TAG_ATTRIBUTE  tagAttr;
	memcpy(tagAttr.TID, pdlg->m_TID, 8);
	memcpy(tagAttr.UII, pdlg->m_UII, 32);
	tagAttr.memoryVolume = pdlg->m_memoryVolume;
	tagAttr.inqMaxNum = pdlg->m_inqMaxNum;
	tagAttr.inqMaxMatchNum = (unsigned short)pdlg->m_inqMaxMatchNum;
	memcpy(tagAttr.MIK, pdlg->m_MIK, 32);
	memcpy(tagAttr.MEK, pdlg->m_MEK, 32);

	int Ret = rfInitTag(&tagAttr);
	switch(Ret)
	{
	case IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT:
		AfxMessageBox("���ñ�ǩ������Ϣ��ʱ");
		break;
	case IF_TYPE_INIT_TAG | INIT_TAG_STATE_FAILED:
		AfxMessageBox("���ñ�ǩ������Ϣʧ��");
		break;
	case IF_TYPE_INIT_TAG | INIT_TAG_STATE_SUCCESS:
		AfxMessageBox("���ñ�ǩ������Ϣ�ɹ�");
		break;
	default:
		break;
	}
}


void CRegisterTagDlg::OnBnClickedBtnChangeTagKey()		//���ɻ���ı�ǩ��Կ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	if (m_TID == "")
	{
		MessageBox("������TID!");
		return;
	}

	unsigned char tmpRand[8];					//���������
	FILE *fp;									//���ڱ����ļ�
	int check = 60;
	int len;

	srand((unsigned int)(time(NULL)));			//������������ӣ�����ʱ�䣩
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	len = 0;
	
	makeKey ((char*)tmpRand,m_pubKeyTag,&m_pubLenTag,m_priKeyTag,&m_priLenTag);		//���ɱ�ǩ��Կ

// 	if (m_pubLenTag < 60 || m_priLenTag < 60)
// 	{
// 		MessageBox ("��Կ���ȹ��̣���������.");
// 		return;
// 	}
	
	fp = fopen ((LPCTSTR)m_TID,"wb");
	fwrite (m_pubKeyTag,sizeof(U8),m_pubLenTag + sizeof(U32),fp);			//��ǩ����Կд�뱾���ļ�
	fclose (fp);



 	//����
 	fp = fopen ("TagPriKey","wb");
 	fwrite (m_priKeyTag,1,m_priLenTag + sizeof(U32),fp);
 	fclose (fp);

	MessageBox("���ɱ�ǩ��Կ�ɹ���\n˽Կ������������ǩ����Կ�ѱ����������ļ�");
}


void CRegisterTagDlg::OnBnClickedBtnImporeadPubkey()		//�����Ķ�����Կ�ͱ�ǩ˽Կ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PC_CMD_DATA pcCmdData;
	CSerialDlg::initCmdData(pcCmdData,PC_CMD_TYPE_INIT_TAG);
	memset(m_pubKeyReader,0,PKI_PUBLICKEY_LEN);
	CFileDialog dlg(TRUE);		//TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
	if (dlg.DoModal() == IDOK)
		m_filePathName = dlg.GetPathName();

	FILE *fp;
	U32 keyLength;
	fp = fopen(m_filePathName,"rb");	//д���ļ���ReaderPubKey����ӦΪm_filePathName
	if (fp == NULL)
	{
		MessageBox("���ļ�ʧ�ܣ�\n�ļ����ܲ�����");
		return;
	}
	fread(&keyLength,sizeof(U32),1,fp);		//�ȶ�����Կ����
	fread(m_pubKeyReader + 4,sizeof(U8),PKI_PUBLICKEY_LEN,fp);
	fclose(fp);

	*m_pubKeyReader = keyLength;

	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.ECC_KEY.priKeyTag,m_priKeyTag,PKI_PRIVATEKEY_LEN);

	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.ECC_KEY.pubKeyReader,m_pubKeyReader,PKI_PUBLICKEY_LEN);
	pcCmdData.uParam.TAG_ATTR_PARAM.paraType = 1;//0���������� 1��ECC_key����
//	Sleep(1500);
	((CSerialDlg *)(theApp.m_pMainWnd))->send_cmd(&pcCmdData,sizeof(pcCmdData));
}


void CRegisterTagDlg::OnBnClickedButton1()		//���ı�ǩ�ŵ�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd));
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

	HANDLE startThread;
	BYTE *newChannel = new BYTE;
	*newChannel = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();

	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetTagChannelThread, (LPVOID)newChannel, 0, NULL);
	CloseHandle(startThread);

// 	BYTE newChannel = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
// 	int Ret = rfSetChannel(newChannel, 1);
// 	switch(Ret)
// 	{
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_OVERRANGE:
// 		AfxMessageBox("ע�⣺���ñ�ǩ�ŵ�����0-15��Χ");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
// 		AfxMessageBox("���ñ�ǩ�ŵ�ʧ��");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT:
// 		AfxMessageBox("���ñ�ǩ�ŵ�δ�յ��ظ�����ʱ");
// 		break;
// 	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS:
// 		AfxMessageBox("���ñ�ǩ�ŵ��ɹ�");
// 		break;
// 	default:
// 		break;
// 	}
}


void CRegisterTagDlg::SetTagChannelThread(LPVOID lparam)
{
	BYTE *newChannel = (BYTE*)lparam;
	int Ret = rfSetChannel(*newChannel, 1);
	switch(Ret)
	{
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_OVERRANGE:
		AfxMessageBox("ע�⣺���ñ�ǩ�ŵ�����0-15��Χ");
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED:
		AfxMessageBox("���ñ�ǩ�ŵ�ʧ��");
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT:
		AfxMessageBox("���ñ�ǩ�ŵ�δ�յ��ظ�����ʱ");
		break;
	case IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS:
		AfxMessageBox("���ñ�ǩ�ŵ��ɹ�");
		break;
	default:
		break;
	}
	delete newChannel;
}


LARGE_INTEGER MySleep(LARGE_INTEGER Interval)
{ 
	LARGE_INTEGER privious, current, Elapse;
	QueryPerformanceCounter( &privious );
	current = privious;
	while( current.QuadPart - privious.QuadPart < Interval.QuadPart )
		QueryPerformanceCounter( &current );
	Elapse.QuadPart = current.QuadPart - privious.QuadPart;
	return Elapse;
}


static void delay_100_us(void)
{
	LARGE_INTEGER t;
	double dFrequency;
	long long count;			//��ʱ����Ҫ�ļ���ֵ
	long long cntStart,cntEnd;
	QueryPerformanceCounter(&t);
	cntStart = t.QuadPart;

	QueryPerformanceFrequency(&t);
	dFrequency = (double)t.QuadPart;

	count = (dFrequency/10000);
	cntStart += count;
	while (1)
	{
		QueryPerformanceCounter(&t);
		cntEnd = t.QuadPart;
		if (cntEnd > cntStart)
			break;
	}
}


static void delayMs(unsigned int tMs)
{
	LARGE_INTEGER t;
	double dFrequency;
	long long count;			//��ʱ����Ҫ�ļ���ֵ
	long long cntStart,cntEnd;
	QueryPerformanceCounter(&t);
	cntStart = t.QuadPart;

	QueryPerformanceFrequency(&t);
	dFrequency = (double)t.QuadPart;
	/********************************************
	*
	*    ��ʱʱ��   T(s)            f*tMs
	*count = --------------- = -----------------
	*   ���������� T(1/f)(s)        1000(����λΪs)
	*
	********************************************/
	count = (long long)(dFrequency*tMs/1000.0);
	cntStart += count;
	while (1)
	{
		QueryPerformanceCounter(&t);
		cntEnd = t.QuadPart;
		if (cntEnd > cntStart)
			break;
	}
}


void CRegisterTagDlg::OnBnClickedButton2()		//����ͨ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CRegisterTagDlg::OnBnClickedButton3()		//�ͷ�ͨ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY cmdReply;
	CSerialDlg::initCmdData(pcCmdData,PC_CMD_TYPE_DISCONNECT_TAG);
	CSerialDlg *pdlg = (CSerialDlg *)(theApp.m_pMainWnd);

	((CSerialDlg *)(theApp.m_pMainWnd))->m_comReciveType = 1;//��1:���̴߳��ڲ�������
	((CSerialDlg *)(theApp.m_pMainWnd))->send_cmd(&pcCmdData,sizeof(pcCmdData));
//	Sleep(1);
	pdlg->GetCmdReplyFrameData(&cmdReply,sizeof(PC_CMD_REPLY));
	if(cmdReply.cmdType == PC_CMD_TYPE_DISCONNECT_TAG)
	{
		m_status = "�ͷ�ͨ�ųɹ�";
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	}
	else{
		m_status = "�ͷ�ͨ��ʧ��";
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	}

	UpdateData(FALSE);
	((CSerialDlg *)(theApp.m_pMainWnd))->m_comReciveType = 0;//��0:���̴߳���������

}


void CRegisterTagDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialogEx::OnTimer(nIDEvent);
}


void CRegisterTagDlg::OnBnClickedButton4()		//������ز�����ǩ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	int Ret = rfSendPurewave();
	switch(Ret)
	{
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_SUCCESS:
		AfxMessageBox("���䴿�ز��ɹ�");
		break;
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_FAILED:
		AfxMessageBox("���䴿�ز�ʧ��");
		break;
	case IF_TYPE_SEND_PUREWAVE | SEND_PUREVAVE_STATE_TIMEOUT:
		AfxMessageBox("���䴿�ز���ʱ");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedButton5()		// ֹͣ���ز�����ǩ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	int Ret = rfStopPurewave();
	switch(Ret)
	{
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_SUCCESS:
		PRINT ("ֹͣ���ز��ɹ�\n");
		break;
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_FAILED:
		PRINT ("ֹͣ���ز�ʧ��\n");
		break;
	case IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_TIMEOUT:
		PRINT ("ֹͣ���ز���ʱ\n");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedButton6()		// ���������Ϣ����ǩ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	int Ret = rfSendModulInfo();
	switch(Ret)
	{
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_SUCCESS:
		PRINT ("���������Ϣ�ɹ�\n");
		break;
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_FAILED:
		PRINT ("���������Ϣʧ��\n");
		break;
	case IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_TIMEOUT:
		PRINT ("���������Ϣ��ʱ\n");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedButton9()		// �رյ�����Ϣ����ǩ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	int Ret = rfStopModulInfo();
	switch(Ret)
	{
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_SUCCESS:
		PRINT ("���������Ϣ�ɹ�\n");
		break;
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_FAILED:
		PRINT ("���������Ϣʧ��\n");
		break;
	case IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_TIMEOUT:
		PRINT ("���������Ϣ��ʱ\n");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedButton17()		// ���ñ�ǩ���书��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

	HANDLE startThread;
	BYTE *newPower = new BYTE;
	*newPower = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();

	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetTagPowerThread, (LPVOID)newPower, 0, NULL);
	CloseHandle(startThread);

// 	BYTE newPower = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();
// 	int Ret = rfSetPower(newPower, 1);
// 	switch(Ret)
// 	{
// 	case IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_OVERRANGE:
// 		AfxMessageBox("ע�⣺���ñ�ǩ���ʳ���0-8��Χ");
// 		break;
// 	case IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_FAILED:
// 		AfxMessageBox("���ñ�ǩ����ʧ��");
// 		break;
// 	case IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_TIMEOUT:
// 		AfxMessageBox("���ñ�ǩ���ʳ�ʱ");
// 		break;
// 	case IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_SUCCESS:
// 		AfxMessageBox("���ñ�ǩ���ʳɹ�");
// 		break;
// 	default:
// 		break;
// 	}
}


void CRegisterTagDlg::SetTagPowerThread(LPVOID lparam)
{
	BYTE *newPower = (BYTE *)lparam;

	int Ret = rfSetPower(*newPower, 1);
	switch(Ret)
	{
	case IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_OVERRANGE:
		AfxMessageBox("ע�⣺���ñ�ǩ���ʳ���0-8��Χ");
		break;
	case IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_FAILED:
		AfxMessageBox("���ñ�ǩ����ʧ��");
		break;
	case IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_TIMEOUT:
		AfxMessageBox("���ñ�ǩ���ʳ�ʱ");
		break;
	case IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_SUCCESS:
		AfxMessageBox("���ñ�ǩ���ʳɹ�");
		break;
	default:
		break;
	}
	delete lparam;
}


void CRegisterTagDlg::OnBnClickedButton7()		// ���ý���ģʽ����ǩ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	int Ret = rfSetRecvModel();
	switch(Ret)
	{
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS:
		AfxMessageBox("���ý���ģʽ�ɹ�");
		break;
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED:
		AfxMessageBox("���ý���ģʽʧ��");
		break;
	case IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_TIMEOUT:
		AfxMessageBox("���ý���ģʽ��ʱ");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedButton8()		// ֹͣ����ģʽ����ǩ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	int Ret = rfStopRecvModel();
	switch(Ret)
	{
	case IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS:
		AfxMessageBox("ֹͣ����ģʽ�ɹ�");
		break;
	case IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED:
		AfxMessageBox("ֹͣ����ģʽʧ��");
		break;
	case IF_TYPE_STOP_RECV_MODEL | STOP_RECV_MODEL_STATE_TIMEOUT:
		AfxMessageBox("ֹͣ����ģʽ��ʱ");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedBtnGetTagInfo()	//��ȡ��ǩ������Ϣ�����У�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

	m_TID.Empty();
	m_UII.Empty();
	UpdateData(TRUE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetTagInfoThread, this, 0, NULL);
	CloseHandle(startThread);
}


void CRegisterTagDlg::GetTagInfoThread(LPVOID lparam)
{
	CRegisterTagDlg  *pdlg = (CRegisterTagDlg *)lparam; //��ø�����ָ��
	CString tmpTID = "0";	//��ǩ��ѯTID
	CString tmpUII = "0";	//��ǩ��ѯUII
	CString tmpMemoryVolume = 0;	//��ǩ��ѯ�洢������
	CString tmpInqMaxNum = 0;		//��ǩ��ѯԪ���������
	CString tmpInqMaxMatchNum = 0;	//��ǩ��ѯ������ƥ������
	TAG_ATTRIBUTE_R  tagInfo;

	int Ret = rfGetTagInfo(&tagInfo);
	switch(Ret)
	{
	case IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_SUCCESS:
		{
			AfxMessageBox("��ȡ��ǩ��Ϣ�ɹ�");
			TAG_ATTRIBUTE_R  temptagInfo;
			memcpy(&temptagInfo, &tagInfo, sizeof(TAG_ATTRIBUTE_R));

			tmpTID.Format("%c%c%c%c%c%c%c%c",
				temptagInfo.TID[0], temptagInfo.TID[1],
				temptagInfo.TID[2], temptagInfo.TID[3],
				temptagInfo.TID[4], temptagInfo.TID[5],
				temptagInfo.TID[6], temptagInfo.TID[7]);
			pdlg->m_TID = tmpTID;
			tmpUII.Format("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				temptagInfo.UII[0],temptagInfo.UII[1],temptagInfo.UII[2],temptagInfo.UII[3],temptagInfo.UII[4],
				temptagInfo.UII[5],temptagInfo.UII[6],temptagInfo.UII[7],temptagInfo.UII[8],temptagInfo.UII[9],
				temptagInfo.UII[10],temptagInfo.UII[11],temptagInfo.UII[12],temptagInfo.UII[13],temptagInfo.UII[14],
				temptagInfo.UII[15],temptagInfo.UII[16],temptagInfo.UII[17],temptagInfo.UII[18],temptagInfo.UII[19],
				temptagInfo.UII[20],temptagInfo.UII[21],temptagInfo.UII[22],temptagInfo.UII[23],temptagInfo.UII[24],
				temptagInfo.UII[25],temptagInfo.UII[26],temptagInfo.UII[27],temptagInfo.UII[28],temptagInfo.UII[29],
				temptagInfo.UII[30],temptagInfo.UII[31]);
			pdlg->m_UII = tmpUII;
			tmpMemoryVolume.Format("�洢��������%d", temptagInfo.memoryVolume);
			pdlg->m_memoryVolume = temptagInfo.memoryVolume;
			tmpInqMaxNum.Format("��ѯԪ�����������%d", temptagInfo.inqMaxNum);
			pdlg->m_inqMaxNum = temptagInfo.inqMaxNum;
			tmpInqMaxMatchNum.Format("��ѯ������ƥ��������%d", temptagInfo.inqMaxMatchNum);
			pdlg->m_inqMaxMatchNum = temptagInfo.inqMaxMatchNum;
		}
		break;
	case IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_FAILED:
		AfxMessageBox("��ȡ��ǩ��Ϣʧ��");
		break;
	default:
		AfxMessageBox("��ȡ��ǩ��Ϣ��ʱ");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedBtnGetChannel()	//��ȡ��ǩ�ŵ������У�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

	UpdateData(TRUE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetTagChannelThread, this, 0, NULL);
	CloseHandle(startThread);
}


void CRegisterTagDlg::GetTagChannelThread(LPVOID lparam)
{
	CRegisterTagDlg  *pdlg = (CRegisterTagDlg *)lparam; //��ø�����ָ��
	TAG_ATTRIBUTE_R  tagInfo;

	int Ret = rfGetTagInfo(&tagInfo);
	switch(Ret)
	{
	case IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_SUCCESS:
		{
			AfxMessageBox("��ȡ��ǩ�ŵ��ɹ�");
			TAG_ATTRIBUTE_R  temptagInfo;
			memcpy(&temptagInfo, &tagInfo, sizeof(TAG_ATTRIBUTE_R));

			((CComboBox*)pdlg->GetDlgItem(IDC_COMBO1))->SetCurSel(temptagInfo.Channel);
		}
		break;
	case IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_FAILED:
		AfxMessageBox("��ȡ��ǩ�ŵ�ʧ��");
		break;
	default:
		AfxMessageBox("��ȡ��ǩ�ŵ���ʱ");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedBtnGetPower()		//��ȡ��ǩ���ʣ����У�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

	UpdateData(TRUE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetTagPowerThread, this, 0, NULL);
	CloseHandle(startThread);
}


void CRegisterTagDlg::GetTagPowerThread(LPVOID lparam)
{
	CRegisterTagDlg  *pdlg = (CRegisterTagDlg *)lparam; //��ø�����ָ��
	TAG_ATTRIBUTE_R  tagInfo;

	int Ret = rfGetTagInfo(&tagInfo);
	switch(Ret)
	{
	case IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_SUCCESS:
		{
			AfxMessageBox("��ȡ��ǩ���ʳɹ�");
			TAG_ATTRIBUTE_R  temptagInfo;
			memcpy(&tagInfo, &temptagInfo, sizeof(TAG_ATTRIBUTE_R));

			((CComboBox*)pdlg->GetDlgItem(IDC_COMBO2))->SetCurSel(temptagInfo.Power);
		}
		break;
	case IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_FAILED:
		AfxMessageBox("��ȡ��ǩ����ʧ��");
		break;
	default:
		AfxMessageBox("��ȡ��ǩ���ʳ�ʱ");
		break;
	}
}


void CRegisterTagDlg::OnBnClickedButton10() //�����������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

	UpdateData(TRUE);
	m_status.Empty();
	UpdateData(FALSE);

	CSerialDlg newDlg;
	
	unsigned char tmpTid[8];
	int tmpLen = newDlg.StrToHex(m_JianceTid, (char *)tmpTid);
	if(tmpLen != 8)
	{
		AfxMessageBox("����TID�ĳ���Ӧ����8�ֽ�");
		return;
	}
	char *strFileID = (LPSTR)(LPCTSTR)(m_FileID);
	WORD fileID = strtol(strFileID, NULL, 16); //ʮ������ת������
	char *strOffset = (LPSTR)(LPCTSTR)(m_Offset);
	BYTE Offset = strtol(strOffset, NULL, 16);
	char *strDataLen = (LPSTR)(LPCTSTR)(m_DataLen);
	BYTE DataLen = strtol(strDataLen, NULL, 16);
	char *strSlotNum = (LPSTR)(LPCTSTR)(m_SlotNum);
	WORD SlotNum = strtol(strSlotNum, NULL, 16);
	char *strUpCycle = (LPSTR)(LPCTSTR)(m_UpCycle);
	WORD UpCycle = strtol(strUpCycle, NULL, 16);
	char *strJianceCycle = (LPSTR)(LPCTSTR)(m_JianceCycle);
	WORD JianceCycle = strtol(strJianceCycle, NULL, 16);
	char *strSlotWidth = (LPSTR)(LPCTSTR)(m_SlotWidth);
	WORD SlotWidth = strtol(strSlotWidth, NULL, 16);
	char *strSlotSum = (LPSTR)(LPCTSTR)(m_SlotSum);
	WORD SlotSum = strtol(strSlotSum, NULL, 16);
	CycleNum = SlotSum; //�������ֵ���������ʼѭ����������
	UpcycleNum = UpCycle; //�ϱ�����
	char *strSendRecvIndicate = (LPSTR)(LPCTSTR)(m_SendRecvIndicate);
	BYTE SendRecvIndicate = strtol(strSendRecvIndicate, NULL, 16);

	int Ret = rfMonitorTrConf(fileID, Offset, DataLen, SlotNum, UpCycle,
		JianceCycle, SlotWidth, SlotSum, SendRecvIndicate, tmpTid);
	switch(Ret)
	{
	case IF_TYPE_MONITOR_CONF | CONF_MONITOR_SUCCESS:
		m_status = "���ü���������óɹ�";
		break;
	default:
		m_status = "���ü����������ʧ��";
		break;
	}
	UpdateData(FALSE);
}


void CRegisterTagDlg::OnBnClickedButton12() //���������ʼ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

	UpdateData(TRUE);
	m_status.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartJianceThread, this, 0, NULL);
	CloseHandle(startThread);
}


void CRegisterTagDlg::StartJianceThread(LPVOID lparam)
{
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd));
	CRegisterTagDlg *pTagDlg = (CRegisterTagDlg *)lparam;

	int Ret = rfMonitorTrStart();
	if(Ret != (IF_TYPE_MONITOR_START | MONITOR_START_STATE_SUCCESS))
	{
		//rfMonitorTrStop(5);
		AfxMessageBox("���ͼ����ʼʧ��");
		return;
	}

	//��������ϱ�
	BYTE Tid[9]={0};
	BYTE Data[80]={0};
	CYCLE_INFO cycleInfo[100];

	for(int j = 0; j < UpcycleNum; j++)
	{
		for(int i=0; i < CycleNum; i++)
		{
			if((IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_SUCCESS) == rfMonitorTrReport(10, &cycleInfo[i]))
			{
				memcpy(Tid, cycleInfo[i].TID, 8);
				memcpy(Data, cycleInfo[i].Data, 80);

				CString strTmp;
				pdlg->m_recFileContent += "Tid:";
				for(int Tmp = 0; Tmp < 8; Tmp++)
				{
					strTmp.Format("%02x", Tid[Tmp]);
					pdlg->m_recFileContent += strTmp;
				}
				strTmp.Format("  Data:%s\r\n", Data);
				pdlg->m_recFileContent += strTmp;
				pdlg->SendMessage(MY_MESSAGE, NULL, NULL);
			}
		}
	}
	
	//�������ֹͣ
	//rfMonitorTrStop(5);
	AfxMessageBox("������");
}


void CRegisterTagDlg::OnBnClickedButton13() //���ֹͣ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSerialDlg *pdlg = ((CSerialDlg *)(theApp.m_pMainWnd)); 
	if (pdlg->m_OpenCommunication == FALSE)
	{
		AfxMessageBox("���ȴ�ͨ�Ŷ˿�");
		return;
	}

	UpdateData(TRUE);

	int Ret = rfMonitorTrStop(5);
	switch(Ret)
	{
	case IF_TYPE_MONITOR_START | MONITOR_START_STATE_SUCCESS:
		m_status = "���ֹͣ�ɹ�";
		break;
	default:
		m_status = "���ֹͣʧ��";
		break;
	}
}
