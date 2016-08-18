#include "StdAfx.h"
#include "..\include\Voip3DSoundCalcManager.h"

#define TIME_OUT	200
#define SHIFT_POS	40
#define SHIFT_ROT	0.05

CVoip3DSoundCalcManager::CVoip3DSoundCalcManager()
{
	m_dwStartTime = ::GetTickCount();
	m_dwTime = m_dwStartTime - TIME_OUT;
}

CVoip3DSoundCalcManager::~CVoip3DSoundCalcManager()
{
}

//////////////////////////////////////////////////////////////////////////

BOOL CVoip3DSoundCalcManager::IsPosChanged( ml3DPosition pos, ml3DPosition newPos)
{
	if ( pos.x > newPos.x + SHIFT_POS || pos.x < newPos.x - SHIFT_POS
		|| pos.y > newPos.y + SHIFT_POS || pos.y < newPos.y - SHIFT_POS
		|| pos.z > newPos.z + SHIFT_POS || pos.z < newPos.z - SHIFT_POS)
		return TRUE;
	return FALSE;
}

BOOL CVoip3DSoundCalcManager::IsRotChanged( ml3DRotation rot, ml3DRotation newRot)
{
	if ( rot.x > newRot.x + SHIFT_ROT || rot.x < newRot.x - SHIFT_ROT
		|| rot.y > newRot.y + SHIFT_ROT || rot.y < newRot.y - SHIFT_ROT
		|| rot.z > newRot.z + SHIFT_ROT || rot.z < newRot.z - SHIFT_ROT
		|| rot.a > newRot.a + SHIFT_ROT || rot.a < newRot.a - SHIFT_ROT)
		return TRUE;
	return FALSE;
}

BOOL CVoip3DSoundCalcManager::IsTimeOut()
{
	m_dwTime = ::GetTickCount();
	if ( m_dwTime - m_dwStartTime >= TIME_OUT)
		return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOL CVoip3DSoundCalcManager::IsNeedCalculation( voip::IVoipCharacter* pMyIVoipCharacter, voip::IVoipCharacter* pIVoipCharacter)
{
	ml3DPosition myAvPos = pMyIVoipCharacter->getAbsolutePosition();
	ml3DRotation myAvRot = pMyIVoipCharacter->getAbsoluteRotation();

	ml3DPosition avPos = pIVoipCharacter->getAbsolutePosition();
	ml3DRotation avRot = pIVoipCharacter->getAbsoluteRotation();

	BOOL bChanged = FALSE;

	if ( IsPosChanged( m_myAvPos, myAvPos))
		bChanged = TRUE;
	else if ( IsPosChanged( m_avPos, avPos))
		bChanged = TRUE;
	else if ( IsRotChanged( m_myAvRot, myAvRot))
		bChanged = TRUE;
	else if ( IsRotChanged( m_avRot, avRot))
		bChanged = TRUE;

	if ( bChanged && IsTimeOut())
	{
		m_dwStartTime = m_dwTime;
		m_myAvPos = myAvPos;
		m_avPos = avPos;
		m_myAvRot = myAvRot;
		m_avRot = avRot;
		return TRUE;
	}

	return FALSE;
}