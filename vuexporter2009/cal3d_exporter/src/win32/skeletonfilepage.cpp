//----------------------------------------------------------------------------//
// SkeletonFilePage.cpp                                                       //
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
#include "SkeletonFilePage.h"
#include "Exporter.h"
#include "SkeletonCandidate.h"
#include "FaceDesc.h"
#include "..\filesys.h"
#include "ExportConfig.h"
#include "Mpeg4FDP.h"


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

BEGIN_MESSAGE_MAP(CSkeletonFilePage, CPropertyPage)
	//{{AFX_MSG_MAP(CSkeletonFilePage)
	ON_BN_CLICKED(ID_APP_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSEF, OnBrowsef)
	ON_BN_CLICKED(IDC_BROWSEECF, OnBrowseCF)
	ON_BN_CLICKED(IDC_EXPALL, OnExportAll)
	ON_CBN_KILLFOCUS(IDC_EXPCONFIGFILE, OnKillfocusExpconfigfile)
	ON_CBN_SELCHANGE(IDC_EXPCONFIGFILE, OnSelchangeExpconfigfile)
	ON_CBN_KILLFOCUS(IDC_EXPCONFIGNAME, OnKillfocusExpconfigname)
	ON_CBN_SELCHANGE(IDC_EXPCONFIGNAME, OnSelchangeExpconfigname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CSkeletonFilePage::CSkeletonFilePage() : CPropertyPage(CSkeletonFilePage::IDD)
{
	m_nDescriptionID = IDS_NULL;
//	m_pFaceDesc = NULL;
	m_pExportConfig=NULL;
	bExportAllMode = false;
	//{{AFX_DATA_INIT(CSkeletonFilePage)
	m_CreateFDP = FALSE;
	//}}AFX_DATA_INIT
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CSkeletonFilePage::~CSkeletonFilePage()
{
}

//----------------------------------------------------------------------------//
// Initialize the page data                                                   //
//----------------------------------------------------------------------------//

BOOL CSkeletonFilePage::BeginPage()
{
	return TRUE;
}

//----------------------------------------------------------------------------//
// Data exchange callback                                                     //
//----------------------------------------------------------------------------//

void CSkeletonFilePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSkeletonFilePage)
	//DDX_Control(pDX, IDC_FDP, m_FDP);
	DDX_Control(pDX, IDC_EXPALL, m_ExportAll);
	DDX_Control(pDX, IDC_EXPCONFIGFILE, m_ECFCombo);
	DDX_Control(pDX, IDC_EXPCONFIGNAME, m_ConfNameCombo);
	DDX_Control(pDX, IDC_FACEEX, m_faceCombo);
	DDX_Control(pDX, IDC_LRU, m_lruCombo);
	DDX_Control(pDX, IDC_STEP, m_stepStatic);
	DDX_Control(pDX, IDC_DESCRIPTION, m_descriptionStatic);
	//DDX_Check(pDX, IDC_FDP, m_CreateFDP);
	//}}AFX_DATA_MAP
}

//----------------------------------------------------------------------------//
// Cleanup the page data                                                   //
//----------------------------------------------------------------------------//

LRESULT CSkeletonFilePage::EndPage()
{
	if (m_ExportAll.GetCheck()==1)
	{
		bExportAllMode=true;
		CString strECFFilename;
		m_ECFCombo.GetWindowText(strECFFilename);
		if(strECFFilename.IsEmpty() || m_pExportConfig->mslConfigs.GetCount()<1)
		{
			AfxMessageBox("Empty configuration file for Export All setting", MB_OK | MB_ICONEXCLAMATION);
			return -1;
		}
	}
	else
	{
		bExportAllMode=false;
	}

	// get the filename
	CString strFilename;
	m_lruCombo.GetWindowText(strFilename);
	if(strFilename.IsEmpty())
	{
		m_pSkeletonCandidate->IsNull( true);
	}
	else
	{
		CExporter::calexpoptUSELOCALCOORDS = FALSE;
		SECFileSystem fsys;
		TCHAR lpcStrBuffer[500];
		GetModuleFileName( AfxGetInstanceHandle(), lpcStrBuffer, 256);
		CString oExeFullName = fsys.GetFullPathName( lpcStrBuffer);
		CString oExePath = fsys.GetPath( oExeFullName);
		oExeFullName = oExePath+"files.txt";
		FILE *stream;
		LPTSTR	pStr = new char[255];
		bool bFound=false;
		if((stream = fopen( oExeFullName, "r" )) != NULL ){
			while(fgets((char*)pStr, 254, stream))	{
				pStr[strlen(pStr)-1] = '\0';
				if( strFilename == CString(pStr)) bFound = true;
				//m_lruCombo.AddString(pStr);
			}
			fclose( stream );
		}			
		if(!bFound && ((stream = fopen( oExeFullName, "a" )) != NULL) ){
			fwrite( strFilename.GetBuffer(0), strFilename.GetLength(), 1, stream);
			fwrite( "\n", 1, 1, stream);
			fclose( stream );
		}
		// save ECF MRUs
		CString strECFFilename;
		m_ECFCombo.GetWindowText(strECFFilename);
		m_mruECFs.UpdateAndSave(strECFFilename);
		// save data to ECF
		if(m_pExportConfig!=NULL){
			m_lruCombo.GetWindowText(m_pExportConfig->msCSF);
			m_faceCombo.GetWindowText(m_pExportConfig->msFace);
		}

		// create the skeleton candidate from the skeleton file
		if(!m_pSkeletonCandidate->CreateFromSkeletonFile((LPCTSTR)strFilename))
		{
			AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
			return -1;
		}
	}

	// create the skeleton candidate from the skeleton file
	/*
	CFaceDesc* ap_FaceDesc = NULL;
	for( int i=0; i < m_faceCombo.GetCount(); i++){
		//LPSTR fileName = calloc(1, 255);
		m_faceCombo.GetLBText( i, strFilename);
		ap_FaceDesc = new CFaceDesc();
		if((m_pFaceDesc != NULL)&&(strFilename != "")&&(ap_FaceDesc->Create((LPSTR)(LPCTSTR)strFilename)))
		{
			m_pFaceDesc->Add( ap_FaceDesc);
			CString	FileName = strFilename;
			  // Find the last "\" in the string and return everything after it.
			  int nIndex = FileName.Find('\\');
			  while(nIndex != -1)
			  {
				FileName = FileName.Right(FileName.GetLength() - nIndex - 1);
				nIndex = FileName.Find('\\');
			  } 
			//ap_FaceDesc->header.psavefilename = calloc(1, FileName.GetLength() + 1);
			//memcpy( ap_FaceDesc->header.psavefilename, FileName.GetBuffer(0), FileName.GetLength());
			face_names.Add( FileName);
			//AfxMessageBox(theExporter.GetLastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
			//return -1;
		}
	}
	*/
	//if( m_FDP.GetCheck() == 1)	*mpeg4Face = new CMpeg4FDP();

	return 0;
}

//----------------------------------------------------------------------------//
// About button callback                                                      //
//----------------------------------------------------------------------------//

void CSkeletonFilePage::OnAbout() 
{
	CDialog dlg(IDD_ABOUT);
	dlg.DoModal();
}

//----------------------------------------------------------------------------//
// Browse button callback                                                     //
//----------------------------------------------------------------------------//

void CSkeletonFilePage::OnBrowse() 
{
	// display file dialog
	CString strFilename;
	m_lruCombo.GetWindowText(strFilename);

	CFileDialog dlg(TRUE, "csf", strFilename, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "Cal3D Skeleton Files (*.csf)|*.csf|All Files (*.*)|*.*||", this);
	if(dlg.DoModal() != IDOK) return;

	// set new filename
	strFilename = dlg.GetPathName();
	m_lruCombo.SetWindowText(strFilename);
}

//----------------------------------------------------------------------------//
// Export all						                                          //
//----------------------------------------------------------------------------//


void CSkeletonFilePage::OnExportAll()
{
	// get property sheet
	CPropertySheet *pPropertySheet;
	pPropertySheet = dynamic_cast<CPropertySheet *>(GetParent());
	
	// set wizard buttons and text
	DWORD buttons;
	if (m_ExportAll.GetCheck()==1)
	{
		buttons |= PSWIZB_FINISH;
	}
	else
	{
		buttons = PSWIZB_NEXT;
	}

	pPropertySheet->SetWizardButtons(buttons);
}

//----------------------------------------------------------------------------//
// Dialog initialization callback                                             //
//----------------------------------------------------------------------------//

BOOL CSkeletonFilePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// set step and description text
	m_stepStatic.SetWindowText(m_strStep);
	CString str;
	str.LoadString(m_nDescriptionID);
	m_descriptionStatic.SetWindowText(str);

	m_lruCombo.AddString("L:\\Natura2\\CDRoot\\Resources\\s_man2\\skel.CSF");
	m_lruCombo.AddString("L:\\Natura2\\CDRoot\\Resources\\s_girl\\skel.CSF");
	m_lruCombo.AddString("L:\\Natura2\\CDRoot\\Resources\\s_boy\\skel.CSF");

	SECFileSystem fsys;
	TCHAR lpcStrBuffer[500];
	GetModuleFileName( AfxGetInstanceHandle(), lpcStrBuffer, 256);
	CString oExeFullName = fsys.GetFullPathName( lpcStrBuffer);
	CString oExePath = fsys.GetPath( oExeFullName);
	oExeFullName = oExePath+"files.txt";
	FILE *stream;
	LPTSTR	pStr = new char[255];
	if((stream = fopen( oExeFullName, "r" )) != NULL ){
		while(fgets((char*)pStr, 254, stream))	{
			pStr[strlen(pStr)-1] = '\0';
			m_lruCombo.AddString(pStr);
		}
		fclose( stream );
	}			

	m_mruECFs.Load("CafECFs");
	POSITION pos=m_mruECFs.GetHeadPosition();
	while(pos){
		CString sItem=m_mruECFs.GetNext(pos);
		m_ECFCombo.AddString(sItem);
	}

	if(m_pExportConfig==NULL){
		HWND hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_EXPCONFIGFILE);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_BROWSEECF);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_STATICECF);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_EXPCONFIGNAME);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_STATICCONF);
		::ShowWindow(hwndItem,SW_HIDE);
	}else{
		if(m_mruECFs.GetCount()>0){
			CString sFirstECF=m_mruECFs.GetHead();
			if(m_pExportConfig->ThereIsCAFInECF(sFirstECF)){
				m_ECFCombo.SetWindowText(sFirstECF);
				OnKillfocusExpconfigfile();
			}
		}
	}

	/*if( m_bMultSelection)
	{
		HWND hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_LRU);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_BROWSE);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_FACEEX);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_BROWSEF);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_BROWSEECF);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_FDP);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_STATIC);
		::ShowWindow(hwndItem,SW_HIDE);
		::SetDlgItemText(GetSafeHwnd(),IDC_STATIC2, "Select texture path");

		CWnd* mWnd = GetDlgItem(IDC_TEXTPATH);		mWnd->GetWindowRect( &r);
		mWnd->ShowWindow( SW_SHOW);
		mWnd->SetWindowText("..\\textures\\");
		//
		RECT r;
	
		mWnd->GetWindowRect( &r);
		r.left -= 15;	r.right -= 15;
		r.top -= 80; r.bottom -= 80;	mWnd->MoveWindow(&r, TRUE);*/
		/*mWnd = GetDlgItem(IDC_BROWSETEXT);		mWnd->GetWindowRect( &r);
		r.left -= 15;	r.right -= 15;
		r.top -= 80; r.bottom -= 80;	mWnd->MoveWindow(&r, TRUE);*/
	//}else{
		/*HWND hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_TEXTPATH);
		::ShowWindow(hwndItem,SW_HIDE);
		hwndItem=::GetDlgItem(GetSafeHwnd(),IDC_BROWSETEXT);
		::ShowWindow(hwndItem,SW_HIDE);*/
	//}


	return TRUE;
}

//----------------------------------------------------------------------------//
// Activation callback                                                        //
//----------------------------------------------------------------------------//

BOOL CSkeletonFilePage::OnSetActive() 
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

	// initialize page data
	if(!BeginPage()) return FALSE;

	return CPropertyPage::OnSetActive();
}

//----------------------------------------------------------------------------//
// Finish button callback                                                     //
//----------------------------------------------------------------------------//

BOOL CSkeletonFilePage::OnWizardFinish() 
{
	// cleanup page data
	if(EndPage() == -1) return FALSE;
	
	return CPropertyPage::OnWizardFinish();
}

//----------------------------------------------------------------------------//
// Next button callback                                                       //
//----------------------------------------------------------------------------//

LRESULT CSkeletonFilePage::OnWizardNext() 
{
	// cleanup page data
	if(EndPage() == -1) return -1;
	
	return CPropertyPage::OnWizardNext();
}

//----------------------------------------------------------------------------//
// Set the description text id                                                //
//----------------------------------------------------------------------------//

void CSkeletonFilePage::SetDescription(UINT nID)
{
	m_nDescriptionID = nID;
}

//----------------------------------------------------------------------------//
// Set the node hierarchy                                                     //
//----------------------------------------------------------------------------//

void CSkeletonFilePage::SetSkeletonCandidate(CSkeletonCandidate *pSkeletonCandidate)
{
	m_pSkeletonCandidate = pSkeletonCandidate;
}

/*void CSkeletonFilePage::SetFaceDesc(FACEDESCARRAY *pFaceDesc)
{
	m_pFaceDesc = pFaceDesc;
}*/

//----------------------------------------------------------------------------//
// Set the description text id                                                //
//----------------------------------------------------------------------------//

void CSkeletonFilePage::SetStep(int index, int total)
{
	m_stepIndex = index;
	m_stepTotal = total;

	m_strStep.Format("Step %d of %d", m_stepIndex, m_stepTotal);
}

//----------------------------------------------------------------------------//

void CSkeletonFilePage::OnBrowsef() 
{
	// display file dialog
	CString strFilename;
	m_faceCombo.GetWindowText(strFilename);

	CFileDialog dlg(TRUE, "ftx", strFilename, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, "Face Description Files (*.ftx)|*.ftx|All Files (*.*)|*.*||", this);
	if(dlg.DoModal() != IDOK) return;

	// set new filename
	strFilename = dlg.GetPathName();
	//m_faceCombo.SetWindowText(strFilename);	
	POSITION selPos = dlg.GetStartPosition();
	while( selPos != NULL)
		m_faceCombo.AddString( dlg.GetNextPathName( selPos));
}

void CSkeletonFilePage::SetECF(CExportConfig* apExportConfig){
	m_pExportConfig=apExportConfig;
}

void CSkeletonFilePage::OnBrowseCF(){
	// display file dialog
	CString strFilename;
	m_faceCombo.GetWindowText(strFilename);

	CFileDialog dlg(TRUE, "ecf", strFilename, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, "CAF Export Configuration Files (*.ecf)|*.ecf|All Files (*.*)|*.*||", this);
	if(dlg.DoModal() != IDOK) return;

	// set new filename
	strFilename = dlg.GetPathName();
	m_ECFCombo.SetWindowText(strFilename);
	OnKillfocusExpconfigfile();
}


void CSkeletonFilePage::OnKillfocusExpconfigfile() 
{
	// TODO: Add your control notification handler code here
	CString strECFFilename;
	m_ECFCombo.GetWindowText(strECFFilename);
	if(m_pExportConfig->SetECFName(strECFFilename)){
		// update sheet Data
		GetSheetDataFromECF();
	}
	m_ConfNameCombo.ResetContent();
	for(int ii=0; ; ii++){
		CString sCN=m_pExportConfig->GetConfigName(ii);
		if(sCN.IsEmpty()) break;
		m_ConfNameCombo.AddString(sCN);
	}
	m_ConfNameCombo.SetWindowText(m_pExportConfig->GetConfigName());
}

void CSkeletonFilePage::OnSelchangeExpconfigfile() 
{
	// TODO: Add your control notification handler code here
	CString strECFFilename;
//	m_ECFCombo.GetWindowText(strECFFilename);
	int iIdx=m_ECFCombo.GetCurSel();
	if (iIdx == CB_ERR) return;
	m_ECFCombo.GetLBText(iIdx,strECFFilename);
	m_ECFCombo.SetWindowText(strECFFilename);
	OnKillfocusExpconfigfile();
}


void CSkeletonFilePage::OnKillfocusExpconfigname() 
{
	// TODO: Add your control notification handler code here
	// если имя не изменилось, то ничего не делать
	CString strConfName;
	m_ConfNameCombo.GetWindowText(strConfName);
	if(strConfName==m_pExportConfig->GetConfigName()) return;
	// если конфигурация в списке есть, то
	for(int iIdx=0; iIdx<m_ECFCombo.GetCount(); iIdx++){
		CString sCN;
		m_ECFCombo.GetLBText(iIdx,sCN);
		if(sCN.IsEmpty()) break;
		if(strConfName==sCN){
			m_ECFCombo.SetCurSel(iIdx);
			return;
		}
	}
	// а если нет, то
	m_pExportConfig->SetConfigName(strConfName);
}

void CSkeletonFilePage::OnSelchangeExpconfigname() 
{
	// TODO: Add your control notification handler code here
	CString strConfName;
	int iIdx=m_ConfNameCombo.GetCurSel();
	if (iIdx == CB_ERR) return;
	m_ConfNameCombo.GetLBText(iIdx,strConfName);
	// считать поля из ECF-file
	if(m_pExportConfig->SetConfigName(strConfName)){
		// обновить поля в этом Sheet-е
		GetSheetDataFromECF();
	}
}

void CSkeletonFilePage::GetSheetDataFromECF(){
	m_lruCombo.SetWindowText(m_pExportConfig->msCSF);
	m_faceCombo.SetWindowText(m_pExportConfig->msFace);
}
