#include "mlRMML.h"

namespace rmml {

mlRMMLMap::mlRMMLMap(void)
{
	mRMMLType=MLMT_MAP;
	mType=MLMT_MAP;

	source = NULL;
	mjssSource = NULL;
}

mlRMMLMap::~mlRMMLMap(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLMap::~mlRMMLMap");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLMap::_JSPropertySpec[] = {
	JSPROP_RW(source)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLMap::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Map,mlRMMLMap,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLMap)


///// JavaScript Set Property Wrapper
JSBool mlRMMLMap::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLMap);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_source:{
				if(priv->SetSourceRefJSProp(cx,vp))
					priv->NotifyChangeListeners();
				//*vp = JSVAL_NULL;	// Tandy 07.12.05 затычка бага с невыгрузкой сцены из-за
									// Classes.ceBasuEquip.obj.__proto__.materials.__2.diffuse_map.source._parent
									// (когда source-у явно присваивается объект изображения (setMap из equip.xml))
				}break;
			}
	SET_PROPS_END;
	if (JSVAL_IS_INT(id)){
		mlRMMLMap* priv=(mlRMMLMap*)JS_GetPrivate(cx, obj);
		if (priv==NULL){ *vp = JSVAL_NULL; return JS_TRUE; }
		int iID=JSVAL_TO_INT(id);
		if(iID == mlRMMLLoadable::JSPROP_src){
			// ScrChanged, создаем новую картинку с материалом
			if(priv->source != NULL){
				jsval vSrc = STRING_TO_JSVAL(priv->src);
				wr_JS_SetUCProperty(cx, priv->source->mjso, L"src", -1, &vSrc);
				priv->NotifyChangeListeners();
			}else
				priv->CreateDefaultMapImage();
		}
	}
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLMap::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	if (JSVAL_IS_INT(id)){
		int iID=JSVAL_TO_INT(id);
		mlRMMLMap* priv=(mlRMMLMap*)JS_GetPrivate(cx, obj);
		if (priv==NULL){ *vp = JSVAL_NULL; return JS_TRUE; }
		if(iID == mlRMMLLoadable::JSPROP_src){
			// ScrChanged, создаем новую картинку с материалом
			if(priv->source != NULL){
				if(priv->btRefFlags & REFF_SOURCE){
					wr_JS_GetUCProperty(cx, priv->source->mjso, L"src", -1, vp);
				}else 
					*vp = JS_GetEmptyStringValue(cx);
			}else{
				// Пытаемся от RenderManager-а получить src изображения текстуры
				mlString sSrc;
				bool bRet = priv->GetTextureSrc(sSrc);
				if(!bRet)
					*vp = JS_GetEmptyStringValue(cx);
				else
					*vp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sSrc.c_str()));
			}
			return JS_TRUE;
		}
	}
	GET_PROPS_BEGIN(mlRMMLMap);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_source:
				*vp=JSVAL_NULL;
				if(priv->btRefFlags & REFF_SOURCE){
					if(priv->source!=NULL)
						*vp=OBJECT_TO_JSVAL(priv->source->mjso);
				}else{
					if(priv->mjssSource!=NULL)
						*vp=STRING_TO_JSVAL(priv->mjssSource);
				}
if(*vp == JSVAL_NULL)
int hh=0;
				break;
			}
	GET_PROPS_END;
return JS_TRUE;
}

// Реализация mlRMMLElement
mlresult mlRMMLMap::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLMap::CreateMedia");
	mpMO = PMOV_CREATED;
	if(mjssSource!=NULL && !(btRefFlags & REFF_SOURCE)){
		jsval v=STRING_TO_JSVAL(mjssSource);
		JS_SetProperty(mcx,mjso,"source",&v); 
	}
	NotifyChangeListeners(); // 28.04.2009
	return ML_OK;
}

mlresult mlRMMLMap::Load(){
	CreateDefaultMapImage();
	return ML_OK;
}

mlRMMLElement* mlRMMLMap::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLMap* pNewEL=(mlRMMLMap*)GET_RMMLEL(mcx, mlRMMLMap::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// реализация mlIMap
//mlMedia* mlRMMLMap::GetSource(){ 
//	if (source != NULL)
//		return source;
//	// ??
//}

bool mlRMMLMap::CreateDefaultMapImage(){
	if(mpMO == NULL) return false;
	if(source != NULL) return false;
	const wchar_t* pwcSrc = src!=NULL?wr_JS_GetStringChars(src):NULL; GetLoadable()->GetSrc();
	if(pwcSrc == NULL){
		JS_DeleteProperty(mcx, mjso, "__DefMap_");
		NotifyChangeListeners();
		return true;
	}
	JSObject* jsoImg = mlRMMLImage::newJSObject(mcx);
	jsval vVisible = BOOLEAN_TO_JSVAL(false);
	wr_JS_SetUCProperty(mcx, jsoImg, L"_visible", -1, &vVisible);
	jsval vSrc = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx,pwcSrc));
	wr_JS_SetUCProperty(mcx, jsoImg, L"src", -1, &vSrc);
	jsval vFor3D = BOOLEAN_TO_JSVAL(true);
	wr_JS_SetUCProperty(mcx, jsoImg, L"for3d", -1, &vFor3D);
	jsval vImg = OBJECT_TO_JSVAL(jsoImg);
	wr_JS_SetUCProperty(mcx, mjso, L"__DefMap_", -1, &vImg);
	if(!(btRefFlags & REFF_SOURCE) && mjssSource!=NULL)
		FREE_STR_FOR_GC(mcx,mjso,mjssSource);
	source = (mlRMMLElement*)JS_GetPrivate(mcx, jsoImg);
	source->SetSrcFileIdx(miSrcFilesIdx, miSrcLine);
	source->CreateAndLoadMOs();
	btRefFlags |= REFF_SOURCE;
	NotifyChangeListeners();
	return true;
}

bool mlRMMLMap::SetSourceRefJSProp(JSContext *cx, jsval *vp){
	if(JSVAL_IS_NULL(*vp) || JSVAL_IS_VOID(*vp)){
		source=NULL;
	}else if(JSVAL_IS_OBJECT(*vp)){
		JSObject* jso=JSVAL_TO_OBJECT(*vp);
		if(mlRMMLImage::IsInstance(cx, jso) || mlRMMLAnimation::IsInstance(cx, jso) 
			|| mlRMMLVideo::IsInstance(cx, jso) || mlRMMLActiveX::IsInstance(cx, jso)
			|| mlRMMLBrowser::IsInstance(cx, jso) || mlRMMLFlash::IsInstance(cx, jso)
			|| mlRMMLComposition::IsInstance(cx, jso)
		){
			source=(mlRMMLElement*)JS_GetPrivate(cx,jso);
		}else{
			JS_ReportError(mcx, "Parameter is not a image/composition/animation/video/activeX-object");
			return false;		}
	}else if(JSVAL_IS_STRING(*vp)){
		JSString* jss=JSVAL_TO_STRING(*vp);
		if(mpMO==NULL){
			mjssSource=jss;
			SAVE_STR_FROM_GC(mcx,mjso,mjssSource);
			return true;
		}
		mlRMMLElement* pMLEl=FindElemByName2(wr_JS_GetStringChars(jss));
		if(pMLEl==NULL){ 
			JS_ReportError(mcx, "source-object '%s' for '%s' not found",JS_GetStringBytes(jss),cLPCSTR(GetName()));
			return false;
		}
		if(pMLEl->mRMMLType!=MLMT_IMAGE && pMLEl->mRMMLType!=MLMT_ANIMATION 
			&& pMLEl->mRMMLType!=MLMT_VIDEO && pMLEl->mRMMLType!=MLMT_ACTIVEX
			&& pMLEl->mRMMLType!=MLMT_BROWSER && pMLEl->mRMMLType!=MLMT_FLASH
			&& pMLEl->mRMMLType!=MLMT_COMPOSITION
		){
			JS_ReportError(mcx, "Named object is not a image/composition/animation/html/video/activeX-object");
			return false;
		}
		if(!(btRefFlags & REFF_SOURCE) && mjssSource!=NULL)
			FREE_STR_FOR_GC(mcx,mjso,mjssSource); 
		source=pMLEl;
		btRefFlags |= REFF_SOURCE;
	}
	return true;
}

void mlRMMLMap::NotifyChangeListeners(){
	if(mpParent == NULL) return;
	if(mpParent->mRMMLType != MLMT_MATERIAL) return;
	mlRMMLMaterial* pMaterial = (mlRMMLMaterial*)mpParent;
	if(pMaterial->mpParent == NULL) return;
	if(pMaterial->mpParent->mRMMLType != MLMT_MATERIALS) return;
	mlRMMLMaterials* pMaterials = (mlRMMLMaterials*)pMaterial->mpParent;
	pMaterials->NotifyChangeListeners(pMaterial, this);
}

bool mlRMMLMap::GetTextureSrc(mlString &asSrc){
	if(mpParent == NULL) return NULL;
	if(mpParent->mRMMLType != MLMT_MATERIAL) return NULL;
	mlRMMLMaterial* pMaterial = (mlRMMLMaterial*)mpParent;
	if(pMaterial->mpParent == NULL) return NULL;
	if(pMaterial->mpParent->mRMMLType != MLMT_MATERIALS) return NULL;
	mlRMMLMaterials* pMaterials = (mlRMMLMaterials*)pMaterial->mpParent;
	return pMaterials->GetMaterialTextureSrc(pMaterial, this, asSrc);
}

}














