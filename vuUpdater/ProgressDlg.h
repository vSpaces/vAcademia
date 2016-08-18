// ProgressDlg.h: interface for the CProgressDlg class.
//
//////////////////////////////////////////////////////////////////////
/*
#if !defined(AFX_PROGRESSDLG_H__019BA76D_6AED_4B0F_ADA1_8AB1D4BB9325__INCLUDED_)
#define AFX_PROGRESSDLG_H__019BA76D_6AED_4B0F_ADA1_8AB1D4BB9325__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H 

#include "resource.h"
#include "atlwin.h"

class CProgressDlg : public CDialogImpl<CProgressDlg>
{
public:
	//enum	{IDD = IDD_DIALOGBAR};
	enum	{IDD = IDC_PROGRESS1};

	CProgressDlg();
	virtual ~CProgressDlg();

	BEGIN_MSG_MAP(CProgressDlg)
	END_MSG_MAP()
};

#endif // !defined(AFX_PROGRESSDLG_H__019BA76D_6AED_4B0F_ADA1_8AB1D4BB9325__INCLUDED_)
