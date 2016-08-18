
#include "mlRMML.h"
#include "ILogWriter.h"

namespace rmml {

mlRMMLObject::mlRMMLObject(void)
{
	mRMMLType=MLMT_OBJECT;
	mType=MLMT_OBJECT;

	billboard = false;
	unpickable = false;
	mbProbablyHandlerFuncIsSet = true;
}

mlRMMLObject::~mlRMMLObject(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLObject::~mlRMMLObject");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLObject::_JSPropertySpec[] = {
	JSPROP_RW(shadow)
	JSPROP_RW(materials)
	JSPROP_RW(motion)
	JSPROP_RW(gravitation)
	JSPROP_RW(intangible)
	JSPROP_RW(billboard)
	JSPROP_RW(unpickable)
	JSPROP_RW(color)
	JSPROP_RW(opacity)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLObject::_JSFunctionSpec[] = {
	JSFUNC(queryFullStateZatuchka, 0)
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	JSFUNC(getSubobjBounds, 1)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLObject::_EventSpec[] = {
	MLEVENT(onLODChanged)
	{0, 0, 0}
};

MLJSCLASS_IMPL_BEGIN(Object3D,mlRMMLObject,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMML3DObject)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLObject)


///// JavaScript Set Property Wrapper
JSBool mlRMMLObject::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLObject);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMML3DObject);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
				case JSPROP_shadow:{
					if(priv->SetShadowRefJSProp(cx,vp))
						priv->ShadowChanged();
					}break;
				case JSPROP_materials:{
					if(priv->SetMaterialsRefJSProp(cx,vp))
						priv->MaterialsChanged();
					}break;
				case JSPROP_motion:{
					if(priv->SetMotionRefJSProp(cx,vp))
						priv->MotionChanged();
					}break;
				case JSPROP_gravitation:{
					ML_JSVAL_TO_BOOL(priv->gravitation,*vp);
					priv->GravitationChanged();
					}break;
				case JSPROP_intangible:{
					ML_JSVAL_TO_BOOL(priv->intangible,*vp);
					priv->IntangibleChanged();
					}break;
				case JSPROP_billboard:{
					ML_JSVAL_TO_BOOL(priv->billboard,*vp);
					priv->BillboardChanged();
					}break;
				case JSPROP_unpickable:{
					ML_JSVAL_TO_BOOL(priv->unpickable,*vp);
					priv->UnpickableChanged();
					}break;
				case JSPROP_color:{
					priv->Get3DObject()->SetColorJSProp(cx, *vp);
					}break;
				case JSPROP_opacity:{
					priv->Get3DObject()->SetOpacityJSProp(cx, *vp);
					}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

#define GET_REF_JSPROP2(OBJ,JSS,FLG) \
					*vp=JSVAL_NULL; \
					if(priv->btRefFlags & FLG){ \
						if(OBJ!=NULL) \
							*vp=OBJECT_TO_JSVAL(OBJ->mjso); \
					}else{ \
						if(JSS!=NULL) \
							*vp=STRING_TO_JSVAL(JSS); \
					}

///// JavaScript Get Property Wrapper
JSBool mlRMMLObject::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLObject);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMML3DObject);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
				case JSPROP_shadow:{
					GET_REF_JSPROP2(priv->shadow,priv->mjssShadow,REFF_SHADOW)
					}break;
				case JSPROP_materials:{
//{
//if(JSVAL_IS_REAL_OBJECT(*vp)){
//JSObject* jso = JSVAL_TO_OBJECT(*vp);
//if(JSO_IS_MLEL(jso)){
//mlRMMLElement* pML = (mlRMMLElement*)JS_GetPrivate(cx, jso);
//int hh=0;
//}
//}
//}
					*vp=JSVAL_NULL;
					if(priv->btRefFlags & REFF_MATERIALS){
						if(priv->materials!=NULL)
							*vp=OBJECT_TO_JSVAL(priv->materials->mjso);
					}else{
						if(priv->mjssMaterials!=NULL)
							*vp=STRING_TO_JSVAL(priv->mjssMaterials);
						else{
							priv->CreateDefaultMaterials();
							if(priv->materials!=NULL)
								*vp=OBJECT_TO_JSVAL(priv->materials->mjso);
						}
					}
					}break;
				case JSPROP_motion:{
					GET_REF_JSPROP2(priv->motion,priv->mjssMotion,REFF_MOTION)
					}break;
				case JSPROP_gravitation:{
					*vp = BOOLEAN_TO_JSVAL(priv->gravitation);
					}break;
				case JSPROP_intangible:{
					*vp = BOOLEAN_TO_JSVAL(priv->intangible);
					}break;
				case JSPROP_billboard:{
					*vp = BOOLEAN_TO_JSVAL(priv->billboard);
					}break;
				case JSPROP_unpickable:{
					*vp = BOOLEAN_TO_JSVAL(priv->billboard);
					}break;
				case EVID_onLODChanged:{
					int iRes = GPSM(cx)->CheckEvent(iID, priv, vp);
					if(iRes >= 0)
						*vp = BOOLEAN_TO_JSVAL(iRes);
					}break;
				case JSPROP_color:{
					priv->GetColorJSProp(cx, vp);
					}break;
				case JSPROP_opacity:{
					priv->GetOpacityJSProp(cx, vp);
					}break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLObject::CreateMedia(omsContext* amcx){
	mpMO = PMOV_CREATED;
	if(mjssShadow!=NULL && !(btRefFlags & REFF_SHADOW)){
		jsval v=STRING_TO_JSVAL(mjssShadow);
		JS_SetProperty(mcx,mjso,MLELPN_SHADOW,&v); 
	}
	if(mjssMaterials!=NULL && !(btRefFlags & REFF_MATERIALS)){
		jsval v=STRING_TO_JSVAL(mjssMaterials);
		JS_SetProperty(mcx,mjso,MLELPN_MATERIALS,&v); 
	}
	if(mjssMotion!=NULL && !(btRefFlags & REFF_MOTION)){
		jsval v=STRING_TO_JSVAL(mjssMotion);
		JS_SetProperty(mcx,mjso,MLELPN_MOTION,&v); 
	}
	mpMO=NULL; // ??
	//
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLObject::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLObject::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. Restore
		return ML_ERROR_FAILURE;
	}else{
		SetPSR2MO();
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLObject::Duplicate(mlRMMLElement* apNewParent){
	JSObject* jsoNew = mlRMMLObject::newJSObject(mcx);
	mlRMMLObject* pNewEL=(mlRMMLObject*)GET_RMMLEL(mcx, jsoNew);
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

void mlRMMLObject::BillboardChanged(){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D==NULL)	return;
	pmo3D->BillboardChanged();
}

void mlRMMLObject::UnpickableChanged(){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)	return;
	pmo3D->UnpickableChanged();
}

void Set_Event_visionDistance(mlRMMLElement* apMLEl, double adVisionDistance){
	JSContext* cx = apMLEl->mcx;
	jsval v = INT_TO_JSVAL(0);
	mlJS_NewDoubleValue(cx, adVisionDistance, &v);
	JS_SetProperty(cx,GPSM(cx)->GetEventGO(),"visionDistance",&v);
}

void mlRMMLObject::onLODChanged(double adVisionDistance){
	mlRMMLElement* pMLEl = (mlRMMLElement*)this;
	bool bDontSendSynchEvents = (pMLEl->mucSynchFlags4Events & EVID_onLODChanged)?true:false;
	// если в моем update-е, 
	if(GPSM(pMLEl->mcx)->Updating()){
		// то выполняем сразу же
		Set_Event_visionDistance(pMLEl, adVisionDistance);
		GPSM(pMLEl->mcx)->mbDontSendSynchEvents = bDontSendSynchEvents;
		pMLEl->CallListeners(EVID_onLODChanged);
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(pMLEl->mcx)->SaveInternalPath(Data,pMLEl);
		Data << bDontSendSynchEvents;
		Data << adVisionDistance;
		// ставим в очередь событий
		GPSM(pMLEl->mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onLODChanged, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(pMLEl->mcx)->GetMode() == smmInitilization);
	}
}

bool mlRMMLObject::SetEventData(char aidEvent, mlSynchData &Data){
	if(mlRMMLElement::SetEventData(aidEvent, Data))
		return true;
	switch(aidEvent){
	case EVID_onLODChanged:{
		double dVisionDistance = 0;
		Data >> dVisionDistance;
		Set_Event_visionDistance(this, dVisionDistance);
		}break;
	}
	return true;
}

jsval mlRMMLObject::GetHandlerFunc(const wchar_t* apwcFuncName){
	jsval vFunc = JSVAL_NULL;
	if(JS_GetUCProperty(mcx, mjso, (const jschar*)apwcFuncName, -1, &vFunc) == JS_TRUE){
		if(JSVAL_IS_STRING(vFunc) || (JSVAL_IS_REAL_OBJECT(vFunc) && JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc)))){
		}else
			vFunc = JSVAL_NULL;
	}else 
		vFunc = JSVAL_NULL;
	if(vFunc == JSVAL_NULL)
		mbProbablyHandlerFuncIsSet = false;
	return vFunc;
}

mlresult mlRMMLObject::Update(const __int64 alTime)
{
	// z coordinate synchronization
	return ML_ERROR_NOT_IMPLEMENTED;
}

void mlRMMLObject::PosChanged(ml3DPosition &aPos){
	if (mpSynch != NULL && mpSynch->NeedToSetZ())
		aPos.z = mpSynch->GetZ();
	mlRMML3DObject::PosChanged(aPos);
}

//JSFUNC(queryFullStateZatuchka, 0) // запрос состояния объекта на сервер
JSBool mlRMMLObject::JSFUNC_queryFullStateZatuchka(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLObject* pObject = (mlRMMLObject*)JS_GetPrivate(cx, obj);

	GPSM(cx)->GetContext()->mpSyncMan->QueryObject(pObject->GetID(), pObject->GetBornRealityID(), GPSM(cx)->GetContext()->mpMapMan->GetCurrentRealityID(), 0, false);
	

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

// object getSubobjBounds(materialID)
JSBool mlRMMLObject::JSFUNC_getSubobjBounds(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	*rval = JSVAL_NULL;
	mlRMMLObject* pObject = (mlRMMLObject*)JS_GetPrivate(cx, obj);

	if(argc < 1){
		JS_ReportError(cx,"getSubobjBounds-method must get one argument");
		return JS_TRUE;
	}
	if(!JSVAL_IS_INT(argv[0])){
		JS_ReportError(cx,"getSubobjBounds-method argument must be an integer");
		return JS_TRUE;
	}

	int iMatID = JSVAL_TO_INT(argv[0]);

	ml3DBounds bndBounds = pObject->getSubobjBounds(iMatID);

	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, obj, jso);
	jsval val;
	mlJS_NewDoubleValue(cx, bndBounds.xMin, &val); JS_SetUCProperty(cx,jso,(const jschar*)L"xMin",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.xMax, &val); JS_SetUCProperty(cx,jso,(const jschar*)L"xMax",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.yMin, &val); JS_SetUCProperty(cx,jso,(const jschar*)L"yMin",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.yMax, &val); JS_SetUCProperty(cx,jso,(const jschar*)L"yMax",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.zMin, &val); JS_SetUCProperty(cx,jso,(const jschar*)L"zMin",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.zMax, &val); JS_SetUCProperty(cx,jso,(const jschar*)L"zMax",4,&val);
	*rval = OBJECT_TO_JSVAL(jso);
	FREE_FOR_GC(cx, obj, jso);
	return JS_TRUE;
}

}
