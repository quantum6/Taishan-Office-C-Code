// RapidMenuDoc.cpp : implementation of the CRapidMenuDoc class
//

#include "stdafx.h"
#include "RapidMenu.h"

#include "RapidMenuDoc.h"


/////////////////////////////////////////////////////////////////////////////
// CRapidMenuDoc

IMPLEMENT_DYNCREATE(CRapidMenuDoc, CDocument)

BEGIN_MESSAGE_MAP(CRapidMenuDoc, CDocument)
	//{{AFX_MSG_MAP(CRapidMenuDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuDoc construction/destruction

CRapidMenuDoc::CRapidMenuDoc()
{
	// TODO: add one-time construction code here

}

CRapidMenuDoc::~CRapidMenuDoc()
{
}

BOOL CRapidMenuDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CRapidMenuDoc serialization

void CRapidMenuDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuDoc diagnostics

#ifdef _DEBUG
void CRapidMenuDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRapidMenuDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuDoc commands
