// TestEth.h : main header file for the TESTETH application
//

#if !defined(AFX_TESTETH_H__F7B6331C_3284_48F0_B66E_5078C5A37543__INCLUDED_)
#define AFX_TESTETH_H__F7B6331C_3284_48F0_B66E_5078C5A37543__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestEthApp:
// See TestEth.cpp for the implementation of this class
//

class CTestEthApp : public CWinApp
{
public:
	CTestEthApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestEthApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestEthApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTETH_H__F7B6331C_3284_48F0_B66E_5078C5A37543__INCLUDED_)
