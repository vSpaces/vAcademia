// FlaresDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\cal3d_max_exporter\cal3d_max_exporter.h"
#include "FlaresDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSceneFlaresDlg dialog


CSceneFlaresDlg::CSceneFlaresDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSceneFlaresDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSceneFlaresDlg)
	m_Size = 0.0f;
	m_Dist = 0.0f;
	//}}AFX_DATA_INIT
}


void CSceneFlaresDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSceneFlaresDlg)
	DDX_Control(pDX, IDC_LIST1, m_ListFlares);
	DDX_Text(pDX, IDC_EDIT2, m_Size);
	DDV_MinMaxFloat(pDX, m_Size, 0.1f, 100000.0f);
	DDX_Text(pDX, IDC_EDIT3, m_Dist);
	DDV_MinMaxFloat(pDX, m_Dist, 0.1f, 100000.0f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSceneFlaresDlg, CDialog)
	//{{AFX_MSG_MAP(CSceneFlaresDlg)
	ON_WM_DRAWITEM()
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDSet, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSceneFlaresDlg message handlers
int CSceneFlaresDlg::get_sel_indices(int **pi, bool show_ex)
{
	CListBox*	lb = (CListBox*)GetDlgItem(IDC_LIST1);
	int sc = lb->GetSelCount();

	if(sc <= 0)
	{
		if( show_ex)	MessageBox("Select at least one flare");
		return 0;
	}
	int* indices = (int*)malloc(sc*sizeof(int));
	lb->GetSelItems(sc, indices);

	*pi = indices;
	return sc;
}

BOOL CSceneFlaresDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	for (int i=0;i<flares->size();i++)
	{
		m_ListFlares.AddString((*flares)[i]->GetName().c_str());
	}

	((CEdit*)GetDlgItem(IDC_EDIT2))->SetWindowText("100");
	((CEdit*)GetDlgItem(IDC_EDIT3))->SetWindowText("1000");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSceneFlaresDlg::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
//	UpdatePlugProperties();
	UpdateDialogProperties();
}

void CSceneFlaresDlg::UpdatePlugProperties()
{
	if( UpdateData())
	{
		CEdit* flare_name = (CEdit*)GetDlgItem(IDC_EDIT1);

		int* inds;
		int count;
		if ( (count=get_sel_indices(&inds)) != 0)
		{
			for( int i=0; i<count; i++)
			{
				CMaxFlare*	flare = (*flares)[inds[i]];
				CString fnane;
				flare_name->GetWindowText(fnane);
				flare->flare.fname = Cal3DComString(fnane.GetBuffer(0));
				flare->flare.dist = m_Dist;
				flare->flare.size = m_Size;
			}
			free(inds);
		}
	}

	Invalidate();
}

LRESULT CSceneFlaresDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
TRACE("message: %i\n", message);
	return CDialog::WindowProc(message, wParam, lParam);
}

void CSceneFlaresDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CSceneFlaresDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdatePlugProperties();
	//CDialog::OnOK();
}

void CSceneFlaresDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CSceneFlaresDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CString filter = "PNG (*.png)|*.png|Bitmaps (*.bmp)|*.bmp||";
	CFileDialog	fd(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
	if( fd.DoModal())
	{
	//	house->load(fd.GetPathName());		
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetWindowText(fd.GetPathName());
		UpdatePlugProperties();
	}	
}

void CSceneFlaresDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSceneFlaresDlg::OnSet() 
{
	// TODO: Add your control notification handler code here
	OnOK();
	CDialog::OnOK();
}

void CSceneFlaresDlg::UpdateDialogProperties()
{
	CEdit* flare_name = (CEdit*)GetDlgItem(IDC_EDIT1);

	flare_name->EnableWindow();
	((CEdit*)GetDlgItem(IDC_EDIT2))->EnableWindow();
	((CEdit*)GetDlgItem(IDC_EDIT3))->EnableWindow();

	int* inds;
	int count;
	if ( (count=get_sel_indices(&inds)) != 0)
	{
		CMaxFlare*	flare = (*flares)[inds[0]];
		flare_name->SetWindowText(flare->flare.fname.GetBuffer());
		m_Dist = flare->flare.dist;
		m_Size = flare->flare.size;
		UpdateData(FALSE);
		free(inds);
	}
}
