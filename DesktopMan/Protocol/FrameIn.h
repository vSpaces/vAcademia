#pragma once
#include "CommonPacketIn.h"

class VASEFramesDifPack;

class CFrameIn : public Protocol::CCommonPacketIn
{
public:
	CFrameIn ( BYTE *aData, int aDataSize);
	virtual bool Analyse();

	void	SetFramesPack( VASEFramesDifPack* apFramesPack);
	bool	GetKeyFrameFlag();
	bool	UseNewCodec();

	unsigned short	GetKeyFrameWidth()
	{
		return keyFrameWidth;
	}

	unsigned short	GetKeyFrameHeight()
	{
		return keyFrameHeight;
	}


protected:
	bool					useNewCodec;
	byte					isKeyframe;
	VASEFramesDifPack*		framesPack;
	unsigned short	keyFrameWidth;
	unsigned short	keyFrameHeight;
};
