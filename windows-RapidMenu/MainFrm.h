// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__0B55A31A_AB37_4598_A224_A8497553C5D9__INCLUDED_)
#define AFX_MAINFRM_H__0B55A31A_AB37_4598_A224_A8497553C5D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LanguageRes.h"


class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

private:
	NOTIFYICONDATA m_tnd;
	
	BOOL m_bAutoRun;
	CString sInstallDir;
	CLanguageRes* m_pLanguage; 

	//BOOL bMsgBoxShow;

private:
	void InitCaption();
	void CreateSystemIcon(void);
	void InitInstallDir(void);

	afx_msg LRESULT OnIconNotify(WPARAM wParam, LPARAM lParam);
	void OnAppLaunch(void);
	void OnAppExit(void);

	void PopupMenu(int x, int y);
	BOOL ReadStartState();
	void WriteStartState(BOOL sbStatus);
    void SetRapidMenuDefaultState();
	BOOL IsTsoProcessExist();

	void GetUserAppDataLocation(unsigned char* appDir);
	BOOL SendCommand(const wchar_t* command);
	LRESULT OnTaskBarCreated(WPARAM wp, LPARAM lp);	
	
	//++
	DWORD GetCurrentTIOPid();
	//--

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL bIconState;
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAPPNEWAll();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__0B55A31A_AB37_4598_A224_A8497553C5D9__INCLUDED_)

