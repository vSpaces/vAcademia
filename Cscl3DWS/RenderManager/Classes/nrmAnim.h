
#if !defined(AFX_nrmAnim_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmAnim_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "Sprite.h"
#include "nrmObject.h"
#include "nrmMOIVisibleStub.h"
#include "NRManager.h"
#include "IChangesListener.h"

class nrmAnim :		public nrmObject,
					public nrmMOIVisibleStub,
					public moIButton,
					public moIContinuous,
					public IChangesListener
{
public:
	nrmAnim(mlMedia* apMLMedia);
	virtual ~nrmAnim();

// реализация moMedia
public:
	moIContinuous* GetIContinuous(){ return this; }
	moILoadable* GetILoadable(){ return this; }
	moIVisible* GetIVisible(){ return this; }
	moIButton* GetIButton(){ return this; }

	void OnChanged(int eventID);

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src
// реализация  moIVisible
public:
	// Свойства
	void AbsXYChanged(); // изменилось абсолютное положение 
	void SizeChanged();
	void AbsDepthChanged(); // изменилась абсолютная глубина
	void AbsVisibleChanged(); // изменилась абсолютная видимость
	void AbsOpacityChanged(); // изменилась абсолютная видимость
	mlSize GetMediaSize(); 
	void SetMask(int /*aiX*/, int /*aiY*/, int /*aiW*/, int /*aiH*/);
	int IsPointIn(const mlPoint /*aPnt*/); // -1 - not handled, 0 - point is out, 1 - point is in

// реализация  moIButton
public:
	void onKeyDown(){}
	void onKeyUp(){}

	void onSetFocus(){}
	void onKillFocus(){}
	void AbsEnabledChanged();

// реализация  moIContinuous
public:
	// Свойства
	void PlayingChanged();	// Начать/Закончить проигрывание 
	void FrameChanged();	// переставить на новое место
	void PosChanged();	// переставить на новое место (ms)
	void LengthChanged(); // изменилось количество кадров в картиночной анимации
	void DurationChanged(); // изменилась продолжительность картиночной анимации (в ms)
	void FPSChanged(); // изменился FPS картиночной анимации
	unsigned long GetCurPos();	// запрашивает текущее значение (продолжительность в ms)
	long GetDuration();	// запрашивает значение _duration (продолжительность в ms)

	void setCheckPoints(long* /*apfCheckPoints*/, int /*aiLength*/){}
	long getPassedCheckPoint(){ return 0; }

	CSprite* m_sprite;

private:
	bool m_isFirstLoaded;
};

#endif // !defined(AFX_nrmAnim_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
