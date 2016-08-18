// nrmCharacter.cpp: implementation of the nrmCharacter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmCharacter.h"
#include "nrmMotion.h"
#include "nrmText.h"
#include "nrmPath3D.h"
#include "nrmAudio.h"
#include "nrmVisemes.h"
#include "nrmCamera.h"
#include "RMComp.h"
#include "UserData.h"
#include "nrm3DSoundManager.h"
#include "ResourceUrlParser.h"
#include "GlobalSingletonStorage.h"
#include "HelperFunctions.h"
#include "VoipManClient.h"
#include <yvals.h>
#include <atlconv.h>

nrmCharacter::nrmCharacter(mlMedia* apMLMedia): 
	MP_STRING_INIT(sCurrentVoiceFileName),
	MP_STRING_INIT(sVoiceUserName),
	nrm3DObject(apMLMedia),
	m_isAsynch(false),
	mpSpeech(NULL)
{
	unsigned int bornRealityID = 0;
	uID = apMLMedia->GetSynchID(bornRealityID);
	mpIdles = NULL;
	dwSpeechDuration = 0xFFFFFFFF;
	m_pLeftStepSound = m_pRightStepSound = NULL;
	ZeroMemory(&keys, 255*sizeof(byte));
	bPlayEnd = false;
    
	m_walkController = MP_NEW(CWalkController);
	m_walkController->SetCharacter(NULL);
	m_isTrackingRightHandEnabled = false;
	m_isTrackingLeftHandEnabled = false;
	m_FirstLexem = 0;
	m_isEmptyBinState = false;
}


void nrmCharacter::OnChanged(int eventID)
{
	if (!GetMLMedia())
	{
		return;
	}		

	if (2 == eventID)
	{
		if ((GetMLMedia()) && (GetMLMedia()->GetICharacter()))
		{
			GetMLMedia()->GetICharacter()->onNeedToChangePosition();
		}
		return;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();

	if (eventID == 4)
	{		
		if (GetMLMedia())
		{
			ml3DPosition rightHandPosition, rightHandDirection, rightHandDirectionExtended;
			CVector3D rightHandPositionVec = sao->GetRightHandPosition();
			CVector3D rightHandDirectionVec = sao->GetRightHandDirection();
			CVector3D rightHandDirectionExtendedVec = sao->GetExtendedRightHandDirection();
			ML_COPY_3DPOSITION(rightHandPosition, rightHandPositionVec);
			ML_COPY_3DPOSITION(rightHandDirection, rightHandDirectionVec);
			ML_COPY_3DPOSITION(rightHandDirectionExtended, rightHandDirectionExtendedVec);			

			if (GetMLMedia()->GetICharacter())
			{
				GetMLMedia()->GetICharacter()->onRightHandPosChanged(rightHandPosition, rightHandDirection, 
					rightHandDirectionExtended);
			}
		}
		return;
	}

	if (eventID == 5)
	{		
		if (GetMLMedia())
		{
			ml3DPosition leftHandPosition, leftHandDirection, leftHandDirectionExtended;
			CVector3D leftHandPositionVec = sao->GetLeftHandPosition();
			CVector3D leftHandDirectionVec = sao->GetLeftHandDirection();
			CVector3D leftHandDirectionExtendedVec = sao->GetExtendedLeftHandDirection();
			ML_COPY_3DPOSITION(leftHandPosition, leftHandPositionVec);
			ML_COPY_3DPOSITION(leftHandDirection, leftHandDirectionVec);
			ML_COPY_3DPOSITION(leftHandDirectionExtended, leftHandDirectionExtendedVec);			

			if (GetMLMedia()->GetICharacter())
			{
				GetMLMedia()->GetICharacter()->onLeftHandPosChanged(leftHandPosition, leftHandDirection, 
					leftHandDirectionExtended);
			}
		}
		return;
	}

	if (sao)
	if (sao->IsLoaded())
	{
		if (!m_isLoaded)
		{
			m_isLoaded = true;

			if (eventID != -1)
			{
				OnLoaded();
			}

			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);					

			if (m_isTrackingRightHandEnabled)
			{
				SetTrackingRightHand(m_isTrackingRightHandEnabled);
			}

			if (m_isTrackingLeftHandEnabled)
			{
				SetTrackingLeftHand(m_isTrackingLeftHandEnabled);
			}
			CheckCollisionsChanged();

		}
	}
	else if (eventID != -1)
	{
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);		
	}	
}

bool	nrmCharacter::setSleepingState(bool isSleeped)
{
	if (!m_obj3d)
	{
		return false;
	}

	if (!m_obj3d->GetAnimation())
	{
		return false;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	sao->SetSleepingState(isSleeped);

	return true;
}

bool	nrmCharacter::handle_message(LPVOID target, DWORD message, LPVOID /*data*/)
{
	if (!m_obj3d)
	{
		return FALSE;
	}

	nrmCharacter*	person = (nrmCharacter*)target;
	if( message == WME_PATHDONE)
	{
		person->onPointArrived();
	}
	else	if( message == WME_AFTER_TURN)
	{
		person->onTurnArrived();
	}
	return TRUE;
}

void nrmCharacter::OnSetRenderManager()
{
	m_pRenderManager->RegisterController(m_walkController);
}

void nrmCharacter::SetTrackingRightHand(bool isEnabled)
{
	m_isTrackingRightHandEnabled = true;
	
	if (!m_obj3d)
	{
		return;
	}

	if (!m_obj3d->GetAnimation())
	{
		return;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	sao->SetTrackingRightHand(isEnabled);
}

void nrmCharacter::SetTrackingLeftHand(bool isEnabled)
{
	m_isTrackingLeftHandEnabled = true;
	
	if (!m_obj3d)
	{
		return;
	}

	if (!m_obj3d->GetAnimation())
	{
		return;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	sao->SetTrackingLeftHand(isEnabled);
}

void nrmCharacter::UnpickableChanged()
{
	if (!m_obj3d)
	{
		return;
	}

	if (!m_obj3d->GetLODGroup())
	{
		return;
	}

	if (!GetMLMedia())
	{
		return;
	}

	bool isUnpickable = GetMLMedia()->GetICharacter()->GetUnpickable();		
	if (isUnpickable)
	{
		g->lod.MakeUnpickable(m_obj3d->GetLODGroup());
	}
	else
	{
		g->lod.MakePickable(m_obj3d->GetLODGroup());
	}
}

void nrmCharacter::VisibleChanged()
{
	if (!m_obj3d)
	{
		return;
	}

	if (!GetMLMedia())
	{
		return;
	}

	bool isVisible = GetMLMedia()->GetI3DObject()->GetVisible();	
	m_obj3d->SetVisible(isVisible);

	if (!m_obj3d->GetLODGroup())
	{
		return;
	}	
	
	m_obj3d->GetLODGroup()->SetVisibleAll(isVisible);
}

void nrmCharacter::OnLoaded()
{
	CVector3D oldPos = m_obj3d->GetCoords();
	bool isManageNeeded = true;

	int modelID = m_obj3d->GetAnimation()->GetModelID();

	CRotationPack* rotationLocal = m_obj3d->GetLocalRotation();

	//m_obj3d->SetUserData(USER_DATA_LEVEL, (void*)LEVEL_NO_COLLISION);

	CLODGroup* lodGroup;
	if (-1 == g->lod.GetObjectNumber(m_objectName))
	{
		int lodGroupID = g->lod.AddObject(m_objectName);
		lodGroup = g->lod.GetObjectPointer(lodGroupID);
		lodGroup->AddModelAsLODLevel(&modelID, 1, 1, 4000.0f);
		lodGroup->AddModelAsLODLevel(&modelID, 0, 1, 7000.0f);
		
		std::string billboardName = "billboard_";
		billboardName += m_objectName;

		g->rl->GetAsynchResMan()->SetLoadingMesh(m_obj3d);
		int billboardModelID = g->mm.LoadAsynch(":ui:resources/avatars/billboard/model/mesh.bmf.zip", billboardName);		
		g->rl->GetAsynchResMan()->SetLoadingMesh(NULL);
		lodGroup->AddModelAsLODLevel(&billboardModelID, 0, 1, 20000.0f);
		lodGroup->Finalize();
	}
	else
	{
		int lodGroupID = g->lod.GetObjectNumber(m_objectName);
		lodGroup = g->lod.GetObjectPointer(lodGroupID);
	}
	
	g->o3dm.AddObject3D(m_obj3d, lodGroup, 0, 0, 0, rotationLocal, false);	

	if (isManageNeeded)
	{
		g->o3dm.ManageObject(m_obj3d);
	}
	else
	{
		g->o3dm.RestoreObject(m_obj3d);
		g->lw.WriteLn("Restore object ", m_obj3d->GetName());
	}

	m_obj3d->GetLODGroup()->OnObjectCreated(m_obj3d);
	m_obj3d->SetUserData(USER_DATA_NRMOBJECT, this);
	m_obj3d->AddChangesListener(this);
	if ((oldPos.x != 0) || (oldPos.y != 0) || (oldPos.z != 0))
	{
		m_obj3d->SetCoords(oldPos);
	}
	else
	{
		m_obj3d->SetCoords(m_ScriptedPosition.x, m_ScriptedPosition.y, m_ScriptedPosition.z);
	}

	/*if (m_isAsynch)
	{*/		
		m_obj3d->GetLODGroup()->SetDrawState(false);
	//}
}

// загрузка нового ресурса
bool nrmCharacter::SrcChanged()
{
	USES_CONVERSION;

	if (!mpMLMedia)
	{
		return false;
	}
	
	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	mlString sSRC;	
	if (pwcSrc)
	{
		sSRC = pwcSrc;
	}
	else
	{
		return false;
	}

#ifdef RESTORE_POSITION_AFTER_LOAD_ALEX_2009_08_01_CHANGES
	if( isObjectLoaded())
	{
		ml3DPosition pos = getAbsolutePosition();
		m_ScriptedPosition = CalVector((float)pos.x, (float)pos.y, (float)pos.z);
		m_bScriptedPositionSet = true;
	}
#endif

	wchar_t*	pwcName = GetStringProp("name");
	if (pwcName != NULL)	
	{
		m_objectName = W2A(pwcName);
	}

	if (m_objectName.size() == 0)
	{
		m_objectName = W2A(pwcSrc);
	}

	std::string src = W2A(pwcSrc);
	IKinect* kinectObj = NULL;

	C3DObject* oldObj3d = m_obj3d;
	bool isKinectEnabled = false;
	if (m_obj3d)
	{
		int saoID = g->skom.GetObjectNumber(m_obj3d, src);	
	
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
		if (sao)
		{
			if (sao->GetKinectObject())
			{
				kinectObj = sao->GetKinectObject();
				isKinectEnabled = sao->IsKinectEnabled();
			}
			if (sao->GetPath() == src)
			{
				GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);					
				return true;
			}
		}
	}

	bool isManageNeeded = false;

	if (m_obj3d)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
		if (sao)
		{
			sao->DeleteChangesListener(this);
		}

		m_obj3d->SetRotation(NULL);

		if (m_obj3d->GetLODGroup())
		{
			m_obj3d->GetLODGroup()->DeleteAllObjects();
			m_obj3d->GetLODGroup()->UnmarkDeleted();
			g->lod.ManageObject(m_obj3d->GetLODGroup());
		}

		m_obj3d->UnmarkDeleted();
	}
	else
	{
		m_obj3d = MP_NEW(C3DObject);
		m_obj3d->SetName( A2W(m_objectName.c_str()));
		isManageNeeded = true;
	}
	
	m_walkController->SetCharacter(this);
	
	int saoID = g->skom.GetObjectNumber(m_obj3d, src);	
	if (saoID != -1)
	{
		g->skom.DeleteObject(saoID);
	}
	
	saoID = g->skom.AddObject(m_obj3d, src);	
	CSkeletonAnimationObject* animation = g->skom.GetObjectPointer(saoID);	
	animation->AddChangesListener(this);
	if (kinectObj)
	{
		animation->SetKinectObject(kinectObj);
		if (isKinectEnabled)
		{
			animation->EnableKinect();
		}
	}
	m_isLoaded = false;
	if (GetMLMedia()->GetBooleanProp("asynch") == true)
	{ 
		animation->LoadAsynch(src, false, NULL);
		m_obj3d->SetAnimation(animation);
		if (m_obj3d->GetLODGroup())
		{			
			m_obj3d->GetLODGroup()->SetDrawState(false);
		}
		m_isAsynch = true;
	}
	else if (animation->Load(src, false, NULL))
	{
		m_obj3d->SetAnimation(animation);		
		OnLoaded();		
		OnChanged(-1);
		m_isAsynch = false;
	}
	else
	{	
		m_isAsynch = false;
		return false;
	}

	ScanPlugins();
	InitializePlugins();

	//mplugin = new cn3DObject(this);

	VisibleChanged();
	UnpickableChanged();
	AbsEnabledChanged();
	MotionChanged();
	ShadowChanged();
	IdlesChanged();
	VisemesChanged();
	FrozenChanged();
	CheckCollisionsChanged();

	loaded();
	return true;
}

void nrmCharacter::PrepareForDestroy()
{
	USES_CONVERSION;
	std::string deletedName = "deleted_";
	deletedName += W2A( m_obj3d->GetName());
	deletedName += "_";
	deletedName += IntToStr((int)this);
	g->o3dm.RenameObject(m_obj3d->GetID(), deletedName);			

	deletedName = "deleted_";
	deletedName += W2A( m_obj3d->GetLODGroup()->GetName());
	deletedName += "_";
	deletedName += IntToStr((int)this);
	g->lod.RenameObject(m_obj3d->GetLODGroup()->GetID(), deletedName);
}

CWalkController* nrmCharacter::GetWalkController()const
{
	return m_walkController;
}

void nrmCharacter::loaded()
{
	MovementChanged();	

	// Visemes management
	VisemesChanged();
}

void nrmCharacter::GetSynchData(mlSynchData &aSynchData)
{
	if (m_obj3d)
	{
		CalVector pos;
		m_obj3d->GetCoords(&pos.x, &pos.y, &pos.z);
		CQuaternion rtn = m_obj3d->GetRotation()->GetAsDXQuaternion();
		aSynchData.put(&pos, sizeof(CalVector));
		aSynchData.put(&rtn, sizeof(CQuaternion));
	}
}

void nrmCharacter::SetSynchData(mlSynchData &aSynchData)
{
	if (!m_obj3d)
	{
		return;
	}

	CalVector	pos;
	CalQuaternion	rtn;

	aSynchData.get(&pos, sizeof(CalVector));
	aSynchData.get(&rtn, sizeof(CalQuaternion));

	m_obj3d->SetCoords(pos.x, pos.y, pos.z);
	if (m_obj3d->GetRotation())
	{
		m_obj3d->GetRotation()->SetAsDXQuaternion(rtn.x, rtn.y, rtn.z, rtn.w);
	}
}

void nrmCharacter::onKeyDown()
{
	return;
/*	
	if (keys[code] == 0)
	{
		switch(code)
		{
		case 87:	//w
			m_walkController->HandleEvent(WME_START_GO_FORWARD, (LPVOID)1);
			break;
		case 83:	//s
			m_walkController->HandleEvent(WME_START_GO_BACKWARD, (LPVOID)1);
			break;
		case 65:	//a
			m_walkController->HandleEvent(WME_START_L_STRAFE, (LPVOID)1);
			break;
		case 68:	//d
			m_walkController->HandleEvent(WME_START_R_STRAFE, (LPVOID)1);
			break;
		case 188:	//<
			m_walkController->HandleEvent(WME_START_L_RTN, (LPVOID)1);
			break;
		case 190:	//>
			m_walkController->HandleEvent(WME_START_R_RTN, (LPVOID)1);
			break;
		}

		keys[code] = 1;
	}
*/
}

void nrmCharacter::onKeyUp()
{
return;
	mlIButton*	pBtn = GetMLMedia()->GetIButton();
	int	code = pBtn->GetKeyCode();
	
	
	if( keys[code] == 1)
	{
		switch(code)
		{
		case 87:	//w
			m_walkController->HandleEvent(WME_END_GO_FORWARD, (LPVOID)0);
			break;
		case 83:	//s
			m_walkController->HandleEvent(WME_END_GO_BACKWARD, (LPVOID)0);
			break;
		case 65:	//a
			m_walkController->HandleEvent(WME_END_L_STRAFE, (LPVOID)0);
			break;
		case 68:	//d
			m_walkController->HandleEvent(WME_END_R_STRAFE, (LPVOID)0);
			break;
		case 188:	//<
			m_walkController->HandleEvent(WME_END_L_RTN, (LPVOID)1);
			break;
		case 190:	//>
			m_walkController->HandleEvent(WME_END_R_RTN, (LPVOID)1);
			break;
		}
		keys[code] = 0;
	}
}

/**
 * реализация moICharacter
 */

bool nrmCharacter::say(mlMedia* apSpeech, int* apDuration)
{
	if (!m_obj3d)
	{
		return FALSE;
	}

	assert(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = m_pRenderManager->GetSoundManager();
	if( !m_pSoundManager)
	{
		rm_trace("Sound manager is not set (nrmCharacter::say)\n");
		return false;
	}

	if( mpSpeech)
	{
		m_pSoundManager->StopSound(((nrmAudio*)mpSpeech->GetSafeMO())->m_iSoundId);
		/*if( picharacter)
			picharacter->set_current_lexeme(-1);*/
		detachSound( mpSpeech);
		mpSpeech = NULL;
	}
	if( !apSpeech || apSpeech->GetIAudible() == NULL) return false;

	dwSpeechDuration = 0xFFFFFFFF;
	nrmAudio* pMOSpeech = (nrmAudio*) apSpeech->GetSafeMO();
	assert( pMOSpeech->get_p3dobj() == this || pMOSpeech->get_p3dobj() == NULL);
	if ( pMOSpeech->get_p3dobj() != NULL && pMOSpeech->get_p3dobj() != this)
		return false;
	if ( pMOSpeech->get_p3dobj() == NULL)
	{
		ml3DPosition pos3D;
		pos3D.x = 0.0f;
		pos3D.y = 0.0f;
		pos3D.z = 0.0f;
		pMOSpeech->set_p3dobj( this, pos3D);
	}

	mpSpeech = apSpeech;
	mlIContinuous* pCont = apSpeech->GetIContinuous();
	if( pCont == NULL)	// speech
	{ 		
		ml3DPosition pos3D;
		pos3D.x = 0.0f;
		pos3D.y = 0.0f;
		pos3D.z = 0.0f;
		if ( !attachSound( apSpeech, pos3D))
			return false;		
		if ( pMOSpeech->IsLoading())
		{
			return true;
		}

		if( pMOSpeech->m_iSoundId == -1)
		{
			const wchar_t* pwcSrc = apSpeech->GetILoadable()->GetSrc();
			rm_trace("Cannot open sound '%S' (nrmCharacter::say)\n", pwcSrc);
			return false;
		}

		m_pSoundManager->GetDuration( pMOSpeech->m_iSoundId, dwSpeechDuration);
		if( apDuration)
			*apDuration = dwSpeechDuration;

		bPlayEnd = false;
		//return m_pSoundManager->PlaySound( pMOSpeech->m_iSoundId, uID, false);
		return false;
	}
	else
	{	
		// audio
		pCont->Rewind();
		pCont->Play();
		return pCont->GetPlaying();
	}
	mpSpeech = apSpeech;
}

mlMedia* nrmCharacter::GetSpeech()
{
	return mpSpeech;
}

mlICharacter** nrmCharacter::GetInterlocutors()
{
	return NULL;
}

void nrmCharacter::update(DWORD dwTime, void *data)
{
	if (!m_obj3d)
	{
		return;
	}

	assert(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = m_pRenderManager->GetSoundManager();
	if( m_pSoundManager != NULL && mpSpeech != NULL)
	{
		nrmAudio* pMOSpeech = (nrmAudio*) mpSpeech->GetSafeMO();
		if ( pMOSpeech->m_iSoundId == -1)
		{
			nrm3DObject::update(dwTime, data);
			return;
		}

		if( !m_pSoundManager->IsPlaying( pMOSpeech->m_iSoundId))
		{
			if ( bPlayEnd)
			{
				mpSpeech->GetISpeech()->onSaid();
				if (GetMLMedia())
				{
					GetMLMedia()->GetICharacter()->onSaid();
				}
				nrmAudio* pMOSpeech=(nrmAudio*)mpSpeech->GetSafeMO();
				if( pMOSpeech)
				{
					m_pSoundManager->StopSound(pMOSpeech->m_iSoundId);
				}
				mpSpeech=NULL;
				//picharacter->set_current_lexeme(-1);
			}
			else
				bPlayEnd = true;
		}
		else
		{
			nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();
			if ((p3DSoundManager != NULL) && (m_obj3d))
			{
				CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
				if (sao)
				{
					CVector3D _dir = sao->GetFaceVector();
					if (m_obj3d->GetRotation())
					{
						_dir *= m_obj3d->GetRotation()->GetAsDXQuaternion();
					}
					CVector3D dir(_dir.x, _dir.y, _dir.z);
					CVector3D pos;
					m_obj3d->GetCoords(&pos.x, &pos.y, &pos.z);
					if ( pMOSpeech->IsFactorDist())
					{
						C3DSoundFactor t3DSoundFactor;
						if ( p3DSoundManager->calc( pMOSpeech->GetFactorDist(), pMOSpeech->GetMinDist(), pos, dir, &t3DSoundFactor))
							m_pSoundManager->Set3DSoundParams( pMOSpeech->m_iSoundId, t3DSoundFactor._attenutedCoef,
							t3DSoundFactor._leftAttenutedCoef, t3DSoundFactor._rightAttenutedCoef);
						else
							m_pSoundManager->SetVolume( pMOSpeech->m_iSoundId, 0.0f);
					}
					else
					{
						ml3DPosition vp_pos;
						ml3DPosition vp_dir;
						ml3DPosition cam_dir;
						if ( p3DSoundManager->calc( pos, dir, vp_pos, vp_dir, cam_dir))
							m_pSoundManager->Set3DSoundParams( pMOSpeech->m_iSoundId, vp_pos, vp_dir, cam_dir);
						else
							m_pSoundManager->SetVolume( pMOSpeech->m_iSoundId, 0.0f);
					}
				}
			}
			dwSpeechDuration -= dwTime;
			/*int	letter_id;
			m_pSoundManager->GetLexema( uID, letter_id);
			//picharacter->set_current_lexeme( letter_id);
			*/
		}		
	}

	nrm3DObject::update(dwTime, data);	
}

void nrmCharacter::handleKinectState()
{
	if( !m_obj3d)
		return;

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if ( !sao)
		return;

	if( !sao->IsControlledByKinect())
		return;

	mlIBinSynch* binSynch = GetMLMedia()->GetIBinSynch();
	if (!binSynch)
		return;

	unsigned int aStateVersion = NULL;
	unsigned char* data = NULL;
	int dataSize;
	GetFullStateImpl( aStateVersion, NULL, dataSize);
	
	if (dataSize == 0)
	{
		if (!m_isEmptyBinState)
		{
			unsigned char data = 0;
			binSynch->StateChanged(0, BSSCC_CLEAR, &data, 1);
			m_isEmptyBinState = true;
		}
	}
	else
	{
		data = MP_NEW_ARR(unsigned char, dataSize);
		bool isNewState = GetFullStateImpl( aStateVersion, data, dataSize);

		if (isNewState)
		{
			binSynch->StateChanged(0, BSSCC_REPLACE_ALL, data, dataSize);
			m_isEmptyBinState = false;
		}
		MP_DELETE_ARR(data);
	}
}

void nrmCharacter::ResetSynchKinectState()
{
	if( !m_obj3d)
		return;

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if ( !sao)
		return;

	if( !sao->IsControlledByKinect())
		Reset();
}

void nrmCharacter::MovementChanged()
{
	if (!m_obj3d)
	{
		return;
	}

	if (!mpMLMedia)
	{
		return;
	}

	m_walkController->HandleEvent(WME_CHANGE_MOTIONS_START, (LPVOID)0);
	
	/************************************************************************/
	/* Init character movements
	/************************************************************************/
	// MID_STGF, MID_GF, MID_ENGF, MID_STGB, MID_GB, MID_ENGB, MID_SL, MID_SR, MID_MAINIDLE
	wchar_t*	names[]=
	{
		OLESTR("_start_forward"), OLESTR("_forward"), OLESTR("_end_forward"),
			OLESTR("_start_backward"), OLESTR("_backward"), OLESTR("_end_backward"),
			OLESTR("_step_left"), OLESTR("_step_right"), 
			OLESTR("_lrotation"), OLESTR("_end_lrotation"), 
			OLESTR("_rrotation"), OLESTR("_end_rrotation"), 
			OLESTR("_idle"), OLESTR("_rstepsound"), OLESTR("_lstepsound"),
			OLESTR("_stay")
	};

	mlICharacter*	pch = mpMLMedia->GetICharacter();
	mlIMovement*	pmoves = pch->GetMovement();
	if (!pmoves)	
	{
		return;
	}

	m_walkController->set_step_length((float)pmoves->GetStepLength());
	m_walkController->set_rotation_speed((float)pmoves->GetRotSpeed());
		
	if (pmoves)
	{
		int motionCount = sizeof(names) / sizeof(wchar_t*);
		for (int i = 0; i < motionCount; i++)
		{
			mlMedia*	lmotion;
			lmotion = pmoves->GetMotion(names[i]);
			if (!lmotion)	
			{				
				continue;
			}

			if( lmotion->GetType() == MLMT_MOTION)
			{
				if (i + 1 != motionCount)
				{
					nrmMotion*	omotion = (nrmMotion*)lmotion->GetSafeMO();
					int	mlID = omotion->m_motionID;
					if (mlID < 0) continue;
					omotion->m_motion->SetDuration((float)omotion->GetRMMLDuration());
					m_walkController->add_motion((WALK_MOTIONS_IDs)i, mlID);
				}
			}
			else	if( lmotion->GetType() == MLMT_AUDIO)
			{
				/*nrmAudio*	pAudio = (nrmAudio*)lmotion->GetSafeMO();
				if( names[i] == OLESTR("rstepsound"))
					m_pLeftStepSound = pAudio;
				else
					m_pRightStepSound = pAudio;*/
			}
		}
	}

	m_walkController->HandleEvent(WME_CHANGE_MOTIONS_END, (LPVOID)0);

	/*mlMedia*	motion = NULL;
	motion = pmoves->GetMotion(L"eye_left");
	if (motion)
	{
		nrmMotion*	omotion = (nrmMotion*)motion->GetSafeMO();
		int	mlID = omotion->m_motionID;

		motion = pmoves->GetMotion(L"eye_right");
		if (motion)
		{
			omotion = (nrmMotion*)motion->GetSafeMO();
			int	mlID2 = omotion->m_motionID;

			sao->SetEyePoses(mlID, mlID2);
		}
	}*/
}

void nrmCharacter::setIndexedVisemes(int /*iFirstFomen*/, int /*iSecondFonem*/, float /*fMiddleFactor*/)
{
	// written by Slava for avatar's mimic via skeleton animation
	// now it's not used

	/*mlICharacter*	pch = mpMLMedia->GetICharacter();
	if (!pch){return;}
	mlIMovement*	pmoves = pch->GetMovement();
	if (!pmoves){return;}
	mlMedia*	motion = NULL;
	motion = pmoves->GetMotion(L"viseme");
	if (motion)
	{
		if ( m_obj3d )
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
			if ( sao )
			{
				nrmMotion*	omotion = (nrmMotion*)motion->GetSafeMO();
				if ( omotion )
				{
					sao->SetVisemesMotion(omotion->m_motionID);
					sao->SetVisemeParams(iFirstFomen, iSecondFonem, fMiddleFactor);
				}
			}
		}
	}*/
}

#include <time.h>
void nrmCharacter::handleVoiceData( int /*nSampleRate*/, const short* /*pRawAudio*/, int /*nSamples*/)
{
}

bool nrmCharacter::isSayingAudioFile(LPCSTR alpcFileName)
{
	if( sCurrentVoiceFileName.size() == 0)
		return false;

	std::string fileName = alpcFileName;
	fileName = StringToLower(fileName);

	if (StringToLower(sCurrentVoiceFileName) != fileName)
		return false;

	return true;
}

ml3DPosition nrmCharacter::getAbsolutePosition()
{
	return nrm3DObject::getAbsolutePosition();
}

ml3DRotation nrmCharacter::getAbsoluteRotation()
{
	return nrm3DObject::getAbsoluteRotation();
}

void nrmCharacter::VisemesChanged()
{
	}
	
void nrmCharacter::IdlesChanged()
{
	if (mpMLMedia)
	{
		mlICharacter* pMLIChar=mpMLMedia->GetICharacter();
	
		SetIdles(pMLIChar->GetIdles());
	}
}

void nrmCharacter::FaceIdlesChanged()
{
	if (mpMLMedia)
	{
		mlICharacter* pMLIChar=mpMLMedia->GetICharacter();
		mlIIdles* pMLIIdles = pMLIChar->GetFaceIdles();

		SetFaceIdles(pMLIIdles);
	}
}

void nrmCharacter::goPath(mlMedia* /*apPath*/, int /*aiFlags*/)
{
}

void nrmCharacter::goPath(ml3DPosition* apPath, int aiPointCount, unsigned int auShiftTime)
{
	if (aiPointCount <= 0)
	{
		return;
	}

	CVector3D* path = MP_NEW_ARR(CVector3D, aiPointCount);

	for (int i = 0; i < aiPointCount; i++)
	{
		path[i].Set(apPath[i].x, apPath[i].y, apPath[i].z);
	}

	m_walkController->GoPath(path, aiPointCount);

	ShiftTime(auShiftTime);

	MP_DELETE_ARR(path);
}

void nrmCharacter::setPathVelocity(float /*afVelocity*/)
{
}


void nrmCharacter::setPathPercent(float /*afPercent*/)
{
}

float nrmCharacter::getPathPercent()
{
	return 0.0f;
}

// показать на заданную точку или объект
bool nrmCharacter::pointByHandTo(const ml3DPosition& aPos, mlMedia* apObject)
{
	// если aPos==(0,0,0) и apObject == NULL, то перестаем показывать на то, на что показывали
	// ??

	if (!m_obj3d)
	{
		return false;
	}

	if (!m_obj3d->GetAnimation())
	{
		return false;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
	sao->PointByHandTo(aPos.x, aPos.y, aPos.z);

	return true;
}


void nrmCharacter::ShiftTime( unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return;
	}

	if (shiftTime > WME_FIRST_TRANSTIME)
	{
		bool frozen = m_walkController->IsCharacterFrozen();
		if (frozen)
		{
			m_walkController->Unfreeze();
		}

		unsigned int prepairTime = (unsigned int)WME_FIRST_TRANSTIME + 1;
		unsigned int motionTime = shiftTime - prepairTime;
		m_walkController->Update(10); // переводим аватара в состояние START_GO
		m_walkController->Update(prepairTime); // переводим аватара в состояние GO
		m_walkController->Update(motionTime); // задаем время задержки

		if (frozen)
		{
			m_walkController->Freeze();
		}

		if (m_obj3d->GetLODGroup())
		{
			m_obj3d->GetLODGroup()->Update(false, true, motionTime);
		}
	}
}

bool nrmCharacter::goTo(mlMedia* /*apObject*/, ml3DPosition* /*apTurnToPos*/,  mlMedia* /*apTurnToObject*/, unsigned int /* shiftTime */)
{
	if (!m_obj3d)
	{
		return false;
	}
	m_walkController->EnableImmediateMotionRemoveMode( m_walkController->IsCharacterFrozen());
	m_walkController->HandleEvent(WME_STOP, NULL);
	return true;
}

bool nrmCharacter::goTo(ml3DPosition &aPos, ml3DPosition* apTurnToPos, mlMedia* apTurnToObject, unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}

	if (!m_walkController)
	{
		return false;
	}

	CalVector dest((float)aPos.x, (float)aPos.y, (float)aPos.z);
	CalVector turn;
	if (apTurnToPos)
	{
		turn.set((float)apTurnToPos->x, (float)apTurnToPos->y, (float)apTurnToPos->z);
	}

	m_walkController->EnableImmediateMotionRemoveMode( m_walkController->IsCharacterFrozen());
	m_walkController->HandleEvent(WME_GOTO_POINT, (LPVOID)&dest);
		
	if (apTurnToPos)
	{
		m_walkController->HandleEvent(WME_AFTER_TURN, &turn);
	}
	else if (apTurnToObject != NULL)
	{
		nrm3DObject* pnrm3DObject = (nrm3DObject*)apTurnToObject->GetSafeMO();
		C3DObject* obj = pnrm3DObject->m_obj3d;
		if (obj)
		{
			CalVector turnPoint;
			obj->GetCoords(&turnPoint.x, &turnPoint.y, &turnPoint.z);
			m_walkController->HandleEvent(WME_AFTER_TURN, &turnPoint);
		}
	}

	ShiftTime(shiftTime);

	return true; 
}

bool nrmCharacter::turnTo(mlMedia* /*apObject*/)
{
	return true;
}

bool nrmCharacter::turnTo(ml3DPosition &/*aPos*/)
{
	return true;
}

bool nrmCharacter::turnTo(int /*aiFlags*/)
{
	assert(false);
	return true;
}

bool nrmCharacter::lookAt(mlMedia* apObject, int aiMs, double adKoef)
{
	if (!m_obj3d)
	{
		return false;
	}

	if (!m_obj3d->GetAnimation())
	{
		return false;
	}

	if (apObject)
	{
		if ((apObject->GetType() == MLMT_OBJECT) || (apObject->GetType() == MLMT_CHARACTER))
		{
			nrm3DObject* nrm3DObj = (nrm3DObject*)apObject->GetSafeMO();
			if (!nrm3DObj)	
			{
				return false;
			}
			
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
			if (sao)
			{
				sao->LookAt(nrm3DObj->m_obj3d, aiMs, adKoef);
			}else
			{
				return false;
			}
		}
	}
	else
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
		if ( sao )
		{
			sao->LookAt(NULL, aiMs, adKoef);
		}
		else	
		{
			return false;
		}
		
	}

	return true;
}

bool nrmCharacter::lookAt(ml3DPosition& aPos, int aiMs, double adKoef)
{
	if (!m_obj3d)
	{
		return false;
	}

	if (!m_obj3d->GetAnimation())
	{
		return false;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();

#pragma warning(push)
#pragma warning(disable : 4239)
	sao->LookAt(CVector3D(aPos.x, aPos.y, aPos.z), aiMs, adKoef);
#pragma warning(pop)

	return true; 
}

bool nrmCharacter::lookAtByEyes(mlMedia* /*apObject*/, int /*aiMs*/, double /*adKoef*/)
{
	// ??
	return true;
}

bool nrmCharacter::lookAtByEyes(ml3DPosition &/*aPos*/, int /*aiMs*/, double /*adKoef*/)
{
	// ??
	return true;
}

bool nrmCharacter::lookAtByEyes(const wchar_t* /*apDest*/, int /*aiMs*/, double /*adKoef*/)
{
	// ??
	return true;
}

void nrmCharacter::SetIdles(mlMedia* apIdles)
{
	if (m_obj3d == NULL || !m_obj3d->GetAnimation())
	{
		return;
	}

	((CSkeletonAnimationObject*)m_obj3d->GetAnimation())->RemoveIdles();

	if( apIdles == NULL)
	{
		return;
	}

	const wchar_t* pwcSrc = apIdles->GetILoadable()->GetSrc();
	if (pwcSrc != NULL && *pwcSrc != L'\0')
	{
		USES_CONVERSION;
		std::string path = W2A(pwcSrc);
		((CSkeletonAnimationObject*)m_obj3d->GetAnimation())->LoadIdles(path);
		return;
	}

	// Grab all idles animation
	for (int i = 0; ; i++)
	{
		mlMedia* pmlidle = apIdles->GetIIdles()->GetIdle(i);
		if (!pmlidle) 
		{
			break;
		}

		mlIIdle* pidle = pmlidle->GetIIdle();

		mlMedia* pmlIMotion = pidle->GetMotion();
		if (!pmlIMotion)
		{
			continue;
		}

		nrmMotion*	idleMotion = (nrmMotion*)pmlIMotion->GetSafeMO();
		if (!idleMotion)	
		{
			continue;
		}
		
		((CSkeletonAnimationObject*)m_obj3d->GetAnimation())->AddIdle(idleMotion->m_motionID, pidle->GetBetween(), (int)pidle->GetFreq());
	}
}

void nrmCharacter::SetFaceIdles(mlIIdles* /*apIdles*/)
{
	/*if (!m_obj3d->GetAnimation())
	{
		return;
	}

	m_animation->RemoveFaceIdles();

	if( apIdles == NULL)
	{
		return;
	}

	// Grab all idles animation
	for (int i = 0; ; i++)
	{
		mlMedia* pmlidle = apIdles->GetIdle(i);
		if (!pmlidle) 
		{
			break;
		}

		mlIIdle* pidle = pmlidle->GetIIdle();

		mlMedia* pmlIMotion = pidle->GetMotion();
		if (!pmlIMotion)
		{
			continue;
		}

		nrmMotion*	idleMotion = (nrmMotion*)pmlIMotion->GetSafeMO();
		if (!idleMotion)	
		{
			continue;
		}
		
		m_animation->AddFaceIdle(idleMotion->m_motionID, pidle->GetBetween(), (int)pidle->GetFreq());
	}*/
}

void nrmCharacter::onPointArrived()
{
	if (!m_obj3d)
	{
		return;
	}

	nrm3DObject::onPointArrived();
}

void nrmCharacter::onTurnArrived()
{
	if (!m_obj3d)
	{
		return;
	}

	if ((GetMLMedia()) && (GetMLMedia()->GetICharacter()))
	{
		GetMLMedia()->GetICharacter()->onTurned();
	}
}

void nrmCharacter::HandleCal3DAction(ACTIONTYPE type, int actvalue)
{
	if (!m_obj3d)
	{
		return;
	}

	nrm3DObject::HandleCal3DAction(type, actvalue);

	switch (type)
	{
	case AT_STEPSOUNDR:
		if(m_pRightStepSound != NULL)
		{
			mlMedia*	pSoundMLMedia = m_pRightStepSound->GetMLMedia();
			pSoundMLMedia->GetIContinuous()->Rewind();
			pSoundMLMedia->GetIContinuous()->Play();
		}
		break;
	case AT_STEPSOUNDL:
		if(m_pLeftStepSound != NULL)
		{
			mlMedia*	pSoundMLMedia = m_pRightStepSound->GetMLMedia();
			pSoundMLMedia->GetIContinuous()->Rewind();
			pSoundMLMedia->GetIContinuous()->Play();
		}	
		break;
	}
}

bool nrmCharacter::addInterlocutor(mlICharacter* /*apCharacter*/){
	// ??
	return true;
}

bool nrmCharacter::removeInterlocutor(mlICharacter* /*apCharacter*/){
	// ??
	return true;
}

bool nrmCharacter::setViseme(mlMedia* /*apVisemes*/, unsigned int /*auiIndex*/, int /*aiTime*/, int /*aiKeepupTime*/){
	// ??
	return true;
}

bool nrmCharacter::unsetViseme(mlMedia* /*apVisemes*/, unsigned int /*auiIndex*/, int /*aiTime*/){
	// ??
	return true;
}

bool nrmCharacter::doVisemes(mlMedia* /*apVisemes*/, int /*aiTime*/){
	// ??
	return true;
}

bool nrmCharacter::setLexeme( unsigned int auiIndex, int aiTime)
{
	if( !m_obj3d)
		return false;
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
	if( !sao)
		return false;
	sao->SetLexeme( auiIndex, aiTime);
	return true;	
}

// установить персонажа в свободное место в области, заданной аргументами:
// apStartPos - начальная позиция, adRadius - радиус области
// abInRoom - поместить в пределах комнаты
bool nrmCharacter::setToRandomFreePlace(ml3DPosition* /*apStartPos*/, double /*adRadius*/, bool /*abInRoom*/)
	{
	return true;
}

void nrmCharacter::toInitialPose()
{
	if (!m_obj3d)	
	{
		return;
	}

	/*CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
	if (!sao)
	{
		return;
	}*/
	if (m_obj3d->GetLODGroup())
	{
		m_obj3d->GetLODGroup()->ToInitialPose();
	}
}

void nrmCharacter::fromInitialPose()
{
	if (!m_obj3d)	
	{
		return;
	}
/*
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
	if (!sao)
	{
		return;
	}
*/
	if (m_obj3d->GetLODGroup())
	{
		m_obj3d->GetLODGroup()->FromInitialPose();
	}
//	sao->FromInitialPose();
}
			
// иди вперед
bool nrmCharacter::goForward(unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_STOP, NULL);
	m_walkController->HandleEvent(WME_START_GO_FORWARD, (LPVOID)1);
	ShiftTime(shiftTime);

	return true;
}

// пяться
bool nrmCharacter::goBackward(unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_STOP, NULL);
	m_walkController->HandleEvent(WME_START_GO_BACKWARD, (LPVOID)1);
	ShiftTime(shiftTime);

	return true;
}

// иди вперед и налево
bool nrmCharacter::goLeft(unsigned int shiftTime){
	if (!m_obj3d)
	{
		return false;
	}
	
	m_walkController->HandleEvent(WME_START_GO_LEFT, (LPVOID)1);
	ShiftTime(shiftTime);

	return true;
}

// иди вперед и направо
bool nrmCharacter::goRight(unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_START_GO_RIGHT, (LPVOID)1);
	ShiftTime(shiftTime);

	return true;
}

// иди назад и налево
bool nrmCharacter::goBackLeft(unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_START_GO_BACK_LEFT, (LPVOID)1);
	ShiftTime(shiftTime);

	return true;
}

// иди назад и направо
bool nrmCharacter::goBackRight(unsigned int shiftTime){
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_START_GO_BACK_RIGHT, (LPVOID)1);
	ShiftTime(shiftTime);

	return true;
}

// иди боком влево
bool nrmCharacter::strafeLeft(unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}
	
	m_walkController->HandleEvent(WME_START_L_STRAFE, (LPVOID)1);
	ShiftTime(shiftTime);

	return true;
}

// иди боком вправо
bool nrmCharacter::strafeRight(unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_START_R_STRAFE, (LPVOID)1);
	ShiftTime(shiftTime);
	
	return true;
}

// перестань идти 
bool nrmCharacter::stopGo()
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_END_GO_FORWARD, (LPVOID)0);
	m_walkController->HandleEvent(WME_END_GO_BACKWARD, (LPVOID)0);
	m_walkController->HandleEvent(WME_END_L_STRAFE, (LPVOID)0);
	m_walkController->HandleEvent(WME_END_R_STRAFE, (LPVOID)0);

	m_walkController->ClearAllActions();
	
	return true;
}

// поворачивайся влево
bool nrmCharacter::turnLeft(unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_STOP, NULL);
	m_walkController->HandleEvent(WME_START_L_RTN, (LPVOID)1);
	ShiftTime(shiftTime);
	
	return true;
}

// поворачивайся вправо
bool nrmCharacter::turnRight(unsigned int shiftTime)
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_STOP, NULL);
	m_walkController->HandleEvent(WME_START_R_RTN, (LPVOID)1);
	ShiftTime(shiftTime);
	
	return true;
}

// перестань поворачиваться
bool nrmCharacter::stopTurn()
{
	if (!m_obj3d)
	{
		return false;
	}

	m_walkController->HandleEvent(WME_END_L_RTN, (LPVOID)1);
	m_walkController->HandleEvent(WME_END_R_RTN, (LPVOID)1);

	return true;
}

void nrmCharacter::FrozenChanged()
{
bool abfrozen = false;
wchar_t* apwcName = NULL;
if( mpMLMedia)
{
	apwcName = mpMLMedia->GetStringProp("name");
	abfrozen = mpMLMedia->GetFrozen();
}

	ApplyFrozenToSpeech();

	nrm3DObject::FrozenChanged();
}

void nrmCharacter::ApplyFrozenToSpeech()
{
	if (mpMLMedia)
	{
		if ((m_pRenderManager->GetVoipMan()) && (sVoiceUserName.size() > 0))
		{
			if (mpMLMedia->GetFrozen())
			{
				m_pRenderManager->GetVoipMan()->PauseUserAudio( sVoiceUserName.c_str());
			}
			else
			{
				m_pRenderManager->GetVoipMan()->ResumeUserAudio( sVoiceUserName.c_str());
			}
		}
	}
}

bool nrmCharacter::sayRecordedVoice( LPCSTR alpcUserName, LPCSTR alpcFileName, LPCSTR alpcCommunicationAreaName, unsigned int auPosition, unsigned int auSyncVersion)
{
	ATLASSERT(m_pRenderManager);
	ATLASSERT(alpcUserName);
	if( !alpcUserName)
		return false;

	if( !m_pRenderManager->GetVoipMan())
		return false;

	sVoiceUserName = alpcUserName;

	// Сначала накладываем паузу или воспроизведение
	ApplyFrozenToSpeech();

	if (alpcFileName) 
	{
		/*if (mpMLMedia)
		{
			ATLASSERT(g_nrman->GetContext()->mpSM->isCurSeancePlaying() && !mpMLMedia->GetFrozen());	// если сработает позвать Alex`a
			закоментил, мотому что в любом случае должна происходить установка файла на воспроизведение при seek(). max
		}*/
		sCurrentVoiceFileName = alpcFileName;
		m_pRenderManager->GetVoipMan()->StartUserAudioPlayback( alpcUserName, alpcFileName, alpcCommunicationAreaName, auPosition, auSyncVersion);
	}
	else
	{
		sCurrentVoiceFileName = "";
		m_pRenderManager->GetVoipMan()->StopUserAudioPlayback( alpcUserName);
	}

	return true;
}

bool nrmCharacter::getFaceVector(float& x, float& y, float& z)
{
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if (!sao)
		return false;
	CVector3D vec3d = sao->GetRotatedFaceVector();
	x = vec3d.x;
	y = vec3d.y;
	z = vec3d.z;
	return true;
}

bool nrmCharacter::attachSound(mlMedia* pAudio, ml3DPosition& pos3D)
{
	//return false;
	return nrm3DObject::attachSound(pAudio, pos3D);
}

void nrmCharacter::IdlesSrcChanged(const wchar_t* apwcIdlesName, const wchar_t* apwcSrc)
{
	IdlesChanged();
}

// C сервера пришло полное состояние: установить его
// В случае, если Kinect не подключен, а состояние синхронизации говорит об обратном - нужно его обнулить
bool nrmCharacter::SetFullState(unsigned int auStateVersion, const unsigned char* apState, int aiSize)
{
	if (!m_obj3d)
	{
		return false;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if ( !sao)
	{
		return false;	
	}

	if( !sao->IsControlledByKinect())
	{
		if( aiSize != 0)
		{
			if (g->phom.GetControlledObject())
			{
				if (m_obj3d == g->phom.GetControlledObject()->GetObject3D())
				{
					mlIBinSynch* binSynch = GetMLMedia()->GetIBinSynch();
					if (binSynch)
					{
						binSynch->StateChanged(0, BSSCC_CLEAR, NULL, 0);
						return false;
					}
				}
			}
		}
	}
	return sao->SetKinectSynchData( apState, aiSize);
}

// с сервера пришли изменения состояния: обновить состояние
bool nrmCharacter::UpdateState(unsigned int auStateVersion, BSSCCode aCode, const unsigned char* apState, int aiSize
								, unsigned long aulOffset, unsigned long aulBlockSize)
{
	ATLASSERT( aulOffset == 0);
	ATLASSERT( aulBlockSize == 0);
	return SetFullState( auStateVersion, apState, aiSize);
}

// нужно отправить на сервер полное состояние: запросить его
// (возвращает размер данных, apState может быть null)
unsigned long nrmCharacter::GetFullState(unsigned int& auStateVersion, const unsigned char* apState, int aiSize)
{
	if (!m_obj3d)
	{
		return 0;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if (!sao)
	{
		return 0;
	}

	if (!sao->IsControlledByKinect())
	{
		return 0;
	}

	mlIBinSynch* binSynch = GetMLMedia()->GetIBinSynch();
	if (!binSynch)
	{
		return 0;
	}


	auStateVersion = 0;
	if (!apState)
	{
		aiSize = sao->GetKinectSynchDataSize();
	}
	else
	{
		sao->GetKinectSynchData(apState, aiSize);
	}

	return aiSize;
}

// нужно отправить на сервер полное состояние: запросить его
// (возвращает размер данных, apState может быть null)
bool nrmCharacter::GetFullStateImpl(unsigned int& auStateVersion, const unsigned char* apState, int& aiSize)
{
	if (!m_obj3d)
	{
		return false;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if (!sao)
	{
		return false;
	}

	if (!sao->IsControlledByKinect())
	{
		return false;
	}

	mlIBinSynch* binSynch = GetMLMedia()->GetIBinSynch();
	if (!binSynch)
	{
		return false;
	}

	bool isNewState = false;
	auStateVersion = 0;
	if (!apState)
	{
		aiSize = sao->GetKinectSynchDataSize();
	}
	else
	{
		isNewState = sao->GetKinectSynchData(apState, aiSize);
	}

	return isNewState;
}

// Сбросить состояние объекта, обнулить его версию
void nrmCharacter::Reset()
{
	SetFullState( 0, NULL, 0);
}

moIBinSynch* nrmCharacter::GetIBinSynch()
{
	return this;
}

bool nrmCharacter::setKinectDeskPoints(const ml3DPosition& aPos, const ml3DPosition& aPos2, const moMedia* moObj)
{
	if (!moObj)
	{
		return false;
	}

	if (!m_obj3d)
	{
		return false;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if (!sao)
	{
		return false;
	}

	CVector3D start(aPos.x, aPos.y, aPos.z);
	CVector3D end(aPos2.x, aPos2.y, aPos2.z);
	
	nrm3DObject* obj = (nrm3DObject*)moObj;

	if (!obj->m_obj3d)
	{
		return false;
	}

	sao->SetKinectDeskPoints(start, end, obj->m_obj3d);

	return true;
}

void nrmCharacter::disableKinectDesk()
{
	if (!m_obj3d)
	{
		return;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if (!sao)
	{
		return;
	}

	sao->DisableKinectDesk();
}

nrmCharacter::~nrmCharacter()
{
	if (m_obj3d)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
		if (sao)
		{
			sao->DeleteChangesListener(this);
		}
	}

	m_pRenderManager->UnregisterController(m_walkController);
	MP_DELETE(m_walkController);
}
