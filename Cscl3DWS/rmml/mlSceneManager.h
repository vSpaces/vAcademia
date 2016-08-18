#pragma once

#include <string>
#include <map>
//#include <fstream>
#include "jsdbgapi.h"
#include "rmmld.h"
#include "mlBuildEvalCtxt.h"
#include "mlNextUpdateDTimeList.h"
//#ifdef JS_THREADSAFE
//#include "jslock.h"
//#endif
#ifdef CSCL
#include "mlRMMLJSPlServer.h"
#include "mlRMMLJSPlVoice.h"
#include "mlRMMLJSPlVideoCapture.h"
#include "mlRMMLJSPlCodeEditor.h"
#include "mlRMMLJSPlDesktop.h"
#include "mlRMMLJSPlMap.h"
#include "mlLogSeance.h"
#include "mlLogRecord.h"
#include "mlSynch.h"
#include "mlSynchPropCache.h"
#include "mlUtilityThread.h"
#include "mlScriptTerminationThread.h"
#include "mlScriptPreloader.h"
#include "ScriptedObjectManager.h"
#endif
#include "rmmlsafe.h"
#include "mlObjectsStat.h"
#include "mlRecordManager.h"
#include "IXBOXController.h"

#define TRACE_LOST_ELEMS 

namespace rmml {

/**
 * Менеджер сцен
 * Инициализирует JS-Engine, 
 * загружает прототипы элементов/композиций/сцен, 
 * загружает и инициализирует сцены
 */

using namespace std;

struct mlXMLBuffer;
class cstr_streambuf;
extern const char gpszUIDefault[];
typedef gd_xml_stream::CXMLArchive<mlXMLBuffer> CXMLArchiveWithXmlBuffer;

struct mlXMLArchive: public res::resIResource{
	MP_WSTRING msSrc;
	int miSrcIdx;
	res::resIResource* mpRes;
	res::resIWrite* mpWrite;
	cstr_streambuf* mpStrStreamBuf;
	istream* mpIS;
	iostream* mpOS;
	mlXMLBuffer* mpBuf;
	gd_xml_stream::CXMLArchive<mlXMLBuffer>* mpAr;
	mlRMMLBuilder* mpBuilder;
	mlXMLArchive():
		MP_WSTRING_INIT(msSrc)
	{
		miSrcIdx=-1;
		mpRes=NULL;
		mpWrite=NULL;
		mpStrStreamBuf=NULL;
		mpIS=NULL;
		mpOS=NULL;
		mpBuf=NULL;
		mpAr=NULL;
		mpBuilder=NULL;
	}
// res::resIResource
public:
	omsresult GetIStream(istream* &apistrm){
		apistrm = mpIS;
		return OMS_OK;
	}
	omsresult GetStream(oms::Stream* &apOMSStream){
		return OMS_ERROR_NOT_IMPLEMENTED;
	}
#ifndef CSCL
	shared_ptr<oms::Stream> GetStream(){
		return shared_ptr<oms::Stream>();
	}
#endif
	unsigned int Read( unsigned char* aBuffer, unsigned int aBufferSize, unsigned int *aErrorRead = NULL);
	unsigned int Write( unsigned char* aBuffer, unsigned int aBufferSize){
		return 0;
	}
	bool Seek( long aPos, unsigned char aFrom){
		return false;
	}
	unsigned long GetPos(){
		return 0; //mpStrStreamBuf->
	}
	unsigned long GetSize();
	void Close(){
		MP_DELETE_THIS;
	}
	const wchar_t* GetURL(){
		return L"";
	}
	bool GetFileNameRefineLoaded(wchar_t **bstrName, const wchar_t *pCurName)
	{
		return false;
	}
	void Download(res::resIAsynchResource* aAsynch, const wchar_t* apwcLocalPath){}
	void DownloadToCache(res::resIAsynchResource* aAsynch){}
	bool Unpack(res::resIAsynchResource* aAsynch=NULL){ return false; }
	bool UnpackToCache(res::resIAsynchResource* aAsynch=NULL){ return false; }
	unsigned long GetDownloadedSize(){ return 0; }
	unsigned int GetState(){ return 0; }
	unsigned int GetUnpackingProgress(){ return 0; }
	bool Upload(resIResource* apSrcRes, res::resIAsynchResource* aAsynch=NULL){ return false; }
	unsigned long GetUploadedSize(){ return 0; }
	__int64 GetTime() {return 0;}
	unsigned long GetCachedSize() {return 0;}
	bool RegisterAtResServer( res::resIAsynchResource* aAsynch){ return false;}
};

class mlStringCollection{
	MP_VECTOR<wchar_t*> mvColl;
public:
	mlStringCollection():
		MP_VECTOR_INIT(mvColl)
	{
	}
	~mlStringCollection(){
		std::vector<wchar_t*>::iterator vi;
		for(vi=mvColl.begin(); vi!=mvColl.end(); vi++){
			MP_DELETE_ARR( *vi);
		}
		mvColl.clear();
	}
	int Find(const wchar_t* apwcStr){
		int iIdx = 0;
		std::vector<wchar_t*>::iterator vi;
		for(vi = mvColl.begin(); vi != mvColl.end(); vi++,iIdx++){
			if(isEqual(apwcStr,(const wchar_t*)*vi)) return iIdx;
		}
		return -1;
	}
	int FindMatch(const wchar_t* apwcStr){
		mlString sFilePath(apwcStr);
		mlString::size_type pos = 0;
		// sFilePath.replace('/', '\\');
		while((pos = sFilePath.find(L"/",pos)) != mlString::npos){
			sFilePath.replace(pos, 1, L"\\");
		}
		pos = 0;
		while((pos = sFilePath.find(L"\\\\",pos)) != mlString::npos){
			sFilePath.replace(pos, 1, L"\\");
		}
		int iIdx = 0;
		std::vector<wchar_t*>::iterator vi;
		for(vi = mvColl.begin(); vi != mvColl.end(); vi++,iIdx++){
			mlString sSrcFile((const wchar_t*)*vi);
			// убираем все префиксы
			pos = sSrcFile.rfind(L':');
			if(pos != mlString::npos)
				sSrcFile.erase(0, pos);
			else
				pos = 0;
			// sSrcFile.replace('/', '\\');
			while((pos = sSrcFile.find(L"/",pos)) != mlString::npos){
				sSrcFile.replace(pos, 1, L"\\");
			}
			//
			if(sFilePath.find(sSrcFile) != mlString::npos)
				return iIdx;
			if(sSrcFile.find(sFilePath) != mlString::npos)
				return iIdx;
			if(isEqual(apwcStr,(const wchar_t*)*vi)) return iIdx;
		}
		return -1;
	}
	int Add(const wchar_t* apwcStr){
		int iIdx = Find(apwcStr);
		if(iIdx >= 0) return iIdx;
		int iLen=wcslen(apwcStr);
		wchar_t* pwcAddStr = MP_NEW_ARR( wchar_t, iLen+1);
		//wcscpy(pwcAddStr,apwcStr);
		rmmlsafe_wcscpy(pwcAddStr, iLen+1, 0, apwcStr);
		mvColl.push_back(pwcAddStr);
		return mvColl.size()-1;
	}
	const wchar_t* operator[](int aIdx) const{
		if(aIdx < 0 || aIdx >= ((int)mvColl.size())) return NULL;
		return mvColl[aIdx];
	}
};

class mlSceneDatas{
public:
	class mlSceneData{
	public:
		mlRMMLElement* mpScene;
		MP_WSTRING msPath;
	public:
		mlSceneData(mlRMMLElement* apScene):
			MP_WSTRING_INIT(msPath)
		{ 
			mpScene=apScene; 
		}
	};
private:
	MP_VECTOR<mlSceneData*> mvDatas;
	typedef std::vector<mlSceneData*>::iterator mlSceneDataIter;
public:
	mlSceneDatas():
		MP_VECTOR_INIT(mvDatas)
	{		
	}
	~mlSceneDatas(){
		mlSceneDataIter vi=mvDatas.begin();
		for(; vi!=mvDatas.end(); vi++)
		{
			MP_DELETE( *vi);
		}
		mvDatas.clear();
	}
	void AddSceneData(mlSceneData* apData){
		if(apData==NULL) return;
		mvDatas.push_back(apData);
	}
	bool RemoveSceneData(mlRMMLElement* apScene){
		for(mlSceneDataIter vi=mvDatas.begin(); vi!=mvDatas.end(); vi++){
			mlSceneData* pData=*vi;
			if(pData->mpScene==apScene){
				mvDatas.erase(vi);
				MP_DELETE( pData);
				return true;
			}
		}
		return false;
	}
	mlSceneData* GetSceneData(mlRMMLElement* apScene){
		for(mlSceneDataIter vi=mvDatas.begin(); vi!=mvDatas.end(); vi++){
			mlSceneData* pData=*vi;
			if(pData->mpScene==apScene) return pData;
		}
		return NULL;
	}
};

#define GOID_Player		1
#define GOID_Module		2
#define GOID_Mouse		3
#define GOID_Map		4
#define GOID_Url		5
#define GOID_Voice		6
#define GOID_Recoder	7
#define GOID_Server		8
#define GOID_RecordEditor		9
#define GOID_VideoCapture		10
#define GOID_CodeEditor		11

typedef MP_VECTOR_DC<mlRMMLElement*> v_elems_dc;

class mlSceneManager:
		public mlISceneManager
{
friend class mlRMMLBuilder;
#ifdef RMML_DEBUG
friend class rmmld::mldRMMLDebugger;
#endif
	JSContext *cx;

	// RMML-классы композиций и элементов (являются JS-прототипами)
	JSObject* mjsoClasses;
//	map<wchar_t*, mlRMMLElement*> mMapProtos;
//	typedef pair<wchar_t*,mlRMMLElement*> Pair4ProtoMap;
//	typedef map<wchar_t*,mlRMMLElement*>::const_iterator Iter4ProtoMap;

	// основная сцена
	mlRMMLComposition* mpScene;
	JSObject* mpPreviousScene;
	// ссылка на XML-ку со сценой
	MP_WSTRING msSceneXML;

	// композиция пользовательского интерфейса
	mlRMMLComposition* mpUI;

	// сцена режима редактирования
	mlRMMLComposition* mpAuthScene;

	mlRMML mRMML;

	omsContext* mpContext;

	int miUpdateNumber; 

	void SetScene(mlRMMLComposition* apNewScene=NULL);
	void SetUIScene(mlRMMLComposition* apNewUIScene=NULL);
	typedef std::vector<JSObject*> v_jso;
	void RemoveSceneElemsRefs(JSObject* obj, v_jso* apLookedup=NULL);

	mlEModes meMode;

	mlETestModes meTestMode;
	std::wstring msTestResDir;

	mlSceneDatas mSceneDatas;

	// =true, если поток находтся в методе Update(...)
	bool mbUpdating;
	// счетчик: через сколько вызовов Update(...)-а IsSceneStructChanged() бедет равно false
	unsigned char muStructChangedUpdCntr;

	mlBuildEvalCtxt mBuildEvalCtxt;

	bool bRestoreSynchronization;

	// Вектор обновляемых на Update элементов
	v_elems mvUpdatableElements;

	// время последнего обновления списка обновляемых объектов сцены
	__int64 mlLastUpdatableListBuildingTime;
	
	// Вектор обновляемых на Update элементов пользовательского интерфейа
	v_elems mvUpdatableUIElements;

	// время последнего обновления списка обновляемых объектов UI
	__int64 mlLastUIUpdatableListBuildingTime;

	// изменился список обновляемых элементов
	bool mbUpdatableElementsChanged;

	MP_VECTOR<unsigned int> m_synchObjectsInReality;
	unsigned int m_synchObjectsInRealityCreated;
	unsigned int m_synchObjectsInRealityCount;

	// список вновь созданных элементов: 
	// когда они попадут в иерархию сцены, 
	// их надо будет вставить при необходимости в mvUpdatableElements/mvUpdatableUIElements и в mvObjectsToSynch
	v_elems mvNewElements; 

	v_elems mvObjectsToSynch;

	MP_VECTOR<unsigned int> mvDeletedInUpdate;	// список объектов, которые были удалены за предыдущий Update
								// (используется в LoadAndCheckInternalPath)

	v_mos mvMOToDeleteOut;

	bool mbDestroying; // идет выполнение деструктора SceneManager-а
	v_mos mvPluginsMOs; // список медиа-объектов плагинов для удаления их после остальных элементов в деструкторе SceneManager-а

	mlScriptPreloader* mScriptPreloader;

	bool mbDontUpdateOneTime;

#ifdef JS_THREADSAFE
	// список элементов, которые надо будет удалить на очередном Update-е
	v_elems mvElemsToDelete;
	MP_VECTOR<mlJSClass*> mvObjectsToDelete;
#endif

	MP_WSTRING msStartURL;

public:
	unsigned int mCountIds;
	mlLogSeanceJSO* pSeance;
	mlLogRecordJSO* pRecord;
	mlSceneManager(omsContext*);
	~mlSceneManager(void);

	void OnVisibilityDistanceKoefChanged(int koef);

	void OnWindowMoved();
	void OnKinectFound();
	void OnOculusRiftFound() override;
	void OnWindowSizeChanged( const int& auClientWidth, const int& auClientHeight);

	bool mbObjetcsUnfreezenAfterFreeze;
	bool mbObjetcsFreezeUnfreezenObjects;
	bool mbObjectsFrozen;

	bool mbPreloadingMode;
	bool mbLoadingMode;

	bool mbNewEventsToNextUpdateMode;	// если ==true, то обработка всех сгенерированных 
										// на текущем Update-е событий будет переноситься на следующий Update
	oms::omsUserEvent::UpdateID mEventQueueUpdateID;

	bool mbTraceEvents;	// если ==true, то в лог выводится вся инфа по обрабатываемым на update-е событиям

	bool InitJSE(void);
	void DestroyJSE(void);
	bool IsDestroying(){ return mbDestroying; }

	bool mbObjectCreating; // Tandy: для выявления одной исключительной ситуации...

	void SetCurrentExecutingScriptLine(int lineIndex)
	{
		m_currentExecutingScriptLine = lineIndex;
	}

	int GetCurrentExecutingScriptLine()
	{
		return m_currentExecutingScriptLine;
	}

//#ifdef ENABLE_OBJECT_STAT
	void SetCurrentExecutingScriptParams( const std::string& srcName, int lineIndex)
	{
		m_currentExecutingScriptSrc = srcName;
		m_currentExecutingScriptLine = lineIndex;
	}

	void GetCurrentExecutingScriptParams( std::string& srcName, int& lineIndex)
	{
		srcName = m_currentExecutingScriptSrc;
		lineIndex = m_currentExecutingScriptLine;
	}
//#endif

	mlSyncCallbackManager * GetSyncCallbackManager();
	mlWorldEventManager*	GetWorldEventCallbackManager();

	omsContext* GetContext(){ return mpContext; }
	JSContext* GetJSContext(){ return cx; }
//	mlEModes GetMode(){ return meMode; }
	bool IsGlobalObjectEvent(const wchar_t* apwcName,const wchar_t* apwcEvent,char &aidObj,char &aidEvent);
	static mlString RefineResPath(mlString aXPath,const wchar_t* apwcResPath);
	mlString RefineResPathEl(mlRMMLElement* apElem,const wchar_t* apwcResPath);
	mlString GetFullResPath(const wchar_t* apwcResPath);
	// заменяет все псевдонимы модулей на идентификаторы модулей
	mlString ReplaceAliasesToModuleIDs(const mlString awsPath);
	void RemoveScene(mlRMMLElement* apScene){
		mSceneDatas.RemoveSceneData(apScene);
	}
	void CallGC(mlRMMLElement* apMLEl = NULL){ mpElemRefToClear = apMLEl; mbNeedToCallGC = true; }
#ifndef JS_THREADSAFE
	bool Updating(){ return mbUpdating; }
#else
	bool Updating(){ return mbUpdating && (JS_GetContextThread(cx) == JS_GetCurrentThreadId()); }
#endif
	// =true, если несколько Update-ов назад изменилась сцена или структура активной сцены
	bool IsSceneStructChanged(){ return muStructChangedUpdCntr!=0; }
	void SceneStructChanged(){ /*ResetEvent();*/ muStructChangedUpdCntr=2; }
	//	bool IsSceneLoaded(){ return mbSceneLoaded; }
	void SaveInternalPath(mlSynchData &Data,mlRMMLElement* apMLEl);
	mlRMMLElement* LoadAndCheckInternalPath(mlSynchData &Data);
	void GetRMMLVersion(unsigned &auMajor, unsigned &auMinor){
		auMajor=RMML_VERSION_MAJOR;
		auMinor=RMML_VERSION_MINOR;
	}
	mlRMMLElement* FindElemByStringID(const wchar_t* apwcStringID);
	mlBuildEvalCtxt* GetBuildEvalCtxt(){ return &mBuildEvalCtxt; }
	mlRMMLComposition* GetScene(){ return mpScene; }

	mlRMMLElement* GetMyAvatar();

	// в деструкторе SM-а уничтожить плагины последними
	void DestroyPluginLater(moMedia* apMO);

	// JS-объект удаляется в отдельном потоке, 
	// поэтому добавляем элемент в отдельный список для последующего удаления в Update-е
	void AddObjToDeleteLater(void* apObj, int aiType);

	bool IsAlreadyIncluded(const wchar_t* apwcSrc){ return mScriptPreloader->IsAlreadyIncluded(apwcSrc); }

	// получить JavaScript call stack в виде строки
	mlString GetJSCallStack();

	void OptimizeLists();
	
	mlString mwsAllocComment;
	void SetAllocComment(const wchar_t* apwcAllocComment);
	const wchar_t* GetAllocComment(){ return mwsAllocComment.c_str(); }

	// Инициализирует объект класса
	void InitClassObject(mlRMMLElement* apClass);

	// Удалить из Classes ссылку на указанный класс
	jsval FreeClass(const mlString wsClassName);

	// Началось выполнение какого-то JS-скрипта
	void JSScriptExecutionStarted(void* aCurrentScript){ if(mpScriptTerminationThread.get() != NULL) mpScriptTerminationThread->ScriptExecutionStarted(aCurrentScript); }

	// JS-скрипт выполнился
	void JSScriptExecuted(void* aCurrentScript){ if(mpScriptTerminationThread.get() != NULL) mpScriptTerminationThread->ScriptExecuted(aCurrentScript); }

	// Пропускать "повисания" текущего JS-скрипт
	void JSScriptCancelTerimation(){ JSScriptExecuted(NULL); }

	// Случился timeout выполнения JS-скрипта (вызывается из mlScriptTerminationThread)
	void JSScriptExecutionTimeout(bool abInterrupted, int aiTimeout);

	// Получить ссылку поток прерывания скриптов (для callback-функции mlScriptTerminationThread)
	mlScriptTerminationThread* GetScriptTerminationThread(){ return mpScriptTerminationThread.get(); }

	void DontUpdateOneTime(){ mbDontUpdateOneTime = true; }

protected:
	static JSClass JSRMMLClassesClass;
	virtual mlresult InitGlobalJSObjects(JSContext* cx , JSObject* obj);

	void OptimizeList(v_elems& avElements);
	void DumpList(v_elems& avElements, const wchar_t* apwcListName);
	void DumpLists();	

// механизм задержки обработки событий на несколько update-ов
	struct mlNextUpdatesEvent{
		mlSynchData mData;
		int miEventID;
		int muUpdateCounter;	// сколько update-ов еще осталось пропустить
		bool mbForceBUMCheck; // надо ли чаще проверять какой объект под мышкой?
		mlNextUpdatesEvent(const mlSynchData& aData, int aiEventID, int auUpdateCounter, bool abForceBUMCheck){
			mData.put(aData.data(), aData.size());
			miEventID = aiEventID;
			muUpdateCounter = auUpdateCounter;
			mbForceBUMCheck = abForceBUMCheck;
		}
	};
	typedef MP_VECTOR<mlNextUpdatesEvent*> mlNextUpdatesEvents;
	mlNextUpdatesEvents mvlNUEvents;

// поддержка механизма обработки событий (операций && и || с событиями)
protected:
	struct EventToRestore{
		mlRMMLElement* pElem;
		char evID;
		jsval val;
		EventToRestore()
		{
			pElem = NULL;
			evID = 0;			
		}
		EventToRestore(mlRMMLElement* apElem, char aEvID, jsval aVal){
			pElem = apElem;
			evID = aEvID;
			val = aVal;
		}
	};
public:
	// флаг режима проверки событий или установки слушателей 
	bool mbCheckingEvents;
	bool IsCheckingEvents(){ return mbCheckingEvents; }
	mlRMMLElement* mpSELMLEl; // элемент, который надо устанавливать слушателем в режиме ECM_Check
	// список свойств, значения которых надо восстанавливать после mpvFiredEvents = NULL
	MP_VECTOR<EventToRestore> mvEventsToRestore;
	void RestoreEventJSVals();
	// список событий, которые он ждал sequencer и которые уже произошли, 
	// в GetProperties на них возвращается true, если mpvFiredEvents != NULL
	std::vector<EventSender>* mpvFiredEvents; // если != NULL, значит checking events mode
//	std::vector<EventSender>* mpvWaitingEvents; // если != NULL, значит get waiting events mode
	int CheckEvent(char aIdEvent, mlRMMLElement* apSender, jsval *vp);

// реализация глобального JS-объекта "Mouse"
public:
	int miCursorType;
	int miHandCursorType;
	// сбросить все ссылки на элемент, связанные c мышиными событиями 
	// (например, если элемент удаляется)
	// Если apMLEl==NULL, сбрасываются ссылки на любые элементы
	void ResetMouse(mlRMMLElement* apMLEl);
protected:
	JSObject* mjsoMouse;
	#define ML_MOUSE_EVENTS_COUNT 9
	v_elems_dc maMouseListeners[ML_MOUSE_EVENTS_COUNT];
	void HandleMouseEvent(oms::omsUserEvent &ev);
	void CallVisibleListeners(const v_elems* apvElems, int aiEventIdx);
	mlPoint mMouseXY;
	mlRMMLElement* mpButUnderMouse;
	mlButtonUnderMouse mBUM; // дополнительная инфа об объекте под мышкой
	v_elems* mpBUMParents;	// родительские композиции-кнопки кнопки под мышкой
	// mlRMMLVisible* mpVisibleUnderMouse;
	// mlRMMLElement* mp3DObjectUnderMouse; // 3D-объект, который под мышкой
	// mlRMMLElement* mpTextureObjectUnderMouse; // RMML-объект-текстура, которая под мышкой
	// mlPoint mpntTextureObjectMouseXY;	// координаты мыши в масштабах объекта-текстуры
	bool mbUseHandCursorChanged;
	bool m_realityLoadedEventWasGeneratedAlready;
	mlRMMLElement* mpButPressed;
	v_elems* mpBPParents;	// родительские композиции-кнопки нажатой кнопки
	mlRMMLElement* mpButRPressed;
	v_elems* mpBRPParents;	// родительские композиции-кнопки нажатой правой клавишей мыши кнопки
	__int64 alLastInpEvTime;
	__int64 mlTime;
	bool mbBigCursor;
	bool mbDragging;
	struct mlModalButton{
		mlRMMLElement* mpButton;
		mlRMMLVisible* mpVisible;	// если mpButton - 3D-объект, то mpVisible - ссылка на viewport
		mlModalButton(mlRMMLElement* apButton, mlRMMLVisible* apVisible = NULL){
			mpButton = apButton;
			mpVisible = apVisible;
		}
	};
	// объект, который захватил мышку.
	// все управление от мыши будет передаваться ему, пока он не отпустит мышку или не исчезнет сам
	mlModalButton mCaptureMouseObject; 

	bool mbForceSetCursor;

//	bool mbMouseEnabled;
//	void DisableMouse();
//	void EnableMouse();
	void GetMouseState(mlSynchData &aData);
	void SetMouseState(mlSynchData &aData);
	inline bool GetAbsEnabled(mlRMMLVisible* apVisible);
	inline void SetCursorForUnderMouse(bool abForce = false);	

	struct mlMouseEventParams
	{
		int doubleClick;
		int leftButton; 
		int middleButton; 
		int rightButton;
		int ctrl;
		int shift;

		mlMouseEventParams();
	//	void Update(const mlMouseEventParams &aMEP);
	};

	mlMouseEventParams mMouseEventParams;

	void SetMouseEventBoolProperty(const char* apcName, int aiValue);
	void SetMouseEventParams(/*const mlMouseEventParams aMEP, bool abSetEventObjProps = true*/);
	void DeleteMouseEventParams();

public:
	// в этот вектор будут складываться координаты пропущенных событий MouseMove
	MP_VECTOR<unsigned short> mvSkipedMouseMoves;
	bool addMouseListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeMouseListener(char aidEvent,mlRMMLElement* apMLEl);
	void NotifyMouseListeners(char chEvID);
	void hideMouse();	// { mpContext->mpWindow->SetCursor(0); }
	void showMouse();   // { mpContext->mpWindow->SetCursor(1); }
	mlPoint GetMouseXY(){ return mMouseXY; }
	void SetMouseXY(mlPoint &aPoint){ mMouseXY=aPoint; }
	mlPoint GetTextureObjectMouseXY(){ return mBUM.mLocalXY; }
	_int64 GetTime(){ return mlTime; }
	void SetNormalCursor(int aiCursor=-1, bool abForce = false);	//{ mpContext->mpWindow->SetCursor(1); }
	void SetHandCursor(int auCursor=0, bool abForce = false);	//{ mpContext->mpWindow->SetCursor(2); }
	void HideCursor();		//{ mpContext->mpWindow->SetCursor(0); }
	void ShowCursor();		// {}
	void UseHandCursorChanged(mlRMMLElement* apMLEL){
		if(apMLEL == mpButUnderMouse)
			mbUseHandCursorChanged = true;
	}
	void SetBigCursor(bool abBig);
	bool IsCursorBig(){ return mbBigCursor; }
	void ForceSetCursor(){ mbForceSetCursor = true; }
	void NotifyMouseModalButtons();
	mlButtonUnderMouse* GetButtonUnderMouse(){ return &mBUM; }
	mlRMMLVisible* GetVisibleUnderMouse(){ return mBUM.mpVisible; }
	mlRMMLElement* Get3DObjectUnderMouse(){ return mBUM.mp3DObject; }
	mlRMMLElement* GetTextureObjectUnderMouse(){ return mBUM.mpTextureObject; }
	void ResetMouseTarget();
	// захватить мышь (все обработчики мыши будут вызываться только у этого объекта)
	bool CaptureMouse(mlRMMLElement* apMLEl);
	bool ReleaseMouse(mlRMMLElement* apMLEl);
	bool SendMouseEvent(unsigned int eventId, unsigned int wparam, uintN argc, jsval *argv, jsval *rval);
	// Установить в глобальный объект Event текстурные координаты из mBUM
	void SetTextureXYToEventGO(mlRMMLElement* apMLEl = NULL);

// реализация глобального JS-объекта "Key"
protected:
	JSObject* mjsoKey;
	#define ML_KEY_EVENTS_COUNT 3
	v_elems_dc maKeyListeners[ML_KEY_EVENTS_COUNT];
	void HandleKeyEvent(oms::omsUserEvent &aEvent);
	unsigned short muKeyCode;
	unsigned short muKeyStates;
	unsigned short muScanCode;
	unsigned short muAscii;
	unsigned short muUnicode;
	mlRMMLElement* mpButFocused;
	mlSynchData mFocusedButInternalPath;
	void ResetKey(bool bUI = true);
	void GetKeyState(mlSynchData &aData);
	void SetKeyState(mlSynchData &aData);
	struct mlPressedKey{ 
		unsigned short muKeyCode;
		unsigned short muKeyStates;
		unsigned short muScanCode;
		unsigned short muAscii;
		unsigned short muUnicode;
		__int64 miTime;	
		mlPressedKey(){}
		mlPressedKey(unsigned short auKeyCode, unsigned short auKeyStates, unsigned short auScanCode, 
			unsigned short auAscii, unsigned short auUnicode, __int64 aiTime)
		{
			muKeyCode = auKeyCode;
			muKeyStates = auKeyStates;
			muScanCode = auScanCode;
			muAscii = auAscii;
			muUnicode = auUnicode;
			miTime = aiTime;
		}
	};
	typedef MP_VECTOR<mlPressedKey> PressedKeyVector;
	PressedKeyVector mvPressedKeys;
	bool IsEqual(const mlPressedKey& aKey1, const mlPressedKey& aKey2);
	void AddPressedKey(const mlPressedKey& aKey);
	void RemovePressedKey(const mlPressedKey& aKey);
		// проверяет вектор нажатых клавиш на наличие "просроченных" и если они на самом деле уже отжаты, то генерирует для них onKeyUp
	void CheckPressedKeys();
	
public:
	bool addKeyListener(char aidEvent,mlRMMLElement* apMLEl);
	bool addGamepadListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeKeyListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeGamepadListener(char aidEvent,mlRMMLElement* apMLEl);
	bool OnControllerFound() override;
	void OnControllerLost() override;
	short GetGamepadKey();
	short GetLeftThumbState();
	short GetRightThumbState();
	const char* GetLeftThumbValue();
	const char* GetRightThumbValue();
	short GetXThumbValue(JSString* value);
	short GetYThumbValue(JSString*value);
	unsigned short GetLeftTrigger();
	unsigned short GetRightTrigger();
	unsigned short GetKeyCode(){ return muKeyCode; }
	unsigned short GetKeyStates(){ return muKeyStates; }
	unsigned short GetKeyScanCode(){ return muScanCode; }
	void SetKeyCode(unsigned short auKeyCode){ muKeyCode=auKeyCode; }
	unsigned short GetAsciiKeyCode(){ return muAscii; }
	unsigned short GetUnicodeKeyCode(){ return muUnicode; }
	void SetFocus(mlRMMLElement* apMLEl=NULL, bool abDontCheckActive = false);
	mlRMMLElement* GetFocus(){ return mpButFocused; }
private:
	void handleGamepadButton(WORD button, WORD newButtons);
	void HandleThumb(short &curState, short &thumb, short newState);
	short GetThumbState(SHORT xPower, SHORT yPower, SHORT deadzone);
	short InternalGetLeftThumbState();
	short InternalGetRightThumbState();
	const char* GetThumbValue(SHORT xPower, SHORT yPower, SHORT deadzone);
	unsigned short GetTrigger(BYTE power, SHORT threshold);
	std::vector<WORD> m_XInputButtons;
	IXBOXController *m_ctrl;
	WORD m_prevButtons;
	WORD m_pressedButton;
	short m_leftThumbState, m_pressedLeftThumb;
	short m_rightThumbState, m_pressedRightThumb;

	//Typedef for extern function for XBOXController
	HINSTANCE hGamepadDll;
	
	typedef IXBOXController* (*newControllerType)();
	newControllerType NewController;

// реализация механизма событий глобальных объектов
public:
	// v_elems mvUIGOListeners;
	// установить слушателя событий глобального объекта
	void SetEventListener(char aidSender,char aidEvent,mlRMMLElement* apListener);
	// удалить слушателя из списка событий глобального объекта 
	void RemoveEventListener(char aidSender,mlRMMLElement* apMLEl);
	// выдать имя глобального объекта
	mlString GetEventName(char aidSender,char aidEvent);
	// удалить слушателя из всех списков'ов
	void RemoveEventListener(mlRMMLElement* apMLEl);

private:
	void CreateSyncObjectsIfNeeded(const bool isNormalMode);

// реализация глобального объекта "Player"
private:
	JSObject* mjsoPlayer;
	#define ML_PLAYER_EVENTS_COUNT 30
	v_elems_dc maPlayerListeners[ML_PLAYER_EVENTS_COUNT];
	bool addPlayerListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removePlayerListener(char aidEvent,mlRMMLElement* apMLEl);
	void ResetPlayer();
	void GetPlayerState(mlSynchData &aData);
	void SetPlayerState(mlSynchData &aData);
	bool SetPlayerEventData(char aidEvent, mlSynchData &Data);
	void SavePlayerInternalPath(mlSynchData &Data);
	void OnShowInfoStateInternal(mlSynchData& aData);
	// "module"
	#define ML_MODULE_EVENTS_COUNT 8
	v_elems_dc maModuleListeners[ML_MODULE_EVENTS_COUNT];
	bool addModuleListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeModuleListener(char aidEvent,mlRMMLElement* apMLEl);
	void ResetModule();
	mlresult LoadEduMeta(mlXMLArchive* pXMLArchive, JSObject* ajsoModule=NULL, bool abGetInfo=false);
	mlresult LoadTechMeta(mlXMLArchive* pXMLArchive, JSObject* ajsoModule=NULL, bool abGetInfo=false);
	mlresult LoadTechMetaDefault();
	void LoadTechMetaScene( JSObject* jsaScenes, const wchar_t *aTitle, const wchar_t *aPath);
	mlresult AddResourceModuleAliases(const mlString asResourceModuleID, const mlString asAlias);
	void TranslateKeyCode();

	void OnWindowFileDropInternal(mlSynchData& aData);
	void OnWindowMovedInternal();
	void OnKinectFoundInternal();
	void OnOculusRiftFoundInternal();
	void OnVisibilityDistanceKoefChangedInternal(mlSynchData& aData);
	void OnWindowSizeChangedInternal(mlSynchData& aData);

	MP_WSTRING mwsModuleID;
public:
	int miCurrentSceneIndex;
	JSObject* mjsoModule;
	void NotifyPlayerListeners(char chEvID);
	void NotifyPlayerOnRestoreUnsynchedListeners();
	void NotifyPlayerOnDisconnectListeners();
	void NotifyPlayerOnRestoreStateUnsynchedListeners();
	void NotifyPlayerOnWindowSizeChanged( const int& auClientWidth, const int& auClientHeight);
	void NotifyPlayerOnErrorReportListeners();
	void NotifyModuleListeners(char chEvID);
	void NotifyCurrentSceneIndex();
	void NotifyUnloadScene();
	std::wstring GetModuleID(){ return mwsModuleID; }
	void SetModuleID(const wchar_t* apwcModuleID){ mwsModuleID = apwcModuleID==NULL?L"":apwcModuleID; }
	bool UIButPressed(){ 
		if(mpScene==NULL) return true;
		return (mpButPressed!=NULL && mpButPressed->GetScene()==mpUI); 
	}
	mlresult LoadModuleMeta(const wchar_t* apwcModuleID, JSObject* ajsoModule, bool abGetInfo=false);
	mlresult Load(const wchar_t* asSrc, bool abAsynch){ return LoadXML(asSrc, false, abAsynch);	}
	mlresult LoadInto(const wchar_t* asSrc, mlRMMLElement* apParent);

// поддержка глобального объекта Player.server
#ifdef CSCL
friend void CreateServerJSObject(JSContext *cx, JSObject *ajsoPlayer);
friend void CreateRecordEditorJSObject(JSContext *cx, JSObject *ajsoPlayer);
friend void CreateUrlJSObject(JSContext *cx, JSObject *ajsoPlayer);
friend void CreateVoiceJSObject(JSContext *cx, JSObject *ajsoPlayer);
friend void CreateVideoCaptureJSObject(JSContext *cx, JSObject *ajsoPlayer);
friend void CreateCodeEditorJSObject(JSContext *cx, JSObject *ajsoPlayer);
private:
	JSObject* mjsoServer;
	JSObject* mjsoUrl;
	JSObject* mjsoVoice;
	JSObject* mjsoVideoCapture;
	JSObject* mjsoCodeEditor;
	mlRequestList mRequestList;
	mlServerCallbackManager* mServerCallbackManager;
	mlServiceCallbackManager* mServiceCallbackManager;
	
private:
	#define ML_SERVER_EVENTS_COUNT 14
	v_elems_dc maServerListeners[ML_SERVER_EVENTS_COUNT];
	bool addServerListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeServerListener(char aidEvent,mlRMMLElement* apMLEl);
	void NotifyServerListeners(char chEvID);
	void OnServerConnectedInternal(mlSynchData& aData);
	void OnServerConnectionErrorInternal(mlSynchData& aData);
	void OnServerDisconnectedInternal(mlSynchData& aData);
	void OnObjectInServerNotFound(mlSynchData& aData);
	void onSyncServerErrorInternal(mlSynchData& aData);

	omsresult DeleteObjectMyAvatar( mlMedia* apObject);
	omsresult DeleteObjectsOnLogout();
	void OnLogoutInternal(mlSynchData& aData);
	void OnNotifyServerTOInfoInternal(mlSynchData& aData);
	void OnSeancesInfoReceivedInternal(mlSynchData& aData);
	void OnEventsInfoIntoLocationReceivedInternal(mlSynchData& aData);
	void OnProxyAuthErrorInternal(mlSynchData& aData);
	void OnConnectToServerErrorInternal(mlSynchData& aData);
	void OnErrorCreateTempLocationInternal(mlSynchData& aData);

	void OnIPadSessionChangeSlideInternal(mlSynchData& aData);

	void OnInfoMessageInternal(mlSynchData& aData);

public:
	void ResetPlayerServer();
	mlRequestList* GetRequestList(){ return &mRequestList; }
	void SetServerCallbacks(){ mServerCallbackManager->SetCallbacks(cx); }
	void SetServiceCallbacks(){ mServiceCallbackManager->SetCallbacks(cx); }
	JSObject* GetPlServerGO(){ return mjsoServer; }
	JSObject* GetPlayerGO(){ return mjsoPlayer; }
	JSObject* GetPlUrlGO(){ return mjsoUrl; }
	JSObject* GetPlVoiceGO(){ return mjsoVoice; }
	JSObject* GetPlVideoCaptureGO(){ return mjsoVideoCapture; }
	JSObject* GetPlCodeEditorGO(){ return mjsoCodeEditor; }
	int SendRequestForObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, mlSynchLinkJSO* apObjectLink);
	void onReceivedMapServerObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, int aiStatus, unsigned int uiUserData);
#endif


	// поддержка глобального объекта Player.recordEditor
#ifdef CSCL
	friend void CreateServerJSObject(JSContext *cx, JSObject *ajsoPlayer);
	friend void CreateRecordEditorJSObject(JSContext *cx, JSObject *ajsoPlayer);
	friend void CreateUrlJSObject(JSContext *cx, JSObject *ajsoPlayer);
	friend void CreateVoiceJSObject(JSContext *cx, JSObject *ajsoPlayer);
private:
	JSObject* mjsoRecordEditor;

private:
#define ML_RECORD_EDITOR_EVENTS_COUNT 14
	v_elems_dc maRecordEditorListeners[ML_RECORD_EDITOR_EVENTS_COUNT];
	bool addRecordEditorListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeRecordEditorListener(char aidEvent,mlRMMLElement* apMLEl);
	void NotifyRecordEditorListeners(char chEvID);

	void OnCutOperationCompleteInternal(mlSynchData& aData);
	void OnCopyOperationCompleteInternal(mlSynchData& aData);
	void OnPasteOperationCompleteInternal(mlSynchData& aData);
	void OnDeleteOperationCompleteInternal(mlSynchData& aData);
	void OnUndoOperationCompleteInternal(mlSynchData& aData);
	void OnRedoOperationCompleteInternal(mlSynchData& aData);
	void OnSaveOperationCompleteInternal(mlSynchData& aData);
	void OnSaveAsOperationCompleteInternal(mlSynchData& aData);
	void OnPlayOperationCompleteInternal(mlSynchData& aData);
	void OnPauseOperationCompleteInternal(mlSynchData& aData);
	void OnRewindOperationCompleteInternal(mlSynchData& aData);
	void OnSeekOperationCompleteInternal(mlSynchData& aData);
	void OnOpenOperationCompleteInternal(mlSynchData& aData);

public:
	JSObject* GetPlRecordEditorGO(){ return mjsoServer; }
#endif


// поддержка глобального объекта Player.server.recorder
#ifdef CSCL
friend void CreateSRecorderJSObject(JSContext *cx, JSObject *ajsoPlServer);
private:
	mlWorldEventManager*		mWorldEventManager;
	mlSyncCallbackManager*	mSyncCallbackManager;
	JSObject* mjsoSRecorder;
	unsigned int muiPlayingEventID;
	unsigned int muiPlayingRecordID;
public:
	#define ML_RECORDER_EVENTS_COUNT 38
	v_elems_dc maRecorderListeners[ML_RECORDER_EVENTS_COUNT];
	bool addRecorderListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeRecorderListener(char aidEvent,mlRMMLElement* apMLEl);
	void ResetRecorderEvents();
	void NotifyRecorderListeners(char chEvID);
	void NotifyOnDesktopFileSaved( wchar_t* fileName, char* target);
	void NotifyOnSharingSessionStarted(const char* alpcTarget, const char* alpcSessionKey, const char* alpcFileName);
	void NotifyOnSharingPreviewCancel(const char* alpcTarget);
	void NotifyOnSharingPreviewOK(const char* alpcTarget);
	void NotifyOnSharingFileSaved(const char* alpcTarget, const char* alpcFileName);
	void NotifyOnSharingViewSessionStarted(const char* alpcTarget, const char* alpcSessionKey, bool canRemoteAccess);
	void NotifyOnSharingSetBarPosition(const char* alpcTarget, double sound, double duration);
	void NotifyOnSharingVideoEnd(const char* alpcTarget);	
	void NotifyOnTextureSizeChanged(const char* alpcTarget, unsigned int width, unsigned int height);
	void NotifyOnSessionClosed(const char* alpcTarget);
	void NotifyOnViewSessionClosed(const char* alpcTarget);
	void NotifyOnRemoteAccessModeChanged(const char* alpcTarget, bool canRemoteAccess);
	void NotifyWebCameraState(const char* alpcTarget, int aCode);
	void NotifySharingEventByCode(const char* alpcTarget, int stateCode);
	void NotifyGlobalMouseEvent(unsigned int wParam, float aX, float aY);
	void NotifyAvatarImageFileSaved( wchar_t* tmpFileName, const char* fileName);
	void NotifyAfterCreatingOfObjects(int aiObjectsLoaded, int aiToLoadObjectCount);
	void NotifyBeforeCreatingOfObjects();
	void SetSyncCallbacks(){ mSyncCallbackManager->SetCallbacks(cx); }
	JSObject* GetSRecorderGO(){ return mjsoSRecorder; }
	void onSeancePaused();
	void onSeancePlayed();
	void onSeanceRewinded();
	void onSeanceFinishedPlaying();
	void onSeanceLeaved();
#endif
	unsigned int GetCurrentEventID() { return muiPlayingEventID; }
	unsigned int GetPlayingRecordID() { return muiPlayingRecordID; }
	void SetCurrentEventID(unsigned int aPlayingEventID) { muiPlayingEventID = aPlayingEventID; }
	void SetPlayingRecordID(unsigned int aPlayingRecordID) { muiPlayingRecordID = aPlayingRecordID; }

// поддержка глобального объекта Player.map
private:
	mlMapCallbackManager* mMapCallbackManager;
public:
	JSObject* mjsoMap;
	#define ML_MAP_EVENTS_COUNT 11
	v_elems_dc maMapListeners[ML_MAP_EVENTS_COUNT];
	bool addMapListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeMapListener(char aidEvent,mlRMMLElement* apMLEl);
	void ResetMap();
	void NotifyMapListeners(char chEvID);
	void NotifyMapOnMyAvatarCreatedListeners();
	void SetMapCallbacks(){ mMapCallbackManager->SetCallbacks(cx); }
	JSObject* GetPlMapGO(){ return mjsoMap; }
	JSObject* LocationToJSObject(const ml3DLocation& aLocation, bool abWithParams);

	// поддержка глобального объекта Player.reality
#ifdef CSCL
public:
	unsigned int uCurrentRealityID;
	__forceinline void SetCurrentRealityID( unsigned int auRealityID) {
		uCurrentRealityID = auRealityID;
	}
#endif

	// поддержка глобального объекта Player.url
#ifdef CSCL
public:
#define ML_URL_EVENTS_COUNT 1
	v_elems_dc maUrlListeners[ML_URL_EVENTS_COUNT];
	bool addUrlListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeUrlListener(char aidEvent,mlRMMLElement* apMLEl);
	void ResetUrl();
	void NotifyUrlListeners(char chEvID);
	void NotifyUrlOnSet();
#endif

	// поддержка глобального объекта Player.voice
#ifdef CSCL
public:
	mlVoipCallbackManager* mVoipCallbackManager;
	mlDesktopCallbackManager* mDesktopCallbackManager;
	void SetDesktopCallbacks(){ mDesktopCallbackManager->SetCallbacks(cx); }
#define ML_VOICE_EVENTS_COUNT 27
#define ML_VIDEOCAPTURE_EVENTS_COUNT 2
	v_elems_dc maVoiceListeners[ML_VOICE_EVENTS_COUNT];
	v_elems_dc maVideoCaptureListeners[ML_VIDEOCAPTURE_EVENTS_COUNT];
#define ML_CODEEDITOR_EVENTS_COUNT 2
	v_elems_dc maCodeEditorListeners[ML_CODEEDITOR_EVENTS_COUNT];
	bool addVoiceListener(char aidEvent,mlRMMLElement* apMLEl);
	bool removeVoiceListener(char aidEvent,mlRMMLElement* apMLEl);
	void SetVoipCallbacks(){ mVoipCallbackManager->SetCallbacks(cx); }
	void ResetVoice();
	void NotifyVoiceListeners(char chEvID);
	void NotifyVoiceOnConnectSuccess();
	void NotifyVoiceOnConnectStart();
	void NotifyVoiceOnConnectFailed( char* aVoipServerIP);
	void NotifyVoiceOnConnectLost();
	void NotifyVoiceOnDisconnected();
	void NotifyVoiceOnKicked();
	void NotifyVoiceOnAddRoom(const char* roomName);
	void NotifyVoiceOnRemoveRoom(const char* roomName);
	void NotifyVoiceOnUserTalking(const char* userName);
	void NotifyVoiceOnUserStoppedTalking(const char* userName);
	void NotifyVoiceOnUserFileSaved(const char* userName, const char* fileName);
	void NotifyVoiceOnAudioFilePlayed(const char* userName, const char* fileName);
	void NotifyVoiceOnRecordStarted();
	void NotifyVoiceOnRecordStopped(unsigned int auErrorCode);
	void NotifyVoiceOnEvoClientWorkStateChanged(unsigned int aSessionType, unsigned int aWorkState, unsigned int auErrorCode);
	void NotifyVoiceOnSRModeStatusChanged(unsigned int auStatus);
	void NotifyVoiceOnNotified(unsigned int auCode, unsigned int auData);
	// для асинхронного режима
	void NotifySetInputDevice(unsigned int auCode, int aiToIncludeSoundDevice);
	void NotifySetOutputDevice(unsigned int auCode, int aiToIncludeSoundDevice);
	void NotifyGetInputDevices( const wchar_t *apMicDeviceNames, const wchar_t *apMixDeviceNames);
	void NotifyGetOutputDevices(wchar_t *apDeviceNames);
	void NotifyAutoFindedMicName(const wchar_t *apDeviceGuid, const wchar_t *apDeviceName, const wchar_t *apDeviceLineName);
	void NotifyDeviceErrorCode(int anErrorCode);
	void NotifyReInitializeVoipSystem();
#endif

// поддержка глобального объекта Player.videoCapture
	bool addVideoCaptureListener(char aidEvent, mlRMMLElement* apMLEl);
	void NotifyVideoCaptureListeners(char chEvID);
	bool removeVideoCaptureListener(char aidEvent,mlRMMLElement* apMLEl);
	void NotifyVideoCaptureError();
	void ResetVideoCapture();
	void OnVideoCaptureError(const unsigned int errorCode);
	void OnVideoCaptureErrorInternal(mlSynchData& aData);

	// поддержка глобального объекта Player.videoCapture
	bool addCodeEditorListener(char aidEvent, mlRMMLElement* apMLEl);
	void NotifyCodeEditorListeners(char chEvID);
	bool removeCodeEditorListener(char aidEvent,mlRMMLElement* apMLEl);
	void OnCodeEditorClosedInternal(mlSynchData& aData);
	void NotifyCodeEditorClosed();
	void ResetCodeEditor();
	void OnCodeEditorClose(const wstring& text);

// реализация глобального объекта "Event"
private:
	JSObject* mjsoEvent;
public:
	void ResetEvent();
	JSObject* GetEventGO(){ return mjsoEvent; }

// реализация синхронизации 
private:
	typedef std::vector<cm::cmObjectIDs> CVectorID;

	omsresult HandleObjectNotFound( unsigned int aObjectID, unsigned int aBornReality, unsigned int aRealityID);
	bool NeedToUpdateRMNow(const __int64 alTime, int &aiDeltaTime, const bool isNormalMode);
	void UpdateAllElements(const __int64 alTime, const bool isNormalMode);
	void GlobalTimeShifted(const __int64 alTime, const __int64 alTimeShift);
	
	// Пересоздает список обновляемых элементов
	bool RebuildUpdatableElements(const __int64 alTime);
	// Обновляет обновляемые элементы
	void UpdateUpdatableElements(const __int64 alTime);

	// Создает синхронизируемые объекты
	void CreateSyncObjects( unsigned int aTimeQuota);
	// Применяет изменение состояние синхронизируемого объекта
	void ApplySyncState( const syncObjectState* const pState, CVectorID* aNotFoundSynchObjectIDs = NULL);
	// Обновляет синхронизируемые объекты
	void UpdateSyncElements();
	// Обновляет объекты, связанные с IPad
	void UpdateIPadElements();
	// Отправляет на сервер измения синхронизируемых объектов
	void SendSyncElements();


	// Отправляет запрос на состояния синхронизируемых объектов, которые давно не обновлялись с сервера.
	void SendQueryOldSyncElements( ObjectsVector &objects, unsigned int aCurrentTime, unsigned int aWaitTime, mlRMMLElement* pNonSendElem = NULL);
	// Запрашивает состояние синхронизируемых объектов, которые давно не обновлялись с сервера. Исправление #414
	void QueryOldSyncElements();

	// удаляет запомненный идентификаторы объекта которого ждем полный запрос.
	void DeleteUnicalID( unsigned int auiID, unsigned int auiBornRealityID);

	mlNextUpdateDTimeList mlNUDTime;
	unsigned long mulSynchEventsGlobalTime;
	__int64 mlLastUpdateTime;
	int miLastUpdateDeltaTime;
	mlEModes mePrevMode;
	bool mbNeedToShiftGlobalTime;
	mlRMMLElement* mbUIScriptActivatingStartElem;
	MP_WSTRING msClientStringID;
	typedef MP_VECTOR<mlSynchLinkJSO*> VecSynchLinks;
	VecSynchLinks mvTrackingLinks;

	// Время последнего запроса старых элементов. Исправление #414
	__int64 mlLastOldSyncElementsTime;
	__int64 mlLastOldAvatarsTime;

	// Список скриптовых объектов
	CScriptedObjectManager scriptedObjectManager;

public:
	mlRMMLElement* GetScriptedObjectByID(unsigned int auObjID, unsigned int auRealityID, bool aAccurately = true);
//	void ChangeObjectBornReality( mlMedia* apMedia, unsigned int auBornRealityID);
	omsresult DeletePreviousRealityObjects(const std::vector<unsigned int>& aExcludeObjects);

public:
	enum SynchModes{
		synchMode1,
		synchMode2,
		synchMode3
	} mSynchMode;
	bool mbDontSendSynchEvents;
	bool mbSceneMayHaveSynchEls;
	unsigned short muExternEventID;
	void SetSynchJSHook();
	static void* SynchJSHook(JSContext *cx, JSStackFrame *fp, JSBool before,
						JSBool *ok, void *closure);
	JSObject* CreateElementByClassName(const char* apszClassName){
		return mRMML.CreateElement(cx,apszClassName);
	}
	void GetSceneData(mlRMMLElement* apScene, mlSynchData &aData);
	void SetSceneData(mlRMMLElement* apScene, mlSynchData &aData);
	void UpdateNextUpdateDeltaTime(int aiDeltaTime);
	unsigned long GetSynchEventsGlobalTime(){ return mulSynchEventsGlobalTime; }
	void SetActivationUIScriptsFlag(mlRMMLElement* apStartElem);
	void ResetActivationUIScriptsFlag(mlRMMLElement* apElem);
	bool IsUIScriptIsActivating(){ return mbUIScriptActivatingStartElem != NULL; }
	void SetSynchMode(SynchModes aSynchMode){ mSynchMode = aSynchMode; }
	SynchModes GetSynchMode(){ return mSynchMode; }
	const wchar_t* GetClientStringID(){ return msClientStringID.c_str(); }
	void SetClientStringID(const wchar_t* apwcStringID);
	// для статистики
	void ElementCreating(mlRMMLElement* apMLEl);
	void ElementDestroying(mlRMMLElement* apMLEl);

	void MayBeAddedToScene(mlRMMLElement* apMLEl); // у rmml-элемента parent был null, а стал не null: добавим его в список новых объектов
	void MayBeRemovedFromScene(mlRMMLElement* apMLEl); // у rmml-элемента parent был не null, а стал null: уберем его из списков

		// abCheck - проверять, может объект уже есть в списке
	void AddUpdatableElement(mlRMMLElement* apElem, bool abCheck);
	void AddSynchedElement(mlRMMLElement* apElem, bool abCheck);

	static void AddElemToList(v_elems& avElements, mlRMMLElement* apElem, bool abCheck);
	static bool RemoveFromList(v_elems& avElements, mlRMMLElement* apMLEl);

	void TrackLink(mlSynchLinkJSO* apLink);
	void LinkDestroying(mlSynchLinkJSO* apLink);
	void NotifyLinksElemDestroying(mlRMMLElement* apElem);

	// Приостановка посылки синхронизируемых состояний при телепортации
	bool IsSynchUpdatingSuspended();
	void SuspendSynchStatesSending();
	void ResumeSynchStatesSending();

	void DeleteObjectInternal( unsigned int auObjectID, unsigned int auBornRealityID);

	// выполнить предзагрузку и компиляцию include-ов для скрипта
	// чтобы когда он реально стал грузиться, то грузился бы быстрее
	mlresult Preload(const wchar_t* asSrc);

	// Выводит в лог список RMML-элементов, которые не в иерархии сцены, но JS-ссылка на них имеется
	void TraceLeakedElements();
	// Выводит в лог список обновляемых RMML-элементов (у которых вызывается Update() на каждый update sceneManager-а)
	void TraceUpdatableElements();

private:
	mlSynchData msdLastState;
	void GetClassesState(mlSynchData &aData);
	void SetClassesState(mlSynchData &aData);
	void GetGOState(mlSynchData &aData);
	void SetGOState(mlSynchData &aData);
	void HandleSyncMessageInternal(mlSynchData& aData);
	void RestoreSynchronizationInternal( const wchar_t* apwcRequestedUrl);

//
public:
	mlStringCollection mSrcFileColl;
	bool mbWasExitFromScript;
	jsval mjsvalExit;
	void FreezeElementIfNeed(mlRMMLElement* pElem);
	void UnfreezeElement(mlRMMLElement* pElem);
	bool mbParentIsSetFromJS; // =true, если parent элементу устанавливается откуда-то из JS-скрипта: тогда надо добавлять в mvNewElements на всякий случай

	enum enCustomEventIDs{
		CEID_serverQueryComplete = 1,
		CEID_mapTeleported,
		CEID_mapTeleportError,
		CEID_mapShowZone,
		CEID_syncDeleteObject,
		CEID_syncHandleMessage,
		CEID_syncOnReceivedRecordsInfo,
		CEID_syncOnReceivedCreatedSeanceInfo,
		CEID_syncOnReceivedSeancesInfo,
		CEID_mapObjectCreated,
		CEID_syncOnObjectStatusReceived,
		CEID_mapObjectStatusReceived,
		CEID_syncOnReceivedFullSeanceInfo,
		CEID_serviceOnGetService,
		CEID_serviceOnReceivedMethodResult,
		CEID_serviceOnNotify,
		CEID_serviceOnConnectRestored,
		CEID_syncOnFreezeSeance,
		CEID_syncOnJoinedToSeance,
		CEID_voipOnConnectSuccess,
		CEID_voipOnConnectStart,
		CEID_voipOnConnectFailed,
		CEID_voipOnConnectLost,
		CEID_voipOnDisconnected,
		CEID_voipOnKicked,
		CEID_voipOnAddRoom,
		CEID_voipOnRemoveRoom,
		CEID_voipOnUserTalking,
		CEID_voipOnUserStoppedTalking,
		CEID_voipOnUserFileSaved,
		CEID_voipOnAudioFilePlayed,
		CEID_voipOnRecordingChanged,
		CEID_voipOnEvoClientWorkStateChanged,
		CEID_voipOnSRModeStatusChanged,
		CEID_voipOnNotified,
		CEID_voipOnSetInputDevice,
		CEID_voipOnSetOutputDevice,
		CEID_voipOnGetInputDevices,
		CEID_voipOnGetOutputDevices,
		CEID_voipOnDeviceErrorCode,
		CEID_voipOnReInitializeVoipSystem,
		CEID_voipOnRecievedFindedMicName,
		CEID_syncOnUnfreezeSeance,
		CEID_syncRestoreSynchronization,
		CEID_syncOnUnfreezeWithFreezeSeance,
		CEID_ConnectRecordInLocation,
		CEID_desktopOnApplyFrame,
		CEID_desktopOnDestinationRequest,
		CEID_desktopOnRaiseSessionStartedEvent,
		CEID_desktopOnRaisePreviewCancelEvent,
		CEID_desktopOnRaisePreviewOKEvent,
		CEID_desktopOnRaiseViewSessionStartedEvent,
		CEID_desktopOnTextureSizeChangedEvent,
		CEID_desktopOnSessionClosedEvent,
		CEID_desktopOnViewSessionClosedEvent,
		CEID_desktopOnWaitOwnerEvent,
		CEID_desktopOnRemoteAccessModeChangedEvent,
		CEID_desktopOnVideoInputDeviceInitializedEvent,
		CEID_desktopOnSharingEventByCode,
		CEID_desktopOnSetBarPositionEvent,
		CEID_desktopOnVideoEndEvent,		
		CEID_desktopOnGlobalMouseEvent,		
		CEID_RecordStartFailed,
		CEID_DisconnectRecordInLocation,
		CEID_StopRecordInLocation,
		CEID_UpdatePlayingTime,
		CEID_SeanceSeekPos,
		CEID_syncOnSeancePlayed,
		CEID_syncOnSeanceRewinded,
		CEID_syncOnSeanceFinishedPlaying,
		CEID_serverOnConnected,
		CEID_serverOnConnectionFailed,
		CEID_serverOnDisconnected,
		CEID_onPreciseURL,
		CEID_syncObjectInServerNotFound,
		CEID_worldServiceOnGetService,
		CEID_worldServiceOnReceivedMethodResult,
		CEID_worldServiceOnNotify,
		CEID_syncOnSeanceLeaved,
		CEID_moduleOnOpened,
		CEID_moduleOnCreated,
		CEID_moduleOnError,
		CEID_desktopOnSharingFileSaved,
		CEID_setModuleToLoadAndLoadScene,
		CEID_syncOnServerError,
		CEID_eventEntered,
		CEID_eventLeaved, 
		CEID_locationEntered,
		CEID_onShowInfoState,
		CEID_onLogout,
		CEID_eventUpdateInfo,
		CEID_playerFileDroped,
		CEID_ResourceLoadedAsynchronously,
		CEID_mapURLChanged,
		CEID_onNotifyServerTOInfo,
		CEID_onSeancesInfo,		
		CEID_onProxyAuthError,		
		CEID_onEventUserRightsInfo,
		CEID_onEventUserRightUpdate,
		CEID_onEventUserBanUpdate,
		CEID_onConnectToServerError,
		CEID_onErrorCreateTempLocation,
		CEID_onUserSleep,
		CEID_onUserWakeUp,
		CEID_onEventsInfoIntoLocation,
		CEID_onWindowMoved,
		CEID_onCutOperationComplete,
		CEID_onCopyOperationComplete,
		CEID_onPasteOperationComplete,
		CEID_onDeleteOperationComplete,
		CEID_onSaveOperationComplete,
		CEID_onSaveAsOperationComplete,
		CEID_onUndoOperationComplete,
		CEID_onRedoOperationComplete,
		CEID_onPlayOperationComplete,
		CEID_onPauseOperationComplete,
		CEID_onRewindOperationComplete,
		CEID_onSeekOperationComplete,
		CEID_onOpenOperationComplete,
		CEID_onKinectFound,
		CEID_serverIPadSessionChangeSlide,
		CEID_onVisibilityDistanceKoefChanged,
		CEID_onWindowSizeChangedExternally,
		CEID_onLoadStateChanged,
		CEID_syncOnInfoMessage,
		CEID_updaterKey,
		CEID_onVideoCaptureError,
		CEID_onVersionReceived,
		CEID_onCodeEditorClosed,
		CEID_onOculusRiftFound
	};
private:
	mlRMMLElement* mpElemRefToClear;
	bool mbNeedToCallGC;
	__int64 mlLastCallGCTime;
	__int64 mlLastForciblyCallGCTime;
	__int64 mlLastMemSizeCheckTime;
	// загрузить RMML-скрипт
	// abUI = true, если загружается UI
	// abAsynch = true, если можно загружать асинхронно (т.е. растянуть загрузку на несколько update-ов)
	mlresult LoadXML(const char* asSrc, bool abUI, bool abAsynch, mlRMMLElement* apParent = NULL);
	mlresult LoadXML(const wchar_t* asSrc, bool abUI, bool abAsynch, mlRMMLElement* apParent = NULL);
	//	void FreezeElementIfNeed(mlRMMLElement* pElem);
	void FreezeAllObjects();
	void UnFreezeAllObjects();
	void HandleScriptEvent(oms::omsUserEvent &aEvent);
	void HandleInternalEvent(oms::omsUserEvent &aEvent);
	void HandleCustomEvent(oms::omsUserEvent &aEvent);
	unsigned int GetUpdateEventHandlingTimeQuota();
	void CallGCIfNeeded(const __int64 alTime);

	// customEvent
	void OnLoadStateChanged(mlSynchData& aData);

// поддержка XML
public:
	struct mlLibXml2Funcs{
#ifdef RMML_USE_LIBXML2
		typedef xmlParserInputPtr (*Func_xmlNewIOInputStream)
			(xmlParserCtxtPtr ctxt,
			 xmlParserInputBufferPtr input,
			 xmlCharEncoding enc);
		/*
		Create a new input stream structure encapsulating the @input into a stream suitable for the parser.

		ctxt: an XML parser context 
		input: an I/O Input 
		enc: the charset encoding if known 
		Returns: the new input stream or NULL 
		*/
		Func_xmlNewIOInputStream xmlNewIOInputStream;
		typedef xmlParserInputBufferPtr (*Func_xmlAllocParserInputBuffer)(xmlCharEncoding enc);
		Func_xmlAllocParserInputBuffer xmlAllocParserInputBuffer;
		typedef xmlParserCtxtPtr (*Func_xmlNewParserCtxt)(void);
		Func_xmlNewParserCtxt xmlNewParserCtxt;
		typedef void (*Func_xmlFreeParserCtxt)(xmlParserCtxtPtr ctxt);
		Func_xmlFreeParserCtxt xmlFreeParserCtxt;
		typedef int (*Func_xmlParseDocument)(xmlParserCtxtPtr ctxt);
		Func_xmlParseDocument xmlParseDocument;
		typedef void (*Func_xmlFreeDoc)(xmlDocPtr cur);
		Func_xmlFreeDoc xmlFreeDoc;
		typedef xmlErrorPtr (*Func_xmlGetLastError)(void);
		Func_xmlGetLastError xmlGetLastError;
		typedef int (*Func_xmlAddEncodingAlias)(const char *name, const char *alias);
		Func_xmlAddEncodingAlias xmlAddEncodingAlias;
		typedef xmlCharEncoding (*Func_xmlParseCharEncoding)(const char *name);
		Func_xmlParseCharEncoding xmlParseCharEncoding;
		typedef xmlCharEncodingHandlerPtr (*Func_xmlFindCharEncodingHandler)(const char *name);
		Func_xmlFindCharEncodingHandler xmlFindCharEncodingHandler;
		typedef int (*Func_xmlSwitchInputEncoding)
			(xmlParserCtxtPtr ctxt,
			 xmlParserInputPtr input,
			 xmlCharEncodingHandlerPtr handler);
		Func_xmlSwitchInputEncoding xmlSwitchInputEncoding;
		typedef void (*Func_xmlSetInputStreamFilePath)
			(xmlParserInputPtr input,
			 const char *filename,
			 const char *directory);
		Func_xmlSetInputStreamFilePath xmlSetInputStreamFilePath;
		
		typedef xmlTextReaderPtr (*Func_xmlNewTextReader)(xmlParserInputBufferPtr input, const char *asURI);
		Func_xmlNewTextReader xmlNewTextReader;
		
		typedef xmlTextReaderPtr (*Func_xmlReaderForMemory)(const char * buffer,int size,const char * URL,const char * encoding,int options);
		Func_xmlReaderForMemory xmlReaderForMemory;

		typedef void (*Func_xmlFreeTextReader)(xmlTextReaderPtr reader);
		Func_xmlFreeTextReader xmlFreeTextReader;
		typedef int (*Func_xmlTextReaderRead)(xmlTextReaderPtr reader);
		Func_xmlTextReaderRead xmlTextReaderRead;
		typedef xmlChar* (*Func_xmlTextReaderName)(xmlTextReaderPtr reader);
		Func_xmlTextReaderName xmlTextReaderName;
		typedef int (*Func_xmlTextReaderNodeType)(xmlTextReaderPtr reader);
		Func_xmlTextReaderNodeType xmlTextReaderNodeType;
		typedef int (*Func_xmlTextReaderDepth)(xmlTextReaderPtr reader);
		Func_xmlTextReaderDepth xmlTextReaderDepth;
		typedef int (*Func_xmlTextReaderIsEmptyElement)(xmlTextReaderPtr reader);
		Func_xmlTextReaderIsEmptyElement xmlTextReaderIsEmptyElement;
		typedef int (*Func_xmlTextReaderHasAttributes)(xmlTextReaderPtr reader);
		Func_xmlTextReaderHasAttributes xmlTextReaderHasAttributes;
		typedef int (*Func_xmlTextReaderMoveToNextAttribute)(xmlTextReaderPtr reader);
		Func_xmlTextReaderMoveToNextAttribute xmlTextReaderMoveToNextAttribute;
		typedef int (*Func_xmlTextReaderMoveToElement)(xmlTextReaderPtr reader);
		Func_xmlTextReaderMoveToElement xmlTextReaderMoveToElement;
		typedef int (*Func_xmlTextReaderHasValue)(xmlTextReaderPtr reader);
		Func_xmlTextReaderHasValue xmlTextReaderHasValue;
		typedef xmlChar* (*Func_xmlTextReaderValue)(xmlTextReaderPtr reader);
		Func_xmlTextReaderValue xmlTextReaderValue;
		typedef void (*Func_xmlSetGenericErrorFunc)(void *ctx, xmlGenericErrorFunc handler);
		Func_xmlSetGenericErrorFunc xmlSetGenericErrorFunc;

#endif
	};
private:
	unsigned mhmLibXml2;
	mlLibXml2Funcs mLibXml2Funcs;
	void DetectLibXml2();
public:
	mlresult OpenXMLArchive(const wchar_t* apwcSrc, mlXMLArchive* &apXMLArch, bool abCreateGDArchive = true);
//	mlresult OpenXMLArchive(const wchar_t* apwcSrc, res::resIResource* &apXMLArch, int &aiSrcIdx);
	mlresult OpenXMLArchive2Write(const wchar_t* apwcSrc, mlXMLArchive* &apXMLArch);
	mlresult CloseXMLArchive(mlXMLArchive* &apXMLArch);
	mlLibXml2Funcs* GetLibXml2Funcs(){ 
#ifdef RMML_USE_LIBXML2
		return (mhmLibXml2!=NULL)?&mLibXml2Funcs:NULL;
#else
		return NULL;
#endif
	}

// поддержка отладки
private:
	mlSynchData oDebugInfoData;
	void HandleGetDebugInfoEvent(oms::omsUserEvent &aEvent);
	// список RMML-элементов, которые не привязаны ни к одной из сцен (parent==null)
	typedef std::map<mlRMMLElement*, int> MapNotInScenesElems;
	MapNotInScenesElems mmapNotInScenesElems;
	// список всех элементов-классов, которые вообще появлялись в mmapNotInScenesElems
	// и до сих пор живы
	MapNotInScenesElems mmapClasses;
	void OutRMMLObjectTreeByIter(mlRMMLIterator& iter, std::ostream* aOStream, mlRMMLElement* apElem = NULL);
public:
	void CheckBug();
	unsigned long mStartLoadSceneTime;
	void UpdateNotInScenesElems(mlRMMLElement* apElem, bool abRemove = false, bool abClass = false);
	void RecheckNotInScenesElems();

// поддержка режима редактирования (Authoring mode)
private:
	omsresult AuthoringUpdate();
	mlRMMLElement* CreateElementByType(int aiType);
	mlresult SaveXML(const wchar_t* asSrc);
	void LoadAuthScene();
	void SetAuthScene(mlRMMLComposition* apAuthScene);
	mlRMMLElement* GetSceneVisibleAt(const mlPoint aPnt, mlIVisible** appVisible=NULL);
	mlRMMLElement* GetUIVisibleAt(const mlPoint aPnt, mlIVisible** appVisible=NULL);

public:
	mlRMMLElement* GetVisibleAt(const mlPoint aPnt);

// поддержка режима загрузки
private:
	unsigned int muElToLoadCnt; // общее количество элементов, которые надо загрузить
	unsigned int muElLoadedCnt; // количество уже загруженных элементов
	mlresult SetLoadingProgress(double adProgress); // 0.0-1.0
	bool mbWasAuthoringMode;
	bool bMainModuleLoaded;
	bool IsMainModuleLoaded();

// поддержка модальных кнопок
friend JSBool JSMouseGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
private:
	std::list<mlModalButton> mlModalButs;
	bool mbNeedToNotifyMouseModalButtons;
public:
	void UpdateModalButList(mlRMMLElement* apMLEl);
	void RemoveFromModalButList(mlRMMLElement* apMLEl, bool abNotifyLater = false);
	mlRMMLElement* GetModalButtonUnderMouse(mlPoint &aMouseXY, bool &bAllModalsInvisible);

// поддержка вспомогательного потока
#ifdef JS_THREADSAFE
private:
	std::auto_ptr<mlUtilityThread> mpUtilityThread;
	std::auto_ptr<mlScriptTerminationThread> mpScriptTerminationThread;
#endif

// реализация интерфейса mlISceneManager
public:
	// загрузить композицию пользовательского интерфейса
	omsresult LoadUI(const char* asSrc);
	omsresult LoadUI(const wchar_t* asSrc);
	// загрузить метаданные модуля
	omsresult LoadModuleMeta();
	// загрузить метаданные модуля и сцену
	mlresult LoadModuleMetaAndLoadScene(unsigned int aID, wchar_t *asModuleID, wchar_t* asScene);
	omsresult LoadModuleMetaAndLoadSceneInternal(mlSynchData& aData);
	// загрузить сцену модуля по номеру
	omsresult LoadScene(int aiNum);
	// загрузить сцену
	omsresult LoadScene(const char* asSrc);
	omsresult LoadScene(const wchar_t* asSrc);
	// Обновить сцену
	omsresult Update(const _int64 alTime, int &aiDeltaTime, const bool isNormalMode);
	// выдать указатель на композицию пользовательского интерфейса
	mlMedia* GetUI(){ return mpUI; }
	// выдать указатель на активную сцену
	mlMedia* GetActiveScene(){ if(meMode==smmLoading) return NULL;  return mpScene; }
	// выполнить JavaScript
	omsresult ExecScript(const wchar_t* apwcScript, int aiLength=-1, wchar_t* *appwcRetVal=NULL);
	// установить режим работы менеджера сцен
	omsresult SetMode(mlEModes aeMode);
	// получить режим работы менеджера сцен
	mlEModes GetMode(){ return meMode; }
	// установить тестовый режим работы менеджера сцен
	void SetTestMode(mlETestModes aeTesmMode);
	// получить режим тестов (0 без тестов, запуск теста 1)
	mlETestModes GetTestMode(){ return meTestMode; }
	// установить директорию тестовых ресурсов
	void SetTestResDir(const wchar_t* apwсTestResDir);
	// получить директорию тестовых ресурсов
	bool GetTestResDir( wchar_t* apwсTestResDir, int aSizeTestResDir);
	// взять состояние сцены
	omsresult GetState(void* &apData, unsigned long &aSize);
	// восстановить состояние сцены
	omsresult SetState(void* aData, unsigned long aDataSize);
	// выдать указатель на композицию авторинга
	mlMedia* GetAuthScene(){ return mpAuthScene; }
	// выдать указатель на элемент по его полному имени
	mlMedia* GetObjectByTarget(const wchar_t* apwcTarget);
	//выдать RealityID элементa по его полному имени
	unsigned int GetObjectRealityIDByTarget(const wchar_t* apwcTarget);
	// выдать указатель на сцену мира
	mlMedia* GetWorldScene() const { return mpWorld3DScene;}

	//// Поддержка синхронизации
	// аватар сменил локацию
	virtual void LocationChanged(const wchar_t* apwcLocationName,  unsigned int auiObjectID, unsigned int auiBornRealityID);
	virtual void CommunicationAreaChanged(const wchar_t* apCommunicationAreaName,  unsigned int auiObjectID, unsigned int auiBornRealityID);
	// создать моего аватара
	virtual mlMedia* CreateAvatar(mlObjectInfo4Creation &aAvatarInfo);
	// создать объект
	virtual mlMedia* CreateObject(mlObjectInfo4Creation &aObjectInfo);
	// Уведомление пользователя об ошибка и нештатных ситуациях
	// количество параметров может различаться и зависит от кода ошибки
	// !!!! Последним параметром долженм быть всегда NULL
	//  !!! reportError(L"message", L"сообщение", 0);
	//  !!! reportError(L"no_flash_decoder", 0);
	// Предопределенные коды
	// no_flash_decoder - не найден декодер Flash
	// no_stream_decoder - не найден декодер для декодирования потокового контента
	virtual void reportError(const wchar_t *code, ...);
	// Предопределенные коды
	// no_found_resource - не удалось найти ресурс
	//  param0 - ресурс
	// message - вывсти сообщение
	//  param0 - строка c сообщением
	virtual void reportWarning(const wchar_t *code, ...);
	// восстанавливает синхронизацию после потери связи с сервером
	virtual omsresult RestoreSynchronization( const wchar_t* apwcRequestedUrl);
	// определяет существование объекта в рммл
	virtual bool IsObjectExist( unsigned int auObjectID, unsigned int auBornRealityID);
	// удаляет объект
	virtual omsresult DeleteObject( unsigned int auObjectID, unsigned int auBornRealityID);
	virtual omsresult DeleteObject( mlMedia* apObject, moMedia** *appaObjects = NULL);
	// обрабатывает сообщение от другого клиента
	virtual omsresult HandleSyncMessage( unsigned int aiObjectID, unsigned int auBornRealityID, const wchar_t* apwcSender, unsigned char* apData, unsigned short aiDataSize);
	// возвращает список инклудов
	virtual int ExtractIncludes( const wchar_t* apcFileSrc, const wchar_t** appIncludes, int count);

	/*
	// Vladimir 2011/04/14
	*/
	// установить логин для автоматического входа
	virtual void SetLogin(const wchar_t* apwcLogin);
	// установить пароль для автоматического входа
	virtual void SetPassword(const wchar_t* apwcPassword);
	// установить тип пароля: закодированный - 1 или не закодированный - 0 - для автоматического входа
	virtual void SetPasswordEncoding(int iPasswordEncodingMode);
	// установить ключ для автоматического входа
	virtual void SetLoginKey(const wchar_t* apwcLoginKey);
	// установить ключ для автоматического входа (после обновления с loginKey)
	virtual void SetUpdaterKey(const wchar_t* apwcUpdaterKey);
	/*
	// end
	*/

	// установить переход на URL
	virtual void SetLocationUrl(const wchar_t* apwcURL);
	virtual void SetLocationUrlAndTeleport(const wchar_t* apwcURL);
	virtual void SetLocationUrl(const wchar_t* apwcLocation, const wchar_t* apwcPlace
		, float x, float y, float z, unsigned long zoneID
		, const wchar_t* apwcAction, const wchar_t* apwcActionParam);
	virtual const wchar_t* GetLocationURL();
	// существует ли определение для класса rmml-объектов
	virtual bool IsJSObjClassDefined(const wchar_t* apwcClassName);
	// выводит инфу о ссылках на заданные объекты в trace
	// abLost - выводить инфу только по потерянным RMML-элементам (parent==null)
	// aiType - 0: все, 1: 3D-объекты, 2: 3D-персонажи, 3: 3D-группы, 4: композиции
	// apwcObjName - имя объекта
	virtual bool TraceObjectsRefs(bool abLost=true, int aiType=0, const wchar_t* apwcObjName=NULL);
	// вывести в tracer инфу об объекте, который под мышкой
	virtual void TraceObjectUnderMouse();
	// выдает видимый 2D-RMML-элемент, который находится под мышкой (для 3D-объектов, соответственно, viewport)
	virtual mlMedia* GetVisibleUnderNouse();

	// установлена (или восстановлена) связь с одним из серверов
	virtual void OnServerConnected(int aServerMask, bool abRestored); // mlServerMask
	// установка (или восстановление) связи с одним из серверов закончилось неудачно
	virtual void OnServerConnectionError(int aServerMask, int aiError); // mlServerMask
	// потеряна связь с одним из серверов
	virtual void OnServerDisconnected(int aServerMask, int aiCause); // mlServerMask

	// завершение телепортации
	virtual void OnPreciseURL(const wchar_t* aURL);

	virtual void OnSeancesInfo( const wchar_t* aSeancesInfo, bool bFull);
	virtual void OnEventsInfoIntoLocation( const wchar_t* aEventsInfoIntoLocation);

	virtual void OnProxyAuthError( const wchar_t* aSettingName, int aProxyType, const wchar_t* aProxyServer);

	virtual void OnConnectToServerError(int errorCode);


	// Уведомляет, что с сервера пришли описания синхронизируемых объектов
	virtual void OnMapObjectsLoaded();

	// генерация события о показе зон синхронизации
	virtual void ShowZoneEvent( bool abVal);

	// генерация события о показе специальной отладочной информации на экране о системе
	virtual void SetInfoState( int idInfoState);

	// код ошибки при отмены авторизации
	virtual void OnLogout( int iCode);
	
	// время до начала ТО на сервере
	void OnNotifyServerTOInfo( int minutes);

	// вывести в поток определенное поддерево или 
	// все дерево RMML-объектов в целом в зависимости от режима (aiMode)
	//  aiMode: 0 - все дерево
	virtual void OutRMMLObjectTree(std::ostream* aOStream, int aiMode);

	// вывести в поток определенное поддерево или 
	// все дерево RMML-объектов в целом в зависимости от режима (aiMode)
	//  aiMode: 0 - все дерево
	virtual void GetRMMLObjectTree( std::vector<mlObjectInfo> &avElements, int aiMode);

	// вызвать процедуру добавления объекта под мышкой в галерею объектов
	virtual void CopyObjectUnderMouseToGallery();

	virtual std::wstring GetJsonSynchedProps(const wchar_t* apwcTarget);
	virtual std::wstring GetJsonFormatedProps(const wchar_t* apwcTarget, int aIndent = 0);
	virtual void TransposeBinState(const wchar_t* apwcTarget);

	// вызвать сборщик мусора
	// если abAsynch == true, то по возможности в асинхроннном режиме
	virtual	void CallJS_GC(bool abAsynch = true, bool abForceCall = false);
	
	#define JSVS_CLASS            0x1
	#define JSVS_MULTILINE_OBJECT 0x2
	#define JSVS_MULTILINE_ARRAY  0x4

	mlString JSVal2String(JSContext* mcx, jsval vProp, int iFormatOptions = JSVS_CLASS, int iIndent = 0, wchar_t* sIndentStr = L"\t");

	unsigned int GetCurSeanceID();
	bool isCurSeancePlaying();
	void SetSeancePlaying(bool playing);

	// приложение стало активным
	virtual void OnAppActivated();
	// приложение перестало быть активным
	virtual void OnAppDeactivated();

	// Приложение пытаются закрыть
	// Если вернет false, то приложение не надо закрывать.
	virtual bool OnClose();

	// вывести в tracer и лог глубины 2D-объекта под мышкой и его родителей
	virtual void TraceUnderMouseObjectDepth();

	void SetWorldScene(mlRMMLScene3D* val) { mpWorld3DScene = val; }

	// Обрабатывает ответ от сервера по созданию объекта
	void OnObjectCreationResult( unsigned int uErrorCode, unsigned int uObjectID, unsigned int uBornRealityID, unsigned int auRmmlHandlerID);

	// вызывается, когда на окно плеера перетащили файл
	void OnWindowFileDrop(const wchar_t* apwcFilePath, int x, int y);

	// вызывается, когда от пользователя не приходили больше 5 минут события мыши и клавиатуры или он свернул окно  (спящий режим)
	virtual void OnUserSleep();

	// вызывается, когда пользователь уходил в спящий режим и вернулся
	virtual void OnUserWakeUp(); 

	// устанавливаем права пользвоателя в данном событии
	void OnEventUserRightInfo( unsigned int aEventUserRight);

	void OnIPadSessionChangeSlide( int changeSlide);

private:
	void OnUpdaterKeyByThreadInternal( mlSynchData &oData);
	void OnVersionReceivedInternal( mlSynchData &oData);
public:
	void SetUpdaterKeyByThread(const wchar_t* apwcUpdaterKey);

	//////////////////////////////////////////////////////////////////////////
	//// Редактор записей
	//////////////////////////////////////////////////////////////////////////
	// результат выполнения операции Cut Редактора записей
	virtual void OnCutOperationComplete( int aErrorCode, unsigned int aRecordDuration);
	// результат выполнения операции Copy Редактора записей
	virtual void OnCopyOperationComplete( int aErrorCode);
	// результат выполнения операции Paste Редактора записей
	virtual void OnPasteOperationComplete( int aErrorCode, unsigned int aRecordDuration);
	// результат выполнения операции Delete Редактора записей
	virtual void OnDeleteOperationComplete( int aErrorCode, unsigned int aRecordDuration);
	// результат выполнения операции Save Редактора записей
	virtual void OnSaveOperationComplete( int aErrorCode);
	// результат выполнения операции SaveAs Редактора записей
	virtual void OnSaveAsOperationComplete( int aErrorCode);
	// результат выполнения операции Play Редактора записей
	virtual void OnPlayOperationComplete( int aErrorCode);
	// результат выполнения операции Pause Редактора записей
	virtual void OnPauseOperationComplete( int aErrorCode);
	// результат выполнения операции Rewind Редактора записей
	virtual void OnRewindOperationComplete( int aErrorCode);
	// результат выполнения операции Seek Редактора записей
	virtual void OnSeekOperationComplete( int aErrorCode);
	// результат выполнения операции Undo Редактора записей
	virtual void OnUndoOperationComplete( int aErrorCode, unsigned int aRecordDuration);
	// результат выполнения операции Redo Редактора записей
	virtual void OnRedoOperationComplete( int aErrorCode, unsigned int aRecordDuration);
	// результат выполнения операции Open Редактора записей
	virtual void OnOpenOperationComplete( int aErrorCode, std::wstring aRecordName, std::wstring aRecordDesciption, unsigned int aRecordDuration, std::wstring aAuthor, std::wstring aCreationDate, std::wstring aLocation, std::wstring aLanguage, std::wstring aURL);
	// установить версию 
	virtual void SetVersionInfo(wchar_t* version);

private:
	unsigned int uiCreatedAvatarID;
	unsigned int muiMyAvatarID;
	unsigned int mMyAvatarBornRealityID;
	mlRMMLElement* mpMyAvatar;
	mlRMMLScene3D* mpWorld3DScene;
	bool m_isMyAvatarCreated;
	bool m_isSynchUpdatesSuspended;
	MP_STRING m_currentExecutingScriptSrc;
	int m_currentExecutingScriptLine;

	// Обработка создания объектов
	typedef std::map<unsigned int/*objectID*/, unsigned int/*handler*/> MapObjectCreationResultHandlers;
	MapObjectCreationResultHandlers	mapObjectCreationResultHandlers;
	// end Обработка создания объектов

	int lastMouseX, lastMouseY;

	// статистика для отладки
	unsigned long muiSceneElementCount; // кол-во существующих на данный момент RMML-элементов в сцене
	unsigned int muiUIElementCount; // кол-во существующих на данный момент RMML-элементов в сцене
	__int64 mlLastTraceElemCountTime; // время последнего вывода информации о кол-ве существующих RMML-элементов

	void OnPreciseURLInternal(mlSynchData& aData);

	int iSceneWidthDefault;
	int iSceneHeightDefault;
	MP_WSTRING sSceneTitleDefault;
	MP_WSTRING sScenePathDefault;
	MP_WSTRING sSceneTempTitleDefault;
	MP_WSTRING sSceneTempPathDefault;

#ifndef CSCL
public:
	void add_updatable (shared_ptr<Updatable> smp_new_updbl);

private:
	typedef list<shared_ptr<Updatable> > UpdatableStorage;

	UpdatableStorage _updbl;
#endif
};

//#ifndef JS_THREADSAFE
extern mlSceneManager* gpSM;
//#endif

class mlDontSendSynchEvents
{
	bool mbDontSendSynchEventsOld;
	mlSceneManager* mpSM;
public:
	mlDontSendSynchEvents(mlSceneManager* apSM){
		mpSM = apSM;
		mbDontSendSynchEventsOld = mpSM->mbDontSendSynchEvents;
		mpSM->mbDontSendSynchEvents = true;
	}
	~mlDontSendSynchEvents(){
		mpSM->mbDontSendSynchEvents = mbDontSendSynchEventsOld;
	}
};

extern JSFunctionSpec shell_functions[];

extern JSClass JSRMMLMouseClass;
extern JSPropertySpec _JSMousePropertySpec[];
extern JSFunctionSpec _JSMouseFunctionSpec[];
extern char GetMouseEventID(const wchar_t* apwcEventName);

extern JSClass JSRMMLKeyClass;
extern JSPropertySpec _JSKeyPropertySpec[];
extern JSFunctionSpec _JSKeyFunctionSpec[];

extern JSClass JSRMMLPlayerClass;
extern void InitPlayerJSObject(JSContext *cx, JSObject *mjsoPlayer);
extern char GetPlayerEventID(const wchar_t* apwcEventName);
extern wchar_t* GetPlayerEventName(char aidEvent);
extern void CreateModuleJSObject(JSContext *cx, JSObject *ajsoPlayer);
extern void CreateServerJSObject(JSContext *cx, JSObject *ajsoPlayer);
extern void CreateRecordEditorJSObject(JSContext *cx, JSObject *ajsoPlayer);
extern void CreateSRecorderJSObject(JSContext *cx, JSObject *ajsoPlayer);
extern void CreateSRCurrentSeanceJSObject(JSContext *cx, JSObject *ajsoPlayer);
//extern void CreateRealityJSObject(JSContext *cx, JSObject *ajsoPlayer);
extern void CreateMapJSObject(JSContext *cx, JSObject *ajsoPlayer);
extern void TraceServerError(JSContext *cx, int aiErrorCode);
extern void CreateUrlJSObject(JSContext *cx, JSObject *ajsoPlayer);

extern char GetModuleEventID(const wchar_t* apwcEventName);
extern wchar_t* GetModuleEventName(char aidEvent);

extern char GetMapEventID(const wchar_t* apwcEventName);
extern wchar_t* GetMapEventName(char aidEvent);

extern char GetUrlEventID(const wchar_t* apwcEventName);
extern wchar_t* GetUrlEventName(char aidEvent);

extern char GetVoiceEventID(const wchar_t* apwcEventName);
extern wchar_t* GetVoiceEventName(char aidEvent);

extern char GetVideoCaptureEventID(const wchar_t* apwcEventName);
extern wchar_t* GetVideoCaptureEventName(char aidEvent);

extern char GetCodeEditorEventID(const wchar_t* apwcEventName);
extern wchar_t* GetCodeEditorEventName(char aidEvent);

extern char GetRecordEventID(const wchar_t* apwcEventName);
extern wchar_t* GetRecordEventName(char aidEvent);

extern char GetServerEventID(const wchar_t* apwcEventName);
extern wchar_t* GetServerEventName(char aidEvent);

extern char GetRecordEditorEventID(const wchar_t* apwcEventName);
extern wchar_t* GetRecordEditorEventName(char aidEvent);

extern JSClass JSRMMLMath3DClass;
extern void InitMath3DJSObject(JSContext *cx, JSObject *ajsoMath3D);

char* WC2MB(const wchar_t* apwc);
char* WC2MBq(const wchar_t* apwc);
wchar_t* MB2WC(const char* apch);
wchar_t* MB2WCq(const char* apch);

extern JSBool TraceRefs(JSContext *cx, JSObject *obj, JSObject* objToFind);
extern JSBool LogRefs(JSContext *cx, JSObject *obj, JSObject* objToFind);extern JSObject* GetRealityObject( JSContext *cx, cm::cmIMapManager* aMapMan, unsigned int aRealityID);
}