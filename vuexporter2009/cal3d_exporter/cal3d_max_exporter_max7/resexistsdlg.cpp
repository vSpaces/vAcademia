// resexistsdlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\..\cal3d_max_exporter\cal3d_max_exporter.h"
#include "resexistsdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResExistsDlg dialog
extern BOOL	calexpoptAutoReplaceSameTextures;


CResExistsDlg::CResExistsDlg(CWnd* pParent /*=NULL*/, CString resName)
	: CDialog(CResExistsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResExistsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sResPath = resName;
}


void CResExistsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResExistsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResExistsDlg, CDialog)
	//{{AFX_MSG_MAP(CResExistsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResExistsDlg message handlers

void CResExistsDlg::OnOK() 
{
	// TODO: Add extra validation here
	CButton*	btn = (CButton*)GetDlgItem(IDC_HIDEWND);
	ASSERT(btn);
	m_bHideWindow = btn->GetCheck();
	CDialog::OnOK();
}

void CResExistsDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CButton*	btn = (CButton*)GetDlgItem(IDC_HIDEWND);
	ASSERT(btn);
	m_bHideWindow = btn->GetCheck();	
	CDialog::OnCancel();
}

BOOL CResExistsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CEdit*	label = (CEdit*)GetDlgItem(IDC_RESNAME);
	ASSERT(label);
	label->SetWindowText( m_sResPath);

	CButton*	btn = (CButton*)GetDlgItem(IDC_HIDEWND);
	ASSERT(btn);
	btn->SetCheck( TRUE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
