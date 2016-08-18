#include "stdafx.h"
#include "autoexport_desc.h"
#include "autoexport.h"
#include "../cal3d_max_exporter.h"
#include "../MaxInterface.h"
#include "../exporter.h"
#include "AutoExportSettings.h"

static TownAutoExportUtility	theTownAutoExporter;

//==============================================================================
// class CMMLabTownAutoExportUtilityClassDesc
//==============================================================================
void*	CMMLabTownAutoExportUtilityClassDesc::Create(BOOL loading)
{
	theTownAutoExporter.SetInstance( m_hInstance);
	return &theTownAutoExporter;
}

int 	CMMLabTownAutoExportUtilityClassDesc::IsPublic() 
{
	return 1;
}

const TCHAR *	CMMLabTownAutoExportUtilityClassDesc::ClassName() 
{
	return _T("Autoexport to VirtualUniver server");
}

SClass_ID		CMMLabTownAutoExportUtilityClassDesc::SuperClassID() 
{
	return UTILITY_CLASS_ID;
}

Class_ID		CMMLabTownAutoExportUtilityClassDesc::ClassID() 
{
	return MAX_AUTO_EXPORT_ID;
}

const TCHAR* 	CMMLabTownAutoExportUtilityClassDesc::Category() 
{
	return _T("Game Utilities");
}

void CMMLabTownAutoExportUtilityClassDesc::SetInstance(HINSTANCE hInstance)
{ 
	m_hInstance = hInstance;
}

//==============================================================================
// class TownAutoExportUtilityClassDesc
//==============================================================================
static INT_PTR CALLBACK ApplySplitMeshesDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_COMMAND:
		switch (LOWORD(wParam))
		{
        case IDC_DOAUTOEXPORT:
            theTownAutoExporter.ApplySelected();
            break;
		case IDC_AUTOEXPORTSETTINGS:
            theTownAutoExporter.DoSettings();
            break;
		case IDC_LIBEXPORT:
			theTownAutoExporter.ExportToLib();
			break;
		case IDC_WELD:
			float	fCollapseDistance = 3.0f;
			if( MessageBox( hWnd, "No history available!!! Continue welding?", "Warning!", MB_YESNO) == IDNO)
				break;
            theTownAutoExporter.WeldSelected( fCollapseDistance);
            break;
		}
        break;
        
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
        theTownAutoExporter.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
        break;

	case WM_KEYDOWN:
		{
		int iii=0;
		}	break;
	
    default:
        return FALSE;
    }
    return FALSE;
}	

TownAutoExportUtility::TownAutoExportUtility()
{
    iu = NULL;
    ip = NULL;	
    hPanel = NULL;
}

TownAutoExportUtility::~TownAutoExportUtility()
{
}

void TownAutoExportUtility::BeginEditParams(Interface *ip,IUtil *iu) 
{
    this->iu = iu;
    this->ip = ip;
    hPanel = ip->AddRollupPage (m_hInstance,
        MAKEINTRESOURCE(IDD_AUTOEXPORTUTILITY), ApplySplitMeshesDlgProc,
        //GetString(IDS_AUTOEXPORT_MESHES), 0);
		_T("Автоэкспорт на сервер"), 0);
}

void TownAutoExportUtility::EndEditParams(Interface *ip,IUtil *iu) 
{
    this->iu = NULL;
    this->ip = NULL;
    ip->DeleteRollupPage(hPanel);
    hPanel = NULL;
}

bool	TownAutoExportUtility::ExportDirectoriesExists()
{
/*	if( CAutoExportSettings::GetBDEditorPath().IsEmpty())
	{
		MessageBox(0, "CAutoExportSettings::GetBDEditorPath().IsEmpty()", "Error!", MB_OK);
		return false;
	}
	if( CAutoExportSettings::GetServerPath().IsEmpty())
	{
		MessageBox(0, "CAutoExportSettings::GetServerPath().IsEmpty()", "Error!", MB_OK);
		return false;
	}
*/


	return true;
}

void TownAutoExportUtility::ExportToLib()
{
	// экспорт объектов в библиотеку
	CExporter::libExport = TRUE;

	ApplySelected();

	CExporter::libExport = FALSE;
}

void TownAutoExportUtility::ApplySelected()
{
	// можно сделать проверку на наличие скрипта и папки
	if( !ExportDirectoriesExists())
	{
		MessageBox( 0, "Invalid export directories. Check settings..", "Error!", MB_OK);
		DoSettings();
		if( !ExportDirectoriesExists())
		{
			MessageBox( 0, "Invalid export directories", "Error!", MB_OK);
			return;
		}
	}
	// Get temp folder name
	TCHAR szTempPath[MAX_PATH+1];	ZeroMemory( &szTempPath, sizeof(szTempPath)/sizeof(szTempPath[0]));
	TCHAR szTempPath2[MAX_PATH+1];	ZeroMemory( &szTempPath2, sizeof(szTempPath2)/sizeof(szTempPath2[0]));
	GetTempPath( MAX_PATH, szTempPath);
	strcat(szTempPath, "cal3d_autoexport");

	// Clear temp dir
	SHFILEOPSTRUCT Operation=
	{
		AfxGetMainWnd()->GetSafeHwnd(),
		FO_DELETE,szTempPath,NULL,FOF_NOCONFIRMATION|FOF_SILENT
	};
	SHFileOperation(&Operation);

	strcat(szTempPath, "\\");
	strcat(szTempPath2, szTempPath);

	strcat(szTempPath, "mesh.mml");
	strcat(szTempPath2, "mesh.xml");

	//////////////////////////////////////////////////////////////////////////
	// Do Export
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// create an export interface for 3d studio max
	CMaxInterface maxInterface;

	Interface* ip=GetCOREInterface(); //hack
	if(!maxInterface.Create(NULL, ip))
	{
		AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	// create an exporter instance
	if(!theExporter.Create(&maxInterface))
	{
		AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CExporter::calexprotAutomatedExport = TRUE;
	CExporter::calexpoptAutoexportToServer = TRUE;
	CExporter::calexpoptUSELOCALCOORDS = FALSE;
	CExporter::calexpoptEXPORTSHADOWDATA = FALSE;
	CExporter::calexpoptNORMALIZEVERTS = FALSE;
	CExporter::calexpoptAutoReplaceSameMaterials = 1;
	CExporter::calexpoptAutoReplaceSameTextures = 1;
	CExporter::calexportTexturesPath = "..\\textures\\"; //
	CExporter::calexportMaterialsPath = "..\\materials\\"; //
	CExporter::calexpoptUseTexturesPath = TRUE;
	CExporter::calexpoptUseMaterialsPath = TRUE;
	CExporter::calexpoptExportMaterials = TRUE;
	CExporter::calexpoptGetnMtrlsNames = TRUE;

	CExporter::calexpoptGenerateGlobalCoords = FALSE;

	CExporter::calexportAbsoluteTexturesPaths = FALSE;//TRUE;

	CString texPath = CExporter::calexportTexturesPath;
	CString matPath = CExporter::calexportMaterialsPath;
	//CExporter::calexportTexturesPath = "textures\\";
	//CExporter::calexportMaterialsPath = "materials\\";

	CExporter::useFloatVertexCoords = CAutoExportSettings::GetHPoly();
	CExporter::exportGeometryOnly = CAutoExportSettings::GetGeometryOnly();
	CExporter::exportWithoutTextures = CAutoExportSettings::GetWithoutTextures();
	CExporter::texturesExportQuality = 50;

	/*
	Все текстуры при экспорте складываются в одну папку.
	 1. Эта папка лежит на том же уровне что и каталоги объектов, если вложенность экспорта каталога = 1
	 Например:	[папка экспорта]\SampleObject\mesh.bmf
				[папка экспорта]\CExporter::calexportTexturesPath

	 2. Эта папка лежит на уровне ниже папки экспорта, если вложенность экспорта каталога > 1
	 Например:	[папка экспорта]\sq4\SampleGroup\SampleObject\mesh.bmf
				[папка экспорта]\sq4\CExporter::calexportTexturesPath
	*/
	
	theExporter.ExportTownMesh(std::string(szTempPath));

	CExporter::calexportAbsoluteTexturesPaths = FALSE;
	CExporter::calexprotAutomatedExport = FALSE;
	CExporter::calexpoptAutoexportToServer = FALSE;
	CExporter::calexportTexturesPath = texPath;
	CExporter::calexportMaterialsPath = matPath;
}

void TownAutoExportUtility::DoSettings()
{
	// Настройки
	/*
		Путь к BDEditor
		Путь к геометрии на сервере
	*/
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CAutoExportSettings	dialog;
	dialog.DoModal();
}

void TownAutoExportUtility::WeldSelected( float dist)
{
	CMaxInterface maxInterface;

	Interface* ip=GetCOREInterface();	//	hack
	if(!maxInterface.Create(NULL, ip))
	{
		AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	// create an exporter instance
	if(!theExporter.Create(&maxInterface))
	{
		AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	theExporter.WeldSelected( dist);
}