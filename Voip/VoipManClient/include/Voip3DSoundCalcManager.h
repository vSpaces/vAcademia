#pragma once

struct voip::IVoipCharacter;

class CVoip3DSoundCalcManager
{
public:
	CVoip3DSoundCalcManager();
	virtual ~CVoip3DSoundCalcManager();

protected:
	BOOL IsPosChanged( ml3DPosition pos, ml3DPosition newPos);
	BOOL IsRotChanged( ml3DRotation rot, ml3DRotation newRot);
	BOOL IsTimeOut();

public:
	BOOL IsNeedCalculation( voip::IVoipCharacter* pMyIVoipCharacter, voip::IVoipCharacter* pIVoipCharacter);

private:
	DWORD m_dwStartTime;
	DWORD m_dwTime;
	ml3DPosition m_myAvPos;
	ml3DRotation m_myAvRot;
	ml3DPosition m_avPos;
	ml3DRotation m_avRot;
};
