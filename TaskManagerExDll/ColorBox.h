#if !defined(AFX_COLORBOX_H__1F411462_E4B2_11D8_B14D_002018574596__INCLUDED_)
#define AFX_COLORBOX_H__1F411462_E4B2_11D8_B14D_002018574596__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorBox.h : header file
//

/*
	CColorBox is a very simple button that shows a color and
	let the user change the color by clicking on it.

    It's not more complicated than that :-).

    Get the latest version at http://www.codeproject.com

    PEK
  */
/////////////////////////////////////////////////////////////////////////////
// CColorBox window

class CColorBox : public CButton
{
// Construction
public:
	CColorBox();

// Attributes
public:

// Operations
public:
	//Set/Get the current color
	void SetColor(const COLORREF newColor);
	COLORREF GetColor() const;

	//Set/get allow change mode. When on, the user can change color
	//when he clicks on the box..
	void SetAllowChange(const BOOL allowchange);
	BOOL GetAllowChange() const;
	
	//Get/set selected mode.
	//When selected, a small frame is drawn around the box
	void SetSelected(const BOOL selected);
	BOOL GetSelected() const;
	
	//Start the color selector and let user change color.
	//Return true if color changed.
	BOOL SelectColor();

	//Set a pointer to the custom colors.
	//IMPORTANT: It must be a pointer to an array with at least 16 elements.
	//When user change color in the color selector, the colors here might be
	//changed as well.
	void SetCustomColors(COLORREF *customcolors);

	//Redraw the box
	void Redraw();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorBox();

	// Generated message map functions
protected:
	COLORREF m_color;
	BOOL m_allowChange;
	BOOL m_selected;

	COLORREF*  m_CustomColors;

	//{{AFX_MSG(CColorBox)
	afx_msg BOOL OnClickedEx();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBOX_H__1F411462_E4B2_11D8_B14D_002018574596__INCLUDED_)
