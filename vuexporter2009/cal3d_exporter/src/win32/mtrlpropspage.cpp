// mtrlpropspage.cpp : implementation file
//

#include "stdafx.h"
#include "..\..\cal3d_max_exporter\cal3d_max_exporter.h"
#include "mtrlpropspage.h"
#include "resource.h"
#include "Exporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMtrlPropsPage property page

//! return A value from ARGB format
#define A_ARGB(x)			((x&0xFF000000)>>24)
//! return R value from ARGB format
#define B_ARGB(x)			((x&0x00FF0000)>>16)
//! return G value from ARGB format
#define G_ARGB(x)			((x&0x0000FF00)>>8)
//! return B value from ARGB format
#define R_ARGB(x)			(x&0x000000FF)

#define COLOR_ARGB(x)			(x&0x000000FF)


IMPLEMENT_DYNCREATE(CMtrlPropsPage, CPropertyPage)

CMtrlPropsPage::CMtrlPropsPage() : CPropertyPage(CMtrlPropsPage::IDD)
{
	//{{AFX_DATA_INIT(CMtrlPropsPage)
	//}}AFX_DATA_INIT
}

CMtrlPropsPage::~CMtrlPropsPage()
{
	//m_dwDiffuse = m_dwAmbient = 0xFFFFFFFF;
}

void CMtrlPropsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMtrlPropsPage)
	DDX_Control(pDX, IDC_CHECKDIF, m_CheckDif);
	DDX_Control(pDX, IDC_CHECKAMB, m_CheckAmb);
	DDX_Control(pDX, IDC_DESCRIPTION, m_descriptionStatic);
	DDX_Control(pDX, IDC_STEP, m_stepStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMtrlPropsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMtrlPropsPage)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMtrlPropsPage message handlers

BOOL CMtrlPropsPage::BeginPage()
{
	CWnd* mWnd = GetDlgItem(IDC_TEXTPATH);
	ASSERT(mWnd);
	mWnd->ShowWindow( SW_SHOW);
	mWnd->SetWindowText(CExporter::calexportTexturesPath);

	mWnd = GetDlgItem(IDC_MTRLPATH);
	ASSERT(mWnd);
	mWnd->ShowWindow( SW_SHOW);
	mWnd->SetWindowText(CExporter::calexportMaterialsPath);

	m_CheckAmb.SetCheck(TRUE);
	m_CheckDif.SetCheck(TRUE);

	CButton*	btn = (CButton*)GetDlgItem(IDC_USEMATERIALSPATH);
	assert(btn);
	btn->SetCheck(CExporter::calexpoptUseMaterialsPath);

	btn = (CButton*)GetDlgItem(IDC_AUTOEXPORT);
	assert(btn);
	btn->SetCheck(CExporter::calexpoptAutoexportToServer);

	btn = (CButton*)GetDlgItem(IDC_USEFLOATVERTEX);
	assert(btn);
	btn->SetCheck(CExporter::useFloatVertexCoords);

	btn = (CButton*)GetDlgItem(IDC_USETEXTURESPATH);
	assert(btn);	
	btn->SetCheck(CExporter::calexpoptUseTexturesPath);

	btn = (CButton*)GetDlgItem(IDC_GETMTRLSNAMES);
	assert(btn);
	btn->SetCheck(CExporter::calexpoptGetnMtrlsNames);

	btn = (CButton*)GetDlgItem(IDC_GENDBID);
	assert(btn);
	btn->SetCheck(CExporter::calexportGenerateDBID);
	
	CButton*	btnMtrls = (CButton*)GetDlgItem(IDC_MATERIALS);
	assert(btnMtrls);
	btnMtrls->SetCheck( CExporter::calexpoptExportMaterials);

	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_QUALITY_SLIDER);
	assert(slider);
	slider->SetRange(1, 100);
	slider->SetPos(CExporter::texturesExportQuality);


	if( GetDlgItem(IDC_KEEPCFG))
		((CButton*)GetDlgItem(IDC_KEEPCFG))->SetCheck(TRUE);

	if( GetDlgItem(IDC_USEPREFIXES))
		 ((CButton*)GetDlgItem(IDC_USEPREFIXES))->SetCheck(CExporter::calexportUsePrefixAsDirName);
	if( GetDlgItem(IDC_SEPCOMMONOBJECTS))
		 ((CButton*)GetDlgItem(IDC_SEPCOMMONOBJECTS))->SetCheck(CExporter::calexportSeparateCommonObjects);
	if( GetDlgItem(IDC_SEPCOMMONTEXTURES))
		 ((CButton*)GetDlgItem(IDC_SEPCOMMONTEXTURES))->SetCheck(CExporter::calexportSeparateCommonTextures);
	if( GetDlgItem(IDC_USELODS))
		 ((CButton*)GetDlgItem(IDC_USELODS))->SetCheck(CExporter::calexportFindLODs);
	if( GetDlgItem(IDC_GENSQL))
		 ((CButton*)GetDlgItem(IDC_GENSQL))->SetCheck(CExporter::calexportGenerateSQL);

	if( GetDlgItem(IDC_CHECK_USE_LEFT_ASSIGMENT2RIGHT))
		((CButton*)GetDlgItem(IDC_CHECK_USE_LEFT_ASSIGMENT2RIGHT))->SetCheck(CExporter::calexprotUseLeftAssigment2Right);
	if( GetDlgItem(IDC_CHECK_USE_RIGHT_ASSIGMENT2LEFT))
		((CButton*)GetDlgItem(IDC_CHECK_USE_RIGHT_ASSIGMENT2LEFT))->SetCheck(CExporter::calexprotUseRigthAssigment2Left);


	return TRUE;
}

LRESULT CMtrlPropsPage::EndPage()
{

	CEdit*	m_TxPath = (CEdit*)GetDlgItem(IDC_TEXTPATH);
	ASSERT(m_TxPath);
	//m_TxPath->GetLine(0, txPath->GetBufferSetLength(m_TxPath->LineLength()));
	m_TxPath->GetLine(0, CExporter::calexportTexturesPath.GetBufferSetLength(m_TxPath->LineLength()));

	CEdit*	m_MtrlsPath = (CEdit*)GetDlgItem(IDC_MTRLPATH);
	ASSERT(m_MtrlsPath);
	m_MtrlsPath->GetLine(0, CExporter::calexportMaterialsPath.GetBufferSetLength(m_MtrlsPath->LineLength()));

	CButton*	btnMtrls = (CButton*)GetDlgItem(IDC_MATERIALS);
	assert(btnMtrls);
	CExporter::calexpoptExportMaterials = btnMtrls->GetCheck();

	CButton*	btn = (CButton*)GetDlgItem(IDC_USEMATERIALSPATH);
	assert(btn);
	CExporter::calexpoptUseMaterialsPath = btn->GetCheck();

	btn = (CButton*)GetDlgItem(IDC_AUTOEXPORT);
	assert(btn);
	CExporter::calexpoptAutoexportToServer = btn->GetCheck();

	btn = (CButton*)GetDlgItem(IDC_USEFLOATVERTEX);
	assert(btn);
	CExporter::useFloatVertexCoords = btn->GetCheck();

	btn = (CButton*)GetDlgItem(IDC_USETEXTURESPATH);
	assert(btn);
	CExporter::calexpoptUseTexturesPath = btn->GetCheck();

	btn = (CButton*)GetDlgItem(IDC_GETMTRLSNAMES);
	assert(btn);
	CExporter::calexpoptGetnMtrlsNames = btn->GetCheck();

	if( GetDlgItem(IDC_KEEPCFG))
		CExporter::calexpoptKeepConfig = ((CButton*)GetDlgItem(IDC_KEEPCFG))->GetCheck();

	if( (*m_overAmbient = m_CheckAmb.GetCheck()) == TRUE)
	{
	}

	if( (*m_overDiffuse = m_CheckDif.GetCheck()) == TRUE)
	{
		
	}

	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_QUALITY_SLIDER);
	assert(slider);
	CExporter::texturesExportQuality = slider->GetPos();

	if( GetDlgItem(IDC_USEPREFIXES))
		CExporter::calexportUsePrefixAsDirName = ((CButton*)GetDlgItem(IDC_USEPREFIXES))->GetCheck();
	if( GetDlgItem(IDC_SEPCOMMONOBJECTS))
		CExporter::calexportSeparateCommonObjects = ((CButton*)GetDlgItem(IDC_SEPCOMMONOBJECTS))->GetCheck();
	if( GetDlgItem(IDC_SEPCOMMONTEXTURES))
		CExporter::calexportSeparateCommonTextures = ((CButton*)GetDlgItem(IDC_SEPCOMMONTEXTURES))->GetCheck();
	if( GetDlgItem(IDC_USELODS))
		CExporter::calexportFindLODs = ((CButton*)GetDlgItem(IDC_USELODS))->GetCheck();
	if( GetDlgItem(IDC_GENSQL))
		 CExporter::calexportGenerateSQL = ((CButton*)GetDlgItem(IDC_GENSQL))->GetCheck();
	if( GetDlgItem(IDC_GENDBID))
		CExporter::calexportGenerateDBID = ((CButton*)GetDlgItem(IDC_GENDBID))->GetCheck();

	return 0;
}

BOOL CMtrlPropsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_stepStatic.SetWindowText(m_strStep);
	CString str;
	str.LoadString(m_nDescriptionID);
	m_descriptionStatic.SetWindowText(str);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMtrlPropsPage::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	// get property sheet
	CPropertySheet *pPropertySheet;
	pPropertySheet = dynamic_cast<CPropertySheet *>(GetParent());

	// set wizard buttons and text
	DWORD buttons;
	buttons = PSWIZB_NEXT;

	if(m_stepIndex == m_stepTotal) buttons |= PSWIZB_FINISH;
	if(m_stepIndex > 1) buttons |= PSWIZB_BACK;

	if( m_bMultSelection)	buttons = PSWIZB_FINISH;

	pPropertySheet->SetWizardButtons(buttons);

	// initialize page data
	if(!BeginPage()) return FALSE;

	return CPropertyPage::OnSetActive();
}

LRESULT CMtrlPropsPage::OnWizardNext() 
{
	// cleanup page data
	if(EndPage() == -1) return -1;
	
	return CPropertyPage::OnWizardNext();
}

BOOL CMtrlPropsPage::OnWizardFinish() 
{
	// cleanup page data
	if(EndPage() == -1) return FALSE;
	
	return CPropertyPage::OnWizardFinish();
}

void CMtrlPropsPage::SetDescription(UINT nID)
{
	m_nDescriptionID = nID;
}

void CMtrlPropsPage::SetStep(int index, int total)
{
	m_stepIndex = index;
	m_stepTotal = total;

	m_strStep.Format("Step %d of %d", m_stepIndex, m_stepTotal);
}

void CMtrlPropsPage::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CColorDialog	cd(*m_dwDiffuse, CC_FULLOPEN, this);
	if( cd.DoModal() != IDOK) return;
	*m_dwDiffuse = cd.GetColor();
	GetDlgItem(IDC_BUTTON1)->Invalidate();
}

void CMtrlPropsPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDCtl==IDC_BUTTON1 || nIDCtl==IDC_BUTTON2)
	{
		LOGBRUSH	lb;
		lb.lbStyle = BS_SOLID;
		if( nIDCtl==IDC_BUTTON1)
			lb.lbColor = *m_dwDiffuse;
		else
			lb.lbColor = *m_dwAmbient;
		HBRUSH	newb;
		newb = CreateBrushIndirect( &lb);
		FillRect( lpDrawItemStruct->hDC, (CONST RECT *) &lpDrawItemStruct->rcItem, newb);
		DeleteObject(newb);
		return;
	}
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMtrlPropsPage::OnButton2() 
{
	// TODO: Add your control notification handler code here
	CColorDialog	cd(*m_dwAmbient, CC_FULLOPEN, this);
	if( cd.DoModal() != IDOK) return;
	*m_dwAmbient = cd.GetColor();
	GetDlgItem(IDC_BUTTON2)->Invalidate();	
}
