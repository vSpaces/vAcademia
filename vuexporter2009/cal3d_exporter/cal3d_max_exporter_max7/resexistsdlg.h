#if !defined(AFX_RESEXISTSDLG_H__47B0C69A_ED35_4F3D_8592_FF1B23FA2A3E__INCLUDED_)
#define AFX_RESEXISTSDLG_H__47B0C69A_ED35_4F3D_8592_FF1B23FA2A3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// resexistsdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResExistsDlg dialog

class CResExistsDlg : public CDialog
{
// Construction
public:
	CResExistsDlg(CWnd* pParent = NULL, CString resName="");   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResExistsDlg)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResExistsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	BOOL	m_bHideWindow;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CResExistsDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString	m_sResPath;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESEXISTSDLG_H__47B0C69A_ED35_4F3D_8592_FF1B23FA2A3E__INCLUDED_)
