#pragma once

#include "mlSynch.h"

namespace rmml {

// имена свойств
#define MLELPN_PARENT "_parent"
#define MLELPNW_PARENT L"_parent"
#define MLELPNA_PARENT "parent"
#define MLELPNAW_PARENT L"parent"
#define MLELPN_CLASS "_class"
#define MLELPNA_CLASS "class"
#define MLELPNAW_CLASS L"class"
#define MLELPN_NAME "_name"
#define MLELPN_SCRIPTPROVIDERS L"scriptProviders"
#define MLELPN_CLASSCOMMON "classCommon"
	
#define SNCMAG_FUNC ((char)0x7F)
#define SNCMAG_SERIES ((char)0x7E)
#define SNCMAG_STATES ((char)0x7D)

// синхронизация объекта
#define SNCOT_ELEM	((char)4)
// синхронизация свойства
#define SNCOT_PROP	((char)6)
// синхронизация метода
#define SNCOT_FUN	((char)7)
// специальная синхронизация медиа-объекта
// (внутреннюю структуру знает только сам медиа-объект)
#define SNCOT_MOS	((char)8)

typedef MP_VECTOR<mlRMMLElement*> v_elems;
typedef MP_VECTOR<mlRMMLElement*>::iterator v_elems_iter;
typedef v_elems* pv_elems;
typedef MP_VECTOR<moMedia*> v_mos;

//typedef v_elems[] a_listeners;

__forceinline const wchar_t* findDot(const wchar_t* apwc){
	while(*apwc!=L'\0'){
		if(*apwc==L'.') return apwc;
		apwc++;
	}
	return NULL;
}

__forceinline const wchar_t* findLastDot(const wchar_t* apwc){
	wchar_t* pwcLastDot=NULL;
	while(*apwc!=L'\0'){
		if(*apwc==L'.') pwcLastDot=(wchar_t*)apwc;
		apwc++;
	}
	return pwcLastDot;
}

struct EventSender{
	mlRMMLElement* pSender;
	char idEvent;
	EventSender()
	{
		pSender = NULL;
		idEvent = 0;
	}
	EventSender(char aIDEvent, mlRMMLElement* apSender){
		pSender = apSender;
		idEvent = aIDEvent;
	}
//	EventSender& operator=(const EventSender& aevs){
//		pSender = aevs.pSender;
//		idEvent = aevs.idEvent;
//		return *this;
//	}
};

struct SynchPropEventSender{
	mlRMMLElement* pSender;
	MP_WSTRING msSynchProp;

	SynchPropEventSender(const wchar_t* apwcSynchProp, mlRMMLElement* apSender):
		MP_WSTRING_INIT(msSynchProp)
	{
		pSender = apSender;
		msSynchProp = apwcSynchProp;
	}

	SynchPropEventSender(const SynchPropEventSender& other):
		MP_WSTRING_INIT(msSynchProp)
	{
		pSender = other.pSender;
		msSynchProp = other.msSynchProp;
	}
};

struct SynchEventSender{
	mlRMMLElement* pSender;
	long lValue;
	enum VType{
		ms,
		sec,
		frame
	}eValueType;
};

struct mlSynchedProp
{
	unsigned short usNameLen;
	char* pcName;
	unsigned short usValueLength;
	int iValuePos;
};

struct mlSynchedObjProps
{
	unsigned int uObjID;
	unsigned int uBornRealityID; // идентификатор реальности рождения
	MP_STRING sSubobjPath;	// путь к подобъекту
	mlSysSynchProps oSysProps;
	MP_VECTOR<mlSynchedProp> vProps;

	mlSynchedObjProps();
	mlSynchedObjProps(const mlSynchedObjProps& other);
};

typedef MP_VECTOR<mlSynchedObjProps> VecSynchedObjProps;

class mlPropertyValues{
public:
	struct mlPropertyValue{
		const char* name;
		bool copy;
		jsval val;
	};
	typedef MP_VECTOR<mlPropertyValue*> mlPropValVec;
	mlPropValVec mvPropVals;

public:
	mlPropertyValues():
		MP_VECTOR_INIT(mvPropVals)
	{
	}
	~mlPropertyValues();
	mlPropertyValue* AddNew();
	mlPropertyValue* Find(const char* alpcName) const;
};

class mlRMMLVisible;

#define ML_EVENT_TEXTURE_OBJECT_PROP_NAME "textureObject"
#define ML_EVENT_TEXTURE_X_PROP_NAME "textureX"
#define ML_EVENT_TEXTURE_Y_PROP_NAME "textureY"

struct mlButtonUnderMouse
{
	// mlRMMLElement* mpButUnderMouse; // объект под мышкой
	mlRMMLVisible* mpVisible; // видимый объект под мышкой (если mpButUnderMouse - 3D-объект, то будет Viewport-ом)
	ml3DPosition mXYZ; // координаты пересечения 
	mlRMMLElement* mp3DObject; // 3D-объект, который под мышкой
	mlRMMLElement* mpTextureObject; // RMML-объект-текстура, которая под мышкой
	mlPoint mLocalXY; // координаты мыши в масштабах объекта-текстуры

	mlButtonUnderMouse(){
		Reset();
	}
	void Reset(){
		// mpButUnderMouse = NULL;
		mpVisible = NULL;
		ML_INIT_3DPOSITION(mXYZ);
		mp3DObject = NULL;
		mpTextureObject = NULL;
		ML_INIT_POINT(mLocalXY);
	}
};

typedef MP_VECTOR<unsigned char> ByteVector;

#define EVNM_onSynched L"onSynched"
class RMML_NO_VTABLE mlRMMLElement: 
									// public mlJSClassProto,
									public mlMedia
{
friend class mlSceneManager;
friend class mlRMMLBuilder;
friend class mlRMMLIterator;
friend class mlRMMLLoadable;
friend class mlRMMLVisible;
friend class mlRMMLPButton;
friend class mlRMMLContinuous;
friend class mlRMML3DObject;
friend class mlRMMLScript;
friend class mlRMMLText;
friend class mlRMMLSequencer;
friend class mlRMMLMap;
friend class mlSynch;
friend class mlSynchLinkJSO;
#ifdef RMML_DEBUG
friend class rmmld::mldRMMLDebugger;
#endif
public:
	JSContext* mcx;
	union{
		JSObject* mjso;
		JSScript* mjss;
//		JSFunction* mjss;
	};
	unsigned short mRMMLType;
	v_elems* mpChildren;
	v_elems* mpvNonJSOChildren;
private:
	class RMML_NO_VTABLE mlListeners{
		char* maIDs;
		pv_elems* maListeners;
		int miSize;
		bool mbChanged;
		//
		__forceinline int GetIdx(char aidEvent){
			char* pId=maIDs;
			for(int ii=0; ii<miSize; ii++){
				if((*pId++)==aidEvent) return ii;
			}
			return -1;
		}
	public:
		mlListeners(EventSpec* apEventSpec){
			miSize=0;
			mbChanged=true;
			EventSpec* pEventSpec=apEventSpec;
			while(pEventSpec->id){ miSize++; pEventSpec++; }
			if(miSize==0) return;
			maIDs = MP_NEW_ARR( char, miSize);
			maListeners = MP_NEW_ARR( pv_elems, miSize);
			pEventSpec=apEventSpec;
			for(int ii=0; ii<miSize; ii++){
				maIDs[ii]=pEventSpec->id;
				maListeners[ii]=NULL;
				pEventSpec++;
			}
//			sortchars(maIDs, &(maIDs[miSize-1]));
		}
		~mlListeners(){
			if(miSize==0) return;
			for(int ii=0; ii<miSize; ii++){
				pv_elems pv=maListeners[ii];
				if(pv!=NULL){
					if(maIDs[ii] < 0) //  delete user event ID
					{
						wchar_t* p = (wchar_t*)(*pv->begin());
						MP_DELETE_ARR( p);
					}
					pv->clear();
					 MP_DELETE( pv);
				}
			}
			MP_DELETE_ARR( maListeners);
			MP_DELETE_ARR( maIDs);
		}
		void RemoveAllListeners(){
			if(miSize==0) return;
			for(int ii=0; ii<miSize; ii++){
				pv_elems pv=maListeners[ii];
				if(pv!=NULL){
					if(maIDs[ii] < 0) //  delete user event ID
					{
						v_elems::iterator vi = pv->begin();
						vi++;
						if(vi != pv->end())
							pv->erase(vi, pv->end()); // название события оставляем
					}else{
						pv->clear();
					}
				}
			}
		}
		void NotifySenderDestroyed(mlRMMLElement* apMLElSender){
			for(int ii=0; ii<miSize; ii++){
				pv_elems pv=maListeners[ii];
				if(pv!=NULL){
					v_elems::iterator vi = pv->begin();
					if(maIDs[ii] < 0) vi++; // skip user event ID
					for(; vi!=pv->end(); vi++){
						mlRMMLElement* pMLEl=*vi;
						pMLEl->EventSenderDestroyed(apMLElSender);
					}
				}
			}
		}
		pv_elems Get(char aidEvent){
			int iIdx=GetIdx(aidEvent);
			if(iIdx<0) return NULL;
			return maListeners[iIdx];
		}
		void Set(char aidEvent, mlRMMLElement* apMLElListener){
			int iIdx = GetIdx(aidEvent);
			if(iIdx < 0) return;
			pv_elems pv = maListeners[iIdx];
			if(pv == NULL){
				MP_NEW_VECTOR_V(pv, v_elems);
				if (maListeners[iIdx] != NULL)
					MP_DELETE(maListeners[iIdx]);
				maListeners[iIdx] = pv;
				pv->push_back(apMLElListener);
			}else{
				v_elems::iterator vi = pv->begin();
				for(; vi!=pv->end(); vi++){
					mlRMMLElement* pMLEl=*vi;
					if(pMLEl == apMLElListener)
						break;
				}
				if(vi==pv->end())
					pv->push_back(apMLElListener);
			}
			mbChanged=true;
		}
		void Remove(char aidEvent, mlRMMLElement* apMLElListener){
			int iIdx=GetIdx(aidEvent);
			if(iIdx<0) return;
			pv_elems pv=maListeners[iIdx];
			if(pv==NULL) return;
			v_elems::iterator vi;
			for(vi=pv->begin(); vi!=pv->end(); vi++){
				if(*vi==apMLElListener){
					pv->erase(vi);
					if(pv->empty()){
						MP_DELETE( pv);
						maListeners[iIdx]=NULL;
					}
					mbChanged=true;
					return;
				}
			}
		}
		char AddUserEvent(const wchar_t* apwcUserEvent);
		char GetUserEventID(const wchar_t* apwcIDName);
		const wchar_t* GetUserEventName(char aidEvent);
		mlString GetUserEvents();
		bool IsChanged(){ return mbChanged; }
		void ClearChanged(){ mbChanged=false; }
	};
	std::auto_ptr<mlListeners> mpListeners;
public:
	union{
		// =true, если хотя бы для одного из кнопочных собыий 
		// зарегистрирован хотя бы один слушатель
		bool mbBtnEvListenersIsSet;
		// для скрипта: если указан src, то от сохраняется в mjss 
		// как ссылка на строку и скрипт должен быть загружен и откомпилирован 
		// при выполнении или до установки value 
		bool mbNeedToLoadScript;
	};
protected:
	short miSrcFilesIdx;
	const wchar_t* mpSrcFilePath; // !!! только в качестве инфы при отладке
	short miSrcLine;
public:
	mlRMMLElement(void);
	virtual void destroy() = 0;
	virtual ~mlRMMLElement(void);

public:
	void SetJSContext(JSContext* cx);
	bool IsComposition(){ return ((mRMMLType&0xFF)>=250); }
	bool IsScene(){ return mRMMLType==MLMT_SCENE; }
	bool IsJSO(){ return (mjso!=NULL && mRMMLType!=MLMT_SCRIPT); }
	bool IsOnloadScript(){ return (mType==MLMT_SCRIPT || mType==(MLST_SYNCH | MLSTF_NOTSET)); }
	void RemoveChild(mlRMMLElement* pChild);
	void ChildIsDestroying(mlRMMLElement* pChild);
	void AddChild(mlRMMLElement* pNewChild);
	mlRMMLElement* GetChild(const wchar_t* apwcName, int aiNameLen=-1);
	mlRMMLElement* GetChild(JSString* ajssName){
		return GetChild( (const wchar_t*)JS_GetStringChars(ajssName),JS_GetStringLength(ajssName));
	}
	mlRMMLElement* GetChild(unsigned short aRMMLType, unsigned short aNum=0);
	mlRMMLElement* GetChildByIdx(unsigned short auNum);
	mlRMMLElement* FindChildByLastNamed(const wchar_t* apwcName, int aiShift);
	bool IsChild(mlRMMLElement* apMLEl);
	short GetChildIdx(mlRMMLElement* apMLEl);
	wchar_t* GetName();
	mlRMMLElement* FindElemByName(const wchar_t* apwcName);
	mlRMMLElement* FindElemByName(JSString* ajssName){
		if(ajssName == NULL) return NULL;
		return FindElemByName((const wchar_t*)JS_GetStringChars(ajssName));
	}
	mlRMMLElement* FindElemByName2(const wchar_t* apwcName);
	mlRMMLElement* FindElemByName2(JSString* ajssName){
		if(ajssName == NULL) return NULL;
		return FindElemByName2((const wchar_t*)JS_GetStringChars(ajssName));
	}
	//
	const wchar_t* GetEventName(char acID);
	jsval GetEventJSVal(char aidEvent);
	void SetEventJSVal(char aidEvent, jsval aValue);
	void SetEventListener(char aidEvent, mlRMMLElement* apMLElListener);
	void RemoveEventListener(char aidEvent, mlRMMLElement* apMLElListener);
	// Сообщить о событии всем слушателям-скриптам,
	// в том числе вызвать JS-функцию onXXX.
	// abAllEvents=true, если элемент является слушателем всех 
	// событий мыши или клавиатуры
	void CallListeners(char aidEvent, bool abAllEvents=false, bool abProp=false);
	EventSender* GetEventSender(const wchar_t* apwcEvent);
	mlRMMLElement* FindElemByStringID(wchar_t* apwcStringID);
	mlresult CallFunction(mlSynchData &aData);
	void MakeAllPropsEnumerable(bool abRecurse = false);
	mlRMMLElement* GetClass(){ return mpClass; }
	inline bool IsAvatar() const { return (muID >= OBJ_ID_AVATAR && muID < OBJ_ID_CLASS);}
	inline bool IsEditable() const { return (muID >= OBJ_ID_EDITABLE && muID < OBJ_ID_AVATAR);}
	inline bool IsStaticObject() const { return (muID < OBJ_ID_EDITABLE);}

	inline void SetSrcFileIdx(short aSrcFilesIdx, short aiSrcLine);

	static bool IsNameValid(JSString* ajssName);
	// получить список всех родительских композиций, у которых надо вызывать кнопочные обработчики
	v_elems* GetButtonParents();
private:
	enum {JSPROP0_mlRMMLElement=TIDB_mlRMMLElement-1,
		JSPROP__name,
		JSPROP__class,
		JSPROP__parent,
		JSPROP__synch,
		JSPROP__frozen,
		JSPROP__target,
		JSPROP_userEvents,
		JSPROP_scriptProviders,
		TIDE_mlRMMLElement
	};
	enum { EVID_mlRMMLElement=TIDE_mlRMMLElement-1,
		EVID_onSynched,
		TEVIDE_mlRMMLElement
	};
	JSString* _name;
	bool _frozen;
	bool _updateBinState;
//#ifdef _DEBUG
wchar_t* mpName;
//#endif
public:
	mlRMMLElement* mpParent;
protected:
	mlRMMLElement* mpClass;

	MLJSPROTO_DECL

	JSFUNC_DECL(duplicate)
	JSFUNC_DECL(synchronize)
	JSFUNC_DECL(fireEvent)
	JSFUNC_DECL(getSubclass)
	JSFUNC_DECL(getChildren)
	JSFUNC_DECL(synch_fixPos)
	JSFUNC_DECL(synch_remove)
	JSFUNC_DECL(getBornReality)
	// Создает mlRMMLScript, который будет выполняться при возникновении определенного события.
	// Аналог декларативного способа <script event="событие">скрипт<script>.
	// bool createEventScript(event, script);
	JSFUNC_DECL(createEventScript)
	// перебирает все дочерние элементы первого уровня 
	// и вызывает для кождого из них функцию, переданную в качестве аргумента
	JSFUNC_DECL(applyToChildren)
	// Запрашивает полное состояние объекта
	JSFUNC_DECL(queryFullState)
	JSFUNC_DECL(getServerVersion)

	mlresult SetUserProperties(const wchar_t* apwcUserProperties);
	JSObject* FindClass(const wchar_t* apwcClassName);
	mlresult SetClass(JSString* ajssClass);
	mlresult SetClass(JSObject* ajsoClass);
	mlresult InitFromClass();
	mlresult GetPropsAndChildrenCopyFrom(mlRMMLElement* apMLEl, bool abInitFromClass=false);
	mlresult SetParent(mlRMMLElement* apParent);
	mlRMMLElement* GetParent(){ return mpParent; }
	mlMedia* GetParentElement(){ return (mlMedia*)mpParent; }
	struct mlCreateAndLoadMOsInfo{
		int miToLoad;
		int miTimeQuota;
		int miLoaded;
		mlCreateAndLoadMOsInfo(int aiToLoad = -1, int aiTimeQuota = 0): miToLoad(aiToLoad), miTimeQuota(aiTimeQuota){
			miLoaded = 0;
		}
	};
	mlresult CreateAndLoadMOs(mlCreateAndLoadMOsInfo* apCLMOInfo = NULL);
	mlresult ActivateScripts();
	mlresult AllParentComposLoaded();
	static void PropIsSet(mlRMMLElement* apThis, char idProp);
	static int CheckUserEvent(JSString* ajssEventID, mlRMMLElement* apThis, jsval *vp);
	bool IsFrozen(){ return _frozen; }
	bool IsForcedUpdateBinState();
	void SetForcedUpdateBinState(bool updateBinState);
	bool FreezeChildren();
	bool UnfreezeChildren();
	bool GetPropertiesInfo(mlSynchData &aData, bool abSkipROProps = false);
	bool GetTypeEventsInfo(mlSynchData &aData);
	void GetAllProperties(mlPropertyValues &avPropVals);
	bool Get3DPropertiesInfo(mlSynchData &aData);
	std::string Get3DProperty(char* apszName,char* apszSubName);
	mlresult Set3DProperty(char* apszName,char* apszSubName,char* apszVal);
	mlString GetSrcFileRef();
	mlString GetElemRefName(mlRMMLElement* apElem);
	void PutForSynch(mlSynchData &aData, bool abProps=true, bool abMedia=true);
	void GetAndSynch(mlSynchData &aData);
	static void PutJSValForSynch(JSContext *cx, jsval vProp, mlSynchData &oData);
	static jsval GetJSValForSynch(JSContext *cx, mlSynchData &oData);
	static mlString GetSJSValForSynch(JSContext *cx, mlSynchData &oData);
	void RelinkClassFunction(jsval &vFunc, const wchar_t* apwcFuncName);
	// const wchar_t* GetFullClassName(mlRMMLElement* apMLElClass);
	void SynchronizeProperty(mlSynchData &aData);
	bool realSynchronize(mlSynchData* apData); // synchronize(mlSynchData* apData=NULL) после кэширования
	// объект точно будет удален, поэтому уже гарантировано можно уничтожать его MO (и дочерних элементов)
	void ForceMODestroy(v_mos *apMOs = NULL);
	// Когда объект исключается из иерархии сцены (parent=null), то надо, чтобы он перестал обрабатывать и события
	virtual void UnlinkFromEventSenders(bool abWithChildren = true);
	// обработка уведомления от mlSynch о том, что изменилось какое-то свойство
	void SetSynchChanged();
	// устанавливает флаг, сигнализирующий, что этот элемент является классом, а не частью сцены, например
	void SetClassFlag(){ muID = OBJ_ID_CLASS; }
	// возвращает true, если элемент является частью класса
	bool IsClassChildren();
	// Время последнего изменения синхронизируемых свойств
	unsigned __int64 GetSynchChangingTime();

public:
	unsigned int mCountId;

public:
	// флаги для режима синхронизации 2 
	// (в синхронизируемой ли цепочке был вызван метод запуска длительной операции)
	unsigned char mucSynchFlags4Events; 
protected:
	unsigned int muLastEvaluatedHash;
	bool mbNeedHashEvaluating;
	mlSynch* mpSynch; // для режима синхронизации 3
	__int64 lLastUpdateTime;
	mlresult UpdateSynchZ(const __int64 alTime, bool aForce = false);
	mlresult UpdateSynchXY();
private:
	char mchSynchronized;
	bool mbMayHaveSynchChildren; // может иметь синхронизируемые дочерние элементы (флаг введен для опитимизации получения состояние)
	unsigned int muID;
	unsigned int muBornRealityID;
	unsigned int muRealityID;
	mlCallback* mpCreationCallback;
	int GetChildNum(mlRMMLElement* apChild);
	typedef std::vector<const char*> mlPropNames;
	typedef std::vector<const char*>::iterator mlPropNamesIter;
	int SavePropsInfo(mlSynchData &aData, JSPropertySpec* apPropSpec, mlPropNames &aPropNames, bool abSkipROProps);
	typedef std::vector<const wchar_t*> mlEventNames;
	typedef std::vector<const wchar_t*>::iterator mlEventNamesIter;
	int SaveEventsInfo(mlSynchData &aData, EventSpec* apPropSpec, mlEventNames &aEventNames);
	mlString JSValToString(jsval v, char &acMLType);
	void ParseUserEvents(const wchar_t* apwcValue);
	char GetUserEventID(const wchar_t* apwcIDName){
		if(mpListeners.get() != NULL)
			return mpListeners->GetUserEventID(apwcIDName);
		return 0;
	}
	mlString GetUserEvents();
	void ParseScriptProviders(const wchar_t* apwcValue);
	void ImportScriptFromProvider(const wchar_t* apwcClassName);
	bool PropValueIsSynchronizable(jsval vProp);
	bool CreateSynchObject(JSString* ajssSynch);
	bool IsSynchChanged();
	v_elems* GetSynchedChildren();
	void CallFrozenChangedJSFunc();
	void ResetSynchState();
	void ResetSynchStateChildren();
	bool GetFullStateForHash( syncObjectState& aStateFull);
	mlRMMLElement* GetFirstElementInSynchHirarchy();
	bool IsReadyForSynch();
	void SimulateFullStateCome(bool abSynchRoot);
	bool IsRootReadyForSynch();
public:
	mlString GetFullPath();
	bool NeedToSynchronize();
	unsigned int GetID(bool abGetIfUndefined = true);
	inline unsigned int GetBornRealityID() const { return muBornRealityID;}
	inline void SetBornRealityID( unsigned int aBornRealityID) { muBornRealityID = aBornRealityID;}
	inline unsigned int GetRealityID() const { return muRealityID;}
	inline unsigned int GetServerVersion() const { return (mpSynch != NULL ? mpSynch->GetServerVersion() : 0);}
	inline unsigned int GetClientVersion() const { return (mpSynch != NULL ? mpSynch->GetClientVersion() : 0);}
	inline unsigned int GetLastEvaluatedHash() const { return muLastEvaluatedHash;}
	mlString GetStringID();
	void synchronize(mlSynchData &aData);
	bool SynchronizeProp(jsval id, jsval *vp);
	void PutPropsForSynch(mlSynchData &aData, const wchar_t* apwcPropList, unsigned char &aiNObjs);
	bool MayHaveSynchChildren(){ return mbMayHaveSynchChildren;	}
	// для режима синхронизации 3
	SynchPropEventSender* GetSynchPropEventSender(const wchar_t* apwcEvent);
	void SetSynchPropEventListener(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener);
	void RemoveSynchPropEventListener(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener);
	bool GetSynchProps( syncObjectState& aoState);
	bool GetForcedState( syncObjectState& aoState);
	unsigned int GetObjectStateHash();
	
	
	void ResetSynch();
	void ResetSynchedChildren();
	void ForceWholeStateSending();
	bool IsSynchronized();
	bool IsSynchInitialized();
	inline __int64 GetFullStateQueryTime() { return (mpSynch == NULL ? 0 : mpSynch->GetFullStateQueryTime());}
	inline void SetFullStateQueryTime(const __int64 aTime) { if (mpSynch != NULL) mpSynch->SetFullStateQueryTime( aTime);}
	void ComleteInitialization();
	bool IsExclusiveOwn();
	ESynchStateError SetSynchProps( const syncObjectState& aState);
	void FinishInitialization( jsval jsv, bool abForce = false);
	static void SkipSynchProps(mlSynchData& oData);
	void SetCreationCallback(mlCallback* apCallback){ mpCreationCallback = apCallback; }
	bool Capture();
	void SetSynchFlags(unsigned int auiFlags);
	unsigned int GetSynchFlags();
	void HandleSynchMessage(const wchar_t* apwcSender, mlSynchData& aData);
	void DontSynchLastChanges();
	bool IsMyOwner();
	void FireSynchOnInitialized();
	void SetLocationSID(const wchar_t* pszLocationID);
	void SetCommunicationAreaSID(const wchar_t* pszCommunicationAreaID);	
	// Приостановить синхронизацию объекта (но сохранять все изменения до разрешения синхронизировать дальше)
	void DontSynchAwhile();
	// Возобновить синхронизацию 
	void RecommenceSynchronization();
	// вызывается, когда на окно плеера перетащили файл
	// и при этом мышь была над этим объектом
	bool OnFileDrop(const wchar_t* apwcFilePath);
	void OnLoadStateChanged();
	void ResetSynchOnlyFireEvent();
	void FireOnSynched(JSObject* jsoSynched, bool bFullState);
public:
//	virtual mlRMMLElement* GetRMMLElement()=0;
	virtual mlString GetProperty(const wchar_t* apwcName){
		if(!IsJSO()) return L"";
		jsval vProp;
		if(!JS_GetUCProperty(mcx,mjso,(const jschar*)apwcName,-1,&vProp)) return L"";
		char cPropType;
		return JSValToString(vProp,cPropType);
	}
	virtual mlresult SetProperty(const wchar_t* apwcName, const wchar_t* apwcValue){ 
		if(mjso==NULL) return ML_ERROR_NOT_INITIALIZED;
		jsval v;
		JSString* sVal=JS_NewUCStringCopyZ(mcx,(const jschar*)apwcValue); // ??
		v=STRING_TO_JSVAL(sVal);
		JS_SetUCProperty(mcx, mjso, (const jschar*)apwcName, wcslen(apwcName), &v);
		return ML_OK; 
	}
	virtual mlresult SetValue(wchar_t* apwcValue){ return ML_ERROR_NOT_IMPLEMENTED; }
	virtual mlresult CreateMedia(omsContext* amcx){ return ML_ERROR_NOT_IMPLEMENTED; }
	virtual mlresult Load(){ return ML_ERROR_NOT_IMPLEMENTED; }
	virtual mlRMMLLoadable* GetLoadable(){ return NULL; }
	virtual mlRMMLVisible* GetVisible(){ return NULL; }
	virtual mlRMMLPButton* GetButton(){ return NULL; }
	virtual mlRMMLContinuous* GetContinuous(){ return NULL; }
	virtual mlRMML3DObject* Get3DObject(){ return NULL; }
//	virtual bool NeedToBeDeleted(){ return false; }
//	virtual mlresult GetPropsFrom(mlRMMLElement* apElSrc){ mpClass=apElSrc->mpClass; return ML_OK; }
	virtual mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	virtual void DuplicateMedia(mlRMMLElement* apSource)
	{
    }
	virtual mlresult Update(const __int64 alTime);
	virtual void ChildAdded(mlRMMLElement* apNewChild){}
	virtual void ChildRemoving(mlRMMLElement* apChild){}
	virtual char GetEventID(wchar_t* apwcIDName){ return 0; }
	virtual EventSpec* GetEventSpec(){ return NULL; }
	// if EventNotify return true then remove the listener from the list
	virtual bool EventNotify(char aidEvent, mlRMMLElement* apSender){ return false; }
	virtual void EventSenderDestroyed(mlRMMLElement* apSender){ return; }
	virtual void YouAreSetAsEventListener(char aidEvent, mlRMMLElement* apSender){ return; }
	virtual bool SetEventData(char aidEvent, mlSynchData &Data);
	virtual bool IsPointIn(mlPoint &aPnt);
	virtual mlRMMLElement* GetButtonUnderMouse(mlPoint &aMouseXY, mlButtonUnderMouse &aBUM);
	virtual bool Freeze();
	virtual bool Unfreeze();
	virtual void GetState(mlSynchData &aData){ return; }
	virtual void SetState(mlSynchData &aData){ return; }
	virtual JSPropertySpec* GetPropSpec(){ return NULL; }
	virtual EventSpec* GetEventSpec2(){ return NULL; }
	virtual mlString GetTagName();
	virtual bool NeedToSave(int aiPropGroupType,const wchar_t* apwcName,
		const wchar_t* apwcVal,mlRMMLElement* apTempElem);
	virtual mlString GetValue(int aiIndent){ return L""; }
	virtual mlRMMLElement* GetRefPropContext(const wchar_t* apwcName){ return this; }
	virtual SynchEventSender* GetSynchEventSenderStruct(){ return NULL; }
	virtual void SetSynchEventListener(mlRMMLElement* apMLElListener, SynchEventSender* apSEEventSender){ return; }
	virtual void RemoveSynchEventListener(mlRMMLElement* apMLElListener){ return;}
	virtual void GlobalTimeShifted(const __int64 alTime, const __int64 alTimeShift, mlEModes aePrevMode){}
	//// поддержка синхронизации по требованию медиа-объекта (synchronize(mlSynchData* apData=NULL);)
	virtual void SaveDataForSynch(mlSynchData* apData){}
	virtual mlSynchData* GetDataForSynch(){ return NULL; }
	virtual void NotifyParentChanged();
	virtual bool NeedToClearRefs(){ return false; }
	virtual mlRMMLElement* GetElemToClearRefsIn(){ return NULL; }
	virtual void ClearRefs(){}
	// установлено какое-либо непредопределенное свойство
	virtual void UnknownPropIsSet(){}
	// родитель был не null, а стал null
	virtual void ParentNulled(){}
protected:
	// установить необходимые свойства в Event на кнопочные события
	virtual void SetButtonEventData(char aidEvent){}
	// установить необходимые свойства в Event на события видимого объекта
	//virtual void SetVisibleEventData(char aidEvent);

// реализация mlMedia
public:
	mlMedia* GetScene();
	bool IsAllParentComposLoaded();
	bool GetFrozen(){ return _frozen; }
	int EnumPlugins(std::vector<mlMedia*>& avPlugRefs);
	unsigned int GetSynchID(unsigned int &auiBornReality, bool abGetIfUndefined = true);
	mlMedia** GetChildren();
	mlMedia** GetAllVisibleChildren();
	void FreeChildrenArray(mlMedia** apaChildren);
	// произвольный доступ к свойствам 
	int GetPropType(const char*);
	int GetIntProp(const char*);
	bool GetBooleanProp(const char*);
	double GetDoubleProp(const char*);
	wchar_t* GetStringProp(const char*);
	mlMedia* GetRefProp(const char*);
	void SetProp(const char*, int);
	void SetProp(const char*, double);
	void SetProp(const char*, const char*);
	void SetProp(const char*, const wchar_t*);
// 
	void FrozenChanged();
	//// поддержка синхронизации
	// сиинхронизировать состояние объекта на всех клиентах
	bool synchronize(mlSynchData* apData=NULL);
	// объект переместился из одной зоны в другую (вызывается MapManager-ом)
	void ZoneChanged(int aiZone);
	// объект перешел в другую реальность (вызывается MapManager-ом)
	void RealityChanged(unsigned int auReality);
	void ReadyForBinState();
	// изменилось визуальное представление объекта
	// (используется как сигнал для перерисовки объектов-текстур 3D-объектов)
	virtual void VisualRepresentationChanged();
	//
	virtual mlIBinSynch* GetIBinSynch(){ return (mpSynch != NULL)?(mlIBinSynch*)mpSynch:NULL; }
};

#define GET_RMMLEL(cx,jso) ((mlRMMLElement*)JS_GetPrivate(cx, jso))
#define GET_RMMLCOMP(cx,jso) ((mlRMMLComposition*)JS_GetPrivate(cx, jso))

#define MLRMMLELEMENT_IMPL \
public: \
	JSPropertySpec* GetPropSpec(){ return _JSPropertySpec; }

bool IsElementRef(JSString* ajssName);

// установить флаг синхронизации цепочки вызовов для события
// (модель синхронизации 2)
#define SET_SYNCH_FLAG4EVENT(SM, EL, EV) \
	if((SM)->mbSceneMayHaveSynchEls && EL != NULL){ \
		if((SM)->mbDontSendSynchEvents) \
			(EL)->mucSynchFlags4Events |= (EV); \
		else \
			(EL)->mucSynchFlags4Events &= ~(EV); \
	}


}
