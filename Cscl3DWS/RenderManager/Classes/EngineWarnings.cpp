
#include "StdAfx.h"
#include "XMLList.h"
#include "EngineWarnings.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"

CEngineWarnings::CEngineWarnings(CUserSettings* settings):
	m_isAnalyzed(false),
	m_settings(settings)
{
	assert(m_settings);
}

void CEngineWarnings::AddWarning(unsigned int id)
{
	if (id == ENGINE_WARNING_BETA_DRIVER)
	{
		if (m_settings->GetSetting("disable_betadriver") == "true")
		{
			g->lw.WriteLn("Warning disable: disable_betadriver");
			return;
		}
	}

	if (id == ENGINE_WARNING_OLD_DRIVER)
	{
		if (m_settings->GetSetting("disable_olddriver") == "true")
		{
			g->lw.WriteLn("Warning disable: disable_olddriver");
			return;
		}
	}

	if ((id == ENGINE_WARNING_RAM_NOT_ENOUGH) || (id == ENGINE_WARNING_VIDEOCARD_TOO_WEAK) || 
		(id == ENGINE_WARNING_CPU_FREQ_NOT_ENOUGH) || (id == ENGINE_WARNING_OS_OUTDATED) || 
		(id == ENGINE_WARNING_VIDEOMEM_NOT_ENOUGH))
	{
		if (m_settings->GetSetting("disable_minsysreq") == "true")
		{
			g->lw.WriteLn("Warning disable: disable_minsysreq");
			return;
		}
	}

	m_ids.push_back(id);
}

void CEngineWarnings::AnalyzeIfNeeded()
{
	if (m_isAnalyzed)
	{
		return;
	}

	m_isAnalyzed = true;

	if (g->gi.GetOptimusDeviceStatus() == OPTIMUS_INTEL_USED)
	{
		AddWarning(ENGINE_WARNING_OPTIMUS_INTEL_USED);		
	}

	if (!g->gi.IsOfficialVersion())
	{
		AddWarning(ENGINE_WARNING_BETA_DRIVER);		
	}	

	bool areDriversNotInstalled = false;
		
    int vendorCheckAnswer = g->cc.IsVendorSupported();

	switch (vendorCheckAnswer)
	{
	case DRIVER_UNSUPPORTED_VERSION:
		{
			AddWarning(ENGINE_WARNING_OLD_DRIVER);
			break;
		}

	case VENDOR_UNSUPPORTED:
		{
			AddWarning(ENGINE_WARNING_VENDOR_UNSUPPORTED);
			break;
		}		

	case DRIVER_INVALID:
		{
			AddWarning(ENGINE_WARNING_DRIVER_INVALID);
			break;
		}				

	case DRIVERS_NOT_INSTALLED:
		{
			AddWarning(ENGINE_WARNING_DRIVERS_NOT_INSTALLED);
			areDriversNotInstalled = true;
			break;			
		}		
	}

	// probably drivers not installed
	if ((m_ids.size() == 0) || (m_ids[m_ids.size() - 1] != ENGINE_WARNING_DRIVERS_NOT_INSTALLED))
	if (!g->cc.AreRequiredExtensionsExists())
	{		
		areDriversNotInstalled = true;
		AddWarning(ENGINE_WARNING_DRIVERS_NOT_INSTALLED);
	}

	if (!gRM->minSysReq->IsMemoryOk())
	{
		AddWarning(ENGINE_WARNING_RAM_NOT_ENOUGH);
	}

	if (!gRM->minSysReq->IsVideocardOk())
	{
		if (!areDriversNotInstalled)
		{
			AddWarning(ENGINE_WARNING_VIDEOCARD_TOO_WEAK);
		}
	}

	if (!gRM->minSysReq->IsProcessorOk())
	{
		AddWarning(ENGINE_WARNING_CPU_FREQ_NOT_ENOUGH);
	}

	if (!gRM->minSysReq->IsSystemOk())
	{
		AddWarning(ENGINE_WARNING_OS_OUTDATED);
	}

	if (!gRM->minSysReq->IsVideoMemoryOk())
	{
		if (g->gi.GetVendorID() != VENDOR_INTEL)
		{
			if (g->gi.GetVideoMemory()->GetTotalSize() != 0)
			{
				AddWarning(ENGINE_WARNING_VIDEOMEM_NOT_ENOUGH);
			}
		}
		else
		{
			if (g->gi.GetVideoMemory()->GetTotalSize() != 0)
			{
				AddWarning(ENGINE_WARNING_NEED_TO_ENLARGE_VIDEOMEM);
			}
		}
	}

	CXMLList list(GetApplicationRootDirectory() + L"emulate_warnings.xml", false);
	std::string value;
	while (list.GetNextValue(value))
	{
		if (value == "optimus") AddWarning(ENGINE_WARNING_OPTIMUS_INTEL_USED);
		if (value == "betadriver") AddWarning(ENGINE_WARNING_BETA_DRIVER);
		if (value == "olddriver") AddWarning(ENGINE_WARNING_OLD_DRIVER);
		if (value == "vendorunsupported") AddWarning(ENGINE_WARNING_VENDOR_UNSUPPORTED);
		if (value == "driverinvalid") AddWarning(ENGINE_WARNING_DRIVER_INVALID);
		if (value == "nodrivers") AddWarning(ENGINE_WARNING_DRIVERS_NOT_INSTALLED);
		if (value == "smallram") AddWarning(ENGINE_WARNING_RAM_NOT_ENOUGH);
		if (value == "videocardtooweak") AddWarning(ENGINE_WARNING_VIDEOCARD_TOO_WEAK);
		if (value == "processortooweak") AddWarning(ENGINE_WARNING_CPU_FREQ_NOT_ENOUGH);
		if (value == "osoutdated") AddWarning(ENGINE_WARNING_OS_OUTDATED);
		if (value == "intelvideomemsmall") AddWarning(ENGINE_WARNING_NEED_TO_ENLARGE_VIDEOMEM);
		if (value == "videomemsmall") AddWarning(ENGINE_WARNING_VIDEOMEM_NOT_ENOUGH);
	}
}

std::vector<unsigned int>& CEngineWarnings::GetIDs()
{
	AnalyzeIfNeeded();

	return m_ids;
}

CEngineWarnings::~CEngineWarnings()
{
}