
#include "stdafx.h"
#include "cal3d/CoreAnimationFlags.h"
#include "GlobalSingletonStorage.h"
#include "SkeletonAnimationObject.h"
#include "HelperFunctions.h"
#include "WalkController.h"
#include "cal3d/skeleton.h"
#include "UserData.h"
#include "cal3d/vector.h"
#include "nrmCharacter.h"
#include <quaternion.h>
#include "cal3d/bone.h"
#include "Vector3D.h"
#include "Plane3D.h"

#define MIN_WALKABLE_Z		-5

CWalkController::CWalkController():
	MP_STRING_INIT(m_lockedBoneName),
	MP_VECTOR_INIT(m_motionIDs),
	MP_VECTOR_INIT(m_motions)
{
	m_pathPercent = m_lastPercent = 0.0f;
	m_stepLength = 124.179f;

	m_isInitialized = false;	

	m_isCollision = false;
	
	m_mainDuration = 1000.0f;

	m_lrMotionState = 0;
	m_bfMotionState = 0;
	m_rotatingState = 0;
	m_rotDirection = 0;
	m_lrDirection = 0;
	m_bfDirection = 0;
	m_lrMoveTick = 0;
	m_bfMoveTick = 0;

	m_buttonState = 0;

	m_lastPathLine = NULL;

	m_isAfterTurn = false;
	m_isStoppedOnArrived = true;
	m_isClearingNeeded = false;

	m_isMovementChanged = false;

	m_path = NULL;
	m_internalPath = NULL;
	
	m_motions.reserve(WALK_MOTIONS_COUNT);
	m_motionIDs.reserve(WALK_MOTIONS_COUNT);	
	for (int i = 0; i < WALK_MOTIONS_COUNT; i++)	
	{
		m_motions.push_back(NULL);
		m_motionIDs.push_back(-1);		
	}

	m_isBoneLocked = false;
	m_isFirstBoneLockedFrame = false;
	m_addAngle = 0;

	m_isRotatedBeforePath = false;
	m_isStopStart = false;

	m_isBoneLocked = false;
	m_isFirstBoneLockedFrame = false;
	m_lockShift = 0;

	m_character = NULL;
	m_rmCharacter = NULL;

	m_lockDelta.Set(0, 0, 0);

	m_isRotatingLeft = false;
	m_isRotatingRight = false;
	m_bImmediateMotionRemove = false;
}

void	CWalkController::SetCharacter(nrmCharacter* rmCharacter)
{
	m_rmCharacter = rmCharacter;
	if (rmCharacter)
	{
		m_character = rmCharacter->m_obj3d;
	}
	else
	{
		m_character = NULL;
	}
}

bool CWalkController::IsCharacterFrozen()
{
	return m_character->IsFrozen();
}

void CWalkController::Unfreeze()
{
	m_character->SetFreeze(false);
}

void CWalkController::Freeze()
{
	m_character->SetFreeze(true);
}

/************************************************************************/
/* Update walkable state of the character
/************************************************************************/
void	CWalkController::Update(unsigned int deltaTime)
{
	if (!m_character)
	{
		return;
	}

	if (m_character->IsFrozen())
	{
		return;
	}

	bool isCollision = (m_character->GetUserData(USER_DATA_WAS_COLLISION) != 0);

	mlMedia* rmmlObj = m_rmCharacter->GetMLMedia();
	if (rmmlObj)
	{
		if (rmmlObj->GetI3DObject())
		{
			ml3DPosition endPoint;
			endPoint.x = m_lastGoToEndPos.x;
			endPoint.y = m_lastGoToEndPos.y;
			endPoint.z = m_lastGoToEndPos.z;

			if ((m_isCollision) && (!isCollision))
			{
				if ((0 == m_bfDirection) && (0 == m_lrDirection) && (0 == m_rotDirection))
				{			
					bool isStatic = true;

					int objectID = g->o3dm.GetObjectNumber(g->phom.GetCollisionObjectName());
					if (objectID != -1)
					{
						C3DObject* obj = g->o3dm.GetObjectPointer(objectID);
						if (obj->GetAnimation())
						{
							isStatic = false;
						}

						if (obj->GetUserData(USER_DATA_EDITABLE)!= NULL)
						{
							isStatic = false;
						}
					}

					if (m_path)
					{
						if (m_path->MarkSegmentIfNeeded(m_pathPercent))
						{
							isStatic = false;
						}
					}

					rmmlObj->GetI3DObject()->onCollision(NULL, MLCF_COLLISION_END, endPoint, m_lastPathPointCount, isStatic);					
				}
			}
			else if ((!m_isCollision) && (isCollision))
			{
				rmmlObj->GetI3DObject()->onCollision(NULL, MLCF_COLLISION_BEGIN, endPoint, m_lastPathPointCount, true);				
			}
		}
	}

	if ((isCollision) || ((0 == m_bfDirection) && (0 == m_lrDirection) && (0 == m_rotDirection)))
	{
		m_isCollision = isCollision;
	}

	if (m_isStopStart)
	{
		m_stopStartTime += deltaTime;

		if (m_stopStartTime > WME_LAST_TRANSTIME)
		{
			handle_message(WME_PATHDONE);
			m_isStopStart = false;
		}
	}

	if (m_isMovementChanged)
	{
		if (m_character->GetAnimation())
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();

			if (sao)
			{
				if ((m_motions[MID_GF]) && (get_duration( MID_GF) != 0.0f))
				{
					m_mainDuration = get_duration( MID_GF);
				}

				m_isMovementChanged = false;
			}			
		}
	}

	float x, y, z;
	m_character->GetScale(&x, &y, &z);
	float	frameStepLength = deltaTime / m_mainDuration * m_stepLength * x;

	if (m_bfDirection & (WME_GO_FORWARD | WME_GO_BACKWARD))
	{
		if (g->phom.GetControlledObject())
		if (g->phom.GetControlledObject()->GetObject3D() == m_character)
		{
			g->phom.GetControlledObject()->SetForwardMoveDirection(!(m_bfDirection & WME_GO_BACKWARD));
			gRM->mapMan->ExecJS("scene.getMyAvatar().changeAvatarRotAndPosByKeys()");
		}

		update_forward_backward_moving(deltaTime, frameStepLength);
	}

	if (m_lrDirection & (WME_L_STRAFE | WME_R_STRAFE))
	{
		update_strafe_moving(deltaTime, frameStepLength);
	}

	if (m_rotDirection & (WME_POINT_TURN))
	{
		update_point_rotating(deltaTime, frameStepLength);
	}

	if (m_rotDirection & (WME_START_L_RTN | WME_START_R_RTN))
	{
		update_rotating(deltaTime);
	}

	update_locked_bones();
}

/************************************************************************/
/* New event orrured:	key pressed or key unpressed
/************************************************************************/
void	CWalkController::HandleEvent(unsigned int eventID, void* data)
{
	if (!m_character)
	{
		return;
	}

	switch(eventID) 
	{
	case WME_CHANGE_MOTIONS_START:
		change_motions_start();
		break;

	case WME_CHANGE_MOTIONS_END:
		change_motions_end();
		break;

	case WME_START_GO_LEFT:
	case WME_START_GO_RIGHT:
	case WME_START_GO_FORWARD:					// forward key pressed
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
			if (sao)
			{
				sao->LockIdles();
			}
		}
		if (m_bfDirection != WME_GO_BACKWARD)
		{
			if (WME_L_STRAFE == m_lrDirection)
			{
				if (m_specialMode != LEFT_FORWARD_MODE)
				{
					end_strafe_left();
					start_go_forward(0);					
					m_specialMode = LEFT_FORWARD_MODE;
				}
			}
			else if (WME_R_STRAFE == m_lrDirection)
			{
				if (m_specialMode != RIGHT_FORWARD_MODE)
				{
					end_strafe_right();
					start_go_forward(0);					
					m_specialMode = RIGHT_FORWARD_MODE;
				}
			}
			else
			{
				if ((eventID != WME_START_GO_LEFT) && (eventID != WME_START_GO_RIGHT))
				{
					start_go_forward(0);
				}
				else
				{
					if (WME_START_L_RTN == m_rotDirection)
					{
						end_left_rotation();
						start_go_forward(0);
					}
					else if (WME_START_R_RTN == m_rotDirection)
					{
						end_right_rotation();
						start_go_forward(0);
					}
					
				}
				m_specialMode = 0;
			}
		}

		m_buttonState |= UP_KEY_PRESSED;
		m_isRotatingLeft = (eventID == WME_START_GO_LEFT);
		m_isRotatingRight = (eventID == WME_START_GO_RIGHT);
		break;
		
	case WME_END_GO_FORWARD:					// forward key unpressed
		if (WME_GO_FORWARD == m_bfDirection)
		{
			end_go_forward();
			m_specialMode = 0;
		}
		
		m_buttonState &= ~UP_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_START_GO_BACK_LEFT:
	case WME_START_GO_BACK_RIGHT:
	case WME_START_GO_BACKWARD:					// BACKward key pressed
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
			if (sao)
			{
				sao->LockIdles();
			}
		}
		if (m_bfDirection != WME_GO_FORWARD)
		{
			if (WME_L_STRAFE == m_lrDirection)
			{
				if (m_specialMode != LEFT_BACKWARD_MODE)
				{
					end_strafe_left();
					start_go_backward(45, false);
					m_specialMode = LEFT_BACKWARD_MODE;
				}
			}
			else if (WME_R_STRAFE == m_lrDirection)
			{
				if (m_specialMode != RIGHT_BACKWARD_MODE)
				{
					end_strafe_right();
					start_go_backward(-45, false);
					m_specialMode = RIGHT_BACKWARD_MODE;
				}
			}
			else
			{
				if ((eventID != WME_START_GO_BACK_LEFT) && (eventID != WME_START_GO_BACK_RIGHT))
				{
					start_go_backward(0);
				}
				else
				{
					if (WME_START_L_RTN == m_rotDirection)
					{
						end_left_rotation();
						start_go_backward(0);
					}
					else if (WME_START_R_RTN == m_rotDirection)
					{
						end_right_rotation();
						start_go_backward(0);
					}
				}
				m_specialMode = 0;
			}
		}
		m_isRotatingLeft = (eventID == WME_START_GO_BACK_LEFT);
		m_isRotatingRight = (eventID == WME_START_GO_BACK_RIGHT);
		m_buttonState |= DOWN_KEY_PRESSED;
		break;

	case WME_END_GO_BACKWARD:					// BACKward key unpressed
		if (WME_GO_BACKWARD == m_bfDirection)
		{
			end_go_backward();
			m_specialMode = 0;
		}

		m_buttonState &= ~DOWN_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_START_L_STRAFE:					// left strafe key pressed
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
			if (sao)
			{
				sao->LockIdles();
			}
		}

		if (WME_START_L_RTN == m_rotDirection)
		{
			end_left_rotation();						
		}
		else if (WME_START_R_RTN == m_rotDirection)
		{
			end_right_rotation();						
		}

		if (m_lrDirection != WME_R_STRAFE)
		{
			start_strafe_left();
			m_specialMode = 0;			
		}
		m_buttonState |= LEFT_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_END_L_STRAFE:						// left strafe key unpressed
		if (0 == m_bfDirection)
		{
			if (WME_L_STRAFE == m_lrDirection)
			{
				end_strafe_left();
				m_specialMode = 0;
			}
		}
		m_buttonState &= ~LEFT_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_START_R_STRAFE:					// right strafe key pressed
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
			if (sao)
			{
				sao->LockIdles();
			}
		}

		if (WME_START_L_RTN == m_rotDirection)
		{
			end_left_rotation();						
		}
		else if (WME_START_R_RTN == m_rotDirection)
		{
			end_right_rotation();						
		}

		if (m_lrDirection != WME_L_STRAFE)
		{
			start_strafe_right();
			m_specialMode = 0;			
		}
		m_buttonState |= RIGHT_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_END_R_STRAFE:						// right strafe key unpressed
		if (0 == m_bfDirection)
		{
			if (WME_R_STRAFE == m_lrDirection)
			{
				end_strafe_right();
				m_specialMode = 0;
			}			
		}
		m_buttonState &= ~RIGHT_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_INITIALIZE:
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		initialize();	
		return;
		break;

	case WME_STOP:		
		HandleEvent(WME_END_GO_FORWARD, NULL);
		HandleEvent(WME_END_GO_BACKWARD, NULL);
		HandleEvent(WME_END_L_STRAFE, NULL);
		HandleEvent(WME_END_R_STRAFE, NULL);
		HandleEvent(WME_END_R_RTN, NULL);
		HandleEvent(WME_END_L_RTN, NULL);
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		m_addAngle = 0;
		m_specialMode = 0;
		m_isBoneLocked = false;
		break;

	case WME_SET_PATH:
		set_path(data);
		m_specialMode = 0;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_AFTER_TURN:
		m_isAfterTurn = true;
		m_afterTurnPoint = *(CalVector*)data;
		m_specialMode = 0;
		break;

	case WME_POINT_TURN:
		point_turn(data);
		m_specialMode = 0;
		break;

	case WME_GOTO_POINT:
		go_to_point(data);
		m_specialMode = 0;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_SET_PATHPERCENT:
		m_pathPercent = *((float*)data);
		m_lastPercent = m_pathPercent;
		break;

	case WME_START_R_RTN:
		if (WME_START_R_RTN == m_rotatingState)	
		{
			break;
		}

		if (WME_L_STRAFE == m_lrDirection)
		{
			end_strafe_left();
			m_lrDirection = 0;
		}
		else if (WME_R_STRAFE == m_lrDirection)
		{
			end_strafe_right();
			m_lrDirection = 0;
		}

		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
			if (sao)
			{
				sao->LockIdles();
			}
		}

		if (m_isRotatingRight)
		{
			if (WME_GO_FORWARD == m_bfDirection)
			{
				end_go_forward();
			}
			else if (WME_GO_BACKWARD == m_bfDirection)
			{
				end_go_backward();
			}
		}
		

		if ((!(m_bfMotionState & (WME_GO_FORWARD | WME_GO_BACKWARD))) 
			&& (!(m_bfMotionState & (WME_L_STRAFE | WME_R_STRAFE)))) 
		{
			start_right_rotation();
			m_specialMode = 0;
		}
		m_buttonState |= MORE_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_START_L_RTN:
		if (WME_START_L_RTN == m_rotatingState)	
		{
			break;
		}

		if (WME_L_STRAFE == m_lrDirection)
		{
			end_strafe_left();
			m_lrDirection = 0;
		}
		else if (WME_R_STRAFE == m_lrDirection)
		{
			end_strafe_right();
			m_lrDirection = 0;
		}

		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
			if (sao)
			{
				sao->LockIdles();
			}
		}

		if (m_isRotatingLeft)
		{
			if (WME_GO_FORWARD == m_bfDirection)
			{
				end_go_forward();
			}
			else if (WME_GO_BACKWARD == m_bfDirection)
			{
				end_go_backward();
			}
		}

		if ((!(m_bfMotionState & (WME_GO_FORWARD | WME_GO_BACKWARD))) &&
			(!(m_lrMotionState & (WME_L_STRAFE | WME_R_STRAFE))))
		{
			start_left_rotation();
			m_specialMode = 0;
		}
		m_buttonState |= LESS_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_END_R_RTN:
		if (WME_START_R_RTN == m_rotDirection)
		{
			end_right_rotation();
			m_specialMode = 0;
		}
	
		m_buttonState &= ~MORE_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;

	case WME_END_L_RTN:
		if (WME_START_L_RTN == m_rotDirection)
		{
			end_left_rotation();
			m_specialMode = 0;
		}

		m_buttonState &= ~LESS_KEY_PRESSED;
		m_isRotatingLeft = false;
		m_isRotatingRight = false;
		break;
	}

}

float	CWalkController::get_duration(int id)
{
	if (m_motionIDs.size() <= (unsigned int)id)
	{
		return 0.0f;
	}

	return (-1 == m_motionIDs[id]) ? 0.0f : m_motions[id]->GetDuration();
}

#ifdef NEW_PATH_METHOD
CPathLine* CWalkController::get_path()
{ 
	return m_path;
}
#else
CBSpline* CWalkController::get_path()
{ 
	return m_path;
}
#endif

void CWalkController::set_step_length(float sl)
{ 
	m_stepLength = sl;
}	

void CWalkController::set_rotation_speed(float rot)
{ 
	m_rotSpeed = rot;
}	

void CWalkController::add_motion(WALK_MOTIONS_IDs id, int mID)
{
	if (m_motions[id]!=NULL) 
	{
		clear_cycle(m_motionIDs[id], 0);
	}
	m_isMovementChanged = true;
	m_motionIDs[id] = mID;
	m_motions[id] = g->motm.GetObjectPointer(mID);
    if (m_motions[id]==NULL) return;
	if (MID_GF == id)
	{
		if ((m_motions[id]->GetCoreAnimation()!=NULL) &&( m_motions[id]->GetCoreAnimation()->getFlags() & CAL3D_COREANIM_KEEPFRAME))
		{
			float* pdata;
			m_motions[id]->GetCoreAnimation()->getFlagData(CAL3D_COREANIM_KEEPFRAME, (LPVOID*)&pdata);
			m_stepLength = *pdata;
		}
	}

	clear_cycle(m_motionIDs[id], 0);
}

void CWalkController::initialize()
{
	if( m_isInitialized)
	{
		return;
	}

	if (!m_character)
	{
		return;
	}

	m_isInitialized = true;

	if (m_motionIDs[MID_GF] != -1)
	{
		m_mainDuration = 1000.0f * get_duration(m_motionIDs[MID_GF]);
	}
}

void	CWalkController::handle_message(DWORD message)
{
	if (message == WME_PATHDONE)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
		if (sao)
		{
			sao->UnlockIdles();
		}

		mlMedia* pmlMedia = m_rmCharacter->GetMLMedia();
		if (pmlMedia)
		{
			nrmCharacter* character = (nrmCharacter *)pmlMedia->GetSafeMO()->GetICharacter();

			if (character)
			{
				character->onPointArrived();
			}
		}		
	}

}

void	CWalkController::clear_old_actions()
{
	if (m_isClearingNeeded)
	{
		if (m_bfMoveTick - m_bfLastMoveTick >= WME_LAST_TRANSTIME)
		{
			clear_action(m_motionIDs[MID_ENGF], 0);
			clear_action(m_motionIDs[MID_ENGB], 0);
			clear_action(m_motionIDs[MID_LRT], 0);
			clear_action(m_motionIDs[MID_RRT], 0);

			clear_cycle(m_motionIDs[MID_GF], 0);
			clear_cycle(m_motionIDs[MID_GB], 0);
			
			m_isClearingNeeded = false;
		}
	}
}

void	CWalkController::change_motions_start()
{
}

void	CWalkController::ClearAllActions()
{
	clear_cycle(m_motionIDs[MID_GF], 0);
	clear_cycle(m_motionIDs[MID_GB], 0);
	clear_cycle(m_motionIDs[MID_LRT], 0);
	clear_cycle(m_motionIDs[MID_RRT], 0);
}

void	CWalkController::change_motions_end()
{
	if (WME_GO_FORWARD == m_bfDirection)
	{
		blend_cycle(m_motionIDs[MID_GF], 1, 0);
	}
}

//
// Действия аватара
//

void	CWalkController::rotate_avatar(int addAngle, bool isSaveAngle)
{
	if (addAngle != 0)
	{
		CQuaternion rot = m_character->GetRotation()->GetAsDXQuaternion();
		rot *= CQuaternion(CVector3D(0, 0, 1), (float)addAngle);
		
		if (m_character->GetRotation())
		{
			m_characterRotation.SetAsDXQuaternion(rot.x, rot.y, rot.z, rot.w);
			m_character->SetRotation(&m_characterRotation);
		}
	}

	if (isSaveAngle)
	{
		m_addAngle = addAngle;
	}
}

void	CWalkController::start_go_forward(int addAngle, bool isSaveAngle)
{
	if (g->phom.GetControlledObject())
	if (g->phom.GetControlledObject()->GetObject3D() == m_character)
	{
		g->phom.GetControlledObject()->OnStartMoving();
	}

	clear_old_actions();

	m_bfDirection = WME_GO_FORWARD;
	m_bfMoveTick = 0;

	if ((m_path) && (!m_isRotatedBeforePath))
	{
		return;
	}

	m_bfMotionState = WME_START_GO_FORWARD;

	set_action/*_with_locked_bones*/(m_motionIDs[MID_GF], (unsigned int)WME_FIRST_TRANSTIME/*, "r foot"*/);
		
	rotate_avatar(addAngle, isSaveAngle);
}

void	CWalkController::end_go_forward()
{
	m_bfLastMoveTick = m_bfMoveTick;
	if (WME_START_GO_FORWARD == m_bfMotionState)
	{
		clear_action(m_motionIDs[MID_GF], 0);
		set_action_with_locked_bones(m_motionIDs[MID_ENGF], (unsigned int)WME_FIRST_TRANSTIME, "l foot");
	}
	else
	{
		clear_cycle(m_motionIDs[MID_GF], WME_LAST_TRANSTIME / 1000.0f);

		if (fmodf((m_bfMoveTick - WME_FIRST_TRANSTIME) / m_mainDuration, 1.0f) > 0.5f)
		{
			set_action_with_locked_bones(m_motionIDs[MID_ENGF], (unsigned int)WME_FIRST_TRANSTIME, "r foot");
		}
		else
		{
			set_action_with_locked_bones(m_motionIDs[MID_ENGF], (unsigned int)WME_FIRST_TRANSTIME, "l foot");
		}
	}

	m_bfDirection = 0;
		
	m_bfMotionState = 0;
	m_path = NULL;
	m_bfMoveTick = 0;

	m_addAngle = 0;
}

void	CWalkController::start_go_backward(int addAngle, bool isSaveAngle)
{
	clear_old_actions();

	m_bfDirection = WME_GO_BACKWARD;
	m_bfMotionState = WME_START_GO_FORWARD;
	m_bfMoveTick = 0;

	set_action/*_with_locked_bones*/(m_motionIDs[MID_GB], (unsigned int)WME_FIRST_TRANSTIME/*, "r foot"*/);
	
	rotate_avatar(addAngle, isSaveAngle);
}

void	CWalkController::end_go_backward()
{
	m_bfLastMoveTick = m_bfMoveTick;
	if (WME_START_GO_FORWARD == m_bfMotionState)
	{
		clear_action(m_motionIDs[MID_GB], 0);
		m_character->Move(-m_lockDelta.x, -m_lockDelta.y, -m_lockDelta.z);
		//set_action_with_locked_bones(m_motionIDs[MID_ENGF], (unsigned int)WME_FIRST_TRANSTIME, "l foot");
	} 
	else
	{	
		clear_cycle(m_motionIDs[MID_GB], WME_LAST_TRANSTIME / 1000.0f);

		if (fmodf((m_bfMoveTick - WME_FIRST_TRANSTIME) / m_mainDuration, 1.0f) > 0.5f)
		{
			set_action_with_locked_bones(m_motionIDs[MID_ENGF], (unsigned int)WME_FIRST_TRANSTIME, "r foot");
		}
		else
		{
			set_action_with_locked_bones(m_motionIDs[MID_ENGF], (unsigned int)WME_FIRST_TRANSTIME, "l foot");
		}
	}

	m_bfDirection = 0;
		
	m_bfMotionState = 0;
	m_path = NULL;
	m_bfMoveTick = 0;

	m_addAngle = 0;
}

void	CWalkController::start_strafe_left()
{
	clear_old_actions();

	m_lrDirection = WME_L_STRAFE;
	blend_cycle(m_motionIDs[MID_SL], 1, WME_FIRST_TRANSTIME / 1000.0f);
	m_lrMotionState = WME_START_GO_FORWARD;
	m_lrMoveTick = 0;
}

void	CWalkController::end_strafe_left()
{
	blend_cycle(m_motionIDs[MID_SL], 0, WME_LAST_TRANSTIME / 1000.0f);
	execute_action(m_motionIDs[MID_ENGF], WME_LAST_TRANSTIME / 1000.0f, 0);
	//blend_cycle(m_motionIDs[MID_ENGF], 1, WME_LAST_TRANSTIME / 1000.0f);
	m_lrLastMoveTick = m_lrMoveTick;
	m_lrMotionState = WME_END_GO_FORWARD;
}

void	CWalkController::start_strafe_right()
{
	clear_old_actions();

	m_lrDirection = WME_R_STRAFE;
	blend_cycle(m_motionIDs[MID_SR], 1, WME_FIRST_TRANSTIME / 1000.0f);
	m_lrMotionState = WME_START_GO_FORWARD;
	m_lrMoveTick = 0;
}

void	CWalkController::end_strafe_right()
{
	blend_cycle(m_motionIDs[MID_SR], 0, WME_LAST_TRANSTIME / 1000.0f);
	//blend_cycle(m_motionIDs[MID_ENGF], 1, WME_LAST_TRANSTIME / 1000.0f);
	execute_action(m_motionIDs[MID_ENGF], WME_LAST_TRANSTIME / 1000.0f, 0);
	m_lrLastMoveTick = m_lrMoveTick;
	m_lrMotionState = WME_END_GO_FORWARD;
}

void	CWalkController::set_path(void* data)
{
	HandleEvent(WME_STOP, NULL);

	if (data)
	{
#ifdef NEW_PATH_METHOD
 		m_path = (CPathLine*)data;
#else
		m_path = (CBSpline*)data;
#endif
	}
	else
	{
		m_isStoppedOnArrived = false;
	}

	m_isAfterTurn = false;
};

void	CWalkController::point_turn(void* data)
{
	bool tmp = m_isAfterTurn;
	HandleEvent(WME_SET_PATH, NULL);
	m_isAfterTurn = tmp;

	m_turnPoint = *(CalVector*)data;

	m_lrMotionState = 0;
	m_bfMotionState = 0;

	m_rotDirection = WME_POINT_TURN;
}

void	CWalkController::SetLastPathLine(CPathLine* pathLine)
{
	if (m_lastPathLine)
	{
		MP_DELETE(m_lastPathLine);
		m_lastPathLine = NULL;
	}

	m_lastPathLine = pathLine;
}

void	CWalkController::GoPath(CVector3D* path, int pointCount)
{
	if (pointCount < 2)
	{
		return;
	}

	if (!m_character)
	{
		return;
	}

	m_isStopStart = false;

	m_lastPathPointCount = pointCount;

	if (m_lastGoToEndPos != path[pointCount - 1])
	{
		m_lastGoToStartPos = path[0];
		m_lastGoToEndPos = path[pointCount - 1];
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
	if (sao)
	{
		sao->LockIdles();
	}

	m_isRotatedBeforePath = false;
	if (m_internalPath)
	{
		MP_DELETE(m_internalPath);
	}

	m_internalPath = MP_NEW(CPathLine);

	bool isDefineSegments = true;

	if ((!m_lastPathLine) || (m_lastPathLine->GetPointCount() != pointCount))
	{
		isDefineSegments = false;
	}

	for (int i = 0; i < pointCount; i++)
	{
		if (isDefineSegments)
		{
			m_internalPath->AddPoint(path[i], m_lastPathLine->GetFirstSegment(i), m_lastPathLine->GetSecondSegment(i));
		}
		else
		{
			m_internalPath->AddPoint(path[i], NULL, NULL);
		}		
	}

	if (m_bfDirection & WME_GO_FORWARD)
	{
		m_path = m_internalPath;
		if (m_pathPercent != 0.0f)
		{
			m_isRotatedBeforePath = true;
		}
		m_bfMotionState = WME_START_GO_FORWARD;
		clear_action(m_motionIDs[MID_ENGF], 0);
	}
	else
	{
		HandleEvent(WME_SET_PATH, (LPVOID)m_internalPath);
		HandleEvent(WME_START_GO_FORWARD, NULL);
	}

	m_pathPercent = m_lastPercent = 0.0;
}

void	CWalkController::go_to_point(void* data)
{
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
	if (sao)
	{
		sao->LockIdles();
	}

	m_isRotatedBeforePath = false;
	if (m_internalPath)
	{
		MP_DELETE(m_internalPath);
	}

#ifdef NEW_PATH_METHOD
	m_internalPath = MP_NEW(CPathLine);
#else
	m_internalPath = MP_NEW(CBSpline);
#endif
	CalVector vDestination = *(CalVector*)data;

	if (vDestination.z < MIN_WALKABLE_Z)
	{
		return;
	}

	CalVector p1;
	m_character->GetCoords(&p1.x, &p1.y, &p1.z);
	CalVector vdir = vDestination - p1;
	vdir.z = p1.z;

	if (m_bfMotionState & (WME_GO_FORWARD | WME_START_GO_FORWARD))
	{
		CQuaternion rot = m_character->GetRotation()->GetAsDXQuaternion();

		float dirlen = vdir.length();
		vdir = CalVector(1, 0, 0);
		vdir *= CalQuaternion(rot.x, rot.y, rot.z, rot.w);
		vdir *= (dirlen > 100 ? 100 : dirlen);
	}

	vdir.normalize();

#ifdef NEW_PATH_METHOD

	if (m_lastGoToEndPos != CVector3D(vDestination.x, vDestination.y, vDestination.z))
	{
		m_lastGoToStartPos.Set(p1.x, p1.y, p1.z);
		m_lastGoToEndPos.Set(vDestination.x, vDestination.y, vDestination.z);	
	}

	m_internalPath->AddPoint(m_lastGoToStartPos, NULL, NULL);
	m_internalPath->AddPoint(m_lastGoToEndPos, NULL, NULL);

 	//m_foundPath.FindPath(m_internalPath , CVector3D(-84213.195 , 44519.500 , 13.472458), CVector3D/*(-84530.281, 44631.102, 9.3050880));*/((-84530.281-84213.195)/2, (44631.102+44519.500 )/2, 9.3050880));

	//g->pf.FindPath(m_internalPath , CVector3D(p1.x, p1.y, p1.z), CVector3D(vDestination.x, vDestination.y, vDestination.z));

	//m_internalPath->AddPoint(CVector3D(p1.x, p1.y, p1.z));
	//CVector3D c = CVector3D(p1.x, p1.y, p1.z) + CVector3D(vDestination.x, vDestination.y, vDestination.z);
	//c /= 2;
	//CVector3D z = c;
	//float r = sqrtf((c.x - p1.x) * (c.x - p1.x) + (c.y - p1.y) * (c.y - p1.y) + (c.z - p1.z) * (c.z - p1.z));
	//c.Normalize();
	//c *= r;
	//c.RotateZ(90);
	//c.x +=z.x;
	//c.y +=z.y;
	//c.z +=z.z;


	//m_internalPath->AddPoint(c);
	//m_internalPath->AddPoint(CVector3D(vDestination.x, vDestination.y, vDestination.z));
#else
	m_internalPath->AddPoint(CBSCPoint(CVector3D(p1.x, p1.y, p1.z), CVector3D(p1.x, p1.y, p1.z), 
		CVector3D(p1.x + vdir.x, p1.y + vdir.y, p1.z + vdir.z)));
	m_internalPath->AddPoint(CBSCPoint(CVector3D(vDestination.x, vDestination.y, vDestination.z), 
		CVector3D(vDestination.x, vDestination.y, vDestination.z), CVector3D(vDestination.x, vDestination.y, vDestination.z)));
#endif

	if (m_bfDirection & WME_GO_FORWARD)
	{
		m_path = m_internalPath;
		if (m_pathPercent != 0.0f)
		{
			m_isRotatedBeforePath = true;
		}
		m_bfMotionState = WME_START_GO_FORWARD;
		clear_action(m_motionIDs[MID_ENGF], 0);
	}
	else
	{
		HandleEvent(WME_SET_PATH, (LPVOID)m_internalPath);
		HandleEvent(WME_START_GO_FORWARD, NULL);
	}

	m_pathPercent = m_lastPercent = 0.0;
	m_lastPathPointCount = 2;
}

void	CWalkController::start_left_rotation()
{
	clear_old_actions();

	blend_cycle(m_motionIDs[MID_LRT], 1, WME_FIRST_TRANSTIME / 1000.0f);

	m_rotDirection = WME_START_L_RTN;
}

void	CWalkController::end_left_rotation()
{
	m_isClearingNeeded = true;
	m_bfMoveTick = (unsigned int)WME_LAST_TRANSTIME + 1;
	m_bfLastMoveTick = 0;

    clear_cycle(m_motionIDs[MID_LRT], WME_FIRST_TRANSTIME / 1000.0f);
	execute_action(m_motionIDs[MID_ENGF], WME_LAST_TRANSTIME / 1000.0f, 0);

	m_rotDirection = 0;
}

void	CWalkController::start_right_rotation()
{
	clear_old_actions();

	blend_cycle(m_motionIDs[MID_RRT], 1, WME_LAST_TRANSTIME / 1000.0f);

	m_rotDirection = WME_START_R_RTN;
}

void	CWalkController::end_right_rotation()
{
	m_isClearingNeeded = true;
	m_bfMoveTick = (unsigned int)WME_LAST_TRANSTIME + 1;
	m_bfLastMoveTick = 0;

	clear_cycle(m_motionIDs[MID_RRT], WME_LAST_TRANSTIME / 1000.0f);
	execute_action(m_motionIDs[MID_ENGF], WME_LAST_TRANSTIME / 1000.0f, 0);

	m_rotDirection = 0;
}

void	CWalkController::set_action_with_locked_bones(int cal3dID, DWORD time, std::string bone)
{
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();

	if (!sao)
	{
		return;
	}
    
	if (!sao->GetCalModel())
	{
		return;
	}

	if (!sao->GetCalModel()->getSkeleton())
	{
		return;
	}

	set_action(cal3dID, (float)time);

	CalBone* pBone = NULL;
	pBone = sao->GetCalModel()->getSkeleton()->getBone(CComString(bone.c_str()));
//	assert(pBone);
	if ( pBone == NULL )
	{
		return;
	}
	pBone->calculateState();
	m_lockedBoneTranslation = pBone->getTranslationAbsolute();

	m_lockedBoneName = bone;
	m_isBoneLocked = true;
	m_isFirstBoneLockedFrame = true;
	m_lockShift = 0;
}

void	CWalkController::remove_action_with_locked_bones(int /*cal3dID*/)
{
	m_lockedBoneName = "";

	m_isBoneLocked = false;
}

//
// UPDATE в различных ситуациях
//

void	CWalkController::update_locked_bones()
{
	if (m_isBoneLocked)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
		
		if ((!sao) || (!sao->GetCalModel()) || (!sao->GetCalModel()->getSkeleton()))
		{
			return;
		}

		CalBone*	pBone = sao->GetCalModel()->getSkeleton()->getBone(CComString(m_lockedBoneName.c_str()));

		if (!pBone)
		{
			return;
		}

		CalVector newPosition = pBone->getTranslationAbsolute();
		
		if (m_isFirstBoneLockedFrame)
		{
			m_lockedBoneTranslation = newPosition;
			m_isFirstBoneLockedFrame = false;
		}

		m_lockShift += (m_lockedBoneTranslation - newPosition).length() * 0.98f;

		CalVector moveDeltaVector = m_lockedBoneTranslation - newPosition;
		CQuaternion q = m_character->GetRotation()->GetAsDXQuaternion();
		moveDeltaVector *= CalQuaternion(q.x,q.y,q.z,q.w);
		m_character->Move(moveDeltaVector.x, moveDeltaVector.y, 0/*moveDeltaVector.z*/);
		m_lockDelta.x = moveDeltaVector.x;
		m_lockDelta.y = moveDeltaVector.y;
		m_lockDelta.z = 0;

		m_lockedBoneTranslation = newPosition;
	}
}

void	CWalkController::update_point_rotating(DWORD dwTime, float /*frameStepLength*/)
{
	CQuaternion charRtn = m_character->GetRotation()->GetAsDXQuaternion();
	
	CVector3D  charNormal(0, 0, 1);	
	charNormal *= charRtn;
	CVector3D  charVector(1, 0, 0);	
	charVector *= charRtn;

	CVector3D avatarPos;
	m_character->GetCoords(&avatarPos.x, &avatarPos.y, &avatarPos.z);

	CPlane3D charPlane(charNormal, avatarPos);

	CVector3D proj = charPlane.GetProjection(CVector3D(m_turnPoint.x, m_turnPoint.y, m_turnPoint.z));

	CVector3D m_turnVector = proj - avatarPos;
	float	  turnVectorLength = m_turnVector.GetLength();
	m_turnVector.Normalize();

	if (turnVectorLength < MIN_TURN_VECTOR_LENGTH)
	{
		m_rotDirection &= ~WME_POINT_TURN;

		if (m_isAfterTurn)
		{
			m_isAfterTurn = false;
			handle_message(WME_PATHDONE);
		}
		else
		{
			handle_message(WME_AFTER_TURN);
		}
		return;
	}

	float	   turnAngle = charVector.GetAngle(m_turnVector);
	CVector3D  turnNormal = charVector.GetNormal(m_turnVector);
	float	   deltaAngle = (float)(PI * dwTime / 1000.0f);
	bool	   isRotationNeeded = false;

	if ((turnAngle > 0) && (turnAngle > deltaAngle))
	{
		turnAngle = deltaAngle;
	}
	else if ((turnAngle < 0) && (turnAngle < -deltaAngle))
	{
		turnAngle = -deltaAngle;
	}
	else
	{
		m_rotDirection &= ~WME_POINT_TURN;

		if (m_rotatingState & WME_START_R_RTN)
		{
			end_right_rotation();
		}
		else if (m_rotatingState & WME_START_L_RTN)
		{
			end_left_rotation();
		}


		isRotationNeeded = true;

		if (m_isAfterTurn)
		{
			m_isAfterTurn = false;
			handle_message(WME_PATHDONE);
		}
		else
		{
			handle_message(WME_AFTER_TURN);
		}
	}

	if (turnNormal.Dot(charNormal) < 0)
	{
		CQuaternion rtn = m_character->GetRotation()->GetAsDXQuaternion();

		CQuaternion yaw(CVector3D(0, 0, 1), turnAngle);
		yaw *= rtn;

		if (m_character->GetRotation())
		{
			m_characterRotation.SetAsDXQuaternion(yaw.x, yaw.y, yaw.z, yaw.w);
			m_character->SetRotation(&m_characterRotation);
		}

		if ((isRotationNeeded) && (!(m_rotatingState & WME_START_R_RTN)))
		if (m_rotDirection & WME_POINT_TURN)
		{
			start_right_rotation();
		}
	}
	else
	{
		CQuaternion rtn = m_character->GetRotation()->GetAsDXQuaternion();
		CQuaternion yaw(CVector3D(0, 0, 1), -turnAngle);
		yaw *= rtn;

		if (m_character->GetRotation())
		{
			m_characterRotation.SetAsDXQuaternion(yaw.x, yaw.y, yaw.z, yaw.w);
			m_character->SetRotation(&m_characterRotation);
		}

		if ((isRotationNeeded) && (!(m_rotatingState & WME_START_L_RTN)))
		if (m_rotDirection & WME_POINT_TURN)
		{
			start_left_rotation();
		}
	}
}

void	CWalkController::update_spline_moving(DWORD /*dwTime*/, float /*frameStepLength*/, float moveLength, bool& removePathOnThisStep)
{
	float remPathLength = m_path->GetLength() * (1.0f - m_pathPercent);
	float minStepLength = m_stepLength / 15.0f;

	if (m_bfMotionState & (WME_GO_FORWARD | WME_START_GO_FORWARD) && (remPathLength <= minStepLength))
	{
		// аватар дошел до конца пути
		HandleEvent(WME_END_GO_FORWARD, NULL);
		removePathOnThisStep = true;
		m_isStoppedOnArrived = true;
		m_isBoneLocked = false;
	}
	else
	{
		CQuaternion charRtn = m_character->GetRotation()->GetAsDXQuaternion();

		CVector3D _tangent = m_path->GetTangent(m_pathPercent);
		CVector3D pathTangent(_tangent.x, _tangent.y, _tangent.z);	
		pathTangent.z = 0;
		CVector3D charVector(1, 0, 0);	
		charVector *= charRtn;	
		charVector.z = 0;
		CVector3D tangentNormal = pathTangent.GetNormal(charVector);
		float tangentAngle = pathTangent.GetAngle(charVector);

		if (!((tangentNormal.x != 0) || (tangentNormal.y != 0) || (tangentNormal.z != 0)))
		{
			if (fabs(pathTangent.Dot(charVector)) > 0.99)
			{
				tangentNormal.Set(0,0,1);
				tangentAngle = (float)PI;
			}
		}

		if ((CVector3D(0, 0, 0) == charVector) || (CVector3D(0, 0, 0) == pathTangent))
		{
			HandleEvent(WME_SET_PATH, NULL);
			return;
		}

		
		if (!m_isRotatedBeforePath)
		{
			m_isRotatedBeforePath = true;
			start_go_forward(0);
		}
		m_lastPercent = m_pathPercent;
		m_pathPercent += moveLength / m_path->GetLength();

		if (m_lockShift != 0)
		{
			m_pathPercent += m_lockShift / m_path->GetLength();
			m_lockShift = 0;
		}

		if (m_pathPercent > 1)
		{
			m_pathPercent = 1;
			removePathOnThisStep = true;
		}
			
		CVector3D _pos = m_path->GetLinearPosition(m_pathPercent); 
		CalVector pos(_pos.x, _pos.y, _pos.z);
		CalVector cp;
		m_character->GetCoords(&cp.x, &cp.y, &cp.z);
		pos.z = cp.z;
		cp = pos - cp;
		moveLength = cp.length();	
		cp.normalize();
		cp.z = 0;	

		CalVector charPos;
		m_character->GetCoords(&charPos.x, &charPos.y, &charPos.z);
		charPos += cp * moveLength;
		CalVector delta = cp * moveLength;
		m_character->Move(delta.x, delta.y, 0);		

		cp = pos - charPos;
		cp.z = 0;
		//float remLength = cp.length();

		bool isCollision = (m_character->GetUserData(USER_DATA_WAS_COLLISION) != 0);

		if (isCollision)	
		{
			removePathOnThisStep = true;
		}
					
		CVector3D _pt = m_path->GetTangent(m_pathPercent);
		if (_pt.x == 1.0f)
		{
			_pt.x = 0.95f;
			_pt.y = 0.1f;
			_pt.Normalize();
		}
		_pt.z = 0;
		CVector3D vec(1, 0, 0);
		CQuaternion rot = CQuaternion(vec, _pt);

		vec *= rot;
		_pt.Normalize();
		vec.Normalize();
		if (_pt.Dot(vec) < 0)
		{			
			rot *= CQuaternion(CVector3D(0, 0, 1), 3.14f);
		}

		if (m_character->GetRotation())
		{
			m_characterRotation.SetAsDXQuaternion(rot.x, rot.y, rot.z, rot.w);
			m_character->SetRotation(&m_characterRotation);
		}
	}
}

void	CWalkController::update_strafe_moving(DWORD dwTime, float frameStepLength)
{
	m_lrMoveTick += dwTime;

	if ((m_lrMoveTick > WME_FIRST_TRANSTIME) && (WME_START_GO_FORWARD == m_lrMotionState))
	{
		int num = (m_lrDirection & WME_L_STRAFE) ? MID_SL : MID_SR;

		m_lrMotionState = WME_GO_FORWARD;
		blend_cycle(m_motionIDs[num], 1, 0);
	}

	if ((WME_END_GO_FORWARD == m_lrMotionState) && (m_lrMoveTick - m_lrLastMoveTick > WME_LAST_TRANSTIME))
	{	
		clear_cycle(m_motionIDs[MID_ENGF], 0);
		m_lrDirection = 0;

		if (m_buttonState & LEFT_KEY_PRESSED)
		{
			HandleEvent(WME_START_L_STRAFE, (LPVOID)1);
		}

		if (m_buttonState & RIGHT_KEY_PRESSED)
		{
			HandleEvent(WME_START_R_STRAFE, (LPVOID)1);
		}
	}

	float moveLength = frameStepLength;

	CVector3D moveDirection = (m_lrDirection & WME_L_STRAFE) ? CVector3D(0, 1, 0) : CVector3D(0, -1, 0);

	CQuaternion rot = m_character->GetRotation()->GetAsDXQuaternion();
	moveDirection *= rot;

	CVector3D delta = moveDirection * moveLength;
	m_character->Move(delta.x, delta.y, delta.z);
}

void	CWalkController::update_forward_backward_moving(DWORD dwTime, float frameStepLength)
{
	if ((m_isRotatingLeft) || (m_isRotatingRight))
	{
		float angle = (float)(PI * dwTime / 1000.0f / 3.0f);
		if (m_isRotatingLeft)
		{
			angle *= -1.0f;
		}
		CQuaternion q(CVector3D(0, 0, 1), angle);
		CQuaternion rot = m_character->GetLocalRotation()->GetAsDXQuaternion();
		rot *= q;
		m_character->GetLocalRotation()->SetAsDXQuaternion(rot.x, rot.y, rot.z, rot.w);
	}

	m_bfMoveTick += dwTime;
	bool removePathOnThisStep = false;
	float moveLength = 0;
	CVector3D moveDirection = (m_bfDirection & WME_GO_FORWARD) ? CVector3D(1, 0, 0) : CVector3D(-1, 0, 0);

	CQuaternion rot = m_character->GetRotation()->GetAsDXQuaternion();
	moveDirection *= rot;

	int startMotionID = MID_STGF;
	int cycleMotionID = MID_GF;
	int endMotionID = MID_ENGF;

	if (m_bfDirection & (WME_GO_BACKWARD))
	{
		startMotionID = MID_STGB;
		cycleMotionID = MID_GB;
		endMotionID = MID_ENGB;
	}

	switch (m_bfMotionState)
	{
	case WME_START_GO_FORWARD:
		if( m_bfMoveTick > WME_FIRST_TRANSTIME)
		{
			m_bfMotionState = WME_GO_FORWARD;

			remove_action_with_locked_bones(m_motionIDs[cycleMotionID]);
			blend_cycle(m_motionIDs[cycleMotionID], 1, 0);
		}
		break;

	case WME_GO_FORWARD:
		moveLength = frameStepLength;
		break;

	case WME_END_GO_FORWARD:
		if (m_bfMoveTick - m_bfLastMoveTick > WME_LAST_TRANSTIME)
		{
			removePathOnThisStep = true;
			moveLength = frameStepLength;
			blend_cycle(m_motionIDs[MID_ENGF], 0, 0);
			handle_message(WME_PATHDONE);
		}
		else
		{
			moveLength = frameStepLength;
		}
		break;
	}

	/*if (moveLength > m_stepLength)
	{
		moveLength = m_stepLength;
	}*/

	if ((moveLength != 0) || ((m_path) && (!m_isRotatedBeforePath)))
	{
		if (!m_path)
		{
			CVector3D delta = moveDirection * moveLength;
			m_character->Move(delta.x, delta.y, delta.z);
		}
		else
		{
			update_spline_moving(dwTime, frameStepLength, moveLength, removePathOnThisStep);

			if (removePathOnThisStep)
			{
				m_isClearingNeeded = true;
				m_isStopStart = true;
				m_stopStartTime = 0;

				if (m_bfMotionState != WME_END_GO_FORWARD)
				{
					HandleEvent(WME_END_GO_FORWARD, NULL);
					m_isStoppedOnArrived = true;
				}
			}
		}
	}

	if (removePathOnThisStep)
	{
		m_bfMoveTick = 0;
		m_bfMotionState = 0;
		m_path = NULL;

		if (m_isStoppedOnArrived)
		{
			m_pathPercent = 1.0;
		}

		m_bfDirection = 0;

		if (m_isAfterTurn)
		{
			HandleEvent(WME_POINT_TURN, &m_afterTurnPoint);
		}
		else
		{
			//handle_message(WME_PATHDONE);
		}
	}
}

void	CWalkController::update_rotating(DWORD dwTime)
{
	float moveLength = (float)(PI * dwTime / 1000.0f / 3.0f);

	if (m_rotDirection & WME_START_L_RTN)
	{
		CQuaternion rot = m_character->GetRotation()->GetAsDXQuaternion();

		rot *= CQuaternion(CVector3D(0, 0, 1), -moveLength);
		
		if (m_character->GetRotation())
		{
			m_characterRotation.SetAsDXQuaternion(rot.x, rot.y, rot.z, rot.w);
			m_character->SetRotation(&m_characterRotation);
		}
	}
	else if (m_rotDirection & WME_START_R_RTN)
	{
		CQuaternion rot = m_character->GetRotation()->GetAsDXQuaternion();

		rot *= CQuaternion(CVector3D(0, 0, 1), moveLength);
		
		if (m_character->GetRotation())
		{
            m_characterRotation.SetAsDXQuaternion(rot.x, rot.y, rot.z, rot.w);
			m_character->SetRotation(&m_characterRotation);
		}
	}
}

void	CWalkController::clear_action(int id, float time)
{
	if( m_bImmediateMotionRemove)
		time = 0;
	if (m_character->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();

		if (sao)
		{
			sao->ClearAction(id, time);
		}
	}
}

void	CWalkController::clear_cycle( int id, float time)
{
	if( m_bImmediateMotionRemove)
		time = 0;
	if (m_character->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();

		if (sao)
		{
			sao->ClearCycle(id, time);
		}
	}
}

bool	CWalkController::blend_cycle(int id, float weight, float delay, bool asynch)
{
	if (m_character->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();
		if (sao)
		{
			sao->BlendCycle(id, weight, delay, asynch);
		}

		return true;
	}
	else
	{
		return false;
	}
}

void	CWalkController::execute_action(int id, float delayIn, float delayOut)
{
	if (m_character->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();

		if (sao)
		{
			//CMotion* motion = g->motm.GetObjectPointer(m_motionIDs[id]);
			sao->ExecuteAction(id, delayIn, delayOut);
		}
	}
}

void	CWalkController::set_action( int id, float time)
{
	if (m_character->GetAnimation())
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_character->GetAnimation();

		if (sao)
		{
			sao->SetAction(id, time);
		}
	}
}

void	CWalkController::EnableImmediateMotionRemoveMode( bool abEnable)
{
	m_bImmediateMotionRemove = abEnable;
}

void	CWalkController::GetLastGoToCoords(float& x1, float& y1, float& z1, float& x2, float& y2, float& z2)
{
	x1 = m_lastGoToStartPos.x;
	y1 = m_lastGoToStartPos.y;
	z1 = m_lastGoToStartPos.z;
	x2 = m_lastGoToEndPos.x;
	y2 = m_lastGoToEndPos.y;
	z2 = m_lastGoToEndPos.z;
}

CWalkController::~CWalkController()
{
	if (m_internalPath)
	{
		MP_DELETE(m_internalPath);
	}
}