//----------------------------------------------------------------------------//
// SkeletonFilePage.h                                                         //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef SKELETON_FILE_PAGE_H
#define SKELETON_FILE_PAGE_H

//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//

class CSkeletonCandidate;
//class CFaceDesc;
class CExportConfig;
class CMpeg4FDP;
#include "MRU.h"

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//
#include <afxtempl.h>
//typedef CArray<CFaceDesc*, CFaceDesc*>	FACEDESCARRAY;
 
class CSkeletonFilePage : public CPropertyPage
{
	CExportConfig *m_pExportConfig;
	CMRU m_mruECFs;
// member variables
public:
	bool bExportAllMode;
	
	CSkeletonCandidate *m_pSkeletonCandidate;

protected:
	CString m_strStep;
	//CFaceDesc *m_pFaceDesc;
	//FACEDESCARRAY *m_pFaceDesc;
	CMpeg4FDP**		mpeg4Face;
	UINT m_nDescriptionID;
	int m_stepIndex;
	int m_stepTotal;

// constructors/destructor
public:
	CSkeletonFilePage();
	virtual ~CSkeletonFilePage();

// member functions
public:
/*	BOOL* m_bExportMaterials;
	BOOL		m_bMultSelection;
	CString*	txPath;*/
	CArray<CString, CString>	face_names;
	void SetDescription(UINT nID);
	void SetSkeletonCandidate(CSkeletonCandidate *pSkeletonCandidate);
	//void SetFaceDesc(CFaceDesc *pFaceDesc);
	void SetMpeg4Face(CMpeg4FDP** fd){ mpeg4Face = fd;}
	//void SetFaceDesc(FACEDESCARRAY *pFaceDesc);
	void SetStep(int index, int total);
	void SetECF(CExportConfig* apExportConfig);

protected:
	BOOL BeginPage();
	LRESULT EndPage();
	void GetSheetDataFromECF();

// MSVC++ stuff
	//{{AFX_DATA(CSkeletonFilePage)
	enum { IDD = IDD_SKELETON_FILE_PAGE };
	//CButton	m_FDP;
	CButton	m_ExportAll;
	CComboBox	m_ECFCombo;
	CComboBox	m_ConfNameCombo;
	CComboBox	m_faceCombo;
	CComboBox	m_lruCombo;
	CStatic	m_stepStatic;
	CStatic	m_descriptionStatic;
	BOOL	m_CreateFDP;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CSkeletonFilePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSkeletonFilePage)
	afx_msg void OnAbout();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	afx_msg void OnBrowsef();
	afx_msg void OnBrowseCF();
	afx_msg void OnExportAll();
	afx_msg void OnKillfocusExpconfigfile();
	afx_msg void OnSelchangeExpconfigfile();
	afx_msg void OnKillfocusExpconfigname();
	afx_msg void OnSelchangeExpconfigname();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//----------------------------------------------------------------------------//
// MSVC++ stuff                                                               //
//----------------------------------------------------------------------------//

//{{AFX_INSERT_LOCATION}}

#endif

//----------------------------------------------------------------------------//
