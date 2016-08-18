// FLNFrame.h: interface for the CFLNFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLNFRAME_H__DDFDBC81_6717_11D5_A579_00C0DF06D8DC__INCLUDED_)
#define AFX_FLNFRAME_H__DDFDBC81_6717_11D5_A579_00C0DF06D8DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flncmmn.h"
#include "InputParameters.h"
#include "../VASEBmp.h"

class CFLNFrame
{
	typedef MP_VECTOR<CRect>	vecBlocks;
	typedef vecBlocks::iterator vecBlocksIt;
private:
	MP_VECTOR<CRect> rlChngBlcks;
	//CString sSrcFileName;
	CInputParameters* pParams;
public:
	CVASEBmp bmp;
	//CVASEBmp bmpTrueColor;
	//CString sImFileName;
public:
	CFLNFrame();
	virtual ~CFLNFrame();

	bool	GetChangedBlock( unsigned int auBlockIndex, byte* alpBits, unsigned int* apuMemSize, CRect*	apRect);
	static bool FillWinBMPFromDIB( CVASEBmp* bmp, unsigned int auWidth, unsigned int auHeight, void* lpBits, unsigned int auBPP);
	bool	LoadFromDIB( unsigned int auWidth, unsigned int auHeight, void* lpBits, unsigned int auBPP);
	// defines change blocks (rlChngBlcks)
	unsigned int DefineChangeBlocks(BOOL abFirstFrame,CVASEBmp &abmpPrev, void * lpMaskBits);
	// 
	void ClearUnChangeBlocks(CInputParameters* apParams);
private:
	// define trim rectangle
	CRect DefineTrimRect(CVASEBmp &abmp,CInputParameters* apParams);
	// for algorithm 1
	void OptimizeBlock(CRect &rc,CVASEBmp & bmp);
	int iFilledPixels(CRect &arcBlock,CVASEBmp& bmp);
	bool bFindMaxFilledBlock(CRect &arcMaxFilled,CVASEBmp &abmpc);
	void RaiseBlock(CRect &arcBlock,CVASEBmp &abmpc);
	bool bIntersectedByAnyOne(CRect &arcBlock);
	CRect* FindIntersection(CRect &arcBlock);
	// for testing:
	void OutChangeBlocks(LPCSTR lpcFileName);

	void SquareSnake();
};

#ifdef FORMAT565
#define HCRSHIFT (5+6-3)
#define HCGSHIFT (5-2)
#endif
#ifdef FORMAT555
#define HCRSHIFT (5+5-3)
#define HCGSHIFT (5-3)
#endif

#endif // !defined(AFX_FLNFRAME_H__DDFDBC81_6717_11D5_A579_00C0DF06D8DC__INCLUDED_)
