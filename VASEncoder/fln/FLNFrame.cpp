// FLNFrame.cpp: implementation of the CFLNFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "../pixeldefs.h"
#include "FLNFrame.h"
#include <malloc.h>
#include <iostream>
#include <mmsystem.h>
#include <assert.h>

CInputParameters *pInputParameters = MP_GLOBAL_NEW( CInputParameters);

#define OUTDEBUGBMPS 1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFLNFrame::CFLNFrame():MP_VECTOR_INIT(rlChngBlcks){
	pParams=NULL;
}

CFLNFrame::~CFLNFrame()
{

}

inline bool IsIntersected(const CRect& arc1, const CRect& arc2){
	if( (arc1.left<arc2.left && arc1.right<arc2.left) ||
		(arc1.left>arc2.right && arc1.right>arc2.right))
		return false;
	if( (arc1.top<arc2.top && arc1.bottom<arc2.top) ||
		(arc1.top>arc2.bottom && arc1.bottom>arc2.bottom))
		return false;
	return true;
}

void UnintersectBlock(CRect& arc, const CRect& arcC){
	// если пересечение с правой стороны, 
	if(arc.right >= arcC.left){
		// то урезаем правую сторону
		arc.right = arcC.left-1;
		// но так, чтобы он не стал никаким
		if(arc.right <= arc.left){
			arc.right = arc.left+1;
			if(!IsIntersected(arc, arcC))
				return;
		}else
			return;
	}
	// если пересечение с левой стороны, 
	if(arc.left <= arcC.right){
		// то урезаем левую сторону
		arc.left = arcC.right+1;
		// но так, чтобы он не стал никаким
		if(arc.left >= arc.right){
			arc.left = arc.right-1;
			if(!IsIntersected(arc, arcC))
				return;
		}else
			return;
	}
	// если пересечение с верхней стороны, 
	if(arc.top <= arcC.bottom){
		// то урезаем верхнюю сторону
		arc.top = arcC.bottom + 1;
		// но так, чтобы он не стал никаким
		if(arc.top >= arc.bottom){
			arc.top = arc.bottom - 1;
			if(!IsIntersected(arc, arcC))
				return;
		}else
			return;
	}
	// если пересечение с нижней стороны, 
	if(arc.bottom >= arcC.top){
		// то урезаем верхнюю сторону
		arc.bottom = arcC.top - 1;
		// но так, чтобы он не стал никаким
		if(arc.bottom <= arc.top){
			arc.bottom = arc.top + 1;
			if(!IsIntersected(arc, arcC))
				return;
		}else
			return;
	}
}

bool IsEmptyBlock(const CRect rc, CVASEBmp & bmp){
	int width = rc.right - rc.left;
	int pitch = bmp.GetPitch() - width - 1;
	BYTE * addr = bmp.GetPixAdress(rc.top, rc.left);
	for(int y = rc.top; y <= rc.bottom; y++)
	{
		for (int i = 0; i < width; ++i)
		{
			if ( *addr != 0)
				return false;
			++addr;
		}
		addr += pitch;
	}
	return true;
}

void UnintersectBlock2(CRect& arc, const CRect arcC){
	// CRect rcSrc = arc;
	// если пересечение с нижней стороны, 
	if(arc.bottom >= arcC.top){
		// то урезаем верхнюю сторону
		arc.bottom = arcC.top - 1;
		// но так, чтобы он не стал никаким
		if(arc.bottom <= arc.top){
			arc.bottom = arc.top + 1;
			if(!IsIntersected(arc, arcC))
				return;
		}else
			return;
	}
	// если пересечение с верхней стороны, 
	if(arc.top <= arcC.bottom){
		// то урезаем верхнюю сторону
		arc.top = arcC.bottom + 1;
		// но так, чтобы он не стал никаким
		if(arc.top >= arc.bottom){
			arc.top = arc.bottom - 1;
			if(!IsIntersected(arc, arcC))
				return;
		}else
			return;
	}
	// если пересечение с левой стороны, 
	if(arc.left <= arcC.right){
		// то урезаем левую сторону
		arc.left = arcC.right+1;
		// но так, чтобы он не стал никаким
		if(arc.left >= arc.right){
			arc.left = arc.right-1;
			if(!IsIntersected(arc, arcC))
				return;
		}else
			return;
	}
	// если пересечение с правой стороны, 
	if(arc.right >= arcC.left){
		// то урезаем правую сторону
		arc.right = arcC.left-1;
		// но так, чтобы он не стал никаким
		if(arc.right <= arc.left){
			arc.right = arc.left+1;
			if(!IsIntersected(arc, arcC))
				return;
		}else
			return;
	}
}

//int iFrameNum=0;

bool CFLNFrame::FillWinBMPFromDIB( CVASEBmp* bmp, unsigned int auWidth, unsigned int auHeight, void* lpBits, unsigned int auBPP)
{
	if(!bmp)
		return false;
	if(!lpBits)
		return false;
	if( auWidth == 0 || auHeight == 0)
		return false;

	bmp->Create( auWidth, auHeight, auBPP, lpBits);
	return true;
}

bool	CFLNFrame::LoadFromDIB( unsigned int auWidth, unsigned int auHeight, void* lpBits, unsigned int auBPP)
{
	if(!lpBits)
		return false;
	if( auWidth == 0 || auHeight == 0)
		return false;

	if( !FillWinBMPFromDIB( &bmp, auWidth, auHeight, lpBits, auBPP))
		return false;

	return true;
}

bool	CFLNFrame::GetChangedBlock( unsigned int auBlockIndex, byte* alpBits, unsigned int* apuMemSize, CRect*	apRect)
{
	if( auBlockIndex >= rlChngBlcks.size())
		return false;

	SIZE bmpSize = bmp.GetSize();

	CRect rect = rlChngBlcks[ auBlockIndex];
	CSize size = rect.Size();
	size += CSize(1,1);

	unsigned int memSize = size.cx*size.cy*4 /*32 bpp*/;

	if( rect.right >= bmpSize.cx)
		rect.right = bmpSize.cx - 1;
	if( rect.bottom >= bmpSize.cy)
		rect.bottom = bmpSize.cy - 1;

	if( apuMemSize)
		*apuMemSize = memSize;

	if( apRect)
		*apRect = rect;

	if( !alpBits)
	{
		if( apuMemSize)
			return true;
		return false;
	}

	unsigned int Pitch = size.cx * 4;
	BYTE * addr = bmp.GetPixAdress(rect.top, rect.left);
	DWORD delta = bmp.GetPitch();
	for (int ih = rect.top; ih <= rect.bottom; ih++)
	{

        memcpy( (void*)alpBits, (void *)addr, Pitch);
		alpBits += Pitch;
		addr += delta;
	}

	return true;
}

inline bool  fISPIXELDIFFERENT(BYTE* PIX1, BYTE* PIX2, unsigned int DOP)
{
	if (*(DWORD *)PIX1 == *(DWORD *)PIX2)
		return false;
	bool bDiff = false;
	if (abs(PIX1[RGBA_RED] - PIX2[RGBA_RED]) > (int)DOP ||
		abs(PIX1[RGBA_GREEN] - PIX2[RGBA_GREEN]) > (int)DOP ||
		abs(PIX1[RGBA_BLUE] - PIX2[RGBA_BLUE]) > (int)DOP)
		return true;
	return false;
}

unsigned int CFLNFrame::DefineChangeBlocks(BOOL abFirstFrame,CVASEBmp &abmpPrev, void * lpMaskBits){

	if(abFirstFrame){
		CRect rc;
		rc.left=0;
		rc.right=bmp.GetWidth()-1;
		rc.top=0;
		rc.bottom=bmp.GetHeight()-1;
		rlChngBlcks.push_back(rc);
		int iW=bmp.GetWidth();
		int iH=bmp.GetHeight();
		return 1;
	}
	// create pixels change map
	CVASEBmp bmpc;

	bmpc.Create(bmp.GetWidth() / 2, bmp.GetHeight(), 8, lpMaskBits);
    memset(lpMaskBits, 0, bmpc.GetDataSize());

	int iQ = pInputParameters->iQuality;
//	int x,y;

	BYTE * pix1 = bmp.GetpDIB();
	BYTE * pix2 = abmpPrev.GetpDIB();
	BYTE * pix = bmpc.GetpDIB();

	int iW = bmp.GetWidth() >> 1; // 2
	int iH = bmp.GetHeight();
	int delta1 = bmp.GetBPP() >> 2; // =8
	int delta2 = abmpPrev.GetBPP() >> 2; // =8

	BYTE * p1 = pix1;
	BYTE * p2 = pix2;
	int i = 0;
	int pitch = bmp.GetPitch();
	for ( i = 0; i < iH; ++i)
	{
		bool bDif = false;
		int dwords = pitch >> 2;
		for (int j = 0; j < dwords; ++j)
		{
			if (*(DWORD*)p1 != *(DWORD*)p2)
			{
				bDif = true;
				break;
			}
			p1 += 4; // DWORD
			p2 += 4; // DWORD
		}
		if (bDif)
			break;
		if (memcmp(p1, p2, pitch % 4)) // DWORD
			break;
	}
	if (i == iH)
		return 0;
	int startLine = i >> 1;

	p1 = bmp.GetLineAdress(iH - 1);
	p2 = abmpPrev.GetLineAdress(iH - 1);
	int pitch2 = pitch << 1;
	for ( i = iH - 1; i >= 0; --i)
	{
		bool bDif = false;
		int dwords = pitch >> 2;
		for (int j = 0; j < dwords; ++j)
		{
			if (*(DWORD*)p1 != *(DWORD*)p2)
			{
				bDif = true;
				break;
			}
			p1 += 4; // DWORD
			p2 += 4; // DWORD
		}
		if (bDif)
			break;
		if (memcmp(p1, p2, pitch % 4)) // DWORD
			break;
		p1 -= pitch2;
		p2 -= pitch2;
	}
	int stopLine = i;

	bool odd = false;
	pix = bmpc.GetLineAdress(startLine);
	for(int y = startLine; y < stopLine; y++)
	{
		pix1 = bmp.GetLineAdress(y);
		pix2 = abmpPrev.GetLineAdress(y);
		if (odd)
		{
			pix1 += (delta1 >> 1);
			pix2 += (delta2 >> 1);
		}
		for (int x = 0; x < iW; ++x)
		{
			*pix = fISPIXELDIFFERENT(pix1,pix2,iQ) ? 0xFF : 0x0;
			++pix;
			pix1 += delta1;
			pix2 += delta2;
		}
		odd = odd ? false : true;
	}

	CRect rcBlock;
	while(bFindMaxFilledBlock(rcBlock, bmpc))
	{
		// optimize origin block
		// (обрезаем пустые края)
		OptimizeBlock(rcBlock,bmpc);
		// если блок пересекается с каким-нибудь другим блоком, то урезать его, 
		// чтобы не пересекался
		CRect* pRect;
		if((pRect = FindIntersection(rcBlock)) != NULL)
		{
			CRect rcSrc = rcBlock;
			UnintersectBlock(rcBlock, *pRect);
			// если урезанный блок оказался пустым, 
			if(IsEmptyBlock(rcBlock, bmpc))
			{
				// то меняем стратегию урезания
				rcBlock = rcSrc;
				UnintersectBlock2(rcBlock, *pRect);
				// если смена стратегии не помогла, (вероятность < 1%)
				if(IsEmptyBlock(rcBlock, bmpc)){
					// то просто оставляем блок неурезанным (вероятность гораздо меньше 1%)
					rcBlock = rcSrc;
				}
			}
		}
		// raise block
		// (расширяем блок, если он не пересекается с другими блоками)
		RaiseBlock(rcBlock,bmpc);
		rlChngBlcks.push_back(rcBlock);
		// erase block in bmpc
		// (стираем пикселы, которые уже вошли в найденный блок)
		BYTE* pixel = bmpc.GetPixAdress(rcBlock.top, rcBlock.left);
		int pitch = bmpc.GetPitch() - (rcBlock.right - rcBlock.left) - 1;
		for(int y = rcBlock.top; y <= rcBlock.bottom; y++)
		{
			for(int x = rcBlock.left; x <= rcBlock.right; x++)
			{
				*pixel = 0x0;
				++pixel;
			}
			pixel += pitch;
		}
	}
	vecBlocksIt poss = rlChngBlcks.begin();
	vecBlocksIt end = rlChngBlcks.end();
	for( ;poss != end;poss++){
		poss->left <<= 1;
		poss->right <<= 1;
		if (poss->top > poss->bottom || poss->right < poss->left)
			assert(false);
	}
	return rlChngBlcks.size();
}

void CFLNFrame::OptimizeBlock(CRect &rc,CVASEBmp & bmp){
	int x,y;
	BYTE* pixel = bmp.GetPixAdress(rc.top, rc.left);
	int pitch = bmp.GetPitch() - (rc.right - rc.left) - 1;
	for(y = rc.top; y <= rc.bottom; y++)
	{
		BOOL bLineIsEmpty = TRUE;
		for(x = rc.left; x <= rc.right; x++)
		{
			if(*pixel)
			{ 
				bLineIsEmpty=FALSE;
				break;
			}
			++pixel;
		}
		if(!bLineIsEmpty) break;
		pixel += pitch;
	}
	rc.top = y;

	pixel = bmp.GetPixAdress(rc.bottom, rc.left);
	pitch = bmp.GetPitch() + (rc.right - rc.left) + 1;
	for(y = rc.bottom; y >= rc.top; y--)
	{
		BOOL bLineIsEmpty=TRUE;
		for(x=rc.left; x<=rc.right; x++)
		{	
			if(*pixel)
			{ 
				bLineIsEmpty=FALSE;
				break;
			}
			++pixel;
		}
		if(!bLineIsEmpty) break;
		pixel -= pitch;
	}
	rc.bottom= y ;

	BYTE * startPixel = pixel = bmp.GetPixAdress(rc.top, rc.left);
	pitch = bmp.GetPitch();
	for(x=rc.left; x<=rc.right; x++)
	{
		BOOL bColomnIsEmpty=TRUE;
		for(y=rc.top; y<=rc.bottom; y++)
		{
			if(*pixel)
			{ 
				bColomnIsEmpty=FALSE;
				break;
			}
			pixel += pitch;
		}
		if(!bColomnIsEmpty) break;
		++startPixel;
		pixel = startPixel;
	}
	rc.left=x;

	startPixel = pixel = bmp.GetPixAdress(rc.top, rc.right);
	pitch = bmp.GetPitch();
	for(x=rc.right; x>=rc.left; x--)
	{
		BOOL bColomnIsEmpty=TRUE;
		for(y=rc.top; y<=rc.bottom; y++)
		{
			if(*pixel)
			{ 
				bColomnIsEmpty=FALSE;
				break;
			}
			pixel += pitch;
		}
		if(!bColomnIsEmpty) break;
		--startPixel;
		pixel = startPixel;
	}
	rc.right=x;
}

bool CFLNFrame::bFindMaxFilledBlock(CRect &arcMaxFilled,CVASEBmp &abmpc)
{
	bool bFound=false;
	int iW=abmpc.GetWidth();
	int iH=abmpc.GetHeight();
	int iMinBlockSize = 10;
	int iMaxNumOfFilledPixels = 0;
	for(int yb = 0; yb < iH; yb += iMinBlockSize)
	{
		for(int xb = 0; xb < iW; xb += iMinBlockSize)
		{
			CRect rc;
			rc.left=xb;
			rc.right=rc.left+iMinBlockSize-1;
			if(rc.right >= iW) rc.right=iW-1;
			rc.top=yb;
			rc.bottom=rc.top+iMinBlockSize-1;
			if(rc.bottom >= iH) rc.bottom=iH-1;
			// 
			int iNumOfFilledPixels=iFilledPixels(rc,abmpc);
			if(iNumOfFilledPixels > iMaxNumOfFilledPixels){
				bFound=true;
				iMaxNumOfFilledPixels=iNumOfFilledPixels;
				arcMaxFilled=rc;
				if(iMaxNumOfFilledPixels==iMinBlockSize*iMinBlockSize)
					return bFound;
			}
		}
	}
	return bFound;
}

int CFLNFrame::iFilledPixels(CRect &arcBlock, CVASEBmp& bmp)
{
	int iNumOfFilledPixels = 0;
	BYTE* pixel = bmp.GetPixAdress(arcBlock.top, arcBlock.left);
	int pitch = bmp.GetPitch() - (arcBlock.right - arcBlock.left) - 1;
	for(int y = arcBlock.top; y <= arcBlock.bottom; y++)
	{
		for(int x = arcBlock.left; x <= arcBlock.right; x++)
		{
			if(*pixel)
				iNumOfFilledPixels++;
			++pixel;
		}
		pixel += pitch;
	}
	return iNumOfFilledPixels;
}

void CFLNFrame::RaiseBlock(CRect &arcBlock,CVASEBmp &abmpc)
{
	int iW = abmpc.GetWidth();
	int iH = abmpc.GetHeight();
	int iMaxInc = 3;
	while(1)
	{
		CRect rcOld=arcBlock;
		for(int iDir=1; iDir <= 4; iDir++)
		{
			// iDir: 1-right, 2-down,3-left, 4-up
			while(1){
				int iInc=iMaxInc;
				// test for not go out
				int iDistToMargin;
				switch(iDir){
				case 1: // to right
					iDistToMargin=(iW-1)-arcBlock.right;
//					iDistToMargin=iW-arcBlock.right;
					break;
				case 3: // to left
					iDistToMargin=arcBlock.left;
					break;
				case 2: // to bottom
					iDistToMargin=(iH-1)-arcBlock.bottom;
//					iDistToMargin=iH-arcBlock.bottom;
					break;
				case 4: // to top
					iDistToMargin=arcBlock.top;
					break;
				}
				if(iDistToMargin==0) break;
				if(iDistToMargin < iInc) iInc=iDistToMargin;
				// test for filling
				switch(iDir){
				case 1: // to right
					while(!iFilledPixels(CRect(arcBlock.right,arcBlock.top,arcBlock.right+iInc,arcBlock.bottom),abmpc))
						if(!(--iInc)) break;
					break;
				case 3: // to left
					while(!iFilledPixels(CRect(arcBlock.left-iInc,arcBlock.top,arcBlock.left,arcBlock.bottom),abmpc))
						if(!(--iInc)) break;
					break;
				case 2: // to bottom
					while(!iFilledPixels(CRect(arcBlock.left,arcBlock.bottom,arcBlock.right,arcBlock.bottom+iInc),abmpc))
						if(!(--iInc)) break;
					break;
				case 4: // to top
					while(!iFilledPixels(CRect(arcBlock.left,arcBlock.top-iInc,arcBlock.right,arcBlock.top),abmpc))
						if(!(--iInc)) break;
					break;
				}
				if(!iInc) break;
				// test for intersection
				switch(iDir){
				case 1: // to right
					while(bIntersectedByAnyOne(CRect(arcBlock.right,arcBlock.top,arcBlock.right+iInc,arcBlock.bottom)))
						if(!(--iInc)) break;
					break;
				case 3: // to left
					while(bIntersectedByAnyOne(CRect(arcBlock.left-iInc,arcBlock.top,arcBlock.left,arcBlock.bottom)))
						if(!(--iInc)) break;
					break;
				case 2: // to bottom
					while(bIntersectedByAnyOne(CRect(arcBlock.left,arcBlock.bottom,arcBlock.right,arcBlock.bottom+iInc)))
						if(!(--iInc)) break;
					break;
				case 4: // to top
					while(bIntersectedByAnyOne(CRect(arcBlock.left,arcBlock.top-iInc,arcBlock.right,arcBlock.top)))
						if(!(--iInc)) break;
					break;
				}
				if(!iInc) break;
				//
				switch(iDir){
				case 1: // to right
					arcBlock.right+=iInc;
					break;
				case 3: // to left
					arcBlock.left-=iInc;
					break;
				case 2: // to bottom
					arcBlock.bottom+=iInc;
					break;
				case 4: // to top
					arcBlock.top-=iInc;
					break;
				}
			} // while(1)
		} // for by iDir
		if(rcOld==arcBlock) break;
	} // while(1)
	// ??
	return;
}

bool CFLNFrame::bIntersectedByAnyOne(CRect &arcBlock)
{
	bool bIsIntersected = FALSE;
	vecBlocksIt poss = rlChngBlcks.begin();
	for(;poss!=rlChngBlcks.end();poss++){
		CRect rc4=(*poss);
		BOOL bIntersected=TRUE;
		if( (rc4.left<arcBlock.left && rc4.right<arcBlock.left) ||
			(rc4.left>arcBlock.right && rc4.right>arcBlock.right))
			bIntersected=FALSE;
		if( (rc4.top<arcBlock.top && rc4.bottom<arcBlock.top) ||
			(rc4.top>arcBlock.bottom && rc4.bottom>arcBlock.bottom))
			bIntersected=FALSE;
//		if( rc4.left >= rcCmb.left &&
//			rc4.right <= rcCmb.right &&
//			rc4.top >= rcCmb.top &&
//			rc4.bottom <= rcCmb.bottom)
//			bIntersected=FALSE;
		if(bIntersected){
			bIsIntersected=TRUE;
			break;
		}
	}
	return bIsIntersected;
}

CRect* CFLNFrame::FindIntersection(CRect &arcBlock){
	vecBlocksIt poss = rlChngBlcks.begin();
	vecBlocksIt end = rlChngBlcks.end();
	for( ;poss != end;poss++){
		CRect rc4=(*poss);
		if( (rc4.left<arcBlock.left && rc4.right<arcBlock.left) ||
			(rc4.left>arcBlock.right && rc4.right>arcBlock.right) ||
			(rc4.top<arcBlock.top && rc4.bottom<arcBlock.top) ||
			(rc4.top>arcBlock.bottom && rc4.bottom>arcBlock.bottom))
			continue;
		return &(*poss);
	}
	return NULL;
}

