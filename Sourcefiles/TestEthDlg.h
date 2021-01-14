// TestEthDlg.h : header file
//

#if !defined(AFX_TESTETHDLG_H__9A0D1B05_3B29_407C_978E_67449CB195E2__INCLUDED_)
#define AFX_TESTETHDLG_H__9A0D1B05_3B29_407C_978E_67449CB195E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <pcap.h>
#include "packet32.h"
#include "IoBuf.h"
#include <ntddndis.h>
#include "Duration.h"

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
void packet_Process(u_char *param);
#define Max_Num_Adapter 10
#define Max_Pkt_Size 1500

#include "GetMacAddrDlg.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CTestEthDlg dialog

class CTestEthDlg : public CDialog
{
// Construction
public:
	CTestEthDlg(CWnd* pParent = NULL);	// standard constructor

	int m_nBufferSize;
	unsigned long long int  m_nByteCntr;
	char *m_pchMem;
	unsigned long long m_wr_cnt;

	char m_chTemp[1024];
	unsigned char m_tempPacket[Max_Pkt_Size]; //Maximum Size

	pcap_if_t *m_alldevs;
	pcap_if_t *m_d;
	pcap_t *m_adhandle;
	struct pcap_pkthdr *header;
	char m_chDevicesList[Max_Num_Adapter][1024];
	pcap_dumper_t *m_dumpfile;
	const u_char *pkt_data;
	char m_errbuf[PCAP_ERRBUF_SIZE];
	CString m_cstrSendFileName;
	CString m_cstrDumpFileName;
	UINT m_uiItemCntr;
	BOOL m_bInputMessage;
	BOOL m_bDisableList;

	HANDLE m_hReceiveThread;
	static DWORD WINAPI ReceiveThreadFunc(void * pParam);

	HANDLE m_hProcessThread;
	static DWORD WINAPI ReceiveProcessFunc(void * pParam);

	HANDLE m_hSendFileThread;
	static DWORD WINAPI SendFileThreadFunc(void * pParam);

	FILE *m_pFileWriteTotal;
	FILE *m_pFileWriteNet;
	FILE *m_pFileRead;
	DWORD m_dwSendFileLength;

	DWORD GetMacAddress();
	BOOL m_bIsDevActive;
	unsigned char m_uchSrcMacAddress[6];
	unsigned char m_uchDstMacAddress[6];
	char AdapterList[Max_Num_Adapter][1024];

	void GetDestMacAddresses();
	void AddDestMacAddress(CString Addr);
	int m_iNumOfAddrs;
	char m_chDestAddrList[Max_Num_Adapter][6];
	u_short char2hex(char ch);
	CGetMacAddrDlg m_dlgGetMac;

	CDuration m_ocTimer;
	CDuration m_ocTimerRx;
	BOOL m_bStartRx;
	BOOL m_bRatePrint;
	CIoBufPool m_ocChBuff;	// Buffer Pool Object
	int	m_nMissProcCount;		// Missed Packets Counter
	int m_iNumRecBuffs;			// Recorded Packets Counter
	BOOL m_bRun;			// Run Indicator Flag
	DWORD  m_dwPrvSample;


// Dialog Data
	//{{AFX_DATA(CTestEthDlg)
	enum { IDD = IDD_TESTETH_DIALOG };
	CProgressCtrl	m_ctrlProgressSendFile;
	CComboBox	m_ctrlComboMacDestAddr;
	CListCtrl	m_ctrlListRcvdMsg;
	CComboBox	m_ctrlComboDevices;
	int	m_iComboDevices;
	CString	m_cstrMessage;
	UINT	m_uiPacketSize;
	int	m_iComboMacDestAddr;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestEthDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestEthDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSend();
	afx_msg void OnButtonReceive();
	afx_msg void OnButtonSearch();
	afx_msg void OnSelchangeComboDevicenames();
	afx_msg void OnCheckResumethread();
	afx_msg void OnButtonOpenFile();
	afx_msg void OnDestroy();
	afx_msg void OnButtonAbout();
	afx_msg void OnCheckInputmethod();
	afx_msg void OnSelchangeComboDstMacAddr();
	afx_msg void OnCheckDisableList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ctrlerr;
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTETHDLG_H__9A0D1B05_3B29_407C_978E_67449CB195E2__INCLUDED_)
