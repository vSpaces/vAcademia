//----------------------------------------------------------------------------//
// MeshExportSheet.cpp                                                        //
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
#include "MeshExportSheet.h"
#include "FaceDesc.h"
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

BEGIN_MESSAGE_MAP(CMeshExportSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMeshExportSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CMeshExportSheet::CMeshExportSheet(UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage) : CPropertySheet(nIDCaption, pParentWnd, 0)
{
	m_bMultSelection = iSelectPage;
	FillPages();
}

CMeshExportSheet::CMeshExportSheet(LPCTSTR pszCaption, CWnd *pParentWnd, UINT iSelectPage) : CPropertySheet(pszCaption, pParentWnd, 0)
{
	m_bMultSelection = iSelectPage;
	FillPages();
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMeshExportSheet::~CMeshExportSheet()
{
}

//----------------------------------------------------------------------------//
// Fill in all propoerty pages                                                //
//----------------------------------------------------------------------------//

void CMeshExportSheet::FillPages()
{
	// add all property pages to this sheet
	if( !m_bMultSelection)
	{
		m_skeletonFilePage.SetStep(1, 5);
		m_skeletonFilePage.SetDescription(IDS_MESH_EXPORT_DESCRIPTION);
		AddPage(&m_skeletonFilePage);

		m_mtrlPropsPage.SetStep(2, 5);
		m_mtrlPropsPage.SetDescription(IDS_MESH_EXPORT_DESCRIPTION5);
		AddPage(&m_mtrlPropsPage);

		m_boneAssignmentPage.SetStep(3, 5);
		m_boneAssignmentPage.SetDescription(IDS_MESH_EXPORT_DESCRIPTION2);
		AddPage(&m_boneAssignmentPage);

		m_levelOfDetailPage.SetStep(4, 5);
		m_levelOfDetailPage.SetDescription(IDS_MESH_EXPORT_DESCRIPTION3);
		AddPage(&m_levelOfDetailPage);

		m_springSystemPage.SetStep(5, 5);
		m_springSystemPage.SetDescription(IDS_MESH_EXPORT_DESCRIPTION4);
		AddPage(&m_springSystemPage);
	}else
	{
		m_mtrlPropsPage.SetStep(1, 1);
		m_mtrlPropsPage.SetDescription(IDS_MESH_EXPORT_DESCRIPTION5);
		AddPage(&m_mtrlPropsPage);
	}
}

//----------------------------------------------------------------------------//
// Set the mesh candidate                                                     //
//----------------------------------------------------------------------------//

void CMeshExportSheet::SetMeshCandidate(CMeshCandidate *pMeshCandidate)
{
	m_boneAssignmentPage.SetMeshCandidate(pMeshCandidate);
	m_levelOfDetailPage.SetMeshCandidate(pMeshCandidate);
	m_springSystemPage.SetMeshCandidate(pMeshCandidate);
}

//----------------------------------------------------------------------------//
// Set the node hierarchy                                                     //
//----------------------------------------------------------------------------//

void CMeshExportSheet::SetSkeletonCandidate(CSkeletonCandidate *pSkeletonCandidate)
{
	m_skeletonFilePage.SetSkeletonCandidate(pSkeletonCandidate);
	m_boneAssignmentPage.SetSkeletonCandidate(pSkeletonCandidate);
}

//----------------------------------------------------------------------------//

/*void CMeshExportSheet::SetFaceDesc(FACEDESCARRAY *fd)
{
	m_skeletonFilePage.SetFaceDesc(fd);
}*/

CSkeletonFilePage* CMeshExportSheet::GetSkelPage()
{
	return &m_skeletonFilePage;
}

bool CMeshExportSheet::SaveConfig()
{
	return m_boneAssignmentPage.bSaveConfig; 
}

void CMeshExportSheet::SetMultSelectMode(BOOL b)
{
	m_bMultSelection = b;
	m_mtrlPropsPage.m_bMultSelection = b;
}

/*void CMeshExportSheet::SetTxPath(CString *path)
{
	m_mtrlPropsPage.txPath = path;
}*/

void CMeshExportSheet::SetMaterialExport(BOOL *b)
{
#ifdef MAX3DS_BUILD
	ATLASSERT( FALSE);
#endif
	//m_mtrlPropsPage.m_bExportMaterials = b;
}

void CMeshExportSheet::SetCFGKeep(BOOL *b)
{
	m_mtrlPropsPage.m_bKeepConfig = b;

}

void CMeshExportSheet::SetColorOverrides(BOOL *overAmbient, BOOL *overDiffuse, DWORD *dwAmbient, DWORD *dwDiffuse)
{
	m_mtrlPropsPage.m_overAmbient = overAmbient;
	m_mtrlPropsPage.m_overDiffuse = overDiffuse;
	m_mtrlPropsPage.m_dwAmbient = dwAmbient;
	m_mtrlPropsPage.m_dwDiffuse = dwDiffuse;
}