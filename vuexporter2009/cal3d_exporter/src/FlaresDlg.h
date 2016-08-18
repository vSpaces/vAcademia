#if !defined(AFX_FLARESDLG_H__C60D6352_8501_4A0D_B35E_CBB6E122B0A8__INCLUDED_)
#define AFX_FLARESDLG_H__C60D6352_8501_4A0D_B35E_CBB6E122B0A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlaresDlg.h : header file
//
#include "maxflare.h"
/////////////////////////////////////////////////////////////////////////////
// CFlaresDlg dialog

class CSceneFlaresDlg : public CDialog
{
// Construction
public:
	void UpdateDialogProperties();
	void UpdatePlugProperties();
	CSceneFlaresDlg(CWnd* pParent = NULL);   // standard constructor

	int get_sel_indices(int** pi, bool show_ex=false);
// Dialog Data
	//{{AFX_DATA(CSceneFlaresDlg)
	enum { IDD = IDD_FLARES };
	CListBox	m_ListFlares;
	float	m_Size;
	float	m_Dist;
	//}}AFX_DATA
	std::vector<CMaxFlare*>*	flares;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneFlaresDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSceneFlaresDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeList1();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButton1();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLARESDLG_H__C60D6352_8501_4A0D_B35E_CBB6E122B0A8__INCLUDED_)
