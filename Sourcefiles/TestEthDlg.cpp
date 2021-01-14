// TestEthDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestEth.h"
#include "TestEthDlg.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define LINE_LEN 16
#include "Packet32.h"
#include <NtDDNdis.h>
//---------------------------------------------------------------------------------- 
//--------------------------------- My Global Codes --------------------------------
//---------------------------------------------------------------------------------- 
CTestEthDlg* pDlg = NULL;
//----------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestEthDlg dialog

CTestEthDlg::CTestEthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestEthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestEthDlg)
	m_iComboDevices = 0;
	m_cstrMessage = _T("");
	m_uiPacketSize = 0;
	m_iComboMacDestAddr = 0;
	m_bIsDevActive = FALSE;
	m_bRatePrint = TRUE;
	m_wr_cnt = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pFileWriteTotal = NULL;
	m_pFileWriteNet = NULL;
	m_pFileRead = NULL;

	m_dwSendFileLength = 0;
}

void CTestEthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestEthDlg)
	DDX_Control(pDX, IDC_PROGRESS_SENDFILE, m_ctrlProgressSendFile);
	DDX_Control(pDX, IDC_COMBO_DSTMACADDR, m_ctrlComboMacDestAddr);
	DDX_Control(pDX, IDC_LIST_RCVDMSG, m_ctrlListRcvdMsg);
	DDX_Control(pDX, IDC_COMBO_DEVICENAMES, m_ctrlComboDevices);
	DDX_CBIndex(pDX, IDC_COMBO_DEVICENAMES, m_iComboDevices);
	DDX_Text(pDX, IDC_EDIT_MESSAGE, m_cstrMessage);
	DDX_Text(pDX, IDC_EDIT_PKTSIZE, m_uiPacketSize);
	DDV_MinMaxUInt(pDX, m_uiPacketSize, 10, 1500);
	DDX_CBIndex(pDX, IDC_COMBO_DSTMACADDR, m_iComboMacDestAddr);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST1, m_ctrlerr);
}

BEGIN_MESSAGE_MAP(CTestEthDlg, CDialog)
	//{{AFX_MSG_MAP(CTestEthDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_RECEIVE, OnButtonReceive)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICENAMES, OnSelchangeComboDevicenames)
	ON_BN_CLICKED(IDC_CHECK_RESUMETHREAD, OnCheckResumethread)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpenFile)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, OnButtonAbout)
	ON_BN_CLICKED(IDC_CHECK_INPUTMETHOD, OnCheckInputmethod)
	ON_CBN_SELCHANGE(IDC_COMBO_DSTMACADDR, OnSelchangeComboDstMacAddr)
	ON_BN_CLICKED(IDC_CHECK_DISABLELIST, OnCheckDisableList)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTestEthDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestEthDlg message handlers

BOOL CTestEthDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//---------------------------------------------------------------------------------- 
	//----------------------------- My Initialization Codes ----------------------------
	//---------------------------------------------------------------------------------- 
	//------------- Allocate Memory ---------------
	m_nBufferSize = (int)1.5*1024*1024*1024;
	m_nByteCntr = 0;
	m_pchMem = NULL;
	m_bRun = TRUE;
	m_iNumRecBuffs = 0;
	m_nMissProcCount = 0;
	m_ocChBuff.Allocate( 0x580, 120000 );
	//m_pchMem = new char[m_nBufferSize];
	//if ( m_pchMem == NULL ) 
	//
	//	// failed
	//	delete [] m_pchMem;
	//	m_pchMem = NULL;
	//	MessageBox("OS could not allocate memory!", "Mem Alloc Error", MB_OK|MB_ICONERROR);
	//} 
	//---------------------------------------------
	m_adhandle = NULL;
	m_hSendFileThread = NULL;
	m_hSendFileThread = NULL;
	m_d = NULL;
 	m_alldevs = NULL;
 	m_pFileWriteNet = NULL;
 	m_pFileWriteTotal = NULL;
 	m_pFileRead = NULL;

	pDlg = this;
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_RECEIVE)->EnableWindow(FALSE);
	
	CFont font;
	font.CreateFont(0,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Times");
	GetDlgItem(IDC_EDIT_MESSAGE)->SetFont(&font);

	m_uiItemCntr = 0;
	m_ctrlListRcvdMsg.SetExtendedStyle(m_ctrlListRcvdMsg.GetExtendedStyle()  | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlListRcvdMsg.InsertColumn(0,"Received Data", LVCFMT_LEFT, 1000, 100);
	m_ctrlListRcvdMsg.InsertColumn(0,"Length",LVCFMT_LEFT, 50, 100);
	m_ctrlListRcvdMsg.InsertColumn(0,"Dst. Addr.",LVCFMT_LEFT, 110, 100);
	m_ctrlListRcvdMsg.InsertColumn(0,"Src. Addr.",LVCFMT_LEFT, 110, 100);
	m_ctrlListRcvdMsg.InsertColumn(0,"ID",LVCFMT_LEFT, 30, 100);
	

	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(TRUE);
	m_bInputMessage = FALSE;
	m_bDisableList = FALSE;

	m_uiPacketSize = 100;
	UpdateData(FALSE);

	GetDestMacAddresses();
	m_ctrlComboMacDestAddr.SetCurSel(0);

	// Set progress bar
	m_ctrlProgressSendFile.SetRange(0,100);
   SetDlgItemText(IDC_STATIC_TXRATE, "0.0");

   m_bStartRx = TRUE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestEthDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestEthDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestEthDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//---------------------------------------------------------------------------------- 
//---------------------------------- OnButtonAbout ---------------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnButtonAbout() 
{
	// TODO: Add your control notification handler code here
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

//---------------------------------------------------------------------------------- 
//------------------------------------ char2hex ------------------------------------
//---------------------------------------------------------------------------------- 
u_short CTestEthDlg::char2hex(char ch)
{
	if ((ch >= 48) && (ch <= 57))
		return ch-48;
	else if  ((ch >= 97) && (ch <= 102))
		return ch-97+10;
	else if  ((ch >= 65) && (ch <= 70))
		return ch-65+10;
	else
		return 0;
}

//---------------------------------------------------------------------------------- 
//--------------------------------- OnButtonSearch ---------------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnButtonSearch() 
{
	// TODO: Add your control notification handler code here
	int i = 0;	

	m_ctrlComboDevices.ResetContent();

	/* Retrieve the device list */
	if(pcap_findalldevs(&m_alldevs, m_errbuf) == -1)
	{
		sprintf(m_chTemp,"Error in pcap_findalldevs: %s\n", m_errbuf);
		MessageBox(m_chTemp);
	}
	
	/* Print the list */
	for(m_d=m_alldevs; m_d; m_d=m_d->next)
	{
		++i;
		sprintf(m_chTemp, "%d. %s", i, m_d->name);
		memset(m_chDevicesList[i-1], 0, 1024);
		sprintf(m_chDevicesList[i-1], "%s", m_d->name);

		if (m_d->description)
		{
			sprintf(m_chTemp, "%d. %s", i, m_d->description);
			m_ctrlComboDevices.AddString(m_chTemp);
		}
		else
		{
			sprintf(m_chTemp, "%d. Unknown", i);
			m_ctrlComboDevices.AddString(m_chTemp);
		}
	}

	if (m_ctrlComboDevices.GetCount() > 0)
	{
		m_ctrlComboDevices.SetCurSel(0);
		
		OnSelchangeComboDevicenames() ;

		UpdateData(TRUE);
		GetMacAddress();
	}

	if( i == 0 )
		MessageBox("No interfaces found! Make sure WinPcap is installed.", "Warning", MB_ICONWARNING|MB_OK);

	if ( !m_bInputMessage )
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_BUTTON_RECEIVE)->EnableWindow(TRUE);
}

//---------------------------------------------------------------------------------- 
//--------------------------- OnSelchangeComboDevicenames --------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnSelchangeComboDevicenames() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	//Map Mac Address to device name
	GetMacAddress();
	
	//Open the selected device
	int i;

	// Jump to the selected adapter
	for(m_d=m_alldevs, i=0; i < m_iComboDevices ;m_d=m_d->next, i++);
	
	// Open the adapter
	if ((m_adhandle = pcap_open_live(m_d->name,	// name of the device
									65536,		// portion of the packet to capture. 
												// 65536 grants that the whole packet will be captured on all the MACs.
									1,			// promiscuous mode (nonzero means promiscuous)
									1000,		// read timeout
									m_errbuf	// error buffer
							 )) == NULL)
	{
		sprintf(m_chTemp,"\nUnable to open the adapter. %s is not supported by WinPcap\n", m_d->name);
		MessageBox(m_chTemp);

		// Free the device list
		pcap_freealldevs(m_alldevs);
		return;
	}

	UpdateData(FALSE);
}

//---------------------------------------------------------------------------------- 
//------------------------------ GetMacAddress Function ----------------------------
//---------------------------------------------------------------------------------- 
DWORD CTestEthDlg::GetMacAddress()
{
	LPADAPTER	lpAdapter = 0;
	int			i;
	DWORD		dwErrorCode;
	char		AdapterName[8192];
	char		*temp,*temp1;
	int			AdapterNum=0,Open;
	ULONG		AdapterLength;
	PPACKET_OID_DATA  OidData;
	BOOLEAN		Status;
	
	// Obtain the name of the adapters installed on this machine
	m_bIsDevActive = FALSE;
	i = 0;	

	AdapterLength = sizeof(AdapterName);
	
	if( PacketGetAdapterNames(AdapterName, &AdapterLength) == FALSE )
	{
		MessageBox("Unable to retrieve the list of the adapters!", "Warning", MB_ICONWARNING|MB_OK);
		return -1;
	}

	temp  = AdapterName;
	temp1 = AdapterName;

	while ((*temp != '\0') || (*(temp-1) != '\0'))
	{
		if (*temp == '\0') 
		{
			memset(AdapterList[i], 0, 1024);
			sprintf(AdapterList[i], "%s", temp1);
			//memcpy(AdapterList[i], temp1, temp-temp1);
			temp1 = temp + 1;
			i++;
		}
		temp++;
	}
		  
	AdapterNum = i;

	for (i = 0; i < AdapterNum; i++)
	{
		int j = strcmp((char *)AdapterList[i], (char *)m_chDevicesList[m_iComboDevices]);
		if (j == 0)
			Open = i;
	}

	// Open the selected adapter
	lpAdapter =  PacketOpenAdapter( AdapterList[Open] );
	
	if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE))
	{
		dwErrorCode = GetLastError();
		char chTemp[10];
		sprintf(chTemp, "Unable to open the adapter, Error Code : %lx", dwErrorCode);
		MessageBox(chTemp, "Opening Adapter Error!", MB_ICONERROR|MB_OK);
		return -1;
	}	

	// Allocate a buffer to get the MAC address
	OidData = (struct _PACKET_OID_DATA *)malloc(6 + sizeof(PACKET_OID_DATA));
	if (OidData == NULL) 
	{
		MessageBox("Error allocating memory for MAC address!", "Memory Error", MB_ICONERROR|MB_OK);
		PacketCloseAdapter(lpAdapter);
		return -1;
	}

	// Retrieve the adapter MAC querying the NIC driver
	OidData->Oid = OID_802_3_CURRENT_ADDRESS;
	OidData->Length = 6;
	ZeroMemory(OidData->Data, 6);

	Status = PacketRequest(lpAdapter, FALSE, OidData);
	if (Status)
	{
		m_uchSrcMacAddress[0] = (OidData->Data)[0];
		m_uchSrcMacAddress[1] = (OidData->Data)[1];
		m_uchSrcMacAddress[2] = (OidData->Data)[2];
		m_uchSrcMacAddress[3] = (OidData->Data)[3];
		m_uchSrcMacAddress[4] = (OidData->Data)[4];
		m_uchSrcMacAddress[5] = (OidData->Data)[5];
		
		char tmp[100];
		sprintf(tmp,"%.2X : %.2X : %.2X : %.2X : %.2X : %.2X",
			(OidData->Data)[0],
			(OidData->Data)[1],
			(OidData->Data)[2],
			(OidData->Data)[3],
			(OidData->Data)[4],
			(OidData->Data)[5]);
		SetDlgItemText(IDC_STATIC_MACSRCADDR, (char *)tmp);
		m_bIsDevActive = TRUE;
	}
	else
	{
		SetDlgItemText(IDC_STATIC_MACSRCADDR, "Error retrieving the MAC address of the adapter!");
	}

	free(OidData);
	PacketCloseAdapter(lpAdapter);
	return 0;
}

//---------------------------------------------------------------------------------- 
//------------------------------- GetDestMacAddresses ------------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::GetDestMacAddresses()
{
	FILE *fp;
	fp = fopen("Dest_Mac_Addr.txt", "r+b");
	char filedata[200];
	int i, j=0;
	CString cstrTemp;
	
	m_iNumOfAddrs = 0;
	m_ctrlComboMacDestAddr.ResetContent();

	if (fp)
	{
		int filesize = fread(filedata, sizeof(char), 200, fp);
		if (((filesize+2)%19) != 0)
		{
			MessageBox("Error in destination MAC address list file format!", "File Format Error", MB_ICONWARNING|MB_OK);
			return;
		}

		m_iNumOfAddrs = (filesize + 2) / 19;

		for (i=0; i < m_iNumOfAddrs; i++)
		{
			cstrTemp.Empty();

			for (int j=0; j < 19; j++)
			{
				if (j < 17)
				{
					cstrTemp.Insert(j, filedata[19*i+j]);
					if ((j%3) == 1)
						m_chDestAddrList[i][j/3] = (char2hex(filedata[19*i+j-1]) << 4) + char2hex(filedata[19*i+j]);
				}
				else if (j == 18)
					m_ctrlComboMacDestAddr.InsertString(i, cstrTemp);
 			}
		}

		fclose(fp);
	}
	else
		MessageBox("Could not find Dest_Mac_Addr.txt file!", "File Error", MB_ICONWARNING|MB_OK);

	m_ctrlComboMacDestAddr.InsertString(m_iNumOfAddrs, "Add to list ...");
}

//---------------------------------------------------------------------------------- 
//--------------------------- OnSelchangeComboDstMacAddr ---------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnSelchangeComboDstMacAddr() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_iComboMacDestAddr == m_iNumOfAddrs)
	{
		m_dlgGetMac.DoModal();		
		AddDestMacAddress(m_dlgGetMac.m_cstrMacAddr);
	}
}

//---------------------------------------------------------------------------------- 
//-------------------------------- AddDestMacAddress -------------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::AddDestMacAddress(CString Addr)
{
	FILE *fp;
	fp = fopen("Dest_Mac_Addr.txt", "ab");
	
	int curIndex = m_iNumOfAddrs;
	char pad[2];
	pad[0] = 10;
	pad[1] = 13;

	if (fp)
	{
		//Check for an empty list file
		if (m_iNumOfAddrs > 0)
		{
			Addr = pad[1] + Addr; //Add carriage return (CR) character
			Addr = pad[0] + Addr; //Add line feed (LF) character
			fwrite(Addr, sizeof(char), 19, fp);
		}
		else
			fwrite(Addr, sizeof(char), 17, fp);
		
		fclose(fp);
	}
	else
		MessageBox("Could not create Dest_Mac_Addr.txt file!", "File Error", MB_ICONWARNING|MB_OK);

	GetDestMacAddresses();
	m_ctrlComboMacDestAddr.SetCurSel(curIndex);
}

//---------------------------------------------------------------------------------- 
//---------------------------------- OnButtonSend ----------------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// Supposing to be on Ethernet, Set MAC destination to 1:1:1:1:1:1
	m_tempPacket[0] = m_chDestAddrList[m_iComboMacDestAddr][0];
	m_tempPacket[1] = m_chDestAddrList[m_iComboMacDestAddr][1];
	m_tempPacket[2] = m_chDestAddrList[m_iComboMacDestAddr][2];
	m_tempPacket[3] = m_chDestAddrList[m_iComboMacDestAddr][3];
	m_tempPacket[4] = m_chDestAddrList[m_iComboMacDestAddr][4];
	m_tempPacket[5] = m_chDestAddrList[m_iComboMacDestAddr][5];
		
	// Set MAC source to 2:2:2:2:2:2
	m_tempPacket[6]  = m_uchSrcMacAddress[0];
	m_tempPacket[7]  = m_uchSrcMacAddress[1];
	m_tempPacket[8]  = m_uchSrcMacAddress[2];
	m_tempPacket[9]  = m_uchSrcMacAddress[3];
	m_tempPacket[10] = m_uchSrcMacAddress[4];
	m_tempPacket[11] = m_uchSrcMacAddress[5];

	// Data Length: 2 Bytes
	m_tempPacket[12] = (unsigned char)((m_uiPacketSize-14) >> 8); //MS Byte
	m_tempPacket[13] = (unsigned char)((m_uiPacketSize-14) % 256); //LS Byte

	if ( !m_bInputMessage )
	{
		UpdateData(TRUE);
		int msgLen = m_cstrMessage.GetLength();
		char *msg = new char [msgLen+1];
		strcpy(msg, m_cstrMessage);

		// Fill the rest of the packet
		for(int i = 14; i < m_uiPacketSize; i++)
		{
			if ( i < (14 + msgLen) )
				m_tempPacket[i]= (u_char)msg[i-14];
			else
				m_tempPacket[i]= 0;
		}
	
		// Send down the packet
		if (pcap_sendpacket(m_adhandle,			// Adapter
							m_tempPacket,		// buffer with the packet
							m_uiPacketSize		// size
			) != 0)
		{

			MessageBox(pcap_geterr(m_adhandle), "Sending Error", MB_ICONERROR|MB_OK);
			return;
		}
	}
	else
	{
		// start the Send File Thread
		m_hSendFileThread = CreateThread( NULL, 0, SendFileThreadFunc, this, 0, 0 );		
		SetThreadPriority( m_hSendFileThread, THREAD_PRIORITY_ABOVE_NORMAL );
		//SetThreadPriority( m_hSendFileThread, THREAD_PRIORITY_HIGHEST );
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	}	
}

//---------------------------------------------------------------------------------- 
//---------------------------- OnButtonOpenFile Function ---------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnButtonOpenFile() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = "Data Files (*.dat)|*.dat|All Files (*.*)|*.*||";
	CFileDialog fd_open(TRUE, ".dat","Input File", OFN_READONLY, szFilter);
	fd_open.m_ofn.lpstrInitialDir = ".\\";
	
	if(fd_open.DoModal() == IDOK)
		m_cstrSendFileName = fd_open.GetFileName();
	
	CFile fp;
	if ( fp.Open(m_cstrSendFileName, CFile::modeRead) )
	{
		m_dwSendFileLength = fp.GetLength();
		fp.Close();
	}

	m_pFileRead = NULL;
 	if (m_cstrSendFileName.GetLength())
 		m_pFileRead = fopen(m_cstrSendFileName, "r+b");

	if ( !m_pFileRead )
		MessageBox("File can't be opened: Please select another file!", "File Error", MB_ICONWARNING|MB_OK);
	else	
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
}

//---------------------------------------------------------------------------------- 
//---------------------------- SendFile Thread Function ----------------------------
//---------------------------------------------------------------------------------- 
DWORD WINAPI CTestEthDlg::SendFileThreadFunc(void * pParam)
{
	//Read DMA	
	CTestEthDlg *pMain = (CTestEthDlg *)pParam;

	unsigned char file_data[Max_Pkt_Size-14];
	UINT iLen = 0;
	float fStep;
	UINT iStepIndex = 0;
	double dDurTotal = 1.0;
	UINT iTxLenTotal = 0;
	char strTmp[10];

	if ( pMain->m_dwSendFileLength != 0 )
		fStep = 100 * (float)(pMain->m_uiPacketSize-14)/(float)pMain->m_dwSendFileLength;
	 else
		fStep = 0;

	// Supposing to be on Ethernet, Set MAC destination to 1:1:1:1:1:1
	pMain->m_tempPacket[0] = pMain->m_chDestAddrList[pMain->m_iComboMacDestAddr][0];
	pMain->m_tempPacket[1] = pMain->m_chDestAddrList[pMain->m_iComboMacDestAddr][1];
	pMain->m_tempPacket[2] = pMain->m_chDestAddrList[pMain->m_iComboMacDestAddr][2];
	pMain->m_tempPacket[3] = pMain->m_chDestAddrList[pMain->m_iComboMacDestAddr][3];
	pMain->m_tempPacket[4] = pMain->m_chDestAddrList[pMain->m_iComboMacDestAddr][4];
	pMain->m_tempPacket[5] = pMain->m_chDestAddrList[pMain->m_iComboMacDestAddr][5];
	
	// Set MAC source to 2:2:2:2:2:2
	pMain->m_tempPacket[6]  = pMain->m_uchSrcMacAddress[0];
	pMain->m_tempPacket[7]  = pMain->m_uchSrcMacAddress[1];
	pMain->m_tempPacket[8]  = pMain->m_uchSrcMacAddress[2];
	pMain->m_tempPacket[9]  = pMain->m_uchSrcMacAddress[3];
	pMain->m_tempPacket[10] = pMain->m_uchSrcMacAddress[4];
	pMain->m_tempPacket[11] = pMain->m_uchSrcMacAddress[5];

	// Data Length: 2 Bytes
	pMain->m_tempPacket[12] = (unsigned char)((pMain->m_uiPacketSize-14) >> 8); //MS Byte
	pMain->m_tempPacket[13] = (unsigned char)((pMain->m_uiPacketSize-14) % 256); //LS Byte

	if ( pMain->m_pFileRead )
	{
		while ( (iLen = fread(file_data, sizeof(char), pMain->m_uiPacketSize-14, pMain->m_pFileRead)) == (pMain->m_uiPacketSize-14) )
		{
	 		pMain->m_ocTimer.Start();
			memcpy(pMain->m_tempPacket+14, file_data, pMain->m_uiPacketSize-14);

			// Send down the packet
			if (pcap_sendpacket(pMain->m_adhandle,		// Adapter
								pMain->m_tempPacket,	// buffer with the packet
								pMain->m_uiPacketSize	// size
				) != 0)
			{
				sprintf(pMain->m_chTemp,"Error sending the packet: %s", pcap_geterr(pMain->m_adhandle));
				TRACE(pMain->m_chTemp);
				return -1;
			}

			iStepIndex++;
			pMain->m_ocTimer.Stop();

			dDurTotal = dDurTotal+pMain->m_ocTimer.GetDuration();
			iTxLenTotal = iTxLenTotal + pMain->m_uiPacketSize;

			if ( (iStepIndex % 20) == 0 )
			{
				// Update Progress bar position and Transfer Rate value
				pMain->m_ctrlProgressSendFile.SetPos((int)(fStep * (float)iStepIndex));
				sprintf(strTmp, "%4.5f", (float)iTxLenTotal/dDurTotal);
				pMain->SetDlgItemText(IDC_STATIC_TXRATE, strTmp);
			}
			
			//Sleep(1);
		}


		//Send the end of file
		memcpy(pMain->m_tempPacket+14, file_data, iLen);
		if (pcap_sendpacket(pMain->m_adhandle,		// Adapter
							pMain->m_tempPacket,	// buffer with the packet
							iLen+14					// size
			) != 0)
		{
			sprintf(pMain->m_chTemp,"Error sending the packet: %s", pcap_geterr(pMain->m_adhandle));
			TRACE(pMain->m_chTemp);
			return -1;
		}
	}

	// Final Updates for Progress bar position and Transfer Rate value
	pMain->m_ctrlProgressSendFile.SetPos(100);
	sprintf(strTmp, "%4.5f", (float)iTxLenTotal/dDurTotal);
	pMain->SetDlgItemText(IDC_STATIC_TXRATE, strTmp);

	return 0;
}

//---------------------------------------------------------------------------------- 
//--------------------------------- OnButtonReceive --------------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnButtonReceive() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_RECEIVE)->EnableWindow(FALSE);

	static char BASED_CODE szFilter[] = "Data Files (*.dat)|*.dat|All Files (*.*)|*.*||";
	CFileDialog fd_save(FALSE,".dat","Output File", OFN_OVERWRITEPROMPT, szFilter);
	fd_save.m_ofn.lpstrInitialDir = ".\\";
	
	if(fd_save.DoModal() == IDOK)
		m_cstrDumpFileName = fd_save.GetFileName();

	m_pFileWriteTotal = NULL;
	m_pFileWriteTotal = fopen(m_cstrDumpFileName, "w+b");
	if ( !m_pFileWriteTotal )
		MessageBox("File can't open", "File Error", MB_ICONWARNING|MB_OK);

	m_pFileWriteNet = NULL;
	m_pFileWriteNet = fopen("Net_"+m_cstrDumpFileName, "w+b");
	if ( !m_pFileWriteNet )
		MessageBox("File can't open", "File Error", MB_ICONWARNING|MB_OK);

// 	// Open the dump file
// 	if ( m_bIsDevActive )
// 		m_dumpfile = pcap_dump_open(m_adhandle, m_cstrDumpFileName);
// 	else
// 	{
// 		MessageBox("Error in opening dump file! \n Select Another device", "Opening Error", MB_ICONWARNING|MB_OK);
// 		GetDlgItem(IDC_BUTTON_RECEIVE)->EnableWindow(TRUE);
// 		return;
// 	}
// 	
// 	if(m_dumpfile == NULL)
// 	{
// 		MessageBox("Error opening output file !!!");
// 		return;
// 	}

	// At this point, we don't need any more the device list. Free it
	pcap_freealldevs(m_alldevs);
	
	// start the Receive Thread
	m_hReceiveThread = CreateThread( NULL, 0, ReceiveThreadFunc, this, 0, 0 );		
	//SetThreadPriority( m_hReceiveThread, THREAD_PRIORITY_ABOVE_NORMAL );
	SetThreadPriority( m_hReceiveThread, THREAD_PRIORITY_TIME_CRITICAL );

	m_hProcessThread = CreateThread( NULL, 0, ReceiveProcessFunc, this, 0, 0 );
	SetThreadPriority( m_hProcessThread, THREAD_PRIORITY_TIME_CRITICAL );
	//pcap_close(m_adhandle);
	return;
}

//---------------------------------------------------------------------------------- 
//-------- Callback function invoked by libpcap for every incoming packet ----------
//---------------------------------------------------------------------------------- 
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	time_t local_tv_sec;
	CString cstrData;
	CString cstrSrcAddr;
	CString cstrDstAddr;
	char charTemp[10];
	unsigned short iLen=0;
	double dDurTemp;
	double fRate;
	DWORD  Read_data;
	char *pch_mem_dummy;
    
	// FILE *fp;
	// fp = fopen("mylog.txt", "ab");
	pch_mem_dummy = pDlg->m_pchMem;
	if ( !pDlg->m_bDisableList )
	{
 		//Delete non-observable characters
		for (int j=0; j < header->len; j++)
		{
			if (j < 6)				// Distinguish Source Physical Address
			{
				iLen = 0;
				sprintf(charTemp, "%02X", pkt_data[j]); 
				cstrDstAddr.Insert(3*j, charTemp);
				if (j < 5)
					cstrDstAddr.Insert(3*j+2, ":");
			}
			else if (j < 12)		// Distinguish Destination Physical Address
			{
				sprintf(charTemp, "%02X", pkt_data[j]); 
				cstrSrcAddr.Insert(3*(j-6), charTemp);
				if (j < 11)
					cstrSrcAddr.Insert(3*j+2, ":");
			}
			else if (j < 14)
			{
				iLen = iLen*256 + pkt_data[j];
			}
			else if (pkt_data[j] == '\0')
				cstrData.Insert(j, " ");
			else
				cstrData.Insert(j, pkt_data[j]);
		}

		// convert the timestamp to readable format
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);
		
		TRACE("%s,%.6d len:%d\n", timestr, header->ts.tv_usec, header->len);	

		if (pDlg)
		{
			int nLEN = cstrData.GetLength();
			if ( nLEN )
			{
				sprintf(charTemp, "%d", pDlg->m_uiItemCntr);
				pDlg->m_ctrlListRcvdMsg.InsertItem(pDlg->m_uiItemCntr, charTemp);
				pDlg->m_ctrlListRcvdMsg.SetItemText(pDlg->m_uiItemCntr, 1, cstrSrcAddr);
				pDlg->m_ctrlListRcvdMsg.SetItemText(pDlg->m_uiItemCntr, 2, cstrDstAddr);
				sprintf(charTemp, "%d", iLen);
				pDlg->m_ctrlListRcvdMsg.SetItemText(pDlg->m_uiItemCntr, 3, charTemp);
				pDlg->m_ctrlListRcvdMsg.SetItemText(pDlg->m_uiItemCntr++, 4, cstrData);
				pDlg->m_ctrlListRcvdMsg.SetBkColor(RGB(0,255,0));
			}

		}
	}
	

//	if ( pDlg->m_pFileWriteTotal )
//	{
//		fwrite(pkt_data, sizeof(char), header->len, pDlg->m_pFileWriteTotal);
//		fflush(pDlg->m_pFileWriteTotal);
// 	}

	if ( pDlg->m_pFileWriteNet )
	{
		//if ((pkt_data[0] & pkt_data[1] & pkt_data[2] & pkt_data[3] & pkt_data[4] & pkt_data[5]) != 0xff)
		//Only from Xilinx FPGAs MAC
		if ((pkt_data[0]!=0xff) && (pkt_data[1]!=0xff) && (pkt_data[2]!=0xff) && (pkt_data[6]==0x00) && (pkt_data[7]==0x0A) && (pkt_data[8]==0x35))
		{
			//fwrite(pkt_data, sizeof(char), header->len, fp);
			//fwrite(pkt_data+14, sizeof(char), header->len-14, pDlg->m_pFileWriteNet);
			//fflush(pDlg->m_pFileWriteNet);
			if (pDlg->m_bStartRx)
			{
				pDlg->m_ocTimerRx.Start();
				pDlg->m_bStartRx = FALSE;
			}
			if (pDlg->m_bRun)
			{
				/*if ( pDlg->m_nByteCntr <= (pDlg->m_nBufferSize - header->len + 14) )
				{*/

					CIoBuf *pocBuf = pDlg->m_ocChBuff.GetEmptyBuf();
					if( pocBuf )
					{
						memcpy( pocBuf->m_pBufferMem,  pkt_data+14,header->len-14 );
						pDlg->m_ocChBuff.ReleaseEmptyBuf();
						ResumeThread(pDlg->m_hProcessThread );
						pDlg->m_nByteCntr = pDlg->m_nByteCntr + header->len - 14;	
						pDlg->m_iNumRecBuffs++;
					
					}
					else
					{
						pDlg->m_nMissProcCount++;
					}

					/*memcpy(pDlg->m_pchMem+pDlg->m_nByteCntr, pkt_data+14, header->len-14);
					pDlg->m_nByteCntr = pDlg->m_nByteCntr + header->len - 14;	*/
				//}

			}
			/*if ((pDlg->m_nByteCntr >=  1073740800) )
			{
				pDlg->m_ocTimerRx.Stop();
				dDurTemp = pDlg->m_ocTimerRx.GetDuration();
				fRate = pDlg->m_nByteCntr / dDurTemp / 1.048576;
				pDlg->SetDlgItemInt(IDC_STATIC_RATE, (int)fRate);
				pDlg->m_bRatePrint = FALSE;
			}*/
		}
	}

// 	// save the packet on the dump file
// 	pcap_dump(param, header, pkt_data);
}


//=========================================================
DWORD WINAPI CTestEthDlg:: ReceiveProcessFunc(void * pParam)
//=========================================================
{
	
	int nMissCount = 0;
	int nError = 0;
	unsigned char memBuf[0x58e-14];
	DWORD nSample = 0;
	pDlg->m_dwPrvSample = 0;
	int i, j;
	double dDurTemp;
	double fRate;
	//unsigned long wr_cnt = 0;
	CString	StrTmp;
	FILE *pSaveFile = NULL;
	if( pSaveFile == NULL )
		pSaveFile = fopen("Test.hex", "wb");
//	int k = sizeof(unsigned long long  int);
	while( pDlg->m_bRun )
	{
		CIoBuf *pocBuf = pDlg->m_ocChBuff.GetFilledBuf();

		if( pocBuf )
		{
			memcpy(memBuf, pocBuf->m_pBufferMem, pocBuf->m_nBufferSize);
			pDlg->m_wr_cnt++;
			/*if( pSaveFile ){
				pDlg->m_wr_cnt++;
				fwrite(pocBuf->m_pBufferMem, 1, pocBuf->m_nBufferSize, pSaveFile);

			}*/
			for (i = 0; i < pocBuf->m_nBufferSize/4; i++)
			{
				nSample = 0;
				for (j = 0; j < 4 ; j++)
					nSample = nSample + (memBuf[4*i+j] << (j*8));

				if (nSample != (pDlg->m_dwPrvSample+1) && (pDlg->m_dwPrvSample != 0xffffffff) )
				{
					nError++;
					StrTmp.Format("%08d",pDlg->m_wr_cnt);
					pDlg->m_ctrlerr.AddString(StrTmp);

					pDlg->m_ocTimerRx.Stop();
					dDurTemp = pDlg->m_ocTimerRx.GetDuration();
					fRate = pDlg->m_nByteCntr / dDurTemp / 1.048576;
					pDlg->SetDlgItemInt(IDC_STATIC_RATE, (int)fRate);
					pDlg->m_bRatePrint = FALSE;
					Sleep(5000);
			
				}
				
				pDlg->m_dwPrvSample = nSample;
			}

			if ( nError )
				pDlg->m_nMissProcCount++;

			nError = 0;
			pDlg->m_ocChBuff.ReleaseFilledBuf();			
		}
		else
			SuspendThread(pDlg->m_hProcessThread);
		
		if( nMissCount != pDlg->m_nMissProcCount )
		{
			nMissCount = pDlg->m_nMissProcCount;

			StrTmp.Format("%08X",nMissCount);
			pDlg->m_ctrlerr.AddString(StrTmp);
		//	pMain->SetDlgItemInt(IDC_STATIC_MISSEDBUFFS, nMissCount );
		}
	}
	
	if( pSaveFile )
		fclose(pSaveFile);

	return 0  ;
}



//---------------------------------------------------------------------------------- 
//--------------------------------- Thread Function --------------------------------
//---------------------------------------------------------------------------------- 
DWORD WINAPI CTestEthDlg::ReceiveThreadFunc(void * pParam)
{int res,i;

	//Read DMA	
	CTestEthDlg *pMain = (CTestEthDlg *)pParam;

	//pcap_loop(pMain->m_adhandle, 0, packet_handler, NULL);
	pcap_loop(pMain->m_adhandle, 0, packet_handler, (unsigned char *)pMain->m_dumpfile);
	//while((res = pcap_next_ex( pMain->m_adhandle, &(pMain->header), &(pMain->pkt_data))) >= 0)
	//{

	//	if(res == 0)
	//		/* Timeout elapsed */
	//		continue;

	//	/* print pkt timestamp and pkt len */
	//	printf("%ld:%ld (%ld)\n", (pMain->header)->ts.tv_sec, (pMain->header)->ts.tv_usec, (pMain->header)->len);			
	//	
	//	/* Print the packet */
	//	for (i=1; (i < (pMain->header)->caplen + 1 ) ; i++)
	//	{
	//		printf("%.2x ", pMain->pkt_data[i-1]);
	//		if ( (i % LINE_LEN) == 0) printf("\n");
	//	}
	//	
	//	printf("\n\n");		
	//}

	//if(res == -1)
	//{
	//	printf("Error reading the packets: %s\n", pcap_geterr(pMain->m_adhandle));
	//	return -1;
	//}

	pcap_close(pMain->m_adhandle);
	return -1;
}

//---------------------------------------------------------------------------------- 
//-------------------------- OnCheckResumethread Function --------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnCheckResumethread() 
{
	// TODO: Add your control notification handler code here
	if( IsDlgButtonChecked(IDC_CHECK_RESUMETHREAD) )
		SuspendThread(m_hReceiveThread);
	else
		ResumeThread(m_hReceiveThread);
}

//---------------------------------------------------------------------------------- 
//--------------------------- OnCheckInputmethod Function --------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnCheckInputmethod() 
{
	// TODO: Add your control notification handler code here
	if( IsDlgButtonChecked(IDC_CHECK_INPUTMETHOD) )
	{
		GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		m_bInputMessage = TRUE;
	}
	else
	{
		GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		m_bInputMessage = FALSE;
	}
}

//---------------------------------------------------------------------------------- 
//--------------------------- OnCheckInputmethod Function --------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnCheckDisableList() 
{
	// TODO: Add your control notification handler code here
	if( IsDlgButtonChecked(IDC_CHECK_DISABLELIST) )
	{
		m_bDisableList = TRUE;
		GetDlgItem(IDC_LIST_RCVDMSG)->EnableWindow(FALSE);
	}
	else
	{
		m_bDisableList = FALSE;
		GetDlgItem(IDC_LIST_RCVDMSG)->EnableWindow(TRUE);
	}	
}

//---------------------------------------------------------------------------------- 
//------------------------------- OnDestroy Function -------------------------------
//---------------------------------------------------------------------------------- 
void CTestEthDlg::OnDestroy()
{
	// TODO: Add your message handler code here
	CDialog::OnDestroy();
	
	int nCntr = 0;
	int WriteLen = 1024;

	m_bRun = FALSE;
	if ( m_hReceiveThread )
		TerminateThread(m_hReceiveThread, NULL);
	
	if ( m_hSendFileThread )
		TerminateThread(m_hSendFileThread, NULL);
 
	//At this point, we no longer need the device list. Free it
    if ( m_adhandle != NULL )
		pcap_close(m_adhandle);

	//Write Memory Data to file
	/*if ( m_pFileWriteNet )
	{
		while (nCntr <= (m_nByteCntr-WriteLen) )
		{
			fwrite(m_pchMem+nCntr, sizeof(char), WriteLen, m_pFileWriteNet);
			
			nCntr = nCntr + WriteLen;
		}
	}*/

	if ( m_pchMem != NULL ) 
	{
		delete [] m_pchMem;
		m_pchMem = NULL;
	}

	if (m_pFileWriteNet)
	{
		fclose( m_pFileWriteNet );
		m_pFileWriteNet = NULL;
	}

	if (m_pFileWriteTotal)
	{
		fclose(m_pFileWriteTotal);
		m_pFileWriteTotal = NULL;
	}

	if ( m_pFileRead )
	{
		fclose( m_pFileRead );
		m_pFileRead = NULL;
	}
}

//---------------------------------------------------------------------------------- 
//-------------------------- PreTranslateMessage Function --------------------------
//---------------------------------------------------------------------------------- 
BOOL CTestEthDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:	
			{
				// ...

				return TRUE;
				break;
			}
		case VK_ESCAPE:
			{
				return TRUE;
				break;
			}
		}
	}	
	
	return CDialog::PreTranslateMessage(pMsg);
}


void CTestEthDlg::OnBnClickedButton1()
{
	double dDurTemp;
	double fRate;
	CString StrTmp;

		if ((pDlg->m_bRatePrint == TRUE))
			{
				StrTmp.Format("%10d",pDlg->m_wr_cnt); // pDlg->m_wr_cnt* 1408 size of final file 
				pDlg->m_ctrlerr.AddString(StrTmp);
				pDlg->m_ocTimerRx.Stop();
				dDurTemp = pDlg->m_ocTimerRx.GetDuration();
				fRate = pDlg->m_nByteCntr / dDurTemp / 1.048576;
				pDlg->SetDlgItemInt(IDC_STATIC_RATE, (int)fRate);
				pDlg->m_bRatePrint = FALSE;

			}
}
