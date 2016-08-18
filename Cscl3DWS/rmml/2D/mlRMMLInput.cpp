#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLInput::mlRMMLInput(void)
{
	mType=MLMT_INPUT;
	mRMMLType=MLMT_INPUT;
	value=NULL;
	mbStylesRef=false;
	mjsoStyles=NULL;
	style=NULL;
	html=true;
	sliceTail = NULL;
	adjustHeight = false;
	InputWidth = InputHeight = 0;
	mbInGetValue = false;
	password = false;
	multiline = false;
	tabGroup = -1;
	tabIndex = -1;
}

mlRMMLInput::~mlRMMLInput(void){
	omsContext* pContext=GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLInput::~mlRMMLInput");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLInput::_JSPropertySpec[] = {
	JSPROP_RW(value)
	JSPROP_RW(style)
	JSPROP_RW(adjustHeight)
	JSPROP_RW(password)
	JSPROP_RW(multiline)
	JSPROP_RW(tabGroup)
	JSPROP_RW(tabIndex)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLInput::_JSFunctionSpec[] = {
	JSFUNC(insertText, 1)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Input,mlRMMLInput,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLInput)

///// JavaScript Set Property Wrapper
JSBool mlRMMLInput::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLInput);
		SET_PROTO_PROP(mlRMMLElement);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
			case JSPROP_value:
				priv->value=JS_ValueToString(cx,*vp);
				if( !priv->mbInGetValue)
					priv->ValueChanged();
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
								mlRMMLElement* pMLEl=priv->FindStyles(jss);
								if(pMLEl==NULL){
									JS_ReportError(cx, "Styles-object not found");
									return JS_FALSE;
								}
								if(pMLEl->mRMMLType!=MLMT_STYLES){
									JS_ReportError(cx, "Named object is not a styles");
									return JS_FALSE;
								} 
								priv->mjsoStyles=pMLEl->mjso;
								SAVE_FROM_GC(cx,priv->mjso,priv->mjsoStyles)
								priv->mbStylesRef=true;
								priv->style=NULL;
								priv->StylesChanged();
							}
						}
					}else{
						// create own styles-object
						// ??
						// and init it with jss value
						// ??
					}
				}
				break;
			case JSPROP_password:
				ML_JSVAL_TO_BOOL(priv->password,*vp);
				priv->PasswordChanged();
				break;
			case JSPROP_multiline:
				ML_JSVAL_TO_BOOL(priv->multiline,*vp);
				priv->MultilineChanged();
				break;
			case JSPROP_tabGroup:
				ML_JSVAL_TO_INT(priv->tabGroup,*vp);
				priv->TabGroupChanged();
				break;
			case JSPROP_tabIndex:
				ML_JSVAL_TO_INT(priv->tabIndex,*vp);
				priv->TabIndexChanged();
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLInput::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLInput);
		GET_PROTO_PROP(mlRMMLElement);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
			case JSPROP_value:{
				priv->mbInGetValue = true;
				priv->GetValue();
				if(priv->value == NULL) *vp = JSVAL_NULL;
				else *vp=STRING_TO_JSVAL(priv->value);
				priv->mbInGetValue = false;
				}break;
			case JSPROP_style:
				if(priv->mjsoStyles==NULL){
					if(priv->style==NULL) *vp=JSVAL_NULL;
					else *vp=STRING_TO_JSVAL(priv->style);
				}else *vp=OBJECT_TO_JSVAL(priv->mjsoStyles);
				break;
			case JSPROP_password:
				*vp = BOOLEAN_TO_JSVAL(priv->password);
				break;
			case JSPROP_tabGroup:
				*vp = INT_TO_JSVAL(priv->tabGroup);
				break;
			case JSPROP_tabIndex:
				*vp = INT_TO_JSVAL(priv->tabIndex);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

mlRMMLElement* mlRMMLInput::FindStyles(JSString* ajssStylesName){
	mlRMMLElement* pMLEl = FindElemByName2(ajssStylesName);
	if(pMLEl == NULL) return NULL;
	if(!mlRMMLStyles::IsInstance(mcx, pMLEl->mjso)) return NULL;
	return pMLEl;
}

// реализация mlMedia
void* mlRMMLInput::GetInterface(long iid)
{
	if (iid == 1)
		return static_cast<moIInput*>(this);

	return 0;
}

// реализация mlRMMLElement
mlresult mlRMMLInput::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, mpMO==NULL,"mlRMMLInput::CreateMedia");
	// set styles
	if(style!=NULL && mjsoStyles==NULL){
		mpMO = PMOV_CREATED;
		jsval v=STRING_TO_JSVAL(style);
		JS_SetProperty(mcx,mjso,"style",&v);
		mpMO=NULL;
	}
	//
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLInput::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLInput::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	int iOldType=mType;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. Restore
		mType=iOldType;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLInput::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLInput* pNewEL=(mlRMMLInput*)GET_RMMLEL(mcx, mlRMMLInput::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

mlresult mlRMMLInput::SetValue(const wchar_t* apwcValue){
	JSString* jssValue=wr_JS_NewUCStringCopyZ(mcx,apwcValue);
	jsval v=STRING_TO_JSVAL(jssValue);
	JS_SetProperty(mcx,mjso,"value",&v);
	return ML_OK;
}

// реализация mlIInput
const wchar_t* mlRMMLInput::GetValue(){	
	moIInput *pIInput = (!PMO_IS_NULL(mpMO)) ? mpMO->GetIInput() : 0;
	if (pIInput)
		SetValue(pIInput->GetValue());
	if(value==NULL) return NULL;
	return (wchar_t*)JS_GetStringChars(value);
}
mlMedia* mlRMMLInput::GetParent(){
	return (mlMedia*)mlRMMLElement::GetParent();
}
mlStyle* mlRMMLInput::GetStyle(const char* apszTag){
	if(mjsoStyles==NULL && style!=NULL){ 
		jsval v=STRING_TO_JSVAL(style); 
		JS_SetProperty(mcx,mjso,"style",&v); 
	}
	if(mjsoStyles==NULL) return NULL;
	mlRMMLStyles* pStyles=(mlRMMLStyles*)JS_GetPrivate(mcx,mjsoStyles);
	ML_ASSERTION(mcx, pStyles!=NULL,"mlRMMLInput::GetStyle");
	return pStyles->GetStyle(apszTag);
}

bool mlRMMLInput::IsAdjustHeight(){
	return adjustHeight;
}

void mlRMMLInput::SetInputSize(unsigned int auWidth, unsigned int auHeight){
}

void mlRMMLInput::OnKeyDown(int aiCode){
	oms::omsUserEvent evKeyDown;
	evKeyDown.ev = oms::omsUserEvent::KEYDOWN;
	evKeyDown.key_code = aiCode;
	GPSM(mcx)->GetContext()->mpInput->AddEvent(evKeyDown);
}

void mlRMMLInput::OnSetFocus(){
	GPSM(mcx)->SetFocus(this);
}

void mlRMMLInput::OnKillFocus(){
	GPSM(mcx)->SetFocus(NULL);
}

// реализация moIInput
#define MOIINPUTEVENT_IMPL(M) \
void mlRMMLInput::M(){ \
	if(PMO_IS_NULL(mpMO)) return; \
	mpMO->GetIInput()->M(); \
}
MOIINPUTEVENT_IMPL(ValueChanged);
MOIINPUTEVENT_IMPL(StylesChanged);
MOIINPUTEVENT_IMPL(PasswordChanged);
MOIINPUTEVENT_IMPL(MultilineChanged);
MOIINPUTEVENT_IMPL(TabGroupChanged);
MOIINPUTEVENT_IMPL(TabIndexChanged);

const wchar_t* mlRMMLInput::GetValue() const{
	if(value==NULL) return NULL;
	return (wchar_t*)JS_GetStringChars(value);
//	return const_cast<mlRMMLInput *>(this)->GetValue();
}
}

bool mlRMMLInput::InsertText(const wchar_t* apwcText, int aiPos){
	if(PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIInput()->InsertText(apwcText, aiPos);
}

JSBool mlRMMLInput::JSFUNC_insertText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if (argc < 1){
		JS_ReportWarning(cx, "insertText must get one string argument at least");
		return JS_TRUE;
	}

	mlRMMLInput* pMLInput = (mlRMMLInput*)JS_GetPrivate(cx, obj);

	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportWarning(cx, "First argument of insertText must be a string");
		return JS_TRUE;
	}
	JSString* jssText = JSVAL_TO_STRING(argv[0]);
	wchar_t* pwcText = wr_JS_GetStringChars(jssText);

	pMLInput->InsertText(pwcText, -1);
	return JS_TRUE;
}