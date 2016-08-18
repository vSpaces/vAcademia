
#pragma once

#include "BasePostEffect.h"
#include "FrameBuffer.h"
#include "LensFlares.h"

static float m_sunX, m_sunY;

class CHDRPostEffect : public CBasePostEffect, public IChangesListener
{
public:
	CHDRPostEffect();
	CHDRPostEffect(int fixedWidth, int fixedHeight);
	~CHDRPostEffect();

	void OnSceneDrawStart(CViewport* viewport);
	void OnSceneDrawEnd(CViewport* viewport, CRenderTarget* renderTarget = NULL);
	void OnLoadFromXML(std::wstring& fileName);
	void SaveScreen(std::wstring fileName, SCropRect* const rect);

	void OnChanged(int eventID);

	unsigned int GetRefractionTextureID()const;
	void BakeRefractionTexture();

	int GetMainTextureID()const;
	void DetachMainTexture();

	CShader* GetOculusRiftShader();

	void SetMultisamplePower(const unsigned char multisamplePower);
	unsigned char GetMultisamplePower()const;
	void PrepareForReading();	
	void EndReading();	

	static void SetSunProjection(float sunX, float sunY);

	bool IsOk();

private:
	void GetShaderName(void* document, std::wstring fileName, std::string name, std::string& shaderName);
	void GetTextureSize(unsigned int& textureWidth, unsigned int& textureHeight);

	// проверяет изменение разрешения окна и при наличие изменений присваивает новые значения размерам текстуры
	bool AreTextureSettingsChanged(); 

	// инициализация буферов
	void InitBuffers(); 
	void DeleteBuffers();

	void ComposeMaps(CViewport* viewport, CRenderTarget* renderTarget);

	void DoXBlur();
	void DoYBlur();
	void DoFilterMap();

	void InitOculusRiftSupport();

	CFrameBuffer* m_buffer;
	CFrameBuffer* m_buffer0;
	CFrameBuffer* m_buffer2;
	CFrameBuffer* m_buffer3;

	int m_buffer0TexID, m_buffer2TexID, m_buffer3TexID, m_bufferTexID;

	int m_xBlurShaderID;
	int m_yBlurShaderID;
	int m_filterMapShaderID;
	int m_toneMapShaderID;
	int m_toneMapOculusRiftShaderID;
	int m_toneMapShaftsShaderID;

	int m_fixedWidth;
	int m_fixedHeight;

	CShader* m_xBlurShader;
	CShader* m_yBlurShader;
	CShader* m_filterMapShader;
	CShader* m_toneMapShader;
	CShader* m_toneMapOculusRiftShader;
	CShader* m_toneMapShaftsShader;
	
	CLensFlares m_lensFlares;
	
	bool m_isFirstCopy;

	unsigned char m_multisamplePower;

	unsigned int m_textureWidth;
	unsigned int m_textureHeight;

	unsigned int m_sunLocation;

	unsigned int m_refractionTextureID;
	unsigned int m_refractionTextureWidth;
	unsigned int m_refractionTextureHeight;

	unsigned int m_zbufferTextureID;	

	bool m_isOk;
};