
#include "mlRMML.h"
#include "mlSPParser.h"
#include "mlRMMLModule.h"
#include "mlSynchData.h"
#include <fcntl.h>
#include <rpc.h>
#include "zlibengn.h"
#include "shlobj.h"
#include <algorithm>
#include <time.h>
#include <stdlib.h>


namespace rmml {

mlRMMLModule::mlRMMLModule(void):
	MP_WSTRING_INIT(msModuleID),
	MP_WSTRING_INIT(msAlias)
{
	mpManifest = NULL;
	mpTechData = NULL;
	mjssModuleID = NULL;
}

mlRMMLModule::~mlRMMLModule(void){
	moduleClose();
}

JSBool mlRMMLModule::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlRMMLModule::JSConstructor(cx, obj, argc, argv, rval) == JS_FALSE)
		return JS_FALSE;
	// ??
	return JS_TRUE;
}

#define SMM_ENC_MASK 0x40

#define UPLOAD_EXT	L".zlib"

enum{
	MDLENC_LOCAL,
	MDLENC_UTF8,
	MDLENC_UNICODE,
	MDLENC_MAX
};

///// JavaScript Variable Table
JSPropertySpec mlRMMLModule::_JSPropertySpec[] = {
	JSPROP_RO(identifier)
	JSPROP_RO(manifest)
	JSPROP_RO(techData)
	JSPROP_RO(MS_NOT_EXISTS)		// 0 - модуль не существует
	JSPROP_RO(MS_CREATION_FAILED)	// 1 - ошибка при создании
	{ "LOCAL", MDLENC_LOCAL|SMM_ENC_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},
	{ "UTF8", MDLENC_UTF8|SMM_ENC_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},
	{ "UNICODE", MDLENC_UNICODE|SMM_ENC_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLModule::_JSFunctionSpec[] = {
	// создать модуль 
	// sModuleID - идентификатор модуля (реально к нему добавляется GUID и ".result")
	// sAlias - псевдоним модуля
	// bRemote - создавать на сервере или в локальном хранилище на компьютере клиента
	JSFUNC(create,2)	// bResult = create(sModuleID[, sAlias[, bRemote]])
	JSFUNC(open,2)		// bResult = open(sModuleID)
	JSFUNC(close,0)		//           close()
	// прочитать текстовый файл из модуля в строку
	JSFUNC(readText,2)	// sText   = readText(sResPath[, iEncloding]) // iEncloding == [ LOCAL | UTF8 | UNICODE ]
	// записать текстовый файл в модуль в указанной кодировке
	JSFUNC(writeText,3)	// bResult = writeText(sResPath, sText[, iEncoding])
	// сохранить медиа-объект в модуль в виде файла (пока работает только для картинок)
	JSFUNC(saveMedia,2)	// bResult = saveMedia(oMedia[, sResPath])
	// скопировать ресурс из другого модуля в этот
	JSFUNC(copy,2)		// bResult = copy(sSrcPath, sDestPath)
	// сгенерировать новое имя файла, какого еще нет в указанной папке в модуле
	// ?? возможно надо будет реализовать этот метод позже. Пока в проекте
	// JSFUNC(getUniqueName, 1)	// sName = getUniqueName([sPath])
	// сохранить в модуле ресурс 
	// (возможно локальный в модуле на сервере)
	// если sSrcPath не указан, то запросить путь к файлу через диалоговое окно открытия файла
	JSFUNC(saveResource,2)	// oResource = saveResource(sDestPath[, sSrcPath])
	// сохраняет в модуль в определенную папку все файлы, которые указаны в массиве asSrcPaths
	// если asSrcPaths не указан, то пути к файлам запрашиваются через диалоговое окно
	// (имена файлов сохраняются)
	// ?? возможно надо будет реализовать этот метод позже. Пока в проекте
	// JSFUNC(saveResources,2)	// aResources = saveResources(sDestPath[, asSrcPaths])
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLModule::_EventSpec[] = {
	MLEVENT(onOpened)			// модуль успешно открыт
	MLEVENT(onCreated)			// модуль успешно создан
	MLEVENT(onError)			// возникла ошибка при создании или открытии модуля
	{0, 0, 0}
};

MLJSCLASS_IMPL_BEGIN(Module,mlRMMLModule,0)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLModule)

JSBool mlRMMLModule::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLModule);
		default:
			switch(iID){
			case JSPROP_identifier:
				// ??
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

JSBool mlRMMLModule::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLModule);
		default:
			if(iID & SMM_ENC_MASK){
				*vp = INT_TO_JSVAL( iID - SMM_ENC_MASK);
				return JS_TRUE;
			}
			switch(iID){
			case JSPROP_identifier:
				if(priv->mjssModuleID == NULL){
					*vp = JSVAL_NULL;
				}else
					*vp = STRING_TO_JSVAL( priv->mjssModuleID);
				break;
			case JSPROP_manifest:
				if(priv->mpManifest == NULL){
					*vp = JSVAL_NULL;
				}else
					*vp = OBJECT_TO_JSVAL(priv->mpManifest->mjso);
				break;
			case JSPROP_techData:
				if(priv->mpTechData == NULL){
					*vp = JSVAL_NULL;
				}else
					*vp = OBJECT_TO_JSVAL(priv->mpTechData->mjso);
				break;
			case JSPROP_MS_NOT_EXISTS:		// 0 - модуль не существует
				*vp = INT_TO_JSVAL(0);
				break;
			case JSPROP_MS_CREATION_FAILED:	// 1 - ошибка при создании
				*vp = INT_TO_JSVAL(1);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

JSBool mlRMMLModule::JSFUNC_create(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLModule* pMLEl = (mlRMMLModule*)JS_GetPrivate(cx, obj);
	mlTrace(pMLEl->mcx, "Create module called %u\n", (unsigned long)pMLEl);
	pMLEl->moduleCreateOpen( true, argc, argv, rval);
	return JS_TRUE;
}

JSBool mlRMMLModule::JSFUNC_open(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLModule* pMLEl = (mlRMMLModule*)JS_GetPrivate(cx, obj);
	mlTrace(pMLEl->mcx, "Open module called %u\n", (unsigned long)pMLEl);
	pMLEl->moduleCreateOpen( false, argc, argv, rval);
	return JS_TRUE;
}

JSBool mlRMMLModule::JSFUNC_close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc > 0){
		JS_ReportError(cx,"close method has no arguments");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}
	mlRMMLModule* pMLEl = (mlRMMLModule*)JS_GetPrivate(cx, obj);
	pMLEl->moduleClose();
	
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

bool local2wstr( const unsigned char* apBuffer, wchar_t** apwcStr)
{
	size_t iLen = 0;
	size_t iConverted = 0;
	mbstowcs_s(&iLen, NULL, 0, (const char*)apBuffer, 0);
	*apwcStr = MP_NEW_ARR( wchar_t, iLen+1);
	mbstowcs_s(&iConverted, *apwcStr, iLen+1, (const char*)apBuffer, iLen);
	(*apwcStr)[ iLen] = 0L;
	return true;
}

bool utf82wstr( const unsigned char* aUTF8, wchar_t** aWStr)
{
	*aWStr = MP_NEW_ARR( wchar_t, strlen( (const char*)aUTF8));
	wchar_t* wstr = *aWStr;
	for (unsigned char ch = *aUTF8;  ch != '\0';  ch = *(++aUTF8))
	{
		if ((ch & 0x80) == 0 )
			*wstr = ch;
		else
		{
			char chTemp;
			unsigned uLength;
			if ((ch & 0xF0) == 0xF0 ) { uLength = 4; chTemp = (char)0xF0; }
			else if( (ch & 0xE0) == 0xE0 ) { uLength = 3; chTemp = (char)0xE0; }
			else if( (ch & 0xC0) == 0xC0 ) { uLength = 2; chTemp = (char)0xC0; }
			else break;
			uLength--;
			*wstr = (ch & ~chTemp) << (uLength * 6);

			ch=*(++aUTF8); uLength--;
			*wstr |= (ch & 0x3F) << (uLength * 6);
			if( uLength ){
				ch=*(++aUTF8); uLength--;
				*wstr |= (ch & 0x3F) << (uLength * 6);
				if( uLength ){
					ch=*(++aUTF8); uLength--;
					*wstr |= (ch & 0x3F);
				}
			}
		}
		wstr++;
	}
	*wstr=L'\0';
	return true;
}

bool unicode2wstr( const unsigned char* apBuffer, wchar_t** apwcStr)
{
	*apwcStr = (wchar_t*)apBuffer;
	return false;
}

static bool (*other2wstr[ 3])( const unsigned char*, wchar_t**) = {
	local2wstr, utf82wstr, unicode2wstr
};

JSBool mlRMMLModule::JSFUNC_readText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JS_GetEmptyStringValue(cx);
	if (argc < 1){
		JS_ReportError( cx,"readText method must get resource path at least");
		return JS_TRUE;
	}
	mlRMMLModule* pMLEl = (mlRMMLModule*)JS_GetPrivate(cx, obj);
	
	jsval vArg = argv[0];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportError( cx,"readText method must get a string as first argument");
		return false;
	}
	mlString sResPath = pMLEl->msAlias;
	sResPath += L':';
	sResPath += wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	

	int iEncoding = MDLENC_LOCAL;
	if (argc >= 2){
		vArg = argv[ 1];
		if(!JSVAL_IS_INT(vArg)){
			JS_ReportError( cx,"readText method must get a number as second argument");
			return false;
		}
		iEncoding = JSVAL_TO_INT(vArg);
		if (iEncoding >= MDLENC_MAX)
			iEncoding = MDLENC_LOCAL;
	}

	res::resIResource* pReadRes;
	if (GPSM(cx)->GetContext()->mpResM->OpenResource( sResPath.c_str(), pReadRes) == OMS_OK)
	{
		mlSynchData oData;
		#define TEXT_BUFFER_SIZE 128000
		unsigned char* pBuffer = MP_NEW_ARR( unsigned char, TEXT_BUFFER_SIZE + 1);
		unsigned int iRead;
		do {
			unsigned int errorRead = 0;
			iRead = pReadRes->Read( pBuffer, TEXT_BUFFER_SIZE, &errorRead);
			oData.put( pBuffer, iRead);
		} while (iRead == TEXT_BUFFER_SIZE);
		MP_DELETE_ARR( pBuffer);
		pReadRes->Close();

		unsigned int iZero = 0;
		oData << iZero;
		oData << iZero;
		wchar_t* wsText;
		bool bFree = other2wstr[ iEncoding]( (const unsigned char*)oData.data(), &wsText);
		*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, wsText));
		if (bFree)
			MP_DELETE_ARR( wsText);
	}
	
	return JS_TRUE;
}

struct MaskItem
{
	unsigned chMask;
	unsigned uShift;
	unsigned uSize;
	unsigned wcMax;
};
static const MaskItem _utf8string_pMaskItem[] = 
{
  { 0x80,  0*6, 1, 0x7F,   },   // 1 byte sequence
  { 0xC0,  1*6, 2, 0x7FF,  },   // 2 byte sequence
  { 0xE0,  2*6, 3, 0xFFFF, },   // 3 byte sequence
  { 0xF0,  3*6, 4, 0x1FFFFF, }, // 4 byte sequence
};

bool wstr2local( const wchar_t* apwcStr, unsigned char** apBuffer, unsigned int* auiBufSize)
{
	size_t bufSize = 0;
	size_t converted = 0;
	wcstombs_s(&bufSize, NULL, 0, apwcStr, 0);
	*auiBufSize = bufSize + 1;
	*apBuffer = MP_NEW_ARR( unsigned char, *auiBufSize);
	wcstombs_s(&converted, (char*)*apBuffer, *auiBufSize, apwcStr, bufSize);
	*auiBufSize = converted;
	return true;
}

bool wstr2utf8( const wchar_t* apwcStr, unsigned char** apBuffer, unsigned int* auiBufSize)
{
   unsigned char* pbszMax;
   unsigned char* pbszUTF8;
   unsigned uShift;
   const MaskItem* pMaskItem;
   const wchar_t* wstr = apwcStr;
   DWORD dwLength = wcslen( wstr);
   const wchar_t* pwszEnd = wstr + dwLength;

   *apBuffer = MP_NEW_ARR( unsigned char, dwLength * 5);
   *auiBufSize = 0;

   pbszUTF8 = *apBuffer;
   pbszMax = *apBuffer + dwLength * 2;

   while( (*wstr != L'\0') && (wstr != pwszEnd) && (pbszUTF8 < pbszMax) )
   {
      if( *wstr < 0x80 ) 
      {
         *pbszUTF8 = (unsigned char)*wstr;
         pbszUTF8++;
      }
      else
      {
         for (pMaskItem = &_utf8string_pMaskItem[1]; pMaskItem->chMask; pMaskItem++ )
         {
            if (*wstr < pMaskItem->wcMax ) break;
         }

         // Check size for buffer
         if (pbszUTF8 > (pbszMax - pMaskItem->uSize)) break;

         uShift = pMaskItem->uShift;
         *pbszUTF8 = (unsigned char)(pMaskItem->chMask | (*wstr >> uShift));
         pbszUTF8++;
         while (uShift > 0)
         {
            uShift -= 6;
            *pbszUTF8 = (unsigned char)(0x80 | (*wstr >> uShift)) & 0xBF;
            pbszUTF8++;
         }
      }
      wstr++;
   }

   *pbszUTF8 = '\0';
   *auiBufSize = pbszUTF8 - *apBuffer;
	return true;
}

bool wstr2unicode( const wchar_t* apwcStr, unsigned char** apBuffer, unsigned int* auiBufSize)
{
	*apBuffer = (unsigned char*)apwcStr;
	*auiBufSize = wcslen( apwcStr);
	return false;
}

static bool (*wstr2other[ 3]) ( const wchar_t*, unsigned char**, unsigned int*) = {
	wstr2local, wstr2utf8, wstr2unicode
};

JSBool mlRMMLModule::JSFUNC_writeText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval=JSVAL_FALSE;
	if (argc < 2){
		JS_ReportError( cx,"writeText method must get two arguments at least");
		return JS_TRUE;
	}
	mlRMMLModule* pMLEl = (mlRMMLModule*)JS_GetPrivate(cx, obj);
	jsval vArg = argv[0];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportError( cx,"writeText method must get a string as first argument");
		return false;
	}
	mlString sResPath = pMLEl->msAlias;
	sResPath += L':';
	sResPath += wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	
	vArg = argv[1];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportError( cx,"writeText method must get a string as second argument");
		return false;
	}
	mlString sText = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	
	int iEncoding = MDLENC_LOCAL;
	if (argc >= 3){
		vArg = argv[2];
		if(!JSVAL_IS_INT(vArg)){
			JS_ReportError( cx,"readText method must get a number as third argument");
			return false;
		}
		iEncoding = JSVAL_TO_INT(vArg);
		if (iEncoding >= MDLENC_MAX)
			iEncoding = MDLENC_LOCAL;
	}

	res::resIWrite* pWrieRes;
	if (GPSM(cx)->GetContext()->mpResM->OpenToWrite( sResPath.c_str(), pWrieRes) == OMS_OK)
	{
		unsigned char* pBuffer = NULL;
		unsigned int iToWrite;
		bool bFree = wstr2other[ iEncoding]( sText.c_str(), &pBuffer, &iToWrite);
		if (pWrieRes->Write( pBuffer, iToWrite) == iToWrite)
			*rval = JSVAL_TRUE;
		pWrieRes->Close();
		if (bFree && pBuffer != NULL)
			MP_DELETE_ARR( pBuffer);
	}
	
	return JS_TRUE;
}

JSBool mlRMMLModule::JSFUNC_copy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval=JSVAL_FALSE;
	if (argc < 1){
		JS_ReportError( cx,"copy method must get one argument at least");
		return JS_TRUE;
	}
	mlRMMLModule* pMLEl = (mlRMMLModule*)JS_GetPrivate(cx, obj);
	jsval vArg = argv[0];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportError( cx,"copy method must get a string argument");
		return false;
	}
	mlString sSrcPath = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	sSrcPath = GPSM(cx)->GetFullResPath( sSrcPath.c_str());
	
	mlString sDestPath = pMLEl->msAlias;
	sDestPath += L':';
	if (argc >= 1)
	{
		vArg = argv[1];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError( cx,"copy method must get a string argument");
			return false;
		}
		sDestPath += wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}
	else
	{
		int pos = sSrcPath.rfind( L':');
		if (pos == mlString::npos)
			sDestPath += sSrcPath;
		else
			sDestPath += sSrcPath.substr( pos);
	}

	if (GPSM(cx)->GetContext()->mpResM->CopyResource( sSrcPath.c_str(), sDestPath.c_str()) == OMS_OK)
		*rval = JSVAL_TRUE;
	
	return JS_TRUE;
}

// сохранить в модуле ресурс 
// (возможно локальный в модуле на сервере)
// если sSrcPath не указан, то запросить путь к файлу через диалоговое окно открытия файла
// sFilter - фильтр имен файлов в формате "All|*.*|Text|*.txt"
//JSFUNC(saveResource,2)
// oResource = saveResource(sDestPath[, sSrcPath])
// oResource = saveResource(sDestPath[, sFilter])
JSBool mlRMMLModule::JSFUNC_saveResource(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc < 1){
		JS_ReportError( cx,"saveResource-method must get one argument at least");
		return JS_TRUE;
	}
	mlRMMLModule* pMLEl = (mlRMMLModule*)JS_GetPrivate(cx, obj);
	//mlString sDestPath = pMLEl->msAlias;
	//sDestPath += L':';
	mlString sDestPath = L":";
	const wchar_t *psModuleID = pMLEl->msModuleID.c_str();
	if ( psModuleID[ 0] == L':')
		psModuleID++;
	sDestPath += pMLEl->msAlias;//psModuleID;
	sDestPath += L':';
	jsval vArg = argv[0];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportError( cx,"saveResource-method must get a string argument");
		return false;
	}

	mlString destFileName = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	unsigned char *sUUID;

	std::wstring::size_type guidIndex = destFileName.find( L"{guid}");
	if ( guidIndex != std::wstring::npos)
	{
		std::wstring	sBefore = destFileName.substr( 0, guidIndex);
		std::wstring	sAfter = destFileName.substr( guidIndex + 6, destFileName.size() - (guidIndex + 6));

		// генерация уникального имени файла 
		UUID uuid;
		if ( UuidCreate( &uuid) != RPC_S_OK)
		{
			JS_ReportError( cx,"saveResource-method generated uuid is failed");
			return false;
		}
		
		if ( UuidToString( &uuid, &sUUID) != RPC_S_OK)
		{
			JS_ReportError( cx,"saveResource-method generated string from uuid is failed");
			return false;
		}
		destFileName.clear();
		destFileName.append( sBefore);
		destFileName.append( MB2WC( (const char*) sUUID));		
		destFileName.append( sAfter);
		if ( RpcStringFree( &sUUID) != RPC_S_OK)
		{
			JS_ReportError( cx,"saveResource-method generated uuid string deleting is failed");
		}
	}
	
	sDestPath += destFileName;
	//
	mlString sSrcPath;
	bool bArgsCorrect = false;
	if(argc > 1){
		jsval vArg = argv[1];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError( cx,"saveResource-method must get a string argument");
			return false;
		}
		sSrcPath = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
		bArgsCorrect = true;
	}

	bool bPack = false;
	if(argc > 2)
	{
		jsval vArg = argv[argc - 1];
		if ((!JSVAL_IS_BOOLEAN(vArg)) && (!bArgsCorrect)) {
			JS_ReportError( cx,"saveResource-method must get a bool argument");
			return false;
		}
		bPack = JSVAL_TO_BOOLEAN(vArg);
	}

	// если путь к исходному файлу не задан аргументом
	// или дан фильтр для файлов (есть '|' или '*')
	mlString sFilter = L"";
	if(sSrcPath.find('|') != mlString::npos && sSrcPath.find('*') != mlString::npos){
		sFilter = sSrcPath;
		sSrcPath = L"";
	}
	if(sSrcPath == L""){
		// то запросить его через диалоговое окно
		omsContext* pContext = GPSM(cx)->GetContext();
		wchar_t pwcPath[3000] = L"";
		if(pContext->mpApp)
		{
			mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
			pSM->JSScriptCancelTerimation();

			if(!(pContext->mpApp->GetLocalFilePath(sFilter.c_str(), L"", true, pwcPath, 2995, L"")))
				return JS_TRUE;
			sSrcPath = pwcPath;
		}
	}

	if(sSrcPath == L"")
		return JS_TRUE;

	if (bPack)
	{
		wchar_t* tmp = MP_NEW_ARR( wchar_t, MAX_PATH);
		SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
		mlString newPath = tmp;
		srand((unsigned)time(NULL));
		int rnd = rand();
		wchar_t* str = MP_NEW_ARR( wchar_t, 20);
		_itow_s(rnd, str, 20, 10);
		newPath += L"\\";
		newPath += str;
		newPath += UPLOAD_EXT;
		MP_DELETE_ARR( str);

		omsContext* pContext = GPSM(cx)->GetContext();
		pContext->mpResM->PackResource((wchar_t*)sSrcPath.c_str(), (wchar_t*)newPath.c_str());

		MP_DELETE_ARR( tmp);
		sSrcPath = newPath;
		sDestPath += UPLOAD_EXT;	
	}

	JSObject* jsoResource = mlRMMLResource::newJSObject(cx);
	ML_ASSERTION(cx, jsoResource != NULL, "mlRMMLModule::JSFUNC_saveResource");
	mlRMMLResource* pResource = (mlRMMLResource*)JS_GetPrivate(cx, jsoResource);
mlTrace(cx, "Resource created %u\n", (unsigned long)pResource);
	if( pResource->StartUploading(sDestPath.c_str(), sSrcPath.c_str(), bPack))
	{
		*rval = OBJECT_TO_JSVAL(jsoResource);
	}
	return JS_TRUE;
}

JSBool mlRMMLModule::JSFUNC_saveMedia(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval=JSVAL_FALSE;
	if (argc < 1){
		JS_ReportError( cx,"copy method must get one argument at least");
		return JS_TRUE;
	}
	mlRMMLModule* pMLEl = (mlRMMLModule*)JS_GetPrivate(cx, obj);
	jsval vArg = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vArg)){
		JS_ReportError( cx,"copy method must get a string argument");
		return false;
	}
	JSObject* jsoMedia = JSVAL_TO_OBJECT(vArg);

	if (JSO_IS_MLEL(cx, jsoMedia))
	{
		mlRMMLElement* pMLElMedia = GET_RMMLEL(cx, jsoMedia);
		mlString sDestPath = pMLEl->msAlias;
		sDestPath += L':';
		if (argc > 1)
		{
			vArg = argv[1];
			if(!JSVAL_IS_STRING(vArg)){
				JS_ReportError( cx,"saveMedia method must get a string argument");
				return false;
			}
			sDestPath += wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
		}
		else
		{
			const wchar_t* pwcSrc = pMLElMedia->GetLoadable()->GetSrc();
			const wchar_t* pwcSrcPath = wcsrchr( pwcSrc, L':');
			if (pwcSrcPath == NULL)
				pwcSrcPath = pwcSrc;
			else
				pwcSrcPath++;
			sDestPath += pwcSrcPath;
		}
		if (GPSM(cx)->GetContext()->mpRM->SaveMedia( pMLElMedia, sDestPath.c_str()) == OMS_OK)
			*rval = JSVAL_TRUE;
	}
	
	return JS_TRUE;
}

void mlRMMLModule::loadMetadata()
{
	// gpSM->
	JSObject* jsoManifest = mlRMMLXML::newJSObject(mcx);
	mpManifest = (mlRMMLXML*)JS_GetPrivate(mcx, jsoManifest);
	//mlString sSrc = msAlias;
	//sSrc += L":\\"
	//jsval vSrc = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, pwcSrc));
	//JS_SetProperty(cx, jsoManifest, "src", &vSrc);
	//pMLEl->mpManifest->SetProperty();
	//
	JSObject* jsoTechData = mlRMMLXML::newJSObject(mcx);
	mpTechData = (mlRMMLXML*)JS_GetPrivate(mcx, jsoTechData);
}

bool mlRMMLModule::moduleCreateOpen( bool aCreate, uintN argc, jsval *argv, jsval *rval)
{
	*rval=JSVAL_FALSE;
	const char* sMethodName = (aCreate ? "create" : "open");
	if (argc < 1){
		JS_ReportError( mcx,"%s method must get module identifier at least", sMethodName);
		return false;
	}
	jsval vArg = argv[0];
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportError( mcx,"%s method must get a string argument", sMethodName);
		return false;
	}
	mjssModuleID = JSVAL_TO_STRING(vArg);
	msModuleID = wr_JS_GetStringChars( mjssModuleID);
	if (aCreate)
	{
		wchar_t wsGUID[ 40];
		wchar_t *pGUID = wsGUID;
		GPSM(mcx)->GetContext()->mpApp->GetStringGUID( pGUID, 40);
		if (pGUID[ 0] != 0)
			pGUID++[ 37] = 0;
		if (JS_GetStringLength( mjssModuleID) != 0)
		{
			if (pGUID[ 0] != 0)
				(--pGUID)[ 0] = L'.';
		}
		else if (pGUID[ 0] == 0)
			return false;
//		msModuleID += L".result";//pGUID;
		mjssModuleID = wr_JS_NewUCStringCopyZ( mcx, msModuleID.c_str());
	}
	mlString sModuleID = msModuleID;
	const wchar_t* pwcModuleAlias = NULL;
	if(argc >= 2){
		jsval vArg = argv[1];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError( mcx,"%s method must get a string argument", sMethodName);
			return false;
		}
		pwcModuleAlias = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	} else
		pwcModuleAlias = msModuleID.c_str();
	msAlias = pwcModuleAlias;
	if (msAlias[ 0] != L':')
		msAlias.insert( 0, L":");
	msAlias += L":";
	msAlias = GPSM(mcx)->GetFullResPath( msAlias.c_str());
	msAlias.resize( msAlias.rfind( L':'));
	if (msAlias[ 0] == L':')
		msAlias.erase( 0, 1);
	if (msModuleID[ 0] == L':')
		msModuleID.erase( 0, 1);
	bool bRemote = false;
	if(aCreate && argc >= 3){
		jsval vArg = argv[2];
		if(!JSVAL_IS_BOOLEAN(vArg)){
			JS_ReportError( mcx,"%s method must get a boolean argument", sMethodName);
			return false;
		}
		bRemote = JSVAL_TO_BOOLEAN(vArg);
	}

	if ((aCreate && GPSM(mcx)->GetContext()->mpResM->CreateModule( msModuleID.c_str(), msAlias.c_str(), bRemote, true, this) == OMS_OK) ||
		(!aCreate && GPSM(mcx)->GetContext()->mpResM->OpenModule( msModuleID.c_str(), msAlias.c_str(), true, this) == OMS_OK))
	{
		*rval=JSVAL_TRUE;
	}
	return true;
}

bool mlRMMLModule::moduleClose()
{
	if( msModuleID != L"")
	{
		GPSM(mcx)->GetContext()->mpResM->CloseModule( msModuleID.c_str(), msAlias.c_str());
		mjssModuleID = NULL;
		msModuleID = L"";
		msAlias.resize( 0);
	}
	if( pThreadModule != NULL)
	{
		pThreadModule->SetResIModuleNotify( NULL);
		pThreadModule->Destroy();
	}
	return true;
}

jsval mlRMMLModule::notifyFunctionIsExists( const wchar_t* lpcFunctionName)
{
// вызвать функцию изменения статуса
	jsval vFunc=JSVAL_NULL;

	if(!wr_JS_GetUCProperty(mcx, mjso, lpcFunctionName, -1, &vFunc))
	{
		mlTraceWarning(mcx, "$S is not defined \n", lpcFunctionName);
		return JSVAL_NULL;
	}
	if(!JSVAL_IS_REAL_OBJECT(vFunc)){
		mlTraceWarning(mcx, "Method %S not found\n", lpcFunctionName);
		return JSVAL_NULL;
	}
	if(!JS_ObjectIsFunction(mcx,JSVAL_TO_OBJECT(vFunc))){
		mlTraceWarning(mcx, "$S is not a function\n", lpcFunctionName);
		return JSVAL_NULL;
	}
	return vFunc;
}

void mlRMMLModule::callNotifyFunction( const wchar_t* lpcFunctionName)
{
	jsval vFunc=JSVAL_NULL;
	if( (vFunc=notifyFunctionIsExists( lpcFunctionName)) == JSVAL_NULL)
		return;
	unsigned char ucArgC = 0;
	jsval* vArgs=NULL;
	jsval v;
	JS_CallFunctionValue(mcx, mjso, vFunc, ucArgC, vArgs, &v);
}

void mlRMMLModule::OnModuleOpened( const wchar_t* lpcModuleName)
{
	mlSynchData data;
	data << (unsigned int)mjso;
	// вызов идет из другого потока, поэтому нельзя делать ничего в JS
	//SAVE_FROM_GC(mcx, GPSM(mcx)->GetPlayerGO(), mjso);
	// ставим в очередь событий
	GPSM(mcx)->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_moduleOnOpened, (unsigned char*)data.data(), data.size());
}

void mlRMMLModule::OnModuleOpenedInternal(mlSynchData& aData){
	loadMetadata();
	CallListeners(EVNM_onOpened);
}

void mlRMMLModule::OnModuleCreated( const wchar_t* lpcModuleName, bool* abIsRemote)
{
	mlSynchData data;
	data << (unsigned int)mjso;
	// вызов идет из другого потока, поэтому нельзя делать ничего в JS
	//SAVE_FROM_GC(mcx, GPSM(mcx)->GetPlayerGO(), mjso);
	// ставим в очередь событий
	GPSM(mcx)->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_moduleOnCreated, (unsigned char*)data.data(), data.size());
}

void mlRMMLModule::OnModuleCreatedInternal(mlSynchData& aData){
	loadMetadata();
	CallListeners(EVNM_onCreated);
}

void mlRMMLModule::OnModuleError( const wchar_t* lpcModuleName, unsigned int aNotifyCode)
{
	mlSynchData data;
	data << (unsigned int)mjso;
	// вызов идет из другого потока, поэтому нельзя делать ничего в JS
	//SAVE_FROM_GC(mcx, GPSM(mcx)->GetPlayerGO(), mjso);
	data << aNotifyCode;
	// ставим в очередь событий
	GPSM(mcx)->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_moduleOnError, (unsigned char*)data.data(), data.size());
}

void mlRMMLModule::SetAsyncModule( IAsyncModuleImpl* aThreadModule)
{
	pThreadModule = aThreadModule;
}

#define MLVP_EVENT_MODULE_ERROR_CODE_PROP_NAME "errorCode"

void mlRMMLModule::OnModuleErrorInternal(mlSynchData& aData){
	unsigned int aNotifyCode = 0;
	aData >> aNotifyCode;
	jsval v = INT_TO_JSVAL(aNotifyCode);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), MLVP_EVENT_MODULE_ERROR_CODE_PROP_NAME, &v);
	CallListeners(EVNM_onError);
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), MLVP_EVENT_MODULE_ERROR_CODE_PROP_NAME);
}

void mlRMMLModule::OnModuleEventInternal(JSContext* acx, mlSynchData& aData, unsigned short ausEventID){
	unsigned int uID;
	aData >> uID;
	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)uID;
	//GET_SAVED_FROM_GC(acx, GPSM(acx)->GetPlayerGO(), jso, &v);
	//if(!JSVAL_IS_REAL_OBJECT(v)){
	//	// ? возможно стоит поругаться 
	//	return;
	//}
	v = OBJECT_TO_JSVAL(jso);	
	JSObject* jsoModule = JSVAL_TO_OBJECT(v);
	mlRMMLModule* pModule = (mlRMMLModule*)JS_GetPrivate(acx, jsoModule);
	if(ausEventID == mlSceneManager::CEID_moduleOnOpened){
		pModule->OnModuleOpenedInternal(aData);
	}else if(ausEventID == mlSceneManager::CEID_moduleOnCreated){
		pModule->OnModuleCreatedInternal(aData);
	}else if(ausEventID == mlSceneManager::CEID_moduleOnError){
		pModule->OnModuleErrorInternal(aData);
	}
	FREE_FOR_GC(acx, GPSM(acx)->GetPlayerGO(), jso);
}

}