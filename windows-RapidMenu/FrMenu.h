// FrMenu.h: interface for the CFrMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRMENU_H__9C72D857_918B_4F19_8F2C_F1D0E08800FE__INCLUDED_)
#define AFX_FRMENU_H__9C72D857_918B_4F19_8F2C_F1D0E08800FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct CMenuInfo
{
	UINT uMenuItemID;
	CString sText;
	double dBkBmpIndex;
};


////////////////////////////////////////////////////////////////////////////////

/*
class CMenuInfo
{
public:
	UINT uMenuItemID;
	CString sText;
};
*/


////////////////////////////////////////////////////////////////////////////////

class CFrMenu : public CMenu  
{
public:
	CFrMenu();
	virtual ~CFrMenu();
	
	//static BOOL m_bRemoveBorder;
	
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);

private:
	BOOL bGetFonts;
	int  m_nColor;
	void SetCompatibleHeight(LPMEASUREITEMSTRUCT lpMIS);
	int  GetSystemColor(int x, int n);
	INT GetCurrentOs();
};

#endif // !defined(AFX_FRMENU_H__9C72D857_918B_4F19_8F2C_F1D0E08800FE__INCLUDED_)





















