#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLResource::mlRMMLResource(void):
	MP_WSTRING_INIT(mwsWarning)
{
	mRMMLType=MLMT_RESOURCE;
	mType=MLMT_RESOURCE;

	mpResource = NULL;
//	downloading = false;
	mchUnpackState = 0;

	mbUploading = false;
	pAsynchCheckExist = NULL;
}

mlRMMLResource::~mlRMMLResource(void){
	Reset();
}

//	enum {
//		JSPROP_size = 1,	
//		JSPROP_downloaded,	
//		JSPROP_state, 		
//		JSPROP_downloading, // RW флаг закачивается/не закачивается
//    };
//
//	JSFUNC_DECL(download)	// начать закачивание ресурса (но не распаковывать)
//	JSFUNC_DECL(downloadAndUnpack)	// начать закачивание и распаковку
//	JSFUNC_DECL(getProgress)	// получить состояние закачки/распаковки в процентах
//	JSFUNC_DECL(copyTo)		// скопировать закачанный и распакованный ресурс в заданную дирректорию

///// JavaScript Variable Table
JSPropertySpec mlRMMLResource::_JSPropertySpec[] = {
	JSPROP_RO(size)			// RO размер файла
	JSPROP_RO(downloaded)	// RO сколько байт выкачано
	JSPROP_RO(state)		// RO состояние (существует вообще|есть в ЛХ|есть в ЛХ и распакован)
	JSPROP_RW(downloading)	// RW флаг закачивается/не закачивается
	JSPROP_RO(uploaded)		// RO сколько байт закачано в модуль
	JSPROP_RW(uploading)	// RW флаг закачивается/не закачивается
	JSPROP_RO(warningText)	// RO 
	JSPROP_RO(RS_NOT_FOUND)	// 0 - ресурс не найден
	JSPROP_RO(RS_REMOTE)	// 1 - ресурс найден на сервере, но не закачан и не закачивается
	JSPROP_RO(RS_DOWNLOADING)	// 2 - ресурс закачивается с сервера
	JSPROP_RO(RS_LOCAL)		// 3 - ресурс найден на локальном компе или на CD
	JSPROP_RO(RS_UNPACKING)	// 4 - ресурс на локальном компе и сейчас распаковывается
	JSPROP_RO(RS_READY)		// 5 - ресурс на локальном компе, распакован и готов к использованию
	JSPROP_RO(RS_UPLOADING)	// 10 - ресурс закачивается в модуль (либо в локальный, либо на севере)
	JSPROP_RO(RS_UPLOADING_PAUSE)	// 11 - закачка ресурса приостановлена
	JSPROP_RO(RS_UPLOADED)		// 12 - ресурс закачан
	JSPROP_RO(RS_ERROR)			// 13 - состояние ошибки. Код ошибки генерится только в on
	JSPROP_RO(RS_REGISTERING)	// 14 - ресурс регистрируется
	JSPROP_RO(RS_REGISTERED)	// 15 - ресурс зарегистрировался
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLResource::_JSFunctionSpec[] = {
	JSFUNC(isExist,3)		// проверка существования ресурса
	JSFUNC(download,0)		// начать закачивание ресурса (но не распаковывать)
	JSFUNC(downloadAndUnpack,0)	// начать закачивание и распаковку
	JSFUNC(getProgress,0)	// получить состояние закачки/распаковки в процентах
	JSFUNC(copyTo,1)		// скопировать закачанный и распакованный ресурс в заданную дирректорию
	JSFUNC(registerFileAtResServer,1)		// зарегистрировать ресурс на сервере ресурсов
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Resource,mlRMMLResource,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLResource)


///// JavaScript Set Property Wrapper
JSBool mlRMMLResource::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLResource);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:{
			switch(iID){
			case JSPROP_downloading:
				// булеановское свойство
				// если присваивается false, а закачка идет, то приостановить закачку
				// если присваивается true и закачка приостановлена, то возобновить закачку
				// ??
				break;
			case JSPROP_uploading:
				// булеановское свойство
				// если присваивается false, а закачка идет, то приостановить закачку
				// если присваивается true и закачка приостановлена, то возобновить закачку
				// ??
				break;
			}
		}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLResource::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLResource);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:{
			switch(iID){
			case JSPROP_size:
				if(priv->mpResource != NULL){
					double dSize = (double)priv->mpResource->GetSize();
					jsdouble* jsdbl = JS_NewDouble(cx, dSize); 
					*vp = DOUBLE_TO_JSVAL(jsdbl);
				}else
					*vp = INT_TO_JSVAL(0);
				break;
			case JSPROP_downloaded:
				if(priv->mpResource != NULL){
					double dSize = (double)priv->mpResource->GetDownloadedSize();
					jsdouble* jsdbl = JS_NewDouble(cx, dSize); 
					*vp = DOUBLE_TO_JSVAL(jsdbl);
				}else
					*vp = INT_TO_JSVAL(0);
				break;
			case JSPROP_state:
				*vp = INT_TO_JSVAL(priv->GetState());
				break;
			case JSPROP_downloading:
				// булеановское свойство
				*vp = JSVAL_FALSE;
				if(priv->mpResource != NULL){
					// если только скачиваем
					if(priv->mchUnpackState == 0){
						if(priv->GetState() == 2)
							*vp = JSVAL_TRUE;
					}else{
						// если скачиваем и распаковываем
						unsigned int uiState = priv->GetState();
						if(uiState >= 2 && uiState < 5)
							*vp = JSVAL_TRUE;
					}
				}
				break;
			case JSPROP_uploaded:
				*vp = INT_TO_JSVAL(0);
				if(priv->mpResource != NULL && priv->mbUploading){
					double dSize = (double)priv->mpResource->GetUploadedSize();
					jsdouble* jsdbl = JS_NewDouble(cx, dSize); 
					*vp = DOUBLE_TO_JSVAL(jsdbl);
				}
				break;
			case JSPROP_uploading:
				// булеановское свойство
				*vp = JSVAL_FALSE;
				if(priv->mpResource != NULL && priv->mbUploading){
					// ??
				}
				break;
			case JSPROP_warningText:{
				*vp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, priv->mwsWarning.c_str()));
				}break;
			//}
			case JSPROP_RS_NOT_FOUND:	// 0 - ресурс не найден
				*vp = INT_TO_JSVAL(0);
				break;
			case JSPROP_RS_REMOTE:		// 1 - ресурс найден на сервере, но не закачан и не закачивается
				*vp = INT_TO_JSVAL(1);
				break;
			case JSPROP_RS_DOWNLOADING:	// 2 - ресурс закачивается с сервера
				*vp = INT_TO_JSVAL(2);
				break;
			case JSPROP_RS_LOCAL:		// 3 - ресурс найден на локальном компе или на CD
				*vp = INT_TO_JSVAL(3);
				break;
			case JSPROP_RS_UNPACKING:	// 4 - ресурс на локальном компе и сейчас распаковывается
				*vp = INT_TO_JSVAL(4);
				break;
			case JSPROP_RS_READY:		// 5 - ресурс на локальном компе, распакован и готов к использованию
				*vp = INT_TO_JSVAL(5);
				break;
			case JSPROP_RS_UPLOADING:	// 10 - ресурс закачивается в модуль (либо в локальный, либо на севере)
				*vp = INT_TO_JSVAL(10);
				break;
			case JSPROP_RS_UPLOADING_PAUSE:	// 11 - закачка ресурса приостановлена
				*vp = INT_TO_JSVAL(11);
				break;
			case JSPROP_RS_UPLOADED:		// 12 - ресурс закачан
				*vp = INT_TO_JSVAL(12);
				break;
			case JSPROP_RS_ERROR:		// 13 - ресурс не закачан. Была ошибка
				*vp = INT_TO_JSVAL(13);
				break;
			case JSPROP_RS_REGISTERING:		// 14 - ресурс регистрируется
				*vp = INT_TO_JSVAL(14);
				break;
			case JSPROP_RS_REGISTERED:		// 15 - ресурс зарегистрирован
				*vp = INT_TO_JSVAL(15);
				break;
			}
		}
	GET_PROPS_END;
	return JS_TRUE;
}

void mlRMMLResource::Reset(){
	if( pAsynchCheckExist != NULL)
		pAsynchCheckExist->setHandler( NULL);
	if(mpResource != NULL){
		mpResource->Close();
		mpResource = NULL;
	}
	mchUnpackState = 0;
}

unsigned int mlRMMLResource::GetState(){
	if(mpResource == NULL)
		return 0;
	if(mbUploading){
		// ??
	}
	// если ресурс не существует, то 0 (JSPROP_RS_NOT_FOUND)
	// если ресурс существует на сервере, но не закачан
		// и не закачивается, то 1
		// и закачивается, то 2
	// если ресурс существует локально или на CD, то 3
	unsigned int uiState = mpResource->GetState();
	if( uiState & RES_ERROR)
	{
		//??
		return 13;
	}
	// если русурс существует локально или на CD, 
	if(uiState == 3){
		// то если он распаковывается, то 4
		if(mchUnpackState == 2)
			uiState = 4;
		// а если уже распакован, то 5
		else if(mchUnpackState == 3)
			uiState = 5;
	}
	if ( uiState == 5)
		uiState = 10;
	else if ( uiState == 6)
		uiState = 11;
	if ( uiState == 7)
		uiState = 12;
	return uiState;
}

// Реализация mlRMMLElement
mlresult mlRMMLResource::CreateMedia(omsContext* amcx){
	// ??
	return ML_OK;
}

mlresult mlRMMLResource::Load(){
	// ??
	return ML_OK;
}

//struct RESM_NO_VTABLE resIResource
//{
//	virtual omsresult GetIStream(istream* &apistrm)=0;
//	virtual omsresult GetStream(oms::Stream* &apOMSStream)=0;
//	virtual unsigned int	Read( unsigned char* aBuffer, unsigned int aBufferSize)=0;
//	virtual unsigned int	Write( unsigned char* aBuffer, unsigned int aBufferSize)=0;
//	virtual bool	Seek( long aPos, unsigned char aFrom)=0;
//	virtual long	GetPos()=0;
//	virtual long	GetSize()=0;
//	virtual void	Close()=0;
//	virtual const	wchar_t* GetURL()=0;
//};

//struct RESM_NO_VTABLE resIResourceNotify
//{
//	virtual omsresult OnResourceNotify( resIResource* aResource, unsigned int aNotifyCode)=0;
//};
//
//struct RESM_NO_VTABLE resIAsynchResource
//{
//	virtual omsresult OnLoaded( )=0;
//};

//struct RESM_NO_VTABLE resIResourceManager
//{
//	virtual omsresult OpenAsynchResource(const wchar_t* apwcURL, resIResource* &ares, void* apNotifier, unsigned int dwFlags=0)=0;
//	virtual omsresult OpenResource(const wchar_t* apwcURL, resIResource* &ares, unsigned int dwFlags=0)=0;
//	virtual omsresult OpenToWrite(const wchar_t* apwcURL, resIWrite* &ares)=0;
//	virtual bool ResourceExists(const wchar_t* apwcURL, unsigned int dwFlags=0)=0;
//	virtual bool ModuleExists(const wchar_t* apwcModuleID)=0;
//	virtual wchar_t* GetFullPath(const wchar_t* apwcURL)=0;
//	virtual wchar_t* GetLastError()=0;
//	virtual omsresult CopyResource(const wchar_t* apwcSrcURL, const wchar_t* apwcDestURL)=0;
//	virtual const wchar_t* Extract(const wchar_t* apwcSrc, const wchar_t* apwcSubPath)=0;
//};


mlRMMLElement* mlRMMLResource::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLResource* pNewEL=(mlRMMLResource*)GET_RMMLEL(mcx, mlRMMLResource::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

bool mlRMMLResource::Freeze(){
	if(!mlRMMLElement::Freeze()) return false; 
	// ??
	return true;
}

bool mlRMMLResource::Unfreeze(){
	if(!mlRMMLElement::Unfreeze()) return false; 
	// ??
	return true;
}

bool mlRMMLResource::IsExist( const wchar_t* apwcPath, bool abLocal, bool abSync){
	Reset();
	omsContext* pContext = GPSM(mcx)->GetContext();
	DWORD dwFlags = (abLocal == true) ? RES_LOCAL : (RES_REMOTE | RES_CACHE_READ);
	unsigned int codeError = 0;
	if( abSync)
		return pContext->mpResM->ResourceExists(apwcPath, dwFlags, true, codeError);
	else
		pAsynchCheckExist = pContext->mpResM->ResourceExistsAsync(this, apwcPath, dwFlags, true);
	return pAsynchCheckExist ? true : false;
}

bool mlRMMLResource::StartDownloading( char aUnpackState, const wchar_t* apwcLocalPath){
	Reset();
	mchUnpackState = aUnpackState;
	omsContext* pContext = GPSM(mcx)->GetContext();
	omsresult res = OMS_OK;
	if(OMS_FAILED(res = pContext->mpResM->OpenResource(GetSrc(), mpResource, RES_REMOTE | RES_CACHE_READ))){
		CallListeners(JSPROP_state);
		return false;
	}
	if (mchUnpackState == 0)
		mpResource->Download(this, apwcLocalPath);
	else
		CallListeners(JSPROP_state);

	return true;
}

bool mlRMMLResource::StartUploading(const wchar_t* apwcDestPath, const wchar_t* apwcSrcPath, bool abPackSource)
{
	// из скрипта будет доступен только путь, куда будет закачан файл в модуль
	jsval vSrc = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx, apwcDestPath));
	JS_SetProperty(mcx, mjso, "src", &vSrc);
	//
	omsContext* pContext = GPSM(mcx)->GetContext();
	omsresult res = OMS_OK;
	mpResource = NULL;
	// открываем исходный ресурс
	res::resIResource* pSrcRes = NULL;
	if(OMS_FAILED(res = pContext->mpResM->OpenResource(apwcSrcPath, pSrcRes, RES_LOCAL))){	
		CallListeners(JSPROP_state);
		return false;
	}
	ML_ASSERTION(mcx, pSrcRes != NULL, "mlRMMLResource::StartUploading");
	// открываем ресурс, куда закачивать
	res::resIWrite* pWrite = NULL;
	if(OMS_FAILED(res = pContext->mpResM->OpenToWrite(GetSrc(), pWrite, RES_ASYNCH | RES_REMOTE | RES_UPLOAD))){
		pSrcRes->Close();
		mpResource = NULL;
		CallListeners(JSPROP_state);
		OnError( RN_UPLOAD_ERROR);
		return false;
	}
	mpResource = pWrite;
	ML_ASSERTION(mcx, mpResource != NULL, "mlRMMLResource::StartUploading");
	// (pSrcRes должен закрыться в Upload(...))
	if(!pWrite->Upload(pSrcRes, this)){
		CallListeners(JSPROP_state);
		return false;
	}
/*
	// копируем в него данные
	for(;;){
		#define MLRES_BUFSIZE 4096
		unsigned char pBuffer[MLRES_BUFSIZE];
		unsigned int uiRead = pSrcRes->Read(pBuffer, MLRES_BUFSIZE);
		if(uiRead == 0)
			break;
		pWrite->Write(pBuffer, uiRead);
	}
*/
	// предполагается, что закачивать на сервер ResManager будет уже асинхронно
	// (mpResource будет закрыт в деструкторе JS-объекта)
	mbUploading = true;
	return true;
}

bool mlRMMLResource::RegisterFileAtResServer(const wchar_t* apwcPath)
{
	// из скрипта будет доступен только путь, куда будет закачан файл в модуль
	jsval vSrc = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx, apwcPath));
	JS_SetProperty(mcx, mjso, "src", &vSrc);
	//
	omsContext* pContext = GPSM(mcx)->GetContext();
	omsresult res = OMS_OK;
	mpResource = NULL;
	// открываем исходный ресурс
	res::resIResource* pRes = NULL;
	if(OMS_FAILED(res = pContext->mpResM->OpenResource(apwcPath, pRes, RES_REMOTE | RES_CACHE_READ))){	
		CallListeners(JSPROP_state);
		return false;
	}
	ML_ASSERTION(mcx, pRes != NULL, "mlRMMLResource::RegisterFileAtResServer");
	mpResource = pRes;
	/*IResource iRes = NULL;
	res->GetIResourceAndClose( &iRes);
	iRes->registerAtResServer();*/
	// (pSrcRes должен закрыться в Upload(...))
	if(!mpResource->RegisterAtResServer( this)){
		CallListeners(JSPROP_state);
		return false;
	}
	return true;
}

// зарегистрировать ресурс на сервере
JSBool mlRMMLResource::JSFUNC_registerFileAtResServer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLResource* priv = (mlRMMLResource*)JS_GetPrivate(cx,obj);
	const wchar_t* pwcPath = L"";
	jsval vArg;
	unsigned int iArg = 0;
	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError(cx, "registerFileAtResServer::First argument of download must be a string");
			return JS_TRUE;
		}
		pwcPath = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}		
	bool bParamNull = (pwcPath == NULL || wcslen(pwcPath) < 1);
	if( bParamNull)
	{
		JS_ReportError(cx, "registerFileAtResServer::Your set src argument");
		return false;
	}	

	if(priv->RegisterFileAtResServer( pwcPath))
		*rval = JSVAL_TRUE;
	else
		*rval = JSVAL_FALSE;
	return JS_TRUE;
}

// проверить существование ресурса (на сервере)
JSBool mlRMMLResource::JSFUNC_isExist(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLResource* priv = (mlRMMLResource*)JS_GetPrivate(cx,obj);
	const wchar_t* pwcPath = L"";
	bool bLocal = false;
	bool bSync = false;
	jsval vArg;
	unsigned int iArg = 0;
	// sFilter
	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError(cx, "First argument of download must be a string");
			return JS_TRUE;
		}
		pwcPath = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}
	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_BOOLEAN(vArg)){
			JS_ReportError(cx, "Second argument of download must be a boolean");
		}
		else
			bLocal = JSVAL_TO_BOOLEAN(vArg);
	}

	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_BOOLEAN(vArg)){
			JS_ReportError(cx, "Third argument of download must be a boolean");
		}
		else
			bSync = JSVAL_TO_BOOLEAN(vArg);
	}
	const wchar_t *pSrc = priv->GetSrc();
	bool bParamNull = (pwcPath == NULL || wcslen(pwcPath) < 1);
	if( pSrc == NULL && bParamNull)
	{
		JS_ReportError(cx, "Your set src or one argument");
		return false;
	}
	if( bParamNull)
		pwcPath = pSrc;

	if(priv->IsExist( pwcPath, bLocal, bSync))
		*rval = JSVAL_TRUE;
	else
		*rval = JSVAL_FALSE;
	return JS_TRUE;
}

// начать закачивание ресурса (но не распаковывать)
JSBool mlRMMLResource::JSFUNC_download(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLResource* priv = (mlRMMLResource*)JS_GetPrivate(cx,obj);
	const wchar_t* pwcLocalPath = L"";
	jsval vArg;
	unsigned int iArg = 0;
	// sFilter
	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError(cx, "First argument of download must be a string");
			return JS_TRUE;
		}
		pwcLocalPath = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}
	if(priv->StartDownloading( 0, pwcLocalPath))
		*rval = JSVAL_TRUE;
	else
		*rval = JSVAL_FALSE;
	return JS_TRUE;
}

// начать закачивание и распаковку
JSBool mlRMMLResource::JSFUNC_downloadAndUnpack(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLResource* priv = (mlRMMLResource*)JS_GetPrivate(cx,obj);
	if(priv->StartDownloading( 1))
		*rval = JSVAL_TRUE;
	else
		*rval = JSVAL_FALSE;
	return JS_TRUE;
}

// получить состояние закачки/распаковки в процентах
JSBool mlRMMLResource::JSFUNC_getProgress(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLResource* priv = (mlRMMLResource*)JS_GetPrivate(cx,obj);
	if(priv->mpResource == NULL){
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}
	res::resIResource* pResource = priv->mpResource;
// если ресурс не существует, то 0
// если ресурс существует на сервере, но не закачан
	// и не закачивается, то 1
	// и закачивается, то 2
// если ресурс существует локально или на CD, то 3
// если ресурс существует локально или на CD, 
	// то если он распаковывается, то 4
	// а если уже распакован, то 5
	unsigned int uiState = priv->GetState();
	unsigned int uiPercent = 0;
	const unsigned int uiMaxPercentToUnpack = 20;
	// если закачивается
	if(uiState == 2){
		long lSize = pResource->GetSize();
		long lDownloadedSize = pResource->GetDownloadedSize();
		double dRate = ((double)lDownloadedSize) / ((double)lSize);
		if(dRate < 0.0) dRate = 0.0;
		if(dRate > 1.0) dRate = 1.0;
		// и собираемся распаковывать
		if(priv->mchUnpackState == 1){
			// то max - 80%
			uiPercent = (dRate * ((double)(100 - uiMaxPercentToUnpack)));
		}else{
			// max - 100%
			uiPercent = (dRate * 100.0);
		}
	/*}else if(uiState == 4){ // если распаковывается,
		uiPercent = 100 - uiMaxPercentToUnpack;
		unsigned int uiUnpackingProgress = pResource->GetUnpackingProgress();
		uiPercent += (((double)uiUnpackingProgress)/100.0)*((double)uiMaxPercentToUnpack);*/
	}else if(priv->mchUnpackState == 3){ // если распакован
		uiPercent = 100;
	}
	*rval = INT_TO_JSVAL(uiPercent);
	return JS_TRUE;
}

// скопировать закачанный и распакованный ресурс в заданную дирректорию
JSBool mlRMMLResource::JSFUNC_copyTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLResource* priv = (mlRMMLResource*)JS_GetPrivate(cx,obj);
	// ??
	return JS_TRUE;
}

mlresult mlRMMLResource::Update(const __int64 alTime){
//	if(mpResource == NULL) return ML_OK;
//	// если идет процесс распаковки
//	if(mchUnpackState == 2){
//		// то проверяем: может уже распаковалось?
//		unsigned int uiUnpachingProgress = mpResource->GetUnpackingProgress();
//		if(uiUnpachingProgress == 100){
//			// если распаковалось, то:
//			CallListeners(EVID_onLoad);
//			mchUnpackState = 3;
//		}
//	}
	return ML_ERROR_NOT_IMPLEMENTED;
}

void mlRMMLResource::StateChanged(){
	mlSynchData Data;
	GPSM(mcx)->SaveInternalPath(Data,this);
	Data << false;
	// ставим в очередь событий
	GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(JSPROP_state, true, 
		(unsigned char*)Data.data(), Data.size());
}

// реализация moILoadable
bool mlRMMLResource::SrcChanged(){
	Reset();
	return true;
}

// реализация res::resIAsynchResource
omsresult mlRMMLResource::OnLoaded(){
	if(mpResource == NULL)
		return OMS_ERROR_FAILURE;
	if(mchUnpackState == 1){
		mchUnpackState = 2;
		//mpResource->Unpack(this);
	}else{
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << false;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onLoad, false, 
			(unsigned char*)Data.data(), Data.size());
		mchUnpackState = 0;
	}
	StateChanged();
	return OMS_OK;
}

omsresult mlRMMLResource::OnDownloaded(){
	if(mpResource == NULL)
		return OMS_ERROR_FAILURE;
	if(mchUnpackState == 1){
		mchUnpackState = 2;
		//mpResource->Unpack(this);
	}else{
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << false;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onLoad, false, 
			(unsigned char*)Data.data(), Data.size());
		mchUnpackState = 0;
	}
	StateChanged();
	// ??
	return OMS_OK;
}

omsresult mlRMMLResource::OnCached(){
	if(mpResource == NULL)
		return OMS_ERROR_FAILURE;
	// ??
	StateChanged();
	return OMS_OK;
}

omsresult mlRMMLResource::OnUnpacked(){
	if(mpResource == NULL)
		return OMS_ERROR_FAILURE;
	// если идет процесс распаковки
	if(mchUnpackState == 2){
		mchUnpackState = 3;
		OnDownloaded();
	}
	else
		StateChanged();
	return OMS_OK;
}

omsresult mlRMMLResource::OnError(unsigned int auiCode, unsigned int auiServerCode, wchar_t* aNotifyServerText){
	if(mpResource == NULL)
	{
		ILogWriter *pLogWriter = GPSM(mcx)->GetContext()->mpLogWriter;
		if( pLogWriter != NULL)
		{
			pLogWriter->WriteLnLPCSTR("mlRMMLResource::OnError::mpResource==NULL");
		}
		return OMS_ERROR_FAILURE;
	}
	mlSynchData Data;
	GPSM(mcx)->SaveInternalPath(Data,this);
	Data << false;
	Data << auiCode;
	Data << auiServerCode;
	if (aNotifyServerText != NULL)
		Data << aNotifyServerText;
	else
		Data << L"";
	// ставим в очередь событий
	GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onLoadError, false, 
		(unsigned char*)Data.data(), Data.size());

	//	Reset();	//Alex 2009-06-29. Если вызывается из потока аплоада, то тут при закрытии виснет
	StateChanged();
	return OMS_OK;
}

omsresult mlRMMLResource::OnUploaded(wchar_t* aNotifyServerText){
	if(mpResource == NULL)
		return OMS_ERROR_FAILURE;
	long lSize = mpResource->GetSize();
	long lUploadedSize = mpResource->GetUploadedSize();
	if(lSize != lUploadedSize){
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);		
		Data << false;		
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(JSPROP_uploaded, true, 
			(unsigned char*)Data.data(), Data.size());
	}else{
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		int state = 0;
		Data << state;
		mwsWarning = aNotifyServerText ? aNotifyServerText : L"";

//		GUARD_JS_CALLS(mcx);
//		jsval vWarningText = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx, ));
//		JS_SetProperty(mcx, mjso, "warningText", &vWarningText);

		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onLoad, false, 
			(unsigned char*)Data.data(), Data.size());
		StateChanged();
	}
	return OMS_OK;
}

omsresult mlRMMLResource::OnRegistered(wchar_t* aNotifyServerText){
	if(mpResource == NULL)
	{
		ILogWriter *pLogWriter = GPSM(mcx)->GetContext()->mpLogWriter;
		if( pLogWriter != NULL)
		{
			pLogWriter->WriteLnLPCSTR("mlRMMLResource::OnRegistered::mpResource==NULL");
		}
		return OMS_ERROR_FAILURE;
	}
	
	mlSynchData Data;
	GPSM(mcx)->SaveInternalPath(Data,this);
	int state = 0;
	Data << state;
	mwsWarning = aNotifyServerText ? aNotifyServerText : L"";

	// ставим в очередь событий
	GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onLoad, false, 
		(unsigned char*)Data.data(), Data.size());
	StateChanged();
	return OMS_OK;
}

omsresult mlRMMLResource::OnExisted( bool abCode){
	if(mchUnpackState == 1){
		mchUnpackState = 2;
		//mpResource->Unpack(this);
	}else{
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		//Data << false;
		// ставим в очередь событий
		if(abCode)
		{
			GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onLoad, false, 
				(unsigned char*)Data.data(), Data.size());
		}
		else
		{
			Data << 1;
			Data << 0;
			// ставим в очередь событий
			GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onLoadError, false, 
				(unsigned char*)Data.data(), Data.size());
		}
		mchUnpackState = 0;
	}
	StateChanged();
	// ??
	return OMS_OK;
}

}
