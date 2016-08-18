#if !defined(AFX_SCENELIGHTSDLG_H__7BF6C116_EE90_41DE_B365_6C2E8FD64BC8__INCLUDED_)
#define AFX_SCENELIGHTSDLG_H__7BF6C116_EE90_41DE_B365_6C2E8FD64BC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// scenelightsdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSceneLightsDlg dialog
class CMaxLight;

class CSceneLightsDlg : public CDialog
{
// Construction
public:
	Point3	ambient;
	bool SaveLights();
	void UpdateLightProperties();
	int get_sel_indices(int** pi, bool show_ex=false);
	CSceneLightsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSceneLightsDlg)
	enum { IDD = IDD_LIGHTS };
	CListBox	m_LightsList;
	double	m_Size;
	double	m_Distance;
	//}}AFX_DATA
	std::vector<CMaxLight*>*	lights;
	//std::vector<D3DLIGHT8>	d3d_lights;
	//std::vector<CalExporter_Flare>	d3d_flares;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneLightsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSceneLightsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSet();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeList1();
	afx_msg void OnButton1();
	afx_msg void OnButton4();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENELIGHTSDLG_H__7BF6C116_EE90_41DE_B365_6C2E8FD64BC8__INCLUDED_)
