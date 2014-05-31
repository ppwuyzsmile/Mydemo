// ReaderTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Serial.h"
#include "ReaderTestDlg.h"
#include "afxdialogex.h"
#include "pcCommand.h"
#include "SerialDlg.h"
#include "..\..\API二期\APIDLL\apidll.h"


// CReaderTestDlg 对话框

IMPLEMENT_DYNAMIC(CReaderTestDlg, CDialogEx)

CReaderTestDlg::CReaderTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReaderTestDlg::IDD, pParent)
{

	m_InfoStatus = _T("");
	m_ReadyToken = 0;
	m_CycleNum = 5;
	m_RequestNum = 12500;
	m_TestRid = _T("303030");
	m_TestTid = _T("a1a2a3a4a5a6a7a8");
	m_Path = _T("D:\\");
}

CReaderTestDlg::~CReaderTestDlg()
{
}

void CReaderTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INFOSTATUS, m_InfoStatus);
	DDX_Control(pDX, IDC_FREQ_ACCURACY, m_FreqAccuracy);
	DDX_Control(pDX, IDC_MINRECVBAND, m_MinRecvBand);
	DDX_Control(pDX, IDC_SWITCHTIME, m_SwitchTime);
	DDX_Control(pDX, IDC_MODULATEMODE, m_ModulateMode);
	DDX_Control(pDX, IDC_BITSPEED, m_BitSpeed);
	DDX_Control(pDX, IDC_BEFORECODE, m_BeforeCode);
	DDX_Control(pDX, IDC_TAGCOLLIONMANAGE, m_TagCollionManage);
	DDX_Control(pDX, IDC_FSSSSEQUENCE, m_FsssSequence);
	DDX_Control(pDX, IDC_FREQRANGE, m_FreqRange);
	DDX_Control(pDX, IDC_CHANNELBAND, m_ChannelBand);
	DDX_Control(pDX, IDC_MAXSENDPOWER, m_MaxSendPower);
	DDX_Control(pDX, IDC_ZASANRADIO, m_ZasanRadio);
	DDX_Control(pDX, IDC_DAIWAIRADIOPOWER, m_DaiwaiRadioPower);
	DDX_Control(pDX, IDC_FREQACCRY, m_FreqAccry);
	DDX_Control(pDX, IDC_RECVSENSITIVITY, m_RecvSensitivity);
	DDX_Control(pDX, IDC_MOVEIDENTISPEED, m_MoveIdentiSpeed);
	DDX_Text(pDX, IDC_ReadyToken, m_ReadyToken);
	DDX_Text(pDX, IDC_CYCLENUM, m_CycleNum);
	DDX_Text(pDX, IDC_REQUESTNUM, m_RequestNum);
	DDX_Text(pDX, IDC_TESTRID, m_TestRid);
	DDX_Text(pDX, IDC_TESTTID, m_TestTid);
	DDX_Control(pDX, IDC_TAGWORKFREQ, m_TagWorkFreq);
	DDX_Control(pDX, IDC_EQUALPOWER, m_EqualPower);
	DDX_Control(pDX, IDC_TAGSENDBAND, m_TagSendBand);
	DDX_Text(pDX, IDC_EDIT1, m_Path);
}


BEGIN_MESSAGE_MAP(CReaderTestDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FREQ_ACCURACY, &CReaderTestDlg::OnBnClickedFreqAccuracy)
	ON_MESSAGE(RENEWINFOSTATUS, &CReaderTestDlg::OnRenewinfostatus)
	ON_BN_CLICKED(IDC_MINRECVBAND, &CReaderTestDlg::OnBnClickedMinrecvband)
	ON_BN_CLICKED(IDC_SWITCHTIME, &CReaderTestDlg::OnBnClickedSwitchtime)
	ON_BN_CLICKED(IDC_MODULATEMODE, &CReaderTestDlg::OnBnClickedModulatemode)
	ON_BN_CLICKED(IDC_BITSPEED, &CReaderTestDlg::OnBnClickedBitspeed)
	ON_BN_CLICKED(IDC_BEFORECODE, &CReaderTestDlg::OnBnClickedBeforecode)
	ON_BN_CLICKED(IDC_TAGCOLLIONMANAGE, &CReaderTestDlg::OnBnClickedTagcollionmanage)
	ON_BN_CLICKED(IDC_FSSSSEQUENCE, &CReaderTestDlg::OnBnClickedFssssequence)
	ON_BN_CLICKED(IDC_FREQRANGE, &CReaderTestDlg::OnBnClickedFreqrange)
	ON_BN_CLICKED(IDC_CHANNELBAND, &CReaderTestDlg::OnBnClickedChannelband)
	ON_BN_CLICKED(IDC_MAXSENDPOWER, &CReaderTestDlg::OnBnClickedMaxsendpower)
	ON_BN_CLICKED(IDC_ZASANRADIO, &CReaderTestDlg::OnBnClickedZasanradio)
	ON_BN_CLICKED(IDC_DAIWAIRADIOPOWER, &CReaderTestDlg::OnBnClickedDaiwairadiopower)
	ON_BN_CLICKED(IDC_FREQACCRY, &CReaderTestDlg::OnBnClickedFreqaccry)
	ON_BN_CLICKED(IDC_MOVEIDENTISPEED, &CReaderTestDlg::OnBnClickedMoveidentispeed)
	ON_BN_CLICKED(IDC_RECVSENSITIVITY, &CReaderTestDlg::OnBnClickedRecvsensitivity)
	ON_BN_CLICKED(IDC_TAGWORKFREQ, &CReaderTestDlg::OnBnClickedTagworkfreq)
	ON_BN_CLICKED(IDC_EQUALPOWER, &CReaderTestDlg::OnBnClickedEqualpower)
	ON_BN_CLICKED(IDC_TAGSENDBAND, &CReaderTestDlg::OnBnClickedTagsendband)
END_MESSAGE_MAP()


// CReaderTestDlg 消息处理程序
afx_msg LRESULT CReaderTestDlg::OnRenewinfostatus(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return 0;
}


void CReaderTestDlg::InitTestCmd(PC_CMD_DATA& pcCmdData, PC_CMD_TYPE cmdType)
{
	memset(&pcCmdData,0,sizeof(PC_CMD_DATA));
	pcCmdData.cmdType = cmdType;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
}


void CReaderTestDlg::OnBnClickedFreqAccuracy()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreqAccuracyThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::FreqAccuracyThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_FREQ_ACCURACY);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送频率及准确度命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_FREQ_ACCURACY, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收频率及准确度失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testFreqAccuracyParam.testFreqAccuacyState)
	{
	case TEST_FREQ_ACCURACY_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试频率及准确度返回状态：成功";
		break;
	case TEST_FREQ_ACCURACY_STATE_FAILED:
		pDlg->m_InfoStatus = "测试频率及准确度返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "频率及准确度未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedMinrecvband()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MinRecvBandThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::MinRecvBandThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_MIN_RCVOR_BAND);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送最小接收机带宽命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_MIN_RCVOR_BAND, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收最小接收机带宽命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testMinRcvorBandParam.testMinRcvorBandState)
	{
	case TEST_MIN_RCVOR_BAND_STATE_SUCCESS:
		{
			CString tmpStr;
			tmpStr.Format("测试最小接收机带宽成功，返回状态：%d，返回数据：%s",
				pcCmdReply.uReplyParam.testMinRcvorBandParam.TagState,
				pcCmdReply.uReplyParam.testMinRcvorBandParam.Data);

			pDlg->m_InfoStatus = tmpStr;
			break;
		}
	case TEST_MIN_RCVOR_BAND_STATE_FAILED:
		pDlg->m_InfoStatus = "测试最小接收机带宽返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "最小接收机带宽未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedSwitchtime() //读写器测试转换时间
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SwitchTimeThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::SwitchTimeThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_READER_SWITCH_TIME);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送转换时间命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_READER_SWITCH_TIME, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收转换时间命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testSwitchTimeParam.testSwitchTimeState)
	{
	case TEST_SWITCH_TIME_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试转换时间返回状态：成功";
		break;
	case TEST_SWITCH_TIME_STATE_FAILED:
		pDlg->m_InfoStatus = "测试转换时间返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "转换时间未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedModulatemode()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ModulateModeThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::ModulateModeThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_MODUL_MODE);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送调制方式命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_MODUL_MODE, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收调制方式命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testModulModeParam.testModulModeState)
	{
	case TEST_MODUL_MODE_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试调制方式返回状态：成功";
		break;
	case TEST_MODUL_MODE_STATE_FAILED:
		pDlg->m_InfoStatus = "测试调制方式返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "调制方式未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedBitspeed()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BitSpeedThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::BitSpeedThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_BIT_SPEED);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送位速率命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_BIT_SPEED, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收位速率命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testBitSpeedParam.testBitSpeedState)
	{
	case TEST_BIT_SPEED_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试位速率返回状态：成功";
		break;
	case TEST_BIT_SPEED_STATE_FAILED:
		pDlg->m_InfoStatus = "测试位速率返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "位速率未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedBeforecode()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BeforeCodeThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::BeforeCodeThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_BEFORE_CODE);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送前导码命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_BEFORE_CODE, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收前导码命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testBeforeCodeParam.testBeforeCodeState)
	{
	case TEST_BEFORE_CODE_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试前导码返回状态：成功";
		break;
	case TEST_BEFORE_CODE_STATE_FAILED:
		pDlg->m_InfoStatus = "测试前导码返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "前导码未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedTagcollionmanage()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TagCollionManageThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::TagCollionManageThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam;
	CString tmpStr;

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_PREVCOLLI_MANAGE);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送标签防碰撞管理命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_PREVCOLLI_MANAGE, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收标签防碰撞管理命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testPreColliManageParam.testPreColliManageState)
	{
	case TEST_PREVCOLLI_MANAGE_STATE_SUCCESS:
		(pDlg->m_InfoStatus).Format("防碰撞管理测试完成。盘点时间为%d毫秒，盘到的个数为%d个", 
			pcCmdReply.uReplyParam.testPreColliManageParam.CollectTime,
			pcCmdReply.uReplyParam.testPreColliManageParam.CollectedNum);
		break;
	case TEST_PREVCOLLI_MANAGE_STATE_FAILED:
		pDlg->m_InfoStatus = "测试标签防碰撞管理返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "标签防碰撞管理未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedFssssequence()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FsssSequenceThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::FsssSequenceThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_FSSS_SEQUENCE);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送扩频序列命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_FSSS_SEQUENCE, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收扩频序列命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testFsssSequencyParam.testFsssSequencyState)
	{
	case TEST_FSSS_SEQUENCE_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试扩频序列返回状态：成功";
		break;
	case TEST_FSSS_SEQUENCE_STATE_FAILED:
		pDlg->m_InfoStatus = "测试扩频序列返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "扩频序列未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedFreqrange()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreqRangeThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::FreqRangeThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_WORK_FREQ_RANGE);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送工作频率范围命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_WORK_FREQ_RANGE, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收工作频率范围命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testWorkFreqRangeParam.testWorkFreqRangeState)
	{
	case TEST_WORK_FREQ_RANGE_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试工作频率范围返回状态：成功";
		break;
	case TEST_WORK_FREQ_RANGE_STATE_FAILED:
		pDlg->m_InfoStatus = "测试工作频率范围返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "工作频率范围未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedChannelband()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ChannelBandThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::ChannelBandThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_CHANNEL_BAND);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送占用信道带宽命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_CHANNEL_BAND, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收占用信道带宽命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testChannelBandParam.testChannelBandState)
	{
	case TEST_CHANNEL_BAND_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试占用信道带宽返回状态：成功";
		break;
	case TEST_CHANNEL_BAND_STATE_FAILED:
		pDlg->m_InfoStatus = "测试占用信道带宽返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "占用信道带宽未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedMaxsendpower()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MaxSendPowerThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::MaxSendPowerThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_MAX_SEND_POWER);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送最大发射功率命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_MAX_SEND_POWER, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收最大发射功率命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testMaxSendPowerParam.testMaxSendPowerState)
	{
	case TEST_MIN_RCVOR_BAND_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试最大发射功率返回状态：成功";
		break;
	case TEST_MIN_RCVOR_BAND_STATE_FAILED:
		pDlg->m_InfoStatus = "测试最大发射功率返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "最大发射功率未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedZasanradio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ZasanRadioThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::ZasanRadioThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_ZASAN_RADIO);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送杂散辐射命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_ZASAN_RADIO, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收杂散辐射命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testZasanRadioParam.testZasanRadioState)
	{
	case TEST_ZASAN_RADIO_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试杂散辐射返回状态：成功";
		break;
	case TEST_ZASAN_RADIO_STATE_FAILED:
		pDlg->m_InfoStatus = "测试杂散辐射返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "杂散辐射未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedDaiwairadiopower()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DaiwanRadioPowerThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::DaiwanRadioPowerThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_DAIWAI_RADIO_POWER);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送带外辐射功率命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_DAIWAI_RADIO_POWER, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收带外辐射功率命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testDaiwaiRadioPowerParam.testDaiwaiRadioPowerState)
	{
	case TEST_DAIWAI_RADIO_POWER_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试带外辐射功率返回状态：成功";
		break;
	case TEST_DAIWAI_RADIO_POWER_STATE_FAILED:
		pDlg->m_InfoStatus = "测试带外辐射功率返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "带外辐射功率未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedFreqaccry()
{
	// TODO: 在此添加控件通知处理程序代码
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreqAccryThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::FreqAccryThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_FREQ_ACCRY_P);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送频率准确度命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_TEST_FREQ_ACCRY_P, TIME_OUT*3) != 1)
	{
		pDlg->m_InfoStatus = "接收频率准确度命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}
	switch(pcCmdReply.uReplyParam.testFreqAccryQParam.testFreqAccryQState)
	{
	case TEST_FREQ_ACCRY_P_STATE_SUCCESS:
		pDlg->m_InfoStatus = "测试频率准确度返回状态：成功";
		break;
	case TEST_FREQ_ACCRY_P_STATE_FAILED:
		pDlg->m_InfoStatus = "测试频率准确度返回状态：失败";
		break;
	default:
		pDlg->m_InfoStatus = "频率准确度未知错误";
	}

	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}


void CReaderTestDlg::OnBnClickedMoveidentispeed() //移动识别速度
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MoveIdentiSpeedThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::MoveIdentiSpeedThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam;

	pDlg->m_InfoStatus = "移动识别速度，测试正在进行中......";
	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);

// 	PC_CMD_DATA pcCmdData;
// 
// 	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_MOVE_IDENTI_SPEED);
// 	pcCmdData.uParam.MOVE_IDENTI_SPEED_PARAM.cycleTimes = pDlg->m_CycleNum;
// 	pcCmdData.uParam.MOVE_IDENTI_SPEED_PARAM.readyToken = pDlg->m_ReadyToken;
// 
// 	if(TestSendCmd(&pcCmdData) != 0)
// 	{
// 		pDlg->m_InfoStatus = "发送命令失败";
// 		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
// 		return;
// 	}

	unsigned int tmpCycleNum = 0; //循环盘存的次数
	unsigned int successNum = 0; //成功的次数
	unsigned int Flag = pDlg->m_CycleNum;

	//创建文件
	//FILE *fp;
	//fp = fopen("移动识别速度测试", "wb"); //打开文件

	while(tmpCycleNum < Flag)
	{
		PC_CMD_DATA pcCmdData;
		InitTestCmd(pcCmdData, PC_CMD_TYPE_COLLECT);
		pcCmdData.uParam.INVENTORY_TAG_PARAM.readyToken = 0;
		pcCmdData.uParam.INVENTORY_TAG_PARAM.retryTimes = 1;
		if(TestSendCmd(&pcCmdData) != 0)
		{
			pDlg->m_InfoStatus = "发送移动识别速度命令失败";
			pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
			return;
		}

		tmpCycleNum++;

		unsigned int tagNumNow = 0;
		unsigned int collectTagNums = 0;
		unsigned char Tid[9];
		unsigned char Uii[33];
		TAG_INFO tagInfoArray[30];
		PC_CMD_REPLY pcCmdReply;

		memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
		memset(&tagInfoArray, 0, sizeof(TAG_INFO)*30);

		while(1)
		{
			if(TestRecvCmd(&pcCmdReply, PC_CMD_TYPE_COLLECT, 20*TIME_OUT) != 1)
			{
				pDlg->m_InfoStatus = "测试移动识别速度阅读器接收失败";
				pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
				break;
			}

			tagNumNow += pcCmdReply.uReplyParam.collectParam.curNumOfTag;

			for(int i = 0; i < pcCmdReply.uReplyParam.collectParam.curNumOfTag; i++)
			{
				memset(Tid, 0, 9);
				memset(Uii, 0, 33);
				tagInfoArray[collectTagNums++] = pcCmdReply.uReplyParam.collectParam.tagInfo[i];
				memcpy(Tid, tagInfoArray[collectTagNums - 1].TID, 8);
				memcpy(Uii, tagInfoArray[collectTagNums - 1].UII, 32);

				PRINT("标签ID:%s\n标签电量:%d\nUII:%s\n", Tid, tagInfoArray[collectTagNums - 1].tagState, Uii);
				/*----------此处添加写文件操作，把标签信息写入文件中进行比较----------*/
				//fprintf(fp, "标签ID:%s  标签电量:%d  UII:%s\n", Tid, tagInfoArray[collectTagNums - 1].tagState, Uii);
			}

			if (tagNumNow == pcCmdReply.uReplyParam.collectParam.numsOfAllTag)
			{
				if(tagNumNow == 0)
					break;

				successNum++;
				PRINT("盘存命令完成！\n");
				PRINT("共收到 %d 个标签，需求 %d 个标签.\n", tagNumNow, pcCmdReply.uReplyParam.collectParam.numsOfAllTag);
				break;
			}
		}

		(pDlg->m_InfoStatus).Format("移动识别速度测试，第%d次循环执行完成，成功第%d次，继续执行下一个循环......", tmpCycleNum, successNum);
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);

		Sleep(8000); //进入下一次盘存前休息片刻，等待标签由会话状态转到就绪状态
	}

	//fclose(fp); //关闭文件
}

void CReaderTestDlg::OnBnClickedRecvsensitivity()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvSensitivityThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::RecvSensitivityThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam;

	PC_CMD_DATA pcCmdData;
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TEST_RCV_SENSITIVITY);
	pcCmdData.uParam.RCV_SENSITIVITY.cycleTimes = pDlg->m_RequestNum;

	CSerialDlg newDlg;
	char tmpTid[10];
	int tmpLen = newDlg.StrToHex(pDlg->m_TestTid, tmpTid);
	memcpy(pcCmdData.uParam.RCV_SENSITIVITY.TID, tmpTid, tmpLen);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送接收灵敏度命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	//创建文件
	FILE *fp;
	//fp = fopen("..\\..\\DEMO\\Debug\\接收灵敏度测试报告", "wb"); //打开文件
	CString tmpPath = pDlg->m_Path + "接收灵敏度测试报告.bin";
	fp = fopen(tmpPath, "wb");

// 	const char* pstrFileName = "D:\\接收灵敏度测试报告.txt";
// 	HANDLE handleFile = CreateFile(pstrFileName, GENERIC_READ | GENERIC_WRITE,
// 		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
	int tmpNum = 0;
	int endFlag = pDlg->m_RequestNum;
	while(endFlag)
	{
		(pDlg->m_InfoStatus).Format("测试接收灵敏度，请求随机数命令正在进行中......已接收%d", tmpNum);
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);

		BYTE RandomNum[128];
		int Ret = TestRecvSensitivityCmd(RandomNum, 3);
		if(Ret != 0)
		{
			/*----------此处添加写文件操作，把返回的请求随机数写入文件中----------*/
			for(int i = 0; i < Ret; i++)
			{
				fwrite(&RandomNum[i], sizeof(unsigned char), 1, fp);
			}
			endFlag -= Ret;
			tmpNum += Ret;
			
			//fprintf(fp, "%x\r\n", &RandomNum[0]);
			//DWORD NumberOfBytesWritten;
			//char tmpbuffer[3] = "\r\n"; //这里/r/n是回车换行符，占两个字节
			//SetFilePointer(handleFile, 0, NULL, FILE_END); //把文件指针指向文件尾
			//WriteFile(handleFile, (LPVOID)RandomNum, 1, &NumberOfBytesWritten, NULL);
			//WriteFile(handleFile, tmpbuffer, 2, &NumberOfBytesWritten, NULL); //每写入一个随机数加入换行
		}
		else
			continue;
	}

	fclose(fp); //关闭文件
	//CloseHandle(handleFile); //关闭文件句柄
	pDlg->m_InfoStatus = "接收灵敏度测试完成";
	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}

void CReaderTestDlg::OnBnClickedTagworkfreq()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TagFreqAccuracyThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::TagFreqAccuracyThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam;
	CSerialDlg newDlg;
	char data1[3], data2[8];

	PC_CMD_DATA pcCmdData;
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TAG_TEST_FREQ_AND_ACCRY);
	int tmpLen1 = newDlg.StrToHex(pDlg->m_TestRid, data1);
	int tmpLen2 = newDlg.StrToHex(pDlg->m_TestTid, data2);

	pcCmdData.uParam.TEST_FREQ_AND_ACCRY.cmdCode = PC_CMD_TYPE_TAG_TEST_FREQ_AND_ACCRY;
	pcCmdData.uParam.TEST_FREQ_AND_ACCRY.Length = 1 + tmpLen1 + tmpLen2;
	memcpy(pcCmdData.uParam.TEST_FREQ_AND_ACCRY.RID, data1, tmpLen1);
	memcpy(pcCmdData.uParam.TEST_FREQ_AND_ACCRY.TID, data2, tmpLen2);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送工作频率准确度命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	else
	{
		pDlg->m_InfoStatus = "发送工作频率准确度命令成功";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
	}

// 	FILE *fp;
// 	fp = fopen("..\\..\\DEMO\\Debug\\接收灵敏度测试报告", "wb"); //打开文件
// 
// 	int endFlag = 10;
// 	while(endFlag--)
// 	{
// 		BYTE RandomNum = 8;
// 
// 		/*----------此处添加写文件操作，把返回的请求随机数写入文件中----------*/
// 		fprintf(fp, "%d\r\n", RandomNum);
// 
// 	}
// 	fclose(fp);
// 	pDlg->m_InfoStatus = "......完成";
// 	pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
}

void CReaderTestDlg::OnBnClickedEqualpower()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TagEqualPowerThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::TagEqualPowerThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TAG_TEST_EQUAL_POWER);

	pcCmdData.uParam.TEST_EQUAL_POWER.cmdCode = PC_CMD_TYPE_TAG_TEST_EQUAL_POWER;
	pcCmdData.uParam.TEST_EQUAL_POWER.Length = 1 + (pDlg->m_TestRid).GetLength() +(pDlg->m_TestTid).GetLength();
	memcpy(pcCmdData.uParam.TEST_EQUAL_POWER.RID, pDlg->m_TestRid, (pDlg->m_TestRid).GetLength());
	memcpy(pcCmdData.uParam.TEST_EQUAL_POWER.TID, pDlg->m_TestTid, (pDlg->m_TestTid).GetLength());

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送等效辐射功率命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	else
	{
		pDlg->m_InfoStatus = "发送等效辐射功率命令成功";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
	}
}

void CReaderTestDlg::OnBnClickedTagsendband()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_InfoStatus.Empty();
	UpdateData(FALSE);

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TagSendBandThreadFunc, this, 0, NULL);
	CloseHandle(startThread);
}

void CReaderTestDlg::TagSendBandThreadFunc(LPVOID lparam)
{
	CReaderTestDlg *pDlg = (CReaderTestDlg *)lparam; 

	PC_CMD_DATA pcCmdData;
	InitTestCmd(pcCmdData, PC_CMD_TYPE_TAG_TEST_SEND_CHANNEL_BAND);

	pcCmdData.uParam.TEST_SEND_CHANNEL_BAND.cmdCode = PC_CMD_TYPE_TAG_TEST_SEND_CHANNEL_BAND;
	pcCmdData.uParam.TEST_SEND_CHANNEL_BAND.Length = 1 + (pDlg->m_TestRid).GetLength() +(pDlg->m_TestTid).GetLength();
	memcpy(pcCmdData.uParam.TEST_SEND_CHANNEL_BAND.RID, pDlg->m_TestRid, (pDlg->m_TestRid).GetLength());
	memcpy(pcCmdData.uParam.TEST_SEND_CHANNEL_BAND.TID, pDlg->m_TestTid, (pDlg->m_TestTid).GetLength());

	if(TestSendCmd(&pcCmdData) != 0)
	{
		pDlg->m_InfoStatus = "发送发射占用信道带宽命令失败";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
		return;
	}

	else
	{
		pDlg->m_InfoStatus = "发送发射占用信道带宽命令成功";
		pDlg->SendMessage(RENEWINFOSTATUS, NULL, NULL);
	}
}