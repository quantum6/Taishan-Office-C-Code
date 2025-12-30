// RapidMenuDoc.h : interface of the CRapidMenuDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPIDMENUDOC_H__CB9CE7B8_F651_4F03_9BD6_57752DC3DB88__INCLUDED_)
#define AFX_RAPIDMENUDOC_H__CB9CE7B8_F651_4F03_9BD6_57752DC3DB88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRapidMenuDoc : public CDocument
{
protected: // create from serialization only
	CRapidMenuDoc();
	DECLARE_DYNCREATE(CRapidMenuDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRapidMenuDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRapidMenuDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRapidMenuDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAPIDMENUDOC_H__CB9CE7B8_F651_4F03_9BD6_57752DC3DB88__INCLUDED_)
