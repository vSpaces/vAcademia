
#include "StdAfx.h"
#include <math.h>
#include <assert.h>
#include "Consts.h"
#include "userdata.h"
#include "LODGroup.h"
#include "CommonMemory.h"
#include "GlobalSingletonStorage.h"
#include "HelperFunctions.h"
#include "miniglut.h"
#include "__resource_log.h"

//#define AVATAR_STRESS_TEST	1

bool g_isShadowsNeeded;

SLODLevel::SLODLevel()
{
	visibleObjects = MP_NEW_ARR(C3DObject*, MAX_GROW_STEP);
	maxObjectCount = MAX_GROW_STEP;

	_modelID = -1;
	visibleObjectCount = 0;
	isLoaded = false;
	isDynamic = false;
}

int SLODLevel::GetModelID()
{
	return _modelID;
}

void SLODLevel::SetModelID(int modelID)
{
	if (_modelID != modelID)
	{
		_modelID = modelID;
		CModel* model = g->mm.GetObjectPointer(_modelID);
		model->AddRef();
	}
}

CLODGroup::CLODGroup():
	m_physicsType(PHYSICS_OBJECT_TYPE_NONE), 
	m_updateAnimationTask(NULL),
	m_collisionBits(4294967295),
	m_isSetToInitialPose(false),
	m_isFirstLODLoaded(false),
	m_updateAnimationTime(0),
	m_isPhysicsLoaded(false),	
	m_physicsModelID(-1),
	m_isNeedToUpdate(false),
	m_isFinalized(false),
	m_isDynamic(false),
	m_updateCount(0),
	m_lodNum(-1),
	m_isOk(true),
	m_isNeedToDraw(true),
	m_isUnpickable(false),
	MP_VECTOR_INIT(m_lodLevels),
	MP_VECTOR_INIT(m_objects),
	MP_VECTOR_INIT(m_physicsObjects)
{
}

bool CLODGroup::IsDynamic()const
{
	return m_isDynamic;
}

void CLODGroup::EnableShadows(bool isEnabled)
{
	std::vector<SLODLevel *>::const_iterator it = m_lodLevels.begin();
	std::vector<SLODLevel *>::const_iterator itEnd = m_lodLevels.end();
	for ( ; it != itEnd; it++)
	{
		SLODLevel* lod = (*it);
		lod->shadowType = isEnabled ? SHADOWS_STENCIL : SHADOWS_NONE;
	}

	NeedToUpdate();
}

void CLODGroup::SetDrawState(bool isNeedToDraw)
{
	if (m_isNeedToDraw != isNeedToDraw)
	{
		m_isNeedToDraw = isNeedToDraw;
		m_isNeedToUpdate = true;

		if (!m_isNeedToDraw)
		{
			std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();
			std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();

			for ( ; lodIter != lodEnd; lodIter++)
			{
				(*lodIter)->visibleObjectCount = 0;
			}
		}
	}	
}

bool CLODGroup::GetDrawState()const
{
	return m_isNeedToDraw;
}

void CLODGroup::DeleteAllExcept(C3DObject* const obj)
{
	m_isNeedToUpdate = true;
	
	m_objects.clear();
	m_objects.push_back(obj);
}

bool CLODGroup::IsLodVisible(const unsigned int lodID)const
{
	if (lodID < m_lodLevels.size())
	{
		return (m_lodLevels[lodID]->visibleObjectCount > 0);
	}
	else
	{
		return false;
	}
}

bool CLODGroup::IsModelVisible(CModel* const model)const
{
	int modelID = model->GetID();

	int sum = 0;

	std::vector<SLODLevel *>::const_iterator it = m_lodLevels.begin();
	std::vector<SLODLevel *>::const_iterator itEnd = m_lodLevels.end();
	for ( ; it != itEnd; it++)
	if ((*it)->GetModelID() == modelID)
	{		
		if ((*it)->visibleObjectCount > 0)
		{
			return true;
		}
		sum += (*it)->visibleObjectCount;
	}

	if (0 == sum)
	{
		return true;
	}

	return false;
}

bool CLODGroup::IsTexturePotentialVisible(CTexture* const texture)const
{
	int textureID = texture->GetID();

	int sum = 0;

	std::vector<SLODLevel *>::const_iterator it = m_lodLevels.begin();
	std::vector<SLODLevel *>::const_iterator itEnd = m_lodLevels.end();
	for ( ; it != itEnd; it++)
	{
		if ((*it)->visibleObjectCount > 0)
		{	
			CModel* model = g->mm.GetObjectPointer((*it)->GetModelID());
			for (int i = 0; i < model->GetTextureCount() - 1; i++)
			if (model->GetTextureNumber(i) == textureID)
			{
				return true;
			}
		}

		sum += (*it)->visibleObjectCount;
	}

	if (0 == sum)
	{
		return true;
	}

	return false;
}

void CLODGroup::GetMaterialBounds(const unsigned int materialID, float& minX, float& minY, float& minZ,
								  float& maxX, float& maxY, float& maxZ)
{
	if (m_lodLevels[0]->isLoaded)
	{
		CModel* model = g->mm.GetObjectPointer(m_lodLevels[0]->GetModelID());

		model->GetMaterialBounds(materialID, minX, minY, minZ, maxX, maxY, maxZ);
	}
	else
	{
		minX = 0.0f;
		minY = 0.0f;
		minZ = 0.0f;
		maxX = 0.0f;
		maxY = 0.0f;
		maxZ = 0.0f;
	}
}

void CLODGroup::SetVisibleAll(const bool isVisible)
{
	std::vector<C3DObject*>::iterator it = m_objects.begin();
	std::vector<C3DObject*>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->SetVisible(isVisible);
	}
}

bool CLODGroup::IsOk()const
{
	return m_isOk;
}

void CLODGroup::NeedToUpdate()
{
	m_isNeedToUpdate = true;
}

void CLODGroup::Add3DObject(C3DObject* const object3d)
{
	assert(object3d);
	m_objects.push_back(object3d);

	if (object3d->GetAnimation())
	{
		int mode = g->mm.GetSupportedRenderModes(object3d->GetAnimation()->GetModelID());
		object3d->GetAnimation()->GetModel()->SetRenderMode((unsigned char)mode);
		object3d->GetAnimation()->GetModel()->AddChangesListener(this);
		m_isDynamic = true;
		m_updateAnimationTask = MP_NEW(CUpdateSkeletonAnimationTask);
		
		if (m_objects.size() == 1)
		{
			UpdateObject(object3d);

			std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
			std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

			for ( ; lodIter != lodEnd; lodIter++)
			{
				(*lodIter)->isDynamic = ((*m_lodLevels.begin())->GetModelID() == (*lodIter)->GetModelID());
				(*lodIter)->isLoaded = (*lodIter)->isDynamic;
			}
		}
	}
	else
	{
		UpdateObject(object3d);	
	}

	if ((m_lodLevels.size() > 0) && (!m_isDynamic))
	{
		UpdateBoundingBox(object3d);
	}

	m_isNeedToUpdate = true;

	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

	for ( ; lodIter != lodEnd; lodIter++)
	if ((*lodIter)->maxObjectCount < m_objects.size())
	{
		C3DObject** tmp = MP_NEW_ARR(C3DObject*, (*lodIter)->maxObjectCount + MAX_GROW_STEP);
		memcpy(tmp, (*lodIter)->visibleObjects, (*lodIter)->maxObjectCount * 4);
		(*lodIter)->maxObjectCount += MAX_GROW_STEP;
		MP_DELETE_ARR((*lodIter)->visibleObjects);
		(*lodIter)->visibleObjects = tmp;
	}
}

void CLODGroup::ChangeCollisions(C3DObject* const object3d, const bool isEnabled)
{
	if (m_physicsModelID == -1)
	{
		if ((m_physicsType != PHYSICS_OBJECT_TYPE_BOX) && ((int)object3d->GetUserData(USER_DATA_LEVEL) != LEVEL_GROUND)
				&& ((int)object3d->GetUserData(USER_DATA_LEVEL) != LEVEL_COLLISION_MESH))
		{
			SetPhysicsType(PHYSICS_OBJECT_TYPE_STATIC_BOX);
		}

		object3d->SetPhysics(isEnabled);
	}
	else
	{
		if (isEnabled)
		{
			object3d->SetPhysics(true, g->mm.GetObjectPointer(m_physicsModelID));
		}
		else
		{
			object3d->SetPhysics(false);
		}
	}

	m_isNeedToUpdate = true;
}

void CLODGroup::UpdateObject(C3DObject* const object3d)
{
	if (m_isPhysicsLoaded)
	{
		bool isPhysicsNeeded = true;

		if (!m_isDynamic)
		{
			float x, y, z;
			object3d->GetCoords(&x, &y, &z);
			if ((0.0f == x) && (0.0f == y) && (0.0f == z))
			{
				isPhysicsNeeded = false;
			}
		}

		if (isPhysicsNeeded)
		{
			ChangeCollisions(object3d, true);
		}
	}

	m_isNeedToUpdate = true;
}

void CLODGroup::UpdateRenderModes()
{
	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

	for ( ; lodIter != lodEnd; lodIter++)
	{
		(*lodIter)->renderMode = g->mm.GetSupportedRenderModes((*lodIter)->GetModelID());
	}

	if (m_isDynamic)
	{
		std::vector<C3DObject*>::iterator it = m_objects.begin();
		std::vector<C3DObject*>::iterator itEnd = m_objects.end();

		for ( ; it != itEnd; it++)
		if ((*it)->GetAnimation())
		{
			int mode = g->mm.GetSupportedRenderModes((*it)->GetAnimation()->GetModelID());
			if (((*it)->GetAnimation()) && ((*it)->GetAnimation()->GetModel()))
			{
				(*it)->GetAnimation()->GetModel()->SetRenderMode((unsigned char)mode);
			}
		}
	}
}

void CLODGroup::Finalize()
{
	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();
	
	float invDist = 0;

	std::vector<float> dists;
	dists.reserve(m_lodLevels.size() + 1);
	dists.push_back(0);
	
	for ( ; lodIter != lodEnd; lodIter++)
	{
		if ((*lodIter)->lodMaxDist > invDist)
		{
			invDist = (*lodIter)->lodMaxDist;
		}

		dists.push_back((*lodIter)->lodMaxDist);
	}

	for (unsigned int i = 0; i < dists.size() - 1; i++)
	{
		int l = i;

		for (unsigned int j = i + 1; j < dists.size(); j++)
		if (dists[l] > dists[j])
		{
			l = j;
		}

		float tmp = dists[l];
		dists[l] = dists[i];
		dists[i] = tmp;
	}
	
	SetInvisibleDist(invDist);

	lodIter = m_lodLevels.begin();

	for ( ; lodIter != lodEnd; lodIter++)
	{
		for (unsigned int i = 1; i < dists.size(); i++)
		if ((*lodIter)->lodMaxDist == dists[i])
		{
			(*lodIter)->lodMinDist = dists[i - 1];
			(*lodIter)->lodMinDistSq = dists[i - 1] * dists[i - 1];
		}
	}

	UpdateRenderModes();

	OnChanged(0);

	m_lastUpdateTime = g->ne.time;
}

bool CLODGroup::IsFrozen()
{
	if (m_objects.size() > 0)
	if (m_objects[0]->IsFrozen())
	{
		return true;
	}

	return false;
}

void CLODGroup::UpdateBoundingBox(C3DObject* const obj3d)
{
	int modelID;
	if (!m_isDynamic)
	{
		modelID = m_lodLevels[0]->GetModelID();
	}
	else
	{
		if (!obj3d->GetAnimation())
		{
			return;
		}

		modelID = obj3d->GetAnimation()->GetModelID();
	}

	CModel* model = g->mm.GetObjectPointer(modelID);
	float maxX = model->GetModelMaxX();
	float maxY = model->GetModelMaxY();
	float maxZ = model->GetModelMaxZ();
	float minX = model->GetModelMinX();
	float minY = model->GetModelMinY();
	float minZ = model->GetModelMinZ();

	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;
	float centerZ = (minZ + maxZ) / 2.0f;

	float sizeX = (-minX + maxX) / 2.0f;
	float sizeY = (-minY + maxY) / 2.0f;
	float sizeZ = (-minZ + maxZ) / 2.0f;

	obj3d->SetBoundingBox(centerX, centerY, centerZ, sizeX, sizeY, sizeZ);
}

void CLODGroup::InitTriMesh(CRayTraceTriMesh* const triMesh)
{
	if (m_lodLevels.size() == 0)
	{
		return;
	}

	int	modelID = m_lodLevels[0]->GetModelID();
	CModel* model = g->mm.GetObjectPointer(modelID);
	
	triMesh->SetModel(model);
}

void CLODGroup::UpdateDynamic(bool isUpdateAllAnyway, const unsigned int shiftTime)
{
	if (m_objects.size() == 0)
	{
		return;
	}

	if (m_isSetToInitialPose)
	{
		return;
	}

	bool isUpdateAll = false;

	bool isAbleToUpdateAnimations = m_updateAnimationTask->IsPerformed();
	bool isRewinded = false;
	
	bool isAnimationUpdated = false;

	if (m_updateAnimationTime < 0)
	{
		m_updateAnimationTime = 0;
	}

	if (shiftTime != 0)
	{
		m_updateAnimationTime = shiftTime;
	}
	else /*if (!IsFrozen())*/
	{
		m_updateAnimationTime += (g->ne.time - m_lastUpdateTime);
	}

	std::vector<C3DObject*>::iterator iter = m_objects.begin();
	std::vector<C3DObject*>::iterator end = m_objects.end();

	std::vector<SLODLevel *>::iterator lodStart = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();
	std::vector<SLODLevel *>::iterator lodIter = lodStart;

	for ( ; lodIter != lodEnd; lodIter++)
	{
		(*lodIter)->visibleObjectCount = 0;
	}

	if (!(*iter)->GetAnimation())
	{
		return;
	}

	bool isMyAvatar = (*iter)->IsControlledObject();
	int updateTime = (*iter)->GetAnimation()->GetUpdateTime();
	if (isMyAvatar)
	{
		updateTime /= 2;
	}

	float eyeX, eyeY, eyeZ;
	g->cm.GetCameraEye(&eyeX, &eyeY, &eyeZ);

	int lodNum = -1;

	float distSq = (*iter)->CalculateDistSq();
	distSq *= g->lod.GetLODScaleSq(*iter);	

	for ( ; iter != end; iter++)
	if ((*iter)->IsVisible())
	{
		stat_updatedObjectCount++;		
		
		if (distSq < m_invisibleDistSq)
		{
			if (lodNum == -1)
			{
				lodIter = lodStart;
				int i = 0;
				for ( ; lodIter != lodEnd; lodIter++, i++)
				{
					if ((distSq >= (*lodIter)->lodMinDistSq) && (distSq <= (*lodIter)->lodMaxDistSq))
					{
						lodNum = i;
						break;
					}
				}			
			}
			else
			{
				(*iter)->CalculateDistSq();
			}
				
			if (lodNum != -1)
			{
				(*iter)->SetVisibleNow(true);
				(*iter)->SetLODNumber((unsigned char)lodNum);

				UpdateBoundingBox(*iter);
								
				IPlugin* plugin = (*iter)->GetPlugin();

				if ((plugin) && (!isUpdateAllAnyway) && g->scm.GetActiveScene())
				{
					g->scm.GetActiveScene()->AddVisiblePluginObject(plugin);
				}

				float x, y, z;
				m_objects[0]->GetCoords(&x, &y, &z);
				float minX, minY, minZ, maxX, maxY, maxZ;
				m_objects[0]->GetBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
				minX += x;
				maxX += x;
				minY += y;
				maxY += y;
				minZ += z;
				maxZ += z;

				bool isCameraInObject = ((eyeX >= minX) && (eyeX <= maxX) && (eyeY >= minY) && (eyeY <= maxY)
					&& (eyeZ >= minZ) && (eyeZ <= maxZ));

				if (isCameraInObject)
				{
					if (g->phom.GetControlledObject())
					if (m_objects[0] == g->phom.GetControlledObject()->GetObject3D())
					{
						isCameraInObject = false;
					}					
				}
				
				if ((!(*iter)->IsBoundingBoxInFrustum()) || (isCameraInObject)) 
				{
					(*iter)->SetVisibleNow(false);

					if (g_isShadowsNeeded)
					if (g->rs.GetBool(SHADOWS_ENABLED))
					if (m_lodLevels[lodNum]->isLoaded)
					if (m_lodLevels[lodNum]->shadowType != SHADOWS_NONE)
					if (m_lodLevels[lodNum]->renderMode != MODE_RENDER_TRANSPARENT + MODE_RENDER_ALL)
					if ((*iter)->IsShadowBoundingBoxInFrustum())
					{
						g->shdw.AddToShadowQueue(*iter);
					}

					if (!isUpdateAll)
					{
						continue;
					}
				}
				else
				{
					m_lodLevels[lodNum]->visibleObjects[m_lodLevels[lodNum]->visibleObjectCount++] = *iter;
				}

				if (m_updateAnimationTime > updateTime)
				{
					isUpdateAll = true;
				}
				
				if (plugin)
				{
					plugin->BeforeUpdate();
				}

				if ((m_updateAnimationTime > updateTime) || (isUpdateAllAnyway) || (m_isNeedToUpdate))
				{
					if (isAbleToUpdateAnimations)
					{
						if (!isRewinded)
						{
							m_updateAnimationTask->Rewind();
							m_updateAnimationTask->SetParams((!IsFrozen()) ? (int)m_updateAnimationTime : 0, distSq);
							isRewinded = true;

							if (!isUpdateAllAnyway)
							{
								m_updateCount++;
							}
						}
						CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)(*iter)->GetAnimation();
						g->pm.CustomMultipleBlockStart(CUSTOM_MULTIPLE_BLOCK_ANIMATION_VBO_UPDATE);
						sao->EndAsynchUpdate();

						isUpdateAllAnyway = false;
						m_updateAnimationTask->AddAnimation(sao);
						sao->BeginAsynchUpdate(m_updateAnimationTask);
						isAnimationUpdated = true;
						g->pm.CustomMultipleBlockEnd(CUSTOM_MULTIPLE_BLOCK_ANIMATION_VBO_UPDATE);

						if (m_updateCount < 3)
						{
							m_lodLevels[lodNum]->visibleObjectCount = 0;
						}
					}
				}
				
				if (plugin)
				{
					plugin->AfterUpdate();
				}
			}
			else
			{
				(*iter)->SetVisibleNow(false);
			}
		}
		else
		{
			if (m_isDynamic)
			{
				IPlugin* plugin = (*iter)->GetPlugin();

				if ((plugin) && (!isUpdateAllAnyway))
				{
					g->scm.GetActiveScene()->AddVisiblePluginObject(plugin);
				}
			}

			(*iter)->SetVisibleNow(false);
		}
	}

	if (isAnimationUpdated)
	{
		int taskUsageMode = MAY_BE_MULTITHREADED;
		if (g->rs.GetBool(UPDATE_ANIMATION_IN_MAIN_THREAD_ONLY))
		{
			taskUsageMode = MUST_RUN_IN_MAIN_THREAD;
		}
		if (isMyAvatar)
		{
			taskUsageMode = MUST_RUN_IN_MAIN_THREAD;
		}
		
		g->pm.CustomMultipleBlockStart(CUSTOM_MULTIPLE_BLOCK_ANIMATION_PROCESSING);
		g->taskm.AddTask(m_updateAnimationTask, taskUsageMode, PRIORITY_HIGH);
		g->pm.CustomMultipleBlockEnd(CUSTOM_MULTIPLE_BLOCK_ANIMATION_PROCESSING);

		m_updateAnimationTime = 0;
		m_isNeedToUpdate = false;
	}

	m_lastUpdateTime = g->ne.time;
}

void CLODGroup::Update(const bool isCameraChanged, const bool isUpdateAllAnyway, const unsigned int shiftTime)
{
	if (m_isDynamic)
	{
		UpdateDynamic(isUpdateAllAnyway, shiftTime);
		return;
	}	

	std::vector<C3DObject*>::iterator iter = m_objects.begin();
	std::vector<C3DObject*>::iterator end = m_objects.end();	

	std::vector<SLODLevel *>::iterator lodStart = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();
	std::vector<SLODLevel *>::iterator lodIter = lodStart;

	bool isUpdateAll = false;

	float eyeX, eyeY, eyeZ;
	g->cm.GetCameraEye(&eyeX, &eyeY, &eyeZ);

	if ((isCameraChanged) || (m_isNeedToUpdate))
	{
		for ( ; lodIter != lodEnd; lodIter++)
		{
			(*lodIter)->visibleObjectCount = 0;
		}

		for ( ; iter != end; iter++)
		if ((*iter)->IsVisible())
		{
			stat_updatedObjectCount++;
			float distSq = (*iter)->CalculateDistSq();
			distSq *= g->lod.GetLODScaleSq(*iter);	

			if (distSq < m_invisibleDistSq)
			{
				lodIter = lodStart;
				int lodNum = -1;
				unsigned char i = 0;

				for ( ; lodIter != lodEnd; lodIter++, i++)
				{
					if ((distSq >= (*lodIter)->lodMinDistSq) && (distSq <= (*lodIter)->lodMaxDistSq))
					{
						if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
						if (m_objects.size() == 1)
						{
							if (m_lodNum != i)
							{ 
								m_lodNum = i;
								(*m_objects.begin())->SetLODNumber(i);
								(*m_objects.begin())->OnChanged(1);
							}
						}

						lodNum = i;
						break;
					}
				}
				
				if (lodNum != -1)
				{
					(*iter)->SetVisibleNow(true);
					(*iter)->SetLODNumber((unsigned char)lodNum);

					if (!(*iter)->IsBoundingBoxInFrustum())
					{
						(*iter)->SetVisibleNow(false);

						if (g_isShadowsNeeded)
						if (g->rs.GetBool(SHADOWS_ENABLED))
						if (m_lodLevels[lodNum]->isLoaded)
						if (m_lodLevels[lodNum]->shadowType != SHADOWS_NONE)
						if (m_lodLevels[lodNum]->renderMode != MODE_RENDER_TRANSPARENT + MODE_RENDER_ALL)
						if ((*iter)->IsShadowBoundingBoxInFrustum())
						{
							g->shdw.AddToShadowQueue(*iter);
						}

						if (!isUpdateAll)
						{
							continue;
						}
					}
					else
					{
						m_lodLevels[lodNum]->visibleObjects[m_lodLevels[lodNum]->visibleObjectCount++] = *iter;
					}
				}
				else
				{
					(*iter)->SetVisibleNow(false);
				}
			}
			else
			{
				if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
				if (m_objects.size() == 1)
				{
					if (m_lodNum != (int)m_lodLevels.size() - 1)
					{
						(*m_objects.begin())->SetLODNumber((unsigned char)(m_lodLevels.size() - 1));
						(*m_objects.begin())->OnChanged(1);
						m_lodNum = m_lodLevels.size() - 1;
					}
				}
				(*iter)->SetVisibleNow(false);
			}
		}
	}
	else if (g_isShadowsNeeded)
	{
		if (g->rs.GetBool(SHADOWS_ENABLED))
		for ( ; iter != end; iter++)
		if ((!(*iter)->IsVisibleNow()) && ((*iter)->IsVisible()))
		{
			unsigned int lodNum = (unsigned int)(*iter)->GetLODNumber();		

			if ((lodNum >= 0) && (lodNum <  m_lodLevels.size()))
			if (m_lodLevels[lodNum]->isLoaded)
			if (m_lodLevels[lodNum]->shadowType != SHADOWS_NONE)
			if (m_lodLevels[lodNum]->renderMode != MODE_RENDER_TRANSPARENT + MODE_RENDER_ALL)
			if ((*iter)->IsShadowBoundingBoxInFrustum())
			{
				g->shdw.AddToShadowQueue((*iter));				
			}
		}
	}

	m_lastUpdateTime = g->ne.time;
	m_isNeedToUpdate = false;
}


void CLODGroup::UpdateAnimations()
{
	if (!m_isDynamic)
	{
		return;
	}

	std::vector<C3DObject*>::iterator iter = m_objects.begin();
	std::vector<C3DObject*>::iterator end = m_objects.end();

	for ( ; iter != end; iter++)
	{
		if (m_updateAnimationTime > (*iter)->GetAnimation()->GetUpdateTime())
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)(*iter)->GetAnimation();
			sao->BeginAsynchUpdate(NULL);
			sao->Update((int)m_updateAnimationTime, 0);
			sao->EndAsynchUpdate();
		}
	}
}

void CLODGroup::OnDeleted(C3DObject* const obj)
{
	m_isNeedToUpdate = true;
	
	std::vector<C3DObject *>::iterator it = m_objects.begin();
	std::vector<C3DObject *>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++)
	if ((*it) == obj)	
	{
		m_objects.erase(it);
		break;
	}
}

void CLODGroup::SetUnpickable(bool isUnpickable)
{
	m_isUnpickable = isUnpickable;
}

bool CLODGroup::IsUnpickable()const
{
	return m_isUnpickable;
}

void CLODGroup::SetCameraSpaceIfNeed(C3DObject* const obj) {
	if (obj->IsInCameraSpace()) {		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}
}

void CLODGroup::UnSetCameraSpaceIfNeed(C3DObject* const obj) {
	if (obj->IsInCameraSpace()) {
		glPopMatrix();
	}
}

void CLODGroup::Draw()
{
	if (m_isDynamic)
	{
		DrawDynamic();
		return;
	}

	float x = 0, y = 0, z = 0;
	CVector3D scale;

	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();
	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();

	for ( ; lodIter != lodEnd; lodIter++)
	if ((*lodIter)->isLoaded)
	{
		C3DObject** _currentObject = &(*lodIter)->visibleObjects[0];
		
		for (int id = 0; id < (*lodIter)->visibleObjectCount; id++)
		{
			C3DObject* currentObject = *_currentObject;
			stat_drawedObjectCount++;
					
			if (MODE_RENDER_TRANSPARENT & (*lodIter)->renderMode)
			{
				g->lod.AddTransparentObject(currentObject);

				if (m_isUnpickable)
				{
					g->stp.SetState(ST_BLEND, true);
				}
			}

			if (MODE_RENDER_ALPHATEST & (*lodIter)->renderMode)
			{
				g->lod.AddAlphaTestObject(currentObject);

				if ((*lodIter)->renderMode == MODE_RENDER_ALPHATEST + MODE_RENDER_ALL)
				if (g->rs.GetBool(SHADOWS_ENABLED))
				if (SHADOWS_STENCIL == (*lodIter)->shadowType)
				{
					g->shdw.AddToShadowQueue(currentObject);
				}	
			}

			if ((MODE_RENDER_SOLID & (*lodIter)->renderMode) || (m_isUnpickable))
			{
				SetCameraSpaceIfNeed(currentObject); 
				currentObject->GetCoords(&x, &y, &z);
				currentObject->GetScale(&scale.x, &scale.y, &scale.z);
				scale *= (*lodIter)->size;

				if (g->rs.GetBool(SHOW_BOUNDING_BOXES))
				{
					ShowObjectBoundingBox(currentObject);
				}
				else if (currentObject->IsBoundingBoxVisible())
				{
					ShowObjectBoundingBox(currentObject);
				}

				g->mm.ShowModelOptimal((*lodIter)->GetModelID(), x, y, z, currentObject->GetRotation(), scale.x, scale.y, scale.z);
				
				if (g->rs.GetBool(SHADOWS_ENABLED))
				if (SHADOWS_STENCIL == (*lodIter)->shadowType)
				{
					g->shdw.AddToShadowQueue(currentObject);
				}
				UnSetCameraSpaceIfNeed(currentObject);
			}			

			_currentObject++;
		}
	}

	if (m_isUnpickable)
	{
		g->stp.SetState(ST_BLEND, false);
	}
}

void CLODGroup::DrawDynamic()
{
	if (g->rs.GetBool(NO_DYNAMIC_OBJECTS))
	{
		return;
	}

	if (!m_isNeedToDraw)
	if ((m_objects.size() > 0) && (m_objects[0]->IsVisibleNow()))
	{				
		g->lod.AddAlphaTestObject(m_objects[0]);
		return;
	}

	std::vector<SLODLevel *>::reverse_iterator lodEnd = m_lodLevels.rend();
	std::vector<SLODLevel *>::reverse_iterator lodIter = m_lodLevels.rbegin();
	int lodNum = 0;

	for ( ; lodIter != lodEnd; lodIter++, lodNum++)
	if ((*lodIter)->isLoaded)
	{
		for (int id = 0; id < (*lodIter)->visibleObjectCount; id++)
		{
			stat_drawedObjectCount++;
			C3DObject* currentObject = (*lodIter)->visibleObjects[id];

			if (!currentObject->GetAnimation())
			{
				continue;
			}

			if (!currentObject->GetAnimation()->GetModel())
			{
				continue;
			}

			bool isRendered = false;
			bool isExit = false;

			int modelID;
			int renderMode;			
			if (((*lodIter)->isDynamic) && (currentObject->GetAnimation()))
			{
				modelID = currentObject->GetAnimation()->GetModelID();
				renderMode = currentObject->GetAnimation()->GetModel()->GetRenderMode();				
			}
			else
			{
				modelID = (*lodIter)->GetModelID();
				renderMode = MODE_RENDER_ALPHATEST + MODE_RENDER_ALL;
				isExit = true;
			}
			
			if (MODE_RENDER_TRANSPARENT & renderMode)
			{ 
				g->lod.AddTransparentObject(currentObject);
			}

			if (MODE_RENDER_ALPHATEST & renderMode)
			{
				g->lod.AddAlphaTestObject(currentObject);
			}

			if (renderMode & renderMode)
			{
				SetCameraSpaceIfNeed(currentObject);
				IPlugin* plugin = currentObject->GetPlugin();
                
				if (plugin)
				{
					plugin->BeforeRender();
				}

				float x = 0, y = 0, z = 0;
				CVector3D scale;
				currentObject->GetCoords(&x, &y, &z);
				currentObject->GetScale(&scale.x, &scale.y, &scale.z);
				scale *= (*lodIter)->size;
				CRotationPack* rotation = currentObject->GetRotation();

				{
					if (currentObject->GetAnimation()->GetType() == TYPE_MORPHING)
					{
						// to do
/*						g->mr.SelectRenderer(VA_RENDERER);
						g->mm.ShowModel(modelID, x, y, z, currentObject->GetRotation(), scale.x, scale.y, scale.z);*/
					}
					else
					{
						if (g->rs.GetBool(SHOW_BOUNDING_BOXES))
						{
							ShowObjectBoundingBox(currentObject);
						}
						else if (currentObject->IsBoundingBoxVisible())
						{
							ShowObjectBoundingBox(currentObject);
						}

						if (currentObject->GetZOffset() != 0.0f)
						{
							GLFUNC(glEnable)(GL_POLYGON_OFFSET_FILL);
							GLFUNC(glPolygonOffset)(0.0f, -currentObject->GetZOffset());
						}

						CModel* model = g->mm.GetObjectPointer(modelID);
						model->SetCullFaceSetting(true);
						model->SetFaceOrient(GL_CCW);

#ifdef AVATAR_STRESS_TEST
						for (int xx = -10; xx <= 10; xx++)
						for (int yy = -10; yy <= 10; yy++)
						{
							g->mm.ShowModelOptimal(modelID, x + xx * 50, y + yy * 50, z, rotation, scale.x, scale.y, scale.z);
						}
#else
						if (!isExit)
						{
							float offset = 0;														
							g->mm.ShowModelOptimal(modelID, x, y, z + offset, rotation, scale.x, scale.y, scale.z);
						}
#endif
						if (currentObject->GetZOffset() != 0.0f)
						{
							GLFUNC(glDisable)(GL_POLYGON_OFFSET_FILL);
						}
					}
				}

				if (plugin)
				{
					plugin->AfterRender();
				}

				isRendered = true;
				UnSetCameraSpaceIfNeed(currentObject);
			}
					
			if (g->rs.GetBool(SHADOWS_ENABLED))
			if (isRendered)
			if (SHADOWS_STENCIL == (*lodIter)->shadowType)
			{
				if (currentObject->GetAnimation()->GetModel()->GetRenderMode() != MODE_RENDER_TRANSPARENT + MODE_RENDER_ALL)
				{
					g->shdw.AddToShadowQueue(currentObject);
				}
			}	

			if (isExit)
			{
				return;
			}
		}
	}
}

void CLODGroup::SetInvisibleDist(const float dist)
{
	m_invisibleDist = dist;
	m_invisibleDistSq = dist * dist;
}

float CLODGroup::GetInvisibleDist()const
{
	return m_invisibleDist;
}

void CLODGroup::AddModelAsLODLevel(const int* modelIDs, const int shadowType, const float scale, const float lodMaxDist)
{
//	assert(livePhazeCount < MAX_LIVE_PHAZES);

	SLODLevel* newLodLevel = MP_NEW(SLODLevel);

	newLodLevel->SetModelID(*modelIDs);
	assert(newLodLevel->GetModelID() >= 0);
	modelIDs++;

	CModel* model = g->mm.GetObjectPointer(newLodLevel->GetModelID());
	model->AddChangesListener(this);

	newLodLevel->size = scale;
	newLodLevel->lodMaxDist = lodMaxDist;
	newLodLevel->lodMaxDistSq = lodMaxDist * lodMaxDist;
	newLodLevel->shadowType = shadowType;

	m_lodLevels.push_back(newLodLevel);
}

void CLODGroup::OnChanged(int eventID)
{
	if (IsDeleted())
	{
		return;
	}

	if (!m_isFirstLODLoaded)
	{
		CModel* model = g->mm.GetObjectPointer((*m_lodLevels.begin())->GetModelID());
		if (model->GetLoadingState() == MODEL_LOADED)
		{
			m_isFirstLODLoaded = true;

			std::vector<C3DObject *>::iterator it = m_objects.begin();
			std::vector<C3DObject *>::iterator itEnd = m_objects.end();

			for ( ; it != itEnd; it++)
			{
				OnObjectCreated(*it);
			}
		}
	}

#ifdef RESOURCE_LOADING_LOG
	g->lw.WriteLn("LODGroup OnChanged: eventID ", eventID, " name = ", GetName(), " isdeleted = ", IsDeleted());
#endif

	m_isNeedToUpdate = true;

	UpdateRenderModes();

	std::vector<C3DObject*>::iterator it = m_objects.begin();
	std::vector<C3DObject*>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++)
	{
		UpdateBoundingBox(*it);
	}

	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();
	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();

	for ( ; lodIter != lodEnd; lodIter++)
	{
		int modelID = (*lodIter)->GetModelID();

		CModel* model = g->mm.GetObjectPointer(modelID);

		if (!model->IsOk())
		{
			m_isOk = false;
			OnAfterChanges(eventID);
			return;
		}

		if (model->GetLoadingState() != MODEL_LOADED)
		{
			continue;
		}
		else
		{
			(*lodIter)->isLoaded = true;
		}

		if (lodIter == m_lodLevels.begin())
		if (!m_isPhysicsLoaded)
		{
			if (model->GetPrimitivesCount() == 0)
			{
				if (!m_isDynamic)
				{
					if (m_physicsType != PHYSICS_OBJECT_TYPE_BOX)
					{
						SetPhysicsType(PHYSICS_OBJECT_TYPE_STATIC_BOX);
					}
				}

				bool isPhysicsNeeded = !m_isDynamic;
			
				if (isPhysicsNeeded)
				{
					std::vector<C3DObject*>::iterator it = m_objects.begin();
					std::vector<C3DObject*>::iterator itEnd = m_objects.end();

					for ( ; it != itEnd; it++)
					{
						(*it)->SetPhysics(true);
					}
				}
			}
			else
			{
				m_physicsModelID = modelID;

				std::vector<C3DObject*>::iterator it = m_objects.begin();
				std::vector<C3DObject*>::iterator itEnd = m_objects.end();

				for ( ; it != itEnd; it++)
				{
					(*it)->SetPhysics(true, model);
				}
			}

			m_isPhysicsLoaded = true;
		}
	}

#ifdef RESOURCE_LOADING_LOG
	lodIter = m_lodLevels.begin();
	for ( ; lodIter != lodEnd; lodIter++)
	{
		g->lw.WriteLn("render mode: ", (*lodIter)->renderMode);
	}
#endif

	OnAfterChanges(eventID);
}

void CLODGroup::SetShadowStatus(const int lodID, const int shadowType)
{
	m_lodLevels[lodID]->shadowType = shadowType;
}

void CLODGroup::AddModelAsLODLevel(const std::string* modelName, const int shadowType, const float scale, const float lodMaxDist)
{
	int modelId = g->mm.GetObjectNumber(*modelName);
	assert(modelId >= 0);
	
	CModel* model = g->mm.GetObjectPointer(modelId);
	model->AddChangesListener(this);

	AddModelAsLODLevel(&modelId, shadowType, scale, lodMaxDist);
}

void CLODGroup::SetPhysicsType(const int type)
{
	m_physicsType = type;
}

void CLODGroup::ClearAll()
{
	m_lodLevels.clear();

	std::vector<C3DObject *>::iterator it = m_objects.begin();
	std::vector<C3DObject *>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->SetLODNumber(0);
	}

	m_isNeedToUpdate = true;
}

int CLODGroup::GetPhysicsType()const 
{
	return m_physicsType;
}

SLODLevel* CLODGroup::GetLodLevel(const unsigned char id)
{
	if (id >= m_lodLevels.size())
	{
		return NULL;
	}

	return m_lodLevels[id];
}

unsigned char CLODGroup::GetLodLevelCount()const 
{
	return (unsigned char)m_lodLevels.size();
}

void CLODGroup::SetCollisionBits(unsigned long bits)
{
	m_collisionBits = bits;
}

unsigned long CLODGroup::GetCollisionBits()const 
{
	return m_collisionBits;
}

void CLODGroup::Clear3DObjLinks()
{
	m_objects.clear();
}

bool CLODGroup::IsGeometryLoaded()const
{
	if (m_isDynamic)
	{
		return true;
	}

	std::vector<SLODLevel *>::const_iterator it = m_lodLevels.begin();
	std::vector<SLODLevel *>::const_iterator itEnd = m_lodLevels.end();

	for ( ; it != itEnd; it++)
	{
		if ((*it)->GetModelID() != -1)
		{
			CModel* model = g->mm.GetObjectPointer((*it)->GetModelID());
			if (model->GetLoadingState()  != MODEL_LOADED)
			{
				return false;
			}
		}
	}

	return true;
}

bool CLODGroup::IsFullyLoaded()const
{
	if (m_isDynamic)
	{
		return true;
	}

	if (!IsGeometryLoaded())
	{
		return false;
	}

	std::vector<SLODLevel *>::const_iterator it = m_lodLevels.begin();
	std::vector<SLODLevel *>::const_iterator itEnd = m_lodLevels.end();

	for ( ; it != itEnd; it++)
	{
		if ((*it)->GetModelID() != -1)
		{
			CModel* model = g->mm.GetObjectPointer((*it)->GetModelID());
			for (int i = 0; i < model->GetTextureCount(); i++)
			{
				int textureID = model->GetTextureNumber(i);
				CTexture* texture = g->tm.GetObjectPointer(textureID);
				if (texture->GetLoadedStatus() == TEXTURE_NOT_LOADED_YET)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void CLODGroup::RemoveObject(C3DObject* const obj)
{
	std::vector<C3DObject*>::iterator it = m_objects.begin();
	std::vector<C3DObject*>::iterator end = m_objects.end();

	for ( ; it != end; it++)
	if (*it == obj)
	{
		m_objects.erase(it);
		break;
	}

	if (m_objects.size() == 0)
	{
		g->lod.DeleteObject(g->lod.GetObjectNumber(GetName()));
	}
}

void CLODGroup::RenderObjectSafely(C3DObject* const obj) 
{
	unsigned int lodID = (unsigned int)obj->GetLODNumber();

	if ((m_lodLevels.size() > 0) && (lodID < m_lodLevels.size()))
	{
		SLODLevel* lod = m_lodLevels[lodID];
		if (lod->isLoaded)
		{
			RenderObject(obj);
		}
	}
}

void CLODGroup::RenderObject(C3DObject* const obj)
{	
	SetCameraSpaceIfNeed(obj);
	int lodID = obj->GetLODNumber();

	if (g->ne.GetWorkMode() == WORK_MODE_SHADOWMAPRENDER)
	{
		if (m_isDynamic)
		{
			lodID = 0;
		}
	}

	if (!m_isNeedToDraw)
	{
		lodID = m_lodLevels.size()-1;
	}

	SLODLevel* lod = m_lodLevels[lodID];

	float x, y, z;
	CVector3D scale;

	obj->GetCoords(&x, &y, &z);
	obj->GetScale(&scale.x, &scale.y, &scale.z);
	scale *= lod->size;

	CRotationPack* rotation = obj->GetRotation();

	int modelID = -1;
	if (!lod->isDynamic)
	{
		if (m_isDynamic)
		{
			scale.x = 1.0f;
			scale.y = 1.0f;
			scale.z = 1.0f;
			rotation = NULL;
			z += 90.0f;

			if (m_isNeedToDraw)
			if (obj->GetAnimation())
			if (((CSkeletonAnimationObject*)obj->GetAnimation())->GetBillboardTextureID() == 0xFFFFFFFF)
			{
				return;
			}
			else
			{
				
			}
		}

		modelID = lod->GetModelID();

#ifdef AVATAR_STRESS_TEST

		g->mm.ShowModelOptimal(modelID, x, y, z, rotation, scale.x, scale.y, scale.z);

#endif
	}
	else if (obj->GetAnimation())
	{
		modelID = obj->GetAnimation()->GetModelID();

#ifdef AVATAR_STRESS_TEST

        for (int xx = -10; xx <= 10; xx++)
		for (int yy = -10; yy <= 10; yy++)
		{
			g->mm.ShowModelOptimal(modelID, x + xx * 50, y + yy * 50, z, rotation, scale.x, scale.y, scale.z);
		}
#endif
	}

#ifndef AVATAR_STRESS_TEST

	if (modelID != -1)
	{
		if (!m_isNeedToDraw)
		{
			int textureID = g->tm.GetAvatarTextureID();
			CModel* model = g->mm.GetObjectPointer(modelID);
	
			int textureNumber = model->GetTextureNumber(0);
			model->SetTextureNumber(0, textureID);

			g->mm.ShowModelOptimal(modelID, x, y, z, rotation, scale.x, scale.y, scale.z);

			model->SetTextureNumber(0, textureNumber);
		}
		else
		{			
			g->mm.ShowModelOptimal(modelID, x, y, z, rotation, scale.x, scale.y, scale.z);
		}
	}
#endif
	UnSetCameraSpaceIfNeed(obj);
}

int CLODGroup::DetectObjectMaterial(C3DObject* const obj, const int mouseX, const int mouseY)
{
	if (obj->GetAnimation())
	{
		return -1;
	}

	int lodID = obj->GetLODNumber();

	SLODLevel* lod = m_lodLevels[lodID];

	float x, y, z;
	CVector3D scale;

	obj->GetCoords(&x, &y, &z);
	obj->GetScale(&scale.x, &scale.y, &scale.z);
	scale *= lod->size;

	CRotationPack* rotation = obj->GetRotation();

	int modelID = lod->GetModelID();

	return g->mm.ShowModelWithMaterialDetect(modelID, x, y, z, rotation, scale.x, scale.y, scale.z, mouseX, mouseY);
}

int CLODGroup::GetCurrentModelID(C3DObject* const obj)const
{
	int lodID = obj->GetLODNumber();
	int modelID = m_lodLevels[lodID]->GetModelID();

	return modelID;
}

void CLODGroup::ShowObjectBoundingBox(C3DObject* const obj)
{
	CColor4* color = obj->GetBoundingBoxColor();

	g->stp.SetState(ST_BLEND, true);
	g->stp.SetColor(color->r, color->g, color->b, color->a);
	GLFUNC(glDisable)(GL_TEXTURE_2D);

	if (g->rs.GetBool(SHADERS_USING))
	{
		g->sm.UnbindShader();
	}
	g->stp.SetMaterial(-1);
						
	g->stp.PushMatrix();
	g->stp.PrepareForRender();	

	CVector3D* ooCorners;
	CVector3D pos;
	obj->GetOOBoundingBox(pos, ooCorners);
	
	GLFUNC(glTranslatef)(pos.x, pos.y, pos.z);
						
	GLFUNC(glBegin)(GL_LINES);
	
	/*float minX, minY, minZ, maxX, maxY, maxZ;
	obj->GetBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);*/

	// גûגמה bounding box-מג הכÿ עוםוגûץ מבתולמג
	
	/*float size = obj->GetHeight();
	minX -= size * SHADOW_BOUNDING_BOX_OFFSET * 0.5f;
	minY -= size * SHADOW_BOUNDING_BOX_OFFSET * 0.5f;
	maxX -= size * SHADOW_BOUNDING_BOX_OFFSET * 0.5f;
	maxY -= size * SHADOW_BOUNDING_BOX_OFFSET * 0.5f;
	float centerX = (maxX + minX) * 0.5f;
	minX += (minX - centerX) * (SHADOW_BOUNDING_BOX_SCALE_XY * 2.0f - 1.0f);
	maxX += (maxX - centerX) * (SHADOW_BOUNDING_BOX_SCALE_XY * 2.0f - 1.0f);
	float centerY = (maxY + minY) * 0.5f;
	minY += (minY - centerY) * (SHADOW_BOUNDING_BOX_SCALE_XY * 2.0f - 1.0f);
	maxY += (maxY - centerY) * (SHADOW_BOUNDING_BOX_SCALE_XY * 2.0f - 1.0f);*/
	
		
	/*GLFUNC(glVertex3f)(minX, minY, minZ);
	GLFUNC(glVertex3f)(minX, minY, maxZ);
	GLFUNC(glVertex3f)(maxX, minY, minZ);
	GLFUNC(glVertex3f)(maxX, minY, maxZ);
	GLFUNC(glVertex3f)(minX, maxY, minZ);
	GLFUNC(glVertex3f)(minX, maxY, maxZ);
	GLFUNC(glVertex3f)(maxX, maxY, minZ);
	GLFUNC(glVertex3f)(maxX, maxY, maxZ);

	GLFUNC(glVertex3f)(minX, minY, minZ);
	GLFUNC(glVertex3f)(minX, maxY, minZ);
	GLFUNC(glVertex3f)(minX, minY, maxZ);
	GLFUNC(glVertex3f)(minX, maxY, maxZ);
	GLFUNC(glVertex3f)(maxX, minY, minZ);
	GLFUNC(glVertex3f)(maxX, maxY, minZ);
	GLFUNC(glVertex3f)(maxX, minY, maxZ);
	GLFUNC(glVertex3f)(maxX, maxY, maxZ);
	
	GLFUNC(glVertex3f)(minX, minY, minZ);
	GLFUNC(glVertex3f)(maxX, minY, minZ);
	GLFUNC(glVertex3f)(minX, maxY, minZ);
	GLFUNC(glVertex3f)(maxX, maxY, minZ);
	GLFUNC(glVertex3f)(minX, maxY, maxZ);
	GLFUNC(glVertex3f)(maxX, maxY, maxZ);
	GLFUNC(glVertex3f)(minX, minY, maxZ);
	GLFUNC(glVertex3f)(maxX, minY, maxZ);*/

	GLFUNC(glVertex3f)(ooCorners[0].x, ooCorners[0].y, ooCorners[0].z);
	GLFUNC(glVertex3f)(ooCorners[1].x, ooCorners[1].y, ooCorners[1].z);
	GLFUNC(glVertex3f)(ooCorners[1].x, ooCorners[1].y, ooCorners[1].z);
	GLFUNC(glVertex3f)(ooCorners[3].x, ooCorners[3].y, ooCorners[3].z);
	GLFUNC(glVertex3f)(ooCorners[3].x, ooCorners[3].y, ooCorners[3].z);
	GLFUNC(glVertex3f)(ooCorners[2].x, ooCorners[2].y, ooCorners[2].z);
	GLFUNC(glVertex3f)(ooCorners[2].x, ooCorners[2].y, ooCorners[2].z);
	GLFUNC(glVertex3f)(ooCorners[0].x, ooCorners[0].y, ooCorners[0].z);

	GLFUNC(glVertex3f)(ooCorners[4].x, ooCorners[4].y, ooCorners[4].z);
	GLFUNC(glVertex3f)(ooCorners[5].x, ooCorners[5].y, ooCorners[5].z);
	GLFUNC(glVertex3f)(ooCorners[5].x, ooCorners[5].y, ooCorners[5].z);
	GLFUNC(glVertex3f)(ooCorners[7].x, ooCorners[7].y, ooCorners[7].z);
	GLFUNC(glVertex3f)(ooCorners[7].x, ooCorners[7].y, ooCorners[7].z);
	GLFUNC(glVertex3f)(ooCorners[6].x, ooCorners[6].y, ooCorners[6].z);
	GLFUNC(glVertex3f)(ooCorners[6].x, ooCorners[6].y, ooCorners[6].z);
	GLFUNC(glVertex3f)(ooCorners[4].x, ooCorners[4].y, ooCorners[4].z);

	GLFUNC(glVertex3f)(ooCorners[0].x, ooCorners[0].y, ooCorners[0].z);
	GLFUNC(glVertex3f)(ooCorners[4].x, ooCorners[4].y, ooCorners[4].z);
	GLFUNC(glVertex3f)(ooCorners[1].x, ooCorners[1].y, ooCorners[1].z);
	GLFUNC(glVertex3f)(ooCorners[5].x, ooCorners[5].y, ooCorners[5].z);
	GLFUNC(glVertex3f)(ooCorners[3].x, ooCorners[3].y, ooCorners[3].z);
	GLFUNC(glVertex3f)(ooCorners[7].x, ooCorners[7].y, ooCorners[7].z);
	GLFUNC(glVertex3f)(ooCorners[6].x, ooCorners[6].y, ooCorners[6].z);
	GLFUNC(glVertex3f)(ooCorners[2].x, ooCorners[2].y, ooCorners[2].z);

	GLFUNC(glEnd)();
	GLFUNC(glEnable)(GL_TEXTURE_2D);
	g->stp.PopMatrix();
	g->stp.SetState(ST_BLEND, false);
	g->stp.SetColor(255, 255, 255, 255);
}

void CLODGroup::ShowCoordinateAxes(C3DObject* const obj)
{
	GLFUNC(glDisable)(GL_TEXTURE_2D);

	g->stp.PushMatrix();

	float x, y, z;
	obj->GetCoords(&x, &y, &z);
	GLFUNC(glTranslatef)(x, y, z);

	float axesLength = 40;

	//OX RED
	g->stp.SetColor(255, 0, 0);
	g->stp.PrepareForRender();
	GLFUNC(glBegin)(GL_LINES);
	GLFUNC(glVertex3f)(0.0f,0.0f,0.0f);
	GLFUNC(glVertex3f)(axesLength,0.0f,0.0f); 
	GLFUNC(glEnd)();
	g->stp.PushMatrix();
	GLFUNC(glTranslatef)(axesLength, 0, 0);
	GLFUNC(glRotatef)(90.0f, 0.0f, 1.0f, 0.0f);
	miniglut::glutSolidCone(4.0f, 20.0f, 6, 1);
	g->stp.PopMatrix();	

	// OY GREEN
	g->stp.SetColor(0, 255, 0);
	g->stp.PrepareForRender();
	GLFUNC(glBegin)(GL_LINES);
	GLFUNC(glVertex3f)(0.0f,0.0f,0.0f);
	GLFUNC(glVertex3f)(0.0f,axesLength,0.0f);
	GLFUNC(glEnd)();
	g->stp.PushMatrix();	
	GLFUNC(glTranslatef)(0, axesLength, 0);
	GLFUNC(glRotatef)(-90.0f, 1.0f, 0.0f, 0.0f);
	miniglut::glutSolidCone(4.0f, 20.0f, 6, 1);
	g->stp.PopMatrix();	

	// OZ BLUE
	g->stp.SetColor(0, 0, 255);
	g->stp.PrepareForRender();
	GLFUNC(glBegin)(GL_LINES);
	GLFUNC(glVertex3f)(0.0f,0.0f,0.0f);
	GLFUNC(glVertex3f)(0.0f,0.0f,axesLength);
	GLFUNC(glEnd)();
	g->stp.PushMatrix();
	GLFUNC(glTranslatef)(0, 0, axesLength);
	miniglut::glutSolidCone(4.0f, 20.0f, 6, 1);
	g->stp.PopMatrix();	

	GLFUNC(glEnable)(GL_TEXTURE_2D);
	g->stp.PopMatrix();	
}

void CLODGroup::SetDynamicTexture(const int orderID, IDynamicTexture* const dynamicTexture)
{
	if (!dynamicTexture)
	{
		return;
	}

	int textureID = g->tm.AddDynamicTexture(dynamicTexture);

	/*std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

	for ( ; lodIter != lodEnd; lodIter++)
	{
		CModel* model = g->mm.GetObjectPointer((*lodIter)->modelID);				
		model->ChangeTexture(orderID, textureID);
	}*/

	SetTexture(orderID, textureID, true);
}

void CLODGroup::SetMaterialAsXML(const int orderID, const std::string& xml)
{
	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

	for ( ; lodIter != lodEnd; lodIter++)
	{
		CModel* model = g->mm.GetObjectPointer((*lodIter)->GetModelID());

		int materialID = model->GetMaterial(orderID);
		bool isNewMaterial = false;
		if (-1 == materialID)
		{
			materialID = g->matm.AddObject("script_material_" + IntToStr((int)this));
			isNewMaterial = true;
		}
				
		CMaterial* material = g->matm.GetObjectPointer(materialID);
		material->LoadFromXML("script_material", (unsigned char *)xml.c_str(), false);

		if (isNewMaterial)
		{
			model->ChangeMaterial(orderID, materialID);
		}

		if (material->IsAlphaBlendExists())
		{
			if (!model->tx[1])
			{
				int vertexCount = model->GetVertexCount();
				if (vertexCount > 0)
				{
					if (!model->tx[1]) 
					{
						model->tx[1] = MP_NEW_ARR(float, vertexCount * 2);
					}				
					memcpy(model->tx[1], model->tx[0], vertexCount * 2 * 4);

					g->mm.MakeVBO((*lodIter)->GetModelID());
				}		
			}
		}

		g->mm.UpdateNotFoundTextures((*lodIter)->GetModelID());
	}

	OnChanged(0);
}

void CLODGroup::SetTextureTiling(const int orderID, CVector2D aspect)
{
	if (orderID < 0)
	{
		return;
	}

	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

	USES_CONVERSION;
	for ( ; lodIter != lodEnd; lodIter++)
	{
		int modelID = (*lodIter)->GetModelID();
		CModel* model = g->mm.GetObjectPointer(modelID);
		int materialID = model->GetMaterial(orderID);

		if (materialID == -1)
		{
			std::string matName = "material_";
			matName += W2A(model->GetName());
			matName += "_";
			matName += IntToStr(lodIter - m_lodLevels.begin());
			matName += "_";
			matName += IntToStr((int)this);
			matName += "_";
			matName += IntToStr(model->GetID());
			matName += "_";
			matName += IntToStr(orderID);
			materialID = g->matm.CloneMaterial(g->rs.GetGlobalMaterialID(), matName);
			model->ChangeMaterial(orderID, materialID);
		}

		if (materialID != -1)
		{
			CMaterial* material = g->matm.GetObjectPointer(materialID);
			if ((aspect.x != 1.0f) || (aspect.y != 1.0f))
			{
				aspect.x *= -1;
				material->EnableTextureMatrix(aspect);
			}
			else
			{
				material->DisableTextureMatrix();
			}
		}
	}
} 

void CLODGroup::SetCommonScale(const float x, const float y, const float z)
{
	std::vector<C3DObject*>::iterator it = m_objects.begin();
	std::vector<C3DObject*>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->SetScale(x, y, z);
	}
}

void CLODGroup::SetTexture(const int orderID, const int textureID, const bool isTraceable, const int level, 
						   const CVector2D aspect)
{
 	if (-1 == textureID)
	{
		return;
	}

	CTexture* newTexture = g->tm.GetObjectPointer(textureID);

	std::vector<int> oldTextureIDs;

	bool isSet = false;

	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();	

	for ( ; lodIter != lodEnd; lodIter++)
	{
		int modelID = (*lodIter)->GetModelID();
		CModel* model = g->mm.GetObjectPointer(modelID);

		if (!isSet)
		{			
			int materialID = model->GetMaterial(orderID);
			int oldTextureID = -1;

			if (materialID != -1)
			{				
				CMaterial* material = g->matm.GetObjectPointer(materialID);
				if (material->GetTextureID(0) != -1)
				{
					oldTextureID = material->GetTextureID((int)((level + 1) / 2));
				}
				else
				{
					oldTextureID = model->GetTextureNumber(orderID);
				}
			}
			else
			{				
				oldTextureID = model->GetTextureNumber(orderID);
			}

			if (oldTextureID != -1)
			{				
				oldTextureIDs.push_back(oldTextureID);
			}
		}
				
		if (-1 == level)
		{
			int materialID = model->GetMaterial(orderID);
			if (materialID != -1)
			{
				CMaterial* material = g->matm.GetObjectPointer(materialID);
				if ((material->GetTextureID(0) != -1) || (aspect.x != 1.0f) || (aspect.y != 1.0f))
				{					
					STexture* texture = MP_NEW(STexture);
					texture->textureID = textureID;
					texture->isAlphaBlend = false;
				
					material->SetTextureLevel(0, TEXTURE_SLOT_TEXTURE_2D, texture);

					if (newTexture->HasNoMipmaps())
					{
						material->SetFiltration(GL_LINEAR, GL_LINEAR, 1, 0.0f);
					}

					if ((aspect.x != 1.0f) || (aspect.y != 1.0f))
					{
						material->EnableTextureMatrix(aspect);
					}
					else
					{
						material->DisableTextureMatrix();
					}
				}
				else
				{					
					model->ChangeTexture(orderID, textureID);
					model->SetTraceableTexture(orderID, isTraceable);
				}
			}
			else
			{				
				model->ChangeTexture(orderID, textureID);
				model->SetTraceableTexture(orderID, isTraceable);
			}
		}
		else
		{			
			int materialID = model->GetMaterial(orderID);
			if (materialID != -1)
			{
				CMaterial* material = g->matm.GetObjectPointer(materialID);
				STexture* texture = MP_NEW(STexture);
				texture->textureID = textureID;
				texture->isAlphaBlend = true;
				material->SetTextureLevel(level, TEXTURE_SLOT_TEXTURE_2D, texture);
			}
			model->SetTraceableTexture(orderID, isTraceable);
		}
		
		CTexture* texture = g->tm.GetObjectPointer(textureID);
		if (texture->GetLoadedStatus() != TEXTURE_LOADED)
		{
			texture->AddChangesListener(this);
		}
		else
		{
			OnChanged(0);
		}

		if (!isSet)
		{
			texture->AddRef();
		}
				
		isSet = true;
	}	

	std::vector<int>::iterator it = oldTextureIDs.begin();
	std::vector<int>::iterator itEnd = oldTextureIDs.end();

	for ( ; it != itEnd; it++)
	{
		CTexture* texture = g->tm.GetObjectPointer(*it);
		texture->ReleaseRef();
	}
}

void CLODGroup::DeleteAllObjects()
{
	if (m_objects.size() == 0)
	{
		return;
	}

	std::vector<C3DObject *> objects;

	std::vector<C3DObject *>::iterator it = m_objects.begin();
	std::vector<C3DObject *>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++)
	{
		objects.push_back(*it);
	}

	m_objects.clear();

	it = objects.begin();
	itEnd = objects.end();

	for ( ; it != itEnd; it++)
	if (!(*it)->IsDeleted())
	{
		g->o3dm.DeleteObject((*it)->GetID());
	}

	/*while (size > 0)
	{
		std::vector<C3DObject *>::iterator it = m_objects.begin();
		g->o3dm.DeleteObject((*it)->GetID());
		size--;
	}*/

	Clear3DObjLinks();
}

void CLODGroup::ClearVisibilityCheckResults()
{
	if (m_lodLevels.size() > 0)
	{
		std::vector<SLODLevel *>::iterator itl = m_lodLevels.begin();
		std::vector<SLODLevel *>::iterator itlEnd = m_lodLevels.end();

		SLODLevel* mainLOD = *itl;

		for ( ; itl != itlEnd; itl++)
		{
			(*itl)->visibleObjectCount = 0;
		}

		std::vector<C3DObject *>::iterator it = m_objects.begin();
		std::vector<C3DObject *>::iterator itEnd = m_objects.end();

		for ( ; it != itEnd; it++)
		if ((*it)->IsVisible())
		{
			(*it)->SetVisibleNow(true);
			(*it)->SetLODNumber(0);

			mainLOD->visibleObjects[mainLOD->visibleObjectCount] = *it;
			mainLOD->visibleObjectCount++;
		}
	}	
}

std::vector<C3DObject*>& CLODGroup::GetObjects()
{
	return m_objects;
}

void CLODGroup::ReloadAll()
{
	m_isPhysicsLoaded = false;
	m_physicsModelID = -1;

	std::vector<SLODLevel *>::iterator it = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator itEnd = m_lodLevels.end();
	USES_CONVERSION;
	for ( ; it != itEnd; it++)
	{
		int modelID = (*it)->GetModelID();
		CModel* model = g->mm.GetObjectPointer(modelID);
		std::string path = W2A(model->GetName());

		for (int i = 0; i < model->GetTextureCount() - 1; i++)
		{
			int textureID = model->GetTextureNumber(i);
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			if (texture->GetUserData(USER_DATA_SRC))
			{
				std::wstring name = texture->GetName();
				std::wstring src = *(std::wstring*)texture->GetUserData(USER_DATA_SRC);
				g->tm.DeleteObject(textureID);
				int newTextureID = g->tm.LoadAsynch(name, src, NULL);

				std::vector<CModel *>::iterator iter = g->mm.GetLiveObjectsBegin();
				std::vector<CModel *>::iterator iterEnd = g->mm.GetLiveObjectsEnd();

				for ( ; iter != iterEnd; iter++)
				{
					int j;
					for (j = 0; j < (*iter)->GetTextureCount() - 1; j++)
					{
						if ((*iter)->GetTextureNumber(j) == textureID)
						{
							(*iter)->SetTextureNumber(j, newTextureID);
						}
					}

					for (j = 0; j < (*iter)->GetTextureCount() - 1; j++)
					{
						int materialID = (*iter)->GetMaterial(j);
						if (materialID != -1)
						{
							CMaterial* mat = g->matm.GetObjectPointer(materialID);
							mat->ReInitialize();
						}
					}
				}
			}
		}

		g->mm.DeleteObject(modelID);
		(*it)->SetModelID(g->mm.LoadAsynch(path, path));
		model = g->mm.GetObjectPointer((*it)->GetModelID());
		model->AddChangesListener(this);
	}

	UpdateRenderModes();
}

void CLODGroup::FreeResources()
{
	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

	for ( ; lodIter != lodEnd; lodIter++)
	{
		CModel* model = g->mm.GetObjectPointer((*lodIter)->GetModelID());
		model->DeleteChangesListener(this);
		model->ReleaseRef();

		MP_DELETE(*lodIter);
	}

	m_lodLevels.clear();
}

void CLODGroup::SetCommonColor(const unsigned char r, const unsigned char _g, const unsigned char b)
{
	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

	for ( ; lodIter != lodEnd; lodIter++)
	{
		CModel* model = g->mm.GetObjectPointer((*lodIter)->GetModelID());
		for (int i = 0; i < model->GetTextureCount() - 1; i++)
		{
			int materialID = model->GetMaterial(i);
			if ((-1 == materialID) || (!g->matm.GetObjectPointer(materialID)->IsUnique()))
			{
				int newMaterialID = g->matm.AddObject("cloned_material_" + IntToStr(g->matm.GetCount()));
				CMaterial* material = g->matm.GetObjectPointer(newMaterialID);
				material->Clone(g->matm.GetObjectPointer((-1 == materialID) ? 0 : materialID));
				model->ChangeMaterial(i, newMaterialID);
				materialID = newMaterialID;
			}

			CMaterial* material = g->matm.GetObjectPointer(materialID);			
			material->AddState("color", IntToStr(r) + ";" + IntToStr(_g) + ";" + IntToStr(b));			
		}
	}
}

void CLODGroup::SetCommonAlpha(const unsigned char alpha)
{
	std::vector<SLODLevel *>::iterator lodIter = m_lodLevels.begin();
	std::vector<SLODLevel *>::iterator lodEnd = m_lodLevels.end();

	for ( ; lodIter != lodEnd; lodIter++)
	{
		CModel* model = g->mm.GetObjectPointer((*lodIter)->GetModelID());
		for (int i = 0; i < model->GetTextureCount() - 1; i++)
		{
			int materialID = model->GetMaterial(i);
			if (-1 == materialID)
			{
				materialID = g->matm.AddObject("cloned_material_" + g->matm.GetCount());
				CMaterial* material = g->matm.GetObjectPointer(materialID);
				material->Clone(g->matm.GetObjectPointer(0));
				model->ChangeMaterial(i, materialID);
			}

			CMaterial* material = g->matm.GetObjectPointer(materialID);	
			material->AddState("opacity", IntToStr(alpha));			
			material->AddState("blend", "true");
		}
	}

	OnChanged(0);
}

void CLODGroup::ToInitialPose()
{
	if (!m_isDynamic)
	{
		return;
	}


	std::vector<C3DObject *>::iterator it = m_objects.begin();
	std::vector<C3DObject *>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)(*it)->GetAnimation();
		if (!sao)
		{
			continue;
		}

		sao->ToInitialPose();
	}

	m_isSetToInitialPose = true;
}

void CLODGroup::FromInitialPose()
{
	if (!m_isDynamic)
	{
		return;
	}

	std::vector<C3DObject *>::iterator it = m_objects.begin();
	std::vector<C3DObject *>::iterator itEnd = m_objects.end();

	for ( ; it != itEnd; it++)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)(*it)->GetAnimation();
		if (!sao)
		{
			continue;
		}

		sao->FromInitialPose();
	}

	m_isSetToInitialPose = false;
}

bool CLODGroup::IsSetToInitialPose()const
{
	return m_isSetToInitialPose;
}

void CLODGroup::OnObjectCreated(C3DObject* obj3d)
{
	if ((m_lodLevels.size() > 0) && (obj3d))
	{
		float x, y, z;
		obj3d->GetCoords(&x, &y, &z);

		int modelID = (*m_lodLevels.begin())->GetModelID();

		CModel* model = g->mm.GetObjectPointer(modelID);
		if (model->GetLoadingState() == MODEL_LOADED)
		{
			g->pf.OnObjectCreated(obj3d->GetID(), modelID);
		}
	}	
}

CLODGroup::~CLODGroup(){}