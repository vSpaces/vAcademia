#include "mlRMML.h"
#include "jsprf.h"
#include "mlSPParser.h"
#include "mlColorJSO.h"

namespace rmml {

mlRMMLMaterial::mlRMMLMaterial(void)
{
	mRMMLType=MLMT_MATERIAL;
	mType=MLMT_MATERIAL;
	ML_INIT_COLOR(diffuse);
	mjsoDiffuse = NULL;
	ML_INIT_COLOR(ambient);
	mjsoAmbient = NULL;
	ML_INIT_COLOR(specular);
	mjsoSpecular = NULL;
	ML_INIT_COLOR(emissive);
	mjsoEmissive = NULL;
	power = 0;
	opacity = 0xFF;
	maps = NULL;

}

mlRMMLMaterial::~mlRMMLMaterial(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLMaterial::~mlRMMLMaterial");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLMaterial::_JSPropertySpec[] = {
	JSPROP_RW(diffuse)
	JSPROP_RW(ambient)
	JSPROP_RW(specular)
	JSPROP_RW(emissive)
	JSPROP_RW(power)
	JSPROP_RW(opacity)
	JSPROP_RO(maps)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLMaterial::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Material,mlRMMLMaterial,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLMaterial)

#define GET_COLOR_PROP(NM) \
				if(JSVAL_IS_STRING(*vp)){ \
					JSString* jssColor=JSVAL_TO_STRING(*vp); \
					const wchar_t* pwc=(const wchar_t*)JS_GetStringChars(jssColor); \
					ParseColor(cx,pwc,priv->NM); \
					priv->NotifyChangeListeners(); \
				}else if(JSVAL_IS_REAL_OBJECT(*vp) && mlColorJSO::IsInstance(cx, JSVAL_TO_OBJECT(*vp))){\
					mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(*vp)); \
					priv->NM = pColor->GetColor(); \
					priv->NotifyChangeListeners(); \
				}else{ \
					JS_ReportError(cx,"Error while setting material"#NM"property"); \
				}

///// JavaScript Set Property Wrapper
JSBool mlRMMLMaterial::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLMaterial);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_diffuse:
				GET_COLOR_PROP(diffuse);
				break;
			case JSPROP_ambient:
				GET_COLOR_PROP(ambient);
				break;
			case JSPROP_specular:
				GET_COLOR_PROP(specular);
				break;
			case JSPROP_emissive:
				GET_COLOR_PROP(emissive);
				break;
			case JSPROP_power:{
				jsdouble jsdbl;
				if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){
					priv->power = jsdbl;
					priv->NotifyChangeListeners();
				}else{
					JS_ReportError(cx,"Error while setting material power property");
				}
				}break;
			case JSPROP_opacity:
				ML_JSVAL_TO_INT(priv->opacity,*vp);
				priv->NotifyChangeListeners();
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

void mlRMMLMaterial::CreateColorJSO(JSObject* &jso, mlColor* apColor){
	jso = mlColorJSO::newJSObject(mcx);
	mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(mcx,jso);
	pColor->SetColorRef(apColor, this);
	SAVE_FROM_GC(mcx,mjso,jso)
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLMaterial::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLMaterial);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_diffuse:{
//				mlColor* pColor = &(priv->diffuse);
//				char* psz=JS_smprintf("#%02x%02x%02x%02x", pColor->a, pColor->r, pColor->g, pColor->b);
//				JSString* jssColor=JS_NewStringCopyZ(cx,psz);
//				JS_smprintf_free(psz);
//				*vp=STRING_TO_JSVAL(jssColor);
				if(priv->mjsoDiffuse == NULL)
					priv->CreateColorJSO(priv->mjsoDiffuse, &priv->diffuse);
				*vp=OBJECT_TO_JSVAL(priv->mjsoDiffuse);
				}break;
			case JSPROP_ambient:{
				if(priv->mjsoAmbient == NULL)
					priv->CreateColorJSO(priv->mjsoAmbient, &priv->ambient);
				*vp=OBJECT_TO_JSVAL(priv->mjsoAmbient);
				}break;
			case JSPROP_specular:{
				if(priv->mjsoSpecular == NULL)
					priv->CreateColorJSO(priv->mjsoSpecular, &priv->specular);
				*vp=OBJECT_TO_JSVAL(priv->mjsoSpecular);
				}break;
			case JSPROP_emissive:{
				if(priv->mjsoEmissive == NULL)
					priv->CreateColorJSO(priv->mjsoEmissive, &priv->emissive);
				*vp=OBJECT_TO_JSVAL(priv->mjsoEmissive);
				}break;
			case JSPROP_power:{
				jsdouble* jsdbl=JS_NewDouble(cx, priv->power);
				*vp=DOUBLE_TO_JSVAL(jsdbl);
				}break;
			case JSPROP_opacity:
				*vp=INT_TO_JSVAL(priv->opacity);
				break;
			case JSPROP_maps:
				*vp = JSVAL_NULL;
				if(priv->maps == NULL){
					JSObject* jsoMaps = mlMaterialMaps::newJSObject(cx);
					priv->maps = (mlMaterialMaps*) JS_GetPrivate(cx,jsoMaps);
					SAVE_FROM_GC(cx, obj, jsoMaps)
					priv->maps->mpMaterial = priv;
				}
				if(priv->maps != NULL){
					*vp = OBJECT_TO_JSVAL(priv->maps->mjso);
if(((unsigned int)(priv->maps->mjso) & 0xcdcd0000) == 0xcdcd0000)
*vp = JSVAL_NULL;
				}
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

// Реализация mlRMMLElement
mlresult mlRMMLMaterial::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLMaterial::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLMaterial::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. 
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLMaterial::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLMaterial* pNewEL=(mlRMMLMaterial*)GET_RMMLEL(mcx, mlRMMLMaterial::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// реализация  mlIMaterial
mlMedia* mlRMMLMaterial::GetMap(const wchar_t* apwcIndex){
	/*jsval v;
	if(maps == NULL){
		JS_GetProperty(mcx, mjso, "maps", &v);
	}
	if(maps == NULL)
		return NULL;
	mlString sMapPropName = apwcIndex; sMapPropName+=L"_map";
	JS_GetUCProperty(mcx, maps->mjso, (const jschar*)sMapPropName.c_str(), -1, &v);
	if(JSVAL_IS_NULL(v) || JSVAL_IS_VOID(v) || !JSVAL_IS_OBJECT(v))
		return NULL;
	JSObject* jsoMap = JSVAL_TO_OBJECT(v);
	if(!mlRMMLMap::IsInstance(mcx, jsoMap))
		return NULL;
	mlRMMLMap* pMLElMap = (mlRMMLMap*)JS_GetPrivate(mcx, jsoMap);
	return pMLElMap->GetIMap()->GetSource();*/

	mlRMMLMap* pMLElMap = GetMapElem(apwcIndex, false);
	if (!pMLElMap)
		return NULL;

	return pMLElMap->GetIMap()->GetSource();
}

void mlRMMLMaterial::GetProps(moIMaterial* apMat){
	diffuse = apMat->GetDiffuse();
	ambient = apMat->GetAmbient();
	specular = apMat->GetSpecular();
	power = apMat->GetPower();
	emissive = apMat->GetEmissive();
	opacity = apMat->GetOpacity();
}

mlRMMLMap* mlRMMLMaterial::GetMapElem(const wchar_t* apwcIndex, bool abCreate){
	jsval v;
	mlString sMapPropName = apwcIndex; sMapPropName+=L"_map";
	JS_GetUCProperty(mcx, mjso, (const jschar*)sMapPropName.c_str(), -1, &v);
	if((JSVAL_IS_NULL(v) || JSVAL_IS_VOID(v)) && abCreate){
		// создаем новый объект, чтобы ему можно было src или source присвоить
		JSObject* jsoNewMap = mlRMMLMap::newJSObject(mcx);
		JSString* jssName = JS_NewUCStringCopyZ(mcx, (const jschar*)sMapPropName.c_str());
		v = STRING_TO_JSVAL(jssName);
		JS_SetProperty(mcx, jsoNewMap, MLELPN_NAME, &v);
		mlRMMLMap* pNewMap = (mlRMMLMap*)JS_GetPrivate(mcx, jsoNewMap);
		this->AddChild(pNewMap);
		return pNewMap;
	}
	if(!JSVAL_IS_OBJECT(v))
		return NULL;
	JSObject* jsoMap = JSVAL_TO_OBJECT(v);
	if(!mlRMMLMap::IsInstance(mcx, jsoMap))
		return NULL;
	return (mlRMMLMap*)JS_GetPrivate(mcx, jsoMap);
}

void mlRMMLMaterial::NotifyChangeListeners(){
	if(mpParent == NULL) return;
	if(mpParent->mRMMLType != MLMT_MATERIALS) return;
	mlRMMLMaterials* pMaterials = (mlRMMLMaterials*)mpParent;
	pMaterials->NotifyChangeListeners(this);
}

static const wchar_t* gwcaMapNames[] = {
	L"diffuse",
	L"ambient",
	L"specular",
	L"emissive",
	L"",
};

mlString mlRMMLMaterial::GetMapName(mlRMMLMap* apMap){
	for(int i = 0; ; i++){
		const wchar_t* wcMapName = gwcaMapNames[i];
		if(*wcMapName == L'\0')
			break;
		mlRMMLMap* pMap = GetMapElem(wcMapName, false);
		if(pMap == apMap)
			return wcMapName;
	}
	return L"";
}

//////////////////////////////////////////////
// mlMaterialMaps
//

mlMaterialMaps::mlMaterialMaps(void)
{
	mpMaterial=NULL;
}

mlMaterialMaps::~mlMaterialMaps(void){
}

///// JavaScript Variable Table
JSPropertySpec mlMaterialMaps::_JSPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlMaterialMaps::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(MaterialMaps,mlMaterialMaps,0)
MLJSCLASS_IMPL_END(mlMaterialMaps)

__forceinline bool IsValidMapName(const wchar_t* apwcMapName){
	if(isEqual(apwcMapName, L"diffuse"))
		return true;
	if(isEqual(apwcMapName, L"specular"))
		return true;
	return false;
}

JSBool mlMaterialMaps::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	mlMaterialMaps* priv = (mlMaterialMaps*)JS_GetPrivate(cx, obj);
	if(JSVAL_IS_STRING(id)){
		JSString* jssMapName = JSVAL_TO_STRING(id);
		jschar* jscMapName = JS_GetStringChars(jssMapName);
		if(!IsValidMapName((const wchar_t*)jscMapName)){
			JS_ReportError(cx, "Material map for %s is not supported", cLPCSTR((const wchar_t*)jscMapName));
			return JS_FALSE;
		}
		if(JSVAL_IS_NULL(*vp) || JSVAL_IS_VOID(*vp) || JSVAL_IS_OBJECT(*vp)){
			// установить source
			mlRMMLMap* pMLElMap = priv->mpMaterial->GetMapElem((const wchar_t*)jscMapName);
			ML_ASSERTION(cx, pMLElMap!=NULL, "mlMaterialMaps::JSGetProperty");
			return JS_SetUCProperty(cx, pMLElMap->mjso, (const jschar*)L"source", -1, vp);
		}else if(JSVAL_IS_STRING(*vp)){
			// установаить src
			mlRMMLMap* pMLElMap = priv->mpMaterial->GetMapElem((const wchar_t*)jscMapName);
			ML_ASSERTION(cx, pMLElMap!=NULL, "mlMaterialMaps::JSGetProperty");
			return JS_SetUCProperty(cx, pMLElMap->mjso, (const jschar*)L"src", -1, vp);
		}else{
			// ??
		}
	}
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlMaterialMaps::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	mlMaterialMaps* priv = (mlMaterialMaps*)JS_GetPrivate(cx, obj);
	if(JSVAL_IS_STRING(id)){
		JSString* jssMapName = JSVAL_TO_STRING(id);
		jschar* jscMapName = JS_GetStringChars(jssMapName);
		if(!IsValidMapName((wchar_t*)jscMapName)){
			JS_ReportError(cx, "Material map for %s is not supported", cLPCSTR((const wchar_t*)jscMapName));
			return JS_FALSE;
		}
		mlRMMLMap* pMLElMap = priv->mpMaterial->GetMapElem((const wchar_t*)jscMapName);
		ML_ASSERTION(cx, pMLElMap!=NULL, "mlMaterialMaps::JSGetProperty");
		*vp = OBJECT_TO_JSVAL(pMLElMap->mjso);
		// ??
//		if(*jscMapName == L'_' || isEqual(jscMapName, L"constructor"))
//			return JS_TRUE;
//		mlString sPropName = L"_"; sPropName += jscMapName;
//		jsval vProp;
//		JS_GetUCProperty(cx, obj, sPropName.c_str(), -1, &vProp);
//		if(JSVAL_IS_NULL(vProp) || JSVAL_IS_VOID(vProp)){
//			JSObject* jsoTrack = mlMotionTrack::newJSObject(cx);
//			mlMotionTrack* pMotionTrack = (mlMotionTrack*)JS_GetPrivate(cx, jsoTrack);
//			pMotionTrack->msBoneName = jscBoneName;
//			pMotionTrack->mpMotion = priv->mpMotion;
//			vProp = OBJECT_TO_JSVAL(jsoTrack);
//			JS_SetUCProperty(cx, obj, sPropName.c_str(), -1, &vProp);
//		}
//		*vp=vProp;
		// ??
	}
	return JS_TRUE;
}

}
