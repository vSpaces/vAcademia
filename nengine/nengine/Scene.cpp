
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "Scene.h"
#include "__crashtest.h"
#include "miniglut.h"
#include <algorithm>

#define SHADOW_DELTA_DIST 2000.0f
//#define ONLY_CUBE	1

void LodSolidRenderingProc()
{
	g->lod.DrawAll();
}

void LodTransparentRenderingProc()
{
	g->lod.DrawAllWithoutUpdate();
}

CScene::CScene():
	m_isShadowmapUpdateLocked(false),
	m_renderMode(RENDER_MODE_NORMAL),
	m_isDisableOnEnd3DEvents(false),
	m_isNeedUnderMouseCheck(false),
	m_lastShadowMapUpdateTime(0),
	m_fallbackPostEffect(NULL),	
	m_oculusRiftObject(NULL),
	m_pluginObjectCount(0),
	m_reflectionFBO(NULL),
	m_isWireframe(false),	
	m_lastUpdateTime(0),
	m_lastParticlesUpdateTime(0),
	m_framesSkippedForParticlesUpdate(0),
	m_postEffect(NULL),
	m_viewport(NULL),
	MP_VECTOR_INIT(m_objects),
	MP_VECTOR_INIT(m_updateObjects),
	MP_VECTOR_INIT(m_objectsID),
	MP_VECTOR_INIT(m_allPlugins),
	MP_VECTOR_INIT(m_objectsWithAxes),
	MP_VECTOR_INIT(m_particles)
{
	m_shadowMapRenderTarget[0] = NULL;
	m_shadowMapRenderTarget[1] = NULL;

	m_lodSolidRenderingCallback.SetRenderPriority(0);
	m_lodSolidRenderingCallback.SetRenderMode(MODE_RENDER_SOLID);
	m_lodSolidRenderingCallback.SetCallBack(&LodSolidRenderingProc);

	m_lodTransparentRenderingCallback.SetRenderPriority(0);
	m_lodTransparentRenderingCallback.SetRenderMode(MODE_RENDER_TRANSPARENT);
	m_lodTransparentRenderingCallback.SetCallBack(&LodTransparentRenderingProc);

	AddSceneObject(&m_lodSolidRenderingCallback);
	AddSceneObject(&m_lodTransparentRenderingCallback);
}

void CScene::AddObjectWithAxes(C3DObject* obj)
{
	MP_VECTOR<C3DObject*>::iterator it = m_objectsWithAxes.begin();
	MP_VECTOR<C3DObject*>::iterator itEnd = m_objectsWithAxes.end();

	for ( ; it != itEnd; it++)
	if (*it == obj)
	{
		return;
	}

	m_objectsWithAxes.push_back(obj);
}

IOculusRiftSupport* CScene::GetOculusRift()
{
	if (!g->rs.IsOculusRiftEnabled())
	{
		return NULL;
	}

	return m_oculusRiftObject;
}
	
void CScene::RemoveObjectWithAxes(C3DObject* obj)
{
	MP_VECTOR<C3DObject*>::iterator it = m_objectsWithAxes.begin();
	MP_VECTOR<C3DObject*>::iterator itEnd = m_objectsWithAxes.end();

	for ( ; it != itEnd; it++)
	if (*it == obj)
	{
		m_objectsWithAxes.erase(it);
	}
}

void CScene::PrepareForReading()
{
	if (m_postEffect != NULL)
	{
		m_postEffect->PrepareForReading();
	}
}

void CScene::EndReading()
{
	if (m_postEffect != NULL)
	{
		m_postEffect->EndReading();
	}
}

void CScene::SetWireframeMode(bool isWireframe)
{
	m_isWireframe = isWireframe;
}

bool CScene::IsWireframeEnabled()const
{
	return m_isWireframe;
}

bool CScene::CreateShadowmapIfNeeded()
{
#ifdef SHADOWMAPS
	if ((!m_shadowMapRenderTarget[0]) || (m_shadowMapRenderTarget[0]->GetWidth() != g->rs.GetInt(SHADOWMAP_SIZE)))
	{
		try
		{
			if (m_shadowMapRenderTarget[0])
			{
				for (int i = 0; i < NUM_SPLITS; i++)
				{
					MP_DELETE(m_shadowMapRenderTarget[i]);
					m_shadowMapRenderTarget[i] = NULL;
					GLFUNC(glDeleteTextures)(1, (GLuint*)&m_depthTextureID[i]);

					if ((g->gi.GetVendorID() == VENDOR_ATI) && ((g->ci.GetOSVersionString().find("6.0") != -1)
						|| (g->ci.GetOSVersionString().find("6.1") != -1)))
					{
						GLFUNC(glDeleteTextures)(1, (GLuint*)&m_colorTextureID[i]);
					}
				}
			}

			for (int i = 0; i < NUM_SPLITS; i++)
			{
				int size = (int)(g->rs.GetInt(SHADOWMAP_SIZE) / (i + 1));
				MP_NEW_V3(m_shadowMapRenderTarget[i], CRenderTarget, size, size, CFrameBuffer::depth32);
				m_shadowMapRenderTarget[i]->Create(false);
				m_shadowMapRenderTarget[i]->Bind();
				int depthFormat = GL_DEPTH_COMPONENT32;

				switch (g->rs.GetInt(SHADOWMAP_BITS))
				{
				case 24:
					depthFormat = GL_DEPTH_COMPONENT24;
					break;

				case 16:
					depthFormat = GL_DEPTH_COMPONENT16;
					break;
				}

				m_depthTextureID[i] = m_shadowMapRenderTarget[i]->CreateDepthTexture(depthFormat, DEPTH_COMPARE_R_LEQUAL);
				m_shadowMapRenderTarget[i]->AttachDepthTexture(m_depthTextureID[i]);
				if ((g->gi.GetVendorID() == VENDOR_ATI) && ((g->ci.GetOSVersionString().find("6.0") != -1)
					|| (g->ci.GetOSVersionString().find("6.1") != -1)))
				{
					m_colorTextureID[i] = m_shadowMapRenderTarget[i]->CreateColorTexture(GL_ALPHA, GL_ALPHA8);
					m_shadowMapRenderTarget[i]->AttachColorTexture(m_colorTextureID[i]);
				}
				else
				{
					GLFUNC(glDrawBuffer)(GL_NONE);
					GLFUNC(glReadBuffer)(GL_NONE);
				}
				
				GLFUNC(glClearDepth)(1.0f);
				GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
				m_shadowMapRenderTarget[i]->Unbind();
			}
		}
		catch (...)
		{
			return false;
		}
	}

	return true;
#endif
}

void CScene::DisableOnEnd3DEventsForNextFrame()
{
	m_isDisableOnEnd3DEvents = true;
}

void CScene::NeedUnderMouseCheck()
{
	m_isNeedUnderMouseCheck = true;
}

void CScene::SetViewport(CViewport* viewport)
{
	m_viewport = viewport;
}

CViewport* CScene::GetViewport()const
{
	return m_viewport;	
}

void CScene::SetPostEffect(CBasePostEffect* postEffect)
{
	m_postEffect = postEffect;
}

CBasePostEffect* CScene::GetPostEffect()const
{
	return m_postEffect;
}
	
void CScene::AddSceneObject(ISceneObject* sceneObject)
{
	m_objects.push_back(sceneObject);
	m_objectsID.push_back(m_objects.size() - 1);
}

void CScene::RemoveSceneObject(ISceneObject* sceneObject)
{ 
	std::vector<ISceneObject*>::iterator it = m_objects.begin();
	std::vector<int>::iterator itID = m_objectsID.begin();
	std::vector<ISceneObject*>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++, itID++)
	if (*it == sceneObject)
	{
		m_objects.erase(it);
		m_objectsID.erase(itID);
		break;
	}
}

void CScene::AddUpdateObject(ISceneObject* updateObject)
{
	m_updateObjects.push_back(updateObject);
}

void CScene::RemoveUpdateObject(ISceneObject* updateObject)
{
	std::vector<ISceneObject*>::iterator it = m_updateObjects.begin();
	std::vector<ISceneObject*>::iterator itEnd = m_updateObjects.end();

	for ( ; it != itEnd; it++)
	if (*it == updateObject)
	{
		m_updateObjects.erase(it);
	}
}

void CScene::AddParticles(ISceneObjectParticles* sceneObject)
{
	m_particles.push_back(sceneObject);
}

void CScene::RemoveParticles(ISceneObjectParticles* sceneObject) 
{
	std::vector<ISceneObjectParticles*>::iterator it = m_particles.begin();
	std::vector<ISceneObjectParticles*>::iterator itEnd = m_particles.end();

	for ( ; it != itEnd; it++)
	if (*it == sceneObject)
	{
		m_particles.erase(it);
	}
}

std::vector<IPlugin *>::iterator CScene::GetPluginsBegin()
{
	return m_allPlugins.begin();
}

std::vector<IPlugin *>::iterator CScene::GetPluginsEnd()
{
	return m_allPlugins.end();
}

void CScene::AddPlugin(IPlugin* plugin)
{
	m_allPlugins.push_back(plugin);
}

void CScene::DeletePlugin(IPlugin* plugin)
{
	std::vector<IPlugin *>::iterator it = m_allPlugins.begin();
	std::vector<IPlugin *>::iterator itEnd = m_allPlugins.end();

	for ( ; it != itEnd; it++)
	if (*it == plugin)
	{
		m_allPlugins.erase(it);
		return;
	}
}

void CScene::AddVisiblePluginObject(IPlugin* plugin)
{
	m_pluginObjects[m_pluginObjectCount] = plugin;
	m_pluginObjectCount++;
}

void CScene::RenderToShadowMap()
{
	static __int64 lastShadowMapFullUpdateTime = 0;

	m_isShadowmapUpdateLocked = false;

	bool isUpdateOnlyMainShadowmap = false;

	if ((!g->lod.IsCameraChanged()) || (g->rs.IsOculusRiftEnabled()))
	{
		if (g->ne.time - m_lastShadowMapUpdateTime < g->rs.GetInt(SHADOWMAP_MIN_UPDATE_TIME))
		{
			m_isShadowmapUpdateLocked = true;
		}
	}
	else
	{
		// do nothing, because of shadow flickering
	}
	
	if (g->ne.time - m_lastShadowMapUpdateTime >= g->rs.GetInt(SHADOWMAP_MIN_UPDATE_TIME))
	if (!g->lod.IsCameraChanged())
	{
		isUpdateOnlyMainShadowmap = true;
	}

	if (m_isShadowmapUpdateLocked)
	{
		g->sm.SetDefineValue(SHADER_DEFINE_SHADOWMAP, true);
		g->sm.BindDefines();
		//m_isShadowmapUpdateLocked = false;

		return;
	}

	g_isShadowsNeeded = true;

	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();

	m_lastShadowMapUpdateTime = g->ne.time;

	CreateShadowmapIfNeeded();

	g->stp.SetDefaultStates();

	int lastMode = g->ne.GetWorkMode();
	unsigned int currentFrameBufferID = 0;
	if (lastMode == WORK_MODE_SNAPSHOTRENDER)
	{
		GLFUNC(glGetIntegerv)(GL_FRAMEBUFFER_BINDING_EXT,(int *)&currentFrameBufferID);
	}

	g->ne.SetWorkMode(WORK_MODE_SHADOWMAPRENDER);	
	
	g->sm.SetDefineValue(SHADER_DEFINE_SHADOWMAP, false);
	g->sm.BindDefines();

	int startShadowIndex = NUM_SPLITS - 1;

	if (isUpdateOnlyMainShadowmap)
	{
		startShadowIndex = 0;
	}
	else
	{
		lastShadowMapFullUpdateTime = g->ne.time;
	}

	for (int i = startShadowIndex; i >= 0; i--)
	{
		g->stp.PushMatrix();
		m_shadowMapRenderTarget[i]->Bind();

		g->stp.PrepareForRender();

		GLFUNC(glClearDepth)(1.0f);
		GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
		g->stp.SetViewport(0, 0, m_shadowMapRenderTarget[i]->GetWidth(), m_shadowMapRenderTarget[i]->GetHeight());
	
		// Сдвиг полигонов - нужен для того, чтобы не было z-fighting'а
		GLFUNC(glEnable)(GL_POLYGON_OFFSET_FILL);
		GLFUNC(glPolygonOffset)(2.0, (GLfloat)g->rs.GetInt(SHADOWMAP_BIAS));

		// Установить матрицы камеры света
		GLFUNC(glMatrixMode)(GL_PROJECTION);
		GLFUNC(glLoadIdentity)();
		CVector3D look;
		g->cm.GetActiveCamera()->GetLookAt(&look.x, &look.y, &look.z);
		const float sf = 2048.0f + float(i) * 4096.0f * 1.5f;
		GLFUNC(glOrtho)(-sf, sf, -sf, sf, g->rs.GetInt(SHADOWMAP_NEAR_PLANE), g->rs.GetInt(SHADOWMAP_FAR_PLANE));
		GLFUNC(glMatrixMode)(GL_MODELVIEW);
		GLFUNC(glLoadIdentity)();
		CVector3D eye;
		g->cm.GetActiveCamera()->GetEyePosition(&eye.x, &eye.y, &eye.z);	
		CVector3D delta(0.0f, 0.0f, 0.0f);
		delta = look;
		delta -= eye;
		delta.z = 0.0f;
		delta.Normalize();
		delta *= SHADOW_DELTA_DIST;
		GLFUNC(gluLookAt)(delta.x + eye.x + g->rs.GetInt(SHADOWMAP_SHIFT_XY)/* * 0.25f*/, delta.y + eye.y + g->rs.GetInt(SHADOWMAP_SHIFT_XY)/* * 0.25f*/, eye.z + g->rs.GetInt(SHADOWMAP_SHIFT_Z), delta.x + eye.x, delta.y + eye.y, eye.z, 0, 1, 0);
	
		// Сохраняем эти матрицы, они нам понадобятся для расчёта матрицы света
		GLFUNC(glGetFloatv)(GL_PROJECTION_MATRIX, &m_lightProjectionMatrix[i][0]);
		if (0 == i)
		{
			GLFUNC(glGetFloatv)(GL_MODELVIEW_MATRIX, &m_lightModelViewMatrix[0]);
		}

		g->stp.SetState(ST_ZWRITE, true);
		g->stp.SetState(ST_ZTEST, true);		

		GLFUNC(glColorMask)(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		g->mr.SetMode(MODE_RENDER_SOLID + MODE_RENDER_ALPHATEST);

		g->stp.PrepareForRender();

		//g->stp.CompareStates(i);

		g->shdw.RenderShadowCasters(i == 0);
		g->mr.SetMode(MODE_RENDER_ALL);
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
		GLFUNC(glDisable)(GL_POLYGON_OFFSET_FILL);
		g->stp.RestoreViewport();
		m_shadowMapRenderTarget[i]->Unbind();
		g->stp.PopMatrix();

		g->stp.SetDefaultStates();
	}

	if (lastMode == WORK_MODE_SNAPSHOTRENDER)
	{
		if (currentFrameBufferID != 0)
		{
			GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, currentFrameBufferID);
		}
	}
	
	g->stp.SetDefaultStates();

	g->sm.SetDefineValue(SHADER_DEFINE_SHADOWMAP, true);
	g->sm.BindDefines();
	g->sm.OnNewFrame(false);

	g->ne.SetWorkMode(lastMode);
}

void InverseMatrix(float* dst, float* src)
{
	dst[0] = src[0];
	dst[1] = src[4];
	dst[2] = src[8];
	dst[3] = 0.0;
	dst[4] = src[1];
	dst[5] = src[5];
	dst[6]  = src[9];
	dst[7] = 0.0;
	dst[8] = src[2];
	dst[9] = src[6];
	dst[10] = src[10];
	dst[11] = 0.0;
	dst[12] = -(src[12] * src[0]) - (src[13] * src[1]) - (src[14] * src[2]);
	dst[13] = -(src[12] * src[4]) - (src[13] * src[5]) - (src[14] * src[6]);
	dst[14] = -(src[12] * src[8]) - (src[13] * src[9]) - (src[14] * src[10]);
	dst[15] = 1.0;
}

void CScene::StartShadowmapping(bool isReprocessMatrices)
{
	float cameraModelViewMatrix[16];
	float cameraInverseModelViewMatrix[16]; 
	float lightMatrix[NUM_SPLITS][16];

	if (isReprocessMatrices)
	{
		// Сохраняем матрицы, они нам нужны для вычисления освещения
		// Инвертированная матрица используется в расчёте матрицы источника света		
		GLFUNC(glGetFloatv)(GL_MODELVIEW_MATRIX, &cameraModelViewMatrix[0]);
		InverseMatrix(cameraInverseModelViewMatrix, cameraModelViewMatrix);

		// Вычисляем матрицу источника света
		for (int i = 0; i < NUM_SPLITS; i++)
		{
			g->stp.PushMatrix();
			GLFUNC(glLoadIdentity)();
			GLFUNC(glTranslatef)(0.5, 0.5, 0.5); // + 0.5
			GLFUNC(glScalef)(0.5, 0.5, 0.5); // * 0.5
			GLFUNC(glMultMatrixf)(&m_lightProjectionMatrix[i][0]);
			GLFUNC(glMultMatrixf)(m_lightModelViewMatrix);
			GLFUNC(glMultMatrixf)(cameraInverseModelViewMatrix);
			GLFUNC(glGetFloatv)(GL_MODELVIEW_MATRIX, &lightMatrix[i][0]);
			g->stp.PopMatrix();
		}
	}

	g->matm.SetDepthTextureID(m_depthTextureID[0], m_depthTextureID[1]);
	g->sm.SetLightMatrix(lightMatrix[0], lightMatrix[1]);
}

void CScene::Render()
{	
	Render(m_objectsWithAxes);
}

void CScene::MainRender(std::vector<C3DObject*>& selectedObjects, bool isUnderMouseCheckNeeded, bool isLeftEye)
{
	#ifndef ONLY_CUBE

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-4");
#endif

	static bool isNeedSaveShadows = false;

#ifdef SHADOWMAPS
		g_isShadowsNeeded = false;
	if (g->rs.GetBool(SHADOWS_ENABLED))
	{
		if (isNeedSaveShadows)
		{
			g->sm.SetDefineValue(SHADER_DEFINE_SHADOWMAP, true);
			g->sm.BindDefines();
		}
		else
		{
			RenderToShadowMap();
		}
	}
	else
	{
		g->sm.SetDefineValue(SHADER_DEFINE_SHADOWMAP, false);
		g->sm.BindDefines();		
	}
#endif

	isNeedSaveShadows = (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER) && (!g->rs.GetBool(SHADOWS_ENABLED));

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-5");
#endif

#ifdef SHADOWMAPS
	if (g->rs.GetBool(SHADOWS_ENABLED))
	{
		StartShadowmapping(true);				
		if (GetOculusRift())
		{
			GetOculusRift()->SetLeftEye(isLeftEye);
			//g->cm.GetActiveCamera()->SetLeftEye(isLeftEye);
			m_viewport->BindCamera(isUnderMouseCheckNeeded, g->stp.GetWidth() / 2.0f, (float)g->stp.GetHeight());
		}
		else if (m_viewport)
		{
			m_viewport->Bind(isUnderMouseCheckNeeded);
		}
	}
#endif

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-6");
#endif

//	g->cm.SetCamera(-50230, 9482, 97, -50165, 9408, 75, 0, 0, 1);
	
	g->cm.ExtractFrustum();

	g->lod.OnNewFrame();

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-7");
#endif

	if (!isLeftEye)
	if ((m_postEffect) && (m_fallbackPostEffect) && (m_postEffect->GetMultisamplePower() != 1))
	{
		MP_DELETE(m_fallbackPostEffect);
		m_fallbackPostEffect = NULL;
		m_renderMode = RENDER_MODE_NORMAL;
	}

	if (!isLeftEye)
	if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
	if ((m_postEffect) && (!m_fallbackPostEffect))
	{
		m_postEffect->OnSceneDrawStart(m_viewport);
		if (GetOculusRift())
		{
			GetOculusRift()->SetLeftEye(isLeftEye);
			m_viewport->BindCamera(isUnderMouseCheckNeeded, g->stp.GetWidth() / 2.0f, (float)g->stp.GetHeight());
		}
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-8");
#endif

	if ((m_renderMode) && ((!m_postEffect) || (g->rs.GetInt(RENDER_TARGET_WITH_STENCIL_MODE) == RENDER_TARGET_WITH_STENCIL_COPY_FROM_FRAMEBUFFER)))
	if (g->cc.AreExtendedAbilitiesSupported())
	{
		if (!m_fallbackPostEffect)
		{
			m_fallbackPostEffect = MP_NEW(CSimplePostEffect);
		}

		m_fallbackPostEffect->OnSceneDrawStart(m_viewport);
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-9");
#endif

	if (m_isWireframe)
	{
		GLFUNC(glPolygonMode)(GL_FRONT_AND_BACK, GL_LINE);
	}	

	m_objectsID.reserve(m_objects.size());

	int count = m_objects.size();
	int i;
	for (i = 0; i < count; i++)
	{
		m_objectsID[i] = i;
	}

	for (i = 0; i < count - 1; i++)
	{
		int l = i;

		for (int k = i + 1; k < count; k++)
		if (m_objects[m_objectsID[k]]->GetRenderPriority() > m_objects[m_objectsID[l]]->GetRenderPriority())
		{
			l = k;
		}

		int tmp = m_objectsID[l];
		m_objectsID[l] = m_objectsID[i];
		m_objectsID[i] = tmp;
	}

	g->mr.SetMode(MODE_RENDER_SOLID);

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-10");
#endif

	// z-only pass

	if (g->rs.GetBool(ZONLY_PASS))
	{
		GLFUNC(glColorMask)(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		GLFUNC(glDisable)(GL_TEXTURE_2D);
		g->tm.DisableTexturing();
		g->sm.DisableShadering();

		g->stp.SetState(ST_CULL_FACE, true);

		for (i = 0; i < count; i++)
		if (m_objects[m_objectsID[i]]->GetRenderMode() & MODE_RENDER_SOLID)
		{
			m_objects[m_objectsID[i]]->Draw();
		}

		g->tm.EnableTexturing();
		g->sm.EnableShadering();
		GLFUNC(glEnable)(GL_TEXTURE_2D);
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	// main pass

	if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
	{
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	}

	g->stp.SetState(ST_DEPTH_FUNC, GL_LEQUAL);
	g->stp.SetState(ST_CULL_FACE, true);
	
	for (i = 0; i < count; i++)
	if (m_objects[m_objectsID[i]]->GetRenderMode() & MODE_RENDER_SOLID)
	{
		m_objects[m_objectsID[i]]->Draw();
	}

	if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
	{
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-11");
#endif	

	g->lod.DrawAllAlphaTest();	

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-12");
#endif	
	
	g->mr.SetMode(MODE_RENDER_TRANSPARENT);

	g->lod.DrawAllTransparent();

	RenderReflectionTexture();
	
	if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
	{
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	}
	RenderParticles();
	if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
	{
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-13");
#endif	

	g->mr.SetMode(MODE_RENDER_ALL);

	if (g->rs.GetBool(DEBUG_PHYSICS_OBJECTS))
	{
		g->phom.DrawDebugInfo();
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-14");
#endif	

	// отрисовка осей у выделенных объектов
	// + линии плоскостей если объект один
	if (selectedObjects.size() > 0)
	{
		if (g->rs.GetBool(SHADERS_USING))
		{
			g->sm.UnbindShader();
		}

		for (unsigned int objectIndex = 0; objectIndex < selectedObjects.size(); objectIndex ++)
		{
			C3DObject* obj = selectedObjects[objectIndex];
			if (!obj->IsVisible())
			{
				continue;
			}

			GLFUNC(glDisable)(GL_TEXTURE_2D);

			g->stp.PushMatrix();

			float x, y, z;
			obj->GetCoords(&x, &y, &z);
			GLFUNC(glTranslatef)(x, y, z);

			bool isLocalCoordSystem = (m_objectsWithAxes.size() > 0);

			if (isLocalCoordSystem)
			{
				obj->GetRotation()->ApplyRotation();
			}

			float axesLength = 40;
			float axesPlaneWidth = axesLength/2;

			g->stp.SetState(ST_ZTEST, false);
			//OX RED
			g->stp.SetColor(255, 0, 0);
			g->stp.PrepareForRender();
			GLFUNC(glLineWidth)(1);
			GLFUNC(glBegin)(GL_LINES);

			GLFUNC(glVertex3f)(0.0f,0.0f,0.0f);
			GLFUNC(glVertex3f)(axesLength,0.0f,0.0f); 
			
			if(selectedObjects.size() == 1)
			{
				GLFUNC(glVertex3f)(axesPlaneWidth,0.0f,0.0f);
				GLFUNC(glVertex3f)(axesPlaneWidth,axesPlaneWidth,0.0f); 		

				GLFUNC(glVertex3f)(axesPlaneWidth,0.0f,0.0f);
				GLFUNC(glVertex3f)(axesPlaneWidth,0.0f,axesPlaneWidth); 		
			}

			GLFUNC(glEnd)();
			g->stp.PushMatrix();
			GLFUNC(glTranslatef)(axesLength, 0, 0);
			GLFUNC(glRotatef)(90.0f, 0.0f, 1.0f, 0.0f);
			g->stp.PrepareForRender();
			miniglut::glutSolidCone(4.0f, 20.0f, 6, 1);
			g->stp.PopMatrix();	

			// OY GREEN
			g->stp.SetColor(0, 255, 0);
			g->stp.PrepareForRender();
			GLFUNC(glBegin)(GL_LINES);

			GLFUNC(glVertex3f)(0.0f,0.0f,0.0f);
			GLFUNC(glVertex3f)(0.0f,axesLength,0.0f);

			if(selectedObjects.size() == 1)
			{
				GLFUNC(glVertex3f)(0.0f,axesPlaneWidth,0.0f);
				GLFUNC(glVertex3f)(axesPlaneWidth,axesPlaneWidth,0.0f);

				GLFUNC(glVertex3f)(0.0f,axesPlaneWidth,0.0f);
				GLFUNC(glVertex3f)(0.0f,axesPlaneWidth,axesPlaneWidth);
			}

			GLFUNC(glEnd)();

			g->stp.PushMatrix();	
			GLFUNC(glTranslatef)(0, axesLength, 0);
			GLFUNC(glRotatef)(-90.0f, 1.0f, 0.0f, 0.0f);
			g->stp.PrepareForRender();
			miniglut::glutSolidCone(4.0f, 20.0f, 6, 1);
			g->stp.PopMatrix();	

			// OZ BLUE
			g->stp.SetColor(0, 0, 255);
			g->stp.PrepareForRender();
			GLFUNC(glBegin)(GL_LINES);

			GLFUNC(glVertex3f)(0.0f,0.0f,0.0f);
			GLFUNC(glVertex3f)(0.0f,0.0f,axesLength);

			if(selectedObjects.size() == 1)
			{
				GLFUNC(glVertex3f)(0.0f,0.0f,axesPlaneWidth);
				GLFUNC(glVertex3f)(0.0f,axesPlaneWidth,axesPlaneWidth);

				GLFUNC(glVertex3f)(0.0f,0.0f,axesPlaneWidth);
				GLFUNC(glVertex3f)(axesPlaneWidth,0.0f,axesPlaneWidth);
			}

			GLFUNC(glEnd)();
			g->stp.PushMatrix();
			GLFUNC(glTranslatef)(0, 0, axesLength);
			g->stp.PrepareForRender();
			miniglut::glutSolidCone(4.0f, 20.0f, 6, 1);
			g->stp.PopMatrix();	

			if(selectedObjects.size() == 1)
			{
				// planes
				unsigned char planeAlpha = 64;
				g->stp.SetColor(255, 255, 0, planeAlpha);
				g->stp.SetState(ST_BLEND, true);
				g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				g->stp.PrepareForRender();
				GLFUNC(glBegin)(GL_QUADS);

				GLFUNC(glVertex3f)(0.0f, 0.0f, 0.0f);
				GLFUNC(glVertex3f)(axesPlaneWidth - 1, 0.0f, 0.0f);
				GLFUNC(glVertex3f)(axesPlaneWidth - 1, axesPlaneWidth - 1, 0.0f);
				GLFUNC(glVertex3f)(0.0f, axesPlaneWidth - 1, 0.0f);

				GLFUNC(glVertex3f)(0.0f, 0.0f, 0.0f);
				GLFUNC(glVertex3f)(axesPlaneWidth - 1, 0.0f, 0.0f);
				GLFUNC(glVertex3f)(axesPlaneWidth - 1, 0.0f, axesPlaneWidth - 1);
				GLFUNC(glVertex3f)(0.0f, 0.0f, axesPlaneWidth - 1);

				GLFUNC(glVertex3f)(0.0f, 0.0f, 0.0f);
				GLFUNC(glVertex3f)(0.0f, axesPlaneWidth - 1, 0.0f);
				GLFUNC(glVertex3f)(0.0f, axesPlaneWidth - 1, axesPlaneWidth - 1);
				GLFUNC(glVertex3f)(0.0f, 0.0f, axesPlaneWidth - 1);

				GLFUNC(glEnd)();

				g->stp.SetState(ST_BLEND, false);
				//
			}

			if (g->sc2d.GetActive2DScene())
			{
				float _axesLength = axesLength * 7.0f / 5.0f;

				CVector3D xAxis(_axesLength, 0, 0);
				CVector3D yAxis(0, _axesLength, 0);
				CVector3D zAxis(0, 0, _axesLength);

				if (isLocalCoordSystem)
				{
					xAxis *= obj->GetRotation()->GetAsDXQuaternion();
					yAxis *= obj->GetRotation()->GetAsDXQuaternion();
					zAxis *= obj->GetRotation()->GetAsDXQuaternion();
				}

				float xxX, xxY;
				m_viewport->GetScreenCoords(x + xAxis.x, y + xAxis.y, z + xAxis.z, &xxX, &xxY);
				g->sc2d.GetActive2DScene()->AddHelperText(L"X", (int)xxX, (int)xxY, 255, 0, 0);
				m_viewport->GetScreenCoords(x + yAxis.x, y + yAxis.y, z + yAxis.z, &xxX, &xxY);
				g->sc2d.GetActive2DScene()->AddHelperText(L"Y", (int)xxX, (int)xxY, 0, 255, 0);
				m_viewport->GetScreenCoords(x + zAxis.x, y + zAxis.y, z + zAxis.z, &xxX, &xxY);
				g->sc2d.GetActive2DScene()->AddHelperText(L"Z", (int)xxX, (int)xxY, 0, 0, 255);
			}

			g->stp.SetState(ST_ZTEST, true);
			GLFUNC(glEnable)(GL_TEXTURE_2D);
			g->stp.PopMatrix();			
		}
	}	

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-15");
#endif	

	if (g->rs.GetBool(DRAW_ZONES_BOUNDS))
	{
		#define STATIC_ZONE_SIZE (20000.0f)
		for (int x = -5; x <= 5; x++)
		{
			for (int y = -5; y <= 5; y++)
			{
				GLFUNC(glPushMatrix)();
				GLFUNC(glTranslatef)(x * STATIC_ZONE_SIZE - 70000.0f, y * STATIC_ZONE_SIZE + 10000.0f, 700.0f);
				GLFUNC(glScalef)(1, 1, 0.01f);
				GLFUNC(glDisable)(GL_TEXTURE_2D);
				g->stp.SetColor(255, 255, 255);
				g->stp.PrepareForRender();
				miniglut::glutWireCube(STATIC_ZONE_SIZE);
				GLFUNC(glEnable)(GL_TEXTURE_2D);
				GLFUNC(glPopMatrix)();
			}
		}
		#define SYNC_ZONE_SIZE (10000.0f)
		for (int x = -10; x <= 10; x++)
		{
			for (int y = -10; y <= 10; y++)
			{
				GLFUNC(glPushMatrix)();
				GLFUNC(glTranslatef)(x * SYNC_ZONE_SIZE - 75000.0f, y * SYNC_ZONE_SIZE + 15000.0f, 710.0f);
				GLFUNC(glScalef)(1, 1, 0.007f);
				GLFUNC(glDisable)(GL_TEXTURE_2D);
				g->stp.SetColor(0, 255, 255);
				g->stp.PrepareForRender();
				miniglut::glutWireCube(SYNC_ZONE_SIZE);
				GLFUNC(glEnable)(GL_TEXTURE_2D);
				GLFUNC(glPopMatrix)();
			}
		}
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-16");
#endif	

	std::vector<ISceneObject*>::iterator it = m_updateObjects.begin();
	std::vector<ISceneObject*>::iterator itEnd = m_updateObjects.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->Draw();
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-17");
#endif	

	if (m_isWireframe)
	{
		GLFUNC(glPolygonMode)(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (isUnderMouseCheckNeeded)
	if (m_viewport)
	{
		POINT tmp;
		GetCursorPos(&tmp);
		ScreenToClient(g->ne.ghWnd, &tmp);
		int mouseX = tmp.x;
		int mouseY = tmp.y;
		float posX, posY, posZ;

		RECT rct;
		GetClientRect(g->ne.ghWnd, &rct);	
		
		if ((!g->rs.IsOculusRiftEnabled()) || (((isLeftEye) && (mouseX < (int)g->stp.GetCurrentWidth() / 2)) ||
				((!isLeftEye) && (mouseX > (int)g->stp.GetCurrentWidth() / 2))))
		if ((rct.bottom - rct.top == m_viewport->GetHeight()) && (rct.right - rct.left == m_viewport->GetWidth()))
		{			
			if (m_postEffect != NULL)
			{
				m_postEffect->PrepareForReading();
			}

			int res = 0;
			if (g->rs.IsOculusRiftEnabled())
			{
				int _x, _y, _w, _h;
				GetOculusRift()->GetViewportParams(_x, _y, _w, _h);				
				m_viewport->SetOculusHeight(_h);
				res = m_viewport->Get3DCoords(mouseX, mouseY, &posX, &posY, &posZ);				
			}
			else
			{				
				res = m_viewport->Get3DCoords(mouseX, mouseY, &posX, &posY, &posZ);			
			}

			if (res != GET_COORDS_NOTHING_UNDER_MOUSE)
			{
				m_viewport->SelectObjectUnderMouse(posX, posY, posZ, mouseX, mouseY, m_isNeedUnderMouseCheck);
				m_isNeedUnderMouseCheck = false;

				if ((GET_COORDS_UNSUPPORTED == res) && ((!m_postEffect) || (m_postEffect->GetMultisamplePower() == 1)))
				{
					if (m_renderMode != RENDER_MODE_FALLBACK_TO_RTT)
					{
						g->lw.WriteLn("[WARNING] Fallback to RTT scene render mode. MSAA = ", m_postEffect ? m_postEffect->GetMultisamplePower() : 0);
						m_renderMode = RENDER_MODE_FALLBACK_TO_RTT;
					}
				}
			}
			if (m_postEffect != NULL)
			{
				m_postEffect->EndReading();
			}			
		}
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-18");
#endif	

	g->lod.DrawAllUnpickable();

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-19");
#endif	

#ifdef SHADOWMAPS
	
#else
	g->shdw.RenderAllShadows();
#endif

#else
#define STATIC_ZONE_SIZE (20000.0f)
	for (int x = -5; x <= 5; x++)
		{
			for (int y = -5; y <= 5; y++)
			{
				for (int z = -5; z <= 5; z++)
				{
					GLFUNC(glPushMatrix)();
					GLFUNC(glTranslatef)(x * STATIC_ZONE_SIZE - 70000.0f, y * STATIC_ZONE_SIZE + 10000.0f, 700.0f + z * 500.0f);
					GLFUNC(glScalef)(1, 1, 1);
					//GLFUNC(glDisable)(GL_TEXTURE_2D);
					g->tm.BindTexture(g->tm.GetErrorTextureID());
					g->stp.SetColor((x%2 == 0) ? 255 : 0, (y%2 == 0) ? 255 : 0, (z%2 == 0) ? 255 : 0);
					g->stp.PrepareForRender();
					miniglut::glutSolidCube(400);				
					//GLFUNC(glEnable)(GL_TEXTURE_2D);
					GLFUNC(glPopMatrix)();
				}
			}
		}

		g->phom.GetControlledObject()->GetObject3D()->GetLODGroup()->Update();
		g->phom.GetControlledObject()->GetObject3D()->GetLODGroup()->ClearVisibilityCheckResults();
		g->phom.GetControlledObject()->GetObject3D()->GetLODGroup()->Draw();
#endif

	g->stp.PopMatrix();

#ifndef ONLY_CUBE

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-20");
#endif	

	if (!m_isDisableOnEnd3DEvents)
	{
		g->pm.CustomBlockStart(CUSTOM_BLOCK_END_RENDERING_3D);
		for (int k = 0; k < m_pluginObjectCount; k++)
		{
			if (g->rs.GetBool(SHADERS_USING))
			{
				g->sm.UnbindShader();
			}
			m_pluginObjects[k]->EndRendering3D();
		}
		g->pm.CustomBlockEnd(CUSTOM_BLOCK_END_RENDERING_3D);
	}	
}

void CScene::SetOculusRiftObject(IOculusRiftSupport* oculus)
{	
	m_oculusRiftObject = oculus;
	g->rs.SetOculusRiftEnabled(oculus != NULL);
	if (oculus == NULL)
	{
		m_viewport->SetOculusHeight(0);
		m_viewport->UpdateProjectionMatrix();
	}
}

void CScene::Render(std::vector<C3DObject*>& selectedObjects)
{
	/*if (g->rs.GetBool(CAVE_SUPPORT))
	{
		glDisable(GL_LIGHTING);
	}*/

	if (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER)
	{
		g->shdw.SaveShadowQueue();
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-1");
#endif

	if (g->rs.GetBool(SHADERS_USING))
	{
		g->sm.UnbindShader();
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-2");
#endif

	m_pluginObjectCount = 0;

	bool isUnderMouseCheckNeeded = false;
	if (g->rs.GetInt(UNDER_MOUSE_CHECK_DELTA_TIME) != 0)
	{
		if ((g->ne.time - m_lastUpdateTime) > g->rs.GetInt(UNDER_MOUSE_CHECK_DELTA_TIME) || g->ne.time < m_lastUpdateTime)
		{
			isUnderMouseCheckNeeded = true;
			m_lastUpdateTime = g->ne.time;
		}
	}
	else
	{
		isUnderMouseCheckNeeded = m_isNeedUnderMouseCheck;
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-3");
#endif

	g->stp.PushMatrix();

	if ((GetOculusRift()) && (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER))	
	{
		float yaw, pitch, roll;
		CVector3D pos;
		GetOculusRift()->BeginRender();
		GetOculusRift()->GetSensorPos(pos, yaw, pitch, roll);
		m_viewport->GetCamera()->SetAdditionalPos(pos * 1000, yaw, pitch, roll);

		GetOculusRift()->SetLeftEye(false);
		m_viewport->BindCamera(isUnderMouseCheckNeeded, g->stp.GetWidth() / 2.0f, (float)g->stp.GetHeight());
		MainRender(selectedObjects, isUnderMouseCheckNeeded, false);

		g->stp.PushMatrix();

		GetOculusRift()->SetLeftEye(true);
		m_viewport->BindCamera(isUnderMouseCheckNeeded, g->stp.GetWidth() / 2.0f, (float)g->stp.GetHeight());
		MainRender(selectedObjects, isUnderMouseCheckNeeded, true);
	}
	else if (m_viewport)
	{		
		m_viewport->Bind(isUnderMouseCheckNeeded);
		m_viewport->GetCamera()->ClearAdditionalPos();
		MainRender(selectedObjects, isUnderMouseCheckNeeded);		
	}



	/*static int frameCount = 0;
	frameCount++;

	if (frameCount > 2000)
	{
		std::string fileName = "c:\\frames\\";
		fileName += IntToStr(frameCount);
		fileName += ".jpg";
		if (m_postEffect)
		m_postEffect->SaveScreen(fileName);
	}*/

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-21");
#endif	

	if ((m_renderMode) && ((!m_postEffect) || (g->rs.GetInt(RENDER_TARGET_WITH_STENCIL_MODE) == RENDER_TARGET_WITH_STENCIL_COPY_FROM_FRAMEBUFFER)))
	{
		if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
		if (m_fallbackPostEffect)
		{
			m_fallbackPostEffect->OnSceneDrawEnd(m_viewport, NULL);
		}
	}

	if ((m_postEffect) && (!m_fallbackPostEffect))
	{
		m_postEffect->OnSceneDrawEnd(m_viewport, NULL);
	}

	if (!m_isDisableOnEnd3DEvents)
	{
		g->pm.CustomBlockStart(CUSTOM_BLOCK_END_RENDERING);
		for (int k = 0; k < m_pluginObjectCount; k++)
		{
			if (g->rs.GetBool(SHADERS_USING))
			{
				g->sm.UnbindShader();
			}
			g->tm.RefreshTextureOptimization();
			m_pluginObjects[k]->EndRendering();
			if (g->rs.GetBool(SHADERS_USING))
			{
				g->sm.UnbindShader();
			}
			g->tm.RefreshTextureOptimization();
		}
		g->pm.CustomBlockEnd(CUSTOM_BLOCK_END_RENDERING);
	}
	else
	{
		m_isDisableOnEnd3DEvents = false;
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-22");
#endif	

	g->phom.Update();

	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();

	if (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER)
	{
		g->shdw.RestoreShadowQueue();
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CScene::Render-23");
#endif	

#endif
}

unsigned int CScene::GetReflectionTextureID()const
{
	return m_reflectionTextureID;
}

void CScene::RenderReflectionTexture()
{	
	if (!g->rs.GetBool(REFLECTION_TEXTURE_NEEDED))
	{
		g->stp.SetMaterial(-1);
		g->stp.PrepareForRender();

		g->tm.RefreshTextureOptimization();

		return;
	}

	if (!m_reflectionFBO)
	{
		MP_NEW_V3(m_reflectionFBO, CRenderTarget, 512, 512, CFrameBuffer::depth32);		
		m_reflectionFBO->Create(true);
		m_reflectionFBO->Bind();
		m_reflectionTextureID = m_reflectionFBO->CreateColorTexture(GL_RGBA, GL_RGBA);
		m_reflectionFBO->AttachColorTexture(m_reflectionTextureID);
		m_reflectionFBO->Unbind();
		m_reflectionTextureID = g->tm.ManageGLTexture("reflection", m_reflectionTextureID, 512, 512);
	}

	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();

	g->tm.RefreshTextureOptimization();

	int currentFrameBufferID;
	GLFUNC(glGetIntegerv)(GL_FRAMEBUFFER_BINDING_EXT, &currentFrameBufferID);

	m_reflectionFBO->Bind();
	GLFUNC(glClearColor)(0.5f, 0.5f, 0.5f, 1.0f);
	GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	g->stp.PushMatrix();
	g->stp.SetViewport(0, 0, 512, 512);
	GLFUNC(glScalef)(1, 1, -1);

	GLFUNC(glEnable)(GL_CLIP_PLANE0);
	double plane[4] = {0, 0, -1, 0.0f};
	GLFUNC(glClipPlane)(GL_CLIP_PLANE0, (double *)&plane);

	g->stp.SetState(ST_FRONT_FACE, GL_CW);
	g->stp.SetState(ST_CULL_FACE, true);
	g->stp.PrepareForRender();
	g->mm.SetInvertCCWFlag(true);
	g->stp.SetDefaultStates();
	g->mr.SetMode(MODE_RENDER_SOLID);
	g->lod.OnNewFrame();
	g->lod.DrawAllWithoutUpdate();
	g->mr.SetMode(MODE_RENDER_ALPHATEST);
	g->lod.DrawAllAlphaTest();
	g->mr.SetMode(MODE_RENDER_TRANSPARENT);
	g->lod.ClearPrivelegedObjects();
	g->lod.DrawAllTransparent();
	g->mm.SetInvertCCWFlag(false);	

	GLFUNC(glDisable)(GL_CLIP_PLANE0);

	g->stp.RestoreViewport();
	g->stp.PopMatrix();
	m_reflectionFBO->Unbind();

	if (currentFrameBufferID != 0)
	{
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, currentFrameBufferID);
	}
}

void CScene::RenderParticles()
{
	if (m_particles.empty())
		return;

	//сортируем частицы по удаленности от камеры
	static struct CompareByDist {
		bool operator() (ISceneObjectParticles* a, ISceneObjectParticles* b) {
			return a->GetDistSq() > b->GetDistSq();
		}
	} compareByDist;

	std::vector<ISceneObjectParticles *>::iterator it = m_particles.begin();
	std::vector<ISceneObjectParticles *>::iterator itEnd = m_particles.end();
	for ( ; it != itEnd; it++)
	{
		(*it)->UpdateDistToCamera();
	}
	std::sort(m_particles.begin(), m_particles.end(), compareByDist);

	//выясняем, настало ли время обновлять частицы
	bool doUpdate = g->ne.time - m_lastParticlesUpdateTime > g->rs.GetInt(PARTICLES_UPDATE_TIME);
	if (doUpdate && m_framesSkippedForParticlesUpdate >= g->rs.GetInt(FRAMES_SKIPPED_FOR_PARTICLES_UPDATE)) {
		m_framesSkippedForParticlesUpdate = 0;
		m_lastParticlesUpdateTime = g->ne.time;
	} else {
		doUpdate = false;
		++m_framesSkippedForParticlesUpdate;
	}

	//обновляем (если можно) и рисуем
	GLFUNC(glEnable)( GL_DEPTH_TEST );
    GLFUNC(glDepthMask)( GL_FALSE );
	GLFUNC(glBlendEquation)(GL_FUNC_ADD_EXT);
	GLFUNC(glBlendFunc)(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLFUNC(glEnable)( GL_BLEND );

	GLFUNC(glTexEnvf)( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
	GLFUNC(glEnable)(GL_VERTEX_PROGRAM_POINT_SIZE);	
	GLFUNC(glEnable)(GL_POINT_SPRITE_ARB);

	if (!g->rs.GetBool(SHADERS_USING)) {
		SetParticlesDistanceAttenuation();
	}

	it = m_particles.begin();
	itEnd = m_particles.end();	
	for ( ; it != itEnd; it++)
	{
		if (doUpdate) 
			(*it)->Update(g->ne.time);
		(*it)->Draw();
	}
	
	GLFUNC(glDisable)( GL_POINT_SPRITE_ARB);
	GLFUNC(glDisable)(GL_VERTEX_PROGRAM_POINT_SIZE);
	GLFUNC(glDepthMask)( GL_TRUE );
    GLFUNC(glDisable)( GL_BLEND );
}

void CScene::SetParticlesDistanceAttenuation() {
	static float projection[16];
	GLFUNC(glGetFloatv)(GL_PROJECTION_MATRIX, &projection[0]);
	float k = 1.0f / (projection[5] * g->stp.GetCurrentHeight());
	static float attenuation[] = {0.0f, 0.0f, 0.0f};
	attenuation[2] = k;
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, attenuation);
	glPointParameterf( GL_POINT_FADE_THRESHOLD_SIZE, 2048.0f);
	glPointParameterf( GL_POINT_SIZE_MIN, 0.0f);
	glPointParameterf( GL_POINT_SIZE_MAX, 2048.0f);
}

void CScene::FreeResources()
{
	for (int i = 0; i < NUM_SPLITS; i++)
	if (m_shadowMapRenderTarget[i])
	{
		MP_DELETE(m_shadowMapRenderTarget[i]);
	}

	if (m_reflectionFBO)
	{
		MP_DELETE(m_reflectionFBO);
	}
}

void CScene::OnEndRendering2D()
{
	if (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER)
	{
		return;
	}

	g->pm.CustomBlockStart(CUSTOM_BLOCK_END_RENDERING_2D);

	std::vector<IPlugin *>::iterator it = m_allPlugins.begin();
	std::vector<IPlugin *>::iterator itEnd = m_allPlugins.end();

	for ( ; it != itEnd; it++)
	{
		if (g->rs.GetBool(SHADERS_USING))
		{
			g->sm.UnbindShader();
		}
		(*it)->EndRendering2D();
	}

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_END_RENDERING_2D);
}

CScene::~CScene()
{
	FreeResources();
}