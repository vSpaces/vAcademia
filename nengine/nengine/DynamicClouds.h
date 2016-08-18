#pragma once

#include "CommonEngineHeader.h"

#include "Shader.h"
#include "Clouds.h"
#include "DisplayList.h"
#include "CubemapClouds.h"
#include "IChangesListener.h"

class CDynamicClouds : public IClouds, public IChangesListener
{
public:
	CDynamicClouds();
	~CDynamicClouds();

	void Destroy();

	void Draw();

	void SetSunTextureName(std::string name);
	void SetSunTextureID(int textureID);

	void SetStarsTextureName(std::string name);
	void SetStarsTextureID(int textureID);

	void Set3DNoiseTextureName(std::string name);
	void Set3DNoiseTextureID(int textureID);

	void SetShader(std::string shader);

	void OnChanged(int eventID);

private:
	CCubemapClouds m_cubemapClouds;

	int m_modelID;
	int m_sunTextureID;
	int m_starsTextureID;
	int m_noiseTextureID;

	int m_timeLocation;
	int m_alphaLocation;
	int	m_sunposLocation;
	
	MP_STRING m_shaderName;
	CShader* m_shader;

	int m_displayListID;
};