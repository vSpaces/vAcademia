#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlSPParser.h"
#include "config/prolog.h"

namespace rmml {

/**
 * Реализация подобъекта "Module" глобального JS-объекта "Player"
 */

JSBool JSModuleGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSModuleSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	
JSClass JSRMMLModuleClass = {
	"_Module", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSModuleGetProperty, JSModuleSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

enum{
	EVID_onLoad=0, // есть копия объявлений EVID_onLoad и EVID_onUnload в SceneManager.cpp
	EVID_onUnload,
	EVID_onSceneComplete,
	EVID_onUnloadScene,
	JSPROP_src,
	JSPROP_currentSceneIndex,
//	JSPROP_base
};

enum {
};

JSPropertySpec _JSModulePropertySpec[] = {
	JSPROP_RW(src)
//	JSPROP_RW(base)
	JSPROP_RW(currentSceneIndex)
	{ 0, 0, 0, 0, 0 }
};

JSBool JSModuleSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		int iID=JSVAL_TO_INT(id);
		switch(iID){
		case JSPROP_src:
			// ??
			break;
		case JSPROP_currentSceneIndex:{
			jsdouble dVal = 0.0;
			if(JS_ValueToNumber(cx, *vp, &dVal)){
				int iCurSceneIdx = (int)dVal;
				pSM->LoadScene(iCurSceneIdx);
			}
			}break;
//		case JSPROP_base:
//			if(*vp==JSVAL_VOID || *vp==JSVAL_NULL){
//			}else if(JSVAL_IS_STRING(*vp)){
//				jschar* jscBase=JS_GetStringChars(JSVAL_TO_STRING(*vp));
//				res::resIResourceManager* pResMan=pSM->GetContext()->mpResM;
//				if(pResMan!=NULL){
//					if(pResMan->ResourceExists(jscBase)){
//						pResMan->SetModuleBase(jscBase);
//					}else{
//						// ??
//					}
//				}
//			}
//			// ??
//			break;
//		// ??
		}
		// уведомляем слушателей
		char chEvID=iID-1;
		pSM->NotifyModuleListeners(chEvID);
	}
	return JS_TRUE;
}

void mlSceneManager::NotifyCurrentSceneIndex(){
	NotifyModuleListeners(JSPROP_currentSceneIndex);
}

void mlSceneManager::NotifyUnloadScene(){
	NotifyModuleListeners(EVID_onUnloadScene);
}

JSBool JSModuleGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id	)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		int iID=JSVAL_TO_INT(id);
		switch(iID){
		case JSPROP_src:
			// ??
			break;
		case JSPROP_currentSceneIndex:
			*vp=INT_TO_JSVAL(pSM->miCurrentSceneIndex);
			break;
		// ??
		}
	}
	return JS_TRUE;
}

JSFUNC_DECL(load)
JSFUNC_DECL(preload)
JSFUNC_DECL(loadScene)
JSFUNC_DECL(gotoLink)
JSFUNC_DECL(linkExists)
JSFUNC_DECL(getInfo)
JSFUNC_DECL(extract)
JSFUNC_DECL(getTaxonomy)
//JSFUNC_DECL(download)
JSFUNC_DECL(getRBRList)
JSFUNC_DECL(loadAndExec)
JSFUNC_DECL(loadInto)

///// JavaScript Function Table
JSFunctionSpec _JSModuleFunctionSpec[] = {
	JSFUNC(load,1) // загрузка сценария (либо сцены, либо просто классов)
	JSFUNC(preload,1) // предзагрузка include-ов 
	JSFUNC(loadScene,1) // загрузка сцены
	JSFUNC(gotoLink,1) // переход по ссылке
	JSFUNC(linkExists,1) // проверка на наличие модуля в локальном хранилище
	JSFUNC(getInfo,1) // выдает информацию о текущем или указанном в параметре модуле 
	JSFUNC(extract,1) // распаковывает ресурс из модуля во временную папку
	JSFUNC(getTaxonomy,1) // возвращает таксономический путь модуля
//	JSFUNC(download,1) // запускает процесс выкачивания модуля (возвращает объект типа Resource)
	JSFUNC(getRBRList, 1) // получить с сервера список всех RBR-модулей, которые требуются для указанного модуля
	JSFUNC(loadAndExec, 2) // загрузить и выполнить JS-скрипт в заданном контексте
	JSFUNC(loadInto, 2) // загрузить RMML-файл, создавая его элементы как подобъекты указанной композиции
	{ 0, 0, 0, 0, 0 }
};

void CreateModuleJSObject(JSContext *cx, JSObject *ajsoPlayer){

    JSObject* jsoModule = JS_DefineObject(cx, ajsoPlayer, GJSONM_MODULE, &JSRMMLModuleClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoModule, _JSModulePropertySpec);
	bR=JS_DefineFunctions(cx, jsoModule, _JSModuleFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	JS_SetPrivate(cx,jsoModule,pSM);
	pSM->mjsoModule=jsoModule;
	//
	jsdouble* jsdbl=JS_NewDouble(cx, 0.0); 
	jsval v=DOUBLE_TO_JSVAL(jsdbl); 
	wr_JS_SetUCProperty(cx,jsoModule,L"loadingProgress",-1,&v);

	// ??
/* пример
	// инициализация "window"
    JSObject* jsoWindow = JS_DefineObject(cx, ajsoPlayer, "window", NULL, NULL, 0);
	oms::mwIWindow* pWindow=pSM->GetContext()->mpWindow;
	int iWidth=0;
	int iHeight=0;
	if(pWindow!=NULL){
		iWidth=pWindow->GetClientWidth();
		iHeight=pWindow->GetClientHeight();
	}
	JS_DefineProperty(cx,jsoWindow,"clientWidth",INT_TO_JSVAL(iWidth),NULL,NULL,0);
	JS_DefineProperty(cx,jsoWindow,"clientHeight",INT_TO_JSVAL(iHeight),NULL,NULL,0);
*/
}

char GetModuleEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onLoad")){
		return (char)EVID_onLoad;
	}else if(isEqual(apwcEventName,L"onUnload")){
		return (char)EVID_onUnload;
	}else if(isEqual(apwcEventName,L"onSceneComplete")){
		return (char)EVID_onSceneComplete;
	}else if(isEqual(apwcEventName,L"onUnloadScene")){
		return (char)EVID_onUnloadScene;
	}else if(isEqual(apwcEventName,L"currentSceneIndex")){
		return (char)JSPROP_currentSceneIndex;
	}
	return -1;
}

wchar_t* GetModuleEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onLoad:
		return L"onLoad";
	case (char)EVID_onUnload:
		return L"onUnload";
	case (char)EVID_onSceneComplete:
		return L"onSceneComplete";
	case (char)EVID_onUnloadScene:
		return L"onUnloadScene";
	case (char)JSPROP_currentSceneIndex:
		return L"currentSceneIndex";
	}
	return L"???";
}

__forceinline int GetModuleEventIdx(char aidEvent){
	switch(aidEvent){
	case EVID_onLoad:
	case EVID_onUnload:
	case EVID_onSceneComplete:
	case EVID_onUnloadScene:
	case JSPROP_currentSceneIndex:
		return aidEvent;
	}
	return 0;
}

bool mlSceneManager::addModuleListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	char chEvID=aidEvent;
	if(chEvID >= ML_MODULE_EVENTS_COUNT) return false;
	v_elems* pv=&(maModuleListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyModuleListeners(char chEvID){
	if(chEvID < ML_MODULE_EVENTS_COUNT){
		v_elems* pv=&(maModuleListeners[chEvID]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Module);
		}
	}
}

bool mlSceneManager::removeModuleListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_MODULE_EVENTS_COUNT; ii++){
			v_elems* pv=&maModuleListeners[ii];
			v_elems::iterator vi;
			for(vi=pv->begin(); vi != pv->end(); vi++ ){
				if(*vi==apMLEl){
					pv->erase(vi);
					break;
				}
			}
		}
		return true;
	}
	v_elems* pv=&(maModuleListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

void mlSceneManager::ResetModule(){
	for(int ii=0; ii<ML_MODULE_EVENTS_COUNT; ii++){
		maModuleListeners[ii].clear();
	}
}

/*
void mlSceneManager::GetPlayerState(mlSynchData &aData){
	// ??
}

void mlSceneManager::SetPlayerState(mlSynchData &aData){
	// ??
}
*/

JSBool JSFUNC_load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc < 1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	wchar_t* jscFilePath=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	mlString sFilePath=pSM->RefineResPath(mlString(),jscFilePath);
	bool bAsynch = false; 
	//? возможно стоит задавать режим асинхронности загрузки вторым аргументом
	pSM->Load(sFilePath.c_str(), bAsynch);
	return JS_TRUE;
}

// загрузить RMML-файл, создавая его элементы как подобъекты указанной композиции
// loadInto(string rmmlScriptFilePath, object parentComposition) 
JSBool JSFUNC_loadInto(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc < 1){
		JS_ReportError(cx, "Player.module.loadInto method must get one argument at least");
		return JS_TRUE;
	}
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "Player.module.loadInto method must get string first argument");
		return JS_TRUE;
	}
	mlRMMLElement* pParentElem = NULL;
	if(argc > 1){
		if(!JSVAL_IS_MLEL(cx, argv[1])){
			JS_ReportError(cx, "Second argument of Player.module.loadInto method must be a rmml object");
			return JS_TRUE;
		}
		pParentElem = JSVAL_TO_MLEL(cx, argv[1]);
		if(!(pParentElem->IsComposition() || pParentElem->mType == MLMT_SCENE3D || pParentElem->mType == MLMT_GROUP)){
			JS_ReportError(cx, "Second argument of Player.module.loadInto method must be a rmml composition");
			return JS_TRUE;
		}
	}
	wchar_t* jscFilePath = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	mlString sFilePath = pSM->RefineResPath(mlString(), jscFilePath);
	JS_GC(cx);
	pSM->LoadInto(sFilePath.c_str(), pParentElem);
	return JS_TRUE;
}


// выполнить предзагрузку и компиляцию include-ов для скрипта
// чтобы когда он реально стал грузиться, то грузился бы быстрее
JSBool JSFUNC_preload(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc < 1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	wchar_t* jscFilePath=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	mlString sFilePath=pSM->RefineResPath(mlString(),(const wchar_t*)jscFilePath);
	pSM->Preload(sFilePath.c_str());
	return JS_TRUE;
}

JSBool JSFUNC_loadScene(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc!=1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	wchar_t* jscFilePath=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	mlString sFilePath=pSM->RefineResPath(mlString(),(const wchar_t*)jscFilePath);
	pSM->LoadScene(sFilePath.c_str());
	return JS_TRUE;
}

JSBool JSFUNC_gotoLink(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);

	*rval=	JSVAL_FALSE;	
	//if(pSM->UIButPressed()){
		// нажали на переход на модуль в UI
		// Значит можно по ModuleID-у переходить
		if(argc==0 || argv[0]==JSVAL_VOID || argv[0]==JSVAL_NULL){
			// выгрузить модуль из памяти
			pSM->NotifyModuleListeners(EVID_onUnload);
			pSM->GetContext()->mpMapMan->SetIsRender3DNeeded( false);
			//pSM->GetContext()->mpRM->ReCreateMapManager();
			pSM->SetWorldScene( NULL);
			pSM->LoadScene(-1);
			pSM->miCurrentSceneIndex=-1;
			// и перевести UI в режим выбора модуля
//		}else if(JSVAL_IS_INT(argv[0])){
//			int iArg=JSVAL_TO_INT(argv[0]);
//			if(iArg==-1){
//				// выгрузить модуль из памяти
//				// ??
//			}
//			// ??
		}else if(JSVAL_IS_STRING(argv[0])){
			jschar* jscBase=JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
			if(*jscBase==0){
				pSM->LoadScene(NULL);
			}else{
				mlString sModuleID;
				jschar* jsc;
				for(jsc=jscBase; *jsc!=L'\0' && *jsc!=L':'; jsc++)
					sModuleID+=*jsc;
				mlString sScene;
				if(*jsc==L':') 
					sScene= (wchar_t*)++jsc;
				res::resIResourceManager* pResMan=pSM->GetContext()->mpResM;
				if(pResMan!=NULL){
					GPSM(cx)->SetServerCallbacks();
					// запускает поток в котором загружается manifest.xml (из кеша при наличии или с сервера). 
					// Затем парсит manifest и tech-data. 
					// При успехе: грузим сцену.
					// Сообщаем скрипту через pRequest результат.
					int iID = pResMan->SetModuleID( (wchar_t*)sModuleID.c_str(), true, (wchar_t*)sScene.c_str());
					if(iID == -1)
					{
						mlTrace(cx, "Cannot set module ID for loading module %S \n",sModuleID.c_str());
					}
					mlTrace(cx, "Loading module %S\n",sModuleID.c_str());

					JSObject* jsoRequest = mlHTTPRequest::newJSObject(cx);
					SAVE_FROM_GC(cx, pSM->GetPlServerGO(), jsoRequest)
					mlHTTPRequest* pRequest = (mlHTTPRequest*)JS_GetPrivate(cx, jsoRequest);
					pRequest->pResultHandler = NULL;
					pRequest->msQuery = L"gotoLink.php"; // save debug info
					pRequest->SetID(iID);
					pRequest->SetRequestID(GPSM(cx)->GetRequestList()->GetNextServiceIDRequest()); 
					GPSM(cx)->GetRequestList()->AddRequest(pRequest);
					jsval vStatus = INT_TO_JSVAL(1);
					JS_SetProperty(cx, jsoRequest, "status", &vStatus);
					*rval = OBJECT_TO_JSVAL(jsoRequest);
					
					/*mlTrace(cx, "Loading module %S\n",sModuleID.c_str());
//						pResMan->SetModuleBase(sModuleID.c_str());
					if(!pResMan->SetModuleID( sModuleID.c_str()))
					{
						mlTrace(cx, "Cannot set module ID for loading module %S \n",sModuleID.c_str());
						//return JSVAL_FALSE;
					}
					if( pSM->LoadModuleMeta() == OMS_OK)
					{
						if(sScene.empty()){
							pSM->LoadScene(NULL);
							pSM->miCurrentSceneIndex=0;
							pSM->NotifyCurrentSceneIndex();
						}else{
							int iSceneNum=_wtoi(sScene.c_str());
							if(iSceneNum>0){
								pSM->LoadScene(iSceneNum-1);
							}else{
								pSM->LoadScene(sScene.c_str());
							}
						}
						*rval=JSVAL_TRUE;
					}
					else
					{
						mlTrace(cx, "Cannot load module meta %S \n",sModuleID.c_str());
					}*/
				}
			}
		}
	//}
	// ??
//	return JS_FALSE; // Если модуль грузится, то остатки скрипта ваполнять не будем
	return JS_TRUE;
}

JSBool JSFUNC_linkExists(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc!=1) return JS_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	*rval=JSVAL_FALSE;
//	if(pSM->UIButPressed()){ // в UI
		if(argv[0]==JSVAL_VOID || argv[0]==JSVAL_NULL){
			// ??
		}else if(JSVAL_IS_STRING(argv[0])){
			wchar_t* jscBase=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
			if(*jscBase==0){
				pSM->LoadScene(NULL);
			}else{
				mlString sModuleID=jscBase;
				res::resIResourceManager* pResMan=pSM->GetContext()->mpResM;
				if(pResMan!=NULL){
					void *spModule = NULL;
					unsigned int dwCodeError = 0;
					if(pResMan->GetModuleExists( sModuleID.c_str(), 0, &spModule, &dwCodeError))
						*rval=JSVAL_TRUE;
					pResMan->DestroyModule( &spModule);
					spModule = NULL;
//					mlString sMetaFile = sModuleID + L":\\META-INF\\manifest.xml";
//					res::resIResource* pRes=NULL;
//					if(OMS_SUCCEEDED(pResMan->OpenResource(sMetaFile.c_str(),pRes))){
//						pRes->Close();
//						*rval=JSVAL_TRUE;
//					}
				}
			}
		}
//	}
	// ??
//	return JS_FALSE; // Если модуль грузится, то остатки скрипта ваполнять не будем
	return JS_TRUE;
}

typedef gd_xml_stream::CXMLArchive<mlXMLBuffer> gdmlXMLArchive;

class mlStringList
{
	vector<void*> mvsList;
public:
	void Clear(){
		vector<void*>::iterator vi;
		for(vi=mvsList.begin(); vi != mvsList.end(); vi++ ){
			mlString* pString = (mlString*)*vi;
			MP_DELETE( pString);
		}
		mvsList.clear();
	}
	~mlStringList(){
		Clear();
	}
	bool Find(const wchar_t* apwcString){
		vector<void*>::iterator vi;
		for(vi=mvsList.begin(); vi != mvsList.end(); vi++ ){
			mlString* pString = (mlString*)*vi;
			if(isEqual(pString->c_str(), apwcString))
				return true;
		}
		return false;
	}
	bool Find(const mlString asString){
		vector<void*>::iterator vi;
		for(vi=mvsList.begin(); vi != mvsList.end(); vi++ ){
			mlString* pString = (mlString*)*vi;
			if(isEqual(pString->c_str(), asString.c_str()))
				return true;
		}
		return false;
	}
	void Add(const wchar_t* apwcString){
		mlString* pNewString = MP_NEW( mlString);
		*pNewString = apwcString;
		mvsList.push_back(pNewString);
	}
	void Add(const mlString asString){
		mlString* pNewString = MP_NEW( mlString);
		*pNewString = asString;
		mvsList.push_back(pNewString);
	}
};

mlStringList slModuleIDs;

mlresult mlSceneManager::AddResourceModuleAliases(const mlString asResourceModuleID, const mlString asAlias){
	if(asResourceModuleID.length() < 4)
		return ML_OK;
	if(slModuleIDs.Find(asResourceModuleID))
		return ML_OK;
	mlString sEduMeta = asResourceModuleID;
	sEduMeta += L':';
	sEduMeta += L"\\META-INF\\manifest.xml";
	mlXMLArchive* pXMLArchive=NULL;
	if(ML_FAILED(OpenXMLArchive(sEduMeta.c_str(),pXMLArchive))){
		mlTraceError(cx, "Required resource module (%S) not found\n",asResourceModuleID.c_str());
		return OMS_ERROR_RESOURCE_NOT_FOUND;
	}
	gdmlXMLArchive* pAr=pXMLArchive->mpAr;
	mlString sResModuleAlias;
	mlString sPath;
	pAr->SkipHeader();
	while(pAr->ReadNext()){
		gd::CString sName = pAr->GetName(); // sName.MakeLower();
		if(pAr->IsEndTag()){
			mlString::size_type iSlashPos = sPath.rfind(L'/');
			if(iSlashPos != mlString::npos)
				sPath.erase(iSlashPos);
			continue;
		}
		sPath+=L'/';
		sPath+=sName;
		//
		if(sPath == L"/lom/relation/resource"){
			sResModuleAlias.erase();
			for(unsigned u = 0; u < pAr->GetAttributeCount(); u++){
				gd::CString sAttrName = pAr->GetAttributeName(u); //sAttrName.MakeLower();
				int iPos;
				if((iPos = sAttrName.Find(L':'))>0)
					sAttrName = sAttrName.Right(sAttrName.GetLength() - iPos - 1);
				if(sAttrName == L"alias")
					sResModuleAlias = pAr->GetAttribute(u);
			}
		}else if(sPath == L"/lom/relation/resource/identifier/entry"){
			std::wstring sModuleID = pAr->GetValue();
			if(!sResModuleAlias.empty() && !sModuleID.empty()){
				mlString sAliasPath = asAlias;
				sAliasPath += L":";
				sAliasPath += sResModuleAlias;
				omsresult res;
				if(OMS_FAILED(res = 
					mpContext->mpResM->AddResourceAliasAndPath(sAliasPath.c_str(), 
						asResourceModuleID.c_str(), sModuleID.c_str())
				)){
					if(res == OMS_ERROR_ILLEGAL_VALUE){
						mlTraceError(cx, "Resource module aliases conflict (%S)\n",sResModuleAlias.c_str());
					}
					if(res == OMS_ERROR_RESOURCE_NOT_FOUND){
						mlTraceError(cx, "Required resource module (%S) not found\n",sModuleID.c_str());
						CloseXMLArchive(pXMLArchive);
						return OMS_ERROR_RESOURCE_NOT_FOUND;
					}
				}
				mlresult mlres;
				if(ML_FAILED(mlres = 
					AddResourceModuleAliases(sModuleID, sAliasPath)
				)){
					CloseXMLArchive(pXMLArchive);
					return mlres;
				}
			}
		}
		//
		if(!pAr->HasChildren()){
			mlString::size_type iSlashPos = sPath.rfind(L'/');
			if(iSlashPos != mlString::npos)
				sPath.erase(iSlashPos);
		}
	}
	CloseXMLArchive(pXMLArchive);
	return OMS_OK;
}

mlresult mlSceneManager::LoadEduMeta(mlXMLArchive* pXMLArchive, JSObject* ajsoModule, bool abGetInfo){
	// стираем все alias-t	if(mpContext->mpResM && !abGetInfo)
		mpContext->mpResM->AddResourceAliasAndPath(NULL, NULL, NULL);
	if(ajsoModule == NULL)
		ajsoModule = mjsoModule;
	JS_DeleteProperty(cx, ajsoModule, "title");
	JS_DeleteProperty(cx, ajsoModule, "description");
	JS_DeleteProperty(cx, ajsoModule, "identifier");
	JS_DeleteProperty(cx, ajsoModule, "language");
	if(pXMLArchive==NULL)
		return ML_OK;
	gdmlXMLArchive* pAr=pXMLArchive->mpAr;
	bool bGoodXML=false;
	bool bInGeneral=false;
	JSObject* jsoIdentifier=NULL;
	bool bInGeneralTitle=false;
	bool bInGeneralDescription=false;
	bool bInClassification=false;
	bool bInRelation=false;
	bool bInRequirementBrowser = false;
	mlString sModuleID;
	mlString sResModuleAlias;
	mlString sPath;
	mwsModuleID.clear();
	pAr->SkipHeader();
	while(pAr->ReadNext()){
		gd::CString sName = pAr->GetName(); // sName.MakeLower();
		if(pAr->IsEndTag()){
			mlString::size_type iSlashPos = sPath.rfind(L'/');
			if(iSlashPos != mlString::npos)
				sPath.erase(iSlashPos);
			if(bInGeneral && sName==L"general"){
				bInGeneral=false;
				continue;
			}
			if(jsoIdentifier!=NULL && sName==L"identifier"){
				jsoIdentifier=NULL;
				continue;
			}
			if(bInGeneralTitle && sName==L"title"){
				bInGeneralTitle=false;
				continue;
			}
			if(bInGeneralDescription && sName==L"description"){
				bInGeneralDescription=false;
				continue;
			}
			if(bInClassification && sName==L"classification"){
				bInClassification=false;
				continue;
			}
//			if(bInResModules && sName==L"resources"){
//				bInResModules=false;
//				continue;
//			}
			continue;
		}
		sPath+=L'/';
		sPath+=sName;
		//
		if(sPath == L"/lom/relation/resource"){
			sResModuleAlias.erase();
			for(unsigned u = 0; u < pAr->GetAttributeCount(); u++){
				gd::CString sAttrName = pAr->GetAttributeName(u); //sAttrName.MakeLower();
				int iPos;
				if((iPos = sAttrName.Find(L':'))>0)
					sAttrName = sAttrName.Right(sAttrName.GetLength() - iPos - 1);
				if(sAttrName == L"alias"){
					sResModuleAlias = pAr->GetAttribute(u);
					if(sResModuleAlias.length() == 1){
						mlTraceError(cx, "Alias '%S' is too small.\n", sResModuleAlias.c_str());
						sResModuleAlias += L'_';
					}
				}
			}
		}else if(sPath == L"/lom/relation/resource/identifier/entry"){
			std::wstring sResModuleID = pAr->GetValue();
			if(!sResModuleAlias.empty() && !sResModuleID.empty() && !abGetInfo){
				mpContext->mpResM->AddResourceAliasAndPath(sResModuleAlias.c_str(), 
															sModuleID.c_str(), sResModuleID.c_str());
				slModuleIDs.Clear();
				slModuleIDs.Add(sModuleID);
				mlresult mlres;
				if(ML_FAILED(mlres = 
					AddResourceModuleAliases(sResModuleID, sResModuleAlias)
				)){
					CloseXMLArchive(pXMLArchive);
					return mlres;
				}
			}
		}
		//
		if(sPath == L"/lom/technical/requirement/orComposite/type/value"){
			std::wstring sReqType = pAr->GetValue();
			bInRequirementBrowser = (sReqType == L"browser");
		}
		if(bInRequirementBrowser){
			if(sPath == L"/lom/technical/requirement/orComposite/minimumVersion"){
				std::wstring sMinPlayerVirsion = pAr->GetValue();
				JSString* jss = wr_JS_NewUCStringCopyZ(cx, sMinPlayerVirsion.c_str());
				JS_DefineProperty(cx, ajsoModule, "minPlayerVersion", STRING_TO_JSVAL(jss), NULL, NULL, 0);
				bInRequirementBrowser = false;
			}
		}
		//
		if(!pAr->HasChildren()){
			mlString::size_type iSlashPos = sPath.rfind(L'/');
			if(iSlashPos != mlString::npos)
				sPath.erase(iSlashPos);
		}
		//
		if(bInGeneral){
			if(bInGeneralTitle){
				if(sName==L"string"){
					gd::CString sValue=pAr->GetValue();
					JSString* jss=wr_JS_NewUCStringCopyZ(cx,sValue.GetBuffer());
					JS_DefineProperty(cx, ajsoModule, "title", STRING_TO_JSVAL(jss), NULL, NULL, 0);
				}
				continue;
			}
			if(bInGeneralDescription){
				if(sName==L"string"){
					gd::CString sValue=pAr->GetValue();
					JSString* jss=wr_JS_NewUCStringCopyZ(cx,sValue.GetBuffer());
					JS_DefineProperty(cx, ajsoModule, "description", STRING_TO_JSVAL(jss), NULL, NULL, 0);
				}
				continue;
			}
			if(jsoIdentifier!=NULL){
				gd::CString sValue=pAr->GetValue();
				/*if(sName==L"catalog"){
					JSString* jss=JS_NewUCStringCopyZ(cx,sValue.GetBuffer());
					JS_DefineProperty(cx, jsoIdentifier, "catalog", STRING_TO_JSVAL(jss), NULL, NULL, 0);
				}*/
				if(sName==L"entry"){
					sModuleID = sValue;
					mwsModuleID = sModuleID;
					JSString* jss=wr_JS_NewUCStringCopyZ(cx,sValue.GetBuffer());
//					JS_DefineProperty(cx, jsoIdentifier, "entry", STRING_TO_JSVAL(jss), NULL, NULL, 0);
					JS_DefineProperty(cx, ajsoModule, "identifier", STRING_TO_JSVAL(jss), NULL, NULL, 0);
				}
				continue;
			}
			if(sName==L"identifier"){
				if(pAr->HasChildren()) 
					jsoIdentifier=JS_DefineObject(cx, ajsoModule, "identifier", NULL, NULL, 0);
				continue;
			}
			if(sName==L"title"){
				if(pAr->HasChildren()) bInGeneralTitle=true;
				continue;
			}
			if(sName==L"language"){
				gd::CString sValue=pAr->GetValue();
				JSString* jss=wr_JS_NewUCStringCopyZ(cx,sValue.GetBuffer());
				JS_DefineProperty(cx, ajsoModule, "language", STRING_TO_JSVAL(jss), NULL, NULL, 0);
				continue;
			}
			if(sName==L"description"){
				if(pAr->HasChildren()) bInGeneralDescription=true;
				continue;
			}
//			if(sName==L"resources"){
//				if(pAr->HasChildren()) bInResModules=true;
//				continue;
//			}
//			if( bInResModules)
//			{
//				if(sName==L"resource")
//				{
//					gd::CString sModulePrefix=L"";
//					gd::CString sModulePath=pAr->GetValue();
//					unsigned uCount = pAr->GetAttributeCount();
//					for(unsigned u=0; u<uCount; u++) 
//					{
//						gd::CString sAttrName = pAr->GetAttributeName(u);
//						gd::CString sAttrValue = pAr->GetAttribute(u);
//						if(sAttrName == L"id")
//						{
//							sModulePrefix = sAttrValue;
//							break;
//						}
//					}
//					if(mpContext->mpResM && !(sModulePrefix.IsEmpty() || sModulePath.IsEmpty()))
//						mpContext->mpResM->AddResourceAliasAndPath(sModulePrefix.GetBuffer(0), sModulePath.GetBuffer(0), false);
//				}
//				continue;
//			}
			continue;
		}
		if(bInClassification){
			// ??
			continue;
		}
		if(sName==L"classification"){
			if(pAr->HasChildren()) bInClassification=true;
			continue;
		}
		// ??
		if(sName==L"general"){
			if(pAr->HasChildren()) bInGeneral=true;
			continue;
		}
		if(sName==L"lom"){
			if(pAr->GetDepth()==1) bGoodXML=true;
			continue; // skip <lom ...
		}
	}
	if(!bGoodXML) return ML_ERROR_FAILURE;
	return ML_OK;
}

mlresult mlSceneManager::LoadTechMeta(mlXMLArchive* pXMLArchive, JSObject* ajsoModule, bool abGetInfo){
	if(ajsoModule == NULL)
		ajsoModule = mjsoModule;
	JS_DeleteProperty(cx, ajsoModule, "scenes");
	JS_DeleteProperty(cx, ajsoModule, "player");
	JS_DeleteProperty(cx, ajsoModule, "help");
	JS_DeleteProperty(cx, ajsoModule, "search");
	if(pXMLArchive==NULL) return ML_OK;
	gdmlXMLArchive* pAr=pXMLArchive->mpAr;
	bool bGoodXML=false;
	bool bInScenes=false;
	JSObject* jsaScenes=NULL;
	JSObject* jsoScene=NULL;
	JSObject* jsoSearch=NULL;
	JSObject* jsaSearchPaths=NULL;
	pXMLArchive->mpAr->SkipHeader();
	while(pAr->ReadNext()){
		gd::CString sName = pAr->GetName(); // sName.MakeLower();
		if(pAr->IsEndTag()){
			if(jsaScenes!=NULL){
				if(sName==L"scene"){
					jsoScene=NULL;
					continue;
				}
				if(sName==L"scenes"){
					jsaScenes=NULL;
					continue;
				}
			}
			if(jsoSearch!=NULL && sName==L"search"){
				jsaSearchPaths=NULL;
				jsoSearch=NULL;
				continue;
			}
			// ??
			continue;
		}
		if(jsaScenes!=NULL){ // в <scenes> ... </scenes>
			if(jsoScene!=NULL){ // в <scene> ... </scene>
				if(sName==L"title"){
					gd::CString sValue=pAr->GetValue();
					JSString* jss=wr_JS_NewUCStringCopyZ(cx,sValue.GetBuffer());
					JS_DefineProperty(cx, jsoScene, "title", STRING_TO_JSVAL(jss), NULL, NULL, 0);
					continue;
				}
				if(sName==L"preview"){
					JSObject* jsoPreview=JS_NewObject(cx,NULL,NULL,NULL);
					jsval v=OBJECT_TO_JSVAL(jsoPreview);
					JS_DefineProperty(cx, jsoScene, "preview", v, NULL, NULL, 0);
					for(unsigned u=0; u<pAr->GetAttributeCount(); u++) {
						if(pAr->GetAttributeName(u)==L"path"){
							gd::CString sPath=pAr->GetAttribute(u);
							JSString* jss=wr_JS_NewUCStringCopyZ(cx,sPath.GetBuffer());
							JS_DefineProperty(cx, jsoPreview, "path", STRING_TO_JSVAL(jss), NULL, NULL, 0);
						}
					}
					continue;
				}
				continue;
			}
			if(sName==L"scene"){
				unsigned long length=0;
				JS_GetArrayLength(cx, jsaScenes, &length);
				jsoScene=JS_NewObject(cx,NULL,NULL,NULL);
				jsval v=OBJECT_TO_JSVAL(jsoScene);
				JS_SetElement(cx, jsaScenes, length, &v);
				for(unsigned u=0; u<pAr->GetAttributeCount(); u++) {
					if(pAr->GetAttributeName(u)==L"path"){
						gd::CString sPath=pAr->GetAttribute(u);
						JSString* jss=wr_JS_NewUCStringCopyZ(cx,sPath.GetBuffer());
						JS_DefineProperty(cx, jsoScene, "path", STRING_TO_JSVAL(jss), NULL, NULL, 0);
					}
				}
				continue;
			}
			continue;
		}
		if(jsoSearch!=NULL){
			if(sName==L"path"){
				unsigned long length=0;
				JS_GetArrayLength(cx, jsaSearchPaths, &length);
				JSObject* jsoPath=JS_NewObject(cx,NULL,NULL,NULL);
				jsval v=OBJECT_TO_JSVAL(jsoPath);
				JS_SetElement(cx, jsaSearchPaths, length, &v);
			}
			continue;
		}
		// ??
		if(sName==L"scenes"){
			if(pAr->HasChildren()){
				jsaScenes = JS_NewArrayObject(cx,0,NULL);
				JS_DefineProperty(cx, ajsoModule, "scenes", OBJECT_TO_JSVAL(jsaScenes), NULL, NULL, 0);
				for(unsigned u=0; u<pAr->GetAttributeCount(); u++) {
					gd::CString sValue = pAr->GetAttribute(u);
					const wchar_t* pwc = (LPCWSTR)sValue;
					int iVal = 0;
					if(!ParseInt(pwc, &iVal)){
						mlTrace(cx, "width and height attributes in player-tag must be unsigned integer numbers\n");
						continue;
					}
					if(pAr->GetAttributeName(u) == L"width"){
						JS_DefineProperty(cx, jsaScenes, "width", INT_TO_JSVAL(iVal), NULL, NULL, 0);
					}else if(pAr->GetAttributeName(u) == L"height"){
						JS_DefineProperty(cx, jsaScenes, "height", INT_TO_JSVAL(iVal), NULL, NULL, 0);
					}
				}
			}
			continue;
		}
		if(sName == L"player"){
			JSObject* jsoPlayer = JS_NewObject(cx,NULL,NULL,NULL);
			jsval v = OBJECT_TO_JSVAL(jsoPlayer);
			JS_DefineProperty(cx, ajsoModule, "player", v, NULL, NULL, 0);
			for(unsigned u=0; u<pAr->GetAttributeCount(); u++) {
				gd::CString sValue = pAr->GetAttribute(u);
				if(pAr->GetAttributeName(u) == L"UISkin"){
					JSString* jssUISkin = wr_JS_NewUCStringCopyZ(cx, (LPCWSTR)sValue);
					JS_DefineProperty(cx, jsoPlayer, "UISkin", STRING_TO_JSVAL(jssUISkin), NULL, NULL, 0);
					continue;
				}
				bool bVal = false;
				bool bIsBool = false;
				gd::CString sBoolVal = sValue;
				sBoolVal.MakeLower();
				if(sBoolVal == L"true"){
					bIsBool = true;
					bVal = true;
				}else if(sBoolVal == L"false"){
					bIsBool = true;
					bVal = false;
				}
				if(pAr->GetAttributeName(u) == L"fullScreen"){
					if(!bIsBool){
						mlTrace(cx, "fullScreen attribute in player-tag must get boolean value\n");
						continue;
					}
					JS_DefineProperty(cx, jsoPlayer, "fullScreen", BOOLEAN_TO_JSVAL(bVal), NULL, NULL, 0);
					continue;
				}
				if(pAr->GetAttributeName(u) == L"largeFonts"){
					if(!bIsBool){
						mlTrace(cx, "largeFonts attribute in player-tag must get boolean value\n");
						continue;
					}
					JS_DefineProperty(cx, jsoPlayer, "largeFonts", BOOLEAN_TO_JSVAL(bVal), NULL, NULL, 0);
					continue;
				}
				if(pAr->GetAttributeName(u) == L"bigCursors"){
					if(!bIsBool){
						mlTrace(cx, "bigCursors attribute in player-tag must get boolean value\n");
						continue;
					}
					JS_DefineProperty(cx, jsoPlayer, "bigCursors", BOOLEAN_TO_JSVAL(bVal), NULL, NULL, 0);
					continue;
				}
				const wchar_t* pwc = (LPCWSTR)sValue;
				int iVal = 0;
				if(!ParseInt(pwc, &iVal)){
					mlTrace(cx, "width and height attributes in player-tag must be unsigned integer numbers\n");
					continue;
				}
				if(pAr->GetAttributeName(u) == L"width"){
					JS_DefineProperty(cx, jsoPlayer, "width", INT_TO_JSVAL(iVal), NULL, NULL, 0);
				}else if(pAr->GetAttributeName(u) == L"height"){
					JS_DefineProperty(cx, jsoPlayer, "height", INT_TO_JSVAL(iVal), NULL, NULL, 0);
				}
			}
			continue;
		}
		if(sName==L"help"){
			JSObject* jsoHelp=JS_NewObject(cx,NULL,NULL,NULL);
			jsval v=OBJECT_TO_JSVAL(jsoHelp);
			JS_DefineProperty(cx, ajsoModule, "help", v, NULL, NULL, 0);
			for(unsigned u=0; u<pAr->GetAttributeCount(); u++) {
				if(pAr->GetAttributeName(u)==L"path"){
					gd::CString sPath=pAr->GetAttribute(u);
					JSString* jss=wr_JS_NewUCStringCopyZ(cx,sPath.GetBuffer());
					JS_DefineProperty(cx, jsoHelp, "path", STRING_TO_JSVAL(jss), NULL, NULL, 0);
				}
			}
			continue;
		}
		if(sName==L"search"){
			jsoSearch=JS_NewObject(cx,NULL,NULL,NULL);
			jsval v=OBJECT_TO_JSVAL(jsoSearch);
			JS_DefineProperty(cx, ajsoModule, "search", v, NULL, NULL, 0);
			for(unsigned u=0; u<pAr->GetAttributeCount(); u++) {
				if(pAr->GetAttributeName(u)==L"extensions"){
					gd::CString sExt=pAr->GetAttribute(u);
					JSString* jss=wr_JS_NewUCStringCopyZ(cx,sExt.GetBuffer());
					JS_DefineProperty(cx, jsoSearch, "extensions", STRING_TO_JSVAL(jss), NULL, NULL, 0);
				}
			}
			if(pAr->HasChildren()){
				jsaSearchPaths=JS_NewArrayObject(cx,0,NULL);
				JS_DefineProperty(cx, jsoSearch, "paths", OBJECT_TO_JSVAL(jsaSearchPaths), NULL, NULL, 0);
			}else{
				jsoSearch=NULL;
			}
			// ??
			continue;
		}
		if(sName==L"module"){
			if(pAr->GetDepth()==1) bGoodXML=true;
			continue; // skip <module ...
		}
	}
	if(!bGoodXML) return ML_ERROR_FAILURE;
	return ML_OK;
}

void mlSceneManager::LoadTechMetaScene( JSObject* jsaScenes, const wchar_t *aTitle, const wchar_t *aPath)
{
	unsigned long length=0;
	JS_GetArrayLength(cx, jsaScenes, &length);
	JSObject* jsoScene=JS_NewObject(cx,NULL,NULL,NULL);
	jsval v=OBJECT_TO_JSVAL(jsoScene);
	JS_SetElement(cx, jsaScenes, length, &v);
	JSString* jss = wr_JS_NewUCStringCopyZ(cx, aTitle);
	JS_DefineProperty(cx, jsoScene, "title", STRING_TO_JSVAL( jss), NULL, NULL, 0);
	JSString* jss2 = wr_JS_NewUCStringCopyZ(cx, aPath);
	JS_DefineProperty(cx, jsoScene, "path", STRING_TO_JSVAL( jss2), NULL, NULL, 0);
}

mlresult mlSceneManager::LoadTechMetaDefault(){
	JS_DeleteProperty(cx, mjsoModule, "scenes");
	JS_DeleteProperty(cx, mjsoModule, "player");
	JS_DeleteProperty(cx, mjsoModule, "help");
	JS_DeleteProperty(cx, mjsoModule, "search");	
	JSObject* jsaScenes=NULL;
	JSObject* jsoScene=NULL;
	jsaScenes = JS_NewArrayObject(cx,0,NULL);
	JS_DefineProperty(cx, mjsoModule, "scenes", OBJECT_TO_JSVAL(jsaScenes), NULL, NULL, 0);
	JS_DefineProperty(cx, jsaScenes, "width", INT_TO_JSVAL( iSceneWidthDefault), NULL, NULL, 0);
	JS_DefineProperty(cx, jsaScenes, "height", INT_TO_JSVAL( iSceneHeightDefault), NULL, NULL, 0);

	LoadTechMetaScene( jsaScenes, sSceneTitleDefault.c_str(), sScenePathDefault.c_str());
	LoadTechMetaScene( jsaScenes,sSceneTempTitleDefault.c_str(), sSceneTempPathDefault.c_str());	
	
	return ML_OK;
}

mlresult mlSceneManager::SetLoadingProgress(double adProgress){
	if(adProgress < 0.0) adProgress=0.0;
	if(adProgress > 1.0) adProgress=1.0;
	jsdouble* jsdbl=JS_NewDouble(cx, adProgress); 
	jsval v=DOUBLE_TO_JSVAL(jsdbl); 
	wr_JS_SetUCProperty(cx, mjsoModule, L"loadingProgress", -1, &v);
	if(adProgress==1.0 && mpScene!=NULL){
		mlSynchData oData;
		oData << 3;
		GPSM(cx)->GetContext()->mpInput->AddCustomEvent(CEID_onShowInfoState, (unsigned char*)oData.data(), oData.size());
		NotifyModuleListeners(EVID_onSceneComplete);
	}
	return ML_OK;
}

JSBool JSFUNC_getInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc == 0){
		*rval = OBJECT_TO_JSVAL(pSM->mjsoModule);
		return JS_TRUE;
	}
	*rval = JSVAL_NULL;
	jsval vArg = argv[0];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportError(cx, "getInfo method must get a string argument");
		return JS_TRUE;
	}
	JSString* jssModuleID = JSVAL_TO_STRING(vArg);
	mlString sModuleID = wr_JS_GetStringChars(jssModuleID);
	res::resIResourceManager* pResMan=pSM->GetContext()->mpResM;
	if(pResMan==NULL){
		JS_ReportError(cx, "Resource manager is not set");
		return JS_FALSE;
	}
	JSObject* jsoModuleInfo = JS_NewObject(cx, NULL, NULL, NULL);
	SAVE_FROM_GC(cx, obj, jsoModuleInfo);
	mlresult res = pSM->LoadModuleMeta(sModuleID.c_str(), jsoModuleInfo, true);
	FREE_FOR_GC(cx, obj, jsoModuleInfo);
	if(ML_SUCCEEDED(res))
		*rval = OBJECT_TO_JSVAL(jsoModuleInfo);
	return JS_TRUE;
}

JSBool JSFUNC_extract(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc <= 0 || argc > 2){
		JS_ReportError(cx, "extract method must get one or two string arguments");
		return JS_TRUE;
	}
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "extract method must get one or two string arguments");
		return JS_TRUE;
	}
	JSString* jssSrc = JSVAL_TO_STRING(argv[0]);
	mlString sSrc = wr_JS_GetStringChars(jssSrc);
	mlString sSubPath;
	if(argc >= 2){
		if(!JSVAL_IS_STRING(argv[1])){
			JS_ReportError(cx, "second argument of extract method must get a string");
			return JS_TRUE;
		}
		JSString* jssPath = JSVAL_TO_STRING(argv[1]);
		sSubPath = wr_JS_GetStringChars(jssPath);
	}
	*rval = JS_GetEmptyStringValue(cx);
	const wchar_t* pwcFullPath = pSM->GetContext()->mpResM->Extract(sSrc.c_str(), sSubPath.c_str());
	if(pwcFullPath != NULL)
		*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcFullPath));
	return JS_TRUE;
}

JSBool JSFUNC_getTaxonomy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	/*if(argc <= 0 || argc > 2){
		JS_ReportError(cx, "extract method must get one or two string arguments");
		return JS_TRUE;
	}
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "extract method must get one or two string arguments");
		return JS_TRUE;
	}
	JSString* jssSrc = JSVAL_TO_STRING(argv[0]);
	mlString sSrc = JS_GetStringChars(jssSrc);
	mlString sSubPath;
	if(argc >= 2){
		if(!JSVAL_IS_STRING(argv[1])){
			JS_ReportError(cx, "second argument of extract method must get a string");
			return JS_TRUE;
		}
		JSString* jssPath = JSVAL_TO_STRING(argv[1]);
		sSubPath = JS_GetStringChars(jssPath);
	}
	*rval = JS_GetEmptyStringValue(cx);*/
	const wchar_t* pwcTaxonomy = pSM->GetContext()->mpResM->GetTaxonomy( NULL);
	if(pwcTaxonomy != NULL)
		*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcTaxonomy));
	else
		*rval = JS_GetEmptyStringValue( cx);
	return JS_TRUE;
}

// запускает процесс выкачивания модуля (возвращает объект типа Resource)
//JSBool JSFUNC_download(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
//	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
//	jsval vArg = argv[0];
//	if(!JSVAL_IS_STRING(vArg)){
//		JS_ReportError(cx, "Player.module.download method must get a string argument");
//		return JS_TRUE;
//	}
//	JSString* jssModuleID = JSVAL_TO_STRING(vArg);
//	mlString sModuleID = JS_GetStringChars(jssModuleID);
//	res::resIResourceManager* pResMan=pSM->GetContext()->mpResM;
//	if(pResMan==NULL){
//		JS_ReportError(cx, "Resource manager is not set");
//		return JS_FALSE;
//	}
//	// ??
//	return JS_TRUE;
//}

JSBool JSFUNC_getRBRList(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	jsval vArg = argv[0];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportError(cx, "Player.module.getRBRList method must get a string argument");
		return JS_TRUE;
	}
	JSString* jssModuleID = JSVAL_TO_STRING(vArg);
	mlString sModuleID = wr_JS_GetStringChars(jssModuleID);
	res::resIResourceManager* pResMan=pSM->GetContext()->mpResM;
	if(pResMan==NULL){
		JS_ReportError(cx, "Resource manager is not set");
		return JS_TRUE;
	}
	const wchar_t** paRBRList = NULL;//pResMan->GetRBRList(sModuleID.c_str());
	if(paRBRList == NULL){
		jsval v[1];
		JS_NewArrayObject(cx, 0, v);
		return JS_TRUE;
	}
	int iLength = 0;
	for(; paRBRList[iLength] != NULL; iLength++);
	if(iLength == 0){
		jsval v[1];
		JS_NewArrayObject(cx, 0, v);
		return JS_TRUE;
	}
	jsval* pjsva = MP_NEW_ARR( jsval, iLength+1);
	for(int ii=0; ii < iLength; ii++){
		JSString* jss = wr_JS_NewUCStringCopyZ(cx, paRBRList[ii]);
		pjsva[ii] = STRING_TO_JSVAL(jss);
	}
	JS_NewArrayObject(cx, iLength, pjsva);
	MP_DELETE_ARR( pjsva);
	return JS_TRUE;
}

// Загрузить и выполнить JS-скрипт в заданном контексте
// loadAndExec(path [,context])
JSBool JSFUNC_loadAndExec(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;
	if(argc < 1){
		JS_ReportWarning(cx, "Player.module.loadAndExec method must get a string argument at least");
		return JS_TRUE;
	}
	jsval vArg = argv[0];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportWarning(cx, "Player.module.loadAndExec method must get a string argument");
		return JS_TRUE;
	}
	JSString* jssScriptPath = JSVAL_TO_STRING(vArg);
	mlString sScriptPath = wr_JS_GetStringChars(jssScriptPath);
	JSObject* jsoContext = obj;
	if(argc > 1){
		jsval vArg = argv[1];
		if(!JSVAL_IS_REAL_OBJECT(vArg)){
			JS_ReportWarning(cx, "Player.module.loadAndExec second argument must be an object");
			return JS_TRUE;
		}
		jsoContext = JSVAL_TO_OBJECT(vArg);
	}
	mlString sFilePath = pSM->RefineResPath(mlString(), sScriptPath.c_str());
	mlString wsScript;
	bool bXML = false;
	// извлекаем из пути к файлу расширение
	mlString::size_type dotPos = sFilePath.rfind(L'.');
	if(dotPos != mlString::npos){
		mlString wsFileExt = sFilePath.substr(dotPos+1);
		wchar_t* wstr = (wchar_t*)wsFileExt.c_str();
		_wcslwr_s(wstr, wsFileExt.length()+1);
		if(wsFileExt == L"xml")
			bXML = true;
	}

	if(bXML){
		mlXMLArchive* pXMLArchive = NULL;
		if(ML_FAILED(pSM->OpenXMLArchive(sFilePath.c_str(), pXMLArchive))){
			mlTraceWarning(cx, "Cannot open XML script \'%S\'.\n", sFilePath.c_str());
			return JS_TRUE;
		}
		// ?? пока тупо перебираем все тэги и если встречаем тэг script, то выдаем его содержимое в и заканчиваем wsScript
		gdmlXMLArchive* pAr = pXMLArchive->mpAr;
		bool bInScript = false;
		pAr->SkipHeader();
		while(pAr->ReadNext()){
			gd::CString sName = pAr->GetName(); // sName.MakeLower();
			if(pAr->IsEndTag()){
				if(sName == L"script"){
					bInScript = false;
				}
				continue;
			}
			if(sName == L"script"){
				if(pAr->HasChildren()) bInScript = true;
				else{
					gd::CString sValue = pAr->GetValue();
					wsScript = sValue.GetBuffer();
					if(!wsScript.empty())
						break;
				}
				continue;
			}
			if(bInScript && sName == L"string"){
				gd::CString sValue = pAr->GetValue();
				wsScript = sValue.GetBuffer();
				break;
			}
		}
		pSM->CloseXMLArchive(pXMLArchive); pXMLArchive=NULL;
	}else{
		res::resIResource* pRes = NULL;
		if(OMS_FAILED(pSM->GetContext()->mpResM->OpenResource(sFilePath.c_str(),pRes))){
			mlTraceWarning(cx, "Cannot open MJS-file (%S)\n", sFilePath.c_str());
			return JS_TRUE;
		}
		//	ML_ASSERTION(pRes!=NULL,"mlRMMLScript::Load()");
		istream* pis=NULL;
		if(OMS_FAILED(pRes->GetIStream(pis))){
			pRes->Close();
			return JS_TRUE;
		}
		//	ML_ASSERTION(pis!=NULL,"mlRMMLXML::Load()");
		string str;
		for(;;){
			int ic = pis->rdbuf()->sbumpc();
			if(ic == -1) 
				break;
			str.append(1, (char)ic);
		}
		pRes->Close();
		//
		wsScript = cLPWCSTR(str.c_str());
	}

	if(wsScript.empty())
	{
		mlTraceWarning(cx, "Error: script is not loaded (%s)\n", cLPCSTR(sFilePath.c_str()));
		return JS_TRUE;
	}

	JSScript* jss = JS_CompileUCScript(cx, obj, (const jschar*)wsScript.c_str(),
		wsScript.length(), cLPCSTR(sFilePath.c_str()), 1);
	if(jss != NULL){
		// защищаем от сборщика мусора
		JSObject* jsso = JS_NewScriptObject(cx, jss);
		jsval v = OBJECT_TO_JSVAL(jsso);
		jschar* pjsch = (jschar*)(void*)(&jss);
		JSBool bR = JS_SetUCProperty(cx, obj, pjsch, sizeof(void*)/sizeof(jschar), &v);

		// выполняем скрипт
		if(!JS_ExecuteScript(cx, jsoContext, jss, rval)){
			*rval = JSVAL_FALSE;
		}

		// отпускаем для сборщика мусора
		bR = JS_DeleteUCProperty2(cx, obj, pjsch, sizeof(void*)/sizeof(jschar), &v);
	}else{
		mlTraceWarning(cx, "Error: script is not compiled (%s)\n", cLPCSTR(sFilePath.c_str()));
		return JS_TRUE;
	}
	return JS_TRUE;
}

}
