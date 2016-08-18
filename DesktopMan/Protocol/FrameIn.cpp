#include "stdafx.h"
#include "FrameIn.h"

#include "..\..\VASEncoder\VASE.h"
#include "..\..\VASEncoder\vasebitmap.h"
#include "..\..\VASEncoder\VASEFramesDifPack.h"

CFrameIn::CFrameIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
	isKeyframe = 0;
	keyFrameWidth = 0;
	keyFrameHeight = 0;
	useNewCodec = false;
}

void	CFrameIn::SetFramesPack( VASEFramesDifPack* apFramesPack)
{
	framesPack = apFramesPack;
}

bool CFrameIn::Analyse()
{
	if( !framesPack)
		return false;

	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	if ( !memparse_object( pData, idx, useNewCodec))
		return false;
	// 1. keyframe
	if ( !memparse_object( pData, idx, isKeyframe))
		return false;

	//unsigned short	keyFrameWidth = 0;
	//unsigned short	keyFrameHeight = 0;
	// 1.1. Keyframe size
	if( 1 == isKeyframe)
	{
		if ( !memparse_object( pData, idx, keyFrameWidth))
			return false;
		if ( !memparse_object( pData, idx, keyFrameHeight))
			return false;
	}

	// 2. sprites count
	unsigned int spritesCount = 0;
	if ( !memparse_object( pData, idx, spritesCount))
		return false;

	for (unsigned int i=0; i<spritesCount; i++)
	{
		// 3. sprite
		VASEBitmap* bitmap = framesPack->AddBitmap();

		unsigned short spriteX = 0;
		unsigned short spriteY = 0;
		unsigned int width = 0;
		unsigned int heigth = 0;
		if ( !memparse_object( pData, idx, spriteX))
			return false;
		if ( !memparse_object( pData, idx, spriteY))
			return false;
		if ( !memparse_object( pData, idx, width))
			return false;
		if ( !memparse_object( pData, idx, heigth))
			return false;
		bitmap->SetPosition( CPoint( spriteX, spriteY));

		if( 1 == isKeyframe)
		{
			bitmap->SetSize( CSize( keyFrameWidth, keyFrameHeight));
		}
		if (useNewCodec)
			bitmap->SetSize( CSize( width, heigth));
		unsigned int bufferSize = 0;
		if ( !memparse_object( pData, idx, bufferSize))
			return false;

		// mega hack
		if( bufferSize != 0)
		{
			byte* frameBuffer = bitmap->GetBufferSetLength( bufferSize);
			bitmap->SetEncodedBufferSize( bufferSize);
			if ((unsigned int)(idx + bufferSize) > iDataSize)
			{
				return 0;
			}
			memcpy(frameBuffer, &pData[idx], bufferSize);
			idx += bufferSize;
		}
		else
			return false;
	}

	return EndOfData( idx);
}

bool	CFrameIn::GetKeyFrameFlag()
{
	return (isKeyframe == 0) ? false : true;
}
bool	CFrameIn::UseNewCodec()
{
	return useNewCodec;
}