
#include "stdafx.h"
#include "mdump.h"
#include "nrm3DObject.h"
#include "nrmMaterial.h"
#include "nrmPlugin.h"
#include "nrmMotion.h"
#include "nrmLight.h"
#include "nrmCharacter.h"
#include "nrm3DGroup.h"
#include "ResourceUrlParser.h"
#include "nrmAudio.h"
#include "UserData.h"
#include "nrm3DScene.h"
#include "MapManagers/MapManager.h"
#include "cal3d\model.h"
#include <algorithm>

#include "../ObjectsCreator.h"
#include "iasynchresourcemanager.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

#include "HelperFunctions.h"
#include "nrm3DSoundManager.h"

#define mlColor2DWORD(c)	(((DWORD)c.a)<<24)+(((DWORD)c.r)<<16)+(((DWORD)c.g)<<8)+(DWORD)c.b

nrm3DObject::nrm3DObject(mlMedia* apMLMedia): 
	nrmObject(apMLMedia),
	MP_VECTOR_INIT(m_materials),
	MP_MAP_INIT(m_materialMapTypes),
	MP_STRING_INIT(m_objectName),
	MP_VECTOR_INIT(m_soundList),
	MP_VECTOR_INIT(m_potentionalTextures)
{	
	m_obj3d = NULL;
	m_bScriptedTransformApplied = false;
	m_bScriptedPositionSet = false;
	m_bScriptedRotationSet = false;
	autoscripted_parent = NULL;
	m_wasCallbackPerformed = false;
	m_level = -1;
}


void nrm3DObject::LevelChanged(int level)
{	
	ChangeCollisionLevel(level);	
}

int nrm3DObject::GetCurrentLevel()
{
	if (!m_obj3d)
	{
		if (m_level != -1)
		{
			return m_level;
		}
		return 0;
	}
	
	return (int)(m_obj3d->GetUserData(USER_DATA_LEVEL));
}

void nrm3DObject::ChangeCollisionLevel(int level)
{
	if (m_obj3d)
	{
		if (m_obj3d->GetLODGroup())
		{
			m_obj3d->GetLODGroup()->ChangeCollisions(m_obj3d, false);
		}

		m_obj3d->SetUserData(USER_DATA_LEVEL, (void*)level);

		if (level != LEVEL_NO_COLLISION)
		if (m_obj3d->GetLODGroup())
		{
			m_obj3d->GetLODGroup()->ChangeCollisions(m_obj3d, true);
		}		
	}

	m_level = level;
}

void nrm3DObject::EnableShadows(bool isEnabled)
{
	if (m_obj3d)
	{
		if (m_obj3d->GetLODGroup())
		{
			m_obj3d->GetLODGroup()->EnableShadows(isEnabled);			
		}
	}
}

void nrm3DObject::showAxis()
{
	if (g->scm.GetActiveScene())
	{
		g->scm.GetActiveScene()->AddObjectWithAxes(m_obj3d);
	}
}

void nrm3DObject::hideAxis()
{
	if (g->scm.GetActiveScene())
	{
		g->scm.GetActiveScene()->RemoveObjectWithAxes(m_obj3d);
	}
}

void nrm3DObject::on_clear()
{	
}

	
// загрузка нового ресурса
bool nrm3DObject::SrcChanged()
{
	USES_CONVERSION;

	if (!mpMLMedia)
	{
		return false;
	}
	
	const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
	mlString sSRC;
	if (pwcSrc)
	{
		sSRC = pwcSrc;
	}
	
	mlI3DObject* pMLI3DO=mpMLMedia->GetI3DObject();
	mlMedia* pmlParentGroup=pMLI3DO->GetParentGroup();
	
	clear();

	wchar_t*	pwcName = GetStringProp("name");
	if (pwcName != NULL)	
	{
		m_objectName = W2A(pwcName);
	}

	if (m_objectName.size() == 0)
	{
		m_objectName = "gen_object_";
		m_objectName += IntToStr(g->o3dm.GetCount()).c_str();
	}

	std::string fn;
	if (pwcSrc)	
	{
		fn = W2A(pwcSrc);
	}

	bool isHighPriority = (GetMLMedia()->GetStringProp("highPriority"));	

	if (!sSRC.empty())
	{
		m_isLoaded = false;

		int level = m_level;
		wchar_t* _class = GetStringProp("className");
		std::string className = "";
		if (_class)
		{
			className = W2A(_class);
		}

		if (!CreateObject3D(fn, className, &m_obj3d, m_objectName, level, this, this, isHighPriority))
		{
			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad(RMML_LOAD_STATE_LOADERROR);
			}
			return false;
		}

		UnpickableCheck();		
	}

	ScanPlugins();
	InitializePlugins();
	
	if( pmlParentGroup)
	{
		nrm3DGroup*	parentGroup = (nrm3DGroup*)pmlParentGroup->GetSafeMO();
		if (parentGroup)	
		{
			parentGroup->AttachScriptedChild(GetMLMedia());
		}
		else
		{
			// иначе группа загрузится и сама добавит объекты к себе
		}
	}

	if ((m_obj3d) && (m_obj3d->GetLODGroup()))
	{
		bool isLoaded = m_obj3d->GetLODGroup()->IsGeometryLoaded();
		if (isLoaded)
		{
			m_isLoaded = true;

			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
				UnpickableCheck();
			}
		}
	}
	
	VisibleChanged();
	InCameraSpaceChanged();

	return true;
}

void nrm3DObject::OnChanged(int eventID)
{	
	if ((m_obj3d) && (!m_obj3d->IsDeleted()) && (m_obj3d->GetLODGroup()))
	{
		if (1 == eventID)
		{
			int lodNum = m_obj3d->GetLODNumber();
			SLODLevel* lodLevel = m_obj3d->GetLODGroup()->GetLodLevel(lodNum);
			if (lodLevel)
			{
				int dist = (int)lodLevel->lodMaxDist;
				if (m_obj3d->GetDistSq() > dist * dist)
				{
					dist = 25000;
				}
				if (GetMLMedia())
				{
					GetMLMedia()->GetI3DObject()->onLODChanged(dist);
				}
			}
		}

		if (!m_obj3d->GetLODGroup()->IsOk())
		{
			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad(RMML_LOAD_STATE_LOADERROR);
			}
			return;
		}

		bool isLoaded = m_obj3d->GetLODGroup()->IsGeometryLoaded();

		if ((isLoaded) && (!m_isLoaded))
		{
			if ((GetMLMedia()) && (GetMLMedia()->GetILoadable()))
			{
				GetMLMedia()->GetILoadable()->onLoad(RMML_LOAD_STATE_SRC_LOADED);
				UnpickableCheck();
				CheckCollisionsChanged();
				if (m_level != -1)
				{
					ChangeCollisionLevel(m_level);
				}				
			}
		}

		m_isLoaded = isLoaded;
	}
}

void nrm3DObject::UnpickableCheck()
{	
	bool isUnpickable = GetBooleanProp("notpickable");
	if (isUnpickable /*|| ((bool)m_obj3d->GetUserData(USER_DATA_IS_COMMUNICATION_AREA)) == true*/ )
	{
		g->lod.MakeUnpickable(m_obj3d->GetLODGroup());
	}
}

bool nrm3DObject::isObjectLoaded()
{
	if (!m_obj3d)
	{
		return false;
	}

	if (!m_obj3d->GetLODGroup())
	{
		return false;
	}

	return m_obj3d->GetLODGroup()->IsGeometryLoaded();
}

bool nrm3DObject::IsFullyLoaded()
{
	if (!m_obj3d)
	{
		return false;
	}

	if (!m_obj3d->GetLODGroup())
	{
		return false;
	}

	if (!m_obj3d->GetLODGroup()->IsFullyLoaded())
	{
		return false;
	}

	return true;
}

bool nrm3DObject::IsIntersectedWithOtherObjects()
{
	if (m_obj3d)
	{		
		m_obj3d->GetLODGroup()->ChangeCollisions(m_obj3d, false);
		m_obj3d->GetLODGroup()->ChangeCollisions(m_obj3d, true);
		return m_obj3d->IsCollidedWithOtherObjects();
	}
	else
	{
		return false;
	}
}

// реализация moI3DObject
ml3DPosition nrm3DObject::GetPos()
{
	ml3DPosition pos;
	ML_INIT_3DPOSITION(pos);

	if (!m_obj3d)
	{
#ifdef RESTORE_POSITION_AFTER_LOAD_ALEX_2009_08_01_CHANGES
		if (!isObjectLoaded())
		{
			if( m_bScriptedPositionSet)
			{
				pos.x = m_ScriptedPosition.x;
				pos.y = m_ScriptedPosition.y;
				pos.z = m_ScriptedPosition.z;
				return pos;
			}
		}
#endif
		return pos;
	}

	float x, y, z;
	m_obj3d->GetLocalCoords(&x, &y, &z);
	pos.x = x;
	pos.y = y;
	pos.z = z;

	return pos;
}

void nrm3DObject::PosChanged(ml3DPosition &pos)
{
	if (!isObjectLoaded())
	{
		m_ScriptedPosition = CalVector((float)pos.x, (float)pos.y, (float)pos.z);
		m_bScriptedPositionSet = true;
	}
#ifdef RESTORE_POSITION_AFTER_LOAD_ALEX_2009_08_01_CHANGES
	else
		m_bScriptedRotationSet = false;
#endif

	if (m_obj3d)
	{
		if (m_obj3d->GetParent())
		{
			CVector3D tmp((float)pos.x, (float)pos.y, (float)pos.z);
			m_obj3d->SetLocalCoords((float)tmp.x, (float)tmp.y, (float)tmp.z);
		}
		else
		{
			m_obj3d->SetLocalCoords((float)pos.x, (float)pos.y, (float)pos.z);
		}
	}
}

ml3DScale nrm3DObject::GetScale()
{
	ml3DScale scl;
	ML_INIT_3DSCALE(scl);

	if (!m_obj3d)
	{
		return scl;
	}

	float x, y, z;
	m_obj3d->GetScale(&x, &y, &z);
	scl.x = x;
	scl.y = y;
	scl.z = z;

	return scl;
}

void nrm3DObject::ScaleChanged(ml3DScale &scl)
{
	if (m_obj3d)
	{
		m_obj3d->SetScale((float)scl.x, (float)scl.y, (float)scl.z);
		CLODGroup* lodGroup = m_obj3d->GetLODGroup();
		if (lodGroup)
		{
			lodGroup->OnChanged(0);

			if (m_obj3d->GetAnimation())
			{
				lodGroup->SetCommonScale((float)scl.x, (float)scl.y, (float)scl.z);
			}
		}		
	}
}

ml3DRotation nrm3DObject::GetRotation()
{
	ml3DRotation rot;
	ML_INIT_3DROTATION(rot);
	if (!m_obj3d)
	{
		return rot;
	}

	if (!m_obj3d->GetLocalRotation())
	{
		return rot;
	}

	CQuaternion	rtn = m_obj3d->GetLocalRotation()->GetAsDXQuaternion();
	rtn.Normalize();
	rot.x = rtn.x;
	rot.y = rtn.y;
	rot.z = rtn.z;
	rot.a = rtn.w;

	return rot;
}

void nrm3DObject::RotationChanged(ml3DRotation &rot)
{
	if (!isObjectLoaded())
	{
		m_ScriptedRotation = CalQuaternion((float)rot.x, (float)rot.y, (float)rot.z, (float)rot.a);
		m_bScriptedRotationSet = true;
	}

	if (m_obj3d)
	{
		if (m_obj3d->GetLocalRotation())
		{
			CRotationPack rotation;
			rotation.SetAsDXQuaternion((float)rot.x, (float)rot.y, (float)rot.z, (float)rot.a);
			m_obj3d->SetRotation(&rotation);
			if (m_obj3d->GetLODGroup())
			{
			m_obj3d->GetLODGroup()->OnChanged(0);
		}
	}
}
}

void nrm3DObject::VisibleChanged()
{
	if (m_obj3d)
	if (GetMLMedia())
	{
		bool isVisible = GetMLMedia()->GetI3DObject()->GetVisible();	

		m_obj3d->SetVisible(isVisible);
	}
}

void nrm3DObject::InCameraSpaceChanged()
{
	if (m_obj3d)
	if (GetMLMedia())
	{
		bool inCameraSpace = GetMLMedia()->GetI3DObject()->GetInCameraSpace();	

		m_obj3d->SetInCameraSpace(inCameraSpace);
	}
}

void nrm3DObject::doMotion(mlMedia* apMotion)
{
	nrmMotion*	omotion = (nrmMotion*)apMotion->GetSafeMO();
	int	mlID = omotion->m_motionID;
	if (mlID == -1)	
	{
		return;
	}

	if (m_obj3d)
	if (m_obj3d->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
		if (sao)
		{
			sao->ExecuteAction(mlID, 0.0, 0.0);
			CMotion* motion = g->motm.GetObjectPointer(mlID);
			sao->StopLookAtForTime(motion->GetDuration());
		}
	}
}

void nrm3DObject::setMotion(mlMedia* apMotion, int aiDurMS)
{
	nrmMotion*	omotion = (nrmMotion*)apMotion->GetSafeMO();
	if (!omotion) 
	{
		return;
	}

	int	mlID = omotion->m_motionID;
	if (mlID == -1)	
	{
		return;
	}

	if (!m_obj3d)
	{
		return;
	}

	if (m_obj3d->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
		if (sao)
		{
			if( mpMLMedia->GetFrozen())
				aiDurMS = 0;
			sao->SetAction(mlID, (float)aiDurMS);
		}
	}
}

void nrm3DObject::removeMotion(mlMedia* apMotion)
{
	removeMotionTimed(apMotion, 0.0f);
}

void nrm3DObject::removeMotionTimed(mlMedia* apMotion, float fadeTime)
{
	nrmMotion*	omotion = (nrmMotion*)apMotion->GetSafeMO();
	int	mlID = omotion->m_motionID;
	if (mlID == -1)	
	{
		return;
	}

	if (m_obj3d->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
		if (sao)
		{
			sao->ClearAction(mlID, fadeTime);
		}
	}
}

void nrm3DObject::removeIdleTimed(mlMedia* apMotion, float fadeTime)
{
	nrmMotion*	omotion = (nrmMotion*)apMotion->GetSafeMO();
	int	mlID = omotion->m_motionID;
	if (mlID == -1)	
	{
		return;
	}

	if (m_obj3d->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
		if (sao)
		{
			sao->ClearAction(mlID, fadeTime);
			assert(false);
		}
	}
}

void nrm3DObject::HandleCal3DAction(ACTIONTYPE type, int actvalue)
{
	switch (type)
	{
	case AT_ACTION:
		{
			OnDoneAction(actvalue);
		}
		break;
	}
}

void nrm3DObject::update(DWORD /*dwTime*/, void* /*data*/)
{	
	/*if ( m_soundList.size() > 0)
	{
		rm::rmISoundManager* m_pSoundManager = m_pRenderManager->GetSoundManager();
		nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();
		if( m_pSoundManager != NULL && p3DSoundManager != NULL)
		{
			std::vector<nrmAudio *>::iterator iter = m_soundList.begin(),
											mlend = m_soundList.end();;
			for( ; iter != mlend; iter++)
			{
				mlMedia* pMedia = (*iter)->GetMLMedia();
				if (!pMedia)
				{
					continue;
				}
				nrmAudio* pMOAudio = (nrmAudio*) pMedia->GetSafeMO();
				if ( pMOAudio->m_iSoundId == -1)
					continue;
				
				if ( m_pSoundManager->IsPlaying( pMOAudio->m_iSoundId))
				{
					ATLASSERT( m_obj3d);
					if ( !m_obj3d)
						return;
					CVector3D dir = CVector3D( 0.0f, 1.0f, 0.0f);
					CVector3D av_dir = CVector3D( 1.0f, 0.0f, 0.0f);
					CRotationPack *rot_pack = m_obj3d->GetRotation();
					CQuaternion qrot = rot_pack->GetAsDXQuaternion();					
					dir *= qrot;
					//ml3DRotation ml_absRot = getAbsoluteRotation();
					//ml3DPosition ml_absPos = getAbsolutePosition();
					//CQuaternion absRot = CQuaternion( ml_absRot.x, ml_absRot.y, ml_absRot.z, ml_absRot.a);
					//dir *= absRot;//picollisionobject->get_rotation_absolute();
					CVector3D absPos = m_obj3d->GetCoords();//CVector3D( ml_absPos.x, ml_absPos.y, ml_absPos.z);

					if ( pMOAudio->is_factor_dist())
					{
						C3DSoundFactor t3DSoundFactor;
						if ( p3DSoundManager->calc( pMOAudio->get_factor_dist(), absPos, dir, &t3DSoundFactor))
							m_pSoundManager->Set3DSoundParams( pMOAudio->m_iSoundId, t3DSoundFactor._attenutedCoef,
							t3DSoundFactor._leftAttenutedCoef, t3DSoundFactor._rightAttenutedCoef);
						else
							m_pSoundManager->SetVolume( pMOAudio->m_iSoundId, 0.0f);
					}
					else
					{
						ml3DPosition vp_pos;
						ml3DPosition vp_dir;
						ml3DPosition cam_dir;
						// s						
						if ( p3DSoundManager->calc( absPos, dir, vp_pos, vp_dir, cam_dir))
							m_pSoundManager->Set3DSoundParams( pMOAudio->m_iSoundId, vp_pos, vp_dir, cam_dir);
						else
							m_pSoundManager->SetVolume( pMOAudio->m_iSoundId, 0.0f);
					}
				}
			}
		}
	}*/
}

void nrm3DObject::AbsEnabledChanged()
{
}

bool nrm3DObject::Get3DPropertiesInfo(mlSynchData &/*aData*/)
{
	assert(false);
	return true;
}

bool nrm3DObject::Get3DProperty(char* /*apszName*/,char* /*apszSubName*/, mlOutString &/*sVal*/)
{
	return true;
}

bool nrm3DObject::Set3DProperty(char* /*apszName*/,char* /*apszSubName*/,char* /*apszVal*/)
{
	assert(false);
	return true;
}

bool nrm3DObject::getCollisions(mlMedia** &/*avObjects*/, int &/*aiLength*/, ml3DPosition &/*aPos3D*/)
{
	// ??
	return false;
}

// изменился флагп роверки на пересечения
void nrm3DObject::CheckCollisionsChanged()
{
	if (GetMLMedia())
	{
		bool isEnabled = GetMLMedia()->GetI3DObject()->GetCheckCollisions();		
	
		if ((m_obj3d) && (m_obj3d->GetLODGroup()))
		{
			if ((g->phom.GetControlledObject()) && (g->phom.GetControlledObject()->GetObject3D() == m_obj3d))
			{
				g->phom.SetWorkStatus(isEnabled);
			}
			else
			{
				m_obj3d->GetLODGroup()->ChangeCollisions(m_obj3d, isEnabled);
			}
		}
	}
}

bool nrm3DObject::getIntersections(mlMedia** &/*avObjects*/, int &/*aiLength*/)
{
	return false;
}

bool nrm3DObject::getIntersectionsIn(mlMedia** &/*avObjects*/, int &/*aiLength*/)
{
	return false;
}

bool nrm3DObject::getIntersectionsOut(mlMedia** &/*avObjects*/, int &/*aiLength*/)
{
	return false;
}

void nrm3DObject::goPath(mlMedia* /*apPath*/, int /*aiFlags*/)
{
	// ??
}

void nrm3DObject::goPath(ml3DPosition* apPath, int aiPointCount, unsigned int /*auShiftTime*/)
{
}

mlMedia* nrm3DObject::getPath()
{ 
	return NULL;
}

// найти путь до указанного места
bool nrm3DObject::findPathTo(ml3DPosition &aFrom, ml3DPosition &aTo, ml3DPosition* &avPath, int &aiLength)
{
#define MAX_POINTS_IN_PATH 1000

	CPathLine* pathLine = MP_NEW(CPathLine);

	CVector3D start = g->phom.GetProjectionToLand(CVector3D(aFrom.x, aFrom.y, aFrom.z));
	CVector3D end = g->phom.GetProjectionToLand(CVector3D(aTo.x, aTo.y, aTo.z));

	g->pf.FindPath(pathLine, start, end);

	static ml3DPosition points[MAX_POINTS_IN_PATH];
	unsigned int pointCount = pathLine->GetPointCount();

	if ((pointCount > MAX_POINTS_IN_PATH) || (pointCount == 0))
	{
		return false;
	}
	
	int cnt = 0;

	for (unsigned int i = 0; i < pointCount; i++)
	{
		CVector3D pnt = pathLine->GetPoint(i);
		if ((i == 0) || ((pnt.x != pathLine->GetPoint(i - 1).x) || (pnt.y != pathLine->GetPoint(i - 1).y)))
		{			
			//pathLine->LogControlPoint(i);
			points[cnt].x = pnt.x;
			points[cnt].y = pnt.y;
			points[cnt].z = pnt.z;			
			cnt++;
		}		
	}

	// if my avatar
	if (g->phom.GetControlledObject())
	if (m_obj3d == g->phom.GetControlledObject()->GetObject3D())
	{
		nrmCharacter* avatar = (nrmCharacter*)this;
		if (avatar->GetWalkController())
		{
			avatar->GetWalkController()->SetLastPathLine(pathLine);
		}		
	}
	else
	{
		MP_DELETE(pathLine);
	}

	avPath = &points[0];
	aiLength = cnt;	

	return true;
}

void nrm3DObject::setPathVelocity(float /*afVelocity*/)
{
	// ??
}

void nrm3DObject::setPathCheckPoints(float* /*apfCheckPoints*/, int /*aiLength*/)
{
}

float nrm3DObject::getPassedCheckPoint()
{ 
	return 0.0f;
}

void nrm3DObject::setPathPercent(float /*afPercent*/)
{
	// ??
}

float nrm3DObject::getPathPercent()
{ 
	return 0.0f; 
}

void nrm3DObject::BoundingBoxChanged()
{
	if ((m_obj3d) && (mpMLMedia))
	{
		mlI3DObject* pMLI3DO = mpMLMedia->GetI3DObject();	
		assert(pMLI3DO);

		m_obj3d->SetBoundingBoxVisible(pMLI3DO->GetBoundingBox() > 0);
	}
}

void nrm3DObject::ShadowChanged()
{
	// deprecated
}

moIMaterial* nrm3DObject::GetMaterial(unsigned auID)
{
	if (!m_obj3d) 
	{
		return NULL;
	}

	if( auID > 64)
	{
		return NULL;
	}

	nrmMaterial* material;

	if( m_materials.size() <= auID)
	{
		int currentSize = m_materials.size();
		m_materials.resize(auID+1);
		for (int i=currentSize; i<m_materials.size(); i++)
			m_materials[i] = NULL;
	}
	if( m_materials[auID] == NULL)
	{
		if (GetMLMedia())
		{
			GetMLMedia()->GetI3DObject()->GetMaterial((unsigned char)auID);
		}	
		MP_NEW_V(m_materials[auID], nrmMaterial, this);
		m_materials[auID]->m_submeshID = auID;
	}
	material = m_materials[auID];

	return (moIMaterial*)material;
}

void nrm3DObject::FrozenChanged()
{
	if ((m_obj3d) && (mpMLMedia))
	{
		m_obj3d->SetFreeze((mpMLMedia->GetFrozen()));
	}
}

bool nrm3DObject::HasAttachParent()
{
	if( m_obj3d)
	{
		return m_obj3d->GetParent() == NULL ? false : true;
	}
	return false;
}

void nrm3DObject::MaterialsChanged()
{
	// ??
}

void nrm3DObject::MaterialChanged(unsigned auID)
{
	if (auID >= m_materials.size())	
	{
		GetMaterial(auID);

		if (auID >= m_materials.size())	
		{
			return;
		}
	}

	nrmMaterial* material = m_materials[auID];
	if (!GetMLMedia())
	{
		return;
	}

	mlIMaterial*	mtrl = GetMLMedia()->GetI3DObject()->GetMaterial((unsigned char)auID);
	if (!mtrl)	
	{
		return;
	}

	mlColor clr = mtrl->GetDiffuse();

	if (m_obj3d)
	if (m_obj3d->GetLODGroup())
	{
		std::string material = "<material><state name=\"color\" value=\"";
		material += IntToStr(clr.r);
		material += ";";
		material += IntToStr(clr.g);
		material += ";";
		material += IntToStr(clr.b);
		material += "\" /></material>";
		m_obj3d->GetLODGroup()->SetMaterialAsXML(auID, material);		
	}

	material->OnParamChanged(mtrl);
}

void nrm3DObject::MaterialMapChanged(unsigned auID, const wchar_t* apwcIndex)
{
	if (auID >= m_materials.size())	
	{
		GetMaterial(auID);

		if (auID >= m_materials.size())	
		{
			return;
		}
	}

	nrmMaterial* material = m_materials[auID];
	if (!GetMLMedia())
	{
		return;
	}

	mlIMaterial* mtrl = GetMLMedia()->GetI3DObject()->GetMaterial((unsigned char)auID);
	if (!mtrl)	
	{
		return;
	}

	material->OnMapChanged(mtrl, auID, apwcIndex);
}

void nrm3DObject::MotionChanged()
{
	if (!mpMLMedia)
	{
		return;
	}

	mlI3DObject* pMLI3DO=mpMLMedia->GetI3DObject();	
	assert(pMLI3DO);
	mlMedia* pMLElMotion=pMLI3DO->GetMotion();

	if (pMLElMotion == NULL) 
	{
		return;
	}

	setMotion(pMLElMotion, 0);
}

void nrm3DObject::OnDoneAction(int /*mID*/)
{
	/*if( mID == -1)	return;
	imotion*	p3dMotion = get_imotionlib()->get_imotion(mID);
	idatacontainer*	pidatacontainer;
	GET_INTERFACE( idatacontainer, p3dMotion, &pidatacontainer);
	mlMedia*	prmMotion = (mlMedia*)pidatacontainer->get_user_data(MLMEDIA_FROM_VISIBLEOBJECT);
	assert( prmMotion != NULL);
	prmMotion->GetIMotion()->onDone();*/
}

void nrm3DObject::OnSetAction(int /*mID*/)
{
	/*if( mID == -1)	return;
	imotion*	p3dMotion = get_imotionlib()->get_imotion(mID);
	idatacontainer*	pidatacontainer;
	GET_INTERFACE( idatacontainer, p3dMotion, &pidatacontainer);
	mlMedia*	prmMotion = (mlMedia*)pidatacontainer->get_user_data(MLMEDIA_FROM_VISIBLEOBJECT);
	assert( prmMotion != NULL);
	prmMotion->GetIMotion()->onSet();*/
}

/*
* Говорит объекту зааттачиться к другому объекту.
*
*/
bool nrm3DObject::attachTo(mlMedia* ap3DODest, const char* apszBoneName)
{
	if (!m_obj3d)
	{
		rm_trace("Error: No media to attach\n");
		return false;
	}

	if (ap3DODest)
	{
		nrm3DObject*	pnrm3DObject = (nrm3DObject*)ap3DODest->GetSafeMO();
		if ((!pnrm3DObject) || (!pnrm3DObject->m_obj3d))	
		{
			return false;
		}

		if ((!apszBoneName) || (0 == apszBoneName[0]))
		{
			//m_obj3d->AttachTo(NULL);
			m_obj3d->AttachTo(pnrm3DObject->m_obj3d);
/*#pragma warning(push)
#pragma warning(disable : 4239)
			CVector3D& pos = m_obj3d->GetCoords() - pnrm3DObject->m_obj3d->GetCoords();
#pragma warning(pop)
			m_obj3d->AttachTo(pnrm3DObject->m_obj3d);
			m_obj3d->SetLocalCoords(pos.x, pos.y, pos.z);*/
		}
		else
		{
			m_obj3d->AttachTo( pnrm3DObject->m_obj3d, apszBoneName);
		}
	}
	else
	{
		m_obj3d->AttachTo(NULL);
	}

	return true;
}

ml3DBounds nrm3DObject::getBounds(mlMedia* /*ap3DOCoordSysSrc*/)
{
	ml3DBounds bnd;
	ML_INIT_3DBOUNDS(bnd);

	if (!m_obj3d)
	{
		return bnd;
	}
	
	if (!m_obj3d->GetLODGroup())
	{
		return bnd;
	}
	
	float minX, minY, minZ, maxX, maxY, maxZ;
	if ((m_obj3d->GetLODGroup()->IsGeometryLoaded()) || ((m_boundsFromDB.x == 0.0f) && (m_boundsFromDB.y == 0.0f)))
	{
		//m_obj3d->GetBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
		m_obj3d->GetInitialAABB(minX, minY, minZ, maxX, maxY, maxZ);
	}
	else
	{
		float x, y, z;
		m_obj3d->GetLocalCoords(&x, &y, &z);
		minX = x - m_boundsFromDB.x;
		maxX = x + m_boundsFromDB.x;
		minY = y - m_boundsFromDB.y;
		maxY = y + m_boundsFromDB.y;
		minZ = z - m_boundsFromDB.z;
		maxZ = z + m_boundsFromDB.z;
	}

	bnd.xMin = minX;
	bnd.yMin = minY;
	bnd.zMin = minZ;
	bnd.xMax = maxX;
	bnd.yMax = maxY;
	bnd.zMax = maxZ;

	return bnd;
}

ml3DBounds nrm3DObject::getSubobjBounds(int /*aiMatID*/)
{
	ml3DBounds bnd;
	ML_INIT_3DBOUNDS(bnd);
	// ??
	return bnd;
}

int nrm3DObject::moveTo(ml3DPosition /*aPos3D*/, int /*aiDuaration*/, bool /*abCheckCollisions*/)
{
	return -1;
}

ml3DPosition nrm3DObject::getAbsolutePosition()
{
	ml3DPosition pos;
	ML_INIT_3DPOSITION(pos);
		
	if (!m_obj3d)
	{
#ifdef RESTORE_POSITION_AFTER_LOAD_ALEX_2009_08_01_CHANGES
		if (!isObjectLoaded())
		{
			if( m_bScriptedPositionSet)
			{
				pos.x = m_ScriptedPosition.x;
				pos.y = m_ScriptedPosition.y;
				pos.z = m_ScriptedPosition.z;
				return pos;
			}
		}
#endif
		return pos;
	}

	CVector3D vPos = m_obj3d->GetCoords();
	pos.x = vPos.x;
	pos.y = vPos.y;
	pos.z = vPos.z;
	return pos;
}

ml3DRotation nrm3DObject::getAbsoluteRotation()
{
	ml3DRotation rot;
	ML_INIT_3DROTATION(rot);
	if (!m_obj3d)
	{
		return rot;
	}

	CQuaternion	rtn = m_obj3d->GetRotation()->GetAsDXQuaternion();
	
	rot.x = rtn.x;
	rot.y = rtn.y;
	rot.z = rtn.z;
	rot.a = rtn.w;
	return rot;
}

bool nrm3DObject::attachSound(mlMedia* pAudio, ml3DPosition& pos3D)
{	
	if (!pAudio)
	{
		return false;
	}

	nrmAudio* pMOAudio = (nrmAudio*) pAudio->GetSafeMO();
	if ((pMOAudio == NULL) || (PMO_IS_NULL(pMOAudio)))
	{
		return false;
	}

	std::vector<nrmAudio*>::iterator iter = find(m_soundList.begin(), m_soundList.end(), pMOAudio);
	if (iter != m_soundList.end())
	{
		return true;
	}
	
	ATLTRACE( "nrm3DObject::attachSound:\n");

	rm::rmISoundManager* m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);	
	if( !m_pSoundManager)
		return false;

	nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();
	ATLASSERT( p3DSoundManager);		
	if( !p3DSoundManager)
		return false;

	ATLASSERT( pMOAudio->get_p3dobj() == this || pMOAudio->get_p3dobj() == NULL);	
	// если звук к кому то уже прицеплен и это не я - выходим
	if ( pMOAudio->get_p3dobj() != NULL && pMOAudio->get_p3dobj() != this)
		return false;

// если звук не прицеплен - прицепим ко мне
	if ( pMOAudio->get_p3dobj() == NULL)
		pMOAudio->set_p3dobj( this, pos3D);

	// запомним ссылку на звукo
	m_soundList.push_back( pMOAudio);

	mlMedia *pmlMedia = this->GetMLMedia();		
	if (!pmlMedia)
		return false;

	//p3DSoundManager->Update3dObject(this);
	//p3DSoundManager->UpdateSound(pMOAudio);
	//if ( pMOAudio->is_loading())
	//	return true;

	//if ( pMOAudio->m_iSoundId == -1)
	//	return false;	
	
	return true;
}

// Вызывается из деструктора nrmAudio
void nrm3DObject::detachSound(nrmAudio* apAudio)
{
	std::vector<nrmAudio *>::iterator iter = m_soundList.begin(),
		mlend = m_soundList.end();

	for( ; iter != mlend; iter++)
	{
		if (*iter == apAudio)
		{
			m_soundList.erase( iter);			
			return;
		}
	}
	//nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();
	//if (p3DSoundManager)
	//{
		//p3DSoundManager->Remove3dObject(this);
	//	p3DSoundManager->RemoveSound(apAudio);
	//}
}

bool nrm3DObject::detachSound(mlMedia* pAudio)
{
	if (!pAudio)
		return false;

	nrmAudio* pMOAudio = (nrmAudio*) pAudio->GetSafeMO();
	if ((!pMOAudio) || (PMO_IS_NULL(pMOAudio)))
		return false;

	std::vector<nrmAudio *>::iterator iter = m_soundList.begin(),
									mlend = m_soundList.end();

	for( ; iter != mlend; iter++)
	{
		mlMedia* pMedia = (*iter)->GetMLMedia();
		if (!pMedia)
		{
			continue;
		}
		unsigned int bornRealityID; 
		if (pMedia->GetSynchID(bornRealityID) == pAudio->GetSynchID(bornRealityID))
		{
			m_soundList.erase( iter);
			ml3DPosition pos3D;
			pos3D.x = pos3D.y = pos3D.z = 0.0;
			pMOAudio->set_p3dobj( NULL, pos3D);
			break;
		}
	}
	/*nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();
	if (p3DSoundManager)
	{
		nrmAudio* pMOAudio = (nrmAudio*) pAudio->GetSafeMO();
		if (!pMOAudio)
		{
			return false;
		}
		p3DSoundManager->RemoveSound(pMOAudio);
	}*/
	return true;
}

std::string nrm3DObject::getAllTextures()
{
	SLODLevel* lod = m_obj3d->GetLODGroup()->GetLodLevel(0);

	int modelID = lod->GetModelID();

	CModel* model = g->mm.GetObjectPointer(modelID);

	std::string textures; 

	for (int i = 0; i < model->GetTextureCount()-1; i++)
	{
		textures += model->GetTextureName(i)+";";
	}
	return textures;
}

int nrm3DObject::detectObjectMaterial(int x, int y)
{
	int id = m_obj3d->GetLODGroup()->DetectObjectMaterial(m_obj3d, x, y);
	return id;
}

bool nrm3DObject::addPotentialTexture(mlMedia* apVisibleObject)
{
	if (!apVisibleObject)
	{
		return false;
	}

	if (!m_obj3d)
	{
		return false;
	}

	wchar_t* src = apVisibleObject->GetStringProp("loadsrc");

	if (!src)
	{
		src = apVisibleObject->GetStringProp("src");
	}

	USES_CONVERSION;
	gRM->resLib->GetAsynchResMan()->AddPotentionalTexture(m_obj3d, W2A(src), 1.0f);

	return true;
}

bool nrm3DObject::addPotentialTexture(const wchar_t* apwcSrc)
{
	if (!apwcSrc)
	{
		return false;
	}

	if (!m_obj3d)
	{
		return false;
	}

	USES_CONVERSION;
	gRM->resLib->GetAsynchResMan()->AddPotentionalTexture(m_obj3d, W2A(apwcSrc), 0.5f);
	
	return true;
}

void nrm3DObject::onPointArrived()
{
	if (GetMLMedia())
	{
		GetMLMedia()->GetI3DObject()->onPassed();
	}
}

void nrm3DObject::CreateShadow(mlE3DShadowType /*shadowType*/, ml3DPlane /*shadowPlane*/, mlColor /*shadowColor*/, mlMedia* /*shadowLight*/)
{
	// deprecated
}

void nrm3DObject::SetMaterialMapMedia(int smid, mlMedia* apmlMedia)
{
	std::map<int, mlMedia*>::iterator	it = m_materialMapTypes.find(smid);
	if (it != m_materialMapTypes.end())	
	{
		m_materialMapTypes.erase(it);
	}

	m_materialMapTypes.insert(std::map<int, mlMedia*>::value_type(smid, apmlMedia));
}

mlMedia* nrm3DObject::GetMaterialMapMedia(int smid)
{
	std::map<int, mlMedia*>::iterator	it = m_materialMapTypes.find(smid);
	if (it != m_materialMapTypes.end())	
	{
		return it->second;
	}

	return MLMT_UNDEFINED;
}

void nrm3DObject::GravitationChanged()
{
	if (GetMLMedia())
	{
		bool isGravitationEnabled = GetMLMedia()->GetI3DObject()->GetGravitation();

		if (g->phom.GetControlledObject())
		if (g->phom.GetControlledObject()->GetObject3D() == m_obj3d)
		{
			g->phom.GetControlledObject()->ChangeGravitation(isGravitationEnabled);
		}
	}
}

void nrm3DObject::IntangibleChanged()
{
}

void nrm3DObject::BillboardChanged()
{
}

void nrm3DObject::UnpickableChanged()
{
}

void nrm3DObject::BoundingBoxColorChanged()
{
	if ((m_obj3d) && (mpMLMedia))
	{
		mlI3DObject* pMLI3DO = mpMLMedia->GetI3DObject();	
		mlColor color = pMLI3DO->GetBoundingBoxColor();
		m_obj3d->SetBoundingBoxColor(color.r, color.g, color.b, color.a);
	}
}

void nrm3DObject::PropIsSet()
{
}

void	nrm3DObject::AddAsynchSound(IAsynchResource* piasynchresource)
{
	if (piasynchresource)
	{
		piasynchresource->AddObject(m_obj3d);
	}
}

void	nrm3DObject::set_auto_scripted_parent(nrm3DGroup* apnrm3DObject)
{
	autoscripted_parent = apnrm3DObject;
}

// получает прогресс загрузки объекта
void nrm3DObject::OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, byte /*percent*/)
{
}

// объект загружен
void nrm3DObject::OnAsynchResourceLoaded(IAsynchResource* /*asynch*/)
{
}

// ошибка загрузки
void nrm3DObject::OnAsynchResourceError(IAsynchResource* /*asynch*/)
{
	// ??
}

bool nrm3DObject::InitializePlugins()
{
	bool at_least_one_initialized = false;

	std::vector<nrmPlugin*>::iterator it = vPlugins.begin();
	std::vector<nrmPlugin*>::iterator itEnd = vPlugins.end();
	for( ; it != itEnd; it++)
	{
		if (!InitializePlugin(*it))
		{
			continue;
		}

		if (!(*it)->GetPluginInterface())
		{
			continue;
		}

		IPlugin* plugin = (*it)->GetPluginInterface()->GetPluginInterface();
		m_obj3d->SetPlugin(plugin);
		plugin->SetObject(m_obj3d);
		(*it)->GetPluginInterface()->SetPropsContainer(this);
	}

	return at_least_one_initialized;
}

void nrm3DObject::SetBoundsFromDB(ml3DPosition& pos)
{
	m_boundsFromDB.x = (float)pos.x;
	m_boundsFromDB.y = (float)pos.y;
	m_boundsFromDB.z = (float)pos.z;
}

// установка материала по его описанию (обычно XML)
void nrm3DObject::SetMaterialDescription(int aiMatIdx, const wchar_t* apwcDescr)
{
	if ((m_obj3d) && (m_obj3d->GetLODGroup()))
	{
		USES_CONVERSION;
		std::string str = W2A(apwcDescr);
		m_obj3d->GetLODGroup()->SetMaterialAsXML(aiMatIdx, str);
	}
}

void nrm3DObject::ColorChanged()
{
	if ((m_obj3d) && (mpMLMedia) && (m_obj3d->GetLODGroup()))
	{
		mlI3DObject* pMLI3DO = mpMLMedia->GetI3DObject();	
		mlColor color = pMLI3DO->GetColor();
		m_obj3d->GetLODGroup()->SetCommonColor(color.r, color.g, color.b);
	}
}

void nrm3DObject::OpacityChanged()
{
	if ((m_obj3d) && (mpMLMedia) && (m_obj3d->GetLODGroup()))
	{
		mlI3DObject* pMLI3DO = mpMLMedia->GetI3DObject();	
		unsigned char opacity = pMLI3DO->GetOpacity();
		m_obj3d->GetLODGroup()->SetCommonAlpha(opacity);
	}
}

void nrm3DObject::SetMaterialTiling(int textureSlotID, float tilingKoef) 
{
	if ((!m_obj3d) || (!m_obj3d->GetLODGroup()))
	{
		return;
	}

	m_obj3d->GetLODGroup()->SetTextureTiling(textureSlotID, CVector2D(tilingKoef, tilingKoef));
}

bool nrm3DObject::GetMaterialTextureSrc(int aiMatIdx, const wchar_t* apwcMapName, mlOutString &asSrc)
{
	if ((!m_obj3d) || (!m_obj3d->GetLODGroup()))
	{
		return false;
	}

	int modelID = m_obj3d->GetLODGroup()->GetLodLevel(0)->GetModelID();
	CModel* model = g->mm.GetObjectPointer(modelID);

	USES_CONVERSION;
	if ((aiMatIdx >= 0) && (aiMatIdx < model->GetTextureCount()))
	{
		std::string texture = model->GetTextureName(aiMatIdx);

		if (texture.size() > 0)
		if (texture[0] != ':')
		{
			std::string name = W2A( model->GetName());
			for (int i = 0; i < 2; i++)
			{
				int pos1 = name.find_last_of("\\");
				int pos2 = name.find_last_of("/");
				if (pos2 > pos1)
				{
					pos1 = pos2;
				}
				name = name.substr(0, pos1);
			}
			
			name += "\\textures\\";
			texture =  name + texture;
		}

		USES_CONVERSION;
		std::wstring wsSrc = A2W(texture.c_str());
		asSrc = wsSrc;
		return true;
	}
	
	return false;
}

nrm3DObject::~nrm3DObject()
{
	MP_VECTOR<MP_STRING>::iterator it = m_potentionalTextures.begin();
	MP_VECTOR<MP_STRING>::iterator itEnd = m_potentionalTextures.end();

	USES_CONVERSION;
	for ( ; it != itEnd; it++)
	{
		gRM->resLib->GetAsynchResMan()->DeletePotentionalTexture(m_obj3d, (char *)(*it).c_str());
	}

	if (m_obj3d)
	{
		if ((bool)m_obj3d->GetUserData(USER_DATA_IS_LOCATION) == true)
		{
			gRM->mapMan->RemoveLocation(  W2A(m_obj3d->GetName()));
		}
		else if ((bool)m_obj3d->GetUserData(USER_DATA_IS_COMMUNICATION_AREA) == true)
		{
			gRM->mapMan->RemoveCommunicationArea( W2A(m_obj3d->GetName()));
		}

		m_obj3d->DeleteChangesListener(this);

		if (m_obj3d->GetID() != 0xFFFFFFFF)
		{
			g->o3dm.DeleteObject(m_obj3d->GetID());

			if (m_obj3d->GetLODGroup())
			if (m_obj3d->GetLODGroup()->IsDynamic())
			{
				m_obj3d->GetLODGroup()->DeleteAllObjects();
				g->lod.DeleteObject(m_obj3d->GetLODGroup()->GetID());
			}
			else
			{
				m_obj3d->GetLODGroup()->RemoveObject(m_obj3d);
			}
		}
		else
		{
			m_obj3d->FreeResources();
		}

		m_obj3d->SetUserData(USER_DATA_NRMOBJECT, NULL);
	}

	std::vector<nrmAudio *>::iterator iter = m_soundList.begin(),
									end = m_soundList.end();
	for( ; iter != end; ++iter)
	{
		nrmAudio* pMOAudio = *iter;
		if ( pMOAudio)
		{
			ml3DPosition pos3D;
			pos3D.x = 0;	pos3D.y = 0;	pos3D.z = 0;
			pMOAudio->set_p3dobj( NULL, pos3D);
		}
	}
	m_soundList.clear();

	//nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();
	//if (p3DSoundManager)
	//{
	//	p3DSoundManager->Remove3dObject(this);
	//}

	std::vector<nrmMaterial*>::iterator	mit;
	for( mit  =m_materials.begin(); mit != m_materials.end(); mit++)
		if( (*mit)!=NULL)
		{
			nrmMaterial* mat = (*mit);
			MP_DELETE(mat);
		}
	m_materials.clear();
	
	if( autoscripted_parent)
	{
		autoscripted_parent->auto_scripted_object_is_destroing( this);
		autoscripted_parent = NULL;
	}

	on_clear();}