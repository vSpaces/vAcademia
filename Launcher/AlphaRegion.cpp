#include "StdAfx.h"
#include "AlphaRegion.h"

#include "atlgdi.h"
#include <atlmisc.h>
#include <vector>

HRGN CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color)
{
	if ( !hBmp) 
		return NULL;

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm ); // get bitmap attributes
	CDC dcBmp;
	dcBmp.CreateCompatibleDC( GetDC( 0)); //Creates a memory device context for the bitmap

	HBITMAP hBmpOld = dcBmp.SelectBitmap( hBmp); //selects the bitmap in the device context

	std::vector<CRect> Regions;

	int nState=-1;
	int nStart=0;
	bool bIsMask;

	// scan the image to find areas of solid regions to draw
	int y;
	for ( y = 0; y < bm.bmHeight; ++y )
	{
		nState = -1;
		nStart = 0;
		int x;
		for ( x = 0; x < bm.bmWidth; ++x )
		{
			COLORREF cr = dcBmp.GetPixel(x,bm.bmHeight-y-1);
			if( cr&0xFF000000 != 0xFF000000)
				int ii=0;
			// get color
			bIsMask=(dcBmp.GetPixel(x,bm.bmHeight-y-1)==color);

			switch( nState )
			{
			case -1: // first pixel
				if( bIsMask )
					nState = 0;
				else
					nState = 1;

				nStart = 0;
				break;

			case 1: // solid
				if( bIsMask )
				{
					// create region
					Regions.push_back( CRect( nStart, bm.bmHeight - y - 1, x, bm.bmHeight - y) );
					nState = 0;
				}
				break;

			case 0: // transaparent

				if( !bIsMask )
				{
					// start region
					nStart = x;
					nState = 1;
				}
				break;
			}
		}

		// solid last pixel?
		if( nStart != x-1 && nState==1)
		{
			// add region
			Regions.push_back( CRect( nStart, bm.bmHeight - y - 1, bm.bmWidth, bm.bmHeight - y) );
		}
	}

	dcBmp.SelectBitmap( hBmpOld);
	dcBmp.DeleteDC(); //release the bitmap


	// create region
	// Under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net)
	// HRGN hRgn = ExtCreateRegion( NULL, RDHDR + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
	// ExtCreateRegion replacement {
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	ATLASSERT( hRgn != NULL );

	for ( int i = 0; i < (int) Regions.size(); ++i )
	{
		const CRect& rt = Regions[i];
		HRGN hr = CreateRectRgn(rt.left, rt.top, rt.right, rt.bottom);
		CombineRgn( hRgn, hRgn, hr, RGN_OR);
		if (hr) 
			DeleteObject(hr);
	}

	Regions.clear();

	ATLASSERT( hRgn != NULL );

	return hRgn;
}

#include <WindowsX.h>


CAlphaRegion::CAlphaRegion()
{
	m_hRegion = 0;
}

CAlphaRegion::~CAlphaRegion()
{
	if( m_hRegion != 0)
		DeleteRgn( m_hRegion);
	m_hRegion = 0;
}

bool CAlphaRegion::Create( HBITMAP ahBitmap)
{
	if( m_hRegion != 0)
		DeleteRgn( m_hRegion);
	m_hRegion = CreateRgnFromBitmap( ahBitmap, RGB(20,188,252));
	return m_hRegion != ERROR ? true : false;
}

HRGN CAlphaRegion::GetRegion()
{
	return m_hRegion;
}