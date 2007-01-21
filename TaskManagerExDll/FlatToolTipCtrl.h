#pragma once

//Arrow drawing is ported from the FLATGUI version in Delphi.
//You are free to use and modify this as long as you don`t claim it .
//Copyright : Tomkat(.ro) 2004

// Modified by Sergey Kolomenkin, 2005
// Tooltips are more common to standart ones now:
//   Tooltips are displayed immediate under the mouse cursor.
// Arrow drawing was removed, because that silly arror
// looked like a mad compass showing some useless direction.  :-)
// Multiline tooltips are supported.

class CToolTipCtrlEx : public CToolTipCtrl
{
	DECLARE_DYNAMIC(CToolTipCtrlEx)

public:
	CToolTipCtrlEx();
	virtual ~CToolTipCtrlEx();
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
private:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
public:
	COLORREF	m_bkColor;
	COLORREF	m_leftColor;
	COLORREF	m_frameColor;
	COLORREF	m_textColor;
};
