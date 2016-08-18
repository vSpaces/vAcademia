
#include "mlRMML.h"
#include "mlSPParser.h"

namespace rmml {

mlRMMLScene3D::mlRMMLScene3D(void)
{
	mRMMLType=MLMT_SCENE3D;
	mType=MLMT_SCENE3D;

	useMapManager = false;

	mbClearingIntersections = false;

//	ResetEditableBBoxColor();

//	ML_INIT_COLOR(mBBoxColor); mBBoxColor.a = 0;	// прозрачный
}

mlRMMLScene3D::~mlRMMLScene3D(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLScene3D::~mlRMMLScene3D");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLScene3D::_JSPropertySpec[] = {
//	{ "_camera", JSPROP__camera, JSPROP_ENUMERATE, 0, 0},
	JSPROP_RW(useMapManager)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLScene3D::_JSFunctionSpec[] = {
//	JSFUNC(setEditableBoundsColor,1)	// установить цвет boundbox-а всех редектируемых объектов и показать их, если цвет непрозрачный
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Scene3D,mlRMMLScene3D,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLScene3D)


///// JavaScript Set Property Wrapper
JSBool mlRMMLScene3D::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLScene3D);
		SET_PROTO_PROP(mlRMMLElement);
		default:
			switch(iID){
			case JSPROP_useMapManager:
				ML_JSVAL_TO_BOOL(priv->useMapManager, *vp);
				priv->UseMapManagerChanged();
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLScene3D::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLScene3D);
		GET_PROTO_PROP(mlRMMLElement);
		default:
			switch(iID){
			case JSPROP_useMapManager:
				*vp = BOOLEAN_TO_JSVAL(priv->useMapManager);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLScene3D::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLScene3D::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlRMMLElement* mlRMMLScene3D::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLScene3D* pNewEL=(mlRMMLScene3D*)GET_RMMLEL(mcx, mlRMMLScene3D::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

void mlRMMLScene3D::ClearIntersections(){
	mbClearingIntersections = true;
mlTrace(mcx, "mlRMMLScene3D::ClearIntersections");
	// перебрать все дочерние объекты и запросить у них значение свойства intersections
	mlRMMLIterator Iter(mcx, this);
	mlRMMLElement* pElem = NULL;
	while((pElem = Iter.GetNext()) != NULL){
		if(PMO_IS_NULL(pElem->GetMO()))
			continue;

		mlRMML3DObject* p3DObj = pElem->Get3DObject();
		if(p3DObj == NULL) continue;
//mlTrace(mcx, "mlRMMLScene3D::ClearIntersections %s", cLPCSTR(pElem->GetName()));
		p3DObj->ClearIntersections();
	}
	mbClearingIntersections = false;
}

// moICamera
#define MOISCENE3DEVENT_IMPL(M) \
void mlRMMLScene3D::M(){ \
	if(PMO_IS_NULL(mpMO)) return; \
	mpMO->GetIScene3D()->M(); \
}

MOISCENE3DEVENT_IMPL(UseMapManagerChanged); 
/*
void mlRMMLScene3D::ResetEditableBBoxColor(){
	ML_INIT_COLOR(mEditableBBoxColor);
	mEditableBBoxColor.a = 0; // по умолчанию прозрачный
}

bool mlRMMLScene3D::IsEditableObject(mlRMMLElement* apElem){
	if(apElem == NULL)
		return false;
	unsigned int uiID = apElem->GetID();
	if(uiID == OBJ_ID_UNDEF)
		return false;
	if(uiID < 1000000000 || uiID >= 4000000000)
		return false;
	if(apElem->mRMMLType == MLMT_OBJECT || apElem->mRMMLType == MLMT_GROUP)
		return true;
	return false;
}

JSBool ParseColor(JSContext* cx, jsval v, mlColor& aColor){
	//	mlStyle::Color Color;
	if(JSVAL_IS_STRING(v)){
		jschar* jscVal=JS_GetStringChars(JSVAL_TO_STRING(v));
		if(!ParseColor(cx, jscVal, aColor))
			return JS_FALSE;
	}else
		return JS_FALSE;
	return JS_TRUE;
}

JSBool mlRMMLScene3D::JSFUNC_setEditableBoundsColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	//	if (argc < 1){
	//		JS_ReportError(cx,"setEditableBoundsColor() must get one color argument");
	//		return JS_FALSE;
	//	}
	mlRMMLScene3D* pScene3D = (mlRMMLScene3D*)JS_GetPrivate(cx, obj);
	if(pScene3D == NULL){
		JS_ReportError(cx,"setEditableBoundsColor() strange error");
		return JS_FALSE;
	}

	if(argc > 0){
		if(!ParseColor(cx, argv[0], pScene3D->mEditableBBoxColor)){
			JS_ReportWarning(cx,"setEditableBoundsColor() argument is invalid");
			return JS_TRUE;
		}
	}

	// перебираем все 3D-объекты сцены, которую показывает viewport
	// и у котороых есть серверный ID и он >1000000000 и <4000000000
	// (редактируемые объекты)
	v_elems::iterator vi;
	for(vi = pScene3D->mpChildren->begin(); vi != pScene3D->mpChildren->end(); vi++ ){
		mlRMMLElement* pChild=*vi;
		if(pChild == NULL) continue;
		if(!(pChild->IsJSO())) continue; // skip scripts
		if(pScene3D->IsEditableObject(pChild)){
			mlRMML3DObject* p3DObj = pChild->Get3DObject();
			if(p3DObj != NULL)
				p3DObj->BoundingBoxChanged();
		}
	}

	return JS_TRUE;
}

// получить цвет по умолчанию для bounding-box-а заданного объекта
mlColor mlRMMLScene3D::GetDefaultBoundingBoxColor(mlRMMLElement* apElem){
	if(IsEditableObject(apElem))
		return mEditableBBoxColor;
	return mBBoxColor;
}
*/

// Зарегистрировать видимый cloud
bool mlRMMLScene3D::RegisterVisibleCloud(mlRMMLCloud* apCloud){
	CloudVector::const_iterator vi = mvVisibleClouds.begin();
	for(; vi != mvVisibleClouds.end(); vi++){
		if((*vi) == apCloud)
			return true;
	}
	mvVisibleClouds.push_back(apCloud);
	return true;
}

// Разрегистрировать cloud
void mlRMMLScene3D::UnregisterCloud(mlRMMLCloud* apCloud){
	CloudVector::iterator vi = mvVisibleClouds.begin();
	for(; vi != mvVisibleClouds.end(); ){
		if((*vi) == apCloud){
			vi = mvVisibleClouds.erase(vi);
		}else{
			vi++;
		}
	}
}

// Поискать кнопку в cloud-е, который под мышкой
mlRMMLElement* mlRMMLScene3D::GetCloudButtonUnderMouse(mlPoint &aMouseXY, mlButtonUnderMouse &aBUM, mlRMMLCloud* &apCloud){
	mlRMMLElement* pMLElMinDepth = NULL;
	mlRMMLCloud* pCloudMinDepth = NULL;
	int iMinDepth = 32000;
	mlButtonUnderMouse oBUM;
	CloudVector::const_iterator vi = mvVisibleClouds.begin();
	for(; vi != mvVisibleClouds.end(); vi++){
		mlRMMLCloud* pCloud = *vi;
		if(pCloud == NULL)
			continue;
		mlRMMLComposition* pContent = (mlRMMLComposition*)pCloud->GetContent();
		if(pContent == NULL)
			continue;
		mlRMMLElement* pMLElBut = pContent->GetButtonUnderMouse(aMouseXY, oBUM);
		if(pMLElBut == NULL || pMLElBut->GetVisible() == NULL)
			continue;
		if(!pMLElBut->GetVisible()->GetAbsVisible())
			continue;
		int iDepth = pMLElBut->GetVisible()->GetAbsDepth();
		if(iDepth < iMinDepth){
			pMLElMinDepth = pMLElBut;
			aBUM = oBUM;
			pCloudMinDepth = pCloud;
			iMinDepth = iDepth;
		}
	}
	if(pMLElMinDepth == NULL)
		return NULL;
	if(!pMLElMinDepth->mbBtnEvListenersIsSet)
		return NULL;	// ? возможно не стоит делать cloud-ы "прозрачными" для мыши
	apCloud = pCloudMinDepth;
	return pMLElMinDepth;
}

}
