
#pragma once

#include "CommonCal3DHeader.h"

#include "global.h"
#include "vector.h"
#include "quaternion.h"

class CalModel;
class CalBone;

class CAL3D_API CalVisemeController  
{
public:
	CalVisemeController(CalModel* pModel);
	virtual ~CalVisemeController();

	void Enable(bool isEnabled);
	bool IsEnabled()const;

	bool Init(int motionID);
	void Update();

	void SetState(int viseme1ID, int viseme2ID, float koef);

private:
	CalModel*				m_model;

	MP_VECTOR<CalBone*>	m_bones;
	MP_VECTOR<int>		m_boneIDs;

	int						m_motionID;
	int						m_currentViseme1ID;
	int						m_currentViseme2ID;

	float					m_currentVisemeKoef;

	bool					m_isEnabled;

};
