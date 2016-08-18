// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: mlRMMLBrowser.cpp,v $
// $Revision: 1.17 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2009/05/06 13:24:36 $
//              $Author: tandy $
//
//////////////////////////////////////////////////////////////////////
#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml
{
mlRMMLBrowser::mlRMMLBrowser():
	MP_WSTRING_INIT(defaultCharset)
{
	mType     = MLMT_BROWSER;
	mRMMLType = MLMT_BROWSER;
	AXFrame = false;
	value = NULL;
	ML_INIT_COLOR(bkgColor)

	mbInMyCall = false;
}

mlRMMLBrowser::~mlRMMLBrowser()
{
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL, "mlRMMLBrowser::~mlRMMLBrowser");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

// реализация mlJSClass
//// JavaScript Variable Table
JSPropertySpec mlRMMLBrowser::_JSPropertySpec[] = {
	JSPROP_RO(documentType)
	JSPROP_RO(locationName)
	JSPROP_RO(locationURL)
	JSPROP_RO(busy)
	JSPROP_RO(readyState)
	JSPROP_RW(offline)
	JSPROP_RW(silent)
	JSPROP_RW(scrollTop)
	JSPROP_RW(scrollLeft)
	JSPROP_RW(bkgColor)
	JSPROP_RW(defaultCharset)
	JSPROP_RW(AXFrame)
	JSPROP_RW(value)
	{ 0, 0, 0, 0, 0 }
};

/// JavaScript Function Table
JSFunctionSpec mlRMMLBrowser::_JSFunctionSpec[] = {
	// JSFUNC(call,10)
	{ "call", mlRMMLActiveX::JSFUNC_call, 10, 0, 0},
	// JSFUNC(set,2)
	{ "set", mlRMMLActiveX::JSFUNC_set, 2, 0, 0},
	// JSFUNC(get,2)
	{ "get", mlRMMLActiveX::JSFUNC_get, 2, 0, 0},
	JSFUNC(navigate, 1) // Navigates to a URL or file
	JSFUNC(goBack, 0) // Navigates to the previous item in the history list
	JSFUNC(goForward, 0) // Navigates to the next item in the history list
	JSFUNC(goHome, 0) // Go home/start page (src)
	JSFUNC(refresh, 0) // Refresh the currently viewed page (NORMAL = 0, IFEXPIRED = 1, CONTINUE = 2, COMPLETELY = 3)
	JSFUNC(stop, 0) // Stops opening a file
	JSFUNC(scrollTo, 2) // Scrolls the window to the specified x- and y-offset
	JSFUNC(scrollBy, 2) // Causes the window to scroll relative to the current scrolled position by the specified x- and y-pixel offset
	// JSFUNC(scrollToPrevPage) // 
	// JSFUNC(scrollToNextPage) // 
	JSFUNC(scrollToAnchor, 1) // 
	JSFUNC(execScript, 1) // 
	JSFUNC(execCommand, 1) // 
	JSFUNC(queryCommandEnabled, 1) // 
	JSFUNC(fireMouseDown, 2) // эмулирует нажатие левой кнопки мыши на браузере (x,y - относительно левого верхнего угла браузера)
	JSFUNC(fireMouseMove, 2) // эмулирует перемещение мыши в браузере
	JSFUNC(fireMouseUp, 2) // эмулирует отпускание левой кнопки мыши на браузере
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLBrowser::_EventSpec[] = {
	MLEVENT(onScrolled)	// документ проскроллировали html-ку (колесом мыши, выделением...)
	MLEVENT(onBeforeNavigate)	// Fired when a new hyperlink is being navigated to
	MLEVENT(onNavigateComplete)	// Fired when the document being navigated to becomes visible and enters the navigation stack
	MLEVENT(onDocumentComplete)	// Fires when a document has been completely loaded and initialized
	MLEVENT(onProgressChange)	// Fired when download progress is updated
	MLEVENT(onDownloadBegin)	// Download of a page started
	MLEVENT(onDownloadComplete)	// Download of page complete
	MLEVENT(onCommandStateChange) // The enabled state of a command changed 
	MLEVENT(onNavigateError)	// Fires when an error occurs during navigation
	{0, 0, 0}
};

MLJSCLASS_IMPL_BEGIN(Browser, mlRMMLBrowser, 1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLBrowser)

  /// JavaScript Set Property Wrapper
JSBool mlRMMLBrowser::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SET_PROPS_BEGIN(mlRMMLBrowser);
	SET_PROTO_PROP(mlRMMLElement);
	SET_PROTO_PROP(mlRMMLVisible);
	SET_PROTO_PROP(mlRMMLPButton);
	default:
		switch(iID){
		case JSPROP_offline: // Controls if the frame is offline (read from cache)
			if(JSVAL_IS_BOOLEAN(*vp)){
				priv->SetOffline(JSVAL_TO_BOOLEAN(*vp));
			}else{
				JS_ReportError(cx, "offline-property is a boolean");
			}
			break;
		case JSPROP_silent: // Controls if any dialog boxes can be shown
			if(JSVAL_IS_BOOLEAN(*vp)){
				priv->SetSilent(JSVAL_TO_BOOLEAN(*vp));
			}else{
				JS_ReportError(cx, "silent-property is a boolean");
			}
			break;
		case JSPROP_scrollTop: // distance between the top of the object and the topmost portion of the content currently visible in the window
			if(JSVAL_IS_INT(*vp)){
				priv->SetScrollTop(JSVAL_TO_INT(*vp));
			}else{
				JS_ReportError(cx, "scrollTop-property is a integer");
			}
			break;
		case JSPROP_scrollLeft: // distance between the left edge of the object and the leftmost portion of the content currently visible in the window
			if(JSVAL_IS_INT(*vp)){
				priv->SetScrollLeft(JSVAL_TO_INT(*vp));
			}else{
				JS_ReportError(cx, "scrollLeft-property is a integer");
			}
			break;
			break;
		case JSPROP_bkgColor: // цвет фона ('#RGB' '#RRGGBB' '#AARRGGBB')
	//#pragma message("!!!!!!!!!! need to realize JSPROP_bkgColor setting !!!!!!!!!!!")
			// ??
			break;
		case JSPROP_defaultCharset: // кодировка по-умолчанию
	//#pragma message("!!!!!!!!!! need to realize JSPROP_defaultCharset setting !!!!!!!!!!!")
			// ??
			break;
		case JSPROP_AXFrame: // включение/выключение стандартной рамки (по умолчанию включена)
	//#pragma message("!!!!!!!!!! need to realize JSPROP_AXFrame setting !!!!!!!!!!!")
			// ??
			break;
		case JSPROP_value:{ // HTML-контент
			JSString* jssSP = JS_ValueToString(cx, *vp);
			jschar* jscSP = JS_GetStringChars(jssSP);
			priv->value = JS_ValueToString(cx, *vp);
			priv->ValueChanged();
			}break;
		}
	SET_PROPS_END;

	return JS_TRUE;
}

/*
virtual void SetScrollLeft(int aiValue);
virtual void BkgColorChanged();
*/

  /// JavaScript Get Property Wrapper
JSBool mlRMMLBrowser::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	GET_PROPS_BEGIN(mlRMMLBrowser);
	GET_PROTO_PROP(mlRMMLElement);
	GET_PROTO_PROP(mlRMMLVisible);
	GET_PROTO_PROP(mlRMMLPButton);
	default:
		switch(iID){
	case JSPROP_documentType:{ // (RO) Type of the contained document object
			const wchar_t* pwcValue = priv->GetLocationName();
			*vp = JS_GetEmptyStringValue(cx);
			if(pwcValue != NULL)
				*vp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcValue));
			}break;
		case JSPROP_locationName:{ // (RO) Short (UI-friendly) name of the URL/file currently viewed
			const wchar_t* pwcValue = priv->GetLocationName();
			*vp = JS_GetEmptyStringValue(cx);
			if(pwcValue != NULL)
				*vp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcValue));
			}break;
		case JSPROP_locationURL:{ // (RO) Full URL/path currently viewed
			const wchar_t* pwcValue = priv->GetLocationURL();
			*vp = JS_GetEmptyStringValue(cx);
			if(pwcValue != NULL)
				*vp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcValue));
			}break;
		case JSPROP_busy: // (RO) If something is still in progress
			*vp = BOOLEAN_TO_JSVAL(priv->GetBusy());
			break;
		case JSPROP_readyState: // (RO) Ready state of the object (UNINITIALIZED = 0, LOADING = 1, LOADED = 2, INTERACTIVE = 3, COMPLETE = 4)
			*vp = INT_TO_JSVAL(priv->GetReadyState());
			break;
		case JSPROP_offline: // Controls if the frame is offline (read from cache)
			*vp = BOOLEAN_TO_JSVAL(priv->GetOffline());
			break;
		case JSPROP_silent: // Controls if any dialog boxes can be shown
			*vp = BOOLEAN_TO_JSVAL(priv->GetSilent());
			break;
		case JSPROP_scrollTop: // distance between the top of the object and the topmost portion of the content currently visible in the window
			*vp = INT_TO_JSVAL(priv->GetScrollTop());
			break;
		case JSPROP_scrollLeft: // distance between the left edge of the object and the leftmost portion of the content currently visible in the window
			*vp = INT_TO_JSVAL(priv->GetScrollLeft());
			break;
		case JSPROP_bkgColor: // цвет фона ('#RGB' '#RRGGBB' '#AARRGGBB')
	//#pragma message("!!!!!!!!!! need to realize JSPROP_bkgColor getting !!!!!!!!!!!")
			// ??
			break;
		case JSPROP_defaultCharset: // кодировка по-умолчанию
	//#pragma message("!!!!!!!!!! need to realize JSPROP_defaultCharset getting !!!!!!!!!!!")
			// ??
			break;
		case JSPROP_AXFrame: // включение/выключение стандартной рамки (по умолчанию включена)
	//#pragma message("!!!!!!!!!! need to realize JSPROP_AXFrame getting !!!!!!!!!!!")
			// ??
			break;
		}
	GET_PROPS_END;

	return JS_TRUE;
}

// реализация mlRMMLElement
mlresult
mlRMMLBrowser::CreateMedia(omsContext* amcx)
{
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLImage::CreateMedia");

	amcx->mpRM->CreateMO(this);

	if (PMO_IS_NULL(mpMO))
		return ML_ERROR_NOT_INITIALIZED;

	mpMO->SetMLMedia(this);

	return ML_OK;
}

mlresult mlRMMLBrowser::Load()
{
	if (PMO_IS_NULL(mpMO))
		return ML_ERROR_NOT_INITIALIZED;

	int iOldType=mType;
	mType = MLMT_BROWSER;
	if (!(mpMO->GetILoadable()->SrcChanged()))
		mType = iOldType;

	return ML_OK;
}

mlRMMLElement* mlRMMLBrowser::Duplicate(mlRMMLElement* apNewParent)
{
	mlRMMLBrowser* pNewEL =
		(mlRMMLBrowser*)GET_RMMLEL(mcx, mlRMMLBrowser::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// Navigates to a URL or file
//  void navigate(
//	  string URL, 
//	  [optional] int flags, // 2 - NoHistory, 4 - NoHistory, 8 - NoWriteToCache
//	  [optional] string targetFrameName, 
//	  [optional] string postData, 
//	  [optional] string headers); 
JSBool mlRMMLBrowser::JSFUNC_navigate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 0){
		JS_ReportError(cx, "navigate-method must get one argument at least");
		return JS_TRUE;
	}
	mlString sURL;
	int iFlags = 0;
	mlString sPostData;
	mlString sHeaders;
	int iarg = 0;
	if(!JSVAL_IS_STRING(argv[iarg])){
		JS_ReportError(cx, "First argument (URL) of navigate-method must be a string");
		return JS_TRUE;
	}
	sURL = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[iarg]));
	do{
		if(++iarg == argc) break;
		if(!JSVAL_IS_INT(argv[iarg])){
			JS_ReportError(cx, "Second argument (flags) of navigate-method must be an integer");
			return JS_TRUE;
		}
		iFlags = JSVAL_TO_INT(argv[iarg]);
		//
		if(++iarg == argc) break;
		if(!JSVAL_IS_STRING(argv[iarg])){
			JS_ReportError(cx, "Third argument (postData) of navigate-method must be a string");
			return JS_TRUE;
		}
		sPostData = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[iarg]));
		//
		if(++iarg == argc) break;
		if(!JSVAL_IS_STRING(argv[iarg])){
			JS_ReportError(cx, "Fourth argument (headers) of navigate-method must be a string");
			return JS_TRUE;
		}
		sHeaders = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[iarg]));
	}while(true);
	omsresult result = pThis->Navigate(sURL.c_str(), iFlags, sPostData.c_str(), sHeaders.c_str());
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(result));
	return JS_TRUE;
}

// Navigates to the previous item in the history list
// void goBack(); 
JSBool mlRMMLBrowser::JSFUNC_goBack(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	pThis->GoBack();
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

//void goForward(); // Navigates to the next item in the history list
JSBool mlRMMLBrowser::JSFUNC_goForward(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	pThis->GoForward();
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// void goHome(); // Go home/start page (src)
JSBool mlRMMLBrowser::JSFUNC_goHome(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	pThis->GoHome();
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// Refresh the currently viewed page (NORMAL = 0, IFEXPIRED = 1, CONTINUE = 2, COMPLETELY = 3)
// void refresh([optional] int level); 
JSBool mlRMMLBrowser::JSFUNC_refresh(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	int iLevel = 0;
	if(argc > 0){
		if(!JSVAL_IS_INT(argv[0])){
			JS_ReportError(cx, "Argument of refresh-method must be a number (NORMAL = 0, IFEXPIRED = 1, CONTINUE = 2, COMPLETELY = 3)");
			return JS_TRUE;
		}
		iLevel = JSVAL_TO_INT(argv[0]);
	}
	pThis->Refresh(iLevel);
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// Stops opening a file
// void stop(); 
JSBool mlRMMLBrowser::JSFUNC_stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	pThis->Stop();
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// bool scrollTo(int x, int y); // абсолютный
JSBool mlRMMLBrowser::JSFUNC_scrollTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 1 || (!JSVAL_IS_INT(argv[0])) || (!JSVAL_IS_INT(argv[1]))){
		JS_ReportError(cx, "scrollTo-method must get two integer arguments");
		return JS_TRUE;
	}
	int x = JSVAL_TO_INT(argv[0]);
	int y = JSVAL_TO_INT(argv[1]);
	pThis->ScrollTo(x, y);
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// bool scrollBy(int x, int y); // относительный 
JSBool mlRMMLBrowser::JSFUNC_scrollBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 1 || (!JSVAL_IS_INT(argv[0])) || (!JSVAL_IS_INT(argv[1]))){
		JS_ReportError(cx, "scrollTo-method must get two integer arguments");
		return JS_TRUE;
	}
	int x = JSVAL_TO_INT(argv[0]);
	int y = JSVAL_TO_INT(argv[1]);
	pThis->ScrollBy(x, y);
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// void scrollToAnchor(string anchor);
JSBool mlRMMLBrowser::JSFUNC_scrollToAnchor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 0){
		JS_ReportError(cx, "scrollToAnchor-method must get one string argument");
		return JS_TRUE;
	}
	mlString sAnchor;
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "First argument (anchor) of scrollToAnchor-method must be a string");
		return JS_TRUE;
	}
	sAnchor = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	pThis->ScrollToAnchor(sAnchor.c_str());
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// VARIANT execScript(string code, [optional, defaultvalue("JScript")] string language);
JSBool mlRMMLBrowser::JSFUNC_execScript(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 0){
		JS_ReportError(cx, "execScript-method must get one argument at least");
		return JS_TRUE;
	}
	mlString sCode;
	mlString sLanguage = L"JScript";
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "First argument (URL) of execScript-method must be a string");
		return JS_TRUE;
	}
	sCode = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	if(argc == 2){
		if(!JSVAL_IS_STRING(argv[1])){
			JS_ReportError(cx, "Second argument (flags) of navigate-method must be a string");
			return JS_TRUE;
		}
		sLanguage = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[1]));
	}
	ML_VARIANT vRes; vRes.vt = MLVT_VOID;
	omsresult res = pThis->ExecScript(sCode.c_str(), sLanguage.c_str(), &vRes);
	if(vRes.vt == MLVT_BSTR){ // WebKit может выдавать в качестве результата только строку
		JSString* jssRes = wr_JS_NewUCStringCopyZ(cx, vRes.bstrVal);
		*rval = STRING_TO_JSVAL(jssRes);
	}else
		*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(res));
	return JS_TRUE;
}

// bool execCommand(string cmdID, [optional, defaultvalue(0)] VARIANT_BOOL showUI, [optional] VARIANT value);
JSBool mlRMMLBrowser::JSFUNC_execCommand(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 0){
		JS_ReportError(cx, "execCommand-method must get one argument at least");
		return JS_TRUE;
	}
	mlString sCmdID;
	bool bShowUI = false;
	ML_VARIANT vValue; vValue.vt = MLVT_EMPTY;
	int iarg = 0;
	if(!JSVAL_IS_STRING(argv[iarg])){
		JS_ReportError(cx, "First argument (CmdID) of execCommand-method must be a string");
		return JS_TRUE;
	}
	sCmdID = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[iarg]));
	do{
		if(++iarg == argc) break;
		if(!JSVAL_IS_BOOLEAN(argv[iarg])){
			JS_ReportError(cx, "Second argument (showUI) of execCommand-method must be a boolean");
			return JS_TRUE;
		}
		bShowUI = JSVAL_TO_BOOLEAN(argv[iarg]);
		//
		if(++iarg == argc) break;
		JSValToMLVariant(&vValue, argv[iarg]);
	}while(true);
	pThis->ExecCommand(sCmdID.c_str(), bShowUI, vValue);
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// bool queryCommandEnabled(string cmdID);
JSBool mlRMMLBrowser::JSFUNC_queryCommandEnabled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 0){
		JS_ReportError(cx, "queryCommandEnabled-method must get one string argument");
		return JS_TRUE;
	}
	mlString sCmdID;
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "First argument (cmdID) of queryCommandEnabled-method must be a string");
		return JS_TRUE;
	}
	sCmdID = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	pThis->QueryCommandEnabled(sCmdID.c_str());
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

bool GetXYFromArgs(JSContext *cx, uintN argc, jsval *argv, int &iX, int &iY, const char* pszMethodName){
	if(argc > 0){
		jsval vArg = argv[0];
		if(JSVAL_IS_INT(vArg)){
			iX = JSVAL_TO_INT(vArg);
			if(argc > 1){
				vArg = argv[1];
				if(JSVAL_IS_INT(vArg)){
					iY = JSVAL_TO_INT(vArg);
				}else{
					JS_ReportWarning(cx, "Invalid argument for Player.%s", pszMethodName);
				}
			}else{
				iX = -1;
				JS_ReportWarning(cx, "Invalid argument for Player.%s", pszMethodName);
			}
		}else{
			JS_ReportWarning(cx, "Invalid argument for Player.%s", pszMethodName);
		}
	}
	return true;
}

// bool fireMouseDown([x, y])
JSBool mlRMMLBrowser::JSFUNC_fireMouseDown(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;
	int iX = -1, iY = -1; // если не указаны, значит в том месте, где сейчас стоит мышка
	bool bRet = GetXYFromArgs(cx, argc, argv, iX, iY, "fireMouseDown");
	omsContext* pContext = GPSM(cx)->GetContext();
	if(!pThis->FireMouseDown(iX, iY)){
		return JS_TRUE;
	}
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// bool fireMouseMove([x, y])
JSBool mlRMMLBrowser::JSFUNC_fireMouseMove(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;
	int iX = -1, iY = -1; // если не указаны, значит в том месте, где сейчас стоит мышка
	bool bRet = GetXYFromArgs(cx, argc, argv, iX, iY, "fireMouseMove");
	omsContext* pContext = GPSM(cx)->GetContext();
	if(!pThis->FireMouseMove(iX, iY)){
		return JS_TRUE;
	}
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// bool fireMouseUp([x, y])
JSBool mlRMMLBrowser::JSFUNC_fireMouseUp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLBrowser* pThis = (mlRMMLBrowser*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;
	int iX = -1, iY = -1; // если не указаны, значит в том месте, где сейчас стоит мышка
	bool bRet = GetXYFromArgs(cx, argc, argv, iX, iY, "fireMouseUp");
	omsContext* pContext = GPSM(cx)->GetContext();
	if(!pThis->FireMouseUp(iX, iY)){
		return JS_TRUE;
	}
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// реализация mlIActiveX
omsresult mlRMMLBrowser::CallExternal(const wchar_t* apProcName, int aiArgC, 
										ML_VARIANT* pvarArgs, ML_VARIANT* varRes, bool abEvent){
	// вместо GPSM(mcx)->Updating() будем использовать IsInMyCall(), который 
	// возвращает true, если CallExternal был вызван в результате вызова метода браузера 
	// (т.е. предположительно в этом же потоке)
	if(!IsInMyCall()) varRes = NULL;
	return rmml::CallExternal(this, apProcName, aiArgC, pvarArgs, varRes, abEvent);
}

// реализация mlIBrowser

mlColor mlRMMLBrowser::GetBkgColor(){
	return bkgColor;
}
const wchar_t* mlRMMLBrowser::GetDefaultCharset(){
	return defaultCharset.c_str();
}
bool mlRMMLBrowser::GetAXFrame(){ return AXFrame; }

const wchar_t* mlRMMLBrowser::GetValue(){
	if(value==NULL) return L"";
	return (wchar_t*)JS_GetStringChars(value);
}

bool mlRMMLBrowser::SetEventData(char aidEvent, mlSynchData &Data){
	if(mlRMMLElement::SetEventData(aidEvent, Data))
		return true;
	switch(aidEvent){
	case EVID_onDocumentComplete:
	case EVID_onNavigateComplete:{
		wchar_t* pwcURL = NULL;
		Data >> pwcURL;
		if(pwcURL){
			jsval v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx, pwcURL));
			JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"URL",&v);
			JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"url",&v);
		}
		}break;
	case EVID_onBeforeNavigate:{
		wchar_t* pwcURL = NULL;
		Data >> pwcURL;
		if(pwcURL){
			jsval v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx, pwcURL));
			JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"URL",&v);
			JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"url",&v);
		}
		// ??
//		Data << flags;
//		Data << postData;
//		Data << headers;
//		Data << cancel;
		}break;
	}
	return true;
}

// документ проскроллировали html-ку (колесом мыши, выделением...)
void mlRMMLBrowser::onScrolled(){
	CallListeners(EVID_onScrolled);
}
// Fired when a new hyperlink is being navigated to.
void mlRMMLBrowser::onBeforeNavigate(const wchar_t* URL, int flags, const wchar_t* targetFrameName, const wchar_t* postData, const wchar_t* headers, bool &cancel){
	if(GPSM(mcx)->Updating()){
		//// то выполняем сразу же
		// помещаем в Event параметры
		JSString* jssURL = wr_JS_NewUCStringCopyZ(mcx, URL!=NULL?URL:L"");
		jsval v = STRING_TO_JSVAL(jssURL);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"URL",&v);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"url",&v);

		v = INT_TO_JSVAL(flags);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"flags",&v);

		JSString* jssTargetFrameName = wr_JS_NewUCStringCopyZ(mcx, targetFrameName!=NULL?targetFrameName:L"");
		v = STRING_TO_JSVAL(jssTargetFrameName);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"targetFrameName",&v);

		JSString* jssPostData = wr_JS_NewUCStringCopyZ(mcx, postData!=NULL?postData:L"");
		v = STRING_TO_JSVAL(jssPostData);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"postData",&v);

		JSString* jssHeaders = wr_JS_NewUCStringCopyZ(mcx, headers!=NULL?headers:L"");
		v = STRING_TO_JSVAL(jssHeaders);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"headers",&v);

		v = BOOLEAN_TO_JSVAL(cancel);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"cancel",&v);

		CallListeners(EVID_onBeforeNavigate);

		JS_GetProperty(mcx,GPSM(mcx)->GetEventGO(),"cancel",&v);
		if(JSVAL_IS_BOOLEAN(v)){
			cancel = JSVAL_TO_BOOLEAN(v);
		}
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << false;
		// сохраняем 
		Data << URL;
		Data << flags;
		if(postData == NULL) postData = L"";
		Data << postData;
		if(headers == NULL) headers = L"";
		Data << headers;
		Data << cancel;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onBeforeNavigate, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(mcx)->GetMode() == smmInitilization);
	}
}
// Fired when the document being navigated to becomes visible and enters the navigation stack
void mlRMMLBrowser::onNavigateComplete(const wchar_t* URL){
	if(GPSM(mcx)->Updating()){
		//// то выполняем сразу же
		// помещаем в Event параметры
		JSString* jssURL = wr_JS_NewUCStringCopyZ(mcx, URL!=NULL?URL:L"");
		jsval v = STRING_TO_JSVAL(jssURL);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"URL",&v);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"url",&v);

		CallListeners(EVID_onNavigateComplete);
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << false;
		// сохраняем check point
		Data << URL;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onNavigateComplete, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(mcx)->GetMode() == smmInitilization);
	}
}

void mlRMMLBrowser::onDocumentComplete(const wchar_t* URL){
	//if(GPSM(mcx)->Updating()){
	//	//// то выполняем сразу же
	//	// помещаем в Event параметры
	//	JSString* jssURL = JS_NewUCStringCopyZ(mcx, URL!=NULL?URL:L"");
	//	jsval v = STRING_TO_JSVAL(jssURL);
	//	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"URL",&v);
	//	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"url",&v);

	//	CallListeners(EVID_onDocumentComplete);
	//}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << false;
		// сохраняем check point
		Data << URL;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onDocumentComplete, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(mcx)->GetMode() == smmInitilization);
	//}
}

// Fired when download progress is updated
void mlRMMLBrowser::onProgressChange(int progress, int progressMax){
	// помещаем в Event параметры
	jsval v = INT_TO_JSVAL(progress);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"progress",&v);

	v = INT_TO_JSVAL(progressMax);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"progressMax",&v);

	CallListeners(EVID_onProgressChange);
}
// Download of a page started
void mlRMMLBrowser::onDownloadBegin(){
	CallListeners(EVID_onDownloadBegin);
}
// Download of page complete
void mlRMMLBrowser::onDownloadComplete(){
	CallListeners(EVID_onDownloadComplete);
}
// The enabled state of a command changed 
void mlRMMLBrowser::onCommandStateChange(int command, bool enable){
	// помещаем в Event параметры
	jsval v = INT_TO_JSVAL(command);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"command",&v);

	v = BOOLEAN_TO_JSVAL(enable);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"enable",&v);

	CallListeners(EVID_onCommandStateChange);
}
// Fires when an error occurs during navigation
void mlRMMLBrowser::onNavigateError(const wchar_t* URL, const wchar_t* frame, int statusCode, bool &cancel){
	// помещаем в Event параметры
	JSString* jssURL = wr_JS_NewUCStringCopyZ(mcx, URL!=NULL?URL:L"");
	jsval v = STRING_TO_JSVAL(jssURL);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"URL",&v);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"url",&v);

	JSString* jssFrame = wr_JS_NewUCStringCopyZ(mcx, frame!=NULL?frame:L"");
	v = STRING_TO_JSVAL(jssFrame);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"frame",&v);

	v = INT_TO_JSVAL(statusCode);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"statusCode",&v);

	v = BOOLEAN_TO_JSVAL(cancel);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"cancel",&v);

	CallListeners(EVID_onNavigateError);

	JS_GetProperty(mcx,GPSM(mcx)->GetEventGO(),"cancel",&v);
	if(JSVAL_IS_BOOLEAN(v)){
		cancel = JSVAL_TO_BOOLEAN(v);
	}
}

// сейчас вызывается, когда на текстуре-браузере нажали кнопку мыши
void mlRMMLBrowser::OnPress(mlPoint apntXY){
	FireMouseDown(apntXY.x, apntXY.y);
}

// реализация moIBrowser
const wchar_t* mlRMMLBrowser::GetDocumentType(){
	if(PMO_IS_NULL(mpMO)) return L"";
	return mpMO->GetIBrowser()->GetDocumentType();
}
const wchar_t* mlRMMLBrowser::GetLocationName(){
	if(PMO_IS_NULL(mpMO)) return L"";
	return mpMO->GetIBrowser()->GetLocationName();
}
const wchar_t* mlRMMLBrowser::GetLocationURL(){
	if(PMO_IS_NULL(mpMO)) return L"";
	return mpMO->GetIBrowser()->GetLocationURL();
}
bool mlRMMLBrowser::GetBusy(){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->GetBusy();
}
int mlRMMLBrowser::GetReadyState(){
	if(PMO_IS_NULL(mpMO)) return 0;
	return mpMO->GetIBrowser()->GetReadyState();
}
bool mlRMMLBrowser::GetOffline(){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->GetOffline();
}
void mlRMMLBrowser::SetOffline(bool abValue){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->GetOffline();
}
void mlRMMLBrowser::SetSilent(bool abValue){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->SetSilent(abValue);
}
void mlRMMLBrowser::SetScrollTop(int aiValue){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->SetScrollTop(aiValue);
}
void mlRMMLBrowser::SetScrollLeft(int aiValue){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->SetScrollLeft(aiValue);
}

bool mlRMMLBrowser::GetSilent(){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->GetSilent();
}
int mlRMMLBrowser::GetScrollTop(){
	if(PMO_IS_NULL(mpMO)) return 0;
	return mpMO->GetIBrowser()->GetScrollTop();
}
int mlRMMLBrowser::GetScrollLeft(){
	if(PMO_IS_NULL(mpMO)) return 0;
	return mpMO->GetIBrowser()->GetScrollLeft();
}
void mlRMMLBrowser::BkgColorChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIBrowser()->BkgColorChanged();
}

void mlRMMLBrowser::ValueChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIBrowser()->ValueChanged();
}

omsresult mlRMMLBrowser::Navigate(const wchar_t* URL, int flags, const wchar_t* postData, const wchar_t* headers){
	if(PMO_IS_NULL(mpMO)) return OMS_ERROR_NOT_INITIALIZED;
	if(URL == NULL) return OMS_ERROR_INVALID_ARG;
	mlString sURL = URL;
	// если начинается на http:, значит это путь к сайту в инете ()
	if(sURL.find(L"http:") == mlString::npos && sURL.find(L"https:") == mlString::npos)
		sURL = GPSM(mcx)->RefineResPathEl(this, URL);
	CInMyCall IMC(this);
	return mpMO->GetIBrowser()->Navigate(sURL.c_str(), flags, postData, headers);
}
void mlRMMLBrowser::GoBack(){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->GoBack();
}
void mlRMMLBrowser::GoForward(){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->GoForward();
}
void mlRMMLBrowser::GoHome(){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->GoHome();
}
void mlRMMLBrowser::Refresh(int level){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->Refresh(level);
}
void mlRMMLBrowser::Stop(){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->Stop();
}
bool mlRMMLBrowser::ScrollTo(int x, int y){
	if(PMO_IS_NULL(mpMO)) return false;
	CInMyCall IMC(this);
	return mpMO->GetIBrowser()->ScrollTo(x, y);
}
bool mlRMMLBrowser::ScrollBy(int x, int y){
	if(PMO_IS_NULL(mpMO)) return false;
	CInMyCall IMC(this);
	return mpMO->GetIBrowser()->ScrollBy(x, y);
}
void mlRMMLBrowser::ScrollToAnchor(const wchar_t* anchor){
	if(PMO_IS_NULL(mpMO)) return;
	CInMyCall IMC(this);
	mpMO->GetIBrowser()->ScrollToAnchor(anchor);
}
omsresult mlRMMLBrowser::ExecScript(const wchar_t* code, const wchar_t* language, ML_VARIANT* result){
	if(PMO_IS_NULL(mpMO)){ return OMS_ERROR_NOT_INITIALIZED; }
	CInMyCall IMC(this);
	return mpMO->GetIBrowser()->ExecScript(code, language, result);
}
bool mlRMMLBrowser::ExecCommand(const wchar_t* cmdID, bool showUI, ML_VARIANT value){
	if(PMO_IS_NULL(mpMO)) return false;
	CInMyCall IMC(this);
	return mpMO->GetIBrowser()->ExecCommand(cmdID, showUI, value);
}
bool mlRMMLBrowser::QueryCommandEnabled(const wchar_t* cmdID){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->QueryCommandEnabled(cmdID);
}

bool mlRMMLBrowser::FireMouseDown(int x, int y){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->FireMouseDown(x, y);
}

bool mlRMMLBrowser::FireMouseMove(int x, int y){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->FireMouseMove(x, y);
}

bool mlRMMLBrowser::FireMouseUp(int x, int y){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->FireMouseUp(x, y);
}

bool mlRMMLBrowser::FireMouseWheel(int x, int y, int distance){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->FireMouseWheel(x, y, distance);
}

bool mlRMMLBrowser::KeyDown(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->KeyDown(keyCode, keyStates, scanCode);
}

bool mlRMMLBrowser::KeyUp(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->KeyUp(keyCode, keyStates, scanCode);
}

bool mlRMMLBrowser::KeyPress(unsigned int charCode, unsigned long scanCode){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIBrowser()->KeyPress(charCode, scanCode);
}

//bool mlRMMLBrowser::SetFocus(){
//	if(PMO_IS_NULL(mpMO)) return false;
//	return mpMO->GetIBrowser()->SetFocus();
//}

//bool mlRMMLBrowser::KillFocus(){
//	if(PMO_IS_NULL(mpMO)) return false;
//	return mpMO->GetIBrowser()->KillFocus();
//}

}
