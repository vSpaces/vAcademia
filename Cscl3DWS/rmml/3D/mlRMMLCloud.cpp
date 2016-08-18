#include "mlRMML.h"

namespace rmml {

mlRMMLCloud::mlRMMLCloud(void)
{
	mRMMLType = MLMT_CLOUD;
	mType = MLMT_CLOUD;
	mbInverse = false;
	mjssContent = NULL;
	//
	visible = true;
	duration = 0.0;
	marginX = 0;
	marginY = 0;
	content = NULL;
	footnote = NULL;
	align = NULL;
	moveOutOfScreen = false;
	allowOverlap = false;
	sortOnDepth = true;
	maxVisibleDistance = 1500.0;
	ML_INIT_3DPOSITION(pos);
	attachedTo = NULL;
	mp3DOattachedTo = NULL;
	mbMLElsCreated = false;
	mpJSPos = NULL;
	mpScene3DRegistered = NULL;
}

mlRMMLCloud::~mlRMMLCloud(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLCloud::~mlRMMLCloud");
	if(content != NULL)
		content->GetVisible()->RemoveVListener(this);
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
	if(mpScene3DRegistered != NULL && !GPSM(mcx)->IsDestroying())
		Unregister();
	if( mpJSPos)
		MP_DELETE( mpJSPos);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLCloud::_JSPropertySpec[] = {
	JSPROP_RW(visible)
	JSPROP_RW(maxVisibleDistance)
	JSPROP_RW(inverse)
	JSPROP_RW(content)
	JSPROP_RW(footnote)
	JSPROP_RW(align)
	JSPROP_RW(moveOutOfScreen)
	JSPROP_RW(allowOverlap)
	JSPROP_RW(sortOnDepth)
	JSPROP_RW(position)
	JSPROP_RW(attachedTo)
	JSPROP_RW(marginX)
	JSPROP_RW(marginY)
	JSPROP_RW(viewVec)
	{ 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLCloud::_JSFunctionSpec[] = {
	JSFUNC(attachTo, 1)
	JSFUNC(setSpecParameters, 2)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Cloud,mlRMMLCloud,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
//	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLCloud)

#define SET_REF_JSPROP(prop,mjss,cls,type) \
					if(JSVAL_IS_NULL(*vp)){ \
						prop=NULL; \
					}else if(JSVAL_IS_OBJECT(*vp)){ \
						JSObject* jso=JSVAL_TO_OBJECT(*vp); \
						if(cls::IsInstance(cx, jso)){ \
							prop=(cls*)JS_GetPrivate(cx,jso); \
						}else{ \
							JS_ReportError(cx, "Parameter is not a composition/3D-object"); \
							return JS_FALSE; \
						} \
					}else if(JSVAL_IS_STRING(*vp)){ \
						JSString* jss=JSVAL_TO_STRING(*vp); \
						if(priv->mpParent == NULL){  \
							mjss=jss; \
							SAVE_STR_FROM_GC(cx,obj,mjss); \
							return JS_TRUE; \
						} \
						mlRMMLElement* pMLEl=priv->FindElemByName2(jss); \
						if(pMLEl==NULL){ \
							JS_ReportError(cx, "composition/3D-object '%s' for '%s' not found",JS_GetStringBytes(jss),cLPCSTR(priv->GetName())); \
							return JS_FALSE; \
						} \
						if(pMLEl->mRMMLType != type){ \
							JS_ReportError(cx, "Named object is not a composition/3D-object"); \
							return JS_FALSE; \
						} \
						prop=(cls*)pMLEl; \
					}

///// JavaScript Set Property Wrapper
JSBool mlRMMLCloud::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLCloud);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
//		SET_PROTO_PROP(mlRMMLVisible);
		default:
			switch(iID){
				case JSPROP_visible:
					ML_JSVAL_TO_BOOL(priv->visible,*vp);
					priv->VisibleChanged();
					break;
				case JSPROP_inverse:
					ML_JSVAL_TO_BOOL(priv->mbInverse,*vp);
					break;
				case JSPROP_content:{
					mlRMMLComposition* pPrevContent = priv->content;
					SET_REF_JSPROP(priv->content, priv->mjssContent, mlRMMLComposition, MLMT_COMPOSITION);
					if(pPrevContent != NULL){
						pPrevContent->GetVisible()->RemoveVListener(priv);
					}
					if(priv->content != NULL){
						priv->content->GetVisible()->SetVListener(priv);
					}
					priv->ContentChanged();
					}break;
				case JSPROP_footnote:
					priv->footnote = JS_ValueToString(cx, *vp);
					priv->FootnoteChanged();
					break;
				case JSPROP_marginX:
					JS_ValueToInt32(cx, *vp, (int32*)&priv->marginX);
					priv->MarginXChanged();
					break;
				case JSPROP_marginY:
					JS_ValueToInt32(cx, *vp, (int32*)&priv->marginY);
					priv->MarginYChanged();
					break;
				case JSPROP_align:
					priv->align = JS_ValueToString(cx, *vp);
					priv->AlignChanged();
					break;
				case JSPROP_maxVisibleDistance:
					ML_JSVAL_TO_DOUBLE(priv->maxVisibleDistance,*vp);
					priv->MaxVisibleDistanceChanged();
					break;
				case JSPROP_viewVec:
//					priv->CreateJSPos();
					if(JSVAL_IS_OBJECT(*vp)){
						JSObject* jso=JSVAL_TO_OBJECT(*vp);
						if(mlPosition3D::IsInstance(cx, jso)){
							mlPosition3D* pSrcPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
							priv->viewVec = pSrcPos->GetPos();
						}
					}
					priv->ViewVecChanged(priv->viewVec);
					break;
				case JSPROP_moveOutOfScreen:
					ML_JSVAL_TO_BOOL(priv->moveOutOfScreen,*vp);
					priv->MoveOutOfScreenChanged();
					break;
				case JSPROP_allowOverlap:
					ML_JSVAL_TO_BOOL(priv->allowOverlap,*vp);
					priv->AllowOverlapChanged();
					break;
				case JSPROP_sortOnDepth:
					ML_JSVAL_TO_BOOL(priv->sortOnDepth,*vp);
					priv->SortOnDepthChanged();
					break;
				case JSPROP_position:
					priv->CreateJSPos();
					if(JSVAL_IS_STRING(*vp)){
						JSString* jssVal=JSVAL_TO_STRING(*vp);
						mlRMML3DObject::ParsePos(wr_JS_GetStringChars(jssVal),&(priv->pos));
					}else if(JSVAL_IS_OBJECT(*vp)){
						JSObject* jso=JSVAL_TO_OBJECT(*vp);
						if(mlPosition3D::IsInstance(cx, jso)){
							mlPosition3D* pSrcPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
							priv->pos = pSrcPos->GetPos();
						}
					}
					priv->PosChanged(priv->pos);
					break;
				case JSPROP_attachedTo:
					if(*vp == JSVAL_NULL || *vp == JSVAL_VOID){
						priv->attachedTo = NULL;
						priv->mp3DOattachedTo = NULL;
						priv->AttachedToChanged(priv->mp3DOattachedTo);
					}else if(JSVAL_IS_REAL_OBJECT(*vp)){
						JSObject* jso = JSVAL_TO_OBJECT(*vp);
						mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx,jso);
						if(pMLEl != NULL && pMLEl->Get3DObject() != NULL){
							priv->attachedTo = NULL;
							priv->mp3DOattachedTo = pMLEl;
							priv->AttachedToChanged(priv->mp3DOattachedTo);
						}else{
							JS_ReportError(cx, "Parameter is not 3D-object");
							return JS_FALSE;
						}
					}else if(JSVAL_IS_STRING(*vp)){
						priv->SetAttachedTo(JSVAL_TO_STRING(*vp));
						priv->AttachedToChanged(priv->mp3DOattachedTo);
					}
					break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLCloud::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLCloud);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
//		GET_PROTO_PROP(mlRMMLVisible);
		default:
			switch(iID){
				case JSPROP_visible:
					*vp=BOOLEAN_TO_JSVAL(priv->visible);
					break;
				case JSPROP_inverse:
					*vp=BOOLEAN_TO_JSVAL(priv->mbInverse);
					break;
				case JSPROP_maxVisibleDistance:
					mlJS_NewDoubleValue( cx, priv->maxVisibleDistance, vp);
					break;
				case JSPROP_content:
					if(priv->content == NULL){
						if(priv->mjssContent == NULL) *vp = JSVAL_NULL;
						else *vp = STRING_TO_JSVAL(priv->mjssContent);
					}else *vp = OBJECT_TO_JSVAL(priv->content->mjso);
					break;
				case JSPROP_footnote:
					if(priv->footnote == NULL) *vp = JSVAL_NULL;
					else *vp = STRING_TO_JSVAL(priv->footnote);
					break;
				case JSPROP_marginX:
					*vp = INT_TO_JSVAL(priv->marginX);
					break;
				case JSPROP_marginY:
					*vp = INT_TO_JSVAL(priv->marginY);
					break;
				case JSPROP_align:
					if(priv->align == NULL) *vp = JSVAL_NULL;
					else *vp = STRING_TO_JSVAL(priv->align);
					break;
				case JSPROP_moveOutOfScreen:
					*vp = BOOLEAN_TO_JSVAL(priv->moveOutOfScreen);
					break;
				case JSPROP_allowOverlap:
					*vp = BOOLEAN_TO_JSVAL(priv->allowOverlap);
					break;
				case JSPROP_sortOnDepth:
					*vp = BOOLEAN_TO_JSVAL(priv->sortOnDepth);
					break;
				case JSPROP_position:
					priv->CreateJSPos();
					*vp = OBJECT_TO_JSVAL(priv->mpJSPos->mjsoPos);
					break;
				case JSPROP_attachedTo:
					if(priv->mp3DOattachedTo != NULL){
						*vp = OBJECT_TO_JSVAL(priv->mp3DOattachedTo->mjso);
					}else if(priv->attachedTo != NULL){
						*vp = STRING_TO_JSVAL(priv->attachedTo);
					}
					break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLCloud::CreateMedia(omsContext* amcx){
	mbMLElsCreated=true;
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLCloud::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	ContentChanged();
	return ML_OK;
}

mlresult mlRMMLCloud::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded.
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLCloud::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLCloud* pNewEL=(mlRMMLCloud*)GET_RMMLEL(mcx, mlRMMLCloud::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// реализация mlICloud
mlMedia* mlRMMLCloud::GetText(){
	mlRMMLElement* pMLElNickname = (mlRMMLElement*)GetNicknameText();
	mlRMMLElement* pMLElText = GetChild(MLMT_TEXT);
	if(pMLElNickname != NULL && pMLElText == pMLElNickname){
		pMLElText = GetChild(MLMT_TEXT,1);
	}
	return (mlMedia*)pMLElText;
}

mlMedia* mlRMMLCloud::GetNicknameText(){
	return (mlMedia*)GetChild(L"nickname");
}

mlMedia* mlRMMLCloud::GetContent(){
	return (mlMedia*)content;
}

mlMedia* mlRMMLCloud::GetFootnote(){
	if(footnote == NULL || content == NULL)
		return NULL;
	return (mlMedia*)content->GetChild(wr_JS_GetStringChars(footnote));
}

const wchar_t* mlRMMLCloud::GetAlign(){
	if(align == NULL) return NULL;
	return (const wchar_t*) wr_JS_GetStringChars(align);
}

bool mlRMMLCloud::GetMoveOutOfScreen(){
	return moveOutOfScreen;
}

int mlRMMLCloud::GetMarginX(){
	return marginX;
}

int mlRMMLCloud::GetMarginY(){
	return marginY;
}

double mlRMMLCloud::GetMaxVisibleDistance()
{
	return maxVisibleDistance;
}

bool mlRMMLCloud::GetAllowOverlap(){
	return allowOverlap;
}

bool mlRMMLCloud::GetSortOnDepth(){
	return sortOnDepth;
}

mlRMMLCloud::mlJS3DPos::mlJS3DPos(mlRMMLCloud* apCloud){
	//	ML_ASSERTION(mcx, ap3DObject!=NULL,"mlRMML3DObject::mlJS3DPos::mlJS3DPos");
	mpCloud = apCloud;
	JSContext* cx = mpCloud->mcx;
	JSObject* obj = mpCloud->mjso;
	mjsoPos = mlPosition3D::newJSObject(cx);
	SAVE_FROM_GC(cx,obj,mjsoPos);
	mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(cx,mjsoPos);
	pPos3D->Set3DPosRef(this);
}

void mlRMMLCloud::CreateJSPos(){
	if(!mpJSPos)
		MP_NEW_V( mpJSPos, mlJS3DPos, this);

	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;
	moICloud* pmoCloud = pMO->GetICloud();

	if(pmoCloud == NULL){ ML_INIT_3DPOSITION(pos);}
	else 	mpJSPos->SetPos(pmoCloud->GetPos());
}

// аргументы:
// null (или без аргументов) - деаттачить облачко
// [obj | string], [x, y, z | pos3D] - привязать к объекту (координаты относительно объекта)
// x, y, z | pos3D - привязать к абсолютным координатам
JSBool mlRMMLCloud::JSFUNC_attachTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	//CHECK_DONT_IMPL;
	mlRMMLCloud* pThis = (mlRMMLCloud*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_TRUE;
	if(argc == 0){
		pThis->attachedTo = NULL;
		pThis->mp3DOattachedTo = NULL;
		ML_INIT_3DPOSITION((pThis->pos));
		pThis->AttachedToChanged(pThis->mp3DOattachedTo);
		return JS_TRUE;
	}
	ml3DPosition pos3D; ML_INIT_3DPOSITION(pos3D);
	int iXYZ = 0; // 0 - x не определен еще, 1 - y, 2 - z, 3 - все три координаты заданы
	mlRMMLElement* p3DOattachTo = NULL;
	int iArg = 0;
	for(iArg = 0; iArg < (int)argc; iArg++){
		// если все определено, а есть еще аргумент, то это не правильно
		if(iXYZ == 3 && p3DOattachTo != NULL)
			break;
		jsval vArg = argv[iArg];
		if(JSVAL_IS_STRING(vArg)){
			JSString* jssObjName = JSVAL_TO_STRING(vArg);
			mlRMMLElement* pMLEl = pThis->FindElemByName2(jssObjName);
			if(pMLEl->Get3DObject() == NULL){
				JS_ReportError(cx, "Argument object is not 3D-object instance.");
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}
			p3DOattachTo = pMLEl;
		}else if(JSVAL_IS_REAL_OBJECT(vArg)){
			JSObject* jsoObj = JSVAL_TO_OBJECT(vArg);
			if(mlPosition3D::IsInstance(cx, jsoObj)){
				if(iXYZ == 3)
					break;
				mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, jsoObj);
				pos3D = pPos->GetPos();
				iXYZ = 3;
			}else if(JSO_IS_MLEL(cx, jsoObj)){
				if(p3DOattachTo != NULL)
					break;
				mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, jsoObj);
				if(pMLEl->Get3DObject() == NULL){
					JS_ReportError(cx, "Argument object is not 3D-object instance.");
					*rval = JSVAL_FALSE;
					return JS_TRUE;
				}
				p3DOattachTo = pMLEl;
			}else
				break;
		}else if(JSVAL_IS_NUMBER(vArg)){
			if(iXYZ == 3)
				break;
			double dNum = 0;
			if(JS_ValueToNumber(cx, vArg, &dNum) == JS_FALSE){
				break;
			}
			if(iXYZ == 0){
				pos3D.x = dNum;
			}else if(iXYZ == 1){
				pos3D.y = dNum;
			}else if(iXYZ == 2){
				pos3D.z = dNum;
			}
			iXYZ++;
		}else{
			break;
		}
	}
	if(iArg < (int)argc){
		JS_ReportError(cx, "Argument (%d) is invalid for attachTo-method", iArg+1);
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	if(p3DOattachTo == NULL && iXYZ < 3){
		JS_ReportError(cx, "Arguments are invalid for attachTo-method");
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	if(p3DOattachTo != NULL){
		pThis->attachedTo = NULL;
		pThis->mp3DOattachedTo = p3DOattachTo;
	}
	if(iXYZ == 3)
		pThis->pos = pos3D;
	pThis->AttachedToChanged(pThis->mp3DOattachedTo);
	return JS_TRUE;
}

moICloud* mlRMMLCloud::GetmoICloud(){
	if(PMO_IS_NULL(mpMO)) return NULL;
	return mpMO->GetICloud();
}

void mlRMMLCloud::ContentChanged()
{
	if(PMO_IS_NULL(mpMO)) return;
	UpdateRegistration();
	mpMO->GetICloud()->ContentChanged();
}

void mlRMMLCloud::MarginXChanged()
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetICloud()->MarginXChanged();
}

void mlRMMLCloud::MarginYChanged()
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetICloud()->MarginYChanged();
}

ml3DPosition mlRMMLCloud::GetPos(){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud)
		return pmoICloud->GetPos(); 
	CreateJSPos();
	return pos;
}

void mlRMMLCloud::SetPos(ml3DPosition &aPos){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud){
		pmoICloud->PosChanged(aPos);
		return;
	}
	CreateJSPos();
	pos = aPos;
}

void mlRMMLCloud::PosChanged(ml3DPosition &pos){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->PosChanged(pos); 
}

void mlRMMLCloud::SetAttachedTo(JSString* ajssName){
	attachedTo = ajssName;
	mp3DOattachedTo = NULL;
	if(!mbMLElsCreated) return;
	mp3DOattachedTo = NULL;
	if(ajssName == NULL) return;
	wchar_t* pwcName = wr_JS_GetStringChars(attachedTo);
	if(mpParent == NULL) return;
	mlRMMLElement* pMLEl = mpParent->FindElemByName2(pwcName);
	if(pMLEl->GetI3DObject() == NULL) return;
	mp3DOattachedTo = pMLEl;
}

mlMedia* mlRMMLCloud::GetAttachedTo(){ 
	if(mp3DOattachedTo == NULL)
		SetAttachedTo(attachedTo); 
	return (mlMedia*)mp3DOattachedTo; 
}

void mlRMMLCloud::SortOnDepthChanged(){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->SortOnDepthChanged(); 
}

void mlRMMLCloud::MaxVisibleDistanceChanged(){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->MaxVisibleDistanceChanged(); 
}

void mlRMMLCloud::ViewVecChanged(ml3DPosition &viewVec){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->ViewVecChanged(viewVec); 
}

void mlRMMLCloud::AllowOverlapChanged(){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->AllowOverlapChanged(); 
}

void mlRMMLCloud::MoveOutOfScreenChanged(){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->MoveOutOfScreenChanged(); 
}

void mlRMMLCloud::AttachedToChanged(mlMedia* pmlMedia){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->AttachedToChanged(pmlMedia); 
}

void mlRMMLCloud::AlignChanged(){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->AlignChanged(); 
}

void mlRMMLCloud::FootnoteChanged(){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->FootnoteChanged(); 
}

void mlRMMLCloud::VisibleChanged(){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->VisibleChanged(); 
}

// изменилось визуальное представление видимого элемента
void mlRMMLCloud::VisualRepresentationChanged(mlMedia* map){
	ContentChanged();
}

// видимый элемент уничтожился и больше от него уведомлений не будет
void mlRMMLCloud::VisibleDestroyed(mlMedia* map){
	content = NULL;
	ContentChanged();
}

void mlRMMLCloud::SetSpecParameters(int adeltaNoChangePos, int ashiftAtAvatarPosZ){
	moICloud* pmoICloud = GetmoICloud(); 
	if(pmoICloud) pmoICloud->SetSpecParameters(adeltaNoChangePos, ashiftAtAvatarPosZ); 
}

JSBool mlRMMLCloud::JSFUNC_setSpecParameters(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLCloud* pThis = (mlRMMLCloud*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_TRUE;
	if(argc < 1 || !JSVAL_IS_INT(argv[0]) || argc > 2){
		JS_ReportError(cx, "setCloudPosition method must get two int arguments");
		return JS_TRUE;
	}
	if(!JSVAL_IS_INT(argv[0])){
		JS_ReportError(cx, "First argument of query method must be a int");
		return JS_TRUE;
	}
	int deltaNoChangePos = JSVAL_TO_INT(argv[0]);
	
	if(!JSVAL_IS_INT(argv[1])){
		JS_ReportError(cx, "Second argument of query method must be a int");
		return JS_TRUE;
	}
	int shiftAtAvatarPosZ = JSVAL_TO_INT(argv[1]);
	pThis->SetSpecParameters(deltaNoChangePos, shiftAtAvatarPosZ);
	return JS_TRUE;
}

// Получить 3D-сцену, которая является одним из родителей cloud-а
mlRMMLScene3D* mlRMMLCloud::Get3DScene(){
	mlRMMLElement* pParent = mpParent;
	for(; pParent != NULL; pParent = pParent->mpParent){
		if(pParent->mRMMLType == MLMT_SCENE3D)
			return (mlRMMLScene3D*)pParent;
	}
	return NULL;
}

// Возвращает true, если и сам cloud и его контент являеются видимыми
bool mlRMMLCloud::IsVisible(){
	return (visible && content != NULL && content->GetAbsVisible());
}

// Зарегистрировать/разрегистрировать cloud в 3D-сцене, если он сам и его контент являются видимыми/невидимым
void mlRMMLCloud::UpdateRegistration(){
	if(mpScene3DRegistered == NULL){
		if(IsVisible()){
			mlRMMLScene3D* pScene3D = Get3DScene();
			if(pScene3D != NULL && pScene3D->RegisterVisibleCloud(this))
				mpScene3DRegistered = pScene3D;
		}
	}else{
		if(!IsVisible()){
			Unregister();
		}
	}
}

// Разрегистрировать cloud из 3D-сцены, если он там был зарегистрирован
void mlRMMLCloud::Unregister(){
	if(mpScene3DRegistered == NULL)
		return;
	mpScene3DRegistered->UnregisterCloud(this);
	mpScene3DRegistered = NULL;
}

}
