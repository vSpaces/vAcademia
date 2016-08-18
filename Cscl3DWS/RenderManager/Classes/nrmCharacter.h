// nrmCharacter.h: interface for the nrmCharacter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmCharacter_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmCharacter_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "../CommonRenderManagerHeader.h"

//////////////////////////////////////////////////////////////////////////
#include "nrmanager.h"
#include "VoipManClient.h"
#include "nrm3DObject.h"
#include "WalkController.h"
#include "SkeletonAnimationObject.h"
#include "Recognize.h"
//////////////////////////////////////////////////////////////////////////
class CEyesBlinkCtrl;
class C3DCharacter;
class CSCloudCtrl;
class nrmAudio;
class nrmVisemes;


//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class nrmCharacter :	public nrm3DObject,
						public moICharacter,
						public moIBinSynch,
						public voip::IVoipCharacter

{
	friend class CEyesBlinkCtrl;
public:
	nrmCharacter(mlMedia* apMLMedia);
	virtual ~nrmCharacter();

public:
	/*BEGIN_INTERFACE_IMPLEMENTATION
	END_INTERFACE_IMPLEMENTATION*/

// реализация imessage_handler
public:
	bool	handle_message(LPVOID target, DWORD message, LPVOID data);

	// реализация moIBinSynch, для работы с Kinect
public:
	// 
	void ResetSynchKinectState();
	// с сервера пришло полное состояние: установить его
	bool SetFullState(unsigned int auStateVersion, const unsigned char* apState, int aiSize);
	// с сервера пришли изменения состояния: обновить состояние
	bool UpdateState(unsigned int auStateVersion, BSSCCode aCode, const unsigned char* apState, int aiSize
						, unsigned long aulOffset, unsigned long aulBlockSize);
	// нужно отправить на сервер полное состояние: запросить его
	// (возвращает размер данных, apState может быть null)
	unsigned long GetFullState(unsigned int& auStateVersion, const unsigned char* apState, int aiSize);
	bool GetFullStateImpl(unsigned int& auStateVersion, const unsigned char* apState, int& aiSize);
	// Сбросить состояние объекта, обнулить его версию
	void Reset();
	// Вернуть себя как генератора бинарного состояния
	moIBinSynch* GetIBinSynch();

	// реализация ieventslistener
public:
	// ET_POSITIONCHANGED
	//void	on_position_changed( icollisionobject* apicollisionobject){}
	// ET_GROUPOBJECTSASYNCHCREATED
	//void	on_group_objects_asynch_created( igroup3d* apigroup3d){}
	// Произвольное событие
	//void	handle_event( events::EVENTS_TYPE event, ibase* apiobject, LPVOID apdata);

public:
	int				uID;

// реализация moMedia
public:
	void GetSynchData(mlSynchData &aSynchData);
	void SetSynchData(mlSynchData &aSynchData);
	moI3DObject* GetI3DObject(){ return this; }
	moIButton* GetIButton(){ return this; }

	bool setKinectDeskPoints(const ml3DPosition& aPos, const ml3DPosition& aPos2, const moMedia* moObj);
	void disableKinectDesk();

	void toInitialPose();
	void fromInitialPose();

	void OnChanged(int eventID);

private:
	void OnLoaded();

// реализация  moILoadable
public:
	void loaded();
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализация  moIButton
public:
	virtual void onKeyDown();
	virtual void onKeyUp();	

	// реализация  moI3DObject
public:
	void FrozenChanged();
	void UnpickableChanged();

	bool	setSleepingState(bool isSleeped);

// реализация  moICharacter
public:
	void onTurnArrived();
	void onPointArrived();
	//bool idleCanStart(mlMedia* apmlMotion);
	virtual void update(DWORD	dwTime, void* data);
	virtual	void HandleCal3DAction(ACTIONTYPE type, int actvalue);
	moICharacter* GetICharacter(){ return this; }
	void IdlesSrcChanged(const wchar_t* apwcIdlesName, const wchar_t* apwcSrc);

	void MovementChanged();
	void VisemesChanged();
	void IdlesChanged();
	void FaceIdlesChanged();
	mlMedia* GetSpeech(); // speech/audio, который сейчас произносится
	mlICharacter** GetInterlocutors();
	bool say(mlMedia* apSpeech=NULL, int* apDuration=NULL);
	bool goTo(mlMedia* apObject, ml3DPosition* apTurnToPos=NULL, mlMedia* apTurnToObject=NULL, unsigned int shiftTime = 0);
	bool goTo(ml3DPosition &aPos, ml3DPosition* apTurnToPos=NULL, mlMedia* apTurnToObject=NULL, unsigned int shiftTime = 0);
	bool turnTo(mlMedia* apObject);
	bool turnTo(ml3DPosition &aPos);
	bool turnTo(int aiFlags);
	bool lookAt(mlMedia* apObject, int aiMs, double adKoef);
	bool lookAt(ml3DPosition &aPos, int aiMs, double adKoef);
	bool lookAtByEyes(mlMedia* apObject, int aiMs, double adKoef);
	bool lookAtByEyes(ml3DPosition &aPos, int aiMs, double adKoef);
	bool lookAtByEyes(const wchar_t* apDest, int aiMs, double adKoef);
	bool addInterlocutor(mlICharacter* apCharacter);
	bool removeInterlocutor(mlICharacter* apCharacter);
	void setIndexedVisemes(int iFirstFomen, int iSecondFonem, float fMiddleFactor);
	void handleKinectState();

		// установить выражение лица с индексом auiIndex, взятое из apVisemes.
		// aiTime - период времени установки/сброса, 
		// aiKeepupTime - период времени, сколько "держать" это выражение на лице
	bool setViseme(mlMedia* apVisemes, unsigned int auiIndex, int aiTime=-1, int aiKeepupTime=-1);
		// убрать с лица выражение с индексом auiIndex, взятое из apVisemes.
		// aiTime - период времени "убирания"
	bool unsetViseme(mlMedia* apVisemes, unsigned int auiIndex, int aiTime=-1);
		// последовательно сменить все выражения лица, описанные в apVisemes, за период времени aiTime
	bool doVisemes(mlMedia* apVisemes, int aiTime=-1);
	// установить положение губ за указанное время
	bool setLexeme( unsigned int auiIndex, int aiTime=0);
		// установить персонажа в свободное место в области, заданной аргументами:
		// apStartPos - начальная позиция, adRadius - радиус области
		// abInRoom - поместить в пределах комнаты
	bool setToRandomFreePlace(ml3DPosition* apStartPos, double adRadius, bool abInRoom);
		// иди вперед
	bool goForward(unsigned int shiftTime = 0);
		// пяться
	bool goBackward(unsigned int shiftTime = 0);
		// иди боком влево
	bool strafeLeft(unsigned int shiftTime = 0);
		// иди боком вправо
	bool strafeRight(unsigned int shiftTime = 0);
		// перестань идти (как goTo(NULL))
	bool stopGo();
		// поворачивайся влево
	bool turnLeft(unsigned int shiftTime = 0);
		// поворачивайся вправо
	bool turnRight(unsigned int shiftTime = 0);
		// перестань поворачиваться
	bool stopTurn();
		// воспроизвести записанный звук
	bool sayRecordedVoice( LPCSTR alpcUserName, LPCSTR alpcFileName, LPCSTR alpcCommunicationAreaName, unsigned int auPosition, unsigned int auSyncVersion);
		// иди вперед и налево
	bool goLeft(unsigned int shiftTime);
		// иди вперед и направо
	bool goRight(unsigned int shiftTime);
		// иди назад и налево
	bool goBackLeft(unsigned int shiftTime);
		// иди назад и направо
	bool goBackRight(unsigned int shiftTime);
	bool getFaceVector(float& x, float& y, float& z);
	//	void doMotion(mlMedia* apMotion);
	//	void setMotion(mlMedia* apMotion, int aiDurMS);
	void goPath(mlMedia* apPath, int aiFlags);
	void goPath(ml3DPosition* apPath, int aiPointCount, unsigned int auShiftTime);
		// установка скорости движения объекта вдоль пути (%/сек)
	void setPathVelocity(float afVelocity);
		// установка котрольных точек (в %-ах пути), на которых будет срабатывать
		// onPassed. getPassedCheckPoint() вернет значение контрольной точки
	void setPathPercent(float afPercent);
	float getPathPercent();
	// показать на заданную точку или объект
	bool pointByHandTo(const ml3DPosition& aPos, mlMedia* apObject);

	// реализация  IVoipCharacter
public:
	void handleVoiceData( int nSampleRate, const short* pRawAudio, int nSamples);
	ml3DPosition getAbsolutePosition();
	ml3DRotation getAbsoluteRotation();	
	bool isSayingAudioFile(LPCSTR  alpcFileName);
	int m_FirstLexem;
	CRecognize m_Recognize;

public:
	bool attachSound(mlMedia* pAudio, ml3DPosition& pos3D);
	void OnSetRenderManager();
	void VisibleChanged();

	void PrepareForDestroy();
	CWalkController* GetWalkController()const;

	void SetTrackingRightHand(bool isEnabled);
	void SetTrackingLeftHand(bool isEnabled);

protected:
	void stopIdles();
	// Properties
	nrmVisemes*				visemes;		// Character visemes
	nrmAudio*				m_pLeftStepSound;	// Sound for left foot step
	nrmAudio*				m_pRightStepSound;	// Sound for right foot step
	//CSCloudCtrl*			cloud_ctrl;		// For "cloud text saying"
	//CEyesBlinkCtrl*			eyesblink_ctrl;	// Blink/Lips controller
	mlMedia*				mpSpeech;		// Speech object to say
	mlIIdles*				mpIdles;		// Current idles array
	byte					keys[255];
	//std::vector<IDLEDESC>	idlesDesc;
	// Methods
	void	SetIdles(mlMedia* apIdles);
	void	SetFaceIdles(mlIIdles* apIdles);
	void	ApplyFrozenToSpeech();
	
	// Event handlers
	//virtual	void	OnDoneAction(int mID);	// Overload because of idles routine

	void ShiftTime( unsigned int shiftTime);

private:
	DWORD	dwSpeechDuration;
	bool	bPlayEnd;
	bool	m_isAsynch;
	bool	m_isTrackingRightHandEnabled;
	bool	m_isTrackingLeftHandEnabled;
	bool	m_isEmptyBinState;
	MP_STRING	sCurrentVoiceFileName;
	MP_STRING	sVoiceUserName;

	CWalkController* m_walkController;	
};

#endif // !defined(AFX_nrmCharacter_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
