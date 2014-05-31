
// SerialDlg.h : 头文件

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

#define MY_MESSAGE (WM_USER + 1)  //自定义的消息的宏by ppwu
#define UPDATETRUE (WM_USER + 2)

// CSerialDlg 对话框
class CSerialDlg : public CDialogEx
{
// 构造
public:
	CSerialDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERIAL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	CString m_staticFileMaxLen;// 表明当前创建文件的属性域,最大允许长度
	CString m_staticFileRecords;// 创建文件参数,最大允许记录数指示
	CString m_staticFileRealLen;// 创建文件第三个参数,实际长度
	CString m_writeFile;
	CString m_FileID;
	CListCtrl m_listCtrl;
	bool ComStateClose;// 打开&关闭键控制变量
	int m_FileOffset;// 偏移量写文件
	CComboBox m_cmbFileType;// 文件类型选择框
	CComboBox m_cmbValidState;// 有效无效状态选择框
	CComboBox m_cmbAthState;
	int m_validCmdType;
	int m_writeCmdType;
	int m_pwdRdRearch;// 创建文件中读取和查询密码
	int m_pwdUpdate;// 创建文件中更新密码
	int m_pwdValid;// 创建文件中有效无效密码
	CListCtrl m_listCtrlGoods;//显示货物变量list Control
	CString m_SecurityAttrB2_3;// 安全控制属性第2\3位
	CString m_SecurityAttrB4_5;// 安全控制属性第4\5位
	CComboBox m_cmbUpdata;
	CComboBox m_cmbB2_3;
	CString m_strRXData;
	bool m_bMF;// 当前目录是否为主目录
	PLIST_FILE_ATTR m_pSelectedFileAttr;//保存当前选择文件
	PLIST_FILE_ATTR m_pTmpSelectedFileAttr;
	PLIST_FILE_ATTR m_pFileList;// 文件属性链表
	PTAG_ATTR m_pTagList;//标签属性链表
	PGOODS_INFO m_pGoodsList;//货物信息链表
	char* m_pGoodsBuff;// 暂存货物信息
	PC_CMD_REPLY m_pcCmdReply[588];
	int	m_ReplyHead;
	HANDLE m_hReply;
	bool m_dataInBuf;
	CString m_pwd;
	CEdit m_strRX;
	CListCtrl m_listCtrlFile;
	TIME_OF_READ_LARGE_FILE m_timeOfReadLargeFile;// 读大文件的时间
	char* pszBigFile;
	char* m_curptr;
	CRect m_rect;// 初始对话框大小
	CRect m_originalRectSize;
	int m_NumOfReadLargeFile;// 统计读大文件的次数
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
	CXPButton m_xpbtn_open_sensor;// 打开传感器
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
	CXPButton m_openSocket;// 打开通信端口
	CXPButton m_closeSocket;// 关闭通信端口
	CXPButton m_wirelessSetKey;// 空中烧写密钥
	CXPButton m_btn_register_tag;// 注册标签
	CXPButton m_ReaderTestButton;
	CXPButton m_CmdTestButton;
	CXPButton m_btn_safe_session;// 安全会话
	CXPButton m_btn_kill_tag;
	CXPButton m_btn_getTagInfo;// 获取标签信息按键
	CXPButton m_btn_cleanAllTag;// 清除所有标签信息
	CXPButton m_btn_wakeupOneTag;// 唤醒单个标签按键
	CXPButton m_btn_file_delete;
	CXPButton m_btn_fileSys_format;
	CXPButton m_btn_impoTag_pubKey;
	CXPButton m_writeLargeFile;
	CXPButton m_SerachRecordButton;
	CXPButton m_btn_updatePwd;// 更新系统口令

	CString m_TID;// 注册标签的TID
	CToolBar m_Toolbar;// 工具栏变量
	CImageList m_ImageList;
	CString m_toolStr;
	Image* m_pImage;
	CString m_strOpenFileName;
	CBrush m_brush;
	CString m_MIK;
	CString m_MEK;
	unsigned char m_pubKeyTag[69];
	CString m_filePathName;
	bool m_isSelected;// 标志位，判断删除文件前是否选择文件
	CReaderManageDlg ReaderManageDlg;//构造对话框类CReaderManageDlg的实例
	CRegisterTagDlg RegisterTagDlg;	//构造对话框类CRegisterTagDlg的实例			
	HANDLE hComEvent;// 串口独占事件
	bool m_comReciveType;
	DWORD m_adminPwd;// 管理员密码
	// 命令索引
	DWORD m_newPwd;// 新密码
	CString m_SearchCondition;
	unsigned char m_recPoint;// 保存读记录文件的每一个偏移量（通过++）
	// 在选中一个目录文件(子目录)中的一个其它文件后,再次列表文件时,需要再一次的验证当前的选中文件是否为此子目录.否则目录在有列表口令时会产生操作对象不是当前目录,而是当前选择的文件的错误
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
	// 插入链表（存文件）
	void InsertFileList(PLIST_FILE_ATTR);
	//插入链表（存标签）
	void InsertTagList(PTAG_ATTR pTagAttr);
	//插入链表(存货物信息)
	void InsertGoodsList(PGOODS_INFO pGoodsInfo);
	// 返回文件属性
	PLIST_FILE_ATTR GetFileAttrByIndex(unsigned int nIndex);
	PLIST_FILE_ATTR GetFileAttrByFileID(unsigned int FileID);
	BOOL DeleteFileByFileID(unsigned int FileID);
	unsigned short m_CurrentSelectFileID;
	// 返回标签属性
	PTAG_ATTR GetTagAttr(unsigned char nIndex);
	// 返回货物信息
	PGOODS_INFO GetGoodsInfo(unsigned char nIndex);
	// 清空文件属性链表
	void ClearFileAttrList(void);
	// 删除某条文件属性链表
	void DelFileAttrList(void);
	// 清空标签属性链表
	void ClearTagList(void);
	// 清空货物信息链表
	void ClearGoodsList(void);
	// 当前读文件状态
	CURRENT_READ_FILE_STATE m_curReadState;
	// 当前验证密码状态
	CURRENT_VERIFY_PWD_STATE m_curVerifyPwdState;
	// 查找文件属性通过 文件ID
	PLIST_FILE_ATTR GetFileAttrByID(unsigned short fileID);
	// 写文件
	void WriteFile(int FileOffset, unsigned short WriteLength, char* pData,int FileType);
	//识别/r/n
	char* FindRecord(char* p);
	//显示货物
	void DisplayGoodsItem(CListCtrl &lstCtrl,int index,char* content);
	void GetSecurityTypeStr(char FileType,char Security,CString& OutputStr );
	void DisplayAllFilelistItems(CListCtrl& lstctrl,PLIST_FILE_ATTR pFilelistAttribute);
	void DisplayOneFilelistItems(CListCtrl& lstctrl,PLIST_FILE_ATTR pFilelistAttribute);
	int GetFileListItemsNum(void);
	void ReadRecordFileItems(unsigned char ReadMode);
	void ReadLargeFile(void);
	bool enum_reg_key_value(HKEY hKey, LPCTSTR lpSubKey);
	// 为工具栏按钮添加鼠标停留提示信息
	BOOL OnDisplay(unsigned int id, NMHDR* pNMHDR, LRESULT* pResult);
	// 弹出注册标签窗口
	void popRegisterTag(void);
	// 弹出阅读器管理
	void popReaderManager(void);
	static void initCmdData(PC_CMD_DATA& pcCmdData,PC_CMD_TYPE cmdType);
	int send_cmd(void* pData, unsigned int Len);
	void DispatchCMD(PC_CMD_REPLY* pkg);
	// 同步收数据帧函数
	int GetCmdReplyFrameData(void* pData, int bufLen);
	int GetCmdReplyDataSync(void* pData, int bufLen);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int StrToHex(CString str, char *data);
	char HexChar(char c);


	/*------------------------------消息事件线程函数------------------------------*/
	static void BeginMsgEventThread(LPVOID lparam);	//开启处理事件辅助工作线程
	static void CollectTagThreadFunc(LPVOID lparam); //盘存工作线程
	static void SelectTagThreadFunc(LPVOID lparam); //选卡工作线程
	static void ListFileThreadFunc(LPVOID lparam); //列表文件
	static void WriteFileThreadFunc(LPVOID lparam); //写文件工作线程
	static void ReadFileThreadFunc(LPVOID lparam); //读文件线程函数
	static void SearchRecordThreadFunc(LPVOID lparam); //检索记录工作线程
	static void SocketConnectThreadFunc(LPVOID lparam);
	static void WakeUpThreadFunc(LPVOID lparam); //唤醒工作线程
	static void KillTagThreadFunc(LPVOID lparam); //标签灭活工作线程
	CString m_PwdIndex;
	int m_WakeupNum;
	CXPButton mWakeUpButton;
	afx_msg void OnBnClickedWakeup();
	BYTE m_CollectNum;
};
