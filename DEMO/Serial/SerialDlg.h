
// SerialDlg.h : ͷ�ļ�

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "PcCommand.h"
#include "dataType.h"
#include "atltypes.h"
#include "XPButton.h"
#include "afxext.h"
#include "ReaderManageDlg.h"
#include "RegisterTagDlg.h"

#define MY_MESSAGE (WM_USER + 1)  //�Զ������Ϣ�ĺ�by ppwu
#define UPDATETRUE (WM_USER + 2)

// CSerialDlg �Ի���
class CSerialDlg : public CDialogEx
{
// ����
public:
	CSerialDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SERIAL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//{{AFX_MSG(CCreateArcBy2Point)
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatetrue(WPARAM wParam, LPARAM lParam);

public:
	CString m_IP;
	CString m_IPMask;
	int m_IPPort;
	CString m_statusbar;
	unsigned int m_maxRecordNum;
	unsigned int m_fileRealLen;
	unsigned int m_maxFileLen;
	CString m_recFileContent;
	unsigned char m_countTid[8];
	CString m_staticFileMaxLen;// ������ǰ�����ļ���������,���������
	CString m_staticFileRecords;// �����ļ�����,��������¼��ָʾ
	CString m_staticFileRealLen;// �����ļ�����������,ʵ�ʳ���
	CString m_writeFile;
	CString m_FileID;
	CListCtrl m_listCtrl;
	bool ComStateClose;// ��&�رռ����Ʊ���
	int m_FileOffset;// ƫ����д�ļ�
	CComboBox m_cmbFileType;// �ļ�����ѡ���
	CComboBox m_cmbValidState;// ��Ч��Ч״̬ѡ���
	CComboBox m_cmbAthState;
	int m_validCmdType;
	int m_writeCmdType;
	int m_pwdRdRearch;// �����ļ��ж�ȡ�Ͳ�ѯ����
	int m_pwdUpdate;// �����ļ��и�������
	int m_pwdValid;// �����ļ�����Ч��Ч����
	CListCtrl m_listCtrlGoods;//��ʾ�������list Control
	CString m_SecurityAttrB2_3;// ��ȫ�������Ե�2\3λ
	CString m_SecurityAttrB4_5;// ��ȫ�������Ե�4\5λ
	CComboBox m_cmbUpdata;
	CComboBox m_cmbB2_3;
	CString m_strRXData;
	bool m_bMF;// ��ǰĿ¼�Ƿ�Ϊ��Ŀ¼
	PLIST_FILE_ATTR m_pSelectedFileAttr;//���浱ǰѡ���ļ�
	PLIST_FILE_ATTR m_pTmpSelectedFileAttr;
	PLIST_FILE_ATTR m_pFileList;// �ļ���������
	PTAG_ATTR m_pTagList;//��ǩ��������
	PGOODS_INFO m_pGoodsList;//������Ϣ����
	char* m_pGoodsBuff;// �ݴ������Ϣ
	PC_CMD_REPLY m_pcCmdReply[588];
	int	m_ReplyHead;
	HANDLE m_hReply;
	bool m_dataInBuf;
	CString m_pwd;
	CEdit m_strRX;
	CListCtrl m_listCtrlFile;
	TIME_OF_READ_LARGE_FILE m_timeOfReadLargeFile;// �����ļ���ʱ��
	char* pszBigFile;
	char* m_curptr;
	CRect m_rect;// ��ʼ�Ի����С
	CRect m_originalRectSize;
	int m_NumOfReadLargeFile;// ͳ�ƶ����ļ��Ĵ���
	int m_curNumOfTag;
	bool m_bRetToMf;
	LIST_FILE_ATTR m_curMFDF;
	PTAG_ATTR m_pSelectedTag;
	PTAG_ATTR m_ptmpSelectTag;
	int m_comboCom;
	CComboBox m_Port;
	CComboBox m_Baud;
	CComboBox m_Check;
	CComboBox m_Data;
	CComboBox m_Stop;
	CXPButton m_xpbtn_open_sensor;// �򿪴�����
	CXPButton m_btn_sleep;
	CXPButton m_btn_select;
	CXPButton m_btn_inventory;
	CXPButton m_btn_open_com;
	CXPButton m_exit_soft;
	CXPButton m_btn_zero_offset_write_file;
	CXPButton m_btn_any_offset_write;
	CXPButton m_btn_create_file;
	CXPButton m_btn_read_big_file;
	CXPButton m_btn_ret_main_def;
	CXPButton m_btn_invaild_file;
	CXPButton m_btn_read_file;
	CXPButton m_btn_select_file;
	CXPButton m_btn_list_file;
	CXPButton m_btn_valid_file;
	CXPButton m_openSocket;// ��ͨ�Ŷ˿�
	CXPButton m_closeSocket;// �ر�ͨ�Ŷ˿�
	CXPButton m_wirelessSetKey;// ������д��Կ
	CXPButton m_btn_register_tag;// ע���ǩ
	CXPButton m_ReaderTestButton;
	CXPButton m_CmdTestButton;
	CXPButton m_btn_safe_session;// ��ȫ�Ự
	CXPButton m_btn_kill_tag;
	CXPButton m_btn_getTagInfo;// ��ȡ��ǩ��Ϣ����
	CXPButton m_btn_cleanAllTag;// ������б�ǩ��Ϣ
	CXPButton m_btn_wakeupOneTag;// ���ѵ�����ǩ����
	CXPButton m_btn_file_delete;
	CXPButton m_btn_fileSys_format;
	CXPButton m_btn_impoTag_pubKey;
	CXPButton m_writeLargeFile;
	CXPButton m_SerachRecordButton;
	CXPButton m_btn_updatePwd;// ����ϵͳ����

	CString m_TID;// ע���ǩ��TID
	CToolBar m_Toolbar;// ����������
	CImageList m_ImageList;
	CString m_toolStr;
	Image* m_pImage;
	CString m_strOpenFileName;
	CBrush m_brush;
	CString m_MIK;
	CString m_MEK;
	unsigned char m_pubKeyTag[69];
	CString m_filePathName;
	bool m_isSelected;// ��־λ���ж�ɾ���ļ�ǰ�Ƿ�ѡ���ļ�
	CReaderManageDlg ReaderManageDlg;//����Ի�����CReaderManageDlg��ʵ��
	CRegisterTagDlg RegisterTagDlg;	//����Ի�����CRegisterTagDlg��ʵ��			
	HANDLE hComEvent;// ���ڶ�ռ�¼�
	bool m_comReciveType;
	DWORD m_adminPwd;// ����Ա����
	// ��������
	DWORD m_newPwd;// ������
	CString m_SearchCondition;
	unsigned char m_recPoint;// �������¼�ļ���ÿһ��ƫ������ͨ��++��
	// ��ѡ��һ��Ŀ¼�ļ�(��Ŀ¼)�е�һ�������ļ���,�ٴ��б��ļ�ʱ,��Ҫ��һ�ε���֤��ǰ��ѡ���ļ��Ƿ�Ϊ����Ŀ¼.����Ŀ¼�����б����ʱ��������������ǵ�ǰĿ¼,���ǵ�ǰѡ����ļ��Ĵ���
	bool m_bListFileAfterSelect;
	bool m_OpenCommunication;//by ppwu 10.21
	bool m_SwitchSignal;
	CComboBox m_CommuFlag;


	afx_msg void OnComm();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnCbnSelchangeCombo6();
	afx_msg void OnCbnSelendokCombo6();
	afx_msg void OnCbnDblclkCombo6();
	afx_msg void OnCbnCloseupCombo6();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSetfocusCombo1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton16();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRegisterTag();
	afx_msg void OnBnClickedBtnGettaginfo();
	afx_msg void OnBnClickedBtnCleanalltag();
	afx_msg void OnBnClickedBtnWakeuponetag();
	afx_msg void OnBnClickedBtnFileDelete();
	afx_msg void OnBnClickedBtnFilesysFormat();
	afx_msg void OnBnClickedBtnKillTag();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedBtnUpdatePwd();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClickedOpensocket();
	afx_msg void OnBnClickedClosesocket();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedReadertest();
	afx_msg void OnBnClickedCmdtest();
	afx_msg void OnBnClickedBtnSafeSession();
	afx_msg void OnBnClickedSearchrecord();

	DECLARE_EVENTSINK_MAP()

public:
	void AddToList_Serial(LPCTSTR FileName, LPCTSTR LastVisit);
	unsigned int receive();
	// �����������ļ���
	void InsertFileList(PLIST_FILE_ATTR);
	//�����������ǩ��
	void InsertTagList(PTAG_ATTR pTagAttr);
	//��������(�������Ϣ)
	void InsertGoodsList(PGOODS_INFO pGoodsInfo);
	// �����ļ�����
	PLIST_FILE_ATTR GetFileAttrByIndex(unsigned int nIndex);
	PLIST_FILE_ATTR GetFileAttrByFileID(unsigned int FileID);
	BOOL DeleteFileByFileID(unsigned int FileID);
	unsigned short m_CurrentSelectFileID;
	// ���ر�ǩ����
	PTAG_ATTR GetTagAttr(unsigned char nIndex);
	// ���ػ�����Ϣ
	PGOODS_INFO GetGoodsInfo(unsigned char nIndex);
	// ����ļ���������
	void ClearFileAttrList(void);
	// ɾ��ĳ���ļ���������
	void DelFileAttrList(void);
	// ��ձ�ǩ��������
	void ClearTagList(void);
	// ��ջ�����Ϣ����
	void ClearGoodsList(void);
	// ��ǰ���ļ�״̬
	CURRENT_READ_FILE_STATE m_curReadState;
	// ��ǰ��֤����״̬
	CURRENT_VERIFY_PWD_STATE m_curVerifyPwdState;
	// �����ļ�����ͨ�� �ļ�ID
	PLIST_FILE_ATTR GetFileAttrByID(unsigned short fileID);
	// д�ļ�
	void WriteFile(int FileOffset, unsigned short WriteLength, char* pData,int FileType);
	//ʶ��/r/n
	char* FindRecord(char* p);
	//��ʾ����
	void DisplayGoodsItem(CListCtrl &lstCtrl,int index,char* content);
	void GetSecurityTypeStr(char FileType,char Security,CString& OutputStr );
	void DisplayAllFilelistItems(CListCtrl& lstctrl,PLIST_FILE_ATTR pFilelistAttribute);
	void DisplayOneFilelistItems(CListCtrl& lstctrl,PLIST_FILE_ATTR pFilelistAttribute);
	int GetFileListItemsNum(void);
	void ReadRecordFileItems(unsigned char ReadMode);
	void ReadLargeFile(void);
	bool enum_reg_key_value(HKEY hKey, LPCTSTR lpSubKey);
	// Ϊ��������ť������ͣ����ʾ��Ϣ
	BOOL OnDisplay(unsigned int id, NMHDR* pNMHDR, LRESULT* pResult);
	// ����ע���ǩ����
	void popRegisterTag(void);
	// �����Ķ�������
	void popReaderManager(void);
	static void initCmdData(PC_CMD_DATA& pcCmdData,PC_CMD_TYPE cmdType);
	int send_cmd(void* pData, unsigned int Len);
	void DispatchCMD(PC_CMD_REPLY* pkg);
	// ͬ��������֡����
	int GetCmdReplyFrameData(void* pData, int bufLen);
	int GetCmdReplyDataSync(void* pData, int bufLen);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int StrToHex(CString str, char *data);
	char HexChar(char c);


	/*------------------------------��Ϣ�¼��̺߳���------------------------------*/
	static void BeginMsgEventThread(LPVOID lparam);	//���������¼����������߳�
	static void CollectTagThreadFunc(LPVOID lparam); //�̴湤���߳�
	static void SelectTagThreadFunc(LPVOID lparam); //ѡ�������߳�
	static void ListFileThreadFunc(LPVOID lparam); //�б��ļ�
	static void WriteFileThreadFunc(LPVOID lparam); //д�ļ������߳�
	static void ReadFileThreadFunc(LPVOID lparam); //���ļ��̺߳���
	static void SearchRecordThreadFunc(LPVOID lparam); //������¼�����߳�
	static void SocketConnectThreadFunc(LPVOID lparam);
	static void WakeUpThreadFunc(LPVOID lparam); //���ѹ����߳�
	static void KillTagThreadFunc(LPVOID lparam); //��ǩ�����߳�
	CString m_PwdIndex;
	int m_WakeupNum;
	CXPButton mWakeUpButton;
	afx_msg void OnBnClickedWakeup();
	BYTE m_CollectNum;
};
