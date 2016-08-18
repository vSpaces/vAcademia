
#include "StdAfx.h"
#include "Cave.h"
#include "RMContext.h"
#include "GlobalSingletonStorage.h"

//#include "trackdAPI_CC.h"

CCave::CCave():
	m_x(0),
	m_y(0),
	m_fixX(0),
	m_fixY(0),	
	m_trackerKey(0),
	m_tracker(NULL)
{
}

void CCave::Update()
{
	if (!g->rs.GetBool(CAVE_SUPPORT))
	{
		return;
	}

	if (g->phom.GetControlledObject())
	{
		C3DObject* avatar = g->phom.GetControlledObject()->GetObject3D();
		if (avatar)
		{
			CVector3D crds = avatar->GetCoords();

			crds.x += GetDeltaX();
			crds.y += GetDeltaY();
			FixPosition();

			avatar->SetLocalCoords(crds.x, crds.y, crds.z);
		}
	}
}

bool CCave::Init()
{
	char pchValue[40] = "";
	g->rs.SetBool(CAVE_SUPPORT, false);
	pchValue[0] = 0;
	gRM->nrMan->GetContext()->mpApp->GetSettingFromIni("settings", "cave", pchValue, 3);
	if (strcmp(pchValue, "1") == 0)
	{
		g->rs.SetBool(CAVE_SUPPORT, true);
	}	

	pchValue[0] = 0;
	gRM->nrMan->GetContext()->mpApp->GetSettingFromIni("settings", "cave_tracker_key", pchValue, 20);
	/*m_trackerKey = rtl_atoi(pchValue);

	if (m_trackerKey != 0) 
	{		
		try
		{
            m_tracker = new TrackerReader(m_trackerKey);
        }
		catch (TrackdError te)
		{
            m_tracker = NULL;
        }
	}*/

	return g->rs.GetBool(CAVE_SUPPORT);
}

float CCave::GetX()
{
	/*if (m_tracker)
	{
		float pos[3];
		((TrackerReader*)m_tracker)->trackdGetPosition(0, pos);
		m_x = pos[0] * 100.0f;
		return m_x;
	}
	else*/
	{
		return 0.0f;
	}
}

float CCave::GetY()
{
	/*if (m_tracker)
	{
		float pos[3];
		((TrackerReader*)m_tracker)->trackdGetPosition(0, pos);
		m_y = pos[2] * 100.0f;
		return m_y;
	}
	else*/
	{
		return 0.0f;
	}
}

float CCave::GetDeltaX()
{
	return GetX() - m_fixX;
}

float CCave::GetDeltaY()
{
	return GetY() - m_fixY;
}

void CCave::FixPosition()
{
	m_fixX = m_x;
	m_fixY = m_y;
}

CCave::~CCave()
{
}