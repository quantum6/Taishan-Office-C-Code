// RapidMenu.h : main header file for the RAPIDMENU application
//

#if !defined(AFX_RAPIDMENU_H__16783D0C_F2CD_426A_84B2_455B17EDC91B__INCLUDED_)
#define AFX_RAPIDMENU_H__16783D0C_F2CD_426A_84B2_455B17EDC91B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuApp:
// See RapidMenu.cpp for the implementation of this class
//

class CRapidMenuApp : public CWinApp
{
public:
	CRapidMenuApp();
	void OnAppAbout(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRapidMenuApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

private:
	BOOL isAboutDlgOpen;
// Implementation
	//{{AFX_MSG(CRapidMenuApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAPIDMENU_H__16783D0C_F2CD_426A_84B2_455B17EDC91B__INCLUDED_)
