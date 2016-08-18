//----------------------------------------------------------------------------//
// MMLabVirtualExportDesc.cpp                                                 //


//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "MMLabVirtualsExportDesc.h"
#include "MMLabVirtualsExport.h"

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

CMMLabVirtualExportDesc::CMMLabVirtualExportDesc()
{
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMMLabVirtualExportDesc::~CMMLabVirtualExportDesc()
{
}

//----------------------------------------------------------------------------//
// Set the DLL instance handle of this plugin                                 //
//----------------------------------------------------------------------------//

void CMMLabVirtualExportDesc::SetInstance(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
}

//----------------------------------------------------------------------------//
// Following methods have to be implemented to make it a valid plugin         //
//----------------------------------------------------------------------------//

const TCHAR *CMMLabVirtualExportDesc::Category()
{
	return _T("Game Utilities");
}

Class_ID CMMLabVirtualExportDesc::ClassID()
{
	return MAX_VESPACE_EXPORT_ID;
}

const TCHAR *CMMLabVirtualExportDesc::ClassName()
{
	return _T("MMLab VESpace Export");
}

void *CMMLabVirtualExportDesc::Create(BOOL loading)
{
	return new CMMLabVirtualExport();
}

HINSTANCE CMMLabVirtualExportDesc::HInstance()
{
	return m_hInstance;
}

const TCHAR *CMMLabVirtualExportDesc::InternalName()
{
	return _T("MMLab_VESpace_Export");
} 

int CMMLabVirtualExportDesc::IsPublic()
{
	return 1;
}

SClass_ID CMMLabVirtualExportDesc::SuperClassID()
{
	return SCENE_EXPORT_CLASS_ID;
}

//----------------------------------------------------------------------------//
