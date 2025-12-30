// MainFrm.cpp : implementation of the CMainFrame class
//

#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstringt.h>

#include "RapidMenu.h"
#include "MainFrm.h"
#include "FrMenu.h"
#include <tlhelp32.h>
#include "LanguageRes.h"
#include "LanguageResFactory.h"
#include "Registry.h"
#include "app_message.h"
#include "fw_kit.h" 

using std::vector;

#define OFFICE	"office"
#define REG_TAISHAN_OFFICE "Taishan\\Office"

#define WM_ICON_NOTIFY  WM_USER + 100
#define MAX_LENGTH		1024
#define	TIMER_ID		1		  

CString g_szCaption = OFFICE;
CString g_szWindowText = "";

int fontSize;
COLORREF rgbF;
COLORREF rgbB;
COLORREF rgbL;
CString fontName;

BOOL g_bIsTioVisible = FALSE;
CString g_szLan = "ENG";

DWORD g_dwTIOProcessId = 0;	 //0 = no initialization
vector<DWORD> vecTIOPid;
BOOL g_bControlEnum = TRUE;


extern HANDLE g_hMutex;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
//++ user227 add
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
//--
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ICON_NOTIFY,      OnIconNotify)
	ON_COMMAND(ID_APP_LAUNCH,       OnAppLaunch)
	ON_COMMAND(ID_APP_EXIT,         OnAppExit)
	//}}AFX_MSG_MAP
	//++ user227 add
	ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, OnTaskBarCreated)
	//--
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////




struct VerNumInfo 
{
	//
	//版本号解析
	//示例：4.0.1001.101ZH[.GE04][.L1][.SP1][.01]
	//
	int		m_nVersionMajor;   //主版本号
	int		m_nVersionMinor;   //次版本号
	long	m_lVersionBuild;   //构建版本号
	CString m_strSortArea;     //版本分类区域
	CString m_strGEName;	   //特殊版本名	
	int		m_nGENum;          //特殊版本号
	int		m_nSPNum;	       //大补丁号
	int		m_nPatchNum;	   //小补丁号
	CString m_strLimit;		   //限制号
	
	//++
	BOOL    m_bReserve;
	//--
};

static BOOL ParseVerNum13XEx(CString strVerNum, VerNumInfo* pInfo)
{
	int nStrLen = strVerNum.GetLength();
	int nPoints = 0;
	for(int i = 0; i < nStrLen; i ++)
	{
		if(strVerNum.GetAt(i) == '.')
		{
			nPoints ++;
		}
	}
	//++
	if(nPoints < 3) return FALSE;
	//--
	
	//保存结点的位置到数组，int[0]所在的位置'.'
	int *pos = new int[nPoints];
	int n = 0;
	for(int j = 0; j < nStrLen; j ++)
	{
		if(strVerNum.GetAt(j) == '.')
		{
			*(pos + n) = j;
			n ++;
		}
	}
	
	//获得两点之间的字符串信息，4.1.1812.101ZH，分段取值保存在字符串数组中。
	CString *sections = new CString[nPoints + 1];
	for(int l = 0; l < nPoints + 1; l ++)
	{
		if(l == 0)
		{
			sections[l] = strVerNum.Mid(0, pos[l]);
		}
		else if(l == nPoints)
		{
			sections[l] = strVerNum.Mid(pos[l - 1] + 1, nStrLen - pos[l - 1] - 1);
		}
		else
		{
			sections[l] = strVerNum.Mid(pos[l - 1] + 1, pos[l] - pos[l - 1] - 1);
		}
	}
	
	//	4.0.1001.101ZH[.GE04][.L1][.SP1][.01]
	//
	//	int		m_nVersionMajor;   //主版本号
	//	int		m_nVersionMinor;   //次版本号
	//	long	m_lVersionBuild;   //构建版本号
	//	CString m_strSortArea;     //版本分类区域
	//	CString m_strGEName;	   //特殊版本名
	//	int		m_nGENum;          //特殊版本号
	//	int		m_nSPNum;	       //大补丁号
	//	int		m_nPatchNum;	   //小补丁号
	//	CString m_strLimit;		   //限制号
	
	
	//++ 初始化结构
	pInfo->m_nVersionMajor = -1;
	pInfo->m_nVersionMinor = -1;
	pInfo->m_lVersionBuild = -1;
	pInfo->m_strSortArea   = "";
	pInfo->m_strGEName     = "";
	pInfo->m_nGENum        = -1;
	pInfo->m_strLimit      = "";
	pInfo->m_nSPNum        = -1;
	pInfo->m_nPatchNum     = -1;
	pInfo->m_bReserve      = FALSE;
	//--
	
	int nBeginSec = 0;
	int nSec = nPoints + 1;
	for (int sec = 0; sec < nSec; sec ++)
	{
		if (((CString)sections[sec]).GetLength() == 1) 
		{
			nBeginSec = sec;
			break;
		}
	}

	if (nBeginSec > 0)
	{
		pInfo->m_bReserve = TRUE;
	}

	pInfo->m_nVersionMajor = _ttoi(sections[nBeginSec]);
	pInfo->m_nVersionMinor = _ttoi(sections[nBeginSec + 1]);
	pInfo->m_lVersionBuild = _ttoi(sections[nBeginSec + 2]);
	pInfo->m_strSortArea   = sections[nBeginSec + 3];

	if(nSec > nBeginSec + 4)
	{
		for(int s = nBeginSec + 4; s < nSec; s ++)
		{
			CString strTemp = sections[s];
			char cTempL1 = (char)(strTemp.GetAt(0));

			//补丁包
			CString tempSp = "SP";
			if(strTemp.Mid(0, 2).CompareNoCase(tempSp) == 0)
			{
				pInfo->m_nSPNum = _ttoi(strTemp.Mid(2, strTemp.GetLength() - 2));
			}
	
			//特殊版本
			else if(strTemp.GetLength() == 4)
			{
				char cL1 = (char)(strTemp.GetAt(0));
				char cL2 = (char)(strTemp.GetAt(1));
				if(cL1 >= 65 && cL1 <= 90 && cL2 >= 65 && cL2 <= 90)
				{
					pInfo->m_strGEName = strTemp.Left(2);
					pInfo->m_nGENum = _ttoi(strTemp.Right(2));
				}
			}

			//限制编号
			else if(strTemp.GetLength() == 2 &&
				   (cTempL1 == 'A' || cTempL1 == 'B' || cTempL1 == 'T' || cTempL1 == 'L'))
			{
				pInfo->m_strLimit = strTemp;
			}
			else
			{
				if (strTemp.GetLength() < 6) 
				{
					//patch编号
					pInfo->m_nPatchNum = _ttoi(strTemp);
				}
				else
				{
					pInfo->m_bReserve = TRUE;
				}
			}
		}
	}
	
	//free memory
	delete[] sections;
	delete[] pos;
	//--
	return TRUE;
}


LRESULT CMainFrame::OnTaskBarCreated(WPARAM wp, LPARAM lp)
{
	CreateSystemIcon();
    return 0;
}
//--

CMainFrame::CMainFrame()
{
	CRegistry reg(HKEY_CLASSES_ROOT);
	reg.Open(_T(REG_TAISHAN_OFFICE));
	CString szVersion;
	reg.Read(_T("tio_version"),&szVersion);

	VerNumInfo info;
	//MessageBox((info.m_strSortArea));
	ParseVerNum13XEx(szVersion,&info);

	//得到主版本号
	int sMainVersion =  info.m_nVersionMajor;
	//MessageBox((info.m_strSortArea));
	//语言版本区分
	CString szTioVersion = info.m_strSortArea.Mid(3, 2);
	CString szDefLanID = "SCH";
	if (szTioVersion == "CH")
	{
		szDefLanID = "SCH";
	}
	else if (szTioVersion == "TW")
	{
		szDefLanID = "TCH";
	}
	else if (szTioVersion == "JA")
	{
		szDefLanID = "JAP";
	}
	else if (szTioVersion == "EN")
	{
		szDefLanID = "ENG";
	}

	//create language resource
	CLanguageResFactory languageFactory;
	m_pLanguage = languageFactory.GetLanguageRes(szDefLanID);
	g_szLan = szDefLanID;

	/////////////////////////////////////////////////////////
	sInstallDir = "";
	InitInstallDir();
	m_bAutoRun = ReadStartState();

	//
	// vista
	//
	if (GetCurrentTIOPid() == 0)
	{
		//MessageBox("GetCurrentTIOPid = 0");
	}
}

CMainFrame::~CMainFrame()
{
	if(m_pLanguage != NULL) 
	{
		delete m_pLanguage;
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	//TODO: Modify the Window class or styles here by modifying
	//the CREATESTRUCT cs
	cs.dwExStyle |= WS_EX_TOOLWINDOW;	//added by linhui 2002.3.21
	cs.style = WS_POPUP;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::CreateSystemIcon()
{	
	BOOL bEnabled = ((GetVersion() & 0xff) >= 4);
	if(!bEnabled)
	{
		return;
	}

	m_tnd.cbSize = sizeof(NOTIFYICONDATA); 
	m_tnd.hWnd   = GetSafeHwnd(); 
	m_tnd.uID    = IDR_POPUP_MENU; 
	m_tnd.hIcon  = AfxGetApp()->LoadIcon(IDI_ICON_SYSTRAY);
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = WM_ICON_NOTIFY;
	wcscpy(m_tnd.szTip, m_pLanguage->szMenu_Name);

	//set system icon
    ::Shell_NotifyIcon(NIM_ADD, &m_tnd);
}

void CMainFrame::InitInstallDir()
{	
	unsigned char returnValue[MAX_LENGTH + 1];
	HKEY hSubKey;
	DWORD dwSize = MAX_LENGTH;
	
	if(::RegOpenKeyExA(HKEY_CLASSES_ROOT, REG_TAISHAN_OFFICE, 0, KEY_READ, &hSubKey))
	{
		return;
	}
	if(::RegQueryValueExA(hSubKey, "install_dir", NULL, NULL, returnValue, &dwSize))
	{
		if(::RegQueryValueEx(hSubKey, NULL, NULL, NULL, returnValue, &dwSize))
		{
			::RegCloseKey(hSubKey);
			return;
		}
	}
	
	CString sValue = returnValue; 
	//MessageBox(sValue);
	int nStart = 0;
	int nLen = sValue.GetLength();
	while(nStart < nLen)
	{
		nStart = sValue.Find(_T("\""), nStart);
		if(nStart == -1)
		{
			break;
		}
		sValue.Delete(nStart, 1);
	}
	nLen = sValue.GetLength();
	if (sValue.GetAt(nLen - 1) == '\\')
	{
		sValue.SetAt(nLen - 1, 0);
	}

	::RegCloseKey(hSubKey);
	sInstallDir = sValue;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//++
	CreateSystemIcon();
	SetTimer(TIMER_ID, 500, NULL);
	//--
	return 0;
}


void CMainFrame::OnAppExit() 

{
	g_bControlEnum  = FALSE;
	g_bIsTioVisible = FALSE;

	::EnumWindows((WNDENUMPROC)EnumWindowsProc, 0);

	/*
	if (IsTioProcessExist()) 
	{
		if (!g_bIsTioVisible) 
		{
			MessageBox(_T("泰山Office"),_T("泰山Office正在运行中，请先保存文件"),MB_OK);
		}
		SendCommand(TS_COMMAND_EXIT);
	}*/
	SendCommand(TS_COMMAND_CLOSE);
	OnClose();
}

void CMainFrame::OnAppLaunch()
{
	SendCommand(TS_COMMAND_START);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMER_ID)
	{
		if (!IsTsoProcessExist())
		{
			OnClose();
		}
	}
	else
	{
		CFrameWnd::OnTimer(nIDEvent);
	}
}

void CMainFrame::OnClose() 
{
	KillTimer(TIMER_ID);
	::Shell_NotifyIcon(NIM_DELETE, &m_tnd);
	//
	if (g_hMutex != NULL)
	{
		CloseHandle(g_hMutex);
	}
	CFrameWnd::OnClose();
}

/*
 * Popup menu when right click RapidMenu. 
 * modify by user227.	
 */
void CMainFrame::PopupMenu(int x, int y)
{
	SetRapidMenuDefaultState();

	CMenuInfo menuInfo[2]; 
	CFrMenu menuPopup;
	if(menuPopup.CreatePopupMenu())
	{
		menuInfo[0].sText = m_pLanguage->szMenu_Launch;//启动
		menuInfo[0].uMenuItemID = IDI_ICON_LAUNCH; 
		menuInfo[0].dBkBmpIndex = 1;
		menuPopup.AppendMenu(MF_OWNERDRAW, ID_APP_LAUNCH, (LPCTSTR)(&(menuInfo[0])));

		menuInfo[1].sText = m_pLanguage->szMenu_Exit;//退出
		menuInfo[1].uMenuItemID = IDI_ICON_EXIT;
		menuInfo[1].dBkBmpIndex = 0;
		menuPopup.AppendMenu(MF_OWNERDRAW, ID_APP_EXIT,   (LPCTSTR)(&(menuInfo[1])));

		SetForegroundWindow();
		//MessageBox(TPM_LEFTALIGN);
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, x, y, this);
		PostMessage(WM_NULL, 0, 0);
		menuPopup.DestroyMenu();
	}
}

/*
 * System callback function for enum window
 */
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	CWnd* pWnd = CWnd::FromHandle(hWnd);
	pWnd->GetWindowText(g_szWindowText);
	
	if (g_szWindowText.GetLength() > 0)
	{
		DWORD dwProcessId;
		::GetWindowThreadProcessId(hWnd, &dwProcessId);

		if (g_bControlEnum)
		{
			for (int i = 0; i < vecTIOPid.size(); i ++)
			{
				if (dwProcessId == vecTIOPid.at(i)) 
				{
					g_dwTIOProcessId = dwProcessId;
					return FALSE;
				}
			}
			return TRUE;
		}
		
		if (::IsWindowVisible(hWnd) && dwProcessId == g_dwTIOProcessId && g_dwTIOProcessId != 0)
		{
			g_bIsTioVisible = TRUE;
			return FALSE;
		}
	}
	return TRUE;
}

afx_msg LRESULT CMainFrame::OnIconNotify(WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(lParam) == /*WM_RBUTTONUP*/WM_RBUTTONDOWN) 
	{
		CPoint pos;
	    GetCursorPos(&pos);

		m_bAutoRun = ReadStartState();
		PopupMenu(pos.x, pos.y);
    }
    else if(LOWORD(lParam) == WM_LBUTTONDBLCLK)
    {
		SendCommand(TS_COMMAND_START);
    }
	return 0;
}


void CMainFrame::WriteStartState(BOOL bStatus) 
{
	char road[]="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";	
	HKEY hKey;
	
	LPCSTR lpValueName = "tsoffice";
	
	if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE, road, 0,KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if(!bStatus) 
		{
			::RegDeleteValueA(hKey,lpValueName);
		}
		else
		{
			CString sTioRunCommand = "\"" + sInstallDir + "\\"+TS_OFFICE_EXE+ "\" \"" + "???" + "\"";	
			const unsigned char * p = (const unsigned char *)sTioRunCommand.GetBuffer(sTioRunCommand.GetLength());
			::RegSetValueExA(hKey,
							lpValueName,
							0,
							REG_SZ,
							p,
							sTioRunCommand.GetLength() + 1);
		}
		::RegCloseKey(hKey);
	}
}


BOOL CMainFrame::ReadStartState() 
{
	char road[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";	
	HKEY hKey;
	BOOL bRet;
	
	if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE, road, 0, KEY_READ, &hKey) == ERROR_SUCCESS)		
	{
		
		if( hKey != NULL )
		{
			bRet = (::RegQueryValueExA(hKey, 
								      "tsoffice", 
									  NULL, 
									  NULL, 
									  NULL,
									  NULL) == ERROR_SUCCESS);
			::RegCloseKey(hKey);
		}
	}
	return bRet;
}

/*
* Set RapidMenu default state.
*/
void CMainFrame::SetRapidMenuDefaultState()
{
	rgbF = RGB(251, 252, 200);
	rgbB = RGB(244, 247, 251);
	rgbL = RGB(196, 219, 249);
	fontSize = 12;

	fontName = "";
}


BOOL CMainFrame::IsTsoProcessExist()
{
	HANDLE handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (handle == NULL)
	{
		return FALSE;
	}
	PROCESSENTRY32W p32;
	p32.dwSize = sizeof(PROCESSENTRY32);
	BOOL bNext = ::Process32FirstW(handle, &p32);
	while (bNext)
	{
		if (p32.th32ProcessID == g_dwTIOProcessId && g_dwTIOProcessId != 0)
		{
			CloseHandle(handle);
			return TRUE;
		}
		bNext = ::Process32NextW(handle, &p32);
	}
	CloseHandle(handle);
	return FALSE;
}


void CMainFrame::InitCaption()
{
	CFile captionFile;
	CString szFilePath = sInstallDir + SYSTEM_RAPIDMENU_PROPERTIES;
//	MessageBox(szFilePath);
	if(captionFile.Open(szFilePath, CFile::modeRead) == 0)
	{
		g_szCaption = OFFICE;

		return;
	}

	TRY
	{
		char arBuf[200];
		UINT uBytesRead = captionFile.Read(arBuf, sizeof(arBuf) - 1);
		if(arBuf[uBytesRead - 2] == 0xD && arBuf[uBytesRead - 1] == 0xA)
		{
			arBuf[uBytesRead - 2] = NULL;
		}
		else
		{
			arBuf[uBytesRead] = NULL;
		}
		g_szCaption = CString(arBuf);
		g_szCaption.TrimRight();
	//	MessageBox(g_szCaption);
	}
	CATCH(CFileException, e) 
	{
		g_szCaption = OFFICE;
	}
	END_CATCH
	//
	captionFile.Close();
}

void CMainFrame::GetUserAppDataLocation(unsigned char* appDir)
{
	HKEY hSubKey;
	DWORD dwSize = 1024;
	::RegOpenKeyExA(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
		0, KEY_READ, &hSubKey);
	::RegQueryValueExA(hSubKey, "AppData", NULL, NULL, appDir, &dwSize);
	::RegCloseKey(hSubKey);
}

BOOL CMainFrame::SendCommand(const wchar_t* command)
{
	//启动主程序, 传递参数后，直接运行。
	TCHAR exePath[MAX_NAME_SIZE] = {0};
	GetModuleFileName(NULL, exePath, MAX_NAME_SIZE-1);
	_tcsrchr(exePath, '\\')[0] = 0;
	//MessageBox(exePath);
	CString param = command;
	wsprintf(exePath, _T("%s\\%s"), exePath, _T(TS_OFFICE_EXE));
	//点击X，直接退出。
	CString open = "open";
	ShellExecute(NULL,
		open, exePath,
		param, NULL, SW_MAXIMIZE);

	return TRUE;
}

/*
 * 返回0 — 出错
 */
DWORD CMainFrame::GetCurrentTIOPid()
{
	if (!vecTIOPid.empty()) 
	{
		vecTIOPid.clear();
	}	

	HANDLE handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (handle == NULL)
	{
		return 0;
	}

	PROCESSENTRY32 p32;
	p32.dwSize = sizeof(PROCESSENTRY32);
	
	CString strProcessName;
	BOOL bNext = ::Process32First(handle, &p32);
	while (bNext)
	{
		strProcessName = p32.szExeFile;
		strProcessName.MakeUpper();

		if (strProcessName.Find(_T(TS_OFFICE_EXE)) != -1)
		{
			vecTIOPid.push_back(p32.th32ProcessID);
		}
		bNext = ::Process32Next(handle, &p32);
	}
	CloseHandle(handle);

	if (vecTIOPid.empty()) 
	{
		return 0;
	}

	g_bControlEnum = TRUE;
	::EnumWindows((WNDENUMPROC)EnumWindowsProc, 0);
	g_bControlEnum = FALSE;

	return g_dwTIOProcessId;
}

