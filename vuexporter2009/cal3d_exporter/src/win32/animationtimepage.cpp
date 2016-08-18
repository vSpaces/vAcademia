//----------------------------------------------------------------------------//
// AnimationTimePage.cpp                                                      //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "AnimationTimePage.h"
#include "ExportConfig.h"
#include "Exporter.h"
#include "resource.h"

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------------------//
// Message mapping                                                            //
//----------------------------------------------------------------------------//

BEGIN_MESSAGE_MAP(CAnimationTimePage, CPropertyPage)
	//{{AFX_MSG_MAP(CAnimationTimePage)
	ON_BN_CLICKED(ID_APP_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_SAVEECF, OnSaveECF)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CAnimationTimePage::CAnimationTimePage() : CPropertyPage(CAnimationTimePage::IDD)
{
	m_nDescriptionID = IDS_NULL;

	//{{AFX_DATA_INIT(CAnimationTimePage)
	//}}AFX_DATA_INIT
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CAnimationTimePage::~CAnimationTimePage()
{
}

//----------------------------------------------------------------------------//
// Initialize the page data                                                   //
//----------------------------------------------------------------------------//

BOOL CAnimationTimePage::BeginPage()
{
	((CButton*)GetDlgItem(IDC_STAY_ATLAST))->SetCheck(CExporter::calexpoptSTAYATLAST);
	((CButton*)GetDlgItem(IDC_ZPOSITION))->SetWindowText(_T("0"));
	((CButton*)GetDlgItem(IDC_OPTIMIZEANIMATIONS))->SetCheck(CExporter::calexpoptOptimizeAnimations);
	
	//m_AnimFloor = calexportFLOORZPOS;
	return TRUE;
}

//----------------------------------------------------------------------------//
// Data exchange callback                                                     //
//----------------------------------------------------------------------------//

void CAnimationTimePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CAnimationTimePage)
	DDX_Control(pDX, IDC_COMMON, m_Common);
	DDX_Control(pDX, IDC_SAVELAST, m_SaveLast);
	DDX_Control(pDX, IDC_SAVEFIRST, m_SaveFirst);
	DDX_Control(pDX, IDC_RELANIM, m_RelativeAnim);
	DDX_Control(pDX, IDC_START_FRAME, m_startFrameEdit);
	DDX_Control(pDX, IDC_FPS, m_fpsEdit);
	DDX_Control(pDX, IDC_END_FRAME, m_endFrameEdit);
	DDX_Control(pDX, IDC_DISPLACEMENT, m_displacementEdit);
	DDX_Control(pDX, IDC_STEP, m_stepStatic);
	DDX_Control(pDX, IDC_DESCRIPTION, m_descriptionStatic);
	//DDX_Text(pDX, IDC_ZPOSITION, m_AnimFloor);
	//DDV_MinMaxFloat(pDX, m_AnimFloor, -100000.0f, 100000.0f);
	//}}AFX_DATA_MAP
}

//----------------------------------------------------------------------------//
// Cleanup the page data                                                   //
//----------------------------------------------------------------------------//

LRESULT CAnimationTimePage::EndPage()
{
	// get the time values
	CString strValue;

	m_startFrameEdit.GetWindowText(strValue);
	m_startFrame = atoi(strValue);

	m_endFrameEdit.GetWindowText(strValue);
	m_endFrame = atoi(strValue);

	m_displacementEdit.GetWindowText(strValue);
	m_displacement = atoi(strValue);

	m_fpsEdit.GetWindowText(strValue);
	m_fps = atoi(strValue);

	m_bRelativeAnim = m_RelativeAnim.GetCheck() == 1 ? true:false;
	m_bSaveFirst = m_SaveFirst.GetCheck() == 1 ? true:false;
	m_bSaveLast = m_SaveLast.GetCheck() == 1 ? true:false;
	m_bCommon = m_Common.GetCheck() == 1 ? true:false;
	CExporter::calexpoptSTAYATLAST = ((CButton*)GetDlgItem(IDC_STAY_ATLAST))->GetCheck() == 1 ? TRUE:FALSE;
	CExporter::calexpoptKEEPFRAME = ((CButton*)GetDlgItem(IDC_WALKANIMATION))->GetCheck() == 1 ? TRUE:FALSE;
	CExporter::calexpoptKEEPROTATIONSINANIMATION = ((CButton*)GetDlgItem(IDC_KEEPRTNSINANIMATION))->GetCheck() == 1 ? TRUE:FALSE;
	
 
	((CEdit*)GetDlgItem(IDC_ZPOSITION))->GetWindowText(strValue);
	CExporter::calexportFLOORZPOS = atof(strValue);

	CExporter::calexpoptOptimizeAnimations = ((CButton*)GetDlgItem(IDC_OPTIMIZEANIMATIONS))->GetCheck();

	return 0;
}

//----------------------------------------------------------------------------//
// Get the displacement in frames                                             //
//----------------------------------------------------------------------------//

int CAnimationTimePage::GetDisplacement()
{
	return m_displacement;
}

//----------------------------------------------------------------------------//
// Get the end frame                                                          //
//----------------------------------------------------------------------------//

int CAnimationTimePage::GetEndFrame()
{
	return m_endFrame;
}

//----------------------------------------------------------------------------//
// Get the frames per second (Fps)                                            //
//----------------------------------------------------------------------------//

int CAnimationTimePage::GetFps()
{
	return m_fps;
}

//----------------------------------------------------------------------------//
// Get the start frame                                                        //
//----------------------------------------------------------------------------//

int CAnimationTimePage::GetStartFrame()
{
	return m_startFrame;
}

//----------------------------------------------------------------------------//
// About button callback                                                      //
//----------------------------------------------------------------------------//

void CAnimationTimePage::OnAbout() 
{
	CDialog dlg(IDD_ABOUT);
	dlg.DoModal();
}

//----------------------------------------------------------------------------//
// Dialog initialization callback                                             //
//----------------------------------------------------------------------------//

BOOL CAnimationTimePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// set step and description text
	m_stepStatic.SetWindowText(m_strStep);
	CString str;
	str.LoadString(m_nDescriptionID);
	m_descriptionStatic.SetWindowText(str);

	//
	// set the time values
	CString strValue;

	if(m_pExportConfig!=NULL){
		m_startFrame=m_pExportConfig->miStartFrame;
		m_endFrame=m_pExportConfig->miEndFrame;
		m_displacement=m_pExportConfig->miDispl;
		m_fps=m_pExportConfig->miFrameRate;
		if(m_pExportConfig->mbMakeRel)
			m_RelativeAnim.SetCheck(1);
		if(m_pExportConfig->mbSaveFirst)
			m_SaveFirst.SetCheck(1);
		if(m_pExportConfig->mbSaveLast)
			m_SaveLast.SetCheck(1);
	}
	strValue.Format("%d", m_startFrame);
	m_startFrameEdit.SetWindowText(strValue);

	strValue.Format("%d", m_endFrame);
	m_endFrameEdit.SetWindowText(strValue);

	strValue.Format("%d", m_displacement);
	m_displacementEdit.SetWindowText(strValue);

	strValue.Format("%d", m_fps);
	m_fpsEdit.SetWindowText(strValue);

	return TRUE;
}

//----------------------------------------------------------------------------//
// Activation callback                                                        //
//----------------------------------------------------------------------------//

BOOL CAnimationTimePage::OnSetActive() 
{
	// get property sheet
	CPropertySheet *pPropertySheet;
	pPropertySheet = dynamic_cast<CPropertySheet *>(GetParent());

	// set wizard buttons and text
	DWORD buttons;
	buttons = PSWIZB_NEXT;

	if(m_stepIndex == m_stepTotal) buttons |= PSWIZB_FINISH;
	if(m_stepIndex > 1) buttons |= PSWIZB_BACK;

	pPropertySheet->SetWizardButtons(buttons);

	// initialize pafe data
	if(!BeginPage()) return FALSE;

	return CPropertyPage::OnSetActive();
}

//----------------------------------------------------------------------------//
// Finish button callback                                                     //
//----------------------------------------------------------------------------//

BOOL CAnimationTimePage::OnWizardFinish() 
{
	// cleanup page data
	if(EndPage() == -1) return FALSE;

	return CPropertyPage::OnWizardFinish();
}

//----------------------------------------------------------------------------//
// Next button callback                                                       //
//----------------------------------------------------------------------------//

LRESULT CAnimationTimePage::OnWizardNext() 
{
	// cleanup page data
	if(EndPage() == -1) return -1;
	
	return CPropertyPage::OnWizardNext();
}

//----------------------------------------------------------------------------//
// Set the animation time values                                              //
//----------------------------------------------------------------------------//

void CAnimationTimePage::SetAnimationTime(int startFrame, int endFrame, int displacement, int fps)
{
	m_startFrame = startFrame;
	m_endFrame = endFrame;
	m_displacement = displacement;
	m_fps = fps;
}

//----------------------------------------------------------------------------//
// Set the description text id                                                //
//----------------------------------------------------------------------------//

void CAnimationTimePage::SetDescription(UINT nID)
{
	m_nDescriptionID = nID;
}

//----------------------------------------------------------------------------//
// Set the description text id                                                //
//----------------------------------------------------------------------------//

void CAnimationTimePage::SetStep(int index, int total)
{
	m_stepIndex = index;
	m_stepTotal = total;

	m_strStep.Format("Step %d of %d", m_stepIndex, m_stepTotal);
}

//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Save export configuration file
//----------------------------------------------------------------------------//
void CAnimationTimePage::OnSaveECF(){
	// ??
}
