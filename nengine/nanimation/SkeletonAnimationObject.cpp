
#include "StdAfx.h"
#include <Assert.h>
#include "TinyXML.h"
#include "HelperFunctions.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"
#include "SkeletonAnimationObject.h"
#include "SkeletonAnimationObjectEvents.h"
#include "ianimationeventshandler.h"
#include "Cal3DModelAccessObject.h"

#include "cal3d\animation_action.h"
#include "cal3d\animation_cycle.h"
#include "cal3d\CoreMaterial.h"
#include "cal3d\CoreSkeleton.h"
#include "cal3d\CoreSubmesh.h"
#include "cal3d\Submesh.h"
#include "cal3d\Skeleton.h"
#include "cal3d\Renderer.h"
#include "cal3d\Mixer.h"
#include "cal3d\Mesh.h"
#include "cal3d\Bone.h"
#include "cal3d\CoreBone.h"
#include "cal3d\LookController.h"

#define MAX_RECALC_DIST_SQ		(6000 * 6000)
#define MIN_APPLY_IDLES_TIME	500

#define DELETE_MATERIALS		1

_SBlends::_SBlends(std::string _src, std::string _param, std::string _target):
	MP_STRING_INIT(src),
	MP_STRING_INIT(param),
	MP_STRING_INIT(target)
{
	src = _src;
	param = _param;
	target = _target;
	sao = NULL;
}

_SMaterialFile::_SMaterialFile():
	MP_STRING_INIT(name),
	MP_STRING_INIT(path)
{
	data = NULL;
}

_SMaterialFile::_SMaterialFile(std::string _name, std::string& _path, void* _data, bool _isNeedToDelete):
	MP_STRING_INIT(name),
	MP_STRING_INIT(path)
{
	name = _name;
	path = _path;
	data = _data;
	isNeedToDelete = _isNeedToDelete;
}

_STransitionalInfo::_STransitionalInfo():
	MP_STRING_INIT(baseMaterial),
	MP_VECTOR_INIT(materialFiles),
	MP_VECTOR_INIT(singleMaterials),
	MP_VECTOR_INIT(blends)
{
	baseMaterial = "";
	memset(clipboxs, 0, sizeof(SClipboxs*)*CLIPPING_VOLUME_COUNT);
	obj = NULL;
	newSAO = NULL;
	sao = NULL;
}

void _STransitionalInfo::Clear()
{
	singleMaterials.clear();
	baseMaterial = "";
	obj = NULL;
	newSAO = NULL;
	sao = NULL;

	std::vector<SMaterialFile*>::iterator it = materialFiles.begin();	
	std::vector<SMaterialFile*>::iterator itEnd = materialFiles.end();	

	for ( ; it != itEnd; it++)
	{
		//MP_DELETE_ARR((*it)->data);
		MP_DELETE(*it);
	}	
}

_SClipboxs::_SClipboxs():
	MP_STRING_INIT(valueID),
	MP_STRING_INIT(valueOrt),
	MP_STRING_INIT(valuePlaneStart),
	MP_STRING_INIT(valuePlaneEnd),
	MP_STRING_INIT(valueOrt2),
	MP_STRING_INIT(valuePlaneStart2),
	MP_STRING_INIT(valuePlaneEnd2),
	MP_STRING_INIT(valueOrt3),
	MP_STRING_INIT(valuePlaneStart3),
	MP_STRING_INIT(valuePlaneEnd3)
{
}

CSkeletonAnimationObject::CSkeletonAnimationObject():
	m_mask(NULL),
	m_zOffset(0),
	m_maskSize(0),	
	m_parent(NULL),
	m_faceCount(0),
	m_calModel(NULL),
	m_vertexCount(0),
	m_isBlend(false),
	m_isLoaded(false),
	m_parentObj(NULL),
	m_isFrozen(false),
	m_isSleeped(false),
	m_isUpdated(false),
	m_currentVBONum(0),
	m_lastMotionID(-1),
	m_savedFaceCount(0),
	m_updatingTask(NULL),
	m_compatabilityID(0),
	m_calCoreModel(NULL),
	m_isUsualOrder(true),
	m_savedIndices(NULL),
	m_isKinectActive(false),
	m_isPosChanged(false),
	m_savedVertices(NULL),
	m_isInitialized(false),
	m_isIdlesPlaying(false),
	MP_STRING_INIT(m_dir),
	MP_STRING_INIT(m_path),	
	m_kinectEmulator(NULL),
	m_kinectController(NULL),
	m_isStartApplyIdle(false),
	m_isEndApplyIdle(false),
	m_startIdlesTime(0),
	m_isNeedToUpdate(false),
	m_pointController(NULL),
	m_mimicController(NULL),
	m_isClippingChanged(false),
	m_isNeedUpdateBuffers(false),
	m_isTrackingRightHand(false),
	m_isTrackingLeftHand(false),
	m_blendOptimizationInfo(NULL),
	m_isNeedUpdateBlending(false),
	m_billboardTextureID(0xFFFFFFFF),
	m_lastSetAnimationID(0xFFFFFFFF),
	MP_STRING_INIT(m_baseTexturesPath),
	m_isContainsExternalTextures(false),
	MP_VECTOR_INIT(m_clipboxsId),
	MP_VECTOR_INIT(m_idles),
	MP_VECTOR_INIT(m_lockedIdles),
	MP_VECTOR_INIT(m_motions),
	MP_VECTOR_INIT(m_events),
	MP_MAP_INIT(m_texturesPath),
	MP_MAP_INIT(m_motionsList),
	MP_MAP_INIT(m_blends),
	m_leftEyePoseID(-1),
	m_rightEyePoseID(-1),
	m_lookController(NULL),
	m_endStopLookAtTime(0),
	m_kinectObj(NULL),
	m_kinectVersionTemp(0),
	m_kinectVideoTexture(NULL)
{
	// in main thread
	m_lookAtPos.Set(0, 0, 0);
	m_offset.Set(0.0f, 0.0f, 0.0f);
	m_quat.Set(0, 0, 1, 0);
	m_oldQuat.Set(0, 0, 1, 0);

	m_clipboxsId.reserve(CLIPPING_VOLUME_COUNT);

	for (unsigned int vboNum = 0; vboNum < BUFFER_COUNT; vboNum++)
	{
		m_vbo[vboNum] = NULL;		
	}

	InitializeCriticalSection(&m_assignMotionCS);
	InitializeCriticalSection(&m_calModelAccessCS);

	GetMimicController()->SetMimicTarget(this);
}

int CSkeletonAnimationObject::GetCompatabilityID()
{
	return m_compatabilityID;
}

bool CSkeletonAnimationObject::IsUsualOrder()const
{
	return m_isUsualOrder;
}

CMimicController* CSkeletonAnimationObject::GetMimicController()
{
	if (!m_mimicController)
	{
		m_mimicController = MP_NEW(CMimicController);		
	}

	return m_mimicController;
}

void CSkeletonAnimationObject::PointByHandTo(float x, float y, float z)
{
	return;
	if (!m_pointController)
	{
		MP_NEW_P(m_pointController, CalPointController, m_calModel);//new CalPointController(m_calModel);
		m_calModel->SetPointController(m_pointController);
	}

	if ((0.0f == x) && (0.0f == y) && (0.0f == z))
	{
		m_isNeedToUpdate = true;
		CalVector pnt(0, 0, 0);
		m_pointController->SetPoint(pnt);
		return;
	}

	float x2, y2, z2;
	m_obj3d->GetCoords(&x2, &y2, &z2);
	CalVector pnt(x - x2, y - y2, z - 120 - z2);
	pnt = pnt.normalized();
	m_pointController->SetPoint(pnt);

	/*if (m_calModel)
	{
		CalBone* bone1 = NULL;
		int boneID = m_calModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("r upperarm");
		if (boneID != -1)
		{
			bone1 = m_calModel->getSkeleton()->getBone(boneID);
		}

		CalBone* bone2 = NULL;
		boneID = m_calModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("r forearm");
		if (boneID != -1)
		{
			bone2 = m_calModel->getSkeleton()->getBone(boneID);
		}

		m_obj3d->GetRotation()->SetAsDXQuaternion(0, 0, 1, 0);

		CVector3D _realFaceVec = GetRotatedFaceVector();
		CalVector realFaceVec(_realFaceVec.x, _realFaceVec.y, _realFaceVec.z);
		realFaceVec = realFaceVec.normalized();

        CalQuaternion frameRtn = m_calModel->getSkeleton()->getBone(0)->getRotation();
		CalVector faceVec(0, -1, 0);
		//faceVec *= frameRtn;
		CQuaternion rot = m_obj3d->GetRotation()->GetAsDXQuaternion();
		faceVec *= CalQuaternion(rot.x, rot.y, rot.z, rot.w);
		if ((bone1) && (bone2)) 
		{
			float x2, y2, z2;
			m_obj3d->GetCoords(&x2, &y2, &z2);
			CalVector dest(x - x2, y - y2, 0);			
			dest = dest.normalized();
			
			float minLen = 100.0f;
			CalVector minTestVec;
			CalQuaternion resultQ(0, 0, 1, 0);

			for (int i = 0; i < 360; i++)
			{
				CalQuaternion newRotation(CalVector(0, 0, 1), (float)i * 2.0f * 3.14f / 360.0f); 
				bone1->setRotation(newRotation);
				bone1->calculateState();

				CalQuaternion testQ = bone1->getRotationAbsolute();
				CalVector testVec = CalVector(1, 0, 0) * testQ;					
				testVec *= frameRtn;
				testVec *= CalQuaternion(rot.x, rot.y, rot.z, rot.w);
				testVec = testVec.normalized();

				CalVector testVec2 = testVec - dest;
				testVec2.z = 0;
				float len = testVec2.length();

				if (realFaceVec.dot(testVec) > 0)
				if (len < minLen)
				{
					minLen = len;
					minTestVec = testVec;

					resultQ = newRotation;
				}
			}

			bone1->setRotation(resultQ);
			bone1->calculateState();
			
			
		}
	}*/
}

void CSkeletonAnimationObject::SetParent(CSkeletonAnimationObject* parent)
{
	m_parent = parent;
}

CSkeletonAnimationObject* CSkeletonAnimationObject::GetParent()const
{
	return m_parent;
}

int CSkeletonAnimationObject::GetFaceCount()const
{
	return m_faceCount;
}

int CSkeletonAnimationObject::GetVertexCount()const
{
	return m_vertexCount;
}

void CSkeletonAnimationObject::GetUsedClipboxIDs(std::vector<int>& clipboxsIDs)
{
	clipboxsIDs.reserve(m_clipboxsId.size());

	std::vector<int>::iterator it = m_clipboxsId.begin();
	std::vector<int>::iterator itEnd = m_clipboxsId.end();

	for ( ; it != itEnd; it++)
	{
		clipboxsIDs.push_back(*it);
	}
}

void CSkeletonAnimationObject::SetLexeme(const int lexemeID, const int time)
{
	if (lexemeID > 0)
	{
		GetMimicController()->SetLipSyncNextLexemID(lexemeID, time);
	}
	else
	{
		GetMimicController()->SetViseme(-lexemeID, time);
	}
}

void CSkeletonAnimationObject::OnPosChanged()
{
	// in main thread

	m_isPosChanged = true;
}

int CSkeletonAnimationObject::GetType()const
{
	// in main thread

	return TYPE_SKELETON_ANIMATION;
}

bool CSkeletonAnimationObject::IsUpdated()
{
	// in main thread

	bool isUpdated = m_isUpdated;
	m_isUpdated = false;		

	return isUpdated;
}

int CSkeletonAnimationObject::GetUpdateTime()const
{
	// in main thread

	if (m_isIdlesPlaying)
	{
		return g->rs.GetInt(REPROCESS_IDLES_DELTA_TIME);
	}
	else
	{
		return g->rs.GetInt(REPROCESS_ANIMATION_DELTA_TIME);
	}
}

void CSkeletonAnimationObject::BeginAsynchUpdate(CUpdateSkeletonAnimationTask* const task)
{
	if (!m_calModel)
	{
		return;
	}

	if (m_calModel->getSkeleton() == NULL)
	{
		return;
	}

	m_updatingTask = task;

	m_isKinectActive = !((!m_kinectController) || (!m_kinectController->Update()));

	// двигаем если тело и если одежда, у которой у тела установлен кинект
	if (!m_isKinectActive)
	{
		PerformEvents();
	}
	else
	{
		/*if (m_lookController)
		{
			m_lookController->Enable(false);
		}
		m_isNeedToUpdate = true;*/
	}

	if ((m_kinectController) && (m_kinectController->IsEnabled()))
	{
		m_isNeedToUpdate = true;
	}

	if ((m_parent) && (m_parent->m_kinectController)
		&& (m_parent->m_kinectController->IsEnabled()))
	{
		m_isNeedToUpdate = true;
	}

	if ((m_pointController) && (m_obj3d))
	{
#pragma warning(push)
#pragma warning(disable : 4239)
		CQuaternion& q = m_obj3d->GetRotation()->GetAsDXQuaternion();
		m_pointController->SetCurrentObjectRotation(CalQuaternion(q.x, q.y, q.z, q.w));
#pragma warning(pop)
	}
}

void CSkeletonAnimationObject::Update(const int deltaTime, const float distSq)
{
	UpdateImpl(deltaTime, distSq);
	m_updatingTask = NULL;
}

void CSkeletonAnimationObject::UpdateImpl(int deltaTime, float distSq)
{
	if (!m_calModel)
	{
		return;
	}

	if (m_calModel->getSkeleton() == NULL)
	{
		return;
	}

	// asynch execution

	//пропускаем проверку на расстояние до тех пор, пока не будет выставлена хотя бы одна поза телу аватара.
	CSkeletonAnimationObject* parent = GetParent();
	__int64 time = g->tp.GetTickCount();

	if ( (parent==NULL && m_isStartApplyIdle && (time - m_startIdlesTime) > MIN_APPLY_IDLES_TIME) 
		|| (parent!=NULL && parent->IsStartApplyIdle() && (time - parent->GetStartIdlesTime()) > MIN_APPLY_IDLES_TIME) )
	{
		// поза выставлена, можно делать билборд
		m_isEndApplyIdle = true;

		if (distSq > MAX_RECALC_DIST_SQ ) 
		{
			return;
		}
	}
	
	assert(m_model);
	assert(m_calModel);	

	if ((!m_model) || (!m_calModel))
	{
		return;
	}

	m_isNeedUpdateBuffers = false;

	CCal3DModelAccessObject accessObj(this);
	
	//if (!m_isKinectActive)
	if ((distSq <= g->rs.GetInt(MAX_DIST_SQ_FOR_IDLES)) && (!m_isFrozen))
	{
		PlayNextIdleMotion();
	}

	if ((distSq <= g->rs.GetInt(MAX_DIST_SQ_FOR_MIMIC)) && (!m_isFrozen))
	{
		GetMimicController()->Update();
	}

	if (m_isNeedUpdateBlending && !m_isFrozen)
	{
		UpdateBlending();
		m_isNeedUpdateBlending = false;
	}

	/*bool isEnabled;
	if ((m_lookController) && (m_endStopLookAtTime > g->ne.time))
	{
		isEnabled = m_lookController->IsEnabled();
		m_lookController->Enable(false);
	}*/

	bool isChanged = m_calModel->update((float)deltaTime / 1000.0f);
	if (m_isNeedToUpdate)
	{
		isChanged = true;
		m_isNeedToUpdate = false;
	}

	/*if ((m_lookController) && (m_endStopLookAtTime > g->ne.time))
	{
		m_lookController->Enable(isEnabled);
	}*/

	if ((isChanged) || (!m_isInitialized))
	{
		m_isUpdated = true;

		int faceCount = 0;
		unsigned short vertexCount = 0;
		CalRenderer *pCalRenderer = m_calModel->getRenderer();
	
		if (pCalRenderer->beginRendering())
		{
			int meshCount = m_calCoreModel->getCoreMeshCount();

			for (int i = 0; i < meshCount; i++)
			{
				CalMesh* mesh = m_calModel->getMesh(i);
	
				int submeshCount = mesh->getSubmeshCount();

				for (int k = 0; k < submeshCount; k++)
				if (pCalRenderer->selectMeshSubmesh(i, k))
				{
					// get the transformed vertices of the submesh
					unsigned short _vertexCount = 0;
					if (m_model->vx)
					{
						float* vertices = &m_model->vx[vertexCount * 3];
						_vertexCount = (unsigned short)pCalRenderer->getVertices(&vertices, m_isUsualOrder);
					}					
										
					if (m_model->tns && g->rs.IsNotOnlyLowQualitySupported())
					{
						pCalRenderer->getTangents( 0, m_model->tns + vertexCount * 4);
					}
					
					// get the transformed normals of the submesh
					if (m_model->vn)
					{
						pCalRenderer->getNormals(&m_model->vn[vertexCount * 3]);
					}

					if (!m_isInitialized)
					{	
						// get the texture coordinates of the submesh
						if (m_model->tx[0])
						{
							float* textureCoords = &m_model->tx[0][vertexCount * 2];
							pCalRenderer->getTextureCoordinates(0, textureCoords);
						}

						int _faceCount = 0;
						if (m_model->fs)
						{
							unsigned short* indices = &m_model->fs[0];
							indices += faceCount * 3;
							_faceCount = pCalRenderer->getFaces(&indices);
							if (!m_isUsualOrder)
							{
								for (int i = 0; i < _faceCount * 3; i+=3)
								{
									unsigned short tmp = indices[i];
									indices[i] = indices[i + 2];
									indices[i + 2] = tmp;
								}
							}

							indices = &m_model->fs[faceCount * 3];
							for (int i = 0; i < _faceCount * 3; i++)
							{
								indices[i] = indices[i] + vertexCount;
							}
						}

						faceCount += _faceCount;
					}

					vertexCount = vertexCount + _vertexCount;
				}
			}		
		}

		pCalRenderer->endRendering();

		if (!m_isInitialized)
		{
			m_savedIndices = MP_NEW_ARR(unsigned short, faceCount * 3);//new unsigned short[faceCount * 3];
			memcpy(m_savedIndices, m_model->fs, faceCount * 6);
			m_savedFaceCount = faceCount;
		}

		m_model->ReCountBoundingBox();

		m_isNeedUpdateBuffers = true;
	}

	m_isInitialized = true;
}

void CSkeletonAnimationObject::EndAsynchUpdate()
{
	// in main thread

	if (!m_calModel)
	{
		return;
	}

	if (m_calModel->getSkeleton() == NULL)
	{
		return;
	}

	if (m_isClippingChanged)
	{
		ApplyClippingVolumeIfNeeded();
	}

	if (m_isNeedUpdateBuffers)
	if ((m_isInitialized) && (m_model->m_vbo))
	if (m_vbo[m_currentVBONum])
	{
		m_vbo[m_currentVBONum]->SetVertexArray(&m_model->vx[0], m_model->GetVertexCount());
		//m_vbo[m_currentVBONum]->SetTextureCoordsArray(1, m_model->tx[0]);		
		m_vbo[m_currentVBONum]->SetNormalArray(&m_model->vn[0]);

		if (g->rs.IsNotOnlyLowQualitySupported())
			m_vbo[m_currentVBONum]->SetTangentsArray(&m_model->tns[0]);

		m_model->m_vbo = m_vbo[m_currentVBONum];
		g->mm.SwitchVBO(m_model->GetID());
		m_currentVBONum = (m_currentVBONum + 1)%BUFFER_COUNT;
	}

	if (m_isInitialized)
	{
		CalVector pos;
		CalQuaternion quat;
		bool isRelative;

		if ( (m_calModel->basisChanged(pos, quat, isRelative)) && (m_parent))
		{
			if (m_obj3d)
			if (isRelative)
			{
				if ((pos.x != 0.0f) || (pos.y != 0.0f))				
				if (m_obj3d->IsPhysicsObject())
				{
					m_offset.x += pos.x;
					m_offset.y += pos.y;
				}
				else
				{
					CVector3D currentPos = 	m_obj3d->GetCoords();
					currentPos.x += pos.x;
					currentPos.y += pos.y;
					currentPos.z += pos.z;

					if (!m_obj3d->GetParent())
					{
						m_obj3d->SetCoords(currentPos);
					}
				}

				CQuaternion addonRotation(quat.x, quat.y, quat.z, quat.w);
				addonRotation *= m_quat;
				m_quat = addonRotation;
			}
			else if (m_obj3d)
			{
				// запомним абсолютные координаты, с учетом m_offset
				CVector3D currentPos = m_obj3d->GetCoords();

				// сбрасываем m_offset
				m_offset.x = 0.0f;
				m_offset.y = 0.0f;
				m_offset.z = 0.0f;

				// восстановим координаты
				if (!m_obj3d->GetParent())
				{
					m_obj3d->SetCoords(currentPos);
				}
				
				// запомним абсолютный поворот, с учетом m_quat
				CQuaternion currentRtn = m_obj3d->GetRotation() ? m_obj3d->GetRotation()->GetAsDXQuaternion() : CQuaternion(0,0,1,0);

				// сбрасываем m_quat
				m_quat.Set(0, 0, 1, 0);

				// восстановим поворот
				if (!m_obj3d->GetParent())
				{
					m_obj3d->GetRotation()->SetAsDXQuaternion(currentRtn.x,currentRtn.y,currentRtn.z,currentRtn.w);
				}
			}
		}		
	}	

	m_isPosChanged = false;

	PerformEvents();

	TrackingHands();

	if( m_asyncUpdateListener != NULL)
	{
		m_asyncUpdateListener->OnAsynchUpdateComplete();
	}	
}


void CSkeletonAnimationObject::TrackingHands()
{
	bool isTrackingHand = false;

	// отслеживание правой руки
	if (m_isTrackingRightHand)
	{
		CVector3D pos = GetBonePosition("R Hand");
		CVector3D direction = GetBonePosition("R Forearm");
		direction = pos - direction;
		if (direction != m_rightHandDirection)
		{
			m_rightHandDirection = direction;
			if ((m_kinectController) && (m_kinectController->IsKinectDeskModeEnabled()))
			{
				m_rightHandDirectionExtended = m_kinectController->GetExtendedHandDirection(pos, direction);
			}
			OnAfterChanges(4);
			if (m_rightHandDirectionExtended != CVector3D(0, 0, 0))
			{
				isTrackingHand = true;
			}			
		}
		if (pos != m_rightHandPosition)
		{
			m_rightHandPosition = pos;
			OnAfterChanges(4);			
		}
	}

	// отслеживание левой руки
	if (m_isTrackingLeftHand)
	{
		CVector3D pos = GetBonePosition("L Hand");
		CVector3D direction = GetBonePosition("L Forearm");
		direction = pos - direction;
		if (direction != m_leftHandDirection)
		{
			m_leftHandDirection = direction;
			if ((m_kinectController) && (m_kinectController->IsKinectDeskModeEnabled()))
			{
				m_leftHandDirectionExtended = m_kinectController->GetExtendedHandDirection(pos, direction);
			}
			OnAfterChanges(5);
			if (m_leftHandDirectionExtended != CVector3D(0, 0, 0))
			{
				isTrackingHand = true;
			}
		}
		if (pos != m_leftHandPosition)
		{
			m_leftHandPosition = pos;
			OnAfterChanges(5);
		}
	}

	if (((m_isTrackingRightHand) || (m_isTrackingLeftHand)) && (!isTrackingHand))
	{
		if ((m_kinectController) && (m_kinectController->IsKinectDeskModeEnabled()))
		{
			m_kinectController->OnHandsNotRecognized();
		}
	}
}

bool CSkeletonAnimationObject::IsContainsExternalTextures()const
{
	// in main thread

	return m_isContainsExternalTextures;
}

CVector3D CSkeletonAnimationObject::GetBonePosition(std::string name)
{
	if (!m_calModel)
	{
		return CVector3D(0, 0, 0);
	}

	CalSkeleton* skeleton = m_calModel->getSkeleton();
	if (!skeleton)
	{
		return CVector3D(0, 0, 0);
	}

	CalCoreSkeleton* coreSkeleton = skeleton->getCoreSkeleton();
	if (!coreSkeleton)
	{
		return CVector3D(0, 0, 0);
	}

	CalBone* bone = NULL;	
	if (coreSkeleton->containBone(name))
	{
		int boneID = coreSkeleton->getCoreBoneIdSubName(name.c_str());
		bone = skeleton->getBone(boneID);
	}
	else
	{
		return CVector3D(0, 0, 0);
	}	
	
	CalVector vec = bone->getTranslationAbsolute();
	CVector3D res(vec.x, vec.y, vec.z);

	if (!m_obj3d)
	{
		return CVector3D(0, 0, 0);
	}

	res *= m_obj3d->GetRotation()->GetAsDXQuaternion();
	res += m_obj3d->GetCoords();

	return res;
}

void CSkeletonAnimationObject::SetFreeze(const bool isFrozen)
{
	// in main thread

	m_isFrozen = isFrozen;

	if (m_calModel)
	{
		if (m_isFrozen)
		{
			m_calModel->freeze();
		}
		else
		{
			m_calModel->unFreeze();
		}
	}
}

void CSkeletonAnimationObject::SetTrackingRightHand(bool isEnabled)
{
	m_isTrackingRightHand = isEnabled;
	m_rightHandPosition = CVector3D(0, 0, 0);
	m_rightHandDirection = CVector3D(0, 0, 0);
	m_rightHandDirectionExtended = CVector3D(0, 0, 0);
}

void CSkeletonAnimationObject::SetTrackingLeftHand(bool isEnabled)
{
	m_isTrackingLeftHand = isEnabled;
	m_leftHandPosition = CVector3D(0, 0, 0);
	m_leftHandDirection = CVector3D(0, 0, 0);
	m_leftHandDirectionExtended = CVector3D(0, 0, 0);
}

CVector3D CSkeletonAnimationObject::GetRightHandPosition()
{
	return m_rightHandPosition;
}

CVector3D CSkeletonAnimationObject::GetLeftHandPosition()
{
	return m_leftHandPosition;
}

CVector3D CSkeletonAnimationObject::GetRightHandDirection()
{
	return m_rightHandDirection;
}

CVector3D CSkeletonAnimationObject::GetLeftHandDirection()
{
	return m_leftHandDirection;
}

CVector3D CSkeletonAnimationObject::GetExtendedRightHandDirection()
{
	return m_rightHandDirectionExtended;
}

CVector3D CSkeletonAnimationObject::GetExtendedLeftHandDirection()
{
	return m_leftHandDirectionExtended;
}

int CSkeletonAnimationObject::AssignMotion(const int motionID)
{
	// in main thread with adding event

	if (-1 == motionID)
	{
		return -1;
	}

	if ((!m_calModel) || (!m_calCoreModel))
	{
		return -1;
	}

	int cal3DMotionID = -1;

	EnterCriticalSection(&m_assignMotionCS);

	std::map<int, int>::iterator it = m_motionsList.find(motionID);

	if (it == m_motionsList.end())
	{
		CMotion* motion = g->motm.GetObjectPointer(motionID);

		if (motion->GetCoreAnimation())
		{
			m_lastMotionID++;			
			int id = m_lastMotionID;

			cal3DMotionID = AssignMotionByEvent( (void *)motionID);  
			
			motion->SetCalModel(m_calModel);
			
			m_motions.push_back(id);
			m_motionsList.insert(std::map<int, int>::value_type(motionID, cal3DMotionID));
			motion->AddDestroyListener(this);
		}
		else
		{
			LeaveCriticalSection(&m_assignMotionCS);
			return -1;
		}
	}
	else
	{
		cal3DMotionID = (*it).second;
	}

	LeaveCriticalSection(&m_assignMotionCS);
	return cal3DMotionID;
}

void CSkeletonAnimationObject::RemoveMotion(const int motionID)
{
	// in main thread with CS-protected access to CalModel

	CMotion* motion = g->motm.GetObjectPointer(motionID);

	CCal3DModelAccessObject accessObj(this);	
	m_calModel->deleteAnimations(motion->GetCoreAnimation());
	m_calCoreModel->detachCoreAnimation(motion->GetCoreAnimation());	

	std::map<int, int>::iterator it = m_motionsList.find(motionID);

	if (it != m_motionsList.end())
	{
		m_motions[(*it).second] = -1;
		m_motionsList.erase(it);
	}
}

void CSkeletonAnimationObject::ClearAction(const unsigned int id, const float time)
{
	// in main thread with adding event

	SClearEvent* clearEvent = MP_NEW(SClearEvent);//new SClearEvent();
	clearEvent->id = id;
	clearEvent->time = time;

	SaveEvent(EVENT_ID_CLEAR_ACTION, clearEvent);
}

void CSkeletonAnimationObject::ClearCycle(const unsigned int id, const float time)
{
	// in main thread with adding event

	SClearEvent* clearEvent = MP_NEW(SClearEvent);//new SClearEvent();
	clearEvent->id = id;
	clearEvent->time = time;

	SaveEvent(EVENT_ID_CLEAR_CYCLE, clearEvent);
}

void CSkeletonAnimationObject::BlendCycle(const unsigned int id, const float weight, const float delay, const bool isAsynch)
{
	// in main thread

	SBlendCycleEvent* blendCycleEvent = MP_NEW(SBlendCycleEvent);//new SBlendCycleEvent();
	blendCycleEvent->id = id;
	blendCycleEvent->weight = weight;
	blendCycleEvent->delay = delay;
	blendCycleEvent->isAsynch = isAsynch;

	SaveEvent(EVENT_ID_BLEND_CYCLE, blendCycleEvent);
}

void CSkeletonAnimationObject::ExecuteAction(const unsigned int id, const float delayIn, const float delayOut)
{
	// in main thread with adding event

	SExecuteActionEvent* executeActionEvent = MP_NEW(SExecuteActionEvent);//new SExecuteActionEvent();
	executeActionEvent->id = id;
	executeActionEvent->delayIn = delayIn;
	executeActionEvent->delayOut = delayOut;

	SaveEvent(EVENT_ID_EXECUTE_ACTION, executeActionEvent);
}
 
// id - идентификатор движения в nengine
void CSkeletonAnimationObject::ExecuteActionImpl(const unsigned int id, const float delayIn, const float delayOut)
{
	// asynch execution and in main thread after asynch execution

	if (!m_calModel)
	{
		return;
	}

	if (0xFFFFFFFF == id)
	{
		return;
	}

	CMotion* motion = g->motm.GetObjectPointer( id);
	assert( motion);
	if( !motion)
	{
		return;
	}

	if (motion->GetCompatabilityID() != m_motionCompatabilityID)
	{
		return;
	}

	unsigned int cal3DMotionID = AssignMotion( id);

	//assert(cal3DMotionID < m_motions.size());
	if (cal3DMotionID >= m_motions.size())
	{
		return;
	}

	if ((m_calModel->getMixer()) && (m_motions[cal3DMotionID] != -1))
	{
		AdjustSkeletonTransformation();
		m_calModel->getMixer()->executeAction(cal3DMotionID, delayIn, delayOut, motion->GetDuration() / 1000.0f);
	}
}

void CSkeletonAnimationObject::SetAction(const unsigned int id, const float time)
{
	if (!m_obj3d->IsVisibleNow())
	{
		const int maxEvents = 1000;
		const int halfMaxEvents = (int)(maxEvents / 2);

		if (m_events.size() > maxEvents)
		{
			int cnt = 0;

			std::vector<SEvent *>::iterator it = m_events.begin();
			std::vector<SEvent *>::iterator itEnd = m_events.end();

			for ( ; it != itEnd; it++, cnt++)
			if (cnt < halfMaxEvents)
			{
				MP_DELETE((*it));//delete (*it);
			}

			m_events.erase(m_events.begin(), m_events.begin() + halfMaxEvents);
		}		
	}

	// in main thread with adding event

	SSetActionEvent* setActionEvent = MP_NEW(SSetActionEvent);//new SSetActionEvent();
	setActionEvent->id = id;
	setActionEvent->time = time;

	SaveEvent(EVENT_ID_SET_ACTION, setActionEvent);
}

CalModel* CSkeletonAnimationObject::GetCalModel()const
{
	// in main thread
		
	return m_calModel;
}

CVector3D CSkeletonAnimationObject::GetRotatedFaceVector()const
{
	CVector3D _res = GetFaceVector();
	CVector3D res = _res;
	if (m_obj3d)
	if (m_obj3d->GetRotation())
	{
		res *= m_obj3d->GetRotation()->GetAsDXQuaternion();
	}

	if (res.GetLengthSq() == 0.0f)
	{
		res = _res;
	}

	return res;
}

CVector3D CSkeletonAnimationObject::GetFaceVector()const
{
	// in main thread

	CalVector faceVec(1.0f, 0.0f, 0.0f);

	if (m_calModel)
	if ((m_calModel->getSkeleton()) && (m_calModel->getSkeleton()->getBone(0)))
	{
		CalQuaternion frameRtn = m_calModel->getSkeleton()->getBone(0)->getRotation();
		if (frameRtn.w != 0.0f)
		{
			faceVec *= frameRtn;
		}
	}

	return CVector3D(faceVec.x, faceVec.y, faceVec.z);
}

bool CSkeletonAnimationObject::BindToSkeleton(CSkeletonAnimationObject* const sao)
{
	if (!sao)
	{
		return false;
	}

	if (m_compatabilityID != sao->m_compatabilityID)
	{
		return false;
	}

	if (!sao->GetCalModel())
	{
		return false;
	}

	CCal3DModelAccessObject accessObj(this);	
	sao->GetCalModel()->bindToModelSkeleton(m_calModel);		

	return true;
}

void CSkeletonAnimationObject::UnbindSkeleton()
{
	if (m_calModel)
	{
		CCal3DModelAccessObject accessObj(this);	
		m_calModel->unbindSkeleton();		
	}
}

CQuaternion CSkeletonAnimationObject::GetAnimationRotation()const
{
	return m_quat;
}

void CSkeletonAnimationObject::GetAnimationRotation(float* const x, float* const y, float* const z, float* const w)
{
	*x = m_quat.x;
	*y = m_quat.y;
	*z = m_quat.z;
	*w = m_quat.w;
}

void CSkeletonAnimationObject::AdjustSkeletonTransformation()
{
	// in main thread

	CVector3D currentPos = 	m_obj3d->GetCoords();
	CQuaternion currentRtn = m_obj3d->GetRotation() ? m_obj3d->GetRotation()->GetAsDXQuaternion() : CQuaternion(0,0,1,0);

	if( m_calModel && m_calModel->getMixer())
	{
		m_calModel->getMixer()->setPredefinedBasisMode( CalVector(currentPos.x,currentPos.y,currentPos.z),
														CalQuaternion(currentRtn.x,currentRtn.y,currentRtn.z,currentRtn.w));
	}
}

std::string CSkeletonAnimationObject::GetPath()const
{
	// in main thread

	return m_path;
}

std::string CSkeletonAnimationObject::GetDir()const
{
	// in main thread

	return m_dir;
}

CVector3D CSkeletonAnimationObject::GetOffset()const
{
	// in main thread

	CVector3D offset = m_offset;
	offset.z = m_zOffset;
	if (!IsUsualOrder())
	{
		if (!m_obj3d->GetParent())
		{
			offset.z += 90;
		}
		else
		{
			offset.z += 60;
		}
	}
	
	return offset;
}

void CSkeletonAnimationObject::GetOffset(float* const x, float* const y, float* const z)
{
	// in main thread

	*x = m_offset.x;
	*y = m_offset.y;
	*z = m_zOffset;
}

void CSkeletonAnimationObject::SetBillboardTextureID(const int textureID)
{
	// in main thread

	m_billboardTextureID = textureID;

	unsigned char lodCount = m_obj3d->GetLODGroup()->GetLodLevelCount();
	if (lodCount != 0)
	{
		int modelID = m_obj3d->GetLODGroup()->GetLodLevel(lodCount - 1)->GetModelID();
		CModel* model = g->mm.GetObjectPointer(modelID);
		model->ChangeTexture(0, m_billboardTextureID);
	}
}
	
int CSkeletonAnimationObject::GetBillboardTextureID()const
{
	// in main thread

	return m_billboardTextureID;
}

void CSkeletonAnimationObject::OnDestroy(CMotion* const motion)
{
	// in main thread

	if (motion->IsDeleted())
	{	
		RemoveMotion(motion->GetID());
	}
}

void CSkeletonAnimationObject::CopyMotionsTo(CSkeletonAnimationObject* const sao)
{
	std::map<int, int>::iterator it = m_motionsList.begin();
	std::map<int, int>::iterator itEnd = m_motionsList.end();	

	for ( ; it != itEnd; it++)
	{
		//!! Здесь AssignMotion нужно оставить
		sao->AssignMotion((*it).first);
		CMotion* motion = g->motm.GetObjectPointer((*it).first);
		motion->RemoveDestroyListener(this);
	}

	std::vector<SIdleInfo *>::iterator iter = m_idles.begin();
	std::vector<SIdleInfo *>::iterator iterEnd = m_idles.end();

	for ( ; iter != iterEnd; iter++)
	{
		sao->AddIdle((*iter)->motionID, (*iter)->between, (*iter)->freq);
	}
}

void CSkeletonAnimationObject::DumpMotionsInfoToLog()
{
	if (m_calModel)
	{
		std::vector<std::string> arr = m_calModel->getMixer()->getMotionsInfo();

		std::vector<std::string>::iterator it = arr.begin();
		std::vector<std::string>::iterator itEnd = arr.end();

		for ( ; it != itEnd; it++)
		{
			g->lw.WriteLn(*it);
		}
	}
}

void CSkeletonAnimationObject::FreeResources()
{
	// in main thread with protection from animation update after destruction

	UnbindSkeleton();

	CCal3DModelAccessObject accessObj(this);		

	if (m_pointController)
	{
		MP_DELETE(m_pointController);
		m_pointController = NULL;
	}

	if (m_mimicController)
	{
		MP_DELETE(m_mimicController);
		m_mimicController = NULL;
	}

	if (m_billboardTextureID != -1)
	{
		g->tm.DeleteObject(m_billboardTextureID);
		m_billboardTextureID = 0xFFFFFFFF;
	}

	if (m_updatingTask)
	{
		m_updatingTask->RemoveAnimation(this);
		m_updatingTask = NULL;
	}

	if (m_savedVertices)
	{
		MP_DELETE_ARR(m_savedVertices);
		m_savedVertices = NULL;
	}

	if (m_savedIndices)
	{
		MP_DELETE_ARR(m_savedIndices);
		m_savedIndices = NULL;
	}

	if (m_mask)
	{
		MP_DELETE_ARR(m_mask);
		m_mask = NULL;
	}

	m_blends.clear();

	if (m_calModel)
	{
		m_calModel->destroy();
		MP_DELETE(m_calModel);
		m_calModel = NULL;
	}

	if (m_calCoreModel)
	{
		m_calCoreModel->destroy();
		MP_DELETE(m_calCoreModel);
		m_calCoreModel = NULL;
	}

	if (m_model)	
	{
		m_model->m_vbo = NULL;

		for (int i = 0; i < m_model->GetTextureCount() - 1; i++)
		{
			int textureID = m_model->GetTextureNumber(i);
			if (textureID != -1)
			{
				CTexture* texture = g->tm.GetObjectPointer(textureID);
				texture->ReleaseRef();
			}

#ifdef DELETE_MATERIALS
			int materialID = m_model->GetMaterial(i);
			if (materialID != g->rs.GetGlobalMaterialID())
			{
				g->matm.DeleteObject(materialID);				
			}
#endif
		}

		g->mm.DeleteObject(m_model->GetID());
	
		for (int k = 0; k < BUFFER_COUNT; k++)
		{
			if (m_vbo[k])
			{
				bool isEqual = false;

				for (int i = 0; i < k; i++)
				if (m_vbo[i] == m_vbo[k])
				{
					isEqual = true;
				}

				if (!isEqual)
				{
					MP_DELETE_UNSAFE(m_vbo[k]);
				}
			}
		}

		for (int k = 0; k < BUFFER_COUNT; k++)
		{
			m_vbo[k] = NULL;
		}

		for (int k = 0; k < MAX_PARTS; k++)
		{
			m_model->m_vbos[k] = NULL;
		}
	}

	std::vector<SIdleInfo *>::iterator iti = m_idles.begin();
	std::vector<SIdleInfo *>::iterator itiEnd = m_idles.end();

	for ( ; iti != itiEnd; iti++)
	{
		MP_DELETE((*iti));
	}

	m_idles.clear();

	iti = m_lockedIdles.begin();
	itiEnd = m_lockedIdles.end();

	for ( ; iti != itiEnd; iti++)
	{
		MP_DELETE((*iti));
	}

	m_lockedIdles.clear();

	std::map<int, int>::iterator itm = m_motionsList.begin();
	std::map<int, int>::iterator itmEnd = m_motionsList.end();

	for ( ; itm != itmEnd; itm++)
	if ((*itm).first != -1)
	{
		CMotion* motion = g->motm.GetObjectPointer((*itm).first);
		motion->RemoveDestroyListener(this);
	}

	m_motionsList.clear();	

	if (m_blendOptimizationInfo)
	{
		MP_DELETE(m_blendOptimizationInfo);
		m_blendOptimizationInfo = NULL;
	}

	if ((m_parentObj) && (!m_parentObj->IsDeleted()))
	if ((m_parent) && (!m_parent->IsDeleted()))
	{
		std::vector<int>::iterator itcl = m_clipboxsId.begin();
		std::vector<int>::iterator itclEnd = m_clipboxsId.end();

		for ( ; itcl != itclEnd; itcl++)
		{
			m_parent->DisableClippingVolume(*itcl);
		}

		m_clipboxsId.clear();
	}

	m_texturesPath.clear();

	m_motions.clear();	

	if (m_kinectController)
	{
		MP_DELETE(m_kinectController);
	}
}

void CSkeletonAnimationObject::LookAt(CVector3D& pos, int /*time*/, float /*koef*/)
{
	return;
	if (!IsLoaded())
	{
		m_lookAtPos = pos;
		return;
	}

	if (m_kinectController)
	if (m_kinectController->IsEnabled())
	{
		return;
	}

	InitLookControllerIfNeeded();
	m_lookTarget = NULL;

	pos = pos - m_obj3d->GetCoords();

	CQuaternion q = m_obj3d->GetRotation()->GetAsDXQuaternion();
	CVector3D a;
	float angle;
	q.Extract(a, angle);
	CQuaternion q2(a, -angle);
	pos *= q2;
	pos.z = 160;

	m_lookController->SetTarget(CalVector(pos.x, pos.y, pos.z));
}

void CSkeletonAnimationObject::InitLookControllerIfNeeded()
{	
	return;

	if (!m_lookController)
	{
		m_lookController = new CalLookController(m_calModel);

		LOOKATPARAMS lookAtParams;
		lookAtParams.delay = 1000;
		lookAtParams.time = 1000;
		lookAtParams.trTime = 1000;
		m_lookController->Init(lookAtParams);		

		m_calModel->SetLookController(m_lookController);
	}

	m_lookController->Enable(true);
}

void CSkeletonAnimationObject::StopLookAtForTime(unsigned int time)
{
	m_endStopLookAtTime = (int)(g->ne.time + time);
}

void CSkeletonAnimationObject::LookAt(C3DObject* obj, int time, float koef)
{	
	return;
	if (obj)
	{
		if (m_kinectController)
		if (m_kinectController->IsEnabled())
		{
			return;
		}

		float minX, minY, minZ, maxX, maxY, maxZ;
		obj->GetBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
		CVector3D pos = obj->GetCoords() + CVector3D(0, 0, (maxZ - minZ) / 2.0f);
		LookAt(pos, time, koef);
		m_lookTarget = obj;
	}
	else
	{
		if (m_lookController)
		{
			m_lookController->Enable(false);
		}
		m_lookTarget = NULL;
		m_lookAtPos.Set(0, 0, 0);
	}
}

CSkeletonAnimationObject::~CSkeletonAnimationObject()
{
	FreeResources();

	if (m_obj3d)
	{
		if (m_obj3d->GetAnimation() == this)
		{
			m_obj3d->SetAnimation(NULL);
		}
	}

	DeleteCriticalSection(&m_calModelAccessCS);
	DeleteCriticalSection(&m_assignMotionCS);
}