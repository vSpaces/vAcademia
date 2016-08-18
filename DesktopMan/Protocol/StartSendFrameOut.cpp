#include "StdAfx.h"
#include ".\startsendframeout.h"

using namespace SharingManProtocol;

StartSendFrameOut::StartSendFrameOut(LPCSTR alpName)
{
	GetDataBuffer().reset();

	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpName));
}
