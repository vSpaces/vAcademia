
#pragma once

#include "CommonHeader.h"

#include "Vector3D.h"
#include "CommonObject.h"
#include "cal3d\Model.h"
#include "cal3d\CoreAnimation.h"
#include "cal3d\CoreKeyframe.h"
#include "cal3d\animcallback.h"
#include "EventSource.h"
#include "IDestroyListener.h"
#include "ianimationeventshandler.h"

class CMotion : public CCommonObject,
				public CalAnimationCallback,
				public CEventSource<IAnimationEventsHandler>,
				public CBaseChangesInformer,
				public IChangesListener
{
public:
	CMotion();
	~CMotion();

	//! Реализация CalAnimationCallback
public:
	//! Вызывается после апдейта анимации. Частота вызовов определяется при регистрации callback`a
	virtual void AnimationUpdate(const float anim_time, CalModel* const model, void* const userData);
	//! Вызывается после окончания анимации (команда execute_action)
	virtual void AnimationComplete(CalModel* const model, void* const userData);
	//! Вызывается после окончания анимации (команда set_action)
	virtual void AnimationSet(CalModel* const model, void* const userData);

public:
	void OnChanged(int eventID);

	bool IsLoaded()const;
	void SetLoadedStatus(const bool isLoaded);
	void SetPrimaryFlag(const bool isPrimary);

	bool Load(std::string& path);
	void LoadAsynch(std::string& path);
	bool LoadFromFile(const std::string& path);

	void RegisterForPath(std::string& path);
	
	float GetDuration();
	void SetDuration(const float time);

	CVector3D GetFrameTranslation(const int trackID, const int frameID);
	void GetFrameTranslation(const int trackID, const int frameID, float* const x, float* const  y, float* const z);
	void SetFrameTranslation(const int trackID, const int frameID, const float x, const float y, const float z);
	void SetFrameTranslation(const int trackID, const int frameID, const CVector3D& translation);

	void GetFrameRotation(const int trackID, const int frameID, float* const x, float* const y, float* const z, 
		float* const w);
	void SetFrameRotation(const int trackID, const int frameID, const float x, const float y, const float z, 
		const float w);

	void FreeResources();

	CalCoreAnimation* GetCoreAnimation()const;

	void AddDestroyListener(IDestroyListener* const destroyListener);
	void RemoveDestroyListener(IDestroyListener* const destroyListener);

	std::string GetPath()const;

	void SetCalModel(CalModel* const model);

	unsigned int GetCompatabilityID()const;

private:
	CalCoreKeyframe* GetKeyframe(const int trackID, const int frameID);

	MP_VECTOR<IDestroyListener *> m_destroyListeners;

	CalCoreAnimation* m_motion;
	CalModel* m_model;

	MP_STRING m_path;

	float m_duration;

	bool m_isLoaded;
	bool m_isPrimary;

	int m_compatabilityID;

	CRITICAL_SECTION m_listenersCS;
};