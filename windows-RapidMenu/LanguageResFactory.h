// LanguageResFactory.h: interface for the CLanguageResFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LANGUAGERESFACTORY_H__62BC8FCD_874D_49FB_A5A4_C1E2860E450D__INCLUDED_)
#define AFX_LANGUAGERESFACTORY_H__62BC8FCD_874D_49FB_A5A4_C1E2860E450D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LanguageRes.h"
#include "SCHRes.h"
#include "TCHRes.h"
#include "ENGRes.h"
#include "JAPRes.h"

class CLanguageResFactory  
{
public:
	CLanguageResFactory();
	virtual ~CLanguageResFactory();
	
	//++
	CLanguageRes* GetLanguageRes(CString szLan);
	//--

private:
	//CLanguageRes* m_pLanguage;
};

#endif // !defined(AFX_LANGUAGERESFACTORY_H__62BC8FCD_874D_49FB_A5A4_C1E2860E450D__INCLUDED_)
