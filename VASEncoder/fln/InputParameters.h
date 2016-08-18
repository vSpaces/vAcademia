// InputParameters.h: interface for the CInputParameters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTPARAMETERS_H__4975F064_663A_11D5_A579_00C0DF06D8DC__INCLUDED_)
#define AFX_INPUTPARAMETERS_H__4975F064_663A_11D5_A579_00C0DF06D8DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flncmmn.h"

class CTransparentColor{
public:
	BYTE r,g,b,d;
	bool bDefined;
public:
	CTransparentColor(){
		r=0,g=0,b=0,d=0;
		bDefined=FALSE;
	}
};

class CFLNFrame;

class CInputParameters
{
public:
	enum {cmdGiveHelp,cmdAssemble,cmdDisassemble} eCommand;
	BOOL bCompress;
	CTransparentColor oTC;
	bool bTCAuto;
	//CString	sAnimFileName;
	//CStringList slFileNames;	// frame file names
	int iBPP;
	enum {fmtPNG,fmtBMP,fmtTGA,fmtPIN} eOutFormat;
	bool bOutFormatSpecified;
	int iSpeed;
	CRect rcSrc;	// source rectangle
	BOOL bSrcByEmptyMargs;
	CRect rcTrim;	// empty fields trim rectangle
	BOOL bDebugMode;
	BOOL bPreMultiplied; // premultiplied input images
	BOOL bDontCutAnyMargins;
	int iQuality; // quality of animation (0(highest)..6(default)..9(lowest))
	BOOL bUseZlib; // use old Zlib for the compression
	BOOL bMakeAssBckBAT; // make assamble back BAT-file while animation disassambling
	BOOL bSaveAsIs; // save extracted frames 'as is' (changed pixels only)
	BOOL bVideoCompression; // 'video' compression (color reduction, LZ&Code)
	BOOL bTrueColor; // create true color animation
	int iTCCompRate; // 0 - lossless, 1 - forward(default), 2 - +back-matches, 3 - +up-matches
	BOOL bReverse; // create reversed animation
	// BOOL bOldDiffAlg; // use old color differing algorithm
	BOOL bDarking; // dark output 16(pin/fln)->32(png)
	BOOL bApplyAlphaFilter; // apply "alpha filter" for input images
	BOOL bRemoveRareNoise; // remove rare noise
public:
	CInputParameters(){
		eCommand=cmdGiveHelp;
		bCompress=TRUE;
		bTCAuto=FALSE;
		iBPP=15;
		eOutFormat=fmtPNG;
		iSpeed=100;
		rcSrc.SetRect(0,0,0,0);
		bSrcByEmptyMargs=FALSE;
		bDebugMode=FALSE;
		bPreMultiplied=FALSE;
		bDontCutAnyMargins=FALSE;
		iQuality=6;
		bUseZlib=FALSE;
		bMakeAssBckBAT=FALSE;
		bSaveAsIs=FALSE;
		bOutFormatSpecified=FALSE;
		bVideoCompression=FALSE;
		bTrueColor=FALSE;
		iTCCompRate=1;
		bReverse = FALSE;
		// bOldDiffAlg = FALSE;
		bDarking = FALSE;
		bApplyAlphaFilter = FALSE;
		bRemoveRareNoise = FALSE;
	}	
	virtual ~CInputParameters();
public:
	// parse input parameter
	ErrorCls* Parse(LPCSTR alpcsParam);
	// define: what to do by slFileNames
	ErrorCls* DefWhatToDo();
	// update itself by the frame
	ErrorCls* UpdateByFrame(CFLNFrame &aoFrame);
};

extern CInputParameters* pInputParameters;

#endif // !defined(AFX_INPUTPARAMETERS_H__4975F064_663A_11D5_A579_00C0DF06D8DC__INCLUDED_)
