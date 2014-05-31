#pragma once
#include "pcCommand.h"
#include "xpbutton.h"

#define RENEWINFOSTATUS (WM_USER + 30)
// CReaderTestDlg 对话框

class CReaderTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReaderTestDlg)

public:
	CReaderTestDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReaderTestDlg();

// 对话框数据
	enum { IDD = IDD_READERTEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnRenewinfostatus(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	CString m_InfoStatus;
	UINT m_ReadyToken;
	UINT m_CycleNum;
	int m_RequestNum;
	CString m_TestRid;
	CString m_TestTid;

	CXPButton m_FreqAccuracy;
	CXPButton m_MinRecvBand;
	CXPButton m_SwitchTime;
	CXPButton m_ModulateMode;
	CXPButton m_BitSpeed;
	CXPButton m_BeforeCode;
	CXPButton m_TagCollionManage;
	CXPButton m_FsssSequence;
	CXPButton m_FreqRange;
	CXPButton m_ChannelBand;
	CXPButton m_MaxSendPower;
	CXPButton m_ZasanRadio;
	CXPButton m_DaiwaiRadioPower;
	CXPButton m_FreqAccry;
	CXPButton m_RecvSensitivity;
	CXPButton m_MoveIdentiSpeed;

	afx_msg void OnBnClickedFreqAccuracy();
	afx_msg void OnBnClickedMinrecvband();
	afx_msg void OnBnClickedSwitchtime();
	afx_msg void OnBnClickedModulatemode();
	afx_msg void OnBnClickedBitspeed();
	afx_msg void OnBnClickedBeforecode();
	afx_msg void OnBnClickedTagcollionmanage();
	afx_msg void OnBnClickedFssssequence();
	afx_msg void OnBnClickedFreqrange();
	afx_msg void OnBnClickedChannelband();
	afx_msg void OnBnClickedMaxsendpower();
	afx_msg void OnBnClickedZasanradio();
	afx_msg void OnBnClickedDaiwairadiopower();
	afx_msg void OnBnClickedFreqaccry();
	afx_msg void OnBnClickedMoveidentispeed();
	afx_msg void OnBnClickedRecvsensitivity();

	static void InitTestCmd(PC_CMD_DATA& pcCmdData, PC_CMD_TYPE cmdType);
	/*---------------------------工作线程-----------------------*/
	static void FreqAccuracyThreadFunc(LPVOID lparam);
	static void MinRecvBandThreadFunc(LPVOID lparam);
	static void SwitchTimeThreadFunc(LPVOID lparam);
	static void ModulateModeThreadFunc(LPVOID lparam);
	static void BitSpeedThreadFunc(LPVOID lparam);
	static void BeforeCodeThreadFunc(LPVOID lparam);
	static void TagCollionManageThreadFunc(LPVOID lparam);
	static void FsssSequenceThreadFunc(LPVOID lparam);
	static void FreqRangeThreadFunc(LPVOID lparam); //工作频率范围工作线程
	static void ChannelBandThreadFunc(LPVOID lparam);
	static void MaxSendPowerThreadFunc(LPVOID lparam);
	static void ZasanRadioThreadFunc(LPVOID lparam);
	static void DaiwanRadioPowerThreadFunc(LPVOID lparam);
	static void FreqAccryThreadFunc(LPVOID lparam);
	static void MoveIdentiSpeedThreadFunc(LPVOID lparam);
	static void RecvSensitivityThreadFunc(LPVOID lparam);
	static void TagFreqAccuracyThreadFunc(LPVOID lparam);
	static void TagEqualPowerThreadFunc(LPVOID lparam);
	static void TagSendBandThreadFunc(LPVOID lparam);


	CXPButton m_TagWorkFreq;
	CXPButton m_EqualPower;
	CXPButton m_TagSendBand;
	afx_msg void OnBnClickedTagworkfreq();
	afx_msg void OnBnClickedEqualpower();
	afx_msg void OnBnClickedTagsendband();
	CString m_Path;
};
