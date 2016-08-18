
#include "mlRMML.h"
#include "jsprf.h"
#include "mlSPParser.h"

namespace rmml {

mlRMMLShadow3D::mlRMMLShadow3D(void){
	mType=mRMMLType=MLMT_SHADOW3D;
	mbtPropSet=0;
}

mlRMMLShadow3D::~mlRMMLShadow3D(void){
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLShadow3D::_JSPropertySpec[] = {
    JSPROP_RW(type)
    JSPROP_RW(plane)
    JSPROP_RW(color)
    JSPROP_RW(light)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLShadow3D::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Shadow3D,mlRMMLShadow3D,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLShadow3D)

void ml3DShadow::SetJSProperty(JSContext *cx, int aiID, jsval *vp){
	switch(aiID){
	case JSPROP_type:
		if(JSVAL_IS_STRING(*vp)){
			JSString* jssType=JSVAL_TO_STRING(*vp);
			if(isEqual(jssType,MLSHTN_NONE)){
				if(type!=MLSHT_NONE){
					type=MLSHT_NONE;
					NotifyChangeListeners();
					mbtPropSet|=SHPS_type;
				}
			}else if(isEqual(jssType,MLSHTN_PROJECTIVE)){
				if(type!=MLSHT_PROJECTIVE){
					type=MLSHT_PROJECTIVE;
					NotifyChangeListeners();
					mbtPropSet|=SHPS_type;
				}
			}else if(isEqual(jssType,MLSHTN_STENCIL)){
				if(type!=MLSHT_STENCIL){
					type=MLSHT_STENCIL;
					NotifyChangeListeners();
					mbtPropSet|=SHPS_type;
				}
			}else{
				JS_ReportError(cx,"Invalid shadow type");
			}
		}else{
			JS_ReportError(cx,"Shadow type is string property");
		}
		break;
	case JSPROP_plane:
		if(JSVAL_IS_STRING(*vp)){
			JSString* jssPlane=JSVAL_TO_STRING(*vp);
			wchar_t* pwc=wr_JS_GetStringChars(jssPlane);
			ParsePlane(cx,pwc,plane);
			NotifyChangeListeners();
			mbtPropSet|=SHPS_plane;
		}else{
			JS_ReportError(cx,"Error while setting shadow plane property");
		}
		break;
	case JSPROP_color:
		if(JSVAL_IS_STRING(*vp)){
			JSString* jssColor=JSVAL_TO_STRING(*vp);
			wchar_t* pwc=wr_JS_GetStringChars(jssColor);
			ParseColor(cx,(const wchar_t *&) pwc,color);
			NotifyChangeListeners();
			mbtPropSet|=SHPS_color;
		}else{
			JS_ReportError(cx,"Error while setting shadow color property");
		}
		break;
	case JSPROP_light:
		if(SetLightRefJSProp(cx,vp)){
			NotifyChangeListeners();
		}
		break;
	}
}

void ml3DShadow::GetJSProperty(JSContext *cx, int aiID, jsval *vp){
	switch(aiID){
	case JSPROP_type:{
		JSString* jssType=NULL;
		if(type==MLSHT_NONE){
			jssType=wr_JS_NewUCStringCopyZ(cx,MLSHTN_NONE);
		}else if(type==MLSHT_PROJECTIVE){
			jssType=wr_JS_NewUCStringCopyZ(cx,MLSHTN_PROJECTIVE);
		}
		if(jssType!=NULL)
			*vp=STRING_TO_JSVAL(jssType);
		}break;
	case JSPROP_plane:{
		char* psz=JS_smprintf("(%lf,%lf,%lf,%lf)", plane.x, plane.y, plane.z, plane.d);
		JSString* jssPlane=JS_NewStringCopyZ(cx,psz);
		JS_smprintf_free(psz);
		*vp=STRING_TO_JSVAL(jssPlane);
		}break;
	case JSPROP_color:{
		char* psz=JS_smprintf("#%02x%02x%02x%02x", color.a, color.r, color.g, color.b);
		JSString* jssColor=JS_NewStringCopyZ(cx,psz);
		JS_smprintf_free(psz);
		*vp=STRING_TO_JSVAL(jssColor);
		}break;
	case JSPROP_light:
		*vp=JSVAL_NULL;
		if(mbtPropSet & SHPS_light){
			if(light!=NULL)
				*vp=OBJECT_TO_JSVAL(light->mjso);
		}else{
			if(mjssLight!=NULL)
				*vp=STRING_TO_JSVAL(mjssLight);
		}
		break;
	}
}

bool ml3DShadow::SetLightRefJSProp(JSContext *cx, jsval *vp){
	if(JSVAL_IS_NULL(*vp)){
		light=NULL;
	}else if(JSVAL_IS_OBJECT(*vp)){
		JSObject* jso=JSVAL_TO_OBJECT(*vp);
		if(mlRMMLLight::IsInstance(cx, jso)){
			light=(mlRMMLLight*)JS_GetPrivate(cx,jso);
		}else{
			JS_ReportError(cx, "Parameter is not a Light-object");
			return false;
		}
	}else if(JSVAL_IS_STRING(*vp)){
		mlRMMLElement* pMLElThis=GetElem_ml3DShadow();
		JSString* jss=JSVAL_TO_STRING(*vp);
		if(!IsChildrenMLElsCreated()){
			mjssLight=jss;
			SAVE_STR_FROM_GC(cx,pMLElThis->mjso,mjssLight);
			return false;
		}
		// 
		mlRMMLElement* pMLElParent=pMLElThis->mpParent;
		while(pMLElParent!=NULL){
			if(pMLElParent->mRMMLType==MLMT_SCENE3D) break;
			pMLElParent=pMLElParent->mpParent;
		}
		if(pMLElParent==NULL){
			return false;
		}
		mlRMMLElement* pMLEl=pMLElParent->GetChild(wr_JS_GetStringChars(jss));
		if(pMLEl==NULL){ 
			JS_ReportError(cx, "Light-object '%s' for '%s' not found",JS_GetStringBytes(jss),cLPCSTR(pMLElThis->GetName()));
			return false;
		}
		if(pMLEl->mRMMLType!=MLMT_LIGHT){
			JS_ReportError(cx, "Named object is not a Light-object");
			return false;
		}
		if(!(mbtPropSet & SHPS_light) && mjssLight!=NULL){
			FREE_STR_FOR_GC(cx,pMLElThis->mjso,mjssLight); 
		}
		light=(mlRMMLLight*)pMLEl;
		mbtPropSet|=SHPS_light;
	}
	return true;
}

///// JavaScript Set Property Wrapper
JSBool mlRMMLShadow3D::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLShadow3D);
		SET_PROTO_PROP(mlRMMLElement);
		default:
			priv->ml3DShadow::SetJSProperty(cx, iID, vp);
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLShadow3D::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLShadow3D);
		GET_PROTO_PROP(mlRMMLElement);
		default:
			priv->ml3DShadow::GetJSProperty(cx, iID, vp);
	GET_PROPS_END;
	return JS_TRUE;
}

// Реализация mlRMMLElement
mlresult mlRMMLShadow3D::CreateMedia(omsContext* amcx){
	if(mjssLight!=NULL && !(mbtPropSet & SHPS_light)){
		mpMO = PMOV_CREATED;
		jsval v=STRING_TO_JSVAL(mjssLight);
		JS_SetProperty(mcx,mjso,"light",&v); 
		mpMO=NULL;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLShadow3D::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLShadow3D* pNewEL=(mlRMMLShadow3D*)GET_RMMLEL(mcx, mlRMMLShadow3D::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

void mlRMMLShadow3D::NotifyChangeListeners(){
	if(mpParent!=NULL && mpParent->mRMMLType==MLMT_SHADOWS3D)
		((mlRMMLShadows3D*)mpParent)->NotifyChangeListeners();
}

mlE3DShadowType mlRMMLShadow3D::GetType(){
	if(!(mbtPropSet & SHPS_type) && mpParent!=NULL && mpParent->mRMMLType==MLMT_SHADOWS3D)
		return ((mlRMMLShadows3D*)mpParent)->GetType();
	return type;
}
ml3DPlane mlRMMLShadow3D::GetPlane(){
	if(!(mbtPropSet & SHPS_plane) && mpParent!=NULL && mpParent->mRMMLType==MLMT_SHADOWS3D)
		return ((mlRMMLShadows3D*)mpParent)->GetPlane();
	return mlRMMLShadow3D::plane;
}
mlColor mlRMMLShadow3D::GetColor(){
	// если цвет не установлен в xml, то выдать цвет родителя
	if(!(mbtPropSet & SHPS_color) && mpParent!=NULL && mpParent->mRMMLType==MLMT_SHADOWS3D)
		return ((mlRMMLShadows3D*)mpParent)->GetColor();
	return color;
}
mlMedia* mlRMMLShadow3D::GetLight(){
	if(!(mbtPropSet & SHPS_light) && mpParent!=NULL && mpParent->mRMMLType==MLMT_SHADOWS3D)
		return ((mlRMMLShadows3D*)mpParent)->GetLight();
	return light;
}

//bool mlRMMLShadow3D::ParseType(JSContext* cx, wchar_t* &apwc){
//	wchar_t* pwcDelim=GetNextDelimPos(apwc);
//	int iLen=pwcDelim-apwc;
//	if(isEqual(apwc,L"none",iLen)){
//		mShadow.type=MLSHT_NONE;
//	}else if(isEqual(apwc,L"projective",iLen)){
//		mShadow.type=MLSHT_PROJECTIVE;
//	}else{
//		JS_ReportError(cx,"Invalid shadow type");
//		return false;
//	}
//	apwc=SkipDelims(pwcDelim);
//	return true;
//}

mlPropParseInfo mlPlaneParseInfo[]={
	{L"x",MLPT_DOUBLE,NULL},
	{L"y",MLPT_DOUBLE,NULL},
	{L"z",MLPT_DOUBLE,NULL},
	{L"d",MLPT_DOUBLE,NULL},
	{L"",MLPT_UNKNOWN,NULL}
};

bool ml3DShadow::ParsePlane(JSContext* cx, wchar_t* &apwc, ml3DPlane &aPlane){
	mlPlaneParseInfo[0].ptr=&aPlane.x;
	mlPlaneParseInfo[1].ptr=&aPlane.y;
	mlPlaneParseInfo[2].ptr=&aPlane.z;
	mlPlaneParseInfo[3].ptr=&aPlane.d;
	return ParseStruct(cx, (const wchar_t *&) apwc, mlPlaneParseInfo);
}

__forceinline bool IsValidIDChar(wchar_t awc){
	if(awc==L'_') return true;
	if(awc>=L'a' && awc<=L'z') return true;
	if(awc>=L'A' && awc<=L'Z') return true;
	return false;
}

bool ParseDID(JSContext* cx, const wchar_t* &apwc, const wchar_t* &pID){
	apwc=SkipSpaces(apwc);
	const wchar_t* pwc=apwc;
	while(*pwc!=L'\0' && (*pwc==L'.' || IsValidIDChar(*pwc))) pwc++;
	if(pwc==apwc) return false;
	JSString* jss=wr_JS_NewUCStringCopyN(cx,apwc,pwc-apwc);
	pID=wr_JS_GetStringChars(jss);
	apwc=pwc;
	return true;
}

//void mlRMMLShadow3D::ParseValue(JSContext* cx, wchar_t* apwcValue){
//	wchar_t* pwc=apwcValue;
//	if(pwc==NULL) return;
//	pwc=SkipSpaces(pwc);
//	if(*pwc==L'\0'){ mShadow.type=MLSHT_NONE; return; }
//	int iPNLen=GetPropName(pwc);
//	if(iPNLen==0 || isEqual(pwc,L"type",iPNLen)){
//		pwc=SkipPropName(pwc,iPNLen);
//		if(!ParseType(cx,pwc))
//			return;
//	}else{
//		JS_ReportError(cx,"Type must be specified first for a shadow");
//		return;
//	}
//	if(mShadow.type==MLSHT_NONE) return;
//	if(mShadow.type==MLSHT_PROJECTIVE){
//		pwc=SkipDelims(pwc);
//		for(int iParamNum=1; *pwc!=L'\0'; iParamNum++){
//			int iPNLen=GetPropName(pwc);
//			if((iPNLen==0 && iParamNum==1) || (iPNLen>0 && isEqual(pwc,L"plane",iPNLen))){
//				pwc=SkipPropName(pwc,iPNLen);
//				if(!ParsePlane(cx,pwc,mShadow.plane)) return;
//				pwc=SkipDelims(pwc);
//			}else if((iPNLen==0 && iParamNum==2) || (iPNLen>0 && isEqual(pwc,L"color",iPNLen))){
//				pwc=SkipPropName(pwc,iPNLen);
//				if(!ParseColor(cx,pwc,mShadow.color)) return;
//				pwc=SkipDelims(pwc);
//			}else if((iPNLen==0 && iParamNum==3) || (iPNLen>0 && isEqual(pwc,L"light",iPNLen))){
//				pwc=SkipPropName(pwc,iPNLen);
//				if(!ParseDID(cx,pwc,(wchar_t*&)mShadow.light)) return;
//				pwc=SkipDelims(pwc);
//			}else{
//				if(iPNLen==0) break;
//				wchar_t wc=pwc[iPNLen]; pwc[iPNLen]=0;
//				JS_ReportError(cx,"Unknown shadow property '%s'",cLPCSTR(pwc));
//				pwc[iPNLen]=wc;
//				return;
//			}
//		}
//		// ??
//	}
//	// ??
//}

}
