#include "stdafx.h"
#include "ComputerStateOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

ComputerStateOut::ComputerStateOut( const char *fps, int quality, unsigned int currentPhysicalMemory, unsigned int currentVirtualMemory)
{
	data.clear();	
	unsigned short size = strlen(fps);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size, (BYTE*)fps);
	data.add( quality);
	data.add( currentPhysicalMemory);
	data.add( currentVirtualMemory);
}


