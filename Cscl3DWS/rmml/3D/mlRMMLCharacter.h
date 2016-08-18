#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLCharacter_H_
#define _mlRMMLCharacter_H_

namespace rmml {

#define EVNM_onTurned L"onTurned"
#define EVNM_onRightHandPosChanged L"onRightHandPosChanged"
#define EVNM_onLeftHandPosChanged L"onLeftHandPosChanged"

class mlRMMLMovement;
class mlRMMLVisemes;
class mlRMMLIdles;
class mlRMMLSpeech;
class mlRMMLShadows3D;
#define MLELPN_MOVEMENT "movement"
#define MLELPN_VISEMES "visemes"
#define MLELPN_IDLES "idles"
#define MLELPN_SHADOW "shadow"
#define MLELPN_MATERIALS "materials"

/**
 * Класс 3D-персонажей RMML
 */

class mlRMMLCharacter :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMML3DObject,
					public mlRMMLPButton,
					public mlICharacter,
					public moICharacter
{
	bool useZCoordinateSynchonization;
	bool unpickable;
	bool mbChildrenMLElsCreated;
	mlRMMLSpeech* mpSayingSpeech; // speech, который сейчас произносит персонаж
	short miSayingSpeechIdx; // для оптимизации проверки валидности mpSayingSpeech
	JSString* mjssMovement;
	JSString* mjssVisemes;
	JSString* mjssIdles;
	JSString* mjssFaceIdles;
public:
	mlRMMLCharacter(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLCharacter(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_movement = 1,
		JSPROP_visemes,
		JSPROP_idles,
		JSPROP_faceIdles, 
		JSPROP_shadow,
		JSPROP_materials,
		JSPROP_motion,
		JSPROP_speech, // speech/audio, который произносит персонаж в данный момент
		JSPROP_interlocutors, // массив собеседников 
		JSPROP_gravitation,
		JSPROP_intangible,
		JSPROP_unpickable,
		JSPROP_color,		// цвет, который добавляется ко всем материалам объекта в степени, заданной color.a
		JSPROP_opacity,		// степень непрозрачности объекта (0..0xFFFF)
		JSPROP_useZSynchronization,		// степень непрозрачности объекта (0..0xFFFF)
		EVID_onTurned,
		EVID_onSaid,
		EVID_onFalling,
		EVID_onFallAndStop,
		EVID_onWreck,
		EVID_onNeedToChangePosition,
		EVID_onRightHandPosChanged,
		EVID_onLeftHandPosChanged
	};
	enum {
		EVSM_onTurned = (1 << 0),
		EVSM_onSaid = (1 << 1),
		EVSM_onFalling = (1 << 2),
		EVSM_onFallAndStop = (1 << 3),
		EVSM_onWreck = (1 << 4),
		EVSM_onNeedToChangePosition = (1 << 8)
	};
	mlRMMLMovement* movement;
	mlRMMLVisemes* visemes;
	mlRMMLIdles* idles;
	mlRMMLIdles* faceIdles;
	JSFUNC_DECL(say)
	JSFUNC_DECL(goTo)
	JSFUNC_DECL(turnTo)
	JSFUNC_DECL(lookAt)
	JSFUNC_DECL(addInterlocutor) // добавить собеседника
	JSFUNC_DECL(removeInterlocutor) // удалить собеседника
	JSFUNC_DECL(lookAtByEyes) // посмотреть только глазами
	JSFUNC_DECL(setViseme) // установить выражение лица
	JSFUNC_DECL(unsetViseme) // убрать с лица определенное выражение
	JSFUNC_DECL(doVisemes) // последовательно сменить все выражения лица
	JSFUNC_DECL(setToRandomFreePlace) 
	JSFUNC_DECL(goForward) // иди вперед
	JSFUNC_DECL(goBackward) // пяться
	JSFUNC_DECL(strafeLeft) // иди боком влево
	JSFUNC_DECL(strafeRight) // иди боком вправо
	JSFUNC_DECL(stopGo) // перестань идти (как goTo(NULL))
	JSFUNC_DECL(turnLeft) // поворачивайся влево
	JSFUNC_DECL(turnRight) // поворачивайся вправо
	JSFUNC_DECL(stopTurn) // перестань поворачиваться
	JSFUNC_DECL(sayRecordedVoice) // проиграть записанный голос
	JSFUNC_DECL(setLexeme) // установить положение губ
	JSFUNC_DECL(goLeft) // иди вперед и налево
	JSFUNC_DECL(goRight) // иди вперед и направо
	JSFUNC_DECL(goBackLeft) // иди назад и налево
	JSFUNC_DECL(goBackRight) // иди назад и направо
	JSFUNC_DECL(getFaceVector)
	JSFUNC_DECL(toInitialPose) 
	JSFUNC_DECL(fromInitialPose) 
	JSFUNC_DECL(pointByHandTo) // показать на заданную точку или объект
	JSFUNC_DECL(setSleepingState) // установить спящее/неспящее положение
	JSFUNC_DECL(setRightHandTracking) // вкл/выкл отслеживание правой руки
	JSFUNC_DECL(setLeftHandTracking) // вкл/выкл отслеживание левой руки
	JSFUNC_DECL(setKinectDeskPoints)
	JSFUNC_DECL(disableKinectDesk)

private:
	mlRMMLElement* FindVisemes(jsval ajsvVisemes, const char* apcMethodName);
	void CorrectPositionWithSynchronization(ml3DPosition &aPos); 
	bool ZShouldBeSynched();

public:
	virtual void PosChanged(ml3DPosition &aPos); 
	void Said(mlRMMLSpeech* apSpeech);
	void IdlesSrcChanged(const wchar_t* apwcIdlesName, const wchar_t* apwcSrc);	// вызывается из mlRMMLIdles

	void SetMaterialTiling(int, float) {};
	void EnableShadows(bool isEnabled){};
	void ChangeCollisionLevel(int level){};

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	EventSpec* GetEventSpec2(){ return _EventSpec; } 
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult Update(const __int64 alTime);
	bool Freeze();
	bool Unfreeze();
	virtual bool NeedToClearRefs(){ return true; }
	virtual mlRMMLElement* GetElemToClearRefsIn(){ return (mlRMMLElement*)GetScene3D(); }
	virtual void ClearRefs(){ ClearRefs_mlRMML3DObject(); }
	// установить необходимые свойства в Event на кнопочные события
	virtual void SetButtonEventData(char aidEvent){ mlRMML3DObject::SetButtonEventData(aidEvent); }

// реализация mlILoadable
MLILOADABLE_IMPL

// реализация mlI3DObject
MLI3DOBJECT_IMPL
	bool GetBillboard(){ return false; }

// реализация  moI3DObject
	void BillboardChanged(){}	
	void UnpickableChanged();

// реализация mlIButton
MLIBUTTON_IMPL

// реализация mlICharacter
	mlICharacter* GetICharacter(){ return this; } 
	// Дочерние элементы
	mlMedia* GetCloud();
	// Свойства
	bool GetUnpickable();
	mlIMovement* GetMovement();
	mlMedia* GetVisemes();
	mlMedia* GetIdles();
	mlIIdles* GetFaceIdles();
	// События
	mlRMMLElement* GetElem_mlRMMLCharacter(){ return this; }
	void onTurned();
	void onSaid();
	void onFalling();
	void onFallAndStop();
	void onWreck();
	void onNeedToChangePosition();
	virtual void onRightHandPosChanged(ml3DPosition aRightHandPos, ml3DPosition aRightHandDir, 
		ml3DPosition aRightHandDirExt);
	virtual void onLeftHandPosChanged(ml3DPosition aLeftHandPos, ml3DPosition aLeftHandDir, 
		ml3DPosition aLeftHandDirExt);

	bool IsFullyLoaded() {return false;}
	bool IsIntersectedWithOtherObjects() {return false;}
	void LevelChanged(int /*level*/) {};
	int GetCurrentLevel() { return 0; }
	// Методы
	void SynchronizeZPosition();


// реализация moICharacter
public:
	// Свойства
	void MovementChanged();
	void VisemesChanged();
	void IdlesChanged();
	void FaceIdlesChanged();
	mlMedia* GetSpeech(); // speech/audio, который сейчас произносится
	mlICharacter** GetInterlocutors();
	// Методы
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
	bool setViseme(mlMedia* apVisemes, unsigned int auiIndex, int aiTime=-1, int aiKeepupTime=-1);
	bool setLexeme( unsigned int auiIndex, int aiTime=0);
	bool setSleepingState( bool abSleep);
	void SetTrackingRightHand( bool abTracking);
	void SetTrackingLeftHand( bool abTracking);
	bool unsetViseme(mlMedia* apVisemes, unsigned int auiIndex, int aiTime=-1);
	bool doVisemes(mlMedia* apVisemes, int aiTime=-1);
		// установить персонажа в свободное место в области, заданной 
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
	bool sayRecordedVoice( const char* alpcUserName, const char* alpcFileName, LPCSTR alpcCommunicationAreaName, unsigned int auPosition, unsigned int auSyncVersion);
	// иди вперед и налево
	bool goLeft(unsigned int shiftTime);
	// иди вперед и направо
	bool goRight(unsigned int shiftTime);
	// иди назад и налево
	bool goBackLeft(unsigned int shiftTime);
	// иди назад и направо
	bool goBackRight(unsigned int shiftTime);
	bool getFaceVector(float& x, float& y, float& z);
	bool setKinectDeskPoints(const ml3DPosition& aPos, const ml3DPosition& aPos2, const moMedia* moObj);
	void disableKinectDesk();

	void toInitialPose();
	void fromInitialPose();

	// показать на заданную точку или объект
	bool pointByHandTo(const ml3DPosition& aPos, mlMedia* apObject = NULL);
};


}

#endif
