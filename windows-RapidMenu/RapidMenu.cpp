// RapidMenu.cpp : Defines the class behaviors for the application.
//


#include "stdafx.h"

#include "app_message.h"
#include "RapidMenu.h"
#include "MainFrm.h"
#include "RapidMenuDoc.h"
#include "RapidMenuView.h"
#include <tchar.h>

#define RAPIDMENU_MUTEX_NAME		RAPIDMENU

HANDLE g_hMutex;

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuApp

BEGIN_MESSAGE_MAP(CRapidMenuApp, CWinApp)
	//{{AFX_MSG_MAP(CRapidMenuApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuApp construction

CRapidMenuApp::CRapidMenuApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	isAboutDlgOpen = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRapidMenuApp object

CRapidMenuApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuApp initialization

BOOL CRapidMenuApp::InitInstance()
{
	//根据指定名称，自动处理。
	g_hMutex = getMutex(RAPIDMENU_MUTEX_NAME);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}
	if (g_hMutex == NULL)
	{
		return FALSE;
	}

	CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(
		IDR_ICON_APP,
		RUNTIME_CLASS(CRapidMenuDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CRapidMenuView)
		);
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
//	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;
	AfxGetApp()->OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL);

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_HIDE);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CRapidMenuApp message handlers
