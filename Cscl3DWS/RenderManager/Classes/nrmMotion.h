// nrmMotion.h: interface for the nrmMotion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmMotion_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmMotion_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nrmObject.h"
#include "Motion.h"

class nrmCharacter;

class nrmMotion :	public nrmObject,
					public moIMotion,
					public IAnimationEventsHandler,
					public IChangesListener
{
	friend	class	nrm3DObject;
	friend	class	nrmCharacter;
public:
	nrmMotion(mlMedia* apMLMedia);
	virtual ~nrmMotion();

	void OnChanged(int eventID);

	// реализация IAnimationEventsHandler
public:
	// анимация выполнена
	virtual void OnAnimationComplete();
	// анимация установлена
	virtual void OnAnimationSet();

// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }
	moIMotion* GetIMotion(){ return this; } 

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализация  moIMotion
public:
	void DurationChanged();
	void InverseChanged(){}
	void PrimaryChanged();
	// какой объект выполняет или выполнил это действие
	mlMedia* GetObject(){	return NULL;}//??
	//
	ml3DPosition GetFramePos(int aiTrack, unsigned int auFrame);
	void SetFramePos(int aiTrack, unsigned int auFrame, ml3DPosition &pos);
	ml3DScale GetFrameScale(int aiTrack, unsigned int auFrame);
	void SetFrameScale(int aiTrack, unsigned int auFrame, ml3DScale &scl);
	ml3DRotation GetFrameRotation(int aiTrack, unsigned int auFrame);
	void SetFrameRotation(int aiTrack, unsigned int auFrame, ml3DRotation &rot);

	void SetFramePos(const wchar_t* /*apwcTrack*/, unsigned int /*auFrame*/, ml3DPosition &/*pos*/){};
	void SetFrameScale(const wchar_t* /*apwcTrack*/, unsigned int /*auFrame*/, ml3DScale &/*scl*/){};
	void SetFrameRotation(const wchar_t* /*apwcTrack*/, unsigned int /*auFrame*/, ml3DRotation &/*rot*/){};
	ml3DPosition GetFramePos(const wchar_t* apwcTrack, unsigned int auFrame);
	ml3DScale GetFrameScale(const wchar_t* apwcTrack, unsigned int auFrame);
	ml3DRotation GetFrameRotation(const wchar_t* apwcTrack, unsigned int auFrame);

	bool moveTo(ml3DPosition& aPos3D);
	bool rotateTo(ml3DRotation& aRot3D);

private:
	void SetParams();

protected:
	DWORD		GetRMMLDuration();
	DWORD		GetSourceDuration();	
	bool		GetRMMLPrimary();

	int			m_motionID;
	CMotion*	m_motion;	

	bool		m_isLoaded;
};

#endif // !defined(AFX_nrmMotion_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
