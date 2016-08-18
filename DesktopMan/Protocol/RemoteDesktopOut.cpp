#include "StdAfx.h"
#include ".\remotedesktopout.h"

using namespace SharingManProtocol;

CRemoteDesktopOut::CRemoteDesktopOut(unsigned short aSessionID, unsigned char aType, unsigned short aFirstParam, unsigned int aSecondParam, unsigned char aCode)
{
	GetDataBuffer().reset();
	GetDataBuffer().add<unsigned short>(aSessionID);
	GetDataBuffer().add<unsigned char>(aType);
	GetDataBuffer().add<unsigned short>(aFirstParam);
	GetDataBuffer().add<unsigned int>(aSecondParam);
	GetDataBuffer().add<unsigned char>(aCode);
}