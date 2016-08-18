// nrmMotion.cpp: implementation of the nrmMotion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmMotion.h"
#include "nrm3DObject.h"
#include "coreanimationflags.h"
#include "transls.h"

#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"
#include "MotionRecognizer.h"

nrmMotion::nrmMotion(mlMedia* apMLMedia): 
	nrmObject(apMLMedia),
	m_isLoaded(false),
	m_motionID(-1),
	m_motion(NULL)
{
}

void nrmMotion::SetParams()
{
	DurationChanged();

	if ((GetRMMLDuration() == 0) && (GetSourceDuration() != 0))
	{
		if (mpMLMedia)
		{
			mpMLMedia->GetIMotion()->SetDuration(GetSourceDuration());
		}
	}	
}

void nrmMotion::OnChanged(int eventID)
{
	if ((!m_motion) || (!GetMLMedia()))
	{
		return;
	}		

	if (m_motion->IsLoaded())
	{
		if (!m_isLoaded)
		{
			m_isLoaded = true;
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);		

			if (eventID != -1)
			{
				SetParams();
			}
		}
	}
	else if (eventID != -1)
	{
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);		
	}
}

// загрузка нового ресурса
bool nrmMotion::SrcChanged()
{
	if (!mpMLMedia)
	{
		return false;
	}

	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();

	if (!pwcSrc) 
	{
		return false;
	}

	mlString sSRC = pwcSrc; 
	if(sSRC.empty()) 
	{
		return false;
	}

	USES_CONVERSION;

	std::string name = W2A(pwcSrc);
	CMotionRecognizer::GetInstance()->CorrectMotionPathIfNeeded(name);	

	if (m_motion)
	if (m_motion->GetPath() == name)
	{
		return true;
	}

	if (!GetMLMedia()->GetBooleanProp("original_name"))
	{
		name += "_";
		name += IntToStr(g->motm.GetCount());
	}
	
	int lastMotionID = m_motionID;
	m_motionID = g->motm.AddObject(name);

	if ((lastMotionID != m_motionID) && (m_motion))
	{
		m_motion->Unsubscribe(this);
	}

	m_motion = g->motm.GetObjectPointer(m_motionID);
	m_motion->AddChangesListener(this);
	std::string path = W2A(pwcSrc);
	CMotionRecognizer::GetInstance()->CorrectMotionPathIfNeeded(path);	
	m_isLoaded = false;
	if (GetMLMedia()->GetBooleanProp("asynch") == true)
	{		
		m_motion->LoadAsynch(path);
	}
	else if (!m_motion->Load(path))
	{  
		rm_trace("Can`t load motion : %s\n", CComString((LPWSTR)sSRC.c_str()).GetBuffer());
		return false;
	}
	
	m_motion->Subscribe( this);

	PrimaryChanged();

	OnChanged(-1);
	SetParams();

	return true;
}

bool		nrmMotion::GetRMMLPrimary()
{
	if (!mpMLMedia)
	{
		return false;
	}

	return mpMLMedia->GetIMotion()->GetPrimary();
}

DWORD	nrmMotion::GetRMMLDuration()
{
	if (-1 == m_motionID) 
	{
		return 0;
	}

	if (!mpMLMedia)
	{
		return 0;
	}

	return mpMLMedia->GetIMotion()->GetDuration();	
}

DWORD	nrmMotion::GetSourceDuration()
{
	if (!m_motion)
	{
		rm_trace("Error: Motion is not loaded");
		return 0;
	}

	return (unsigned int)m_motion->GetDuration();
}

void nrmMotion::PrimaryChanged()
{
	if (!m_motion)
	{
		rm_trace("Error::PrimaryChanged Motion is not loaded");
		return;
	}

	m_motion->SetPrimaryFlag( GetRMMLPrimary());
}

void	nrmMotion::DurationChanged()
{
	if (!m_motion)
	{
		rm_trace("Error::DurationChanged Motion is not loaded");
		return;
	}

	if (GetRMMLDuration() > 0)
	{
		m_motion->SetDuration((float)GetRMMLDuration());
	}
}

ml3DPosition nrmMotion::GetFramePos(int aiTrack, unsigned int auFrame)
{
	ml3DPosition pos; 
	ML_INIT_3DPOSITION(pos);
	
	if (m_motion)
	{
		CalVector cpos;
		m_motion->GetFrameTranslation(aiTrack, auFrame, &cpos.x, &cpos.y, &cpos.z);
		VECTOR2MLPOSITION(cpos, pos);
	}

	return pos;
}

void nrmMotion::SetFramePos(int aiTrack, unsigned int auFrame, ml3DPosition &pos)
{
	if (m_motion)
	{
		CalVector cpos;
		MLPOSITION2VECTOR(pos, cpos);
		m_motion->SetFrameTranslation(aiTrack, auFrame, cpos.x, cpos.y, cpos.z);
	}
}

ml3DScale nrmMotion::GetFrameScale(int /*aiTrack*/, unsigned int /*auFrame*/)
{
	ml3DScale scl; ML_INIT_3DSCALE(scl);
	return scl;
}

void nrmMotion::SetFrameScale(int /*aiTrack*/, unsigned int /*auFrame*/, ml3DScale &/*scl*/)
{
	// do nothing
}

ml3DRotation nrmMotion::GetFrameRotation(int aiTrack, unsigned int auFrame)
{
	ml3DRotation rot; 
	ML_INIT_3DROTATION(rot);

	if (m_motion)
	{
		CalQuaternion	quat;
		m_motion->GetFrameRotation(aiTrack, auFrame, &quat.x, &quat.y, &quat.z, &quat.w);
		QUAT2MLROTATION(quat, rot);
	}
	return rot;
}

void nrmMotion::SetFrameRotation(int aiTrack, unsigned int auFrame, ml3DRotation &rot)
{
	if (m_motion)
	{
		CalQuaternion quat;
		MLROTATION2QUAT(rot, quat);
		m_motion->SetFrameRotation(aiTrack, auFrame, quat.x, quat.y, quat.z, quat.w);
	}
}

ml3DPosition nrmMotion::GetFramePos(const wchar_t* /*apwcTrack*/, unsigned int /*auFrame*/)
{
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);

	return pos;
}

ml3DScale nrmMotion::GetFrameScale(const wchar_t* /*apwcTrack*/, unsigned int /*auFrame*/)
{
	ml3DScale scl; ML_INIT_3DSCALE(scl);

	return scl;
}

ml3DRotation nrmMotion::GetFrameRotation(const wchar_t* /*apwcTrack*/, unsigned int /*auFrame*/)
{
	ml3DRotation rot; ML_INIT_3DROTATION(rot);

	return rot;
}

bool nrmMotion::moveTo(ml3DPosition& aPos3D)
{
	SetFramePos( -1, 0, aPos3D);

	return true;
}

bool nrmMotion::rotateTo(ml3DRotation& aRot3D)
{
	SetFrameRotation( -1, 0, aRot3D);

	return true;
}

//!! реализация CAnimationEvents
//!! Вызывает в скрипте событие animation::onDone
void nrmMotion::OnAnimationComplete()
{
	/*MotionEventsParam	param;
	param.event = ME_ON_DONE;
	FrameEvents.push_back( param);*/
	if (mpMLMedia)
	{
		mpMLMedia->GetIMotion()->onDone();
	}
}

//!! Вызывает в скрипте событие animation::onSet
void nrmMotion::OnAnimationSet()
{
	if (mpMLMedia)
	{
		mpMLMedia->GetIMotion()->onSet();
	}
}

nrmMotion::~nrmMotion()
{
	if (m_motion)
	{
		m_motion->DeleteChangesListener(this);
		m_motion->Unsubscribe(this);
	}

	g->motm.DeleteObject(m_motionID);
}