#include "StdAfx.h"
#include "GraphicQualityController.h"
#include "GlobalSingletonStorage.h"
#include "DynamicClouds.h"
#include "RMContext.h"

#define SKY_SIZE									25000.0f

#define QUALITY_ERROR_NO_ERROR						0
#define QUALITY_ERROR_DRIVERS_NOT_INSTALLED			1
#define QUALITY_ERROR_VENDOR_UNSUPPORTED			2
#define QUALITY_ERROR_DRIVER_UNSUPPORTED_VERSION	3
#define QUALITY_ERROR_MEMORY_SIZE					4

void UpdateCloudsProc()
{
	gRM->nrMan->ProcessClouds();
}

CGraphicQualityController::CGraphicQualityController(oms::omsContext* context):
	m_sky(NULL),
	m_context(context),
	m_postEffect(NULL),
	m_deleteSkyCounter(0),
	m_qualityMode(LOW_QUALITY_MODE_ID)
{ 
}

void CGraphicQualityController::Update()
{
	if (m_deleteSkyCounter > 0)
	{
		if (1 == m_deleteSkyCounter)
		{
			DeleteSky();
		}

		m_deleteSkyCounter--;
	}
}

void CGraphicQualityController::InitHighQualityMode(const bool isVeryHigh)
{
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		m_deleteSkyCounter = -1;
	}

	if (!isVeryHigh)
	{
		m_qualityMode = HIGH_QUALITY_MODE_ID;
		g->rs.LoadAll(L"render_settings_high.xml");
	}
	else
	{
		m_qualityMode = VERY_HIGH_QUALITY_MODE_ID;
		g->rs.LoadAll(L"render_settings_very_high.xml");
	}

	if (!g->sm.IsShadersInit())
	{
		g->sm.Init();
	}

	if (!m_postEffect)
	{
		if ((g->rs.GetBool(POSTEFFECT_ENABLED)) && (g->rs.GetBool(SHADERS_USING)))
		{
			m_postEffect = MP_NEW(CHDRPostEffect);
			g->shdw.CheapSoftShadowSupport(true);
		}
	}

	if (g->cm.GetActiveCamera())
	{
		CVector3D delta = (g->cm.GetActiveCamera()->GetEyePosition() - g->cm.GetActiveCamera()->GetLookAt());
		delta.Normalize();
		delta *= 3.0;
		g->cm.GetActiveCamera()->SetEyePosition(g->cm.GetActiveCamera()->GetEyePosition() + delta);
	}
}

void CGraphicQualityController::SwitchToLowQualityMode()
{
	if (m_postEffect != NULL)
		MP_DELETE(m_postEffect);
	m_postEffect = NULL;
	m_qualityMode = LOW_QUALITY_MODE_ID;
	DeleteSky();
	g->rs.LoadAll(L"render_settings_low.xml");
	g->mm.ReInitialize();
}

void CGraphicQualityController::SwitchToMediumQualityMode()
{
	InitHighQualityMode(false);
	m_qualityMode = MEDIUM_QUALITY_MODE_ID;
	DeleteSky();
	g->rs.LoadAll(L"render_settings_medium.xml");

	for (int k = 0; k < g->matm.GetCount(); k++)
	{
		CMaterial* material = g->matm.GetObjectPointer(k);
		material->ReInitialize();
	}
	g->mm.ReInitialize();
}

void CGraphicQualityController::SwitchToHighQualityMode()
{
	DeleteSky();
	InitHighQualityMode(false);

	m_qualityMode = HIGH_QUALITY_MODE_ID;

	for (int k = 0; k < g->matm.GetCount(); k++)
	{
		CMaterial* material = g->matm.GetObjectPointer(k);
		material->ReInitialize();
	}
	g->mm.ReInitialize();
}

void CGraphicQualityController::SwitchToVeryHighQualityMode()
{
	DeleteSky();
	InitHighQualityMode(true);

	m_qualityMode = VERY_HIGH_QUALITY_MODE_ID;

	for (int k = 0; k < g->matm.GetCount(); k++)
	{
		CMaterial* material = g->matm.GetObjectPointer(k);
		material->ReInitialize();
	}
	g->mm.ReInitialize();
}

void CGraphicQualityController::DeleteSky()
{
	if (m_sky)
	{
		for (int k = 0; k < g->scm.GetCount(); k++)
		{
			CScene* scene = g->scm.GetObjectPointer(k);
			scene->RemoveSceneObject(m_sky);
			scene->RemoveUpdateObject(&m_updateClouds);
		}

		m_sky->Destroy();		
		m_sky = NULL;
	}
}

void CGraphicQualityController::CreateSkyIfNeeded(void* _scene)
{
	CScene* scene = (CScene*)_scene;

	if (!m_sky)
	{
		if ((LOW_QUALITY_MODE_ID == m_qualityMode) || (MEDIUM_QUALITY_MODE_ID == m_qualityMode))
		{
			m_sky = MP_NEW(CCubemapClouds);
			((CCubemapClouds *)m_sky)->SetCubemap("DAYSKY4.JPG");
			m_sky->SetSize(SKY_SIZE);
		}
		else
		{
			m_sky = MP_NEW(CDynamicClouds);
			m_sky->SetSize(SKY_SIZE);

			if (-1 == m_deleteSkyCounter)
			{
				m_deleteSkyCounter = 5;
			}
		}

		scene->AddSceneObject(m_sky);
		m_sky->SetRenderPriority(100);
		m_updateClouds.SetCallBack(&UpdateCloudsProc);
		scene->AddUpdateObject(&m_updateClouds);
	}	
}

int CGraphicQualityController::GetErrorCode(int& qualityMode)
{
	int ret = QUALITY_ERROR_NO_ERROR;

	int vendorCheckAnswer = g->cc.IsVendorSupported();
	if (vendorCheckAnswer != VENDOR_OK)
	{		
		switch(vendorCheckAnswer)
		{
		case DRIVERS_NOT_INSTALLED:						
			return QUALITY_ERROR_DRIVERS_NOT_INSTALLED;
			break;

		case VENDOR_UNSUPPORTED:
			ret = QUALITY_ERROR_VENDOR_UNSUPPORTED;
			break;

		case DRIVER_UNSUPPORTED_VERSION:
			return QUALITY_ERROR_DRIVER_UNSUPPORTED_VERSION;
			break;

		default:
			break;
		}
	}
	int tempQualityMode = qualityMode;

	if (((VERY_HIGH_QUALITY_MODE_ID == qualityMode) || (HIGH_QUALITY_MODE_ID == qualityMode) ||
		(MEDIUM_QUALITY_MODE_ID == qualityMode)) && (!g->ei.IsExtensionSupported(GLEXT_arb_shader_objects)))
	{
		return QUALITY_ERROR_MEMORY_SIZE;
	}

	if ((VERY_HIGH_QUALITY_MODE_ID == qualityMode) && ((g->gi.GetVideoMemory()->GetTotalSize() < 384 * 1024 * 1024)
		|| (g->gi.GetVendorID() == VENDOR_INTEL)))
	{
		qualityMode = HIGH_QUALITY_MODE_ID;
	}

	if ((HIGH_QUALITY_MODE_ID == qualityMode)&&(g->gi.GetVideoMemory()->GetTotalSize() < 250 * 1024 * 1024))
	{
		qualityMode = MEDIUM_QUALITY_MODE_ID;
	}

	if (((HIGH_QUALITY_MODE_ID == qualityMode) || (VERY_HIGH_QUALITY_MODE_ID == qualityMode)) && 
		(g->gi.IsLowEndVideocard()))
	{
		qualityMode = MEDIUM_QUALITY_MODE_ID;
	}

	if (tempQualityMode != qualityMode)
	{
		return QUALITY_ERROR_MEMORY_SIZE;
	}

	return QUALITY_ERROR_NO_ERROR;

}

int CGraphicQualityController::SetQualityMode(int qualityMode)
{
	int ret = GetErrorCode(qualityMode);

	int vendorCheckAnswer = g->cc.IsVendorSupported();
	if (ret == QUALITY_ERROR_NO_ERROR)
	{		
		m_qualityMode = qualityMode;
		TryToSetQualityMode(m_qualityMode);		
	}
	
	return ret;

}

void CGraphicQualityController::TryToSetInitialQualityMode()
{
	int mode = HIGH_QUALITY_MODE_ID;

	if ((g->gi.GetVendorID() == VENDOR_INTEL) || (g->gi.IsLowEndVideocard()) || (g->gi.IsION()))
	{
		mode = MEDIUM_QUALITY_MODE_ID;
	}

	TryToSetQualityMode(mode);
}

unsigned char CGraphicQualityController::AdjustMultisamplePower(unsigned char power)
{
	return power;
}

void CGraphicQualityController::TryToSetQualityMode(int qualityMode)
{
	int vendorCheckAnswer = g->cc.IsVendorSupported();
	if ((vendorCheckAnswer == VENDOR_OK) && (g->ei.IsExtensionSupported(GLEXT_arb_shader_objects)))
	{
		int err = GetErrorCode(qualityMode);
		if (( err == QUALITY_ERROR_NO_ERROR ) || ( err == QUALITY_ERROR_MEMORY_SIZE ))
		{
			switch (qualityMode)
			{
			case MEDIUM_QUALITY_MODE_ID:
				SwitchToMediumQualityMode();
				break;

			case HIGH_QUALITY_MODE_ID: 
				SwitchToHighQualityMode();
				break;

			case VERY_HIGH_QUALITY_MODE_ID:
				SwitchToVeryHighQualityMode();
				break;

			case LOW_QUALITY_MODE_ID:
				SwitchToLowQualityMode();
				break;
			} 
		}
		else 
			SwitchToLowQualityMode();
			 
	}
}
int CGraphicQualityController::GetQualityMode() const
{
	return m_qualityMode;
}


CHDRPostEffect* CGraphicQualityController::GetPostEffect() const
{
	return m_postEffect;
}

std::string CGraphicQualityController::GetDefaultValue(const unsigned int settingID)
{
	if ((settingID != USER_SETTING_SCREEN_WIDTH) && (settingID != USER_SETTING_SCREEN_HEIGHT) &&
		(settingID != USER_SETTING_DISTANCE_KOEF))
	{
		return "";
	}

	bool isVerySlowComputer = g->gi.IsLowEndVideocard();

	int offsetX, width, height;
	switch (settingID)
	{
	case USER_SETTING_SCREEN_WIDTH:
		{
			m_context->mpApp->GetDesktopSize(offsetX, width, height);
			return IntToStr(width);
		}
		
	case USER_SETTING_SCREEN_HEIGHT:
		{
			m_context->mpApp->GetDesktopSize(offsetX, width, height);
			return IntToStr(height);
		}

	case USER_SETTING_DISTANCE_KOEF:
		return isVerySlowComputer ? "200" : "100";
	}

	return "";
}

CGraphicQualityController::~CGraphicQualityController()
{
	if (m_postEffect)
	{
		MP_DELETE(m_postEffect);
	}

	DeleteSky();
}