#pragma once

#include "Clouds.h"
#include "HDRPostEffect.h"
#include "UpdateCallback.h"
#include "IDefaultSettingValueProvider.h"

#include "oms_context.h"

class CGraphicQualityController : public IDefaultSettingValueProvider
{
public:
	CGraphicQualityController(oms::omsContext* context);
	~CGraphicQualityController();

	void Update();

	void CreateSkyIfNeeded(void* _scene);

	int SetQualityMode(int qualityMode);
	int GetQualityMode() const;
	
	void TryToSetQualityMode(int qualityMode);
	void TryToSetInitialQualityMode();

	CHDRPostEffect* GetPostEffect() const;

	std::string GetDefaultValue(const unsigned int settingID);

	unsigned char AdjustMultisamplePower(unsigned char power);

protected:
	CUpdateCallback m_updateClouds;
	IClouds* m_sky;
	CHDRPostEffect* m_postEffect;
	int	m_qualityMode;
	int m_deleteSkyCounter;

	oms::omsContext* m_context;

	void SwitchToLowQualityMode();
	void SwitchToMediumQualityMode();
	void SwitchToHighQualityMode();
	void SwitchToVeryHighQualityMode();


	void InitHighQualityMode(const bool isVeryHigh);

	void DeleteSky();

	int GetErrorCode(int& qualityMode);

};