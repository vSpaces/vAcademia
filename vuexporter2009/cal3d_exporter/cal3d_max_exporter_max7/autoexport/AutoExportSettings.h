#if !defined(AFX_AUTOEXPORTSETTINGS_H__3197759A_1C08_4E64_9769_4D9B0A675443__INCLUDED_)
#define AFX_AUTOEXPORTSETTINGS_H__3197759A_1C08_4E64_9769_4D9B0A675443__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoExportSettings.h : header file
//

#include "RegKeyFilesDialogs.h"
/////////////////////////////////////////////////////////////////////////////
// CAutoExportSettings dialog
//#define	REGKEY_DBEDITORPATH	"DBEditorPath"
//#define	REGKEY_SERVERPATH	"ServerPath"
//#define	REGKEY_MODULENAME	"ModuleName"
//#define	REGKEY_WORLDX		"WorldX"
//#define	REGKEY_WORLDY		"WorldY"

#define REGKEY_PARSERURL		"ParserUrl"
#define REGKEY_GEOMETRYPATH		"GeometryPath"
#define REGKEY_HPOLY			"HPoly"
#define REGKEY_ServerID			"ServerID"
#define REGKEY_GEOMETRYONLY		"GeometryOnly"
#define REGKEY_WITHOUTTEXTURES	"WithoutTextures"

class CAutoExportSettings : public CDialog
{
// Construction
public:
	CAutoExportSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoExportSettings)
	enum { IDD = IDD_AUDOEXPORTSETTINGSDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

//	static CString GetBDEditorPath();
//	static CString GetServerPath();
//	static CString GetModuleName();
//	static CString GetWorldXText();
//	static CString GetWorldYText();
//	static long		GetWorldX();
//	static long		GetWorldY();

	static CString GetParserUrl();
	static CString GetGeometryPath();
	static BOOL GetHPoly();
	static BOOL GetWithoutTextures();
	static BOOL GetGeometryOnly();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoExportSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoExportSettings)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	virtual void OnCancel();
//	afx_msg void OnButton1();
	afx_msg void OnButton5();
	afx_msg void OnServerSwitch77();
	afx_msg void OnServerSwitch78();
	afx_msg void OnServerSwitchOther();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	bool	CheckWorldCoordinates();

	CRegManager	manager;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOEXPORTSETTINGS_H__3197759A_1C08_4E64_9769_4D9B0A675443__INCLUDED_)
