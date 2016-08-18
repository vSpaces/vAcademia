
#include "StdAfx.h"
#include "Motion.h"
#include "cal3d\Loader.h"
#include "cal3d\BVHLoader.h"
#include "cal3d\CoreTrack.h"
#include "cal3d\coreanimationflags.h"
#include "HelperFunctions.h"
#include "MotionRecognizer.h"
#include "GlobalSingletonStorage.h"

CMotion::CMotion():
	m_motion(NULL),
	m_duration(1000),
	m_isLoaded(false),
	m_isPrimary(false),
	MP_STRING_INIT(m_path),
	m_compatabilityID(0),
	MP_VECTOR_INIT(m_destroyListeners)
{
	InitializeCriticalSection(&m_listenersCS);
}

unsigned int CMotion::GetCompatabilityID()const
{
	assert(m_compatabilityID != 0);
	return m_compatabilityID;
}

std::string CMotion::GetPath()const
{
	return m_path;
}

bool CMotion::Load(std::string& path)
{
	g->motm.CorrectPath(path);

	if (path.size() < 3)
	{
		return false;
	}  

	FreeResources();

	m_motion = g->motm.GetCalCoreAnimation(path);
	m_path = path;		

	if (m_motion)
	{		
		m_motion->setLock(path);
		m_motion->registerCallback( this, 0);
		SetLoadedStatus(true);
		SetPrimaryFlag( m_isPrimary);
		return true;
	}
	
	if (!LoadFromFile(m_path))
	{
		return false;
	}

	g->motm.AddCalCoreAnimation(m_path, m_motion);
	SetLoadedStatus(true);

	return true;
}

void CMotion::RegisterForPath(std::string& path)
{
    g->motm.CorrectPath(path);

	g->motm.AddCalCoreAnimation(path, m_motion);
	if (m_motion)
	{		
		m_motion->setLock(path);
	}
}

bool CMotion::LoadFromFile(const std::string& path)
{
	int t1 = (int)g->tp.GetTickCount();
		
	std::string ext = path.substr(path.size() - 3, 3);
	ext = StringToLower(ext);
	if ((ext != "caf") && (ext != "bvh"))
	{
		return false;	
	}

	unsigned char* data = NULL;
	int size = 0;

	ifile* file = g->rl->GetResMemFile(path.c_str());
	if (file)
	{
		size = file->get_file_size();
		data = MP_NEW_ARR(unsigned char, size);//new unsigned char[size];
		file->read(data, size);
		g->rl->DeleteResFile(file);
	}

	if (data)
	{
		int errorCode = 0;
		if ("caf" == ext)
		{
			CalLoader loader;
			m_motion = loader.loadCoreAnimation(data, size);					
		}
		else
		{
			CalBvhLoader	loader;
			m_motion = loader.loadBVH(data, size, errorCode);			
		}		
		
		if (!m_motion)
		{
			MP_DELETE_ARR(data);//delete[] data;

			g->lw.WriteLn("[LOADWARNING] Loading motion ", path, " failed... errorCode = ", errorCode, " size = ", size);
			return false;			

		}
		m_motion->setLock(path);
		m_motion->registerCallback( this, 0);
		SetPrimaryFlag( m_isPrimary);

		MP_DELETE_ARR(data);//delete[] data;
	}

	int t2 = (int)g->tp.GetTickCount();

	stat_allMotionsTime += (t2 - t1);

	return true;
}
	
float CMotion::GetDuration()
{
	return m_duration;
}

void CMotion::SetDuration(float time)
{
	m_duration = time;
}

CalCoreKeyframe* CMotion::GetKeyframe(const int trackID, const int frameID)
{
	if (!m_motion)	
	{
		return NULL;
	}

	CalCoreTrack* track = m_motion->getCoreTrack(trackID);
	if (!track)	
	{
		return NULL;
	}

	std::map<float, CalCoreKeyframe *>&	frames = track->getMapCoreKeyframe();
	if ((unsigned int)frameID >= frames.size())	
	{
		return NULL;
	}

	std::map<float, CalCoreKeyframe *>::iterator fit = frames.begin();
	float tm = fit->first;
	for(int i = 0; i < frameID; i++)
	{
		fit = frames.upper_bound(tm);
		tm = fit->first;
	}

	CalCoreKeyframe* keyframe = fit->second;
	return keyframe;
}

CVector3D CMotion::GetFrameTranslation(const int trackID, const int frameID)
{
	CalCoreKeyframe* keyframe = GetKeyframe(trackID, frameID);

	if (!keyframe)	
	{
		return CVector3D(0.0f, 0.0f, 0.0f);
	}

	return CVector3D(keyframe->getTranslation().x, keyframe->getTranslation().y, keyframe->getTranslation().z);
}

void CMotion::GetFrameTranslation(const int trackID, const int frameID, float* const x, float* const y, 
								  float* const z)
{
	CalCoreKeyframe* keyframe = GetKeyframe(trackID, frameID);

	if (!keyframe)	
	{
		*x = 0;
		*y = 0;
		*z = 0;
	}

	*x = keyframe->getTranslation().x;
	*y = keyframe->getTranslation().y;
	*z = keyframe->getTranslation().z;
}

void CMotion::SetFrameTranslation(const int trackID, const int frameID, const float x, const float y, const float z)
{
	CalCoreKeyframe* keyframe = GetKeyframe(trackID, frameID);

	if (keyframe)
	{
		keyframe->setTranslation(CalVector(x, y, z));
	}
}

void CMotion::SetFrameTranslation(const int trackID, const int frameID, const CVector3D& translation)
{
	CalCoreKeyframe* keyframe = GetKeyframe(trackID, frameID);

	if (keyframe)
	{
		keyframe->setTranslation(CalVector(translation.x, translation.y, translation.z));
	}
}

void CMotion::GetFrameRotation(const int trackID, const int frameID, float* const x, float* const y, 
							   float* const z, float* const w)
{
	CalCoreKeyframe* keyframe = GetKeyframe(trackID, frameID);

	if (!keyframe)	
	{
		*x = 0;
		*y = 0;
		*z = 0;
		*w = 1;
	}

	*x = keyframe->getRotation().x;
	*y = keyframe->getRotation().y;
	*z = keyframe->getRotation().z;
	*w = keyframe->getRotation().w;
}

void CMotion::SetFrameRotation(const int trackID, const int frameID, const float x, const float y, const float z, 
							   const float w)
{
	CalCoreKeyframe* keyframe = GetKeyframe(trackID, frameID);

	if (keyframe)	
	{
		keyframe->setRotation(CalQuaternion(x, y, z, w));
	}
}

void CMotion::AddDestroyListener(IDestroyListener* const destroyListener)
{
	EnterCriticalSection(&m_listenersCS);
	m_destroyListeners.push_back(destroyListener);
	LeaveCriticalSection(&m_listenersCS);
}

void CMotion::RemoveDestroyListener(IDestroyListener* const destroyListener)
{
	EnterCriticalSection(&m_listenersCS);

	std::vector<IDestroyListener *>::iterator it = m_destroyListeners.begin();
	std::vector<IDestroyListener *>::iterator itEnd = m_destroyListeners.end();

	for ( ; it != itEnd; it++)
	if ((*it) == destroyListener)
	{
		m_destroyListeners.erase(it);		
		break;
	}

	LeaveCriticalSection(&m_listenersCS);
}

void CMotion::FreeResources()
{
	if (m_motion)
	{
		if (g->ne.GetWorkMode() != WORK_MODE_DESTROY)
		{
			EnterCriticalSection(&m_listenersCS);

			std::vector<IDestroyListener *>::iterator it = m_destroyListeners.begin();
			std::vector<IDestroyListener *>::iterator itEnd = m_destroyListeners.end();

			for ( ; it != itEnd; it++)
			{	
				(*it)->OnDestroy(this);
			}

			LeaveCriticalSection(&m_listenersCS);

			if (g->motm.ReleaseCalCoreAnimation(m_path))
			{			
				m_motion->destroy();
				CalLoader loader;
				loader.deleteCoreAnimation(m_motion);
				m_motion = NULL;
				//MP_DELETE(m_motion);//delete m_motion;				
			}
		}

		m_motion = NULL;	
	}
}

CalCoreAnimation* CMotion::GetCoreAnimation()const
{
	return m_motion;
}

//! Вызывается после апдейта анимации. Частота вызовов определяется при регистрации callback`a
void CMotion::AnimationUpdate(const float /*animTime*/, CalModel* const /*model*/, void* const /*userData*/)
{
	//??
}

//! Вызывается после окончания анимации (команда execute_action)
void CMotion::AnimationComplete(CalModel* const model, void* const /*userData*/)
{
	if (model == m_model)
	{
		Invoke<&IAnimationEventsHandler::OnAnimationComplete>();
	}
}

//! Вызывается после окончания анимации (команда set_action)
void CMotion::AnimationSet(CalModel* const model, void* const /*userData*/)
{
	if (model == m_model)
	{
		Invoke<&IAnimationEventsHandler::OnAnimationSet>();
	}
}

void CMotion::SetCalModel(CalModel* const model)
{
	m_model = model;
}

void CMotion::LoadAsynch(std::string& path)
{
	g->motm.CorrectPath(path);

	if (path.size() < 3)
	{
		return;
	}  

	m_motion = g->motm.GetCalCoreAnimation(path);
	
	m_path = path;		

	if (m_motion)
	{
		m_motion->setLock(path);
		m_motion->registerCallback( this, 0);
		SetLoadedStatus(true);
		SetPrimaryFlag( m_isPrimary);
		return;
	}
	
	g->motm.AddAsynchMotion(this, path);
}

bool CMotion::IsLoaded()const
{
	return m_isLoaded;
}

void CMotion::SetPrimaryFlag(const bool isPrimary)
{
	m_isPrimary = isPrimary;
	if( m_motion)
	{
		if( m_isPrimary)
			m_motion->setFlag( CAL3D_COREANIM_PRIMARY);
		else
			m_motion->removeFlag( CAL3D_COREANIM_PRIMARY);
	}
}

void CMotion::SetLoadedStatus(const bool isLoaded)
{
	m_isLoaded = isLoaded;

	m_compatabilityID = CMotionRecognizer::GetInstance()->GetIDByPath(m_path);	
}

void CMotion::OnChanged(int /*eventID*/)
{
	OnAfterChanges();
}

CMotion::~CMotion()
{
	FreeResources();
	DeleteCriticalSection(&m_listenersCS);
}