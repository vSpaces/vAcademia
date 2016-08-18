
#pragma once

#include "Material.h"
#include "Viewport.h"
#include "ITextureSaver.h"

class CRenderTarget;

class CBasePostEffect
{
public:
	CBasePostEffect();
	~CBasePostEffect();

	void LoadFromXML(std::wstring fileName);

	virtual void OnSceneDrawStart(CViewport* viewport) = 0;
	virtual void OnSceneDrawEnd(CViewport* viewport, CRenderTarget* renderTarget) = 0;
	virtual void OnLoadFromXML(std::wstring& fileName) = 0;

	virtual void SaveScreen(std::wstring fileName, SCropRect* const rect) = 0;
	virtual void PrepareForReading() = 0;
	virtual void EndReading() = 0;

	virtual unsigned char GetMultisamplePower()const = 0;

private:
	int m_shaderID;
	CMaterial* m_shaderParams;
};