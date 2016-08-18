#include "stdafx.h"
#include "FrameOut.h"

#include "vase.h"
#include "vasebitmap.h"
#include "VASEFramesDifPack.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SharingManProtocol;

CFrameOut::CFrameOut(unsigned short sessionID, unsigned int aTime, VASEFramesDifPack* pack, bool abKeyFrame, bool useNewCodec)
{
	GetDataBuffer().reset();
	
	GetDataBuffer().add<unsigned short>( sessionID);
	byte keyFrameFlag = abKeyFrame ? 1 : 0;
	GetDataBuffer().add<byte>( useNewCodec);
	GetDataBuffer().add<byte>( keyFrameFlag);
	if( keyFrameFlag)
	{
		ATLASSERT( pack->GetBitmapsCount() == 1);
		unsigned short keyFrameWidth = 0;
		unsigned short keyFrameHeight = 0;
		if( pack->GetBitmapsCount() > 0)
		{
			VASEBitmap*	bitmap = pack->GetBitmap(0);
			ATLASSERT( bitmap);
			keyFrameWidth = (unsigned short)bitmap->GetSize().cx;
			keyFrameHeight = (unsigned short)bitmap->GetSize().cy;
		}
		GetDataBuffer().add<unsigned short>(keyFrameWidth);
		GetDataBuffer().add<unsigned short>(keyFrameHeight);
	}

	GetDataBuffer().add<unsigned int>( aTime);
	GetDataBuffer().add<unsigned int>( pack->GetBitmapsCount());

	for (unsigned int i=0; i<pack->GetBitmapsCount(); i++)
	{
		VASEBitmap*	bitmap = pack->GetBitmap(i);
		GetDataBuffer().add<unsigned short>((unsigned short)bitmap->GetPosition().x);
		GetDataBuffer().add<unsigned short>((unsigned short)bitmap->GetPosition().y);
		GetDataBuffer().add<unsigned int>((unsigned int)bitmap->GetSize().cx);
		GetDataBuffer().add<unsigned int>((unsigned int)bitmap->GetSize().cy);

		const void* bitmapData = NULL;
		unsigned int dataSize = 0;
		bitmap->GetBuffer( &bitmapData, &dataSize);
		GetDataBuffer().add<unsigned int>( dataSize);
		GetDataBuffer().addData( bitmapData, dataSize); 
	}
}

