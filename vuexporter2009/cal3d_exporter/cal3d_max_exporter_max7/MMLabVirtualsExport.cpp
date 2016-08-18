//----------------------------------------------------------------------------//
// MMLabVirtualExport.cpp                                                     //

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "MMLabVirtualsExport.h"
#include "Exporter.h"
#include "MaxInterface.h"

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CMMLabVirtualExport::CMMLabVirtualExport()
{
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMMLabVirtualExport::~CMMLabVirtualExport()
{
}

//----------------------------------------------------------------------------//
// Following methods have to be implemented to make it a valid plugin         //
//----------------------------------------------------------------------------//

const TCHAR *CMMLabVirtualExport::AuthorName()
{
	return _T("Alex MMLab");
}

const TCHAR *CMMLabVirtualExport::CopyrightMessage()
{
	return _T("Copyright (C) 2007 Alex MMLab");
}

int CMMLabVirtualExport::DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// create an export interface for 3d studio max
	CMaxInterface maxInterface;
	if(!maxInterface.Create(ei, i))
	{
		AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	// create an exporter instance
	if(!theExporter.Create(&maxInterface))
	{
		AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	// export the animation
	CExporter::calexprotAutomatedExport = FALSE;
	CExporter::calexpoptGenerateGlobalCoords = FALSE;
	if(!theExporter.ExportTownMesh(std::string(name)))
	{
		AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	return 1;
}

const TCHAR *CMMLabVirtualExport::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("mml");
	default:
		return _T("");
	}
}

int CMMLabVirtualExport::ExtCount()
{
	return 1;
}

const TCHAR *CMMLabVirtualExport::LongDesc()
{
	return _T("MMLab VESpace File");
}

const TCHAR *CMMLabVirtualExport::OtherMessage1()
{
	return _T("");
}

const TCHAR *CMMLabVirtualExport::OtherMessage2()
{
	return _T("");
}

const TCHAR *CMMLabVirtualExport::ShortDesc()
{
	return _T("MMLab VESpace File");
}

void CMMLabVirtualExport::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog dlg(IDD_ABOUT);
	dlg.DoModal();
}

unsigned int CMMLabVirtualExport::Version()
{
	return	1;
}

//----------------------------------------------------------------------------//
