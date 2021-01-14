#if !defined(AFX_GETMACADDRDLG_H__0C6EB0E8_4360_4AE7_B8FE_4AC46E4ACD3C__INCLUDED_)
#define AFX_GETMACADDRDLG_H__0C6EB0E8_4360_4AE7_B8FE_4AC46E4ACD3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetMacAddrDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetMacAddrDlg dialog

class CGetMacAddrDlg : public CDialog
{
// Construction
public:
	CGetMacAddrDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetMacAddrDlg)
	enum { IDD = IDD_DIALOG_GETADDR };
	CString	m_cstrMacAddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetMacAddrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetMacAddrDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETMACADDRDLG_H__0C6EB0E8_4360_4AE7_B8FE_4AC46E4ACD3C__INCLUDED_)
