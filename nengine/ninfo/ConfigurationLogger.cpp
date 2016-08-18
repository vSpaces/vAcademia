
#include "StdAfx.h"
#include "Globals.h"
#include "HelperFunctions.h"
#include "ConfigurationLogger.h"	
#include "GlobalSingletonStorage.h"

CConfigurationLogger::CConfigurationLogger()
{
}

void CConfigurationLogger::LogAll(ILogWriter* lw)
{
	g->gi.Analyze();

	if( lw)
	{
		unsigned int ms = timeGetTime();
		unsigned int sec = (int)(ms / 1000);
		unsigned int min = (int)(sec / 60);
		sec = sec % 60;
		ms = ms % 1000;
		unsigned int hours = (int)(min / 60);
		min = min % 60;
		lw->WriteLn("Time from Windows start: ", hours, " hours ", min, " min ", sec, " sec ", ms,  " ms");
		lw->WriteLn("Checking hardware and software configuration");		
		lw->WriteLn("GPU vendor: ", g->gi.GetVendorName());		
		lw->WriteLn("GPU device: ", g->gi.GetDeviceName());
		lw->WriteLn("GPU vendor in OpenGL drivers: ", g->gi.GetVendorGLName());
		lw->WriteLn("GPU renderer in OpenGL drivers: ", g->gi.GetRendererGLName());
		lw->WriteLn("Supported OpenGL version: ", g->gi.GetGLVersion());
		SOpenGLVersion ver = g->gi.GetOpenGLVersionNumber();
		lw->WriteLn("Detected OpenGL version: ", (int)ver.major, ".", (int)ver.minor);
		lw->WriteLn("Supported OpenGL extensions: ", g->ei.GetExtensionsString());

		int leadVersion, majorVersion, minorVersion;
		g->gi.GetDriverVersion(leadVersion, majorVersion, minorVersion);
		lw->WriteLn("Driver version: ", IntToStr(majorVersion), ".", IntToStr(minorVersion), " x", leadVersion);
		lw->WriteLn("NPOT support: ", g->gi.IsNPOTSupport() ? "enabled" : "disabled");
		lw->WriteLn("Computer name: ", g->ci.GetCompName());

		unsigned int videoMemoryAmount = g->gi.GetVideoMemory()->GetTotalSize();		

		lw->WriteLn("Video memory size: ", (videoMemoryAmount == 0xFFFFFFFF) ? "[not detected]" : IntToStr(videoMemoryAmount), " bytes");
		lw->WriteLn("Free video memory size: ", g->gi.GetVideoMemory()->GetFreeSize(), " bytes");
		lw->WriteLn("Physical memory size: ", (__int64)g->ci.GetPhysicalMemorySize(), " bytes");
		lw->WriteLn("Available physical memory size: ", g->ci.GetAvailablePhysicalMemorySize(), " bytes");
		lw->WriteLn("Virtual memory size: ", g->ci.GetVirtualMemorySize(), " bytes");
		lw->WriteLn("Available virtual memory size: ", g->ci.GetAvailableVirtualMemorySize(), " bytes");
		lw->WriteLn("Processor name: ", g->ci.GetProcessorName());
		lw->WriteLn("Processor frequency: ", g->ci.GetCPUFrequency());
		lw->WriteLn("Virtual cores count: ", g->ci.GetProcessorCount());
		lw->WriteLn("Physical cores count: ", g->ci.GetRealCoreCount());
		lw->WriteLn("Hyperthreading support: ", g->ci.IsHyperthreadingEnabled() ? "yes" : "no");
		lw->WriteLn("OS version: ", g->ci.GetOSVersionString());
		lw->WriteLn("OS bits: x", g->ci.GetOSBits());
		lw->WriteLn("Laptop: ", g->ci.IsLaptop() ? "yes" : "no");
		lw->WriteLn("Internal videocard generation number: ", (int)g->gi.GetVideocardGeneration());
		int optStatus = g->gi.GetOptimusDeviceStatus();
		std::string optimusStatus = "not presented";
		switch (optStatus)
		{
		case OPTIMUS_NVIDIA_USED:
			optimusStatus = "presented, used NVidia for rendering";
			break;

		case OPTIMUS_INTEL_USED:
			optimusStatus = "presented, used Intel for rendering";
			break;
		}
	
		lw->WriteLn("Optimus: ", optimusStatus);
		lw->WriteLn("Displays: ", g->ci.GetDisplayCount());
		lw->WriteLn("Mouse: ", g->ci.IsMousePresent() ? "presented" : "missing");
		if (g->ci.IsMousePresent())
		{
			lw->WriteLn("Mouse buttons: ", g->ci.GetMouseButtonCount());
		}
		lw->WriteLn("Network: ", g->ci.IsNetworkPresent() ? "presented" : "missing");

		const unsigned int bigMemorySize = 100 * 1024 * 1024;
		lw->WriteLn("Processes with significant memory usage (more than ", bigMemorySize," bytes):");
		std::vector<SMemoryProcessInfo> list2;
		g->pi.GetProcessesWithMemoryUsageHigherThan(list2, bigMemorySize);

		for (unsigned int i = 0; i < list2.size(); i++)
		{
			lw->WriteLn(list2[i].processName, " - ", list2[i].memorySize, " bytes");
		}		

		g->lw.WriteLn("Antivirus mask: ", (int)g->pi.DetectAntivirus());
		g->lw.WriteLn("SecondLife installed: ", g->ci.IsSecondLifeInstalled() ? "yes" : "no");
	}
}

void CConfigurationLogger::LogProcesses(ILogWriter* lw)
{
	const int bigCPUUsage = 5;

	lw->WriteLn("Processes with significant CPU usage (more than ", bigCPUUsage," percents):");

	std::vector<SCPUProcessInfo> list1;
	g->pi.GetProcessesWithCPUUsageHigherThan(list1, bigCPUUsage);

	for (unsigned int i = 0; i < list1.size(); i++)
	{
		lw->WriteLn(list1[i].processName, " - ", list1[i].cpuUsage, " percents");
	}
}

CConfigurationLogger::~CConfigurationLogger()
{
}
