// LanguageResFactory.cpp: implementation of the CLanguageResFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RapidMenu.h"
#include "LanguageResFactory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLanguageResFactory::CLanguageResFactory()
{
	//m_pLanguage = NULL;
}

CLanguageResFactory::~CLanguageResFactory()
{
	/*
	if(m_pLanguage != NULL)
	{
		delete m_pLanguage;
	}
	*/
}

//Get Language Resource Object
CLanguageRes* CLanguageResFactory::GetLanguageRes(CString szLan)
{
	if(szLan.CompareNoCase(_T("SCH")) == 0)
	{
		return new CSCHRes();
	}
	else if(szLan.CompareNoCase(_T("TCH")) == 0)
	{
		return new CTCHRes();
	}
	else if(szLan.CompareNoCase(_T("ENG")) == 0)
	{
		return new CENGRes();
	}
	else if(szLan.CompareNoCase(_T("JAP")) == 0)
	{
		return new CJAPRes();
	}
	else
	{
		return NULL;
	}

}
