#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlSPParser.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLText::mlRMMLText(void)
{
	mType=MLMT_TEXT;
	mRMMLType=MLMT_TEXT;
	value=NULL;
	mbStylesRef=false;
	mjsoStyles=NULL;
	style=NULL;
	html=true;
	sliceTail = NULL;
	adjustHeight = false;
	textWidth = textHeight = 0;
	mbWidthIsSetInJS = false;
	mbHeightIsSetInJS = false;
	ML_INIT_COLOR(bkgColor);
	bkgColor.a = 0; // по умолчанию фон прозрачный
	textSize = pageSize = 0;
}

mlRMMLText::~mlRMMLText(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLText::~mlRMMLText");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLText::_JSPropertySpec[] = {
	JSPROP_RW(value)
	JSPROP_RW(style)
	JSPROP_RW(html)
	JSPROP_RW(scrollTop)
	JSPROP_RW(scrollLeft)
	JSPROP_RW(sliceTail)
	JSPROP_RO(textWidth)
	JSPROP_RO(textHeight)
	JSPROP_RW(adjustHeight)
	JSPROP_RW(bkgColor)
	JSPROP_RO(pageSize)
	JSPROP_RO(textSize)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLText::_JSFunctionSpec[] = {
	JSFUNC(scrollToLine, 1)
	JSFUNC(addText, 1)
	JSFUNC(getCursorPos, 1)
	JSFUNC(increaseCursor, 1)
	JSFUNC(decreaseCursor, 1)
	JSFUNC(getVisibleBounds, 0)
	JSFUNC(setCursorPos, 1)
	JSFUNC(toClipboard, 1)
	JSFUNC(fromClipboard, 1)
	JSFUNC(setSelection, 1)
	JSFUNC(isSliced, 1)	// был ли текст обрезан при форматировании, из-за того, что не поместился в отведенные под него размеры?
	JSFUNC(getSymbolIDByXY, 1)
	JSFUNC(areWordBreaksExists, 0)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Text,mlRMMLText,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLText)

JSBool mlRMMLText::JSFUNC_addText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLText* pThis = (mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 0){
		JS_ReportError(cx, "addText-method must get one argument!");
		return JS_TRUE;
	}
	mlString sText;
	int iFlags = 0;
	mlString sPostData;
	mlString sHeaders;
	int iarg = 0;
	if(!JSVAL_IS_STRING(argv[iarg])){
		JS_ReportError(cx, "First argument (text) of addText-method must be a string");
		return JS_TRUE;
	}
	sText = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(argv[iarg]));

	pThis->AddText((wchar_t *)sText.c_str());

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_areWordBreaksExists(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLText* pThis = (mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(pThis->AreWordBreaksExists());
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_setCursorPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = BOOLEAN_TO_JSVAL(false);
	if (argc < 1)
	{
		JS_ReportError(cx, "setCursorPos-method must get one argument!");
		return JS_TRUE;
	}

	mlRMMLText* pMLTxt=(mlRMMLText*)JS_GetPrivate(cx, obj);

	if(!JSVAL_IS_INT(argv[0])){		
		JS_ReportError(cx, "First argument (int) of setCursorPos-method must be a integer");
		return JS_TRUE;
	}
	
	unsigned int iPos = 0;
    ML_JSVAL_TO_INT(iPos, argv[0]);
	if (iPos < 0)
	{
		JS_ReportError(cx, "Argument of setCursorPos-method must be > 0");
		return JS_TRUE;
	}
	pMLTxt->SetCursorPos(iPos);
	
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}


JSBool mlRMMLText::JSFUNC_getSymbolIDByXY(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = INT_TO_JSVAL(-1);
	if (argc < 1)
	{
		JS_ReportError(cx, "getSymbolIDByXY-method must get two arguments!");
		return JS_TRUE;
	}

	mlRMMLText* pMLTxt=(mlRMMLText*)JS_GetPrivate(cx, obj);

	if(!JSVAL_IS_INT(argv[0])){
		JS_ReportError(cx, "First argument (int) of getSymbolIDByXY-method must be an integer");
		return JS_TRUE;
	}
	if(!JSVAL_IS_INT(argv[1]))
	{
		JS_ReportError(cx, "Second argument (int) of getSymbolIDByXY-method must be an integer");
		return JS_TRUE;
	}
	
	unsigned int iX = 0;
	unsigned int iY = 0;
    ML_JSVAL_TO_INT(iX, argv[0]);
	ML_JSVAL_TO_INT(iY, argv[1]);
	if ((iX < 0) || (iY < 0))
	{
		JS_ReportError(cx, "Arguments of getSymbolIDByXY-method must be > 0");
		return JS_TRUE;
	}
	int id = pMLTxt->GetSymbolIDByXY(iX, iY);
	
	*rval = INT_TO_JSVAL(id);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_setSelection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = BOOLEAN_TO_JSVAL(false);
	if (argc < 1)
	{
		JS_ReportError(cx, "setSelection-method must get two arguments!");
		return JS_TRUE;
	}

	mlRMMLText* pMLTxt=(mlRMMLText*)JS_GetPrivate(cx, obj);

	if(!JSVAL_IS_INT(argv[0])){
		JS_ReportError(cx, "First argument (int) of setSelection-method must be an integer");
		return JS_TRUE;
	}
	if(!JSVAL_IS_INT(argv[1]))
	{
		JS_ReportError(cx, "Second argument (int) of setSelection-method must be an integer");
		return JS_TRUE;
	}
	
	unsigned int iFrom = 0;
	unsigned int iTo = 0;
    ML_JSVAL_TO_INT(iFrom, argv[0]);
	ML_JSVAL_TO_INT(iTo, argv[1]);
	if ((iFrom < 0) || (iTo < 0))
	{
		JS_ReportError(cx, "Arguments of setSelection-method must be > 0");
		return JS_TRUE;
	}
	pMLTxt->SetSelection(iFrom, iTo);
	
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_getCursorPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLText* pThis = (mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	
	unsigned int x = (unsigned int)pThis->GetCursorX();
	unsigned int y = (unsigned int)pThis->GetCursorY();
	unsigned int pos = (unsigned int)pThis->GetCursorPos();
	unsigned int height = (unsigned int)pThis->GetCursorHeight();

	
	JSObject* jso = JS_NewObject(cx, NULL, NULL, NULL);
	SAVE_FROM_GC(cx, obj, jso);

	jsval vX, vY, vPos, vHeight;

	mlJS_NewDoubleValue(cx, x, &vX);
	JS_DefineProperty(cx, jso, "x", vX, 0, 0, 0);
	mlJS_NewDoubleValue(cx, y, &vY);
	JS_DefineProperty(cx, jso, "y", vY, 0, 0, 0);
	mlJS_NewDoubleValue(cx, pos, &vPos);
	JS_DefineProperty(cx, jso, "pos", vPos, 0, 0, 0);
	mlJS_NewDoubleValue(cx, height, &vHeight);
	JS_DefineProperty(cx, jso, "height", vHeight, 0, 0, 0);
	*rval = OBJECT_TO_JSVAL(jso);

	FREE_FOR_GC(cx, obj, jso);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_fromClipboard(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLText* pThis = (mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	
	std::wstring res = pThis->FromClipboard();
	
	JSObject* jso = JS_NewObject(cx, NULL, NULL, NULL);
	SAVE_FROM_GC(cx, obj, jso);
	JSString* jssValue=wr_JS_NewUCStringCopyZ(cx, (const wchar_t *) res.c_str());
	jsval vStr=STRING_TO_JSVAL(jssValue);
	JS_SetProperty(cx,jso,"value",&vStr);

	//buf = "";
	//fromBuf = L"";

	*rval = OBJECT_TO_JSVAL(jso);
	FREE_FOR_GC(cx, obj, jso);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_toClipboard(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLText* pThis = (mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 0){
		JS_ReportError(cx, "toClipboard-method must get one argument!");
		return JS_TRUE;
	}
	mlString sText;
	int iarg = 0;
	if(!JSVAL_IS_STRING(argv[iarg])){
		JS_ReportError(cx, "First argument (text) of toClipboard-method must be a string");
		return JS_TRUE;
	}
	sText = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[iarg]));

	pThis->ToClipboard((wchar_t *)sText.c_str());

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_increaseCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLText* pThis = (mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);

	unsigned int delta = 1;
	if (argc == 1)
	{
		if (JSVAL_IS_INT(argv[0]))
		{		
			ML_JSVAL_TO_INT(delta, argv[0]);
		}	
	}	

	pThis->IncreaseCursor(delta);

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_decreaseCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLText* pThis = (mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);

	unsigned int delta = 1;
	if (argc == 1)
	{
		if (JSVAL_IS_INT(argv[0]))
		{		
			ML_JSVAL_TO_INT(delta, argv[0]);
		}	
	}	

	pThis->DecreaseCursor(delta);

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_getVisibleBounds(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLText* pThis = (mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);

	int begin = 0;
	int end = 0;

	pThis->GetVisibleBounds(begin, end);

	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	jsval val;

	double _b = (double)begin;
	double _e = (double)end;

	mlJS_NewDoubleValue(cx, _b, &val); wr_JS_SetUCProperty(cx,jso,L"start",5,&val);
	mlJS_NewDoubleValue(cx, _e, &val); wr_JS_SetUCProperty(cx,jso,L"end",3,&val);

	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

///// JavaScript Set Property Wrapper
JSBool mlRMMLText::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLText);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
			case JSPROP_value:

//				if(JSVAL_IS_STRING(*vp)){
//					priv->value=JSVAL_TO_STRING(*vp);
//if(isEqual(priv->GetName(), L"Left"))
//int h=0;
				{
					JSString* jssSP = JS_ValueToString(cx, *vp);
					jschar* jscSP = JS_GetStringChars(jssSP);
					priv->value = JS_ValueToString(cx, *vp);
					priv->ValueChanged();
				}
				break;
			case JSPROP_style:
				if(JSVAL_IS_NULL(*vp)){
					priv->style=NULL;
					priv->mjsoStyles=NULL;
					priv->mbStylesRef=false;
				}else if(JSVAL_IS_OBJECT(*vp)){ 
					JSObject* jso=JSVAL_TO_OBJECT(*vp);
					if(mlRMMLStyles::IsInstance(cx, jso)){
						priv->mjsoStyles=jso;
						priv->mbStylesRef=true;
						priv->style=NULL;
						priv->StylesChanged();
					}else{
						JS_ReportError(cx, "Parameter is not a styles-object");
						return JS_FALSE;
					}
				}else if(JSVAL_IS_STRING(*vp)){ 
					JSString* jss=JSVAL_TO_STRING(*vp);
					if(IsElementRef(jss)){
						if(priv->GetMO()==NULL){ 
							// CreateMedia wasn't called
							priv->style=jss;
						}else{
							if(priv->mpParent==NULL){
								priv->style=jss;
							}else{
								wchar_t* jsc = wr_JS_GetStringChars(jss);
								mlString sTarget = priv->GetFullPath();
if (isEqual( jsc, L"stCaptionActive"))
	int ggg = 0;
								mlRMMLElement* pMLEl=priv->FindStyles(jss);
								if(pMLEl==NULL){
									JS_ReportError(cx, "Styles-object %s for %s not found ", cLPCSTR( jsc), cLPCSTR( sTarget.c_str()));
									return JS_FALSE;
								}
								if(pMLEl->mRMMLType!=MLMT_STYLES){
									JS_ReportError(cx, "Named object is not a styles");
									return JS_FALSE;
								} 
								priv->mjsoStyles = pMLEl->mjso;
								SAVE_FROM_GC(cx,priv->mjso,priv->mjsoStyles)
								priv->mbStylesRef=true;
								priv->style=NULL;
								priv->StylesChanged();
							}
						}
					}else{
						// create own styles-object
						// and init it with jss value
						wchar_t* jsc = wr_JS_GetStringChars(jss);
						if(jsc != NULL && *jsc != L'\0')
							priv->CreateOwnStylesObject(jsc);
						if(priv->mjsoStyles != NULL)
							*vp = OBJECT_TO_JSVAL(priv->mjsoStyles);
					}
				}
				break;
			case JSPROP_html:
				ML_JSVAL_TO_BOOL(priv->html,*vp);
				priv->HtmlChanged();
				break;
			case JSPROP_scrollTop:{
				jsdouble dVal = 0.0;
				if(JS_ValueToNumber(cx, *vp, &dVal))
					priv->SetScrollTop((unsigned int)dVal);
				}break;
			case JSPROP_scrollLeft:{
				jsdouble dVal = 0.0;
				if(JS_ValueToNumber(cx, *vp, &dVal))
					priv->SetScrollLeft((unsigned int)dVal);
				}break;
			case JSPROP_sliceTail:
				priv->sliceTail = JS_ValueToString(cx, *vp);
				break;
			case JSPROP_adjustHeight:
				ML_JSVAL_TO_BOOL(priv->adjustHeight,*vp);
				priv->AdjustChanged();
				break;
			case JSPROP_bkgColor:{
				if(JSVAL_IS_STRING(*vp)){
					mlColor color;
					const wchar_t* jscColor = wr_JS_GetStringChars(JSVAL_TO_STRING(*vp));
					if(ParseColor(cx, jscColor, color)){
						priv->bkgColor = color;
						priv->BkgColorChanged();
					}else
						return JS_TRUE;
				}else{
					JS_ReportError(cx, "bkgColor value must be a string");
				}
				}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLText::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLText);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
			case JSPROP_value:
				if(priv->value==NULL) *vp=JSVAL_NULL;
				else *vp=STRING_TO_JSVAL(priv->value);
				break;
			case JSPROP_style:
				if(priv->mjsoStyles==NULL){
					if(priv->style==NULL) *vp=JSVAL_NULL;
					else *vp=STRING_TO_JSVAL(priv->style);
				}else *vp=OBJECT_TO_JSVAL(priv->mjsoStyles);
				break;
			case JSPROP_scrollTop:
				*vp = INT_TO_JSVAL(priv->GetScrollTop());
				break;
			case JSPROP_scrollLeft:
				*vp = INT_TO_JSVAL(priv->GetScrollLeft());
				break;
			case JSPROP_sliceTail:
				if(priv->sliceTail==NULL) *vp=JSVAL_NULL;
				else *vp=STRING_TO_JSVAL(priv->sliceTail);
				break;
			case JSPROP_textWidth:
					*vp=INT_TO_JSVAL(priv->textWidth);
				break;
			case JSPROP_textHeight:
					*vp=INT_TO_JSVAL(priv->textHeight);
				break;
			case JSPROP_bkgColor:
				// ?? что устанавливали, то и вернет (по умолчанию - undefined наверное)
				break;

			case JSPROP_pageSize:
				*vp=INT_TO_JSVAL(priv->pageSize);
				break;

			case JSPROP_textSize:
				*vp=INT_TO_JSVAL(priv->textSize);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_scrollToLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if (argc < 1)
		return JS_FALSE;

	mlRMMLText* pMLTxt=(mlRMMLText*)JS_GetPrivate(cx, obj);

	unsigned int scrollLine = 0;
	ML_JSVAL_TO_INT(scrollLine, argv[0]);
	pMLTxt->ScrollToLine(scrollLine);
	return JS_TRUE;
}

JSBool mlRMMLText::JSFUNC_isSliced(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLText* pMLTxt=(mlRMMLText*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(pMLTxt->GetSliced());
	return JS_TRUE;
}

mlRMMLElement* mlRMMLText::FindStyles(JSString* ajssStylesName){
	mlRMMLElement* pMLEl = FindElemByName2(ajssStylesName);
	if(pMLEl == NULL) return NULL;
	if(!mlRMMLStyles::IsInstance(mcx, pMLEl->mjso)) return NULL;
	return pMLEl;
//	if(mpParent==NULL) return NULL;
//	// поищем у родительского объекта 
//	mlRMMLElement* pMLEl=mpParent->FindElemByName(ajssStylesName);
//	if(pMLEl!=NULL) return pMLEl;
//	// поищем у себя
//	pMLEl=FindElemByName(ajssStylesName);
//	if(pMLEl!=NULL) return pMLEl;
//	//
//	jschar* jsc = JS_GetStringChars(ajssStylesName);
//	int iLen = JS_GetStringLength(ajssStylesName);
//	const wchar_t* pwcDot = findDot(jsc);
//	if(pwcDot != NULL) return NULL;
//	mlRMMLElement* pParent=mpParent->mpParent;
//	while(pParent!=NULL){
//		pMLEl=pParent->GetChild(jsc,iLen);
//		if(pMLEl!=NULL) return pMLEl;
//		pParent=pParent->mpParent;
//	}
//	// поищем в классах
//	jsval vClss;
//	JS_GetProperty(mcx, JS_GetGlobalObject(mcx),GJSONM_CLASSES, &vClss);
//	if(!JSVAL_IS_OBJECT(vClss)) return NULL;
//	// получаем объект класса
//	jsval vc;
//	JS_GetUCProperty(mcx, JSVAL_TO_OBJECT(vClss),jsc,iLen,&vc);
//	if(!JSVAL_IS_OBJECT(vc)) return NULL;
//	JSObject* jso=JSVAL_TO_OBJECT(vc);
//	if(!mlRMMLStyles::IsInstance(jso)) return NULL;
//	pMLEl=GET_RMMLEL(mcx,jso);
//	return pMLEl;
}

// реализация mlRMMLElement
mlresult mlRMMLText::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, mpMO==NULL,"mlRMMLText::CreateMedia");
	// set styles
	if(style!=NULL && mjsoStyles==NULL){ 
		mpMO = PMOV_CREATED;
		jsval v=STRING_TO_JSVAL(style); 
		JS_SetProperty(mcx,mjso,"style",&v); 
//		FREE_STR_FOR_GC(mcx,mjso,style); 
		mpMO=NULL;
	}
	//
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLText::CreateMedia");
	BeforeTextReformatting();
	amcx->mpRM->CreateMO(this);
	AfterTextReformatting();
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLText::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	int iOldType=mType;	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. Restore
		mType=iOldType;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLText::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLText* pNewEL=(mlRMMLText*)GET_RMMLEL(mcx, mlRMMLText::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

mlresult mlRMMLText::SetValue(wchar_t* apwcValue){
/*	JSString* jssValue=JS_NewUCStringCopyZ(mcx,apwcValue);
	jsval v=STRING_TO_JSVAL(jssValue);
	JS_SetProperty(mcx,mjso,"value",&v);
*/
	return ML_OK;
}

mlString mlRMMLText::GetValue(int aiIndent){
	return GetValue();
}

void mlRMMLText::PropIsSet(mlRMMLElement* apThis, char idProp){
	mlRMMLElement::PropIsSet(apThis, idProp);
	if(idProp == mlRMMLVisible::JSPROP__width){
		mlRMMLText* pThis = (mlRMMLText*)apThis;
		pThis->mbWidthIsSetInJS = (pThis->mSize.width > 0);
		pThis->SizeChanged();
	}else if(idProp == mlRMMLVisible::JSPROP__height){
		mlRMMLText* pThis = (mlRMMLText*)apThis;
		pThis->mbHeightIsSetInJS = (pThis->mSize.height > 0);
		pThis->SizeChanged();
	}
}

// реализация mlIText
const wchar_t* mlRMMLText::GetValue(){
	if(value==NULL) return L"";
	return (wchar_t*)JS_GetStringChars(value);
}
/*mlMedia* mlRMMLText::GetParent(){
	return (mlMedia*)mlRMMLElement::GetParent();
}*/
mlStyle* mlRMMLText::GetStyle(const char* apszTag){
	if(mjsoStyles==NULL && style!=NULL){ 
		jsval v=STRING_TO_JSVAL(style); 
		JS_SetProperty(mcx,mjso,"style",&v); 
	}
	if(mjsoStyles==NULL) return NULL;
	mlRMMLStyles* pStyles=(mlRMMLStyles*)JS_GetPrivate(mcx,mjsoStyles);
	ML_ASSERTION(mcx, pStyles!=NULL,"mlRMMLText::GetStyle");
	return pStyles->GetStyle(apszTag);
}

bool mlRMMLText::IsAdjustHeight(){
	return adjustHeight;
}

void mlRMMLText::SetTextSize(unsigned int auWidth, unsigned int auHeight){
	if (textWidth == auWidth && textHeight == auHeight)
		return;

	textWidth  = auWidth;
	textHeight = auHeight;

	if (IsAdjustHeight()){
		// изменяем только высоту, а ширину не трогаем
		mlSize asz = {mSize.width, textHeight};
		SetSize(asz);
		ValueChanged();
	}
}

const mlColor mlRMMLText::GetBkgColor(){
	return bkgColor;
}

void mlRMMLText::SetScrollSizeInLines(unsigned int auPageSize, unsigned int auTextSize){
        pageSize = auPageSize;
        textSize = auTextSize;
}

void mlRMMLText::BeforeTextReformatting(){
	if(!mbWidthIsSetInJS){
		mSize.width = 0;
		mSize.height = 0;
	}else if(!mbHeightIsSetInJS){
		mSize.height = 0;
	}
}

void mlRMMLText::AfterTextReformatting(){
	mlSize szTextSize = GetVisible()->GetMediaSize();
	textWidth = szTextSize.width;
	textHeight = szTextSize.height;
	if(!mbWidthIsSetInJS){ // ширина не указана через JS
		mSize.width = textWidth;
		mSize.height = textHeight;
	}else if(!mbHeightIsSetInJS){ // ширина указана, а высота - нет
		mSize.height = textHeight;
	}
}

// реализация moIText
#define MOITEXTEVENT_IMPL(M) \
void mlRMMLText::M(){ \
	if(PMO_IS_NULL(mpMO)) return; \
	BeforeTextReformatting(); \
	mpMO->GetIText()->M(); \
	AfterTextReformatting(); \
}
//MOITEXTEVENT_IMPL(ValueChanged);
void mlRMMLText::ValueChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	if(!mbWidthIsSetInJS){
		mSize.width = 0;
		mSize.height = 0;
	}else if(!mbHeightIsSetInJS){
		mSize.height = 0;
	}
	mpMO->GetIText()->ValueChanged();
	mlSize szTextSize = GetVisible()->GetMediaSize();
	textWidth = szTextSize.width;
	textHeight = szTextSize.height;
	if(!mbWidthIsSetInJS){ // ширина не указана через JS
		mSize.width = textWidth;
		mSize.height = textHeight;
	}else if(!mbHeightIsSetInJS){ // ширина указана, а высота - нет
		mSize.height = textHeight;
	}
}

mlSize mlRMMLText::GetFormatSize()
{
	mlSize szTextSize;
	szTextSize.width = mSize.width;
	szTextSize.height = mSize.height;
	if(!mbWidthIsSetInJS){ // ширина не указана через JS
		szTextSize.width = 0;
		szTextSize.height = 0;
	}else if(!mbHeightIsSetInJS){ // ширина указана, а высота - нет
		szTextSize.height = 0;
	}
	return szTextSize;
}

MOITEXTEVENT_IMPL(StylesChanged);
MOITEXTEVENT_IMPL(HtmlChanged);
MOITEXTEVENT_IMPL(AdjustChanged);
MOITEXTEVENT_IMPL(BkgColorChanged);

unsigned int mlRMMLText::GetScrollTop(){
	if(PMO_IS_NULL(mpMO)) return 0;
	return mpMO->GetIText()->GetScrollTop();
}

void mlRMMLText::SetScrollTop(unsigned int auValue){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->SetScrollTop(auValue);
}

unsigned int mlRMMLText::GetScrollLeft(){
	if(PMO_IS_NULL(mpMO)) return 0;
	return mpMO->GetIText()->GetScrollLeft();
}

void mlRMMLText::SetScrollLeft(unsigned int auValue){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->SetScrollLeft(auValue);
}

void mlRMMLText::ScrollToLine(unsigned int auLine){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->ScrollToLine(auLine);
}

void mlRMMLText::AddText(wchar_t* str){
	mlString tmp = wr_JS_GetStringChars(value);
	tmp += str;
	value = wr_JS_NewUCStringCopyZ(mcx, tmp.c_str());
	jsval v;
	JS_GetProperty(mcx,mjso,"value",&v);
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->AddText(str);
}

bool mlRMMLText::GetTextFormattingInfo(mlSynchData &aData){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIText()->GetTextFormattingInfo(aData);
}

bool mlRMMLText::AreWordBreaksExists()
{
	if (PMO_IS_NULL(mpMO)) 
	{
		return false;
	}

	return mpMO->GetIText()->AreWordBreaksExists();
}

void mlRMMLText::CreateOwnStylesObject(const wchar_t* apwcStyle){
	JSObject* jsoNewStyles = mlRMMLStyles::newJSObject(mcx);
	mlRMMLStyles* pNewStyles = (mlRMMLStyles*)JS_GetPrivate(mcx, jsoNewStyles);
	if(OMS_SUCCEEDED(pNewStyles->SetValue((wchar_t*)apwcStyle))){
		jsval v = OBJECT_TO_JSVAL(jsoNewStyles);
		wr_JS_SetUCProperty(mcx, mjso, L"style", -1, &v);
	}
//	// если была ссылка на другой объект стилей
//	if(mbStylesRef){
//		mbStylesRef = false;
//	}
}

void mlRMMLText::IncreaseCursor(unsigned int delta)
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->IncreaseCursor(delta);
}

void mlRMMLText::SetSelection(int from, int to)
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->SetSelection(from, to);
}

int mlRMMLText::GetSymbolIDByXY(int x, int y)
{
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIText()->GetSymbolIDByXY(x, y);
}

void mlRMMLText::DecreaseCursor(unsigned int delta)
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->DecreaseCursor(delta);
}

void mlRMMLText::GetVisibleBounds(int& _begin, int& _end)
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->GetVisibleBounds(_begin, _end);
}

int mlRMMLText::GetCursorX()
{
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIText()->GetCursorX();
}

int mlRMMLText::GetCursorY()
{
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIText()->GetCursorY();
}

int mlRMMLText::GetCursorPos()
{
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIText()->GetCursorPos();
}

int mlRMMLText::GetCursorHeight()
{
	if(PMO_IS_NULL(mpMO)) return -1;
	return mpMO->GetIText()->GetCursorHeight();
}

void mlRMMLText::SetCursorPos(int pos)
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->SetCursorPos(pos);
}

void mlRMMLText::ToClipboard(wchar_t* str)
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIText()->ToClipboard(str);
}

std::wstring mlRMMLText::FromClipboard()
{
	if(PMO_IS_NULL(mpMO)) return L"";
	return mpMO->GetIText()->FromClipboard();
}

bool mlRMMLText::GetSliced(){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIText()->GetSliced();
}

}
