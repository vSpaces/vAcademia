#include "stdafx.h"
#include "PreviewRecordStopQueryOut.h"

using namespace MapManagerProtocol;

PreviewRecordStopQueryOut::PreviewRecordStopQueryOut(unsigned int aiRecordID, unsigned int aiDuration)
{
	data.add(aiRecordID);
	data.add(aiDuration);
}

