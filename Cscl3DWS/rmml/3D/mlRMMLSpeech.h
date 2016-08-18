#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLSpeech_H_
#define _mlRMMLSpeech_H_

namespace rmml {

#define EVNM_onSaid L"onSaid"
#define EVNM_onFalling L"onFalling"
#define EVNM_onFallAndStop L"onFallAndStop"
#define EVNM_onWreck L"onWreck"
#define EVNM_onNeedToChangePosition L"onNeedToChangePosition"

/**
 *  ласс реплик персонажей RMML
 */

struct SynchEvListeners{
	long muLastPos; // последн€€ позици€ (ms)
	v_elems::iterator mvi; // указатель на первого, еще не уведомленного слушател€
	v_elems mvListeners; // список слушателей
	SynchEvListeners():
		MP_VECTOR_INIT(mvListeners)
	{
		Reset();
	}
	void Reset(){
		muLastPos=-1;
		mvi=mvListeners.begin();
	}
};

class mlRMMLSpeech:
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					mlIAudible,
					mlISpeech
{
friend class mlRMMLCharacter;
	std::auto_ptr<SynchEvListeners> mpSEListeners;
public:
	mlRMMLSpeech(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLSpeech(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_actions = 1,
		EVID_onSaid
	};
	enum {
		EVSM_onSaid = (1 << 0)
	};
public:
	void ResetActions();
	void PerformCurActions();

// реализаци€ mlRMMLElement
MLRMMLELEMENT_IMPL
	EventSpec* GetEventSpec2(){ return _EventSpec; } 
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	void SetSynchEventListener(mlRMMLElement* apMLElListener, SynchEventSender* apSEEventSender);
	void RemoveSynchEventListener(mlRMMLElement* apMLElListener);

// реализаци€  mlILoadable
MLILOADABLE_IMPL

// реализаци€  mlIAudible
MLIAUDIBLE_IMPL
	unsigned long GetRMCurPos(){ return 0;};
	const wchar_t* GetEmitterType(){ return NULL;};
	const wchar_t* GetSplineSrc(){ return NULL;};
	const int GetVolume(){ return 1;};
	const wchar_t* GetLocation(){ return NULL;};

// реализаци€  mlISpeech
public:
	mlISpeech* GetISpeech(){ return this; }
	mlRMMLElement* GetElem_mlRMMLSpeech(){ return this; }
	void onSaid();
/*
// реализаци€  mlIContinuous
public:
	// свойства
	bool GetPlaying(){ return false; } // получить флаг проигрывани€
	void SetFPS(const short aFPS){}	// устанавливает значение FPS
	int GetCurFrame(){ return 0; } // получить номер кадра на который нужно перейти
	void SetCurFrame(const int aFrame){};	// устанавливает значение текущего кадра
	void SetLength(const int alLength){ }
	void SetCurPos(unsigned long alPos){ muCurrentPos=alPos; }
	unsigned long GetCurPos(){ return muCurrentPos; }
	void SetDuration(const unsigned long alDur){ }
	bool GetLoop(){ return false; }	// получить флаг цикличности
	// событи€
	void onEnterFrame(){}
	void onPlayed(){}
	//
	void Rewind(){}
	void Play(){}
	void Stop(){}
*/
};

}

#endif










































