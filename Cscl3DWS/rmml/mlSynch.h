#pragma once

#include "mlSynchState.h"

namespace rmml {


struct mlPropToSynch
{
	// пока будем сохранять только имена свойств
	MP_WSTRING msName; // имя свойства
public:
	mlPropToSynch(const wchar_t* apwcName):
		MP_WSTRING_INIT(msName)
	{
		msName = apwcName;
	}

	mlPropToSynch(const mlPropToSynch& other):
		MP_WSTRING_INIT(msName)
	{
		msName = other.msName;
	}
};

class mlPropsToSynch : public MP_VECTOR_DC<mlPropToSynch>
{
	typedef std::vector<mlPropToSynch> baseClass;
public: 
	mlPropsToSynch()
	{
		MP_VECTOR_INIT_THIS;
	}

	void AddProp(const wchar_t* apwcName);
	void Clear(){
		baseClass::clear();
	}
	typedef baseClass::iterator iterator;
	typedef baseClass::reference reference;
#if _MSC_VER >= 1500
	iterator begin(){ return baseClass::begin(); }
	iterator end(){ return baseClass::end(); }
#elif _MSC_VER > 1200
	iterator begin(){ return ((iterator)baseClass::begin()._Myptr); }
	iterator end(){ return ((iterator)baseClass::end()._Myptr); }
#else
	iterator begin(){ return ((iterator)baseClass::begin()); }
	iterator end(){ return ((iterator)baseClass::end()); }
#endif
	reference back(){ return (reference)baseClass::back(); }
	void pop_back(){
		baseClass::pop_back();
	}
};

class mlRMMLElement;

class mlSynchPropListeners{
	typedef MP_VECTOR<mlRMMLElement*> v_elems;
	typedef MP_VECTOR<mlRMMLElement*>::iterator v_elems_iter;
	typedef v_elems* pv_elems;
	struct mlSynchPropListener{
		MP_WSTRING sProp;
		v_elems vListeners;
		mlSynchPropListener():
			MP_VECTOR_INIT(vListeners),
			MP_WSTRING_INIT(sProp)
		{
		}
		mlSynchPropListener(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener):
			MP_VECTOR_INIT(vListeners),
			MP_WSTRING_INIT(sProp)
		{
			sProp = apwcSynchProp;
			vListeners.push_back(apMLElListener);
		}
		mlSynchPropListener(const mlSynchPropListener& oSrc):
			MP_VECTOR_INIT(vListeners),
			MP_WSTRING_INIT(sProp)
		{
			sProp = oSrc.sProp;
			vListeners = oSrc.vListeners;
		}
	};
	MP_VECTOR<mlSynchPropListener> mvListeners;
	typedef MP_VECTOR<mlSynchPropListener>::iterator iterListeners;
	bool mbChanged;
	//
public:
	mlSynchPropListeners():
		MP_VECTOR_INIT(mvListeners)
	{
		mbChanged=true;
	}
	~mlSynchPropListeners(){
	}
	void NotifySenderDestroyed(mlRMMLElement* apMLElSender);
	pv_elems Get(const wchar_t* apwcSynchProp);
	void Set(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener);
	void Remove(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener);
	bool IsChanged(){ return mbChanged; }
	void ClearChanged(){ mbChanged=false; }
};


/*
Вводятся системные (скрытые) синхронизируемые свойства.
Их имена начинаются на “__".
Системные свойства типизированы и контролируются как клиентской, так и серверной частями.
  __zone – (целое) идентификатор зоны, в которой находится объект
    Устанавливается по нотификации от MapManager-а
  __flags – (целое) специальные флаги
    0-й флаг – если установлен, то это объект с назначаемым сервером собственником
      устанавливается через свойство __type=2 в атрибуте synch
    1-й флаг – если установлен, то свойства может менять только собственник, если он установлен (если нет, то никто не может менять)
      устанавливается через свойство __type=1 в атрибуте synch
  __owner – (строка) собственник объекта (только он может изменять свойства)
    Пока логин пользователя.
    Может назначаться при создании через __owner=1 в атрибуте synch (если у объекта собственник еще не установлен, то автоматически собственником будет назначен клиент, создавший объект)
    Методы:
      bool synch.capture() – попытаться захватить объект в свою собственность
      bool synch.free() – освободить находящийся в собственности клиента объект
      bool synch.isMyOwn() – является ли объект собственностью клиента? (если да, то можно менять свойства и они будут синхронизированы)
      bool synch.isFree() – свободен ли объект? (собственник еще не установлен?)
	  bool synch.mayChange() - можно ли менять свойства объекта? (если да, то изменения с высокой вероятностью будут приняты сервером и изменены у других клиентов)
    Событие:
	  synch.onOwnerChange – сменился собственник объекта (или объект освободился)
	  synch.onMessage - пришло сообщение от другого клиента
	  synch.onInitialized - выполнены скрипты инициализации и установлено состояние из кэша
	                        (событие введено, чтобы можно было произвести дополнительную инициализацию, например, переместить объект в иное место, нежели то, в котором он был)
	  synch.onCommunicationAreaChange – сменилась зона общения
	  synch.onLocationChange – сменилась локация

  Один клиент мира может передать сообщение другому объекту. Это сообщение будет передано только клиенту-собственнику объекта.
  В скрипте того, кто шлет сообщение это будет выглядеть так:
	  otherOwnerObjec.synch.sendMessage(...);
  В скрипте принимающего у этого объекта будет сгенерировано событие
	  <script event="synch.onMessage">
		// Event.sender = строковый_идентификатор_клиента-отправителя
		// Event.args – массив аргументов, переданных при вызове sendMessage (ссылки на объекты тоже должны передаваться)
		...
	  </script>

надо сделать еще:
1) механизм сообщений (не отлажено)
2) onInitialized
*/


#define EVNM_onOwnerChange L"onOwnerChange"
#define EVNM_onMessage L"onMessage"
#define EVNM_onInitialized L"onInitialized"
#define EVNM_onReset L"onReset"
#define EVNM_onCommunicationAreaChange L"onCommunicationAreaChange"
#define EVNM_onLocationChange L"onLocationChange"

typedef MP_VECTOR<mlRMMLElement*> v_elems;

bool isPropertyOrObjectNameValid(unsigned char* str);


// MayChange error codes
#define MCERROR_NOERROR					0		// может править
#define MCERROR_OBJECTISCAPTURED		1		// не может править, потому что занят;
#define MCERROR_OBJECTISRECORDED		2		// не может править, потому что записан
#define MCERROR_OBJECTISNONCHANGABLE	3		// не может править, потому что это объект не вошедший в запись (например экран вне локации)
#define MCERROR_WRONGSYNCOBJECT			4		// не может править, потому что это неправильный объект. Это ASSERT

/**
 * JS-класс объектов synch RMML-элементов для 3-й модели синхронизации
 */
class mlSynch:
					public mlJSClass,
					public mlIBinSynch
{
private:
	bool isPaused;	// для отладки
	// серверная версия состояния объекта 
	unsigned short mwServerVersion;
	// клиентскаая версия состояния объекта 
	unsigned short mwClientVersion;

	mlRMMLElement* mpParentElem;
	mlSynch* mpParent;
	MP_WSTRING msName; // имя свойства подобъекта (когда mpParent != NULL)
	mlPropsToSynch mPropsToSynch; // список свойств со значениями, которые изменились и их надо будет синхронизировать
	mlSynchPropListeners mListeners;
	bool mbSynchronizing; // ==true, если идет процесс установки значений свойств в полученные с сервера
	bool mbSetting; // ==true, если идет процесс установки значения свойства
	wchar_t* mpwcPropName; // имя свойства, которое устанавливается
	jsval mjsvProp; // значение свойства, которое устанавливается
	//// флаги изменения свойств 
	// 0й - изменилась зона
	int miChanged;

	// Инициализация завершена
	bool isSynchInitialized;
	// режим инициализации через значение атрибута synch
	bool mbInitialization;
	//unsigned short mwVersion;
	mlSysSynchProps mSSPs;
	// идет установка отдельного свойства подобъекта при установке значения-объекта целиком
	// (если ==true, то при 3м подрежиме не надо записывать, что изменилось это свойство.
	//  достаточно будет записи об изменении всего подобъекта)
	bool mbSubPropSetting;

	// изменения бинарного состояния объекта для отсылки на сервер
	syncObjectBinState binStateForSending;

	// список синхронизируемых подобъектов элемента (в mlSynch для экономии памяти)
	v_elems* mpSynchedChildren; // значение 1 - нет синхронизируемых подобъектов
	// внешний идентификатор локации (может быть разным для одной и той же локации)
	int mOutLocationID;
	int mOutCommunicationAreaID;
	// Если элемент не инициализирован, то время последнего запроса полного состояяния с сервера
	__int64 fullStateQueryTime;

	// Если =true, но не синхронизировать, сохраняя изменения, до разрешения синхронизировать снова
	bool mbDontSynch;
	// Время последней установки свойств синхронизации 
	unsigned __int64 mSetPropLocalTime;
	// Тип следующего посылаемого на сервер состояния
	syncObjectState::StateType nextSendingStateType;
	bool mbWaitForLoading; // watch, Synched, Reset
public:
	mlSynch(void);
	void destroy() { MP_DELETE_THIS; }
	~mlSynch(void);
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlSynch)
	bool IsMyOwn();
	bool IsFree();
	bool IsRecorded();
	int  MayChange();
	void BeginHashEvaluating();
	void EndHashEvaluating();
	void ForceVersions();
	bool GetBinState( BYTE*& aBinState, int& aBinStateSize);
	void SetWaitForLoadingStatus();
	bool IsWaitForLoading();
	void ResetWaitForLoadingStatus();
private:
	enum { 
		JSPROP_serverTime=1
	};
	static JSBool array_length_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSBool array_length_setter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSBool serverTime_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	// bool synch.capture() – попытаться захватить объект в свою собственность
	JSFUNC_DECL(capture)
	// bool synch.free() – освободить находящийся в собственности клиента объект
	JSFUNC_DECL(free)
	// bool synch.isMyOwn() – является ли объект собственностью клиента? (если да, то можно менять свойства и они будут синхронизированы)
	JSFUNC_DECL(isMyOwn)
	// bool synch.isFree() – свободен ли объект? (собственник еще не установлен?)
	JSFUNC_DECL(isFree)
	// начинается изменение состояние объекта
	JSFUNC_DECL(settingStart)
	// изменение состояния объекта завершено
	JSFUNC_DECL(settingEnd)
	// bool synch.mayChange() - можно менять свойства объекта. 
	// (Изменения с высокой вероятностью будут приняты сервером и изменены у других клиентов)
	JSFUNC_DECL(mayChange)
	// фиксирует координаты размещения объекта в базе данных MapServer-а
	JSFUNC_DECL(fixPos)
	// удаляет объект из базы данных MapServer-а
	JSFUNC_DECL(remove)
	// посылает сообщение клиенту-собственнику объекта
	JSFUNC_DECL(sendMessage)
	// сделать запрос на сервер на предмет загрузки объекта по ссылке (Link)
	// JSFUNC_DECL(restoreObject)
	// возвращает идентификатор пользователя-собственника
	JSFUNC_DECL(getOwner)
	// возвращает идентификатор локации
	JSFUNC_DECL(getLocation)
	// возвращает время, когда объект будет удален сервером
	JSFUNC_DECL(getDeleteTime)
	// задать время, когда серверу надо будет удалить объект
	JSFUNC_DECL(setDeleteTime)
	JSFUNC_DECL(pause)
	JSFUNC_DECL(resume)
	// возвращает идентификатор локации
	JSFUNC_DECL(getCommunicationArea)
	// возвращает состояние binState пустой или нет
	JSFUNC_DECL(isEmptyBinState)


	//
	void CallListeners2(const wchar_t* apwcSynchProp);
	jsval GetEventJSVal(char aidEvent);
	void CallListeners(char aidEvent);
	//
	bool Free();
	bool IsSOAObject(){ return (mSSPs.miFlags & MLSYNCH_SERVER_OWNER_ASSIGNATION_MASK)?true:false; }
	bool IsOOSObject(){ return (mSSPs.miFlags & MLSYNCH_OWNER_ONLY_SYNCHR_MASK)?true:false; }
	mlSynch* GetTopParent();
	void CallEventFunction(const wchar_t* apwcEventName);
	void FireEvent(const wchar_t* apwcEventName);
	bool GetParentElementWithID(mlRMMLElement*& apParentElement);
	bool ParentElementIsWithID();
	bool GetSynchOfParentElementWithID(mlSynch*& apSynch);
	moIBinSynch* GetIBinSynch();

	// Задать системное свойство времени исчезновения объекта 
	// aulDeleteTime - в миллисекундах (относительно текущего времени)
	bool SetDeleteTime(unsigned __int64 aulDeleteTime);

	// получить (примерное) серверное время в миллисекундах
	unsigned __int64 GetServerTime();

	//// Получить значение системного свойства времени исчезновения объекта 

	// Получить разницу временных зон клиента и сервера 
	__int64 GetTimeZoneDelta();

	// Перевод значения из серверного времени в UTC в миллисекундах
	unsigned __int64 ServerTimeToLocalUTC(unsigned __int64 aulServerTime);

	// форматирование серверного времени в читабельный вид
	std::string FormatServerTime(unsigned __int64 aulServerTime);

// реализация mlIBinSynch
public:
	bool StateChanged(unsigned int auStateVersion, BSSCCode aCode, void* apData, unsigned long aulSize, 
		unsigned long aulOffset=0, unsigned long aulBlockSize=0);

public:
	bool IsObsoleteFullState( syncObjectState& aState);
	inline unsigned short GetServerVersion() { return mwServerVersion;}
	inline unsigned short GetClientVersion() { return mwClientVersion;}
	bool Capture();
	void SetSynchFlags(unsigned int auiFlags);
	unsigned int GetSynchFlags();
	mlSynchData oPropsToSynch;
	void SetParents(mlRMMLElement* apParentElem, mlSynch* apParent = NULL){
		mpParentElem = apParentElem;
		mpParent = apParent;
	}
	// Сбрасывает все свойства и системные, и обычные. Необходимо для поворного использованию. Исправление #477
	void Reset();
	// Сбрасывает обычные свойства и бинарное состояние
	void ResetState();
	// установить родительскому mlSynch, что этот подоъект изменился
	void SetChanged(const wchar_t* apwcPropName);
	// установить флаг изменения системного свойства
	__forceinline void SetChanged(int aiMask);

	void SetEventListener(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener){
		mListeners.Set(apwcSynchProp, apMLElListener);
	}

	void RemoveEventListener(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener){
		mListeners.Remove(apwcSynchProp, apMLElListener);
	}
	
	// получить изменившиеся значения свойств в новом формате
	bool GetSynchProps( syncObjectState& aoState, JSObject* &jsoSynched);
	// получить изменившиеся значения свойств подобъектов
	bool GetSynchSubObjProps(syncObjectState& aoState, JSObject* &jsoSynched, const std::wstring& awsPrefix);
	// Если инициализация завершена
	inline bool IsSynchInitialized() { return isSynchInitialized;};
	inline void ComleteInitialization() { isSynchInitialized = true;};
	inline bool ResetInitialized() { isSynchInitialized = false;};
	// Возвращает время последнего запроса полного состояяния с сервера
	inline __int64 GetFullStateQueryTime() { return fullStateQueryTime;}
	// Устанавливает время последнего запроса полного состояяния с сервера
	inline void SetFullStateQueryTime( const __int64 aTime) { fullStateQueryTime = aTime;}
	// Если были изменения в синхронизируемых свойствах
	bool IsSynchChanged();
	// форсировать отправку на сервер полного состояния
	void ForceWholeStateSending();
	// сэмулировать, что пришло полное состояние объекта
	void SimulateFullStateCome(JSObject* jsoSynched);
	// установить значения свойств в значения, полученные с сервера (в новом формате)
	ESynchStateError SetSynchProps( const syncObjectState& aState, JSObject* &jsoSynched, bool& aSynchedEmpty);
private:
	// установить значение свойства в значение, полученное с сервера
	bool SetSynchProp( const syncObjectProp& aProp);
	// установить системные свойства в значения, полученные из кэша
	ESynchStateError SetSysSynchProps(const mlSysSynchProps& apSysProps, bool bDontTrace);
	// Установить свойство в Event.synched
	void SetPropToSynchedJSO(JSObject* ajsoSynched, const char* apcPropName);
public:
	// Установить режим инициализации (для инициализации через выполнение значения атрибута synch)
	void SetInitializationMode(bool abMode){ mbInitialization = abMode;	}
	// объект переместился из одной зоны в другую (вызывается MapManager-ом)
	void ZoneChanged(int aiZone);
	// объект перешел в другую реальность (вызывается MapManager-ом)
	void RealityChanged(unsigned int auReality);
	// z
	float GetZ();
	// x
	float GetX();
	// y
	float GetY();

	void SetXY(float aX, float aY);
	void SetZ(float aZ);
	// пришло значение z аватара другого пользователя и надо его установить
	bool NeedToSetZ();
	// с сервера пришло сообщение от другого клиента
	void HandleSynchMessage(const wchar_t* apwcSender, mlSynchData& aData);
	// почистить mPropsToSynch (например, для того, чтобы не отсылать 0-е состояие, потому что есть состояние, полученное с сервера)
	void DontSynchLastChanges();
	// сгенерировать onInitialized
	void FireOnInitialized();
	// сгенерировать onReset
	void FireOnReset();
	// Применить обновление бинарного состояния
	void ApplyBinStateUpdating(const syncObjectBinState& aBinStateUpdating);
	// Применить полное бинарное состояние
	void ApplyFullBinState();
	// сделать запрос на сервер на предмет загрузки объекта по ссылке
	// void RestoreObject(mlSynchLinkJSO* apLink);
	// устанавливает флаг, что изменилось синхронизируемое состояние одного из подобъектов
	__forceinline void SetSubobjChanged() {
		miChanged |= MLSYNCH_SUB_OBJ_CHANGED_MASK;
	}
	__forceinline bool IsSubObjSynchChanged(){ return (miChanged & MLSYNCH_SUB_OBJ_CHANGED_MASK)?true:false; }
	__forceinline v_elems* GetSynchedChildren(){ return mpSynchedChildren; }
	v_elems* CreateSynchedChildren();
	#define ML_SYNCH_CHILDREN_EMPTY_VAL ((v_elems*)1)
	__forceinline void SetSynchedChildrenEmpty(){ mpSynchedChildren = ML_SYNCH_CHILDREN_EMPTY_VAL; }
	// 3D-объект сместился и получена его локация (возможно она и не изменилась)
	void SetLocationSID(const wchar_t* pszLocationID);
	void SetCommunicationAreaSID(const wchar_t* pszCommunicationAreaID);	
	// Приостановить синхронизацию объекта (но сохранять все изменения до разрешения синхронизировать дальше)
	void DontSynchAwhile(){ mbDontSynch = true; }
	// Возобновить синхронизацию 
	void RecommenceSynchronization(){ mbDontSynch = false; }
	// синхронизация объекта приостановлена
	bool DontSynch(){ return mbDontSynch; }
	// Возвращает время последнего изменения. Исправление #414
	inline unsigned __int64 GetLastChangingTime() { return mSetPropLocalTime;};
	// Возвращает системные свойства в json-формате
	mlString GetSysPropertiesJson( int aIndent, const wchar_t* aIdentStr);
	// 
	bool IsApplyableFullState( const syncObjectState& aState);

	bool IsHashEvaluating(){ return mbHashEvaluating;}

private:
	bool			mbHashEvaluating;
	// push state variables
	mlPropsToSynch	mPushPropsToSynch;
	int				mPushMiChanged;
	syncObjectState::StateType mPushNextSendingStateType;
};


}
