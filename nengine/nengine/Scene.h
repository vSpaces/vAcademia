#pragma once

#include "CommonEngineHeader.h"

#include "IOculusRiftSupport.h"
#include "RenderingCallback.h"
#include "BasePostEffect.h"
#include "SimplePostEffect.h"
#include "CommonObject.h"
#include "RenderTarget.h"
#include "SceneObject.h"
#include "ISceneObjectParticles.h"
#include "Viewport.h"
#include <vector>

#define RENDER_MODE_NORMAL				0
#define RENDER_MODE_FALLBACK_TO_RTT		1

#define MAX_PLAGIN_OBJECT_COUNT			1000

#define NUM_SPLITS						2

class CScene : public CCommonObject
{
public:
	CScene();
	~CScene();

	void AddPlugin(IPlugin* plugin);
	void DeletePlugin(IPlugin* plugin);

	void SetOculusRiftObject(IOculusRiftSupport* oculus);

	std::vector<IPlugin *>::iterator GetPluginsBegin();
	std::vector<IPlugin *>::iterator GetPluginsEnd();

	void PrepareForReading();
	void EndReading();

	void SetWireframeMode(bool isWireframe);
	bool IsWireframeEnabled()const;
	
	void AddUpdateObject(ISceneObject* updateObject);
	void AddSceneObject(ISceneObject* sceneObject);
	void AddParticles(ISceneObjectParticles* sceneObject);

	void RemoveUpdateObject(ISceneObject* updateObject);
	void RemoveSceneObject(ISceneObject* sceneObject);
	void RemoveParticles(ISceneObjectParticles* sceneObject);

	void SetViewport(CViewport* viewport);
	CViewport* GetViewport()const;

	void SetPostEffect(CBasePostEffect* postEffect);
	CBasePostEffect* GetPostEffect()const;

	void AddVisiblePluginObject(IPlugin* plugin);

	void Render();
	void Render(std::vector<C3DObject*>& selectedObjects);

	void AddObjectWithAxes(C3DObject* obj);
	void RemoveObjectWithAxes(C3DObject* obj);

	void NeedUnderMouseCheck();
	void DisableOnEnd3DEventsForNextFrame();

	void FreeResources();

	bool CreateShadowmapIfNeeded();

	unsigned int GetReflectionTextureID()const;

	void OnEndRendering2D();

	IOculusRiftSupport* GetOculusRift();

private:
	void MainRender(std::vector<C3DObject*>& selectedObjects, bool isUnderMouseCheckNeeded, bool isLeftEye = false);

	void RenderToShadowMap();
	void RenderReflectionTexture();

	void RenderParticles();
	static void SetParticlesDistanceAttenuation();

	void StartShadowmapping(bool isReprocessMatrices);

	CViewport* m_viewport;
	CBasePostEffect* m_postEffect;
	CSimplePostEffect* m_fallbackPostEffect;

	CRenderTarget* m_shadowMapRenderTarget[NUM_SPLITS];

	CRenderTarget* m_reflectionFBO;
	unsigned int m_reflectionTextureID;

	bool m_isWireframe;
	bool m_isNeedUnderMouseCheck;
	bool m_isDisableOnEnd3DEvents;

	float m_lightProjectionMatrix[2][16];
	float m_lightModelViewMatrix[16];

	int m_renderMode;
	int m_depthTextureID[NUM_SPLITS];
	int m_colorTextureID[NUM_SPLITS];

	__int64 m_lastUpdateTime;
	__int64 m_lastShadowMapUpdateTime;
	DWORD m_lastParticlesUpdateTime;
	int m_framesSkippedForParticlesUpdate;

	bool m_isShadowmapUpdateLocked;

	IPlugin* m_pluginObjects[MAX_PLAGIN_OBJECT_COUNT];
	int m_pluginObjectCount;

	CRenderingCallback m_lodSolidRenderingCallback, m_lodTransparentRenderingCallback;

	MP_VECTOR<ISceneObject*> m_objects;
	MP_VECTOR<ISceneObject*> m_updateObjects;
	MP_VECTOR<int> m_objectsID;
	MP_VECTOR<IPlugin *> m_allPlugins;

	MP_VECTOR<C3DObject*> m_objectsWithAxes;
	MP_VECTOR<ISceneObjectParticles*> m_particles;

	IOculusRiftSupport* m_oculusRiftObject;
};