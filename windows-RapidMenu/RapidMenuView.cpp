// RapidMenuView.cpp : implementation of the CRapidMenuView class
//

#include "stdafx.h"
#include "RapidMenu.h"

#include "RapidMenuDoc.h"
#include "RapidMenuView.h"

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuView

IMPLEMENT_DYNCREATE(CRapidMenuView, CView)

BEGIN_MESSAGE_MAP(CRapidMenuView, CView)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuView construction/destruction

CRapidMenuView::CRapidMenuView()
{
	// TODO: add construction code here

}

CRapidMenuView::~CRapidMenuView()
{
}

BOOL CRapidMenuView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuView drawing

void CRapidMenuView::OnDraw(CDC* pDC)
{
	CRapidMenuDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuView diagnostics

#ifdef _DEBUG
void CRapidMenuView::AssertValid() const
{
	CView::AssertValid();
}

void CRapidMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRapidMenuDoc* CRapidMenuView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRapidMenuDoc)));
	return (CRapidMenuDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRapidMenuView message handlers
