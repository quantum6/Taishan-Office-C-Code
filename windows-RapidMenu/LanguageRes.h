// LanguageRes.h: interface for the CLanguageRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LANGUAGERES_H__043D4939_AC37_4305_A0CB_F25275E8B155__INCLUDED_)
#define AFX_LANGUAGERES_H__043D4939_AC37_4305_A0CB_F25275E8B155__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLanguageRes  
{
public:
	CLanguageRes();
	virtual ~CLanguageRes();

	CString szMenu_Name;
	CString szMenu_Launch;
	CString szMenu_Exit;

	CString szDefFont;
};

#endif // !defined(AFX_LANGUAGERES_H__043D4939_AC37_4305_A0CB_F25275E8B155__INCLUDED_)
