// nrmVideo.h: interface for the nrmVideo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmVideo_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmVideo_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nrmanager.h"
#include "nrmObject.h"
#include "nrmMOIVisibleStub.h"
#include "Sprite.h"
#include "IVideo.h"

class nrmObject;

class nrmVideo :	public nrmObject,
					public nrmMOIVisibleStub,
					public moIButton,
					public moIContinuous,
					public moIVideo
{
public:
	nrmVideo(mlMedia* apMLMedia);
	virtual ~nrmVideo();

// реализаци€ moMedia
public:
	moIContinuous* GetIContinuous(){ return this; }
	moILoadable* GetILoadable(){ return this; }
	moIVisible* GetIVisible(){ return this; }
	moIButton* GetIButton(){ return this; }
	void FrozenChanged();

// реализаци€  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src
// реализаци€  moIVisible
public:
	// —войства
	void AbsXYChanged(); // изменилось абсолютное положение 
	void SizeChanged();
	void AbsDepthChanged(); // изменилась абсолютна€ глубина
	void AbsVisibleChanged(); // изменилась абсолютна€ видимость
	void AbsOpacityChanged(); // изменилась абсолютна€ видимость
	mlSize GetMediaSize(); 
	int IsPointIn(const mlPoint /*aPnt*/){ return -1; }

// реализаци€  moIButton
public:
	void onKeyDown(){}
	void onKeyUp(){}

	void onSetFocus(){}
	void onKillFocus(){}
	void AbsEnabledChanged(){}

// реализаци€  moIContinuous
public:
	// —войства
	void PlayingChanged();	// Ќачать/«акончить проигрывание
	void FrameChanged();	// переставить на новое место
	void PosChanged();	// переставить на новое место (ms)
	unsigned long GetCurPos();	// запрашивает текущее значение (продолжительность в ms)
	long GetDuration();	// запрашивает значение _duration (продолжительность в ms)
	//
	void setCheckPoints(long* /*apfCheckPoints*/, int /*aiLength*/){}
	long getPassedCheckPoint(){ return 0; }

// реализаци€  moIVideo
public:
	void	OnLoaded();
	void	WindowedChanged();
	void	OnSetRenderManager();

	IVideo*				m_video;
};

#endif // !defined(AFX_nrmVideo_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
