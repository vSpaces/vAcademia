// AutoExportSettings.cpp : implementation file
//

#include "stdafx.h"
#include "AutoExportSettings.h"
#include "RegKeyFilesDialogs.h"
#include "shlobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoExportSettings dialog


CAutoExportSettings::CAutoExportSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoExportSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoExportSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAutoExportSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoExportSettings)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoExportSettings, CDialog)
	//{{AFX_MSG_MAP(CAutoExportSettings)
	ON_WM_SHOWWINDOW()
	//ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_SERVER77, OnServerSwitch77)
	ON_BN_CLICKED(IDC_SERVER78, OnServerSwitch78)
	ON_BN_CLICKED(IDC_SERVER_OTHER, OnServerSwitchOther)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoExportSettings message handlers
/*
CString CAutoExportSettings::GetBDEditorPath()
{
	CRegManager	manager;
	CString sBDEditorPath;
	manager.ReadValue(REGKEY_DBEDITORPATH, sBDEditorPath);
	return sBDEditorPath;
}

CString CAutoExportSettings::GetServerPath()
{
	CRegManager	manager;
	CString sServerPath;
	manager.ReadValue(REGKEY_SERVERPATH, sServerPath);
	return sServerPath;
}

CString CAutoExportSettings::GetModuleName()
{
	CRegManager	manager;
	CString sServerPath;
	manager.ReadValue(REGKEY_MODULENAME, sServerPath);
	return sServerPath;
}

CString	CAutoExportSettings::GetWorldXText()
{
	CRegManager	manager;
	CString sWorldX="0";
	manager.ReadValue(REGKEY_WORLDX, sWorldX);
	return sWorldX;
}
*/
CString CAutoExportSettings::GetGeometryPath()
{
	CRegManager manager;
	CString sGeometryPath;
	if(!manager.ReadValue(REGKEY_GEOMETRYPATH, sGeometryPath))
	{
		sGeometryPath = "V:\\home\\virtuniver.loc\\www\\vu\\root\\files\\vu2008-02-20\\geometry\\";
	}

	// check on last slash in path
	if(sGeometryPath.GetAt(sGeometryPath.GetLength()-1) != '\\')
	{
		sGeometryPath += "\\";
	}

	return sGeometryPath;
}

CString CAutoExportSettings::GetParserUrl()
{
	CRegManager manager;
	CString sParserUrl;
	if(!manager.ReadValue(REGKEY_PARSERURL, sParserUrl))
	{
		sParserUrl = "192.168.0.77/parser.php";
	}
	return sParserUrl;
}

BOOL CAutoExportSettings::GetHPoly()
{
	CRegManager manager;	
	CString sHPoly;
	if(!manager.ReadValue(REGKEY_HPOLY, sHPoly))
	{
		return 0;
	}
	else
	{
		if (sHPoly.Compare("1") == 0)
		{
			return 1;
		}
	}

	return 0;
}

BOOL CAutoExportSettings::GetGeometryOnly()
{
	CRegManager manager;	
	CString sGeometryOnly;
	if(!manager.ReadValue(REGKEY_GEOMETRYONLY, sGeometryOnly))
	{
		return 0;
	}
	else
	{
		if (sGeometryOnly.Compare("1") == 0)
		{
			return 1;
		}
	}

	return 0;
}

BOOL CAutoExportSettings::GetWithoutTextures()
{
	CRegManager manager;	
	CString sWithoutTextures;
	if(!manager.ReadValue(REGKEY_WITHOUTTEXTURES, sWithoutTextures))
	{
		return 0;
	}
	else
	{
		if (sWithoutTextures.Compare("1") == 0)
		{
			return 1;
		}
	}

	return 0;
}

/*
long	CAutoExportSettings::GetWorldX()
{
	return atol( GetWorldXText().GetBuffer(0))*100;
}

CString	CAutoExportSettings::GetWorldYText()
{
	CRegManager	manager;
	CString sWorldY="0";
	manager.ReadValue(REGKEY_WORLDY, sWorldY);
	return sWorldY;
}

long	CAutoExportSettings::GetWorldY()
{
	return atol( GetWorldYText().GetBuffer(0))*100;
}
*/
void CAutoExportSettings::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
/*	GetDlgItem(IDC_DBEDITOR)->SetWindowText( GetBDEditorPath());
	GetDlgItem(IDC_SERVER)->SetWindowText( GetServerPath());
	GetDlgItem(IDC_MODULENAME)->SetWindowText( GetModuleName());
	GetDlgItem(IDC_WORLDX)->SetWindowText( GetWorldXText());
	GetDlgItem(IDC_WORLDY)->SetWindowText( GetWorldYText());
*/
	GetDlgItem(IDC_GEOMETRYPATH)->SetWindowText(GetGeometryPath());
	GetDlgItem(IDC_PARSERURL)->SetWindowText(GetParserUrl());

	CButton* btn = (CButton*)GetDlgItem(IDC_AUTOHPOLY);
	btn->SetCheck(GetHPoly());

	btn = (CButton*)GetDlgItem(IDC_AUTOGEOMETRYONLY);
	btn->SetCheck(GetGeometryOnly());

	btn = (CButton*)GetDlgItem(IDC_AUTOWITHOUTTEXTURES);
	btn->SetCheck(GetWithoutTextures());

	CRegManager manager;
	CString sServerID;
	if(!manager.ReadValue(REGKEY_ServerID, sServerID))
	{
		sServerID = "77";
	}

	btn = (CButton*)GetDlgItem(IDC_SERVER77);
	btn->SetCheck((sServerID == "77"));
	btn = (CButton*)GetDlgItem(IDC_SERVER78);
	btn->SetCheck((sServerID == "78"));
	btn = (CButton*)GetDlgItem(IDC_SERVER_OTHER);
	btn->SetCheck((sServerID == "0"));
}
/*
bool StrIsLong( const CString& str)
{
	for (int i=0; i<str.GetLength(); i++)
		if(!((str[i]>='0' && str[i]<='9') ||
			(i==0 && str[i]=='-')))
			return false;
	return true;
}

bool CAutoExportSettings::CheckWorldCoordinates()
{
	CString sWorldX;
	GetDlgItem(IDC_WORLDX)->GetWindowText( sWorldX);
	if( !StrIsLong( sWorldX))
	{
		MessageBox("Exter correct number");
		GetDlgItem(IDC_WORLDX)->SetFocus();
		return false;
	}

	CString sWorldY;
	GetDlgItem(IDC_WORLDY)->GetWindowText( sWorldY);
	if( !StrIsLong( sWorldY))
	{
		MessageBox("Exter correct number");
		GetDlgItem(IDC_WORLDY)->SetFocus();
		return false;
	}
	return true;
}
*/
void CAutoExportSettings::OnOK() 
{
	// TODO: Add extra validation here
	//if( !CheckWorldCoordinates())
	//	return;
/*	CString sBDEditorPath;
	CString sServerPath;
	CString sModuleName;
	CString sWorldX;
	CString sWorldY;
	GetDlgItem(IDC_DBEDITOR)->GetWindowText( sBDEditorPath);
	GetDlgItem(IDC_SERVER)->GetWindowText( sServerPath);
	GetDlgItem(IDC_MODULENAME)->GetWindowText( sModuleName);
	GetDlgItem(IDC_WORLDX)->GetWindowText( sWorldX);
	GetDlgItem(IDC_WORLDY)->GetWindowText( sWorldY);
*/
	CString sGeometryPath;
	CString sParserUrl;
	CString sHPoly;
	CString sGeomOnly;
	CString sWithoutTextures;

	GetDlgItem(IDC_GEOMETRYPATH)->GetWindowText(sGeometryPath);
	GetDlgItem(IDC_PARSERURL)->GetWindowText(sParserUrl);

	CButton* btn = (CButton*)GetDlgItem(IDC_AUTOHPOLY);
	if (btn->GetCheck())
	{
		sHPoly = "1";		
	}
	else
	{
		sHPoly = "0";
	}

	btn = (CButton*)GetDlgItem(IDC_AUTOGEOMETRYONLY);
	if (btn->GetCheck())
	{
		sGeomOnly = "1";		
	}
	else
	{
		sGeomOnly = "0";
	}

	btn = (CButton*)GetDlgItem(IDC_AUTOWITHOUTTEXTURES);
	if (btn->GetCheck())
	{
		sWithoutTextures = "1";		
	}
	else
	{
		sWithoutTextures = "0";
	}

/*	manager.WriteValue(REGKEY_DBEDITORPATH, sBDEditorPath);
	manager.WriteValue(REGKEY_SERVERPATH, sServerPath);
	manager.WriteValue(REGKEY_MODULENAME, sModuleName);
	manager.WriteValue(REGKEY_WORLDX, sWorldX);
	manager.WriteValue(REGKEY_WORLDY, sWorldY);
*/
	manager.WriteValue(REGKEY_GEOMETRYPATH, sGeometryPath);
	manager.WriteValue(REGKEY_PARSERURL, sParserUrl);
	manager.WriteValue(REGKEY_HPOLY, sHPoly);
	manager.WriteValue(REGKEY_GEOMETRYONLY, sGeomOnly);
	manager.WriteValue(REGKEY_WITHOUTTEXTURES, sWithoutTextures);

	btn = (CButton*)GetDlgItem(IDC_SERVER77);
	if (btn->GetCheck())
	{
		manager.WriteValue(REGKEY_ServerID, "77");
	}

	btn = (CButton*)GetDlgItem(IDC_SERVER78);
	if (btn->GetCheck())
	{
		manager.WriteValue(REGKEY_ServerID, "78");
	}

	btn = (CButton*)GetDlgItem(IDC_SERVER_OTHER);
	if (btn->GetCheck())
	{
		manager.WriteValue(REGKEY_ServerID, "0");
	}

	CDialog::OnOK();
}

void CAutoExportSettings::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
/*
void CAutoExportSettings::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CString str;
	CFileDialog	fileDialog( true, "exe", NULL);
	if( fileDialog.DoModal() == IDOK)
	{
		CString pathName = fileDialog.GetPathName();
		GetDlgItem(IDC_DBEDITOR)->SetWindowText( pathName);
	}
}
*/
CString browse_for_folder(HWND hwnd, const char *title, UINT flags)
{
	char *strResult = NULL;
	LPITEMIDLIST lpItemIDList;
	//LPMALLOC lpMalloc;  // pointer to IMalloc
	BROWSEINFO browseInfo;
	char display_name[_MAX_PATH];
	char buffer[_MAX_PATH];
	BOOL retval = FALSE;
	CString retpath = "";



	//if (SHGetMalloc(&lpMalloc) != NOERROR)
	 //return retpath;


	browseInfo.hwndOwner = hwnd;
	// set root at Desktop
	browseInfo.pidlRoot = NULL; 
	browseInfo.pszDisplayName = display_name;
	browseInfo.lpszTitle = title;   // passed in
	browseInfo.ulFlags = flags;   // also passed in
	browseInfo.lpfn = NULL;      // not used
	browseInfo.lParam = 0;      // not used   


	if ((lpItemIDList = SHBrowseForFolder(&browseInfo)) == NULL)
		goto BAIL;


	 // Get the path of the selected folder from the
	 // item ID list.
	if (!SHGetPathFromIDList(lpItemIDList, buffer))
		goto BAIL;

	// At this point, szBuffer contains the path 
	// the user chose.
	if (buffer[0] == '\0')
		goto BAIL;

	// We have a path in szBuffer!
	// Return it.
	retpath = buffer;
	retval = TRUE;


BAIL:
	//lpMalloc->lpVtbl->Free(lpMalloc, lpItemIDList);
	//lpMalloc->lpVtbl->Release(lpMalloc);      

	// If we made it this far, SHBrowseForFolder failed.
	return retpath;
}


void CAutoExportSettings::OnButton5() 
{
	// TODO: Add your control notification handler code here
	CString sFolder = browse_for_folder( AfxGetMainWnd()->GetSafeHwnd(),
										"Выберите папку geometry на сервере",
										BIF_VALIDATE | BIF_EDITBOX);
	if( !sFolder.IsEmpty())
		GetDlgItem(IDC_GEOMETRYPATH)->SetWindowText( sFolder);
}


void CAutoExportSettings::OnServerSwitch77() 
{
	GetDlgItem(IDC_GEOMETRYPATH)->SetWindowText("V:\\home\\virtuniver.loc\\www\\vu\\root\\files\\vu2008-02-20\\geometry\\");
	GetDlgItem(IDC_PARSERURL)->SetWindowText("192.168.0.77/parser.php");
}

void CAutoExportSettings::OnServerSwitch78() 
{
	GetDlgItem(IDC_GEOMETRYPATH)->SetWindowText("U:\\home\\virtuniver.loc\\www\\vu\\root\\files\\vu2008-02-20\\geometry\\");
	GetDlgItem(IDC_PARSERURL)->SetWindowText("192.168.0.78/parser.php");
}

void CAutoExportSettings::OnServerSwitchOther() 
{
	GetDlgItem(IDC_GEOMETRYPATH)->SetWindowText("");
	GetDlgItem(IDC_PARSERURL)->SetWindowText("");
}

/*
CString	sBDEditorPath;
CString	sServerPath;
*/
