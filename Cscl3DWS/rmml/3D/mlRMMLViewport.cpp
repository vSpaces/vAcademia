
#include "mlRMML.h"
#include "mlSPParser.h"

namespace rmml {

mlRMMLViewport::mlRMMLViewport(void):
	MP_VECTOR_INIT(mvInclExclOut)
{
	mType=mRMMLType=MLMT_VIEWPORT;
	_camera=NULL;
	mpCamera=NULL;
	show_hints=false;
	mbExclude = true;
	mjsoInclExclArr = NULL;
	mlRMMLElement::mbBtnEvListenersIsSet=true;
//	mlRMMLPButton::enabled=true;
	soundVolume = 1.0;
	traceObjectOnMouseMove = false;
	mbProbablyHandlerFuncIsSet = true;
}

mlRMMLViewport::~mlRMMLViewport(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLViewport::~mlRMMLViewport");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLViewport::_JSPropertySpec[] = {
	JSPROP_RW(_camera)
	JSPROPA_RW(camera, _camera)
	JSPROP_RW(show_hints)
	JSPROP_RW(include)
	JSPROP_RW(exclude)
	JSPROP_RW(soundVolume)	// 0.0 .. 1.0
	JSPROP_RW(traceObjectOnMouseMove)	// проверять ли какой объект находится под курсором при перемещении мыши
										// (по умолчанию "не проверять" в целях повышения производительности)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLViewport::_JSFunctionSpec[] = {
	JSFUNC(setLODScale,1)
	JSFUNC(getObjectIn,2)
	JSFUNC(getProjection,1)
	JSFUNC(getBounds,2)
	JSFUNC(getRay,2)
	JSFUNC(getIntersectionWithPlane,4) // получить точку пересечения луча с экрана с плоскостью, заданной точкой и нормалью
	JSFUNC(getProjectionToLand,1) // получить точку проекции 3D-точки на землю с учетом гравитации
	JSFUNC(getProjectionToObjects,1) // получить точку проекции 3D-точки на землю с учетом объектов
	JSFUNC(getCloudIn,2)	// получить по 2D-координатам cloud и видимый RMML-элемент в нем
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLViewport::_EventSpec[] = {
	MLEVENT(onHintShown)
	MLEVENT(onShowHint)
	MLEVENT(onHideHint)
	{0, 0, 0}
};

MLJSCLASS_IMPL_BEGIN(Viewport,mlRMMLViewport,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLViewport)

mlRMMLElement* mlRMMLViewport::Resolve3DObject(const wchar_t* apwcRef){
	if(apwcRef == NULL || *apwcRef == L'\0')
		return NULL;
	if(mpCamera == NULL)
		return NULL;
	mlRMMLElement* pScene3D = (mlRMMLElement*)mpCamera->GetScene3D();
	if(pScene3D == NULL)
		return NULL;
	return pScene3D->FindElemByName(apwcRef);
}

void mlRMMLViewport::ResolveInclExcl(){
	if(mvInclExcl.empty()) return;
	if(PMO_IS_NULL(mpMO)) return;
	mjsoInclExclArr = JS_NewArrayObject(mcx, 0, NULL);
	mlStringVector::iterator vi;
	for(vi = mvInclExcl.begin(); vi != mvInclExcl.end(); vi++){
		mlString* ps = *vi;
		ML_ASSERTION(mcx, ps != NULL, "mlRMMLViewport::ResolveInclExcl()")
		mlRMMLElement* pMLEl = Resolve3DObject(ps->c_str());
		unsigned long length=0;
		JS_GetArrayLength(mcx, mjsoInclExclArr, &length);
		jsval v;
		if(pMLEl != NULL){
			v = OBJECT_TO_JSVAL(pMLEl->mjso);
		}else{
			v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx, ps->c_str()));
			mlTraceWarning(mcx, "Element \'%S\' to %s not found\n", ps->c_str(), mbExclude?"exclude":"include");
		}
		JS_SetElement(mcx, mjsoInclExclArr, length, &v);
	}
}

JSBool mlRMMLViewport::ParseInclExcl(wchar_t* apwcValue, bool abExclude){
	if(apwcValue == NULL) return JS_FALSE;
	mvInclExcl.clear();
	const wchar_t* pwcValue = apwcValue; 
	bool bErr = false;
	for(;;){
		pwcValue = SkipSpaces(pwcValue);
		if(*pwcValue == L'\0') break;
		const wchar_t* pwcEnd = GetNextDelimPos(pwcValue);
		if(pwcEnd == pwcValue){	bErr = true; break;	}
		mlString sRef(pwcValue, (size_t)(pwcEnd-pwcValue));
		mvInclExcl.push_back(sRef);
		pwcValue = pwcEnd;
	}
	if(bErr){
		JS_ReportError(mcx, "Incorrect %s-tag value", abExclude?"exclude":"include");
		return JS_FALSE;
	}
	ResolveInclExcl();
	return JS_TRUE;
}

JSBool mlRMMLViewport::SetInclExclProp(jsval ajsv, bool abExclude){
//	JSObject* jsoOldInclExclArr = mjsoInclExclArr;
	if(JSVAL_IS_NULL(ajsv) || JSVAL_IS_VOID(ajsv)){
		mbExclude = abExclude;
		mjsoInclExclArr = NULL;
	}else if(JSVAL_IS_STRING(ajsv)){
		mbExclude = abExclude;
		return ParseInclExcl(wr_JS_GetStringChars(JSVAL_TO_STRING(ajsv)), abExclude);
	}else if(JSVAL_IS_OBJECT(ajsv) && JS_IsArrayObject(mcx, JSVAL_TO_OBJECT(ajsv))){
		mbExclude = abExclude;
		mjsoInclExclArr = JSVAL_TO_OBJECT(ajsv);
		ResolveInclExcl();
	}else{
		JS_ReportError(mcx, "Viewport include/exclude value must be a string or an array");
	}
//	if(mjsoInclExclArr != jsoOldInclExclArr)
		InclExclChanged();
	return JS_TRUE;
}

///// JavaScript Set Property Wrapper
JSBool mlRMMLViewport::JSSetProperty(JSContext *cx,	JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLViewport);
		SET_PROTO_PROP(mlRMMLElement);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLPButton);
		default:{
			switch(iID){
			case JSPROP__camera:
				if(JSVAL_IS_NULL(*vp)){
					bool bChanged=false;
					if(priv->_camera!=NULL || priv->mpCamera!=NULL) bChanged=true;
					priv->_camera=NULL;
					priv->mpCamera=NULL;
					if(bChanged) priv->CameraChanged();
				}else if(JSVAL_IS_OBJECT(*vp)){
					JSObject* jso=JSVAL_TO_OBJECT(*vp);
					if(mlRMMLCamera::IsInstance(cx, jso)){
						mlRMMLCamera* pOldCamera=priv->mpCamera;
						priv->mpCamera=(mlRMMLCamera*)JS_GetPrivate(cx,jso);
						if(priv->mpCamera!=pOldCamera)
							priv->CameraChanged();
					}else{
						JS_ReportError(cx, "Parameter is not a camera-object");
						return JS_FALSE; 
					}
				}else if(JSVAL_IS_STRING(*vp)){
					priv->_camera=JSVAL_TO_STRING(*vp);
					// если объект уже создан
					if(IS_READY(priv)){
						// то пытаемся найти объект камеры
						JSString* jssCameraOld=priv->_camera;
						mlRMMLCamera* pCameraOld=priv->mpCamera;
						wchar_t* pwcName=wr_JS_GetStringChars(priv->_camera);
						const wchar_t* pwcDot=findDot(pwcName);
						if(pwcDot==NULL){
							priv->_camera=jssCameraOld;
							JS_ReportError(cx, "Camera-object not found");
							return JS_FALSE;
						}
						mlRMMLElement* pMLEl=NULL;
						if(priv->mpParent!=NULL){
							pMLEl=priv->mpParent->FindElemByName(pwcName);
						}
						if(pMLEl==NULL){
							priv->mpCamera=NULL;
							if(priv->mpCamera!=pCameraOld)
								priv->CameraChanged();
						}else{
							if(pMLEl->mRMMLType!=MLMT_CAMERA){
								priv->_camera=jssCameraOld;
								JS_ReportError(cx, "Named object is not a camera");
								return JS_FALSE;
							}
							priv->mpCamera=(mlRMMLCamera*)pMLEl;
							FREE_STR_FOR_GC(cx,obj,priv->_camera);
							priv->_camera=NULL;
							if(priv->mpCamera!=pCameraOld)
								priv->CameraChanged();
						}
					}
				}
				break;
			case JSPROP_show_hints:
				ML_JSVAL_TO_BOOL(priv->show_hints,*vp);
				break;
			case JSPROP_include:{
				priv->SetInclExclProp(*vp, false);
				}break;
			case JSPROP_exclude:{
				priv->SetInclExclProp(*vp, true);
				}break;
			case JSPROP_soundVolume:{
				jsdouble dVal = 1.0;
				if(!JS_ValueToNumber(cx, *vp, &dVal)){
					JS_ReportError(cx, "soundVolume must be a number");
				}else{
					if(dVal < 0.0 || dVal > 1.0){
						JS_ReportWarning(cx, "soundVolume must be in range from 0 to 1");
					}
					if(dVal < 0.0) dVal = 0;
					if(dVal > 1.0) dVal = 1;
					priv->soundVolume = dVal;
				}
				}break;
			case JSPROP_traceObjectOnMouseMove:
				ML_JSVAL_TO_BOOL(priv->traceObjectOnMouseMove,*vp);
				priv->TraceObjectOnMouseMoveChanged();
				break;
			}
		}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLViewport::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLViewport);
		GET_PROTO_PROP(mlRMMLElement);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
			case JSPROP__camera:
				*vp=JSVAL_NULL;
				if(priv->_camera!=NULL){
					if(priv->_camera!=NULL && priv->mpCamera==NULL){
						jsval v=STRING_TO_JSVAL(priv->_camera); 
						JS_SetProperty(cx,obj,MLELPN_CAMERA,&v); 
					}
					if(priv->mpCamera!=NULL){
						*vp=OBJECT_TO_JSVAL(priv->mpCamera->mjso);
					}else{
						*vp=STRING_TO_JSVAL(priv->_camera);
					}
				}else{
					if(priv->mpCamera!=NULL){
						*vp=OBJECT_TO_JSVAL(priv->mpCamera->mjso);
//						mlString wsCamera=priv->GetElemRefName(priv->mpCamera);
//						priv->_camera=JS_NewUCStringCopyZ(cx,wsCamera.c_str());
					}else{
						*vp=JSVAL_NULL;
					}
				}
				break;
			case JSPROP_show_hints:
				*vp=BOOLEAN_TO_JSVAL(priv->show_hints);
				break;
			case JSPROP_include:{
				*vp = JSVAL_NULL;
				priv->ResolveInclExcl();
				if(!(priv->mbExclude) && priv->mjsoInclExclArr != NULL)
					*vp = OBJECT_TO_JSVAL(priv->mjsoInclExclArr);
				}break;
			case JSPROP_exclude:{
				*vp = JSVAL_NULL;
				priv->ResolveInclExcl();
				if(priv->mbExclude && priv->mjsoInclExclArr != NULL)
					*vp = OBJECT_TO_JSVAL(priv->mjsoInclExclArr);
				}break;
			case JSPROP_soundVolume:{
				mlJS_NewDoubleValue(cx, priv->soundVolume, vp);
				}break;
			case JSPROP_traceObjectOnMouseMove:
				*vp = BOOLEAN_TO_JSVAL(priv->traceObjectOnMouseMove);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

#define MLVP_SET_REF(jss,var,MLELPN) \
	if(jss!=NULL && var==NULL){ \
		jsval v=STRING_TO_JSVAL(jss); \
		JS_SetProperty(mcx,mjso,MLELPN,&v); \
	}

// Реализация mlRMMLElement
mlresult mlRMMLViewport::CreateMedia(omsContext* amcx){
	// set camera
	MLVP_SET_REF(_camera,mpCamera,MLELPN_CAMERA);
	// затычка для бага, когда устанавливается свойство "_camera", а берется значение свойства "camera"
	// при этом появляется свойство у прототипа класса и, таким образом, остается ссылка в классе на 
	// объект камеры и сцена не перегружается
	jsval vCam;
	JS_GetProperty(mcx, mjso, MLELPN_CAMERA, &vCam);
	JS_SetProperty(mcx, mjso, "camera", &vCam);
	//
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLViewport::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	ResolveInclExcl();
	return ML_OK;
}

mlRMMLElement* mlRMMLViewport::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLViewport* pNewEL=(mlRMMLViewport*)GET_RMMLEL(mcx, mlRMMLViewport::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

mlRMMLElement* pWas=NULL;

mlRMMLElement* mlRMMLViewport::GetButtonUnderMouse(mlPoint &aMouseXY, mlButtonUnderMouse &aBUM){ // , mlRMMLVisible** appVisible, mlRMMLElement** app3DObject, ml3DPosition** appXYZ, mlRMMLElement** appTextureObject, mlPoint* apLocalXY){ 
//	if(!mbBtnEvListenersIsSet) return NULL;
	mlRMMLPButton* pViewportButton = GetButton();
//	if(pViewportButton && !pViewportButton->GetAbsEnabled())
//		return NULL;	// Tandy 11.09.2010
	mlRMMLVisible* pVisible = GetVisible();
	ML_ASSERTION(mcx, pVisible!=NULL,"mlRMMLViewport::GetButton2UnderMouse");
	if(!(pVisible->mbVisible)) return NULL;
	if(mpCamera == NULL) return NULL;
//	mlPoint AbsXY=pVisible->GetAbsXY();
	mlPoint XY=aMouseXY;
	XY.x-=mXY.x;
	XY.y-=mXY.y;
//	XY.x-=AbsXY.x;
//	XY.y-=AbsXY.y;
	if(XY.x<0 || XY.y<0) return NULL;
	mlSize size=pVisible->GetSize();
	if(size.width!=0){
		if(XY.x > size.width || XY.y > size.height) return NULL;
	}
	moMedia* pMO=GetMO();
	if (PMO_IS_NULL(pMO))
		return NULL;
	// Сначала посмотрим, нет ли под мышкой cloud-а с активной кнопкой?
	if(mpCamera->GetParent() && mpCamera->GetParent()->mRMMLType == MLMT_SCENE3D){
		mlRMMLScene3D* pScene3D = (mlRMMLScene3D*)mpCamera->GetParent();
		mlRMMLCloud* pCloud = NULL;
		mlButtonUnderMouse oBUM;
		mlRMMLElement* pCloudButton = pScene3D->GetCloudButtonUnderMouse(XY, oBUM, pCloud);
		if(pCloudButton != NULL)
			return pCloudButton;
	}
	//
	moIViewport* pIViewport=pMO->GetIViewport();
	ML_ASSERTION(mcx, pIViewport!=NULL,"mlRMMLViewport::GetButtonUnderMouse");
	std::vector<mlRMML3DObject*> vSkiped3DObjects;
	mlRMMLElement* pMLEl = NULL;
	for(;;){
		ml3DPosition pos3D;
		int iType = 0;
		ml3DPosition posTexture;
		mlMedia** paCheckObjects = NULL;
		mlMedia* pTextureMedia = NULL;
		mlPoint pntTextureMediaXY; pntTextureMediaXY.x = -1; pntTextureMediaXY.y = -1; 
		mlOutString sLocationID;
		mlOutString sLocationParams;
		ml3DLocation oLocation;
		mlMedia* pMedia=pIViewport->GetObjectUnderMouse(XY, false, &pos3D, &oLocation, &iType, &posTexture, paCheckObjects, &pTextureMedia, &pntTextureMediaXY);
		// * Когда идет получение 3D-объекта под мышкой, то если текстура оказывается 2D-композицией
		// * вызываем у нее получение объекта под мышкой уже в самой композиции
		// * если 2D-объекта под мышкой не находится, то объект с текстурой-композицией скрывается и запускается получение 3D-объекта заново.
		// ?? 
		if(pMedia == NULL || pMedia == (mlMedia*)0xffffffff){
			pMLEl = NULL;
			break;
		}
		aBUM.Reset();
		pMLEl = (mlRMMLElement*)pMedia;
		if(pTextureMedia != NULL){
			mlRMMLElement* pTextureMLEl = (mlRMMLElement*)pTextureMedia;
			mlRMMLElement* pTextureMLElBut = pTextureMLEl;
			if(pTextureMLEl->mType == MLMT_BROWSER){
				// пока будем выдавать 
				// ??
			// }else if(pTextureMLEl->mType == MLMT_COMPOSITION){
				// надо вызвать GetButtonUnderMouse у композиции 
				// ??
			}else if(pTextureMLEl->mType == MLMT_IMAGE || pTextureMLEl->mType == MLMT_COMPOSITION){
				// если ткнули на прозрачном месте
				pTextureMLEl->GetVisible()->ShowInternal();
				mlButtonUnderMouse oBUM;
				mlRMMLElement* pBut = pTextureMLEl->GetButtonUnderMouse(pntTextureMediaXY, oBUM);
				pTextureMLEl->GetVisible()->HideInternal();
				if(pBut == NULL){
					// , то пропускаем этот 3D-объект и трэйсим снова
					mlRMML3DObject* p3DObj = pMLEl->Get3DObject();
					if(p3DObj != NULL){
						p3DObj->SetUnpickable(true);
						vSkiped3DObjects.push_back(p3DObj);
						continue;
					}
				}else{
					pTextureMLElBut = pBut;
				}
			}
			aBUM.mXYZ = pos3D;
			aBUM.mp3DObject = pMLEl;
			aBUM.mpTextureObject = pTextureMLEl;
			aBUM.mLocalXY = pntTextureMediaXY;
			//if(app3DObject != NULL)
			//	*app3DObject = pMLEl;
			//if(appTextureObject != NULL)
			//	*appTextureObject = pTextureMLEl;
			//if(apLocalXY != NULL)
			//	*apLocalXY = pntTex tureMediaXY;
			pMLEl = pTextureMLElBut;
		}else{
			aBUM.mXYZ = pos3D;
			aBUM.mp3DObject = pMLEl;
			aBUM.mpTextureObject = NULL;
			aBUM.mLocalXY = pntTextureMediaXY;
		}
		break;
	//if(pMLEl != pWas){
	//if(pMLEl==NULL){
	//mlTrace("3D-Obj under but:NULL");
	//}else{
	//mlTrace("3D-Obj under but:%S",pMLEl->GetName());
	//}
	//pWas=pMLEl;
	//}
	}
	std::vector<mlRMML3DObject*>::const_iterator cvi = vSkiped3DObjects.begin();
	for(; cvi != vSkiped3DObjects.end(); cvi++){
		mlRMML3DObject* p3DObj = *cvi;
		p3DObj->SetUnpickable(false);
	}
	if(pMLEl == NULL)
		return NULL;
	mlRMMLPButton* pBut=pMLEl->GetButton();
	if(pBut && !pBut->GetAbsEnabled()) return NULL;
	// if(!pMLEl->mbBtnEvListenersIsSet) return NULL;	// Tandy 11.06.08: закомметарил, чтобы не выдавался объект 
														// под вьюпортом
	//if(appVisible != NULL)
	//	*appVisible = this->GetVisible();
	aBUM.mpVisible = this->GetVisible();
	return pMLEl;
}

mlRMMLElement* mlRMMLViewport::GetVisibleAt(const mlPoint aPnt, mlIVisible** appVisible){ 
	mlRMMLVisible* pVisible = GetVisible();
	ML_ASSERTION(mcx, pVisible!=NULL,"mlRMMLViewport::GetVisibleAt");
	if(!(pVisible->mbVisible)) return NULL;

	if(mpCamera==NULL) return NULL;

	mlPoint XY = aPnt;
	XY.x -= mXY.x;
	XY.y -= mXY.y;
	if(XY.x < 0 || XY.y < 0) return NULL;

	mlSize size = pVisible->GetSize();
	if(size.width!=0){
		if(XY.x > size.width || XY.y > size.height) return NULL;
	}

	moMedia* pMO=GetMO();
	if (PMO_IS_NULL(pMO))
		return NULL;
	moIViewport* pIViewport = pMO->GetIViewport();
	ML_ASSERTION(mcx, pIViewport!=NULL,"mlRMMLViewport::GetVisibleAt");

	mlMedia* pMedia = pIViewport->GetObjectUnderMouse(XY);
	if(pMedia==NULL) return NULL;
	if(pMedia == (mlMedia*)NO_OBJECT)
		return NULL;	// ?? прозрачный фон

	mlRMMLElement* pMLEl=(mlRMMLElement*)pMedia;
	if(appVisible != NULL)
		*appVisible = GetIVisible();
	return pMLEl;
}

#define MLVP_ON_BEFORE_PRESS_FUNC_NAME L"onBeforePress"

#define MLVP_EVENT_OBJECT_PROP_NAME "object"

bool mlRMMLViewport::CallOnBeforePress(mlButtonUnderMouse &aBUM){ //  mlRMMLElement* ap3DObject, ml3DPosition* apXYZ, mlRMMLElement* apTextureObject, mlPoint* appntTextureXY){
	mlRMML3DObject* p3DObj = (aBUM.mp3DObject!=NULL)?aBUM.mp3DObject->Get3DObject():NULL;
	bool bProbably3DObjHandlerFuncIsSet = (p3DObj != NULL)?p3DObj->ProbablyHandlerFuncIsSet():false;
	if(!mbProbablyHandlerFuncIsSet && !bProbably3DObjHandlerFuncIsSet)
		return false;
	jsval v3DObjFunc = JSVAL_NULL;
	if(p3DObj != NULL){
		v3DObjFunc = p3DObj->GetHandlerFunc(MLVP_ON_BEFORE_PRESS_FUNC_NAME);
	}
	jsval vFunc = JSVAL_NULL;
	if(wr_JS_GetUCProperty(mcx, mjso, MLVP_ON_BEFORE_PRESS_FUNC_NAME, -1, &vFunc) == JS_TRUE){
		if(JSVAL_IS_STRING(vFunc) || (JSVAL_IS_REAL_OBJECT(vFunc) && JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc)))){
		}else
			vFunc = JSVAL_NULL;
	}else 
		vFunc = JSVAL_NULL;
	// если функция onBeforePress не установлена, то mbProbablyHandlerFuncIsSet = false;
	if(vFunc == JSVAL_NULL){
		mbProbablyHandlerFuncIsSet = false;
	}
	if(vFunc == JSVAL_NULL && v3DObjFunc == JSVAL_NULL){
		return false;
	}
	bool bRetVal = false;
	// установливаем в Event 3D-объект (Event.object), на который нажали, 
	// а также объект текстуры (Event.textureObject) и координаты в текстуре в масштабах этого объекта (Event.textureX и Event.textureY)
	jsval vNullVal = JSVAL_NULL;
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), MLVP_EVENT_OBJECT_PROP_NAME, &vNullVal);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), ML_EVENT_TEXTURE_OBJECT_PROP_NAME, &vNullVal);
	if(aBUM.mp3DObject != NULL){
		jsval v = OBJECT_TO_JSVAL(aBUM.mp3DObject->mjso);
		JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), MLVP_EVENT_OBJECT_PROP_NAME, &v);
		jsdouble* jsdbl = JS_NewDouble(mcx, aBUM.mXYZ.x); 
		v = DOUBLE_TO_JSVAL(jsdbl);
		JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "x", &v);
		jsdbl = JS_NewDouble(mcx, aBUM.mXYZ.y); 
		v = DOUBLE_TO_JSVAL(jsdbl);
		JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "y", &v);
		jsdbl = JS_NewDouble(mcx, aBUM.mXYZ.z); 
		v = DOUBLE_TO_JSVAL(jsdbl);
		JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "z", &v);
		if(aBUM.mpTextureObject != NULL){
			jsval v = OBJECT_TO_JSVAL(aBUM.mpTextureObject->mjso);
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), ML_EVENT_TEXTURE_OBJECT_PROP_NAME, &v);
			v = INT_TO_JSVAL(aBUM.mLocalXY.x);
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), ML_EVENT_TEXTURE_X_PROP_NAME, &v);
			v = INT_TO_JSVAL(aBUM.mLocalXY.y);
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), ML_EVENT_TEXTURE_Y_PROP_NAME, &v);
		}
	}
	jsval v = JSVAL_VOID;
	// вызываем onBeforePress у viewport-а, если он есть
	if(vFunc != JSVAL_NULL){
		if(JSVAL_IS_STRING(vFunc)){
			JSString* jss = JSVAL_TO_STRING(vFunc);
			JS_EvaluateUCScript(mcx, mjso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
				cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]), miSrcLine, &v);
		}else{
			JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
		}
	}
	// если он вернул true, 
	if(v == JSVAL_TRUE){
		// то не пропускаем событие дальше
		bRetVal = true;
	}else{
		// вызываем onBeforePress у 3D-объекта, если он есть
		if(v3DObjFunc != JSVAL_NULL){
			if(JSVAL_IS_STRING(v3DObjFunc)){
				JSString* jss = JSVAL_TO_STRING(v3DObjFunc);
				JS_EvaluateUCScript(mcx, mjso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
					cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]), miSrcLine, &v);
			}else{
				JS_CallFunctionValue(mcx, mjso, v3DObjFunc, 0, NULL, &v);
			}
			if(v == JSVAL_TRUE)
				bRetVal = true;
		}
	}
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), MLVP_EVENT_OBJECT_PROP_NAME, &vNullVal);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), ML_EVENT_TEXTURE_OBJECT_PROP_NAME, &vNullVal);
	return bRetVal;
}

mlRMMLElement* mlRMMLViewport::GetRefPropContext(const wchar_t* apwcName){
	if(apwcName==NULL) return this;
	if(isEqual(apwcName,L"_camera") && mpParent!=NULL){
		return mpParent;
	}
	return this;
}


bool mlRMMLViewport::SetEventData(char aidEvent, mlSynchData &Data){
	if(mlRMMLElement::SetEventData(aidEvent, Data))
		return true;
	switch(aidEvent){
	case EVID_onHintShown:
		break;
	}
	return true;
}

// реализация mlIViewport
mlMedia* mlRMMLViewport::GetCamera(){
	MLVP_SET_REF(_camera,mpCamera,MLELPN_CAMERA);
	if(_camera!=NULL && mpCamera==NULL){
		mlTraceError(mcx, "Camera-object not found %S\n", GetSrcFileRef().c_str());
	}
	return mpCamera;
}

mlMedia* mlRMMLViewport::GetHint(){
	v_elems::iterator vi;
	if( !mpChildren)	return NULL;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		if(pMLEl->mRMMLType==MLMT_HINT) return pMLEl;
	}
	return NULL;
}

void mlRMMLViewport::FillInclExclOut(){
	mvInclExclOut.clear();
	if(mjsoInclExclArr == NULL) return;
	jsuint uiLen=0;
	if(!JS_GetArrayLength(mcx,mjsoInclExclArr,&uiLen))
		return;
//	jsval vLen;
//	if(!JS_GetUCProperty(mcx, mjsoInclExclArr, L"length", -1, &vLen))
//		return;
//	if(JSVAL_IS_INT(vLen))
//		uiLen = JSVAL_TO_INT(vLen);
	for(jsuint jj=0; jj<uiLen; jj++){
		jsval v;
		if(!JS_GetElement(mcx, mjsoInclExclArr, jj, &v))
			continue;
		if(!JSVAL_IS_REAL_OBJECT(v))
			continue;
		JSObject* jso = JSVAL_TO_OBJECT(v);
		if(!JSO_IS_MLEL(mcx, jso))
			continue;
		mlRMMLElement* pMLEl = GET_RMMLEL(mcx,jso);
		if(pMLEl == NULL)
			continue;
		mvInclExclOut.push_back(pMLEl);
	}
	if(!mvInclExclOut.empty())
		mvInclExclOut.push_back(NULL);
}

mlMedia** mlRMMLViewport::GetInclude(){
	if(mbExclude) return NULL;
	FillInclExclOut();
	if(mvInclExclOut.empty()) return NULL;
#if _MSC_VER >= 1600
	return mvInclExclOut.begin()._Ptr;
#elif _MSC_VER > 1200
	return mvInclExclOut.begin()._Myptr;
#else
	return mvInclExclOut.begin();
#endif
}

mlMedia** mlRMMLViewport::GetExclude(){
	if(!mbExclude) return NULL;
	FillInclExclOut();
	if(mvInclExclOut.empty()) return NULL;
#if _MSC_VER >= 1600
	return mvInclExclOut.begin()._Ptr;
#elif _MSC_VER > 1200
	return mvInclExclOut.begin()._Myptr;
#else
	return mvInclExclOut.begin();
#endif
}

double mlRMMLViewport::GetSoundVolume(){
	return soundVolume;
}

void mlRMMLViewport::onHintShown(mlMedia* apObject)
{
	jsval v = OBJECT_TO_JSVAL(((mlRMMLElement*)apObject)->mjso);
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"object",&v);
	CallListeners(EVID_onHintShown);
}

void mlRMMLViewport::onShowHint(mlMedia* apObject, mlPoint aScreenXY, const wchar_t* apwcText)
{
	jsval vObj = JSVAL_NULL;
	if(apObject != NULL)
		vObj = OBJECT_TO_JSVAL(((mlRMMLElement*)apObject)->mjso);
	jsval x = INT_TO_JSVAL(aScreenXY.x);
	jsval y = INT_TO_JSVAL(aScreenXY.y);
	jsval text = STRING_TO_JSVAL(JS_NewUCStringCopyZ(mcx, (jschar*)apwcText));
	if(GPSM(mcx)->Updating()){
		// то выполняем сразу же
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"object",&vObj);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"x",&x);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"y",&y);
		JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),"hint",&text);
		CallListeners(EVID_onShowHint);
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << false;
		// сохраняем параметры
		if(vObj != JSVAL_NULL){
			mlRMMLElement* pMLEl = (mlRMMLElement*)apObject;
			Data << true;
			GPSM(mcx)->SaveInternalPath(Data,pMLEl);
		}else
			Data << false;
		Data << aScreenXY.x;
		Data << aScreenXY.y;
		Data << apwcText;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onShowHint, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(mcx)->GetMode() == smmInitilization);
	}
}

void mlRMMLViewport::onHideHint()
{
	if(GPSM(mcx)->Updating()){
		// то выполняем сразу же
		CallListeners(EVID_onHideHint);
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << false;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onHideHint, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(mcx)->GetMode() == smmInitilization);
	}
}

// реализация moIViewport
#define MOIVIEWPORTEVENT_IMPL(M) \
void mlRMMLViewport::M(){ \
	if(PMO_IS_NULL(mpMO)) return; \
	mpMO->GetIViewport()->M(); \
}
//MOIVIEWPORTEVENT_IMPL(CameraChanged);
void mlRMMLViewport::CameraChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	ResolveInclExcl();
	mpMO->GetIViewport()->CameraChanged();
}

void mlRMMLViewport::InclExclChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIViewport()->InclExclChanged();
}

void mlRMMLViewport::SoundVolumeChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIViewport()->SoundVolumeChanged();
}

void mlRMMLViewport::TraceObjectOnMouseMoveChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIViewport()->TraceObjectOnMouseMoveChanged();
}

mlRect mlRMMLViewport::GetBounds(mlMedia* apObject){
	mlRect rcBounds; ML_INIT_RECT(rcBounds);
	if(PMO_IS_NULL(mpMO)) return rcBounds;
	return mpMO->GetIViewport()->GetBounds(apObject);
}

//

JSBool mlRMMLViewport::JSFUNC_getObjectIn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc < 2){
		JS_ReportError(cx,"getObjectIn(x,y[,array][,ground]) must get two integer arguments at least");
		return JS_FALSE;
	}
	if(!JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1])){
		JS_ReportError(cx,"getObjectIn(x,y[,array][,ground]) must get two integer arguments at least");
		return JS_TRUE;
	}
	mlPoint XY;
	XY.x=JSVAL_TO_INT(argv[0]);
	XY.y=JSVAL_TO_INT(argv[1]);
	//
	typedef mlMedia* PMedia;
	mlMedia** paCheckObjects = NULL;
	bool bGroundOnly = false;
	if(argc >= 3){
		if(JSVAL_IS_BOOLEAN(argv[2])){
			bGroundOnly = JSVAL_TO_BOOLEAN(argv[2]);
		}else{
			if(JSVAL_IS_REAL_OBJECT(argv[2])){
				JSObject* jsoCheckObjects = JSVAL_TO_OBJECT(argv[2]);
				if(JS_IsArrayObject(cx, jsoCheckObjects)){
					jsuint uiLen=0;
					if(JS_GetArrayLength(cx,jsoCheckObjects,&uiLen)){
						paCheckObjects = MP_NEW_ARR( PMedia, uiLen + 1);
						paCheckObjects[uiLen] = NULL;
						for(jsuint jj=0; jj<uiLen; jj++){
							paCheckObjects[jj] = NULL;
							jsval v;
							if(!JS_GetElement(cx, jsoCheckObjects, jj, &v))
								break;
							if(JSVAL_IS_NULL(v) || JSVAL_IS_VOID(v))
								continue;
							if(!JSVAL_IS_MLEL(cx, v)){
								JS_ReportWarning(cx,"Item (%d) of third argument of getObjectIn(x,y[,array][,ground]) method is not 3D-object", jj);
								continue;
							}
							JSObject* jso3DObject = JSVAL_TO_OBJECT(v);
							mlRMMLElement* p3DObject = GET_RMMLEL(cx, jso3DObject);
							if(!(p3DObject->mRMMLType & MLMPT_3DOBJECT)){
								JS_ReportWarning(cx,"Item (%d) of third argument of getObjectIn(x,y[,array][,ground]) method is not 3D-object", jj);
								continue;
							}
							paCheckObjects[jj] = p3DObject;
						}
					}
				}else{
					jsval v = argv[2];
					if(JSVAL_IS_MLEL(cx, v)){
						JSObject* jso3DObject = JSVAL_TO_OBJECT(v);
						mlRMMLElement* p3DObject = GET_RMMLEL(cx, jso3DObject);
						if(!(p3DObject->mRMMLType & MLMPT_3DOBJECT)){
							JS_ReportWarning(cx,"Third argument of getObjectIn(x,y[,array][,ground]) method is not 3D-object");
							return JS_TRUE;
						}
						paCheckObjects = MP_NEW_ARR( PMedia, 2);
						paCheckObjects[0] = p3DObject;
						paCheckObjects[1] = NULL;
					}else{
						JS_ReportWarning(cx,"Third argument of getObjectIn(x,y[,array][,ground]) method is not 3D-object");
						return JS_TRUE;
					}
				}
			}
			if(paCheckObjects == NULL){
				JS_ReportWarning(cx,"Third argument of getObjectIn(x,y[,array][,ground]) method must be an array of 3D-objects");
				return JS_TRUE;
			}
		}
	}
	//
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	moMedia* pMO=pMLEl->GetMO();
	if(PMO_IS_NULL(pMO)) return JS_FALSE;
	moIViewport* pIViewport=pMO->GetIViewport();
	ML_ASSERTION(cx, pIViewport!=NULL,"mlRMMLViewport::JSFUNC_getObjectIn");
	ml3DPosition pos3D;
	int iType=0;
	ml3DPosition posTexture;
	bool bGround = false;
	ml3DLocation oLocation;
	mlMedia* pMedia = pIViewport->GetObjectUnderMouse(XY, bGroundOnly, &pos3D, &oLocation, &iType, &posTexture, paCheckObjects, NULL, NULL, &bGround);
	if(paCheckObjects != NULL)
		MP_DELETE_ARR( paCheckObjects);

//	if(pMedia==NULL){
//		*rval=JSVAL_NULL;
//		return JS_TRUE;
//	}
	JSObject* jso = JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, pMLEl->mjso, jso);
	jsval vX,vY,vZ;
	mlJS_NewDoubleValue(cx,pos3D.x,&vX);
	JS_DefineProperty(cx,jso,"x",vX,0,0,0);
	mlJS_NewDoubleValue(cx,pos3D.y,&vY);
	JS_DefineProperty(cx,jso,"y",vY,0,0,0);
	mlJS_NewDoubleValue(cx,pos3D.z,&vZ);
	JS_DefineProperty(cx,jso,"z",vZ,0,0,0);
	//
	mlJS_NewDoubleValue(cx,posTexture.x,&vX);
	JS_DefineProperty(cx,jso,"tu",vX,0,0,0);
	mlJS_NewDoubleValue(cx,posTexture.y,&vY);
	JS_DefineProperty(cx,jso,"tv",vY,0,0,0);
	//
	jsval vObj = JSVAL_NULL;
	if(pMedia != NULL)
		vObj = OBJECT_TO_JSVAL(((mlRMMLElement*)pMedia)->mjso);
	JS_DefineProperty(cx, jso, "obj", vObj, 0, 0, JSPROP_ENUMERATE);
	JS_DefineProperty(cx, jso, "type", INT_TO_JSVAL(iType), 0, 0, JSPROP_ENUMERATE);
	JS_DefineProperty(cx, jso, "ground", BOOLEAN_TO_JSVAL(bGround), 0, 0, JSPROP_ENUMERATE);
	//
	if(!oLocation.ID.mstr.empty()){
		JSObject* jsoLocation = GPSM(cx)->LocationToJSObject(oLocation, false);
		jsval vLoc = OBJECT_TO_JSVAL(jsoLocation);
		JS_SetProperty(cx, jso, "location", &vLoc);
	}
	//
	*rval = OBJECT_TO_JSVAL(jso);
	FREE_FOR_GC(cx, pMLEl->mjso, jso);
	return JS_TRUE;
}

JSBool mlRMMLViewport::JSFUNC_getCloudIn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc < 2){
		JS_ReportError(cx,"getCloudIn(x,y) must get two integer arguments at least");
		return JS_FALSE;
	}
	if(!JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1])){
		JS_ReportError(cx,"getCloudIn(x,y) must get two integer arguments at least");
		return JS_TRUE;
	}
	mlPoint XY;
	XY.x=JSVAL_TO_INT(argv[0]);
	XY.y=JSVAL_TO_INT(argv[1]);

	mlRMMLViewport* pThis = (mlRMMLViewport*)JS_GetPrivate(cx,obj);
	mlRMMLScene3D* pScene3D = (mlRMMLScene3D*)pThis->mpCamera->GetParent();
	mlRMMLCloud* pCloud = NULL;
	mlButtonUnderMouse oBUM;
	mlRMMLElement* pCloudButton = pScene3D->GetCloudButtonUnderMouse(XY, oBUM, pCloud);
	if(pCloudButton == NULL)
		return JS_TRUE;
	JSObject* jso = JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, pThis->mjso, jso);
	jsval vCloud = OBJECT_TO_JSVAL(pCloud->mjso);
	JS_DefineProperty(cx, jso, "cloud", vCloud, 0, 0, JSPROP_ENUMERATE);
	jsval vObj = OBJECT_TO_JSVAL(pCloudButton->mjso);
	JS_DefineProperty(cx, jso, "obj", vObj, 0, 0, JSPROP_ENUMERATE);
	jsval vButton = BOOLEAN_TO_JSVAL(pCloudButton->mbBtnEvListenersIsSet);
	JS_DefineProperty(cx, jso, "button", vButton, 0, 0, JSPROP_ENUMERATE);
	*rval = OBJECT_TO_JSVAL(jso);
	FREE_FOR_GC(cx, pThis->mjso, jso);
	return JS_TRUE;
}

JSBool mlRMMLViewport::JSFUNC_getRay(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc < 2){
		JS_ReportError(cx,"getRay(x,y[,array]) must get two integer arguments at least");
		return JS_FALSE;
	}
	if(!JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1])){
		JS_ReportError(cx,"getRay(x,y[,array]) must get two integer arguments at least");
		return JS_TRUE;
	}
	mlPoint XY;
	XY.x=JSVAL_TO_INT(argv[0]);
	XY.y=JSVAL_TO_INT(argv[1]);

	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	moMedia* pMO=pMLEl->GetMO();
	if(PMO_IS_NULL(pMO)) return JS_FALSE;

	moIViewport* pIViewport=pMO->GetIViewport();
	ML_ASSERTION(cx, pIViewport!=NULL,"mlRMMLViewport::JSFUNC_getRay");
	ml3DPosition ro, rd;
	pIViewport->GetRay(XY, ro, rd);

	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);

	jsval vX,vY,vZ;

	mlJS_NewDoubleValue(cx,ro.x,&vX);
	JS_DefineProperty(cx,jso,"x0",vX,0,0,0);
	mlJS_NewDoubleValue(cx,ro.y,&vY);
	JS_DefineProperty(cx,jso,"y0",vY,0,0,0);
	mlJS_NewDoubleValue(cx,ro.z,&vZ);
	JS_DefineProperty(cx,jso,"z0",vZ,0,0,0);
	//
	mlJS_NewDoubleValue(cx,rd.x,&vX);
	JS_DefineProperty(cx,jso,"x",vX,0,0,0);
	mlJS_NewDoubleValue(cx,rd.y,&vY);
	JS_DefineProperty(cx,jso,"y",vY,0,0,0);
	mlJS_NewDoubleValue(cx,rd.z,&vZ);
	JS_DefineProperty(cx,jso,"z",vZ,0,0,0);
	//
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSBool mlRMMLViewport::JSFUNC_getProjection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
//	if (argc != 1) return JS_FALSE;
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	moMedia* pMO=pMLEl->GetMO();
	if(PMO_IS_NULL(pMO)) return JS_FALSE;
	moIViewport* pIViewport=pMO->GetIViewport();
	*rval=JSVAL_NULL;
	mlPoint xy; ML_INIT_POINT(xy);
	mlRect rcBounds; ML_INIT_RECT(rcBounds);
	if(argc==3 && JSVAL_IS_NUMBER(argv[0]) && JSVAL_IS_NUMBER(argv[1]) && JSVAL_IS_NUMBER(argv[2])){
		ml3DPosition pos; ML_INIT_3DPOSITION(pos);
		if( JS_ValueToNumber(cx, argv[0], &pos.x)==JS_TRUE && 
			JS_ValueToNumber(cx, argv[1], &pos.y)==JS_TRUE && 
			JS_ValueToNumber(cx, argv[2], &pos.z)==JS_TRUE){
			if(!pIViewport->GetProjection(pos, xy)){
				*rval=JSVAL_NULL;
				return JS_TRUE;
			}
		}else{
			return JS_FALSE;
		}
	}else if(argc==1 && argv[0]!=JSVAL_NULL && JSVAL_IS_OBJECT(argv[0])){
		JSObject* jso=JSVAL_TO_OBJECT(argv[0]);
		if(mlPosition3D::IsInstance(cx, jso)){
			mlPosition3D* pPod3D=(mlPosition3D*)JS_GetPrivate(cx,jso);
			ml3DPosition pos=pPod3D->GetPos();
			if(!pIViewport->GetProjection(pos, xy))
				return JS_TRUE;
		}else if(JSO_IS_MLEL(cx, jso)){
			mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,jso);
			if(pMLEl->mRMMLType & MLMPT_3DOBJECT){
				if(!pIViewport->GetProjection(pMLEl, xy, rcBounds))
					return JS_TRUE;
			}else{
				JS_ReportError(cx,"Invalid argument for getProjection-method");
				return JS_TRUE;
			}
		}else{
			JS_ReportError(cx,"Invalid argument for getProjection-method");
			return JS_TRUE;
		}
	}else{
		JS_ReportError(cx,"Invalid argument(s) for getProjection-method");
		return JS_TRUE;
	}
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	JS_DefineProperty(cx,jso,"x",INT_TO_JSVAL(xy.x),0,0,0);
	JS_DefineProperty(cx,jso,"y",INT_TO_JSVAL(xy.y),0,0,0);
	if(rcBounds.right > 0){
		JS_DefineProperty(cx,jso,"left",INT_TO_JSVAL(rcBounds.left),0,0,0);
		JS_DefineProperty(cx,jso,"right",INT_TO_JSVAL(rcBounds.right),0,0,0);
		JS_DefineProperty(cx,jso,"top",INT_TO_JSVAL(rcBounds.top),0,0,0);
		JS_DefineProperty(cx,jso,"bottom",INT_TO_JSVAL(rcBounds.bottom),0,0,0);
	}
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSBool mlRMMLViewport::JSFUNC_getBounds(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
//	if (argc != 1) return JS_FALSE;
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	*rval=JSVAL_NULL;
	moMedia* pMO=pMLEl->GetMO();
	if(PMO_IS_NULL(pMO)) return JS_TRUE;
	moIViewport* pIViewport=pMO->GetIViewport();
	mlPoint xy; ML_INIT_POINT(xy);
	mlRect rcBounds; ML_INIT_RECT(rcBounds);
	if(argc == 0){
		// возвращаем размер viewport-а
		mlRMMLVisible* pVis = pMLEl->GetVisible();
		rcBounds.right = pVis->mSize.width;
		rcBounds.bottom = pVis->mSize.height;
	}else{
		mlRMMLElement* pMLEl3DObj = NULL;
		if(argc >= 1){
			if(!JSVAL_IS_REAL_OBJECT(argv[0])){
				JS_ReportError(cx, "First argument of getBounds method must be a 3D object");
				return JS_TRUE;
			}
			JSObject* jso3DObj = JSVAL_TO_OBJECT(argv[0]);
			if(mlRMMLObject::IsInstance(cx, jso3DObj) || 
				mlRMMLGroup::IsInstance(cx, jso3DObj) || 
				mlRMMLCharacter::IsInstance(cx, jso3DObj)
			){
				pMLEl3DObj = (mlRMMLElement*)JS_GetPrivate(cx, jso3DObj);
			}else{
				JS_ReportError(cx, "First argument of getBounds method must be a 3D object");
				return JS_TRUE;
			}
			// проверить на принадлежность к 3D-сцене, которую отображает Viewport
			if(((mlRMMLViewport*)pMLEl)->mpCamera == NULL){
				mlTraceWarning(cx, "Viewport doesn't show a 3D scen\n");
				return JS_TRUE;
			}
			mlRMMLElement* p3DScene = (mlRMMLElement*)(pMLEl3DObj->Get3DObject()->GetScene3D());
			mlRMMLElement* pCam3DScene = (mlRMMLElement*)(((mlRMMLViewport*)pMLEl)->mpCamera->GetScene3D());
			if(p3DScene != pCam3DScene){
				mlTraceWarning(cx, "Object %s is not in 3D scene viewport shows");
				return JS_TRUE;
			}
		}
		if(argc > 2){
			mlTraceWarning(cx, "getBounds must get only two arguments\n");
		}
		rcBounds = pIViewport->GetBounds(pMLEl3DObj);
		if(argc == 2){
			mlRMMLVisible* pArgVis = NULL;
			if(JSVAL_IS_REAL_OBJECT(argv[0])){
				JSObject* objArg=JSVAL_TO_OBJECT(argv[0]);
				if(JSO_IS_MLEL(cx, objArg)){
					mlRMMLElement* pMLElArg = (mlRMMLElement*)JS_GetPrivate(cx,objArg);
					pArgVis = pMLElArg->GetVisible();
				}
			}
			if(pArgVis == NULL){
				mlTraceError(cx, "getBounds second argument must be a visible\n");
				return JS_TRUE;
			}else{
				mlPoint pntTargetSpace = pArgVis->GetAbsXY();
				rcBounds = pMLEl->GetVisible()->GetAbsBounds();
				rcBounds.left -= pntTargetSpace.x;
				rcBounds.right -= pntTargetSpace.x;
				rcBounds.top -= pntTargetSpace.y;
				rcBounds.bottom -= pntTargetSpace.y;
			}
		}
	}
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	JS_DefineProperty(cx,jso,"xMin",INT_TO_JSVAL(rcBounds.left),0,0,0);
	JS_DefineProperty(cx,jso,"xMax",INT_TO_JSVAL(rcBounds.right),0,0,0);
	JS_DefineProperty(cx,jso,"yMin",INT_TO_JSVAL(rcBounds.top),0,0,0);
	JS_DefineProperty(cx,jso,"yMax",INT_TO_JSVAL(rcBounds.bottom),0,0,0);
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

//JSFUNC(getIntersectionWithPlane,3) // получить точку пересечения луча с экрана с плоскостью, заданной точкой и нормалью
// Position3D getIntersectionWithPlane(x,y, p3dCenter, p3dNormal)
JSBool mlRMMLViewport::JSFUNC_getIntersectionWithPlane(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 4){
		JS_ReportError(cx,"getIntersectionWithPlane(x,y, p3dCenter, p3dNormal) must get four arguments at least");
		return JS_TRUE;
	}
	if(!JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1])){
		JS_ReportError(cx,"getIntersectionWithPlane(x,y, p3dCenter, p3dNormal) must get two integer arguments as x and y");
		return JS_TRUE;
	}
	mlPoint XY;
	XY.x=JSVAL_TO_INT(argv[0]);
	XY.y=JSVAL_TO_INT(argv[1]);
	//
	jsval vPosPlane = argv[2];
	if(!JSVAL_IS_REAL_OBJECT(vPosPlane)){
		JS_ReportError(cx,"Third parameter of getIntersectionWithPlane(..) must be a 3D-position object");
		return JS_TRUE;
	}
	JSObject* jsoPosPlane = JSVAL_TO_OBJECT(vPosPlane);
	if(!mlPosition3D::IsInstance(cx, jsoPosPlane)){
		JS_ReportError(cx,"Third parameter of getIntersectionWithPlane(..) must be a 3D-position object");
		return JS_TRUE;
	}
	mlPosition3D* pPosPlane = (mlPosition3D*)JS_GetPrivate(cx, jsoPosPlane);
	ml3DPosition pos3dPlane = pPosPlane->GetPos();
	//
	jsval vPosNormal = argv[3];
	if(!JSVAL_IS_REAL_OBJECT(vPosNormal)){
		JS_ReportError(cx,"Fourth parameter of getIntersectionWithPlane(..) must be a 3D-position object");
		return JS_TRUE;
	}
	JSObject* jsoPosNormal = JSVAL_TO_OBJECT(vPosNormal);
	if(!mlPosition3D::IsInstance(cx, jsoPosNormal)){
		JS_ReportError(cx,"Fourth parameter of getIntersectionWithPlane(..) must be a 3D-position object");
		return JS_TRUE;
	}
	mlPosition3D* pPosNormal = (mlPosition3D*)JS_GetPrivate(cx, jsoPosNormal);
	ml3DPosition pos3dNormal = pPosNormal->GetPos();
	//
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	moMedia* pMO=pMLEl->GetMO();
	if(PMO_IS_NULL(pMO)) return JS_FALSE;
	moIViewport* pIViewport=pMO->GetIViewport();
	ML_ASSERTION(cx, pIViewport!=NULL,"mlRMMLViewport::JSFUNC_getObjectIn");
	bool bBackSide = false;
	ml3DPosition p3dRet = pIViewport->GetIntersectionWithPlane(XY, pos3dPlane, pos3dNormal, bBackSide);

	JSObject* jsoPos = mlPosition3D::newJSObject(cx);
	mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx,jsoPos);
	pPos->SetPos(p3dRet);
	jsval jsvBackSide = BOOLEAN_TO_JSVAL(bBackSide);
	wr_JS_SetUCProperty(cx, jsoPos, L"backSide", -1, &jsvBackSide);
	*rval = OBJECT_TO_JSVAL(jsoPos);
	
	return JS_TRUE;
}

//JSFUNC(getProjectionToLand,1) // получить точку проекции 3D-точки на землю с учетом гравитации
// Position3D getProjectionToLand(p3d)
JSBool mlRMMLViewport::JSFUNC_getProjectionToLand(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 1){
		JS_ReportError(cx,"getProjectionToLand(Position3D) must get an argument");
		return JS_TRUE;
	}
	ml3DPosition pos3dParam; ML_INIT_3DPOSITION(pos3dParam);

	bool bError = true;
	for(;;){
		jsval vObj = argv[0];
		if(!JSVAL_IS_REAL_OBJECT(vObj)){
			break;
		}
		JSObject* jsoObj = JSVAL_TO_OBJECT(vObj);
		if(mlPosition3D::IsInstance(cx, jsoObj)){
			mlPosition3D* pPosObj = (mlPosition3D*)JS_GetPrivate(cx, jsoObj);
			pos3dParam = pPosObj->GetPos();
			bError = false;
			break;
		}else if(cx, JSVAL_IS_MLEL(cx, vObj)){
			mlRMMLElement* pMLEL = JSVAL_TO_MLEL(cx, vObj);
			mlRMML3DObject* p3dObj = pMLEL->Get3DObject();
			if(p3dObj != NULL){
				pos3dParam = p3dObj->GetPos();
				bError = false;
				break;
			}
		}
		break;
	}
	if(bError){
		JS_ReportError(cx,"Parameter of getProjectionToLand(..) must be a 3D-position object or a 3D-object");
		return JS_TRUE;
	}
	mlRMMLViewport* pViewport = (mlRMMLViewport*)JS_GetPrivate(cx,obj);
	ML_ASSERTION(cx, pViewport!=NULL,"mlRMMLViewport::JSFUNC_getProjectionToLand");
	ml3DPosition p3dRet = pViewport->GetProjectionToLand(pos3dParam);

	JSObject* jsoPos = mlPosition3D::newJSObject(cx);
	mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx,jsoPos);
	pPos->SetPos(p3dRet);
	*rval = OBJECT_TO_JSVAL(jsoPos);
	return JS_TRUE;
}

//JSFUNC(getProjectionToLand,1) // получить точку проекции 3D-точки на землю с учетом объектов
// Position3D getProjectionToObjects(p3d)
JSBool mlRMMLViewport::JSFUNC_getProjectionToObjects(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 1){
		JS_ReportError(cx,"getProjectionToObjects(Position3D) must get an argument");
		return JS_TRUE;
	}
	ml3DPosition pos3dParam; ML_INIT_3DPOSITION(pos3dParam);

	bool bError = true;
	for(;;){
		jsval vObj = argv[0];
		if(!JSVAL_IS_REAL_OBJECT(vObj)){
			break;
		}
		JSObject* jsoObj = JSVAL_TO_OBJECT(vObj);
		if(mlPosition3D::IsInstance(cx, jsoObj)){
			mlPosition3D* pPosObj = (mlPosition3D*)JS_GetPrivate(cx, jsoObj);
			pos3dParam = pPosObj->GetPos();
			bError = false;
			break;
		}else if(cx, JSVAL_IS_MLEL(cx, vObj)){
			mlRMMLElement* pMLEL = JSVAL_TO_MLEL(cx, vObj);
			mlRMML3DObject* p3dObj = pMLEL->Get3DObject();
			if(p3dObj != NULL){
				pos3dParam = p3dObj->GetPos();
				bError = false;
				break;
			}
		}
		break;
	}
	if(bError){
		JS_ReportError(cx,"Parameter of getProjectionToObject(..) must be a 3D-position object or a 3D-object");
		return JS_TRUE;
	}
	mlRMMLViewport* pViewport = (mlRMMLViewport*)JS_GetPrivate(cx,obj);
	ML_ASSERTION(cx, pViewport!=NULL,"mlRMMLViewport::JSFUNC_getProjectionToLand");
	ml3DPosition p3dRet = pViewport->GetProjectionToObjects(pos3dParam);

	JSObject* jsoPos = mlPosition3D::newJSObject(cx);
	mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx,jsoPos);
	pPos->SetPos(p3dRet);
	*rval = OBJECT_TO_JSVAL(jsoPos);
	return JS_TRUE;
}

JSBool mlRMMLViewport::JSFUNC_setLODScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc < 1){
		JS_ReportError(cx,"setLODScale(x) must get one float argument at least");
		return JS_FALSE;
	}
	if(!JSVAL_IS_NUMBER(argv[0])){
		JS_ReportError(cx,"setLODScale(x) must get one float argument at least");
		return JS_TRUE;
	}
	
	double scale = 0;
	jsval v = argv[0];
	if(!JSVAL_IS_NUMBER(v)){
		JS_ReportError(cx, "Argument of setLODScale must be number");
		return JS_FALSE;
	}
	JS_ValueToNumber(cx, v, &(scale));
	//g->lod.SetLODScale((float)scale);

	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	moMedia* pMO=pMLEl->GetMO();
	if(PMO_IS_NULL(pMO)) return JS_FALSE;

	moIViewport* pIViewport=pMO->GetIViewport();
	ML_ASSERTION(cx, pIViewport!=NULL,"mlRMMLViewport::JSFUNC_getRay");
	pIViewport->SetLODScale((float)scale);

	return JS_TRUE;
}

mlMedia* mlRMMLViewport::GetObjectUnderMouse(mlPoint &aMouseXY, bool abGroundOnly, ml3DPosition *apXYZ, ml3DLocation *apLocation, int *apiType, ml3DPosition *apXYTexture, 
											 mlMedia** apaCheckObjects, 
											 mlMedia** appTextureMedia, mlPoint* apXYTextureMedia, bool* abpGround){
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return NULL;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->GetObjectUnderMouse(aMouseXY, abGroundOnly, apXYZ, apLocation, apiType, apXYTexture, apaCheckObjects, appTextureMedia, apXYTextureMedia, abpGround);
}

void mlRMMLViewport::GetRay(mlPoint& aXY, ml3DPosition &aXYZ0, ml3DPosition &aXYZ)
{
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->GetRay(aXY, aXYZ0, aXYZ);
}

void mlRMMLViewport::SetLODScale(float lodScale)
{
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->SetLODScale(lodScale);
}

bool mlRMMLViewport::GetProjection(ml3DPosition &aXYZ, mlPoint &aXY){
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return false;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->GetProjection(aXYZ, aXY);
}

bool mlRMMLViewport::GetProjection(mlMedia* ap3DO, mlPoint &aXY, mlRect &aBounds){
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return false;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->GetProjection(ap3DO, aXY, aBounds);
}

ml3DPosition mlRMMLViewport::GetIntersectionWithPlane(mlPoint aXY, ml3DPosition aPlane, ml3DPosition aNormal, bool& abBackSide){
	ml3DPosition pos0; ML_INIT_3DPOSITION(pos0);
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return pos0;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->GetIntersectionWithPlane(aXY, aPlane, aNormal, abBackSide);
}

// получить точку проекции 3D-точки на землю с учетом гравитации
ml3DPosition mlRMMLViewport::GetProjectionToLand(ml3DPosition a3DPoint){
	ml3DPosition pos0; ML_INIT_3DPOSITION(pos0);
//	mlRMMLCamera* pCamera = (mlRMMLCamera*)GetCamera();
//	if(pCamera == NULL)
//		return pos0;
//	mlRMMLScene3D* p3DScene = (mlRMMLScene3D*)pCamera->GetI3DObject()->GetScene3D();
//	if(p3DScene == NULL)
//		return pos0;
//	return p3DScene->GetProjectionToLand(a3DPoint);
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return pos0;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->GetProjectionToLand(a3DPoint);
}

// получить точку проекции 3D-точки на землю с учетом объектов
ml3DPosition mlRMMLViewport::GetProjectionToObjects(ml3DPosition a3DPoint){
	ml3DPosition pos0; ML_INIT_3DPOSITION(pos0);
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return pos0;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->GetProjectionToObjects(a3DPoint);
}

bool mlRMMLViewport::GetObjectStringIDAt(mlPoint aXY, mlOutString& asStringID){
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return false;
	moIViewport* pIViewport=pMO->GetIViewport();
	return pIViewport->GetObjectStringIDAt(aXY, asStringID);
}

// нажали где-то (apntXY) на viewport-е
void mlRMMLViewport::onPress(mlPoint apntXY){
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;
	moIViewport* pIViewport = pMO->GetIViewport();
	pIViewport->onPress(apntXY);
}

// отпустили кнопку мыши
void mlRMMLViewport::onRelease(){
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;
	moIViewport* pIViewport = pMO->GetIViewport();
	pIViewport->onRelease();
}

void mlRMMLViewport::UnknownPropIsSet(){
	mbProbablyHandlerFuncIsSet = true;
}

void mlRMMLViewport::SetTraceObjectOnMouseMove(bool abValue){
	traceObjectOnMouseMove = abValue;
	TraceObjectOnMouseMoveChanged();
}

}
