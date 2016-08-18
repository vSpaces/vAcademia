#include "stdafx.h"
#include "ComputerConfigurationOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

ComputerConfigurationOut::ComputerConfigurationOut()
{
}

void ComputerConfigurationOut::AddComputerInfo( std::wstring &compName, std::string &processName, unsigned int &processorCount, unsigned int &realCoreCount, bool &hyperthreadingEnabled, unsigned __int64 physicalMemorySize, unsigned __int64 virtualMemorySize, std::string &osVersion, unsigned char &osBits)
{
	unsigned short size = compName.size();
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)compName.c_str());

	size = processName.size();
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size, (BYTE*) processName.c_str());

	data.add( processorCount);
	data.add( realCoreCount);
	data.add( hyperthreadingEnabled?(byte)1:(byte)0);
	data.add( physicalMemorySize);
	data.add( virtualMemorySize);

	size = osVersion.size();
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size, (BYTE*) osVersion.c_str());

	data.add( osBits);
}

void ComputerConfigurationOut::AddGPUInfo( std::string &vendorName, std::string &deviceName, unsigned __int64 &videoMemorySize, int &major, int &minor)
{
	unsigned short size = vendorName.size();
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size, (BYTE*) vendorName.c_str());

	size = deviceName.size();
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size, (BYTE*) deviceName.c_str());

	data.add( videoMemorySize);
	data.add( major);
	data.add( minor);
}


