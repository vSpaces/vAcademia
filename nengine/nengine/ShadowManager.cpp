
#include "StdAfx.h"
#include <math.h>
#include "Model.h"
#include "ShadowManager.h"
#include "ExtensionsInfo.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

#define RENDER_IN_TWO_PASSES			0
#define RENDER_ATI_SEPARATE_STENCIL		1
#define RENDER_EXT_STENCIL_TWO_SIDE		2

#define MAX_SHADOW_VISIBLE_DIST			7000
#define MAX_SHADOW_VISIBLE_DIST_SQ		(MAX_SHADOW_VISIBLE_DIST * MAX_SHADOW_VISIBLE_DIST)

CShadowManager::CShadowManager():
	m_shadowX1(-1.0f),
	m_shadowY1(-1.0f),
	m_shadowX2(1.0f),
	m_shadowY2(1.0f),
	m_shadowCount(0),
	m_renderMethod(-1),
	m_isShadowsEnabled(true),
	m_shadowProtectedObjectsCount(0),
	m_isNeedToDumpShadowQueue(false),
	m_isCheapSoftShadowingEnabled(false)	
{
	ReserveMemory(500);
}

void CShadowManager::SaveShadowQueue()
{
	m_savedShadowCount = m_shadowCount;
	memcpy(&m_savedShadows, &m_shadows, sizeof(m_savedShadows));
}

void CShadowManager::RestoreShadowQueue()
{
	m_shadowCount = m_savedShadowCount;
	memcpy(&m_shadows, &m_savedShadows, sizeof(m_savedShadows));
}

void CShadowManager::NeedToDumpShadowQueue()
{
	m_isNeedToDumpShadowQueue = true;
}

void CShadowManager::DumpShadowQueueIfNeeded()
{
	if (!m_isNeedToDumpShadowQueue)
	{
		return;
	}

	m_isNeedToDumpShadowQueue = false;

	g->lw.WriteLn("SHADOW QUEUE: ");

	C3DObject** shadowCaster = &m_shadows[0];
	for (int i = 0; i < m_shadowCount; i++)
	{
		if (!(*shadowCaster)->IsDeleted())
		{
			g->lw.WriteLn("Object ", i, " ", (*shadowCaster)->GetName());
		}
		shadowCaster++;
	}
}

void CShadowManager::RenderShadowCasters(bool isDeleteObjects)
{
	DumpShadowQueueIfNeeded();

	stat_shadowObjectsCount = m_shadowCount;
	C3DObject** shadowCaster = &m_shadows[0];
	for (int i = 0; i < m_shadowCount; i++, shadowCaster++)
	if ((!(*shadowCaster)->IsDeleted()) && ((*shadowCaster)->IsVisible()))
	{
		g->stp.PushMatrix();
		GLFUNC(glTranslatef)(15.0f, 15.0f, 0);

		(*shadowCaster)->GetLODGroup()->RenderObjectSafely(*shadowCaster);
		
		g->stp.PopMatrix();

		if (isDeleteObjects)
		{
			(*shadowCaster)->SetNeedRenderShadowsStatus(false);
		}			
	}

	if (isDeleteObjects)
	{
		m_shadowCount = 0;
		m_shadowProtectedObjectsCount = 0;
	}
}

void CShadowManager::CheapSoftShadowSupport(bool isEnabled)
{
	m_isCheapSoftShadowingEnabled = isEnabled;
}

bool CShadowManager::IsCheapSoftShadowingEnabled()const
{
	return m_isCheapSoftShadowingEnabled;
}

void CShadowManager::ReplaceShadowedModel(int shadowID, int modelID)
{
	CShadowObject* shadow = GetObjectPointer(shadowID);
	assert(shadow);

	CModel* model = g->mm.GetObjectPointer(modelID);
	assert(model);

	shadow->SetVertexCount(model->GetVertexCount());
	shadow->SetFaceCount(model->GetFaceCount());

	if (g->rs.GetBool(VERTICAL_AXE_Z))
	{
		shadow->SetModelHeight(model->GetModelMaxZ() - model->GetModelMinZ());
	}
	else
	{
		shadow->SetModelHeight(model->GetModelMaxY() - model->GetModelMinY());
	}

	shadow->m_vx = (CVector3D*)model->vx;
	shadow->m_vn = (CVector3D*)model->vn;

	if (shadow->IsDeleted())
	{
		shadow->UnmarkDeleted();
	}

	int i;
	for (i = 0; i < shadow->GetFaceCount(); i++)
	for (int k = 0; k < 3; k++)
	{
		shadow->m_fs[i].vertexIndices[k] = model->fs[i * 3 + k];
	}

	UpdateModelVertexs(shadowID, modelID);

	if (model->GetLoadingState()!= MODEL_NOT_LOADED)
	{
		shadow->Create();
	}
}

void CShadowManager::UpdateModelVertexs(int shadowID, int modelID)
{
	if (!g->rs.GetBool(SHADOWS_ENABLED))
	{
		return;
	}

	CShadowObject* shadow = GetObjectPointer(shadowID);
	assert(shadow);

	CModel* model = g->mm.GetObjectPointer(modelID);
	assert(model);

	if ((float *)model->vx != (float *)shadow->m_vx)
	{
		ReplaceShadowedModel(shadowID, modelID);
		return;
	}

	int i; 
	for (i = 0; i < shadow->GetFaceCount(); i++)
	{
		const CVector3D& v1 = shadow->m_vx[shadow->m_fs[i].vertexIndices[0]];
		const CVector3D& v2 = shadow->m_vx[shadow->m_fs[i].vertexIndices[1]];
		const CVector3D& v3 = shadow->m_vx[shadow->m_fs[i].vertexIndices[2]];

		shadow->m_fs[i].planeEquation.a = v1.y * (v2.z - v3.z) + 
			v2.y * (v3.z - v1.z) + v3.y * (v1.z - v2.z);
		shadow->m_fs[i].planeEquation.b = v1.z * (v2.x - v3.x) + 
			v2.z * (v3.x - v1.x) + v3.z * (v1.x - v2.x);
		shadow->m_fs[i].planeEquation.c = v1.x * (v2.y - v3.y) + 
			v2.x * (v3.y - v1.y) + v3.x * (v1.y - v2.y);
		shadow->m_fs[i].planeEquation.d = -(v1.x * (v2.y * v3.z - v3.y * v2.z) +
			v2.x * (v3.y * v1.z - v1.y * v3.z) + v3.x * (v1.y * v2.z - v2.y * v1.z));
	}
}

int CShadowManager::LoadShadowedModel(int modelID, std::string name)
{
	int ml = AddObject(name);

	ReplaceShadowedModel(ml, modelID);

	return ml;
}

void CShadowManager::GetCurrentLightPosition(CShadowObject* /*shadow*/, float& lx, float& ly, float& lz)
{
    float angle = g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f * 2.0f;

	if (g->rs.GetBool(VERTICAL_AXE_Z))
	{
		float r = sqrtf(m_lightPosition.x * m_lightPosition.x + m_lightPosition.y * m_lightPosition.y);
		lx = r * cosf(angle);
		ly = r * sinf(angle);
		lz = m_lightPosition.z;
	}
	else
	{
		float r = 0.5f * sqrtf(m_lightPosition.x * m_lightPosition.x + m_lightPosition.z * m_lightPosition.z);
		lx = r * cosf(angle);
		ly = m_lightPosition.y;
		lz = r * sinf(angle);
	}
}

void CShadowManager::CastShadow(int modelID)
{
	/*if (shadow->GetWorldLightPosition() == m_lightPosition)
	{
		return;
	}*/

	CShadowObject* shadow = GetObjectPointer(modelID);
	
	for (int i = 0; i < shadow->GetFaceCount(); i++)
	{
		const Plane& plane = shadow->m_fs[i].planeEquation;

		GLfloat side = plane.a * m_lightPosition.x + plane.b * m_lightPosition.y + plane.c * m_lightPosition.z + plane.d;

		if (side >= 0)
		{
			shadow->m_fs[i].isVisible = true;
		}
		else
		{
			shadow->m_fs[i].isVisible = false;
		}
	}

	shadow->SetCastedStatus(true);
}

void CShadowManager::StartShadowing()
{
	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();

	if (m_renderMethod == -1)
	{
		if (g->ei.IsExtensionSupported(GLEXT_ati_separate_stencil))
		{
			m_renderMethod = RENDER_ATI_SEPARATE_STENCIL;
			g->lw.WriteLn("[DECISION] Render shadows via ATI_separate_stencil");
		}
		else if (g->ei.IsExtensionSupported(GLEXT_ext_stencil_two_side))
		{
			m_renderMethod = RENDER_EXT_STENCIL_TWO_SIDE;
			g->lw.WriteLn("[DECISION] Render shadows via EXT_stencil_two_side");
		}
		else
		{
			m_renderMethod = RENDER_IN_TWO_PASSES;
			g->lw.WriteLn("[DECISION] Render shadows in two passes");
		}
	}

	g->stp.SetState(ST_LIGHTING, false);
	g->stp.SetState(ST_DEPTH_FUNC, GL_LEQUAL);
	GLFUNC(glColorMask)(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	
	if (RENDER_IN_TWO_PASSES == m_renderMethod)
	{
		g->stp.SetState(ST_CULL_FACE, true);
	}
	else
	{
		g->stp.SetState(ST_FRONT_FACE, GL_CCW);
		g->stp.SetState(ST_CULL_FACE, false);

		if (RENDER_ATI_SEPARATE_STENCIL == m_renderMethod)
		{
			GLFUNC(glStencilFuncSeparateATI)(GL_ALWAYS, GL_ALWAYS, 1, 0xFFFFFFFF);
			GLFUNC(glStencilOpSeparateATI)(GL_FRONT, GL_KEEP, GL_KEEP, GL_DECR);
			GLFUNC(glStencilOpSeparateATI)(GL_BACK, GL_KEEP, GL_KEEP, GL_INCR);
		}
		else
		{
			GLFUNC(glEnable)(GL_STENCIL_TEST_TWO_SIDE_EXT);

			GLFUNC(glActiveStencilFaceEXT)(GL_BACK);
			GLFUNC(glStencilOp)(GL_KEEP, GL_KEEP, GL_INCR);
			GLFUNC(glStencilFunc)(GL_ALWAYS, 1, 0xFFFFFFFF);
			GLFUNC(glStencilMask)(0xFFFFFFFF);        

			GLFUNC(glActiveStencilFaceEXT)(GL_FRONT);
			GLFUNC(glStencilOp)(GL_KEEP, GL_KEEP, GL_DECR);
			GLFUNC(glStencilFunc)(GL_ALWAYS, 1, 0xFFFFFFFF);
			GLFUNC(glStencilMask)(0xFFFFFFFF);        
		}
	}

	g->stp.SetState(ST_ZWRITE, false);
	g->stp.SetState(ST_ZTEST, true);
	GLFUNC(glDisable)(GL_TEXTURE_2D);
}

void CShadowManager::SetShadowRect(float x1, float y1, float x2, float y2)
{
	m_shadowX1 = x1;
	m_shadowX2 = x2;
	m_shadowY1 = y1;
	m_shadowY2 = y2;
}

void CShadowManager::EndShadowing()
{
	g->stp.SetState(ST_FRONT_FACE, GL_CCW);
	GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	unsigned char color = 128;

	if (m_renderMethod == RENDER_EXT_STENCIL_TWO_SIDE)
	{
		GLFUNC(glDisable)(GL_STENCIL_TEST_TWO_SIDE_EXT);
	}

	if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
	{
		color = (unsigned char)g->dtc.GetShadowIntensivity();
	}

	if (m_isCheapSoftShadowingEnabled)
	{
		GLFUNC(glBlendEquationEXT)(GL_FUNC_REVERSE_SUBTRACT);
		color *= 70;
		color /= 100;
	}
	
	g->stp.SetColor(0, 0, 0, color);
	
	g->stp.SetState(ST_BLEND, true);
	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLFUNC(glStencilFunc)(GL_GREATER, 127, 0xFFFFFFFF);
	GLFUNC(glStencilOp)(GL_KEEP, GL_KEEP, GL_KEEP);

	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetState(ST_CULL_FACE, false);

	g->stp.PushMatrix();

	GLFUNC(glLoadIdentity)();

	g->cm.SetCamera2d();

	g->stp.PrepareForRender();

	GLFUNC(glBegin)(GL_QUADS);
	GLFUNC(glVertex2f)(m_shadowX1, m_shadowY1);
	GLFUNC(glVertex2f)(m_shadowX1, m_shadowY2);
	GLFUNC(glVertex2f)(m_shadowX2, m_shadowY2);
	GLFUNC(glVertex2f)(m_shadowX2, m_shadowY1);
	GLFUNC(glEnd)();

	g->stp.PopMatrix();

	if (m_isCheapSoftShadowingEnabled)
	{
		GLFUNC(glBlendEquationEXT)(GL_FUNC_ADD);
	}

	g->stp.SetState(ST_ZTEST, true);
	g->stp.SetState(ST_ZWRITE, GL_TRUE);
	g->stp.SetState(ST_BLEND, false);

	g->stp.SetColor(255, 255, 255);
	GLFUNC(glEnable)(GL_TEXTURE_2D); 
	g->stp.PrepareForRender();
}

void CShadowManager::DoShadowPass(int modelID)
{
	CShadowObject* shadow = GetObjectPointer(modelID);
	if (shadow->IsDeleted())
	{
		return;
	}
	assert(shadow);
	
	if (!shadow->IsCreated())
	{
		return;
	}

	if (!shadow->IsCasted())
	{
		CastShadow(modelID);
	}

	float inf = fabsf(shadow->GetInfinity()) * 15.9f;
	
	int vxCnt = 0;
	CVector3D v3, v4;

	for (int i = 0; i < shadow->GetFaceCount(); i++)
	{
		const Face& face = shadow->m_fs[i];

		if (face.isVisible)
		{
			for ( int j = 0; j < 3; j++ )
			{
				int neighbourIndex = face.neighbourIndices[j];

				if ((neighbourIndex == -1) || (shadow->m_fs[neighbourIndex].isVisible == false))
				{
					const CVector3D& v1 = shadow->m_vx[face.vertexIndices[j]];
					const CVector3D& v2 = shadow->m_vx[face.vertexIndices[(j + 1)%3]];

					v3 = v1;
					v3 -= m_lightPosition;
					v3 *= inf;
					
					/*float koef;
					if (g->rs.GetBool(VERTICAL_AXE_Z))
					{
						koef = shadow->GetShadowPower() * shadow->GetModelHeight() / fabsf(v3.z);
					}
					else
					{
						koef = shadow->GetShadowPower() * shadow->GetModelHeight() / fabsf(v3.y);
					}

					if (koef < 1.0f)
					{
						v3 *= koef;
					}*/

					v4 = v2;
					v4 -= m_lightPosition;
					v4 *= inf;
					/*if (g->rs.GetBool(VERTICAL_AXE_Z))
					{
						koef = shadow->GetShadowPower() * shadow->GetModelHeight() / fabsf(v4.z);
					}
					else
					{
						koef = shadow->GetShadowPower() * shadow->GetModelHeight() / fabsf(v4.y);
					}
		
					if (koef < 1.0f)
					{
						v4 *= koef;
					}*/

					/*m_volume->AddVertex(v1.x, v1.y, v1.z);
					m_volume->AddVertex(v1.x + v3.x, v1.y + v3.y, v1.z + v3.z);
					m_volume->AddVertex(v2.x + v4.x, v2.y + v4.y, v2.z + v4.z);
					m_volume->AddVertex(v2.x, v2.y, v2.z);*/
					vxCnt += 4;
				}
			}
		}
	}

	//m_volume->AddCount(vxCnt - 8);

/*	if (m_renderMethod != RENDER_IN_TWO_PASSES)
	{
		m_volume->EndStream();
	}
	else
	{
		m_volume->FastDraw();
	}*/
}

void CShadowManager::DrawShadow(int modelID)
{
	if (RENDER_IN_TWO_PASSES == m_renderMethod)	
	{
		g->stp.SetState(ST_FRONT_FACE, GL_CCW);
		GLFUNC(glStencilOp)(GL_KEEP, GL_KEEP, GL_DECR);

		DoShadowPass(modelID);

		g->stp.SetState(ST_FRONT_FACE, GL_CW);
		GLFUNC(glStencilOp)(GL_KEEP, GL_KEEP, GL_INCR);
	
//		m_volume->EndStream();
	}
	else
	{
		DoShadowPass(modelID);
	}
}

void CShadowManager::NewFrame()
{
/*#ifndef SHADOWMAPS
	m_shadowCount = 0;
#endif*/

	m_shadowProtectedObjectsCount = 0;
}

void CShadowManager::AddToShadowQueue(C3DObject* obj)
{
	if (m_shadowCount < SHADOWED_OBJECT_MAX_COUNT)
	{
		if (obj->IsNeededToRenderShadows())
		{
			return;
		}				

		if (obj->GetDistSq() < MAX_SHADOW_VISIBLE_DIST_SQ)
		{
			obj->SetNeedRenderShadowsStatus(true);
			m_shadows[m_shadowCount] = obj;
			m_shadowCount++;
		}		
	}
}

void CShadowManager::ClearShadowQueue()
{
	m_shadowCount = 0;
	m_shadowProtectedObjectsCount = 0;
}

void CShadowManager::RenderAllShadows()
{
   /* if (!g->rs.GetBool(SHADOWS_ENABLED))
	{
		return;
	}

	GLFUNC(glBindBufferARB)(GL_ELEMENT_ARRAY_BUFFER, 0);
	g->stp.SetVertexBuffer(-1, 0);
	g->stp.SetIndexBuffer(-1);
	g->stp.SetNormalBuffer(-1);
	g->stp.SetTextureCoordsBuffer(0, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);

	g->stp.SetState(ST_STENCILTEST, true);
	GLFUNC(glEnable)(GL_STENCIL_TEST);

	StartShadowing();

	double plane[4] = {0, 0, 1, 0};
	GLFUNC(glEnable)(GL_CLIP_PLANE0);
	GLFUNC(glClipPlane)(GL_CLIP_PLANE0, (double *)&plane);

	for (int b = 1; b >= 0; b--)
	{
		for (int i = 0; i < m_shadowCount; i++)
		if (m_shadows[i].isAvoidSelfShadowing == (b > 0))
		{
			g->stp.PushMatrix();

			GLFUNC(glTranslatef)(m_shadows[i].x, m_shadows[i].y, m_shadows[i].z);

			CShadowObject* shadow = GetObjectPointer(m_shadows[i].shadowID);
			assert(shadow);
			m_lightPosition.Set(*(CVector3D*)&shadow->GetLightPosition());

			if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
			{
				float x, y, z;
				GetCurrentLightPosition(shadow, x, y, z);
				m_lightPosition.x = x;
				m_lightPosition.y = y;
				m_lightPosition.z = z;
			}

/*				need to uncomment if stencil shadows will be used
			if ((m_shadows[i].rotation) && (m_shadows[i].rotation->GetRotationCount() > 0))
			{
				float angle = m_shadows[i].rotation->GetCurrentFirstAngle();

				float _x = m_shadows[i].rotation->m_rotations[0]->x;
				float _y = m_shadows[i].rotation->m_rotations[0]->y;
				float _z = m_shadows[i].rotation->m_rotations[0]->z;

				CMatrix4D matrix;
				CVector3D axis(_x, _y, _z);
				axis.Normalize();

				if ((axis.x != 0.0f) || (axis.y != 0.0f) || (axis.z != 0.0f))
				{
					m_shadows[i].rotation->ApplyRotation();
					matrix = matrix.Rotate(axis, angle / 180.0f * 3.14628f);
					m_lightPosition = matrix.TransformDirection(m_lightPosition);
				}			
			}
			else*//*
			{
				GLFUNC(glRotatef)(m_shadows[i].xAngle, 1, 0, 0);
				GLFUNC(glRotatef)(m_shadows[i].yAngle, 0, 1, 0);
				GLFUNC(glRotatef)(m_shadows[i].zAngle, 0, 0, 1);
			}
		
			if ((m_shadows[i].scaleX != 1.0f) || (m_shadows[i].scaleY != 1.0f) || (m_shadows[i].scaleZ != 1.0f))
			{
				GLFUNC(glScalef)(m_shadows[i].scaleX, m_shadows[i].scaleY, m_shadows[i].scaleZ);
				m_lightPosition.x /= m_shadows[i].scaleX;
				m_lightPosition.y /= m_shadows[i].scaleY;
				m_lightPosition.z /= m_shadows[i].scaleZ;
			}
			
			m_lightPosition.RotateY(-m_shadows[i].yAngle);
			if (m_shadows[i].avoidSelfShadowingObject)
			//if ((m_shadows[i].yAngle != 0) || (m_shadows[i].rotation) )
			{
				shadow->SetCastedStatus(false);
			}
		
			DrawShadow(m_shadows[i].shadowID);

			g->stp.PopMatrix();
		}

		if (1 == b)
		{
			if (m_renderMethod == RENDER_EXT_STENCIL_TWO_SIDE)
			{
				GLFUNC(glDisable)(GL_STENCIL_TEST_TWO_SIDE_EXT);
			}

			GLFUNC(glStencilMask)(0xFFFFFFFF);
			GLFUNC(glStencilFunc)(GL_ALWAYS, 127, 0xFFFFFFFF);
			GLFUNC(glStencilOp)(GL_KEEP, GL_KEEP, GL_REPLACE);

			GLFUNC(glEnable)(GL_POLYGON_OFFSET_FILL);
			GLFUNC(glPolygonOffset)(0.0f, -400.0f);

			// рендерим объекты без самозатенения
			int k;
			for (k = 0; k < m_shadowCount; k++)
			if (m_shadows[k].avoidSelfShadowingObject)
			{
				g->stp.PushMatrix();

				m_shadows[k].avoidSelfShadowingObject->GetLODGroup()->RenderObject(m_shadows[k].avoidSelfShadowingObject);

				g->stp.PopMatrix();
			}

			for (k = 0; k < m_shadowProtectedObjectsCount; k++)
			{
				g->stp.PushMatrix();

				m_shadowsProtectedObjects[k]->GetLODGroup()->RenderObject(m_shadowsProtectedObjects[k]);

				g->stp.PopMatrix();
			}

			GLFUNC(glDisable)(GL_POLYGON_OFFSET_FILL);

			GLFUNC(glBindBufferARB)(GL_ELEMENT_ARRAY_BUFFER, 0);
			g->stp.SetVertexBuffer(-1, 0);
			g->stp.SetIndexBuffer(-1);
			g->stp.SetNormalBuffer(-1);
			g->stp.SetTextureCoordsBuffer(0, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF);
			
			StartShadowing();
		}
	}

	GLFUNC(glDisable)(GL_CLIP_PLANE0);

	EndShadowing();

	GLFUNC(glDisable)(GL_STENCIL_TEST);
	g->stp.SetState(ST_STENCILTEST, false);*/
}

void CShadowManager::EnableShadows()
{
	m_isShadowsEnabled = true;
}

void CShadowManager::DisableShadows()
{
	m_isShadowsEnabled = false;
}

CShadowManager::~CShadowManager()
{
	/*assert(m_volume);
	delete m_volume;	
	m_volume = NULL;*/
}