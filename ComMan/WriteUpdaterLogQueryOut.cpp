#include "stdafx.h"
#include "WriteUpdaterLogQueryOut.h"

using namespace ResManProtocol;

CWriteUpdaterLogQueryOut::CWriteUpdaterLogQueryOut(const char *aLogValue)
{
	unsigned short size = (unsigned short)strlen(aLogValue);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size, (BYTE*)aLogValue);
}
