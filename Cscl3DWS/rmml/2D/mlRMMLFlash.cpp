// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: mlRMMLFlash.cpp,v $
// $Revision: 1.5 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2009/10/30 07:49:04 $
//              $Author: asmo1 $ 
//
//////////////////////////////////////////////////////////////////////
#include "mlRMML.h"
#include "config/oms_config.h"
#include "2D/mlRMMLFlash.h"
#include "config/prolog.h"

namespace rmml
{
  mlRMMLFlash::mlRMMLFlash()
  {
    mType     = MLMT_FLASH;
    mRMMLType = MLMT_FLASH;
  }

  mlRMMLFlash::~mlRMMLFlash()
  {
    omsContext* pContext = GPSM(mcx)->GetContext();
    ML_ASSERTION(mcx, pContext->mpRM!=NULL, "mlRMMLFlash::~mlRMMLFlash");
	if(!PMO_IS_NULL(mpMO))
	    pContext->mpRM->DestroyMO(this);
  }

// реализация mlJSClass
//// JavaScript Variable Table
JSPropertySpec mlRMMLFlash::_JSPropertySpec[] = {
	JSPROP_RW(scaleMode)
	JSPROP_RW(wMode)
	{ 0, 0, 0, 0, 0 }
};

  /// JavaScript Function Table
JSFunctionSpec mlRMMLFlash::_JSFunctionSpec[] = {
	JSFUNC(setVariable,  2)
	JSFUNC(getVariable,  1)
	JSFUNC(callFunction, 1)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLFlash::_EventSpec[] = {
	MLEVENT(onFSCommand) // 
	{0, 0, 0}
};

MLJSCLASS_IMPL_BEGIN(Flash, mlRMMLFlash, 1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLContinuous)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLFlash)

__forceinline int ParseScaleMode(JSString* ajssScaleMode){
	mlString sScaleMode = wr_JS_GetStringChars(ajssScaleMode);
	wchar_t* pwc = (wchar_t*)sScaleMode.data();
	for(; *pwc; pwc++) *pwc = tolower(*pwc);
	if(isEqual(sScaleMode.c_str(), L"showall")) return 0;
	if(isEqual(sScaleMode.c_str(), L"noborder")) return 1;
	if(isEqual(sScaleMode.c_str(), L"exactfit")) return 2;
	if(isEqual(sScaleMode.c_str(), L"noscale")) return 3;
	return -1;
}

__forceinline int ParseWMode(JSString* ajssScaleMode)
{
	mlString sScaleMode = wr_JS_GetStringChars(ajssScaleMode);
	wchar_t* pwc = (wchar_t*)sScaleMode.data();
	for(; *pwc; pwc++) *pwc = tolower(*pwc);
	if(isEqual(sScaleMode.c_str(), L"window")) return 0;
	if(isEqual(sScaleMode.c_str(), L"opaque")) return 1;
	if(isEqual(sScaleMode.c_str(), L"transparent")) return 2;
	return -1;
}

  /// JavaScript Set Property Wrapper
  JSBool
  mlRMMLFlash::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
  {
    SET_PROPS_BEGIN(mlRMMLFlash);
    SET_PROTO_PROP(mlRMMLElement);
    SET_PROTO_PROP(mlRMMLVisible);
    SET_PROTO_PROP(mlRMMLContinuous);
    default:
		switch(iID){
		case JSPROP_scaleMode:{
			if(JSVAL_IS_INT(*vp)){
				int iScaleMode = JSVAL_TO_INT(*vp);
				if(iScaleMode < -1 || iScaleMode > 3){
					JS_ReportWarning(cx, "scaleMode value must be in range from 0 to 3.");
					break;
				}
				priv->setScaleMode(iScaleMode);
			}else if(JSVAL_IS_STRING(*vp)){
				int iScaleMode = ParseScaleMode(JSVAL_TO_STRING(*vp));
				if(iScaleMode < 0){
					JS_ReportWarning(cx, "Illegal scaleMode string value.");
					break;
				}
				priv->setScaleMode(iScaleMode);
			}
			} break;
		case JSPROP_wMode:{
			if(JSVAL_IS_INT(*vp)){
				int iWMode = JSVAL_TO_INT(*vp);
				if(iWMode < -1 || iWMode > 2){
					JS_ReportWarning(cx, "wMode value must be in range from 0 to 2.");
					break;
				}
				priv->setWMode(iWMode);
			}else if(JSVAL_IS_STRING(*vp)){
				int iWMode = ParseWMode(JSVAL_TO_STRING(*vp));
				if(iWMode < 0){
					JS_ReportWarning(cx, "Illegal wMode string value.");
					break;
				}
				priv->setWMode(iWMode);
			}
			} break;
	}
    SET_PROPS_END;

    return JS_TRUE;
  }

  /// JavaScript Get Property Wrapper
  JSBool
  mlRMMLFlash::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
  {
    GET_PROPS_BEGIN(mlRMMLFlash);
    GET_PROTO_PROP(mlRMMLElement);
    GET_PROTO_PROP(mlRMMLVisible);
    GET_PROTO_PROP(mlRMMLContinuous);
    default:;
    GET_PROPS_END;

    return JS_TRUE;
  }

// реализация mlRMMLElement
  mlresult
  mlRMMLFlash::CreateMedia(omsContext* amcx)
  {
    ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLFlash::CreateMedia");

    amcx->mpRM->CreateMO(this);

    if (PMO_IS_NULL(mpMO))
      return ML_ERROR_NOT_INITIALIZED;

    mpMO->SetMLMedia(this);

    return ML_OK;
  }

  mlresult
  mlRMMLFlash::Load()
  {
    if (PMO_IS_NULL(mpMO))
      return ML_ERROR_NOT_INITIALIZED;

    int iOldType=mType;
    mType = MLMT_FLASH;
    if (!(mpMO->GetILoadable()->SrcChanged()))
      mType = iOldType;

    return ML_OK;
  }

  mlRMMLElement*
  mlRMMLFlash::Duplicate(mlRMMLElement* apNewParent)
  {
    mlRMMLFlash* pNewEL =
      (mlRMMLFlash*)GET_RMMLEL(mcx, mlRMMLImage::newJSObject(mcx));
    pNewEL->SetParent(apNewParent);
    pNewEL->GetPropsAndChildrenCopyFrom(this);

    return pNewEL;
  }

  // реализация mlIFlash
void mlRMMLFlash::onFSCommand(const wchar_t *command, const wchar_t *args){
	// ??
}

int mlRMMLFlash::GetWModeOnCreate(){
	// ??
	return 0;
}

JSBool mlRMMLFlash::JSFUNC_setVariable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc != 2)
		return JS_FALSE;

	mlRMMLFlash* pMLFlash = reinterpret_cast<mlRMMLFlash*>(JS_GetPrivate(cx, obj));
	if (!pMLFlash)
		return JS_FALSE;
	moIFlash *flash_ptr = pMLFlash->getFlash();
	if (!flash_ptr)
		return JS_FALSE;

	JSString *var_str   = JS_ValueToString(cx, argv[0]);
	wchar_t   *var = wr_JS_GetStringChars(var_str);

	JSString *val_str   = JS_ValueToString(cx, argv[1]);
	wchar_t   *val = wr_JS_GetStringChars(val_str);

	pMLFlash->setVariable(var, val);

	return JS_TRUE;
}

JSBool
mlRMMLFlash::JSFUNC_getVariable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc != 1)
		return JS_FALSE;

	mlRMMLFlash* pMLFlash = reinterpret_cast<mlRMMLFlash*>(JS_GetPrivate(cx, obj));
	if (!pMLFlash)
		return JS_FALSE;
	moIFlash *flash_ptr = pMLFlash->getFlash();
	if (!flash_ptr)
		return JS_FALSE;

	JSString *str   = JS_ValueToString(cx, argv[0]);
	wchar_t   *var = wr_JS_GetStringChars(str);


	mlOutString sRetVal;
	long ret_size = flash_ptr->getVariable(var, sRetVal);

	*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sRetVal.mstr.c_str()));

	return JS_TRUE;
}

// <invoke name="getFromDelphi"><arguments><string>Hi, Flash</string></arguments></invoke>
// <invoke name="\" returntype="\"><arguments><string>" + piece + "</string></arguments></invoke>
// <invoke name=\"callMeFromCSharp\" returntype=\"xml\"><arguments><bool>false</bool></arguments></invoke>
// <invoke name="Handshake" returntype="xml"><arguments><string>hello world</string></arguments></invoke>
// <invoke name="myfunc" returntype="xml"><arguments><string>433333</string></arguments></invoke>
// returntype="javascript"

JSBool mlRMMLFlash::JSFUNC_callFunction(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc != 1)
		return JS_FALSE;

	mlRMMLFlash* pMLFlash = reinterpret_cast<mlRMMLFlash*>(JS_GetPrivate(cx, obj));
	if (!pMLFlash)
		return JS_FALSE;
	moIFlash *flash_ptr = pMLFlash->getFlash();
	if (!flash_ptr)
		return JS_FALSE;

	JSString *str   = JS_ValueToString(cx, argv[0]);
	wchar_t   *request = wr_JS_GetStringChars(str);

	mlOutString sRetVal;
	long ret_size = flash_ptr->callFunction(request, sRetVal);

	*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sRetVal.mstr.c_str()));

	return JS_TRUE;
}

  // реализация moIFlash
void mlRMMLFlash::setVariable(const wchar_t *var, const wchar_t *value){
	if(PMO_IS_NULL(mpMO)) return;
	return mpMO->GetIFlash()->setVariable(var, value);
}

int mlRMMLFlash::getScaleMode(){
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIFlash()->getScaleMode();
}

void mlRMMLFlash::setScaleMode(int aiScaleMode){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIFlash()->setScaleMode(aiScaleMode);
}

int mlRMMLFlash::getWMode(){
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIFlash()->getWMode();
}

void mlRMMLFlash::setWMode(int aiWMode){
	if(PMO_IS_NULL(mpMO)) return;
	return mpMO->GetIFlash()->setWMode(aiWMode);
}

  // возвращает длину данных
long mlRMMLFlash::getVariable(const wchar_t *var, mlOutString& asRetVal){
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIFlash()->getVariable(var, asRetVal);
}

  // возвращает длину результата
long mlRMMLFlash::callFunction(const wchar_t *request, mlOutString& asRetVal){
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIFlash()->callFunction(request, asRetVal);
}

}