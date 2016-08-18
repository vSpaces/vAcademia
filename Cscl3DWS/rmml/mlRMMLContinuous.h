#pragma once

namespace rmml {

#define EVNM_onEnterFrame L"onEnterFrame"
#define EVNM_onPlayed L"onPlayed"

// типы значений checkpoint-ов для синхронизации continuous-ов и speech-ей
//#define SCHPVT_MS 1
//#define SCHPVT_SEC 2
//#define SCHPVT_FRAME 3

class RMML_NO_VTABLE mlRMMLContinuous: 
										//public mlJSClassProto,
										public mlIContinuous,
										public moIContinuous
{
friend class mlRMMLElement;
public:
	mlRMMLContinuous(void);
	~mlRMMLContinuous(void);
MLJSPROTO_DECL
private:
	enum {JSPROP0_mlRMMLContinuous=TIDB_mlRMMLContinuous-1,
		JSPROP__playing,
		JSPROP_currentFrame,
		JSPROP__length,
		JSPROP_currentPos,
		JSPROP__duration,
		JSPROP_fps,
		JSPROP__loop,
		TIDE_mlRMMLContinuous
	};
	enum { EVID_mlRMMLContinuous=TIDE_mlRMMLContinuous-1,
		EVID_onEnterFrame,
		EVID_onPlayed,
		TEVIDE_mlRMMLContinuous
	};
	enum {
		EVSM_onEnterFrame = (1 << 6),
		EVSM_onPlayed = (1 << 5)
	};
protected:
	bool _playing;
	int currentFrame;
	unsigned int _length;
	unsigned long currentPos; // ms
	long duration; // ms
	char fps;
	bool _loop;

	JSFUNC_DECL(setCheckPoints)
	JSFUNC_DECL(rewind)
	JSFUNC_DECL(play)
	JSFUNC_DECL(stop)
	JSFUNC_DECL(rewindAndPlay)
	JSFUNC_DECL(gotoAndPlay)
	JSFUNC_DECL(gotoAndStop)

public:
	bool Cont_SetEventData(char aidEvent, mlSynchData &Data);
	bool Freeze();
	bool Unfreeze();

protected:
	virtual mlRMMLElement* GetElem_mlRMMLContinuous()=0;
	virtual int GetFrameByName(const wchar_t* apwcName){ return -1; }
	JSBool goTo(jsval avArg, const char* alpcFuncName);
	void FrameChangedDontStop();
	void PosChangedDontStop();

// реализация  mlIContinuous
public:
	// свойства
	bool GetPlaying(){ // получить флаг проигрывания
		if(GetElem_mlRMMLContinuous()->IsFrozen()) return false; 
		return _playing; 
	} 
	void SetFPS(const short aFPS){ fps=(char)aFPS; }	// устанавливает значение FPS
	int GetFPS(){ return fps; }	// получить значение FPS для картиночной анимации
	int GetCurFrame(){ return currentFrame; } // получить номер кадра на который нужно перейти
	void SetCurFrame(const int aFrame){ currentFrame=aFrame;};	// устанавливает значение текущего кадра
	void SetLength(const int alLength){ _length=alLength; }
	int GetLength(){ return _length; }	// получить кол-во кадров картиночной анимации
	void SetCurPos(unsigned long alPos){ currentPos=alPos; }
	unsigned long GetCurPos(){ return currentPos; }
//	void SetDuration(const unsigned long alDur){ _duration=alDur; }
	long GetIADuration(){ return duration; } // получить продолжительность картиночной анимации
	bool GetLoop(){ return _loop; }	// получить флаг цикличности

	// события
	void onEnterFrame();
	void onPlayed();
	//
	void Rewind();
	void Play();
	void Stop();

// реализация moIContinuous
protected:
	moIContinuous* GetmoIContinuous();
	// Свойства
	virtual void PlayingChanged();
	virtual void FrameChanged();
	virtual void PosChanged();
	virtual void LengthChanged(); // изменилось количество кадров в картиночной анимации
	virtual void DurationChanged(); // изменилась продолжительность картиночной анимации (в ms)
	virtual void FPSChanged(); // изменился FPS картиночной анимации
	virtual long GetDuration();	// запрашивает значение _duration (продолжительность в ms)
	//
	void setCheckPoints(long* apfCheckPoints, int aiLength);
	long getPassedCheckPoint();
};

#define MLICONTINUOUS_IMPL \
public: \
	mlRMMLElement* GetElem_mlRMMLContinuous(){ return this; } \
	mlIContinuous* GetIContinuous(){ return this; } \
	mlRMMLContinuous* GetContinuous(){ return this; }

	//void OnEnterFrame(){ mlRMMLContinuous::onEnterFrame(this); } \
	//void OnPlayed(){ mlRMMLContinuous::onPlayed(this); }

}