// GetMacAddrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestEth.h"
#include "GetMacAddrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetMacAddrDlg dialog


CGetMacAddrDlg::CGetMacAddrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetMacAddrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetMacAddrDlg)
	m_cstrMacAddr = _T("");
	//}}AFX_DATA_INIT
}


void CGetMacAddrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetMacAddrDlg)
	DDX_Text(pDX, IDC_EDIT_DESTMACADDR, m_cstrMacAddr);
	DDV_MaxChars(pDX, m_cstrMacAddr, 17);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetMacAddrDlg, CDialog)
	//{{AFX_MSG_MAP(CGetMacAddrDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetMacAddrDlg message handlers
