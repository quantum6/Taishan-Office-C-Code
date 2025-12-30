// Registry.h: interface for the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRY_H__A268A6F9_9D3A_4DF8_B821_6C040A81F7DF__INCLUDED_)
#define AFX_REGISTRY_H__A268A6F9_9D3A_4DF8_B821_6C040A81F7DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegistry : public CObject
{
	//Construction
public:
	CRegistry(HKEY hKey);
	
public:
	BOOL SaveKey(LPCTSTR lpFileName);
	BOOL RestoreKey(LPCTSTR lpFileName);
	BOOL Read(LPCTSTR lpValueName, CString* lpVal);
	BOOL Read(LPCTSTR lpValueName, DWORD* pdwVal);
	BOOL Read(LPCTSTR lpValueName, int* pnVal);
	BOOL Write(LPCTSTR lpSubKey, LPCTSTR lpVal);
	BOOL Write(LPCTSTR lpSubKey, DWORD dwVal);
	BOOL Write(LPCTSTR lpSubKey, int nVal);
	BOOL DeleteKey(HKEY hKey, LPCTSTR lpSubKey);
	BOOL DeleteValue(LPCTSTR lpValueName);
	void Close();
	BOOL Open(LPCTSTR lpSubKey);
	BOOL CreateKey(LPCTSTR lpSubKey);
	virtual ~CRegistry();
	
protected:
	HKEY m_hKey;
	
};

#endif // !defined(AFX_REGISTRY_H__A268A6F9_9D3A_4DF8_B821_6C040A81F7DF__INCLUDED_)
