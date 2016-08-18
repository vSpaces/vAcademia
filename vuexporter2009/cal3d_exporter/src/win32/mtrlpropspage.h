#if !defined(AFX_MTRLPROPSPAGE_H__A97287F0_6E3D_48C3_BB4D_ECACF1C53F1C__INCLUDED_)
#define AFX_MTRLPROPSPAGE_H__A97287F0_6E3D_48C3_BB4D_ECACF1C53F1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// mtrlpropspage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMtrlPropsPage dialog

class CMtrlPropsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMtrlPropsPage)

	BOOL*		m_bKeepConfig;
	BOOL		m_bMultSelection;
	//CString*	txPath;
	BOOL		*m_overAmbient, *m_overDiffuse;
	DWORD		*m_dwAmbient, *m_dwDiffuse;

// Construction
public:
	CMtrlPropsPage();
	~CMtrlPropsPage();
	void SetDescription(UINT nID);
	void SetStep(int index, int total);

// Dialog Data
	//{{AFX_DATA(CMtrlPropsPage)
	enum { IDD = IDD_MATERIALPROPSDLG };
	CButton	m_CheckDif;
	CButton	m_CheckAmb;
	CStatic	m_descriptionStatic;
	CStatic	m_stepStatic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMtrlPropsPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL BeginPage();
	LRESULT EndPage();

	CString m_strStep;
	UINT m_nDescriptionID;
	int m_stepIndex;
	int m_stepTotal;
	// Generated message map functions
	//{{AFX_MSG(CMtrlPropsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MTRLPROPSPAGE_H__A97287F0_6E3D_48C3_BB4D_ECACF1C53F1C__INCLUDED_)
