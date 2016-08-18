
#include "mlRMML.h"
#include "mlSPParser.h"

namespace rmml {

mlRMML3DObject::mlRMML3DObject(void)
{
	level=0;
	mbVisible=true;
	mbInCameraSpace=false;
	mp3DPSR=NULL;
	checkCollisions=true;
	boundingBox=0;
	btRefFlags=0;
	mjssShadow=NULL;
	shadow=NULL;
	mjssMotion=NULL;
	motion=NULL;
	mjssMaterials=NULL;
	materials=NULL;
	in_motion=false;
	intangible=false;
	gravitation=false;
	ML_INIT_COLOR(boundingBoxColor);
	boundingBoxColor.r = boundingBoxColor.g = boundingBoxColor.b = boundingBoxColor.a = 0xFF;
	ML_INIT_COLOR(color);
	color.r = color.g = color.b = color.a = 0x80; // по умолчанию - белый полностью прозрачный цвет
	opacity = 0xFFFF;	// по умолчанию объект полностью непрозрачен 
	mpPVPath = NULL;
	m_positionLocked = false;
}

mlRMML3DObject::~mlRMML3DObject(void)
{
	if(mp3DPSR!=NULL) MP_DELETE( mp3DPSR);
	if((btRefFlags & REFF_SHADOW) && shadow!=NULL)
		shadow->RemoveChangeListener(this);
	shadow=NULL;
	if(mpPVPath != NULL)
		MP_DELETE( mpPVPath);
}

///// JavaScript Variable Table
JSPropertySpec mlRMML3DObject::_JSPropertySpec[] = {
	JSPROP_RW(_pos)
	JSPROPA_RW(_position, _pos)
	JSPROPA_RW(position, _pos)
	JSPROP_RW(_scale)
	JSPROPA_RW(scale, _scale)
	JSPROP_RW(_rotation)
	JSPROPA_RW(rotation, _rotation)
	JSPROP_RW(_visible)
	JSPROPA_RW(visible, _visible)
	JSPROP_RO(_mouse)
	JSPROPA_RO(mouse, _mouse)
	JSPROP_RO(intersections)
	JSPROP_RW(checkCollisions)
	JSPROP_RW(boundingBox)
	JSPROP_RW(level)
	JSPROP_RW(inCameraSpace) //если true, считается, что координаты объекта в пространстве камеры и он рисуется в этих координатах
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMML3DObject::_JSFunctionSpec[] = {
	JSFUNC(doMotion, 1)
	JSFUNC(setMotion, 2)
	JSFUNC(removeMotion, 1)
	JSFUNC(getCollisions, 0)
	JSFUNC(getIntersectionsIn, 0)
	JSFUNC(getIntersectionsOut, 0)
	JSFUNC(findPathTo, 1)	// найти свободный путь из одной точки в другую
	JSFUNC(goPath, 2)
	JSFUNC(getPath, 0)
	JSFUNC(setPathCheckPoints, 1)
	JSFUNC(getPathPercent, 0)
	JSFUNC(setPathPercent, 1)
	JSFUNC(attachTo, 1)	
	JSFUNC(getBounds, 1)
	JSFUNC(moveTo, 2) // переместить объект в определенную позицию за интервал времени в ms
	JSFUNC(getAbsolutePosition, 0)
	JSFUNC(getAbsoluteRotation, 0)
	JSFUNC(attachSound, 2) // bool attachSound(sound[, pos3D]) // присоединение 3D-звука к объекту  
	JSFUNC(detachSound, 1) // bool detachSound(sound) // отсоединение 3D-звука от объекта
	JSFUNC(addPotentialTexture, 1) // добавить потенциальную текстуру
	JSFUNC(setBoundingBoxColor, 1) // задать цвет boundingBox-а
	JSFUNC(getAllTextures, 0) // получить все текстуры
	JSFUNC(detectObjectMaterial, 0) // получить материал под мышкой
	JSFUNC(setMaterialTiling, 1) // установить тайлинг материала
	JSFUNC(showAxis, 0) // показать координатные оси
	JSFUNC(hideAxis, 0) // скрыть координатные оси
	JSFUNC(changeCollisionLevel, 1) // поменять тип коллизии
	JSFUNC(enableShadows, 1) // поменять настройку теней
	JSFUNC(isFullyLoaded, 0) // узнать загрузился ли объект полностью (с текстурами)
	JSFUNC(isIntersectedWithOtherObjects, 0) // узнать пересекается ли объект с другими объектами
	JSFUNC(lockPosition, 1) // 
	{ 0, 0, 0, 0, 0 }
};

#define MLVISEVENT(E) {false, EVNM_##E, mlRMMLVisible::EVID_##E},

EventSpec mlRMML3DObject::_EventSpec[] = {
	MLEVENT(onMouseDown)
	MLEVENT(onRMouseDown)
	MLEVENT(onMouseMove)
	MLEVENT(onRMouseUp)
	MLEVENT(onMouseUp)
	MLEVENT(onPassed)
	MLEVENT(onCollision)
	//MLVISEVENT(onMouseDown)
	//MLVISEVENT(onRMouseDown)
	//MLVISEVENT(onMouseMove)
	//MLVISEVENT(onRMouseUp)
	//MLVISEVENT(onMouseUp)
	{0, 0, 0}
};

ADDPROPFUNCSPECS_IMPL(mlRMML3DObject)

///// JavaScript Set Property Wrapper
JSBool mlRMML3DObject::SetJSProperty(JSContext* cx, int id, jsval *vp) {
	switch(id) {
		case JSPROP__pos: {
			if (m_positionLocked)
				break;
			Create3DPos();
			if(JSVAL_IS_STRING(*vp)){
				JSString* jssVal=JSVAL_TO_STRING(*vp);
				ParsePos(wr_JS_GetStringChars(jssVal),mp3DPSR->pPos);
			}else if(JSVAL_IS_REAL_OBJECT(*vp)){
				JSObject* jso=JSVAL_TO_OBJECT(*vp);
				if(mlPosition3D::IsInstance(cx, jso)){
					mlPosition3D* pSrcPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
					*(mp3DPSR->pPos)=pSrcPos->GetPos();
				}else{
					ml3DPosition pos; ML_INIT_3DPOSITION(pos);
					// выбираем из объекта свойства x,y,z
					// и формируем ml3DPosition
					int iSet = 0;
					jsval v = JSVAL_NULL;
					JS_GetProperty(cx, jso, "x", &v);
					if(!JSVAL_IS_NULL(v))
						if(JS_ValueToNumber(cx, v, &(pos.x)))
							iSet++;
					v = JSVAL_NULL;
					JS_GetProperty(cx, jso, "y", &v);
					if(!JSVAL_IS_NULL(v))
						if(JS_ValueToNumber(cx, v, &(pos.y)))
							iSet++;
					JS_GetProperty(cx, jso, "z", &v);
					if(!JSVAL_IS_NULL(v))
						if(JS_ValueToNumber(cx, v, &(pos.z)))
							iSet++;
					if(iSet == 3)
						*(mp3DPSR->pPos) = pos;

				}
			}
			PosChanged(*(mp3DPSR->pPos));
			break;
		}
		case JSPROP__scale:
			Create3DScl();
			if(JSVAL_IS_STRING(*vp)){
				JSString* jssVal=JSVAL_TO_STRING(*vp);
				ParseScl(wr_JS_GetStringChars(jssVal),mp3DPSR->pScl);
			}else if (JSVAL_IS_OBJECT(*vp)) {
				JSObject* jso=JSVAL_TO_OBJECT(*vp);
				if(mlScale3D::IsInstance(cx, jso)){
					mlScale3D* pSrcScl=(mlScale3D*)JS_GetPrivate(cx,jso);
					*(mp3DPSR->pScl)=pSrcScl->GetScl();
				}else{
					ml3DScale scl; ML_INIT_3DSCALE(scl);
					// выбираем из объекта свойства x,y,z
					// и формируем ml3DPosition
					int iSet = 0;
					if (jso != NULL)
					{
						jsval v = JSVAL_NULL;
						JS_GetProperty(cx, jso, "x", &v);
						if(!JSVAL_IS_NULL(v))
							if(JS_ValueToNumber(cx, v, &(scl.x)))
								iSet++;
						v = JSVAL_NULL;
						JS_GetProperty(cx, jso, "y", &v);
						if(!JSVAL_IS_NULL(v))
							if(JS_ValueToNumber(cx, v, &(scl.y)))
								iSet++;
						JS_GetProperty(cx, jso, "z", &v);
						if(!JSVAL_IS_NULL(v))
							if(JS_ValueToNumber(cx, v, &(scl.z)))
								iSet++;
					}
					else
					{
						iSet = 3;
					}
					if(iSet == 3)
						*(mp3DPSR->pScl) = scl;
				}
			}
			ScaleChanged(*(mp3DPSR->pScl));
			break;
		case JSPROP__rotation:
			Create3DRot();
			if(JSVAL_IS_STRING(*vp)){
				JSString* jssVal=JSVAL_TO_STRING(*vp);
				ParseRot(wr_JS_GetStringChars(jssVal),mp3DPSR->pRot);
			}else if(JSVAL_IS_REAL_OBJECT(*vp)){
				JSObject* jso=JSVAL_TO_OBJECT(*vp);
				if(mlRotation3D::IsInstance(cx, jso)){
					mlRotation3D* pSrcRot=(mlRotation3D*)JS_GetPrivate(cx,jso);
					*(mp3DPSR->pRot)=pSrcRot->GetRot();
				}else{
					ml3DRotationVA rot; ML_INIT_3DROTATION(rot);
					// выбираем из объекта свойства x,y,z,a 
					// и формируем ml3DRotation
					int iSet = 0;
					jsval v = JSVAL_NULL;
					JS_GetProperty(cx, jso, "x", &v);
					if(!JSVAL_IS_NULL(v))
						if(JS_ValueToNumber(cx, v, &(rot.x)))
							iSet++;
					v = JSVAL_NULL;
					JS_GetProperty(cx, jso, "y", &v);
					if(!JSVAL_IS_NULL(v))
						if(JS_ValueToNumber(cx, v, &(rot.y)))
							iSet++;
					JS_GetProperty(cx, jso, "z", &v);
					if(!JSVAL_IS_NULL(v))
						if(JS_ValueToNumber(cx, v, &(rot.z)))
							iSet++;
					JS_GetProperty(cx, jso, "a", &v);
					if(!JSVAL_IS_NULL(v))
						if(JS_ValueToNumber(cx, v, &(rot.a)))
							iSet++;
					if(iSet == 4){
						rot.a = (rot.a*PI/180.0); // в радианы
						*(mp3DPSR->pRot) = rot.GetQuat();
					}
				}
			}
//			Normalize(*(mp3DPSR->pRot));
			RotationChanged(*(mp3DPSR->pRot));
			break;
		case JSPROP__visible:
			ML_JSVAL_TO_BOOL(mbVisible,*vp);
			VisibleChanged();
			//if (!mbVisible)
			//{
			//	CLogValue logValue( 
			//		"[RMML.OBJECTS] An object '",
			//		(const char*)cLPCSTR( GetElem_mlRMML3DObject()->GetName()),
			//		"' (",
			//		(void*)GetElem_mlRMML3DObject(),
			//		") has been hidden (objectID: ",
			//		GetElem_mlRMML3DObject()->GetID()
			//		);
			//	GPSM(cx)->GetContext()->mpLogWriter->WriteLn( logValue);
			//}
			break;
		case JSPROP_checkCollisions:
			ML_JSVAL_TO_BOOL(checkCollisions,*vp);
			CheckCollisionsChanged();
			break;
		case JSPROP_boundingBox:
			ML_JSVAL_TO_INT(boundingBox,*vp);
			BoundingBoxChanged();
			break;
		case JSPROP_level:
			ML_JSVAL_TO_INT(level, *vp);
			levelChanged(level);
			break;
		case JSPROP_inCameraSpace:
			ML_JSVAL_TO_BOOL(mbInCameraSpace,*vp);
			InCameraSpaceChanged();
			break;
	}
//return JS_FALSE;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMML3DObject::GetJSProperty(int id, jsval *vp) {
	switch(id) {
		case JSPROP__pos:
			CreateJS3DPos();
			*vp=OBJECT_TO_JSVAL(mp3DPSR->mpJSPos->mjsoPos);
			break;
		case JSPROP__scale:
			CreateJS3DScl();
			*vp=OBJECT_TO_JSVAL(mp3DPSR->mpJSScl->mjsoScl);
			break;
		case JSPROP__rotation:
			CreateJS3DRot();
			*vp=OBJECT_TO_JSVAL(mp3DPSR->mpJSRot->mjsoRot);
			break;
		case JSPROP__visible:
			*vp=BOOLEAN_TO_JSVAL(mbVisible);
			break;
		case JSPROP__mouse:
//			*vp=INT_TO_JSVAL(GetMouseXY().x);
			break;
		case JSPROP_intersections:{
			moI3DObject* pmo3D=GetmoI3DObject();
			if(pmo3D==NULL){
				*vp=JSVAL_NULL;
				return JS_TRUE; 
			}
			mlRMMLScene3D* pScene3D = (mlRMMLScene3D*)GetScene3D();
			if(pScene3D != NULL){
				if(pScene3D->ClearingIntersections()){
					*vp=JSVAL_NULL;
					return JS_TRUE; 
				}
			}
			mlMedia** vObjects;
			int iLen;
			if(!pmo3D->getIntersections(vObjects,iLen)){
				*vp=JSVAL_NULL;
				return JS_TRUE;
			}
			jsval* pjsvals = MP_NEW_ARR( jsval, iLen+1);
			for(int ii=0; ii<iLen; ii++){
				mlMedia* pMO=vObjects[ii];
				mlRMMLElement* pMLEl = (mlRMMLElement*)pMO;
				if(pMLEl->mpParent == NULL)
					pjsvals[ii] = JSVAL_NULL;
				else
					pjsvals[ii] = OBJECT_TO_JSVAL(pMLEl->mjso);
			}
			JSObject* jso=JS_NewArrayObject(GetElem_mlRMML3DObject()->mcx,iLen,pjsvals);
			//
			*vp=OBJECT_TO_JSVAL(jso);
			}break;
		case JSPROP_checkCollisions:
			*vp=BOOLEAN_TO_JSVAL(checkCollisions);
			break;
		case JSPROP_boundingBox:
			*vp=INT_TO_JSVAL(boundingBox);
			break;
		case JSPROP_level:
			*vp=INT_TO_JSVAL(getCurrentLevel());
			break;
		case JSPROP_inCameraSpace:
			*vp=BOOLEAN_TO_JSVAL(mbInCameraSpace);
			break;
		case EVID_onMouseDown:
		case EVID_onRMouseDown:
		case EVID_onMouseMove:
		case EVID_onRMouseUp:
		case EVID_onMouseUp:
		case EVID_onPassed:
			{
			mlRMMLElement* pMLEl = GetElem_mlRMML3DObject();
			int iRes = GPSM(pMLEl->mcx)->CheckEvent(id, pMLEl, vp);
			if(iRes >= 0)
				*vp = BOOLEAN_TO_JSVAL(iRes);
			}break;
	}
	return JS_TRUE;
}

bool mlRMML3DObject::SetShadowRefJSProp(JSContext *cx, jsval *vp){
	if(JSVAL_IS_NULL(*vp) || JSVAL_IS_VOID(*vp)){
		if((btRefFlags & REFF_SHADOW) && shadow!=NULL)
			shadow->RemoveChangeListener(this);
		shadow=NULL;
	}else if(JSVAL_IS_OBJECT(*vp)){
		JSObject* jso=JSVAL_TO_OBJECT(*vp);
		if(mlRMMLShadows3D::IsInstance(cx, jso)){
			shadow=(mlRMMLShadows3D*)JS_GetPrivate(cx,jso);
		}else{
			JS_ReportError(cx, "Parameter is not a Shadows3D-object");
			return false;
		}
	}else if(JSVAL_IS_STRING(*vp)){
		mlRMMLElement* pMLElThis=GetElem_mlRMML3DObject();
		JSString* jss=JSVAL_TO_STRING(*vp);
		if(pMLElThis->mpMO==NULL){
			mjssShadow=jss;
			SAVE_STR_FROM_GC(cx, pMLElThis->mjso, mjssShadow);
			return true;
		}
		mlRMMLElement* pMLEl=pMLElThis->FindElemByName2(wr_JS_GetStringChars(jss));
		if(pMLEl==NULL){ 
			JS_ReportError(cx, "Shadows3D-object '%s' for '%s' not found",JS_GetStringBytes(jss),cLPCSTR(pMLElThis->GetName()));
			return false;
		}
		if(pMLEl->mRMMLType!=MLMT_SHADOWS3D){
			JS_ReportError(cx, "Named object is not a Shadows3D-object");
			return false;
		}
		if(!(btRefFlags & REFF_SHADOW) && mjssShadow!=NULL)
			FREE_STR_FOR_GC(cx,pMLElThis->mjso,mjssShadow); 
		if((btRefFlags & REFF_SHADOW) && shadow!=NULL)
			shadow->RemoveChangeListener(this);
		shadow=(mlRMMLShadows3D*)pMLEl;
		shadow->AddChangeListener(this);
		btRefFlags |= REFF_SHADOW;
	}
	return true;
}

bool mlRMML3DObject::SetMaterialsRefJSProp(JSContext *cx, jsval *vp){
	if(JSVAL_IS_NULL(*vp) || JSVAL_IS_VOID(*vp)){
		// установить объект по умолчанию, если он был создан
//		materials=NULL;
		CreateDefaultMaterials();
	}else if(JSVAL_IS_OBJECT(*vp)){
		JSObject* jso=JSVAL_TO_OBJECT(*vp);
		if(mlRMMLMaterials::IsInstance(cx, jso)){
			materials=(mlRMMLMaterials*)JS_GetPrivate(cx,jso);
		}else{
			JS_ReportError(cx, "Parameter is not a Material-object");
			return false;
		}
	}else if(JSVAL_IS_STRING(*vp)){
		mlRMMLElement* pMLElThis=GetElem_mlRMML3DObject();
		JSString* jss=JSVAL_TO_STRING(*vp);
		if(pMLElThis->mpMO==NULL){
			mjssMaterials=jss;
			SAVE_STR_FROM_GC(cx,pMLElThis->mjso,mjssMaterials);
			return true;
		}
		mlRMMLElement* pMLEl=pMLElThis->FindElemByName2(wr_JS_GetStringChars(jss));
		if(pMLEl==NULL){ 
			JS_ReportError(cx, "Materials-object '%s' for '%s' not found",JS_GetStringBytes(jss),cLPCSTR(pMLElThis->GetName()));
			return false;
		}
		if(pMLEl->mRMMLType!=MLMT_MATERIALS){
			JS_ReportError(cx, "Named object is not a Materials-object");
			return false;
		}
		if(!(btRefFlags & REFF_MATERIALS) && mjssMaterials!=NULL)
			FREE_STR_FOR_GC(cx,pMLElThis->mjso,mjssMaterials); 
		materials=(mlRMMLMaterials*)pMLEl;
		btRefFlags |= REFF_MATERIALS;
	}
	return true;
}

void mlRMML3DObject::SetColorJSProp(JSContext* cx, jsval aJSVal){
	if(SetColorByJSVal(cx, color, aJSVal)){
		ColorChanged();
	}else{
		JS_ReportWarning(cx, "color property value is of unexpected type");
	}
}

mlColor mlRMML3DObject::GetColor(){
	return color;
}

void mlRMML3DObject::GetColorJSProp(JSContext* cx, jsval* apJSVal){
	JSObject* jsoColor = mlColorJSO::newJSObject(cx);
	mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx,jsoColor);
	pColor->SetColorRef(&color, this);
}

void mlRMML3DObject::ColorChanged(mlColor* apColor){
	ColorChanged();
}

void mlRMML3DObject::SetOpacityJSProp(JSContext* cx, jsval aJSVal){
	double _opacity;
	ML_JSVAL_TO_DOUBLE(_opacity, aJSVal);
	if( _opacity>=0.0 && _opacity<=1.0)
	{
		opacity = (unsigned short)(_opacity*0xFFFF);
		OpacityChanged();
	}
	else
	{
		JS_ReportWarning(cx, "Incorrect bounds for opacity");
	}
}

void mlRMML3DObject::GetOpacityJSProp(JSContext* cx, jsval* apJSVal){
	double _opacity = opacity/65535.0;
	jsdouble* jsdbl = JS_NewDouble(cx, _opacity);
	*apJSVal = DOUBLE_TO_JSVAL(jsdbl);
}

void mlRMML3DObject::CreateDefaultMaterials(){
	mlRMMLElement* pMLElThis = GetElem_mlRMML3DObject();
	JSContext* mcx = pMLElThis->mcx;
	if(PMO_IS_NULL(pMLElThis->mpMO)) return;
	if(!(btRefFlags & REFF_MATERIALS) && mjssMaterials!=NULL)
		FREE_STR_FOR_GC(mcx, pMLElThis->mjso,mjssMaterials);
	jsval v;
	wr_JS_GetUCProperty(mcx, pMLElThis->mjso, L"__DefMats_", -1, &v);
	if(!JSVAL_IS_NULL(v) && !JSVAL_IS_VOID(v) && JSVAL_IS_OBJECT(v)){
		JSObject* jsoDefMats = JSVAL_TO_OBJECT(v);
		materials=(mlRMMLMaterials*)JS_GetPrivate(mcx,jsoDefMats);
	}else{
		JSObject* jsoDefMats = mlRMMLMaterials::newJSObject(mcx);
		materials=(mlRMMLMaterials*)JS_GetPrivate(mcx,jsoDefMats);
		v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx, L"__DefMats_"));
		JS_SetProperty(mcx, jsoDefMats, MLELPN_NAME, &v);
		pMLElThis->AddChild(materials);
//		materials->CreateAndLoadMOs();
//		materials->mpParent = pMLElThis;
		materials->AddChangeListener(this);
//		v = OBJECT_TO_JSVAL(jsoDefMats);
//		JS_SetUCProperty(pMLElThis->mcx, pMLElThis->mjso, L"__DefMats_", -1, &v);
			// принудительно создаем один объект 0го материала, чтобы срабатывало устаревшее обращение 
			// типа materials.__0.maps...
		JS_GetElement(mcx, jsoDefMats, 0, &v);
/*
		moI3DObject* p3DO = pMLElThis->mpMO->GetI3DObject();
		for(unsigned ii=0; ii<20; ii++){
			moIMaterial* pMat = p3DO->GetMaterial(ii);
			if (pMat == NULL) continue;
			JSObject* jsoMat = mlRMMLMaterial::newJSObject(mcx);
			mlString sMatName = L"__";
			wchar_t wsMatNum[10];
			swprintf(wsMatNum, L"%d", ii);
			sMatName += wsMatNum;
			jsval vName = STRING_TO_JSVAL(JS_NewUCStringCopyZ(mcx, sMatName.c_str()));
			JS_SetProperty(mcx, jsoMat, MLELPN_NAME, &vName);
			mlRMMLMaterial* pMLElMat = (mlRMMLMaterial*)JS_GetPrivate(mcx, jsoMat);
			pMLElMat->GetProps(pMat);
			materials->AddChild(pMLElMat);
			for(int jj=0; jj<2; jj++){
				const wchar_t* pwcMapName = L"";
				switch(jj){
				case 0: pwcMapName = L"diffuse"; break;
				case 1: pwcMapName = L"specular"; break;
				}
				mlMedia* pMap = pMat->GetMap(pwcMapName);
				if(pMap != NULL){
					// ??
				}
			}
		}
*/
	}
	btRefFlags |= REFF_MATERIALS;
	return;
}

bool mlRMML3DObject::SetMotionRefJSProp(JSContext *cx, jsval *vp){
	if(JSVAL_IS_NULL(*vp) || JSVAL_IS_VOID(*vp)){
//		if((btRefFlags & REFF_MOTION) && motion!=NULL)
//			motion->RemoveChangeListener(this);
		motion=NULL;
	}else if(JSVAL_IS_OBJECT(*vp)){
		// Если 3D-объект еще не создан, а его свойству motion пытаются присвоить ссылку на объект
		mlRMMLElement* pMLElThis=GetElem_mlRMML3DObject();
		if(pMLElThis->mpMO==NULL){
			// значит пытаются прописать в скрипте дочений элемент с именем motions
			JS_ReportError(cx, "3D-object (%s) cannot have child with name 'motion'", cLPCSTR(pMLElThis->GetName()));
			return false;
		}
		JSObject* jso=JSVAL_TO_OBJECT(*vp);
		if(mlRMMLMotion::IsInstance(cx, jso)){
			motion=(mlRMMLMotion*)JS_GetPrivate(cx,jso);
		}else{
			JS_ReportError(cx, "Parameter is not a Motion-object");
			return false;
		}
	}else if(JSVAL_IS_STRING(*vp)){
		mlRMMLElement* pMLElThis=GetElem_mlRMML3DObject();
		JSString* jss=JSVAL_TO_STRING(*vp);
		if(pMLElThis->mpMO==NULL){
			mjssMotion=jss;
			SAVE_STR_FROM_GC(cx,pMLElThis->mjso,mjssMotion);
			return true;
		}
		mlRMMLElement* pMLEl=pMLElThis->GetChild(jss);
		if(pMLEl==NULL){ 
			JS_ReportError(cx, "Motion-object '%s' for '%s' not found",JS_GetStringBytes(jss),cLPCSTR(pMLElThis->GetName()));
			return false;
		}
		if(pMLEl->mRMMLType!=MLMT_MOTION){
			JS_ReportError(cx, "Named object is not a Motion-object");
			return false;
		}
		if(!(btRefFlags & REFF_MOTION) && mjssMotion!=NULL)
			FREE_STR_FOR_GC(cx,pMLElThis->mjso,mjssMotion); 
//		if((btRefFlags & REFF_MOTION) && motion!=NULL)
//			motion->RemoveChangeListener(this);
		motion=(mlRMMLMotion*)pMLEl;
//		motion->AddChangeListener(this);
		btRefFlags |= REFF_MOTION;
	}
	return true;

}

mlRMML3DObject::ml3DPSR::mlJS3DPos::mlJS3DPos(mlRMML3DObject* ap3DObject){
//	ML_ASSERTION(mcx, ap3DObject!=NULL,"mlRMML3DObject::mlJS3DPos::mlJS3DPos");
	mp3DObject=ap3DObject;
	JSContext* cx=mp3DObject->GetElem_mlRMML3DObject()->mcx;
	JSObject* obj=mp3DObject->GetElem_mlRMML3DObject()->mjso;
	mjsoPos=mlPosition3D::newJSObject(cx);
	SAVE_FROM_GC(cx,obj,mjsoPos);
	mlPosition3D* pPos3D=(mlPosition3D*)JS_GetPrivate(cx,mjsoPos);
	pPos3D->Set3DPosRef(this);
}

void mlRMML3DObject::CreateJS3DPos(){
	Create3DPSR();
	if(mp3DPSR->mpJSPos==NULL){
		MP_NEW_V( mp3DPSR->mpJSPos, ml3DPSR::mlJS3DPos, this);
	}
}

mlRMML3DObject::ml3DPSR::mlJS3DScl::mlJS3DScl(mlRMML3DObject* ap3DObject){
//	ML_ASSERTION(mcx, ap3DObject!=NULL,"mlRMML3DObject::mlJS3DScl::mlJS3DScl");
	mp3DObject=ap3DObject;
	JSContext* cx=mp3DObject->GetElem_mlRMML3DObject()->mcx;
	JSObject* obj=mp3DObject->GetElem_mlRMML3DObject()->mjso;
	mjsoScl=mlScale3D::newJSObject(cx);
	SAVE_FROM_GC(cx,obj,mjsoScl);
	mlScale3D* pScl3D=(mlScale3D*)JS_GetPrivate(cx,mjsoScl);
	pScl3D->Set3DSclRef(this);
}

void mlRMML3DObject::CreateJS3DScl(){
	Create3DPSR();
	if(mp3DPSR->mpJSScl==NULL){
		MP_NEW_V( mp3DPSR->mpJSScl, ml3DPSR::mlJS3DScl, this);
	}
}

mlRMML3DObject::ml3DPSR::mlJS3DRot::mlJS3DRot(mlRMML3DObject* ap3DObject){
//	ML_ASSERTION(mcx, ap3DObject!=NULL,"mlRMML3DObject::mlJS3DRot::mlJS3DRot");
	mp3DObject=ap3DObject;
	JSContext* cx=mp3DObject->GetElem_mlRMML3DObject()->mcx;
	JSObject* obj=mp3DObject->GetElem_mlRMML3DObject()->mjso;
	mjsoRot=mlRotation3D::newJSObject(cx);
	SAVE_FROM_GC(cx,obj,mjsoRot);
	mlRotation3D* pRot3D=(mlRotation3D*)JS_GetPrivate(cx,mjsoRot);
	pRot3D->Set3DRotRef(this);
}

void mlRMML3DObject::CreateJS3DRot(){
	Create3DPSR();
	if(mp3DPSR->mpJSRot==NULL){
		MP_NEW_V( mp3DPSR->mpJSRot, ml3DPSR::mlJS3DRot, this);
	}
}

//void mlRMML3DObject::Normalize(ml3DRotation &rot){
//	double dLength = sqrt(rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.a*rot.a);
//	if( dLength != 0.0)
//	{
//		rot.x /= dLength;
//		rot.y /= dLength;
//		rot.z /= dLength;
//		rot.a /= dLength;
//	}
//}

/*
// реализация moIVisible

#define MOIVIS_EVENT_CALL(E) \
	moIVisible* pmoIVisible=GetmoIVisible(); \
	if(pmoIVisible) pmoIVisible->##E(); 

#define MOIVIS_EVENT_IMPL2(E) \
void mlRMML3DObject::##E(){ \
	MOIVIS_EVENT_CALL(E); \
	mlRMMLElement* pMLEl=GetElem_mlRMML3DObject(); \
	if(pMLEl->IsComposition()){ \
		pv_elems pChildren=pMLEl->mpChildren.get(); \
		if(pChildren!=NULL){ \
			v_elems::iterator vi; \
			for(vi=pChildren->begin(); vi != pChildren->end(); vi++ ){ \
				mlRMMLElement* pMLEl=*vi; \
				mlRMML3DObject* pVis=pMLEl->GetVisible(); \
				if(pVis!=NULL) pVis->##E(); \
			} \
		} \
	} \
}

#define MOIVIS_EVENT_IMPL(E) \
void mlRMML3DObject::##E(){ \
	MOIVIS_EVENT_CALL(E); \
}

MOIVIS_EVENT_IMPL2(AbsXYChanged);
MOIVIS_EVENT_IMPL(SizeChanged);
MOIVIS_EVENT_IMPL2(AbsDepthChanged);
MOIVIS_EVENT_IMPL2(AbsVisibleChanged);
*/

// реализация moI3DObject

moI3DObject* mlRMML3DObject::GetmoI3DObject(){
	moMedia* pMO=GetElem_mlRMML3DObject()->GetMO();
	if(PMO_IS_NULL(pMO)) return NULL;
	return pMO->GetI3DObject();
}

ml3DPosition mlRMML3DObject::GetPos(){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject) 
		return pmoI3DObject->GetPos(); 
	Create3DPos();
	return *(mp3DPSR->pPos);
}


int mlRMML3DObject::getCurrentLevel()
{
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject) 
	{
		return pmoI3DObject->GetCurrentLevel(); 	
	}
	return 0;
}

void mlRMML3DObject::SetPos(ml3DPosition &aPos){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject){
		pmoI3DObject->PosChanged(aPos);
		return;
	}
	Create3DPos();
	*(mp3DPSR->pPos)=aPos;
}

void mlRMML3DObject::PosChanged(ml3DPosition &pos)
{
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject) pmoI3DObject->PosChanged(pos); 

	mlRMMLElement* pMMEl = GetElem_mlRMML3DObject();
	mlSceneManager* pSM = GPSM(pMMEl->mcx);

	// Для моего аватара не посылаем изменение локации на сервер, так как это делается в MapManager
	if( pMMEl->IsSynchronized())
	{
		if( pMMEl != pSM->GetMyAvatar())
		{
			const char* pszLocationID = pSM->GetContext()->mpRM->GetObjectLocation(GetElem_mlRMML3DObject());
			GetElem_mlRMML3DObject()->SetLocationSID( cLPWCSTR(pszLocationID));

			const char* pszCommunicationAreaID = pSM->GetContext()->mpRM->GetObjectCommunicationArea(GetElem_mlRMML3DObject());
			GetElem_mlRMML3DObject()->SetCommunicationAreaSID( cLPWCSTR(pszCommunicationAreaID));

			SyncronizeCoords();
		}
	}
}

ml3DScale mlRMML3DObject::GetScale(){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject) 
		return pmoI3DObject->GetScale(); 
	Create3DScl();
	return *(mp3DPSR->pScl);
}

void mlRMML3DObject::SetScale(ml3DScale &aScl){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject){
		pmoI3DObject->ScaleChanged(aScl);
		return;
	}
	Create3DScl();
	*(mp3DPSR->pScl)=aScl;
}

void mlRMML3DObject::ScaleChanged(ml3DScale &scl){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject) pmoI3DObject->ScaleChanged(scl); 
}

ml3DRotation mlRMML3DObject::GetRotation(){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject) 
		return pmoI3DObject->GetRotation(); 
	Create3DRot();
	return *(mp3DPSR->pRot);
}

void mlRMML3DObject::SetRotation(ml3DRotation &aRot){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject){
		pmoI3DObject->RotationChanged(aRot);
		return;
	}
	Create3DRot();
	*(mp3DPSR->pRot)=aRot;
}

void mlRMML3DObject::RotationChanged(ml3DRotation &rot){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(pmoI3DObject) pmoI3DObject->RotationChanged(rot); 
}

void mlRMML3DObject::ClearRefs_mlRMML3DObject(){
	// почистить все intersections в объектах 3D-сцены
	mlRMMLScene3D* pScene3D = (mlRMMLScene3D*)GetScene3D();
	if(pScene3D == NULL) return;
	pScene3D->ClearIntersections();
}

void mlRMML3DObject::ClearIntersections(){
	mlRMMLElement* pMLElThis = GetElem_mlRMML3DObject();
	if(pMLElThis == NULL) return;
	jsval v;
	wr_JS_GetUCProperty(pMLElThis->mcx, pMLElThis->mjso, L"intersections", -1, &v);
}

#define MOI3DO_EVENT_CALL(E) \
	moI3DObject* pmoI3DObject=GetmoI3DObject(); \
	if(pmoI3DObject) pmoI3DObject->##E(); 

#define MOI3DO_EVENT_IMPL(E) \
void mlRMML3DObject::##E(){ \
	MOI3DO_EVENT_CALL(E); \
}

MOI3DO_EVENT_IMPL(VisibleChanged);
MOI3DO_EVENT_IMPL(InCameraSpaceChanged);
MOI3DO_EVENT_IMPL(CheckCollisionsChanged);
MOI3DO_EVENT_IMPL(BoundingBoxChanged);

void mlRMML3DObject::levelChanged(int level)
{
	moI3DObject* pmoI3DObject = GetmoI3DObject();
	if (pmoI3DObject) 
	{
		pmoI3DObject->LevelChanged(level); 
	}
}

// реализация mlI3DObject
mlMedia* mlRMML3DObject::GetParentGroup(){
	mlRMMLElement* pMLEl=GetParent_mlRMML3DObject();
	if(pMLEl==NULL) return NULL;
	if(pMLEl->mRMMLType!=MLMT_GROUP) return NULL;
	return pMLEl;
}

mlMedia* mlRMML3DObject::GetScene3D(){
	mlRMMLElement* pMLEl=GetParent_mlRMML3DObject();
	while(pMLEl){
		if(pMLEl->mRMMLType==MLMT_SCENE3D) return pMLEl;
		pMLEl=pMLEl->GetParent();
	}
	return NULL;
}

__forceinline bool ScanPos(const wchar_t* apwcVal, ml3DPosition* pPos){
	int iRes=swscanf_s(apwcVal,L"%lf; %lf; %lf",&(pPos->x),&(pPos->y),&(pPos->z));
	if(iRes==3) return true;
	iRes=swscanf_s(apwcVal,L"x:%lf y:%lf z:%lf",&(pPos->x),&(pPos->y),&(pPos->z));
	if(iRes==3) return true;
	return false;
}

bool mlRMML3DObject::ParsePos(const wchar_t* apwcVal, ml3DPosition* pPos){
	if(ScanPos(apwcVal, pPos)) return true;
	// возможно там точки, а нужны запятые
	mlString sVal(apwcVal);
	{for(wchar_t* pwcVal = (wchar_t*)sVal.c_str(); *pwcVal; pwcVal++) if(*pwcVal == L'.') *pwcVal = L','; }
	if(ScanPos((wchar_t*)sVal.c_str(), pPos)) return true;
	// возможно там запятые, а нужны точки
	sVal = apwcVal;
	{ for(wchar_t* pwcVal = (wchar_t*)sVal.c_str(); *pwcVal; pwcVal++) if(*pwcVal == L',') *pwcVal = L'.'; }
	if(ScanPos((wchar_t*)sVal.c_str(), pPos)) return true;
	return false;
}

__forceinline bool ScanScl(const wchar_t* apwcVal, ml3DScale* pScl){
	int iRes = swscanf_s(apwcVal,L"%lf; %lf; %lf",&(pScl->x),&(pScl->y),&(pScl->z));
	if(iRes==3) return true;
	iRes=swscanf_s(apwcVal,L"x:%lf y:%lf z:%lf",&(pScl->x),&(pScl->y),&(pScl->z));
	if(iRes==3) return true;
	return false;
}

bool mlRMML3DObject::ParseScl(const wchar_t* apwcVal, ml3DScale* pScl){
	if(ScanScl(apwcVal, pScl)) return true;
	// возможно там точки, а нужны запятые
	mlString sVal(apwcVal);
	{for(wchar_t* pwcVal = (wchar_t*)sVal.c_str(); *pwcVal; pwcVal++) if(*pwcVal == L'.') *pwcVal = L','; }
	if(ScanScl((wchar_t*)sVal.c_str(), pScl)) return true;
	// возможно там запятые, а нужны точки
	sVal = apwcVal;
	{ for(wchar_t* pwcVal = (wchar_t*)sVal.c_str(); *pwcVal; pwcVal++) if(*pwcVal == L',') *pwcVal = L'.'; }
	if(ScanScl((wchar_t*)sVal.c_str(), pScl)) return true;
	return false;
}

__forceinline bool ScanRot(const wchar_t* apwcVal, ml3DRotationVA* pRot){
	int iRes = swscanf_s(apwcVal, L"%lf; %lf; %lf; %lf", &(pRot->x), &(pRot->y), &(pRot->z), &(pRot->a));
	if(iRes == 4) return true;
	iRes = swscanf_s(apwcVal, L"x:%lf y:%lf z:%lf a:%lf", &(pRot->x), &(pRot->y), &(pRot->z), &(pRot->a));
	if(iRes == 4) return true;
	return false;
}

bool mlRMML3DObject::ParseRot(const wchar_t* apwcVal, ml3DRotation* pRot){
	// apwcVal - ветор + поворот в градусах
	ml3DRotationVA rotVA;
	do{
		if(ScanRot(apwcVal, &rotVA)) break;
		// возможно там точки, а нужны запятые
		mlString sVal(apwcVal);
		{for(wchar_t* pwcVal = (wchar_t*)sVal.c_str(); *pwcVal; pwcVal++) if(*pwcVal == L'.') *pwcVal = L','; }
		if(ScanRot((wchar_t*)sVal.c_str(), &rotVA)) break;
		// возможно там запятые, а нужны точки
		sVal = apwcVal;
		{ for(wchar_t* pwcVal = (wchar_t*)sVal.c_str(); *pwcVal; pwcVal++) if(*pwcVal == L',') *pwcVal = L'.'; }
		if(ScanRot((wchar_t*)sVal.c_str(), &rotVA)) break;
		return false;
	}while(false);
	rotVA.a = (rotVA.a*PI/180.0); // в радианы
	*pRot = rotVA.GetQuat();
	return true;
}

void mlRMML3DObject::SetPSR2MO(){
	if(!mp3DPSR) return;
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(!pmoI3DObject) return;
	if(mp3DPSR->pPos)
		pmoI3DObject->PosChanged(*(mp3DPSR->pPos)); 
	if(mp3DPSR->pScl)
		pmoI3DObject->ScaleChanged(*(mp3DPSR->pScl)); 
	if(mp3DPSR->pRot)
		pmoI3DObject->RotationChanged(*(mp3DPSR->pRot)); 
}

// ?? (doMotion(motion,[from[,to]]) надо доделать, чтобы восстанавливать состояния)

JSBool mlRMML3DObject::JSFUNC_doMotion(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	*rval=BOOLEAN_TO_JSVAL(false);
	if (argc != 1){
		JS_ReportError(cx,"doMotion-method must get only one parameter");
		return JS_TRUE;
	}
	mlRMMLMotion* pMotion=NULL;
	for(;;){
		if(argv[0]==JSVAL_NULL) break;
		if(!JSVAL_IS_OBJECT(argv[0])) break;
		//
		JSObject* jsoMotion=JSVAL_TO_OBJECT(argv[0]);
		mlRMMLElement* pMtnMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jsoMotion);
		if(pMtnMLEl==NULL)
		{ 			
			return JS_FALSE; 
		}
		if(pMtnMLEl->mRMMLType!=MLMT_MOTION) break;
		pMotion=(mlRMMLMotion*)pMtnMLEl;
		break;
	}
	if(pMotion==NULL){
		JS_ReportError(cx,"doMotion-method parameter must be a motion-object");
		return JS_TRUE;
	}
	//
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	SET_SYNCH_FLAG4EVENT(GPSM(cx), pMotion, mlRMMLMotion::EVSM_onDone)
	p3DO->doMotion(pMotion);
#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(cx)->GetMode()==smmInitilization){
mlTrace(cx,"---SynchInitInfo--- Add %s.%s.onDone DT:%d SGT:%d\n", 
	cLPCSTR(p3DOMLEl->GetStringID().c_str()), cLPCSTR(pMotion->GetName()), 
	pMotion->duration + 5, (int)(GPSM(cx)->GetSynchEventsGlobalTime()) + pMotion->duration + 5
);
}
#endif
	GPSM(cx)->UpdateNextUpdateDeltaTime(pMotion->duration + 5);
	*rval=BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}	

JSBool mlRMML3DObject::JSFUNC_setMotion(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	*rval=BOOLEAN_TO_JSVAL(false);
	if (argc < 1 || argc > 2){
		JS_ReportError(cx,"setMotion-method must get only one or two parameters");
		return JS_TRUE;
	}
	mlRMMLMotion* pMotion=NULL;
	for(;;){
		if(argv[0]==JSVAL_NULL) break;
		if(!JSVAL_IS_OBJECT(argv[0])) break;
		//
		JSObject* jsoMotion=JSVAL_TO_OBJECT(argv[0]);
		mlRMMLElement* pMtnMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jsoMotion);
		if(pMtnMLEl==NULL) break;
		if(pMtnMLEl->mRMMLType!=MLMT_MOTION) break;
		pMotion=(mlRMMLMotion*)pMtnMLEl;
		break;
	}
	if(pMotion==NULL){
		JS_ReportError(cx,"setMotion-method first parameter must be a motion-object");
		return JS_TRUE;
	}
	//
	int iDurMS=0;
	if(argc==2){
		// ?? если double, то к int-у привести. Если string, то тоже к int-у
		if(!JSVAL_IS_INT(argv[1])){
			JS_ReportError(cx,"setMotion-method second parameter must be an integer");
			return JS_TRUE;
		}
		iDurMS=JSVAL_TO_INT(argv[1]);
	}
	//
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	SET_SYNCH_FLAG4EVENT(GPSM(cx), pMotion, mlRMMLMotion::EVSM_onSet)
	p3DO->setMotion(pMotion,iDurMS);
#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(cx)->GetMode()==smmInitilization){
mlTrace(cx, "---SynchInitInfo--- Add %s.%s.onSet DT:%d SGT:%d\n", 
	cLPCSTR(p3DOMLEl->GetStringID().c_str()), cLPCSTR(pMotion->GetName()), 
	iDurMS, (int)GPSM(cx)->GetSynchEventsGlobalTime() + iDurMS
);
}
#endif
	iDurMS+=5; // onSet придет на следующем тике
	GPSM(cx)->UpdateNextUpdateDeltaTime(iDurMS);
	*rval=BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}	

JSBool mlRMML3DObject::JSFUNC_removeMotion(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc != 1)
	{
		JS_ReportError(cx,"removeMotion-method must get only one parameter");
		return JS_TRUE;
	}
	if(!JSVAL_IS_REAL_OBJECT(argv[0]))
	{
		JS_ReportError(cx,"removeMotion-method parameter must be a motion-object");
		return JS_TRUE;
	}
	//
	JSObject* jsoMotion=JSVAL_TO_OBJECT(argv[0]);
	mlRMMLElement* pMtnMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jsoMotion);
	if(pMtnMLEl==NULL) return JS_FALSE;
	if(pMtnMLEl->mRMMLType!=MLMT_MOTION) return JS_FALSE;
	mlRMMLMotion* pMotion=(mlRMMLMotion*)pMtnMLEl;
	//
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	p3DO->removeMotion(pMotion);
	return JS_TRUE;
}	

void mlRMML3DObject::doMotion(mlRMMLMotion* apMotion){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(!pmoI3DObject) return;
	pmoI3DObject->doMotion(apMotion);
}

void mlRMML3DObject::setMotion(mlRMMLMotion* apMotion,int aiDurMS){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(!pmoI3DObject) return;
	pmoI3DObject->setMotion(apMotion,aiDurMS);
}

void mlRMML3DObject::removeMotion(mlRMMLMotion* apMotion){
	moI3DObject* pmoI3DObject=GetmoI3DObject(); 
	if(!pmoI3DObject) return;
	pmoI3DObject->removeMotion(apMotion);
}

wchar_t* mlRMML3DObject::GetClassName()
{
	if (!GetElem_mlRMML3DObject())
	{
		return NULL;
	}

	if (!GetElem_mlRMML3DObject()->GetClass())
	{
		return NULL;
	}

	return GetElem_mlRMML3DObject()->GetClass()->GetStringProp("name");
}

bool mlRMML3DObject::GetAbsVisible(){
	mlRMMLElement* pParent = GetParent_mlRMML3DObject();
	if(pParent != NULL){
		mlRMML3DObject* pParent3DO = pParent->Get3DObject();
		if(pParent3DO != NULL){
			if(!pParent3DO->GetAbsVisible())
			{
				return false;
			}
		}
	}else{
		// если самый верхний родитель не является сценой,
		// то этот элемент считать невидимым
		if(!GetElem_mlRMML3DObject()->IsScene())
			return false;
	}
	return mbVisible;

}

JSBool mlRMML3DObject::JSFUNC_getCollisions(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc > 0) return JS_FALSE;
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	mlMedia** vObjects;
	int iLen;
	ml3DPosition pos3D;
	if(!p3DO->getCollisions(vObjects,iLen,pos3D)){
		*rval=JSVAL_NULL;
		return JS_TRUE;
	}
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	// goodPos
	JSObject* jsoGoodPos=JS_NewObject(cx,NULL,NULL,NULL);
	jsval vX,vY,vZ;
	mlJS_NewDoubleValue(cx,pos3D.x,&vX);
	JS_DefineProperty(cx,jsoGoodPos,"x",vX,0,0,0);
	mlJS_NewDoubleValue(cx,pos3D.y,&vY);
	JS_DefineProperty(cx,jsoGoodPos,"y",vY,0,0,0);
	mlJS_NewDoubleValue(cx,pos3D.z,&vZ);
	JS_DefineProperty(cx,jsoGoodPos,"z",vZ,0,0,0);
	JS_DefineProperty(cx,jso,"goodPos",OBJECT_TO_JSVAL(jsoGoodPos),0,0,0);
	// objs
	jsval* pjsvals = MP_NEW_ARR( jsval, iLen+1);
	for(int ii=0; ii<iLen; ii++){
		mlMedia* pMO=vObjects[ii];
		mlRMMLElement* pMLEl=(mlRMMLElement*)pMO;
		pjsvals[ii]=OBJECT_TO_JSVAL(pMLEl->mjso);
	}
	JSObject* jsoObjs=JS_NewArrayObject(cx,iLen,pjsvals);
	JS_DefineProperty(cx,jso,"objs",OBJECT_TO_JSVAL(jsoObjs),0,0,0);
	//
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_getIntersectionsIn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc > 0) return JS_FALSE;
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	mlMedia** vObjects;
	int iLen;
	if(!p3DO->getIntersectionsIn(vObjects,iLen)){
		*rval=JSVAL_NULL;
		return JS_TRUE;
	}
	jsval* pjsvals = MP_NEW_ARR( jsval, iLen+1);
	for(int ii=0; ii<iLen; ii++){
		mlMedia* pMO=vObjects[ii];
		mlRMMLElement* pMLEl=(mlRMMLElement*)pMO;
		pjsvals[ii]=OBJECT_TO_JSVAL(pMLEl->mjso);
	}
	JSObject* jso=JS_NewArrayObject(cx,iLen,pjsvals);
	MP_DELETE_ARR( pjsvals);
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_getIntersectionsOut(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc > 0) return JS_FALSE;
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	mlMedia** vObjects;
	int iLen;
	if(!p3DO->getIntersectionsOut(vObjects,iLen)){
		*rval=JSVAL_NULL;
		return JS_TRUE;
	}
	jsval* pjsvals = MP_NEW_ARR( jsval, iLen+1);
	for(int ii=0; ii<iLen; ii++){
		mlMedia* pMO=vObjects[ii];
		mlRMMLElement* pMLEl=(mlRMMLElement*)pMO;
		pjsvals[ii]=OBJECT_TO_JSVAL(pMLEl->mjso);
	}
	JSObject* jso=JS_NewArrayObject(cx,iLen,pjsvals);
	MP_DELETE_ARR( pjsvals);
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

// Переместить объект по пути
// goPath(point array, shiftTime)
JSBool mlRMML3DObject::JSFUNC_goPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc < 1) return JS_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	mlRMMLPath3D* pPath = NULL;
	if(p3DO->mpPVPath != NULL){
		MP_DELETE( p3DO->mpPVPath);
		p3DO->mpPVPath = NULL;
	}
	jsval vArg = argv[0];
	if(!JSVAL_IS_NULL(vArg)){
		if(JSVAL_IS_MLEL(cx, vArg)){
			mlRMMLElement* pMLELPath = (mlRMMLElement*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(vArg));
			if(pMLELPath != NULL && pMLELPath->mRMMLType == MLMT_PATH3D)
				pPath = (mlRMMLPath3D*)pMLELPath;
		}else if(JSVAL_IS_OBJECT(vArg)){
			JSObject* jsoPA = JSVAL_TO_OBJECT(vArg);
			JSClass* pClass = ML_JS_GETCLASS(cx, jsoPA);
			if(pClass != NULL && (isEqual(pClass->name, "Array"))){
				p3DO->mpPVPath = MP_NEW( PointVectorPath);
				JSIdArray* pGPropIDArr = JS_Enumerate(cx, jsoPA);
				int iLen = pGPropIDArr->length;
				for(int ii = 0; ii < iLen; ii++){
					jsid id = pGPropIDArr->vector[ii];
					jsval v;
					if(!JS_IdToValue(cx, id, &v)){
						break;
					}
					jsval vPoint;
					int iIdx = JSVAL_TO_INT(v);
					JS_GetElement(cx, jsoPA, JSVAL_TO_INT(v), &vPoint);
					if(JSVAL_IS_REAL_OBJECT(vPoint)){
						JSObject* jsoPoint = JSVAL_TO_OBJECT(vPoint);
						if(mlPosition3D::IsInstance(cx, jsoPoint)){
							mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, jsoPoint);
							ml3DPosition pos3d = pPos->GetPos();
							p3DO->mpPVPath->push_back(pos3d);
						}else{
							ml3DPosition pos3d; ML_INIT_3DPOSITION(pos3d);
							jsval vX, vY, vZ;
							if(JS_GetProperty(cx, jsoPoint, "x", &vX) && JS_GetProperty(cx, jsoPoint, "y", &vY) && vX != JSVAL_VOID && vY != JSVAL_VOID){
								jsdouble jsdX = 0.0, jsdY = 0.0;
								if(JS_ValueToNumber(cx, vX, &jsdX) && JS_ValueToNumber(cx, vY, &jsdY)){
									pos3d.x = jsdX; 
									pos3d.y = jsdY; 
								}
							}
							if(JS_GetProperty(cx, jsoPoint, "z", &vZ) && vZ != JSVAL_VOID){
								jsdouble jsdZ = 0.0;
								if(JS_ValueToNumber(cx, vZ, &jsdZ)){
									pos3d.z = jsdZ;
								}
							}
							if(pos3d.x != 0.0 && pos3d.y != 0.0)
								p3DO->mpPVPath->push_back(pos3d);
						}
					}
				}
			}
		}
		if(pPath == NULL && p3DO->mpPVPath == NULL){
			JS_ReportError(cx,"Parameter of goPath function must be a Path3D-object reference or point array");
			return JS_FALSE;
		}
	}
	int iFlags = 0;	// shiftTime
	if(argc >= 2){
		jsval vArg2 = argv[1];
		if(JSVAL_IS_INT(vArg2))
			iFlags = JSVAL_TO_INT(vArg2);
	}
	SET_SYNCH_FLAG4EVENT(GPSM(cx), p3DOMLEl, EVSM_onPassed)
	if(pPath != NULL){
		p3DO->goPath(pPath, iFlags);
	}else if(p3DO->mpPVPath != NULL){
		unsigned int uiShiftTime = (unsigned int)iFlags;
		p3DO->goPath(&((*(p3DO->mpPVPath))[0]), p3DO->mpPVPath->size(), uiShiftTime);
	}
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_getPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc > 0) return JS_FALSE;
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	mlRMMLPath3D* pPath=(mlRMMLPath3D*)p3DO->getPath();
	if(pPath==NULL){
		*rval=JSVAL_NULL;
	}else{
		*rval=OBJECT_TO_JSVAL(pPath->mjso);
	}
	return JS_TRUE;
}

float JSValToPercent(JSContext *cx, jsval v){
	float fPercent=0.0;
	if(JSVAL_IS_INT(v)){
		fPercent=JSVAL_TO_INT(v);
	}else if(JSVAL_IS_DOUBLE(v)){
		fPercent=*JSVAL_TO_DOUBLE(v);
	}
	if(fPercent<0.0 || fPercent>100.0){
		JS_ReportWarning(cx,"Must be number in range from 0 to 100 (value=%f)",fPercent);
		if(fPercent<0.0) fPercent=0.0;
		if(fPercent>100.0) fPercent=100.0;
	}
	return fPercent;
}

JSBool mlRMML3DObject::JSFUNC_setPathCheckPoints(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
//	if (argc < 1) return JS_FALSE;
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	std::vector<float> vCheckPoints;
	if(argc==0){
#if _MSC_VER >= 1600
		p3DO->setPathCheckPoints( vCheckPoints.begin()._Ptr,0);
#elif _MSC_VER > 1200
		p3DO->setPathCheckPoints(vCheckPoints.begin()._Myptr,0);
#else
		p3DO->setPathCheckPoints(vCheckPoints.begin(),0);
#endif
		return JS_TRUE;
	}
	for(unsigned int ii=0; ii<argc; ii++){
		jsval v=argv[ii];
		if(JSVAL_IS_NUMBER(v)){
			vCheckPoints.push_back(JSValToPercent(cx, v));
		}else if(JSVAL_IS_OBJECT(v) && JS_IsArrayObject(cx,JSVAL_TO_OBJECT(v))){
			// Array
			JSObject* jsoArr=JSVAL_TO_OBJECT(v);
			jsuint uiLen=0;
			if(JS_GetArrayLength(cx,jsoArr,&uiLen)){
				for(unsigned int jj=0; jj<uiLen; jj++){
					if(JS_GetElement(cx, jsoArr, jj, &v)){
						if(JSVAL_IS_NUMBER(v)){
							vCheckPoints.push_back(JSValToPercent(cx, v));
						}else{
							JS_ReportWarning(cx,"setPathCheckPoints() arguments must be numbers and arrays");
						}
					}
				}
			}
		}else{
			JS_ReportWarning(cx,"setPathCheckPoints() arguments must be numbers and arrays");
		}
	}
#if _MSC_VER >= 1600
	p3DO->setPathCheckPoints( vCheckPoints.begin()._Ptr,vCheckPoints.size());
#elif _MSC_VER > 1200
	p3DO->setPathCheckPoints( vCheckPoints.begin()._Myptr,vCheckPoints.size());
#else
	p3DO->setPathCheckPoints(vCheckPoints.begin(),vCheckPoints.size());
#endif
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_getPathPercent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc > 0) return JS_FALSE;
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	double dPercent=p3DO->getPathPercent();
	mlJS_NewDoubleValue(cx,dPercent,rval);
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_setPathPercent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc < 1) return JS_FALSE;
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	if(!JSVAL_IS_NUMBER(argv[0])){
		JS_ReportError(cx,"Parameter of setPathPercent(..) must be a number");
		return JS_FALSE;
	}
	p3DO->setPathPercent(JSValToPercent(cx, argv[0]));
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_attachTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc > 2) return JS_FALSE;
	mlRMMLElement* p3DOMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO=p3DOMLEl->Get3DObject();
	mlRMMLElement* pMLElDest=NULL;
	wchar_t* pwcBoneName=NULL;
	if(argc > 0){
		if(argv[0]==JSVAL_NULL || argv[0]==JSVAL_VOID){
		}else{
			for(;;){
				if(!JSVAL_IS_OBJECT(argv[0])) break;
				JSObject* jso3DODest=JSVAL_TO_OBJECT(argv[0]);
				if(!JSO_IS_MLEL(cx, jso3DODest)) break;
				pMLElDest=(mlRMMLElement*)JS_GetPrivate(cx, jso3DODest);
				if(pMLElDest->Get3DObject()==NULL){
					pMLElDest=NULL; break;
				}
				break;
			}
			if(pMLElDest==NULL){
				JS_ReportError(cx,"First parameter of attachTo(..) must be a 3D-object");
				return JS_FALSE;
			}
			if(argc >= 2){
				if(!JSVAL_IS_STRING(argv[1])){
					JS_ReportError(cx,"Second parameter of attachTo(..) must be a string (name of a bone)");
					return JS_FALSE;
				}
				JSString* jssBoneName=JSVAL_TO_STRING(argv[1]);
				pwcBoneName=wr_JS_GetStringChars(jssBoneName);
			}
		}
	}
	bool bRet=p3DO->attachTo(pMLElDest,cLPCSTR(pwcBoneName));
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_getBounds(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if (argc > 1){
		JS_ReportError(cx,"Method getBounds must get only one argument");
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	mlRMMLElement* pMLElCoordSysSrc = NULL;
	if(argc > 0){
		if(argv[0]==JSVAL_NULL || argv[0]==JSVAL_VOID){
		}else{
			do{
				if(!JSVAL_IS_REAL_OBJECT(argv[0])) break;
				JSObject* jso3DOCoordSysSrc = JSVAL_TO_OBJECT(argv[0]);
				if(!JSO_IS_MLEL(cx, jso3DOCoordSysSrc)) break;
				pMLElCoordSysSrc = (mlRMMLElement*)JS_GetPrivate(cx, jso3DOCoordSysSrc);
				if(pMLElCoordSysSrc->mRMMLType == MLMT_SCENE3D)
					break;
				if(pMLElCoordSysSrc->Get3DObject() == NULL){
					pMLElCoordSysSrc = NULL; break;
				}
			}while(false);
			if(pMLElCoordSysSrc == NULL){
				JS_ReportError(cx,"Argument of getBounds method must be a 3D-object or 3D-scene");
				*rval = JSVAL_NULL;
				return JS_TRUE;
			}
		}
	}

	ml3DBounds bndBounds = p3DO->getBounds(pMLElCoordSysSrc);

	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	jsval val;
	mlJS_NewDoubleValue(cx, bndBounds.xMin, &val); wr_JS_SetUCProperty(cx,jso,L"xMin",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.xMax, &val); wr_JS_SetUCProperty(cx,jso,L"xMax",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.yMin, &val); wr_JS_SetUCProperty(cx,jso,L"yMin",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.yMax, &val); wr_JS_SetUCProperty(cx,jso,L"yMax",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.zMin, &val); wr_JS_SetUCProperty(cx,jso,L"zMin",4,&val);
	mlJS_NewDoubleValue(cx, bndBounds.zMax, &val); wr_JS_SetUCProperty(cx,jso,L"zMax",4,&val);

	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_moveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc <= 0){
		JS_ReportError(cx,"Method moveTo must get one argument at least");
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	jsval vDestPos = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vDestPos)){
		JS_ReportError(cx,"First parameter of moveTo(..) must be a 3D-position object");
		return JS_TRUE;
	}
	JSObject* jsoDestPos = JSVAL_TO_OBJECT(vDestPos);
	if(!mlPosition3D::IsInstance(cx, jsoDestPos)){
		JS_ReportError(cx,"First parameter of moveTo(..) must be a 3D-position object");
		return JS_TRUE;
	}
	mlPosition3D* pDestPos = (mlPosition3D*)JS_GetPrivate(cx, jsoDestPos);
	int iDuration = 0;
	bool bCheckCollisions = true;
	if(argc > 1){
		if(JSVAL_IS_BOOLEAN(argv[1])){
			bCheckCollisions = JSVAL_TO_BOOLEAN(argv[1]);
		}else{
			jsval vDur = argv[1];
			if(!JSVAL_IS_INT(vDur)){
				JS_ReportError(cx,"Second parameter of moveTo(..) must be an integer or a boolean");
				return JS_TRUE;
			}
			iDuration = JSVAL_TO_INT(vDur);
		}
		if(argc > 2){
			if(!JSVAL_IS_BOOLEAN(argv[2])){
				JS_ReportError(cx,"Third parameter of moveTo(..) must be an integer or a boolean");
				return JS_TRUE;
			}
			bCheckCollisions = JSVAL_TO_BOOLEAN(argv[2]);
		}
	}
	int iID = p3DO->moveTo(pDestPos->GetPos(), iDuration, bCheckCollisions);
	*rval = INT_TO_JSVAL(iID);
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_getAbsolutePosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	ml3DPosition pos = p3DO->GetAbsolutePosition();
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

// присоединение 3D-звука к объекту  bool attachSound(sound[, pos3D])
JSBool mlRMML3DObject::JSFUNC_attachSound(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	if(argc == 0){
		JS_ReportError(cx,"Method attachSound must get one argument at least");
		return JS_TRUE;
	}
	jsval vArg = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vArg)){
		JS_ReportError(cx,"Method attachSound must get a sound object");
		return JS_TRUE;
	}
	JSObject* jsoSound = JSVAL_TO_OBJECT(vArg);
	if(!mlRMMLAudio::IsInstance(cx, jsoSound)){
		JS_ReportError(cx,"Method attachSound must get a sound object");
		return JS_TRUE;
	}
	mlRMMLAudio* pAudio = (mlRMMLAudio*)JS_GetPrivate(cx, jsoSound);
	ml3DPosition pos3D; ML_INIT_3DPOSITION(pos3D)
	if(argc > 1){
		// значит еще заданы координаты (объектом)
		jsval vPos = argv[1];
		if(!JSVAL_IS_REAL_OBJECT(vPos)){
			JS_ReportError(cx,"Second parameter of attachSound(..) must be a 3D-position object");
			return JS_TRUE;
		}
		JSObject* jsoPos = JSVAL_TO_OBJECT(vPos);
		if(!mlPosition3D::IsInstance(cx, jsoPos)){
			JS_ReportError(cx,"Second parameter of attachSound(..) must be a 3D-position object");
			return JS_TRUE;
		}
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, jsoPos);
		pos3D = pPos->GetPos();
	}
	bool bRet = p3DO->attachSound(pAudio, pos3D);
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

// отсоединение 3D-звука от объекта bool detachSound(sound)
JSBool mlRMML3DObject::JSFUNC_detachSound(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	if(argc == 0){
		JS_ReportError(cx,"Method detachSound must get one argument at least");
		return JS_TRUE;
	}
	jsval vArg = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vArg)){
		JS_ReportError(cx,"Method detachSound must get a sound object");
		return JS_TRUE;
	}
	JSObject* jsoSound = JSVAL_TO_OBJECT(vArg);
	if(!mlRMMLAudio::IsInstance(cx, jsoSound)){
		JS_ReportError(cx,"Method detachSound must get a sound object");
		return JS_TRUE;
	}
	mlRMMLAudio* pAudio = (mlRMMLAudio*)JS_GetPrivate(cx, jsoSound);
	
	bool bRet = p3DO->detachSound(pAudio);
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_getAbsoluteRotation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	ml3DRotation pos = p3DO->getAbsoluteRotation();
	JSObject* jsoNew = mlRotation3D::newJSObject(cx);
	mlRotation3D* pNewPos = (mlRotation3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetRot(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}


JSBool mlRMML3DObject::JSFUNC_getAllTextures(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();

	std::string textures = p3DO->getAllTextures();

	JSString* str = JS_NewStringCopyZ( cx, textures.c_str());

	*rval = STRING_TO_JSVAL(str);

	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_detectObjectMaterial(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();

	int id  = p3DO->detectObjectMaterial( GPSM(cx)->GetMouseXY().x, GPSM(cx)->GetMouseXY().y);

	*rval = INT_TO_JSVAL(id);

	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_showAxis(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();

	p3DO->showAxis();
	*rval = JSVAL_TRUE;

	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_hideAxis(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();

	p3DO->hideAxis();
	*rval = JSVAL_TRUE;

	return JS_TRUE;
}

// addPotentialTexture(src[|image])
JSBool mlRMML3DObject::JSFUNC_addPotentialTexture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	if(argc <= 0){
		JS_ReportError(cx,"Method addPotentialTexture must get one argument at least");
		return JS_TRUE;
	}
	bool bError = true;
	jsval vArg = argv[0];
	if(JSVAL_IS_REAL_OBJECT(vArg) && JSVAL_IS_MLEL(cx, vArg)){
		JSObject* jsoVisible = JSVAL_TO_OBJECT(vArg);
		mlRMMLElement* pMLEl = JSVAL_TO_MLEL(cx, vArg);
		mlRMMLVisible* pVisible = pMLEl->GetVisible();
		if(pVisible != NULL){
			if(pMLEl->mRMMLType == MLMT_IMAGE){ // предполагается, что позже будут video, composition и т.д.
				bool bRet = p3DO->addPotentialTexture(pMLEl);
				*rval = BOOLEAN_TO_JSVAL(bRet);
				bError = false;
			}
		}
	}else if(JSVAL_IS_STRING(vArg)){
		JSString* jssSrc = JSVAL_TO_STRING(vArg);
		const wchar_t* pwcSrc = wr_JS_GetStringChars(jssSrc);
		mlString sSrc = GPSM(cx)->RefineResPathEl(p3DOMLEl, pwcSrc);
		bool bRet = p3DO->addPotentialTexture(sSrc.c_str());
		*rval = BOOLEAN_TO_JSVAL(bRet);
		bError = false;
	}
	if(bError){
		JS_ReportError(cx, "Method addPotentialTexture must get a visible 2D object or SRC-string");
		return JS_TRUE;
	}
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// setMaterialTiling(int|float)
JSBool mlRMML3DObject::JSFUNC_setMaterialTiling(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	if(argc <= 1){
		JS_ReportError(cx,"Method setMaterialTiling must get two arguments at least");
		return JS_TRUE;
	}
	bool bError = true;
	int iTextureSlot = 0;
	double fTilingKoef = 1.0;
	jsval vArg = argv[0];
	if (JSVAL_IS_INT(vArg))
	{
		iTextureSlot = JSVAL_TO_INT(argv[0]);		
	}
	else
	{
		JS_ReportError(cx, "Method setMaterialTiling must get first argument as int");
		return JS_TRUE;
	}

	vArg = argv[1];
	if (JSVAL_IS_DOUBLE(vArg))
	{
		ML_JSVAL_TO_DOUBLE(fTilingKoef, argv[1]);		
	}
	else if (JSVAL_IS_INT(vArg))
	{
		fTilingKoef = (float)JSVAL_TO_INT(argv[1]);		
	}
	else
	{
		JS_ReportError(cx, "Method setMaterialTiling must get second argument as float or int");
		return JS_TRUE;
	}

	p3DO->setMaterialTiling(iTextureSlot, fTilingKoef);

	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// changeCollisionLevel(int)
JSBool mlRMML3DObject::JSFUNC_changeCollisionLevel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	if(argc < 1){
		JS_ReportError(cx,"Method changeCollisionLevel must get one argument at least");
		return JS_TRUE;
	}
	int level = 0;
	jsval vArg = argv[0];

	if (JSVAL_IS_INT(vArg))
	{
		level = JSVAL_TO_INT(argv[0]);		
	}
	else
	{
		JS_ReportError(cx, "Method changeCollisionLevel must get argument as int");
		return JS_TRUE;
	}


	p3DO->changeCollisionLevel(level);

	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_lockPosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	if(argc < 1){
		JS_ReportError(cx,"Method lockPosition must get one argument at least");
		return JS_TRUE;
	}
	bool isLock = false;
	jsval vArg = argv[0];
	if (JSVAL_IS_BOOLEAN(vArg))
	{
		isLock = JSVAL_TO_BOOLEAN(argv[0]);		
	}
	else
	{
		JS_ReportError(cx, "Method lockPosition must get argument as boolean");
		return JS_TRUE;
	}

	p3DO->enableLockPos(isLock);

	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// enableShadow(bool)
JSBool mlRMML3DObject::JSFUNC_enableShadows(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	if(argc < 1){
		JS_ReportError(cx,"Method enableShadow must get one argument at least");
		return JS_TRUE;
	}
	bool isEnabled = false;
	jsval vArg = argv[0];

	if (JSVAL_IS_BOOLEAN(vArg))
	{
		isEnabled = JSVAL_TO_BOOLEAN(argv[0]);		
	}
	else
	{
		JS_ReportError(cx, "Method enableShadow must get argument as boolean");
		return JS_TRUE;
	}


	p3DO->enableShadows(isEnabled);

	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_isIntersectedWithOtherObjects(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	*rval = p3DO->isIntersectedWithOtherObjects() ? JSVAL_TRUE : JSVAL_FALSE;
	return JS_TRUE;
}

JSBool mlRMML3DObject::JSFUNC_isFullyLoaded(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	*rval = p3DO->isFullyLoaded() ? JSVAL_TRUE : JSVAL_FALSE;
	return JS_TRUE;
}

//setBoundingBoxColor([string | Color | r, g, b, a])
JSBool mlRMML3DObject::JSFUNC_setBoundingBoxColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	if(argc <= 0){
		JS_ReportError(cx,"Method setBoundingBoxColor must get one argument at least");
		return JS_TRUE;
	}
	bool bError = true;
	jsval vArg = argv[0];
	if(JSVAL_IS_STRING(vArg)){
		mlColor color;
		wchar_t* jscColor = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
		if(ParseColor(cx, (const wchar_t* &)jscColor, color)){
			p3DO->boundingBoxColor = color;
		}
		bError = false;
	}else if(JSVAL_IS_REAL_OBJECT(vArg)){
		JSObject* jsoArg = JSVAL_TO_OBJECT(vArg);
		if(mlColorJSO::IsInstance(cx, jsoArg)){
			mlColorJSO* pArgColor=(mlColorJSO*)JS_GetPrivate(cx,jsoArg);
			p3DO->boundingBoxColor = pArgColor->GetColor();
			bError = false;
		}
	}else if(JSVAL_IS_INT(vArg)){
		// целое число, значит от [0..255]
		// ??
	}else if(JSVAL_IS_DOUBLE(vArg)){
		// вещественное число, значит от [0..1]
		// ??
	}
	if(bError){
		JS_ReportError(cx,"Invalid argument for setBoundingBoxColor method");
		return JS_TRUE;
	}
	p3DO->BoundingBoxColorChanged();
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// Найти свободный путь из одной точки в другую для указанного 3D-объекта.
// point array findPathTo(from, to)
// point array findPathTo(to) - от текущего положения 3D-объекта
JSBool mlRMML3DObject::JSFUNC_findPathTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	// считать параметры
	if(argc <= 0){
		JS_ReportError(cx,"Method findPathTo must get one arguments at least");
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}
	// 
	jsval vToPos = argv[0];
	ml3DPosition toPos; ML_INIT_3DPOSITION(toPos);
	bool bOK = false;
	if(JSVAL_IS_REAL_OBJECT(vToPos)){
		if(mlPosition3D::IsInstance(cx, JSVAL_TO_OBJECT(vToPos))){
			mlPosition3D* pToPos = (mlPosition3D*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(vToPos));
			toPos = pToPos->GetPos();
			bOK = true;
		}else{
			JSObject* jsoToPos = JSVAL_TO_OBJECT(vToPos);
			jsval vX, vY;
			jsdouble dX = 0, dY = 0;
			if(JS_GetProperty(cx, jsoToPos, "x", &vX) && JS_GetProperty(cx, jsoToPos, "y", &vY)
				&& JS_ValueToNumber(cx, vX, &dX) && JS_ValueToNumber(cx, vY, &dY)
				&& dX != 0 && dY != 0
			){
				toPos.x = dX;
				toPos.y = dY;
				bOK = true;
			}
			jsval vZ;
			jsdouble dZ = 0;
			if(JS_GetProperty(cx, jsoToPos, "z", &vZ) && JS_ValueToNumber(cx, vZ, &dZ) && dZ != 0){
				toPos.z = dZ;
			}
		}
	}
	if(!bOK){
		JS_ReportError(cx,"First argument of findPathTo(..) must be a 3D-position object");
		return JS_TRUE;
	}
	//
	ml3DPosition fromPos; ML_INIT_3DPOSITION(fromPos);
	bOK = false;
	if(argc > 1){
		vToPos = argv[1];
		if(!JSVAL_IS_NULL(vToPos)){
			if(JSVAL_IS_REAL_OBJECT(vToPos)){
				fromPos = toPos;
				if(mlPosition3D::IsInstance(cx, JSVAL_TO_OBJECT(vToPos))){
					mlPosition3D* pToPos = (mlPosition3D*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(vToPos));
					toPos = pToPos->GetPos();
					bOK = true;
				}else{
					JSObject* jsoToPos = JSVAL_TO_OBJECT(vToPos);
					jsval vX, vY;
					jsdouble dX = 0, dY = 0;
					if(JS_GetProperty(cx, jsoToPos, "x", &vX) && JS_GetProperty(cx, jsoToPos, "y", &vY)
						&& JS_ValueToNumber(cx, vX, &dX) && JS_ValueToNumber(cx, vY, &dY)
						&& dX != 0 && dY != 0
						){
							toPos.x = dX;
							toPos.y = dY;
							bOK = true;
						}
						jsval vZ;
						jsdouble dZ = 0;
						if(JS_GetProperty(cx, jsoToPos, "z", &vZ) && JS_ValueToNumber(cx, vZ, &dZ) && dZ != 0){
							toPos.z = dZ;
						}
				}
			}
			if(!bOK){
				JS_ReportError(cx,"Second argument of findPathTo(..) must be a 3D-position object");
				return JS_TRUE;
			}
		}
	}
	mlRMMLElement* p3DOMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMML3DObject* p3DO = p3DOMLEl->Get3DObject();
	// Если только один аргумент (To)
	if(!bOK){
		// то в качестве точки начала (from) берем положение самого объекта
		fromPos = p3DO->GetAbsolutePosition();
	}
	// 
	ml3DPosition* pvPath = NULL;
	int iLen = 0;
	bool bRet = p3DO->findPathTo(fromPos, toPos, pvPath, iLen);
	if(!bRet){
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	// Создать JS-массив
	jsval* pjsvals = MP_NEW_ARR( jsval, iLen + 1);
	for(int ii = 0; ii < iLen; ii++){
		ml3DPosition& pos = pvPath[ii];
		JSObject* jsoPos = mlPosition3D::newJSObject(cx);
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx,jsoPos);
		pPos->SetPos(pos);
		pjsvals[ii] = OBJECT_TO_JSVAL(jsoPos);
	}
	JSObject* jsoPoints = JS_NewArrayObject(cx,iLen,pjsvals);
	//JS_DefineProperty(cx,jso,"objs",OBJECT_TO_JSVAL(jsoObjs),0,0,0);
	*rval = OBJECT_TO_JSVAL(jsoPoints);
	return JS_TRUE;
}

//void mlRMML3DObject::CreateShadow3D(){
//	if(mjsoShadow!=NULL) return;
//	JSContext* cx=GetElem_mlRMML3DObject()->mcx;
//	JSObject* jso=GetElem_mlRMML3DObject()->mjso;
//	mjsoShadow=mlShadow3D::newJSObject(cx);
//	SAVE_FROM_GC(cx,jso,mjsoShadow);
//}

// реализация mlI3DObject
__forceinline JSObject* ToJSArray(JSContext* cx, mlMedia** aapIns,int aiLen){
	if(aiLen<=0) return NULL;
	jsval* pv = MP_NEW_ARR( jsval, aiLen);
	for(int ii=0; ii<aiLen; ii++){
		if(aapIns[ii]==NULL)
			pv[ii]=JSVAL_NULL;
		else
			pv[ii]=OBJECT_TO_JSVAL(((mlRMMLElement*)aapIns[ii])->mjso);
	}
	JSObject* jso=JS_NewArrayObject(cx,aiLen,pv);
	MP_DELETE_ARR( pv);
	return jso;
}
void Set_Event_intersectionsInOut(mlRMMLElement* apMLEl,mlMedia** aapIns,int aiLenIn,mlMedia** aapOuts,int aiLenOut){
	JSContext* cx=apMLEl->mcx;
	JSObject* jsoIns=ToJSArray(cx,aapIns,aiLenIn);
	jsval v=JSVAL_NULL;
	if(jsoIns!=NULL) v=OBJECT_TO_JSVAL(jsoIns);
	wr_JS_SetUCProperty(cx,GPSM(cx)->GetEventGO(),L"intersectionsIn",-1,&v);
	JSObject* jsoOuts=ToJSArray(cx,aapOuts,aiLenOut);
	v=JSVAL_NULL;
	if(jsoOuts!=NULL) v=OBJECT_TO_JSVAL(jsoOuts);
	wr_JS_SetUCProperty(cx,GPSM(cx)->GetEventGO(),L"intersectionsOut",-1,&v);
}

void mlRMML3DObject::IntersectionChanged(){
	mlRMMLElement* pMLEl = GetElem_mlRMML3DObject();
	// если в моем update-е, 
	if(GPSM(pMLEl->mcx)->Updating()){
		// то выполняем сразу же
		// ?? установить Event.intersectionsIn и Event.intersectionsOut
		int iLenIn=0, iLenOut=0;
		mlMedia** apIns;
		mlMedia** apOuts;
		getIntersectionsIn(apIns,iLenIn);
		getIntersectionsOut(apOuts,iLenOut);
		Set_Event_intersectionsInOut(GetElem_mlRMML3DObject(),apIns,iLenIn,apOuts,iLenOut);
		pMLEl->CallListeners(JSPROP_intersections);
		GPSM(pMLEl->mcx)->ResetEvent();	// 2007-03-02 Alex
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(pMLEl->mcx)->SaveInternalPath(Data,pMLEl);
		Data << false;
		// сохраняем intersections In-ы
		int iLen=0;
		mlMedia** apIns;
		getIntersectionsIn(apIns,iLen);
		Data << iLen;
		if(iLen>0)
			Data.put(apIns,iLen*sizeof(mlMedia*));
		// сохраняем intersections Out-ы
		iLen=0;
		getIntersectionsOut(apIns,iLen);
		Data << iLen;
		if(iLen>0)
			Data.put(apIns,iLen*sizeof(mlMedia*));
		// ставим в очередь событий
		GPSM(pMLEl->mcx)->GetContext()->mpInput->AddInternalEvent(JSPROP_intersections, true, 
			(unsigned char*)Data.data(), Data.size(), GPSM(pMLEl->mcx)->GetMode() == smmInitilization);
	}
}

mlIMaterial* mlRMML3DObject::GetMaterial(unsigned char auID){
	if(!(btRefFlags & REFF_MATERIALS))
		return NULL;
	return materials->GetMaterial(auID);
}

mlColor mlRMML3DObject::GetBoundingBoxColor(){
/*
	// если цвет не задан
	if(boundingBoxColor.a == 0){
		// то выдаем цвет по умолчанию для объектов 3D-сцены
		mlRMMLScene3D* pScene3D = (mlRMMLScene3D*)GetScene3D();
		if(pScene3D != NULL){
			return pScene3D->GetDefaultBoundingBoxColor(GetElem_mlRMML3DObject());
		}
	}
*/
	return boundingBoxColor;
}

#define AVATAR_MOVE_DELTA					1000	/*10 метров*/
#define SYNCH_ZONE_BOUND_AREA_SIZE			100
#define AVATAR_MOVE_DELTA_NEAR_ZONE_BOUND	100		/*1 метр*/
#define	ZONE_SIZE							(20000.0f)
#define	ZONE_SIZE_INT						(int)ZONE_SIZE

void mlRMML3DObject::SyncronizeCoords()
{
	mlRMMLElement* pMLEl = GetElem_mlRMML3DObject();
	if( !pMLEl)
		return;

	if( !pMLEl->IsSynchronized())
		return;

	mlRMML3DObject* p3DObj = pMLEl->Get3DObject();

	ml3DPosition pos = p3DObj->GetAbsolutePosition();

	// Здесь определяем необходимость посылки данных на сервер об изменении координат объекта
	// Это нужно для того, чтобы сервер отслеживал синхронизированную зону объекта, поэтому имеет смысл ее 
	// посылать только когда объект реально меняет зону синхронизации. 
	// Для надежности посылаем координату при значительном  изменении в обычном режиме и при незначительном вблизи границы зоны синхронизации.
	int intX = pos.x > 0 ? (int)pos.x : (int)-pos.x;
	int intY = pos.y > 0 ? (int)pos.y : (int)-pos.y;
	bool avatarIsNearSynchZoneBound = ((intY % ZONE_SIZE_INT) < SYNCH_ZONE_BOUND_AREA_SIZE) || ((intY % ZONE_SIZE_INT) < SYNCH_ZONE_BOUND_AREA_SIZE);

	float avatarMoveDelta = AVATAR_MOVE_DELTA;
	if( avatarIsNearSynchZoneBound)
		avatarMoveDelta = AVATAR_MOVE_DELTA_NEAR_ZONE_BOUND;

	// for rmml
	bool isChanged = (fabs(pos.x - pMLEl->mpSynch->GetX()) > avatarMoveDelta) || (fabs(pos.y - pMLEl->mpSynch->GetY()) > avatarMoveDelta);
	if (isChanged)	
	{
		pMLEl->mpSynch->SetXY(pos.x, pos.y);
	}	
}

void Set_Event_checkPoint(mlRMMLElement* apMLEl,float afCheckPoint){
	JSContext* cx=apMLEl->mcx;
	jsval v;
	if(mlJS_NewDoubleValue(cx,afCheckPoint,&v)){
		JS_DefineProperty(cx,GPSM(cx)->GetEventGO(),"checkPoint",v,0,0,0);
	}
}

void mlRMML3DObject::onPassed(){
	mlRMMLElement* pMLEl = GetElem_mlRMML3DObject();
	bool bDontSendSynchEvents = (pMLEl->mucSynchFlags4Events & EVSM_onPassed)?true:false;
	// если в моем update-е, 
	if(GPSM(pMLEl->mcx)->Updating()){
		// то выполняем сразу же
		Set_Event_checkPoint(pMLEl,getPassedCheckPoint());
		GPSM(pMLEl->mcx)->mbDontSendSynchEvents = bDontSendSynchEvents;
		pMLEl->CallListeners(EVID_onPassed);
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(pMLEl->mcx)->SaveInternalPath(Data,pMLEl);
		Data << bDontSendSynchEvents;
		// сохраняем check point
		float fCheckPoint=getPassedCheckPoint();
		Data << fCheckPoint;
		// ставим в очередь событий
		GPSM(pMLEl->mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onPassed, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(pMLEl->mcx)->GetMode() == smmInitilization);
	}
}

void Set_Event_collisionObjects(mlRMMLElement* apMLEl, int aFlags, mlMedia** aapObjects, int aiLen, ml3DPosition endPoint, int pathPointCount, bool isStatic){
	JSContext* cx=apMLEl->mcx;
	jsval v = JSVAL_NULL;

	v = INT_TO_JSVAL( isStatic ? 1 : 0);
	wr_JS_SetUCProperty(cx, GPSM(cx)->GetEventGO(), L"staticFlags", -1, &v);

	v = INT_TO_JSVAL( aFlags);
	wr_JS_SetUCProperty(cx, GPSM(cx)->GetEventGO(), L"collisionFlags", -1, &v);

	v = INT_TO_JSVAL( pathPointCount);
	wr_JS_SetUCProperty(cx, GPSM(cx)->GetEventGO(), L"pathPointCount", -1, &v);

	JSObject* jsoEndPoint = mlPosition3D::newJSObject(cx);
	mlPosition3D* pEndPoint = (mlPosition3D*)JS_GetPrivate(cx, jsoEndPoint);
	pEndPoint->SetPos(endPoint);
	v = OBJECT_TO_JSVAL(jsoEndPoint);
	wr_JS_SetUCProperty(cx, GPSM(cx)->GetEventGO(), L"endPoint", -1, &v);

	JSObject* jsoIns = ToJSArray(cx, aapObjects, aiLen);
	v = JSVAL_NULL;
	if(jsoIns != NULL) v = OBJECT_TO_JSVAL(jsoIns);
	wr_JS_SetUCProperty(cx, GPSM(cx)->GetEventGO(), L"collisionObjects", -1, &v);
}

void mlRMML3DObject::onCollision(mlMedia** aapObjects, int aFlags, ml3DPosition endPoint, int pathPointCount, bool isStatic){
	mlRMMLElement* pMLEl = GetElem_mlRMML3DObject();
	bool bDontSendSynchEvents = (pMLEl->mucSynchFlags4Events & EVSM_onPassed)?true:false;
	int iLen = 0;
	mlMedia** ppMedia = aapObjects;
	if (ppMedia != NULL)
		for(; *ppMedia != NULL; iLen++, ppMedia++);
	// если в моем update-е, 
	//if(GPSM(pMLEl->mcx)->Updating()){
	//	// то выполняем сразу же
	//	Set_Event_collisionObjects(pMLEl, aapObjects, iLen);
	//	GPSM(pMLEl->mcx)->mbDontSendSynchEvents = bDontSendSynchEvents;
	//	pMLEl->CallListeners(EVID_onCollision);
	//}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(pMLEl->mcx)->SaveInternalPath(Data,pMLEl);
		Data << bDontSendSynchEvents;
		// сохраняем объекты 
		Data << iLen;
		if(iLen>0)
			Data.put(aapObjects, iLen * sizeof(mlMedia*));
		Data << aFlags;
		Data << endPoint.x;
		Data << endPoint.y;
		Data << endPoint.z;
		Data << pathPointCount;
		Data << isStatic;
		// ставим в очередь событий
		GPSM(pMLEl->mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onCollision, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(pMLEl->mcx)->GetMode() == smmInitilization);
	//}
}

void mlRMML3DObject::SetButtonEventData(char aidEvent){
	switch(aidEvent){
		case mlRMMLPButton::EVID_onPress:
		case mlRMMLPButton::EVID_onRelease:
			GPSM(GetElem_mlRMML3DObject()->mcx)->SetTextureXYToEventGO(GetElem_mlRMML3DObject());
			break;
	}
}

// установить необходимые свойства в Event на события видимого объекта
void mlRMML3DObject::O3d_SetVisibleEventData(char aidEvent){
	//switch(aidEvent){
	//}
	GPSM(GetElem_mlRMML3DObject()->mcx)->SetTextureXYToEventGO(GetElem_mlRMML3DObject());
}

bool mlRMML3DObject::O3d_SetEventData(char aidEvent, mlSynchData &Data){
	switch(aidEvent){
	case JSPROP_intersections:{
		// Ins
		int iLenIn=0;
		mlMedia** apIns=NULL;
		Data >> iLenIn;
		if(iLenIn > 0){
			apIns=(mlMedia**)Data.getDataInPos();
			Data.skip(iLenIn*sizeof(mlMedia*));
		}
		// Outs
		int iLenOut=0;
		mlMedia** apOuts=NULL;
		Data >> iLenOut;
		if(iLenOut > 0){
			apOuts=(mlMedia**)Data.getDataInPos();
			Data.skip(iLenOut*sizeof(mlMedia*));
		}
		Set_Event_intersectionsInOut(GetElem_mlRMML3DObject(),apIns,iLenIn,apOuts,iLenOut);
		}return true;
	case EVID_onPassed:{
		float fCheckPoint=-1.0;
		Data >> fCheckPoint;
		Set_Event_checkPoint(GetElem_mlRMML3DObject(),fCheckPoint);
		}return true;
	case EVID_onCollision:{
		int iLen = 0;
		mlMedia** apObjects = NULL;
		Data >> iLen;
		if(iLen > 0){
			apObjects=(mlMedia**)Data.getDataInPos();
			Data.skip(iLen * sizeof(mlMedia*));
		}
		int flags;
		Data >> flags;
		ml3DPosition endPoint;
		Data >> endPoint.x;
		Data >> endPoint.y;
		Data >> endPoint.z;
		int pathPointCount;
		Data >> pathPointCount;
		bool isStatic;
		Data >> isStatic;
		Set_Event_collisionObjects(GetElem_mlRMML3DObject(), flags, apObjects, iLen, endPoint, pathPointCount, isStatic);
		}return true;
	}
	return true;
}

//mlE3DShadowType mlRMML3DObject::GetShadowType(){
//	if(mjsoShadow==NULL) return MLSHT_NONE;
//	mlRMMLShadows3D* pShadow=(mlShadow3D*)JS_GetPrivate(GetElem_mlRMML3DObject()->mcx,mjsoShadow);
//	return pShadow->mShadow.type;
//}
//
//ml3DPlane mlRMML3DObject::GetShadowPlane(){
//	if(mjsoShadow==NULL){
//		ml3DPlane plane; ML_INIT_3DPLANE(plane);
//		return plane;
//	}
//	mlShadow3D* pShadow=(mlShadow3D*)JS_GetPrivate(GetElem_mlRMML3DObject()->mcx,mjsoShadow);
//	return pShadow->mShadow.plane;
//}
//
//mlColor mlRMML3DObject::GetShadowColor(){
//	if(mjsoShadow==NULL){
//		mlColor color; ML_INIT_COLOR(color);
//		return color;
//	}
//	mlShadow3D* pShadow=(mlShadow3D*)JS_GetPrivate(GetElem_mlRMML3DObject()->mcx,mjsoShadow);
//	return pShadow->mShadow.color;
//}
//
//mlMedia* mlRMML3DObject::GetShadowLight(){
//	if(mjsoShadow==NULL) return NULL;
//	mlShadow3D* pShadow=(mlShadow3D*)JS_GetPrivate(GetElem_mlRMML3DObject()->mcx,mjsoShadow);
//	return pShadow->mShadow.light;
//}

// реализация moI3DObject
#define MOI3D_GET_MOI3D0 \
	moI3DObject* pmo3D=GetmoI3DObject(); \
	if(pmo3D==NULL)	return; \
	pmo3D
#define MOI3D_GET_MOI3D(ERRV) \
	moI3DObject* pmo3D=GetmoI3DObject(); \
	if(pmo3D==NULL)	return ERRV; \
	return pmo3D

bool mlRMML3DObject::Get3DPropertiesInfo(mlSynchData &aData){ 
	MOI3D_GET_MOI3D(false)->Get3DPropertiesInfo(aData);
}
bool mlRMML3DObject::Get3DProperty(char* apszName,char* apszSubName, mlOutString &sVal){ 
	MOI3D_GET_MOI3D(false)->Get3DProperty(apszName,apszSubName, sVal);
}
bool mlRMML3DObject::Set3DProperty(char* apszName,char* apszSubName,char* apszVal){ 
	MOI3D_GET_MOI3D(false)->Set3DProperty(apszName,apszSubName,apszVal);
}
bool mlRMML3DObject::getCollisions(mlMedia** &avObjects, int &aiLength, ml3DPosition &aPos3D){
	MOI3D_GET_MOI3D(false)->getCollisions(avObjects, aiLength, aPos3D);
}
bool mlRMML3DObject::getIntersections(mlMedia** &avObjects, int &aiLength){
	MOI3D_GET_MOI3D(false)->getIntersections(avObjects, aiLength);
}
void mlRMML3DObject::ShadowChanged(){
	MOI3D_GET_MOI3D0->ShadowChanged();
}
moIMaterial* mlRMML3DObject::GetMaterial(unsigned auID){
	MOI3D_GET_MOI3D(NULL)->GetMaterial(auID);
}
void mlRMML3DObject::MaterialsChanged(){
	MOI3D_GET_MOI3D0->MaterialsChanged();
}
void mlRMML3DObject::MaterialChanged(unsigned auID){
	MOI3D_GET_MOI3D0->MaterialChanged(auID);
}
void mlRMML3DObject::MaterialMapChanged(unsigned auID, const wchar_t* apwcIndex){
	MOI3D_GET_MOI3D0->MaterialMapChanged(auID, apwcIndex);
}
void mlRMML3DObject::MotionChanged(){
	MOI3D_GET_MOI3D0->MotionChanged();
}
void mlRMML3DObject::GravitationChanged(){
	MOI3D_GET_MOI3D0->GravitationChanged();
}
void mlRMML3DObject::IntangibleChanged(){
	MOI3D_GET_MOI3D0->IntangibleChanged();
}
void mlRMML3DObject::BoundingBoxColorChanged(){
	MOI3D_GET_MOI3D0->BoundingBoxColorChanged();
}
bool mlRMML3DObject::getIntersectionsIn(mlMedia** &avObjects, int &aiLength){
	MOI3D_GET_MOI3D(false)->getIntersectionsIn(avObjects, aiLength);
}
bool mlRMML3DObject::getIntersectionsOut(mlMedia** &avObjects, int &aiLength){
	MOI3D_GET_MOI3D(false)->getIntersectionsOut(avObjects, aiLength);
}
void mlRMML3DObject::goPath(mlMedia* apPath, int aiFlags){
	MOI3D_GET_MOI3D0->goPath(apPath, aiFlags);
}
void mlRMML3DObject::goPath(ml3DPosition* apPath, int aiPointCount, unsigned int auShiftTime){
	MOI3D_GET_MOI3D0->goPath(apPath, aiPointCount, auShiftTime);
}
mlMedia* mlRMML3DObject::getPath(){
	MOI3D_GET_MOI3D(NULL)->getPath();
}
bool mlRMML3DObject::findPathTo(ml3DPosition &aTo, ml3DPosition &aFrom, ml3DPosition* &avPath, int &aiLength){
	MOI3D_GET_MOI3D(false)->findPathTo(aTo, aFrom, avPath, aiLength);
}
void mlRMML3DObject::setPathVelocity(float afVelocity){
	MOI3D_GET_MOI3D0->setPathVelocity(afVelocity);
}
void mlRMML3DObject::setPathCheckPoints(float* apfCheckPoints, int aiLength){
	MOI3D_GET_MOI3D0->setPathCheckPoints(apfCheckPoints, aiLength);
}
float mlRMML3DObject::getPassedCheckPoint(){
	MOI3D_GET_MOI3D(0.0)->getPassedCheckPoint();
}
void mlRMML3DObject::setPathPercent(float afPercent){
	MOI3D_GET_MOI3D0->setPathPercent(afPercent);
}
float mlRMML3DObject::getPathPercent(){
	MOI3D_GET_MOI3D(0.0)->getPathPercent();
}
bool mlRMML3DObject::attachTo(mlMedia* ap3DODest,const char* apszBoneName){
	MOI3D_GET_MOI3D(false)->attachTo(ap3DODest,apszBoneName);
}
ml3DBounds mlRMML3DObject::getBounds(mlMedia* ap3DOCoordSysSrc){
	ml3DBounds bnd;
	ML_INIT_3DBOUNDS(bnd);
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL) return bnd;
	return pmo3D->getBounds(ap3DOCoordSysSrc);
}

ml3DBounds mlRMML3DObject::getSubobjBounds(int aiMatID){
	ml3DBounds bnd;
	ML_INIT_3DBOUNDS(bnd);
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL) return bnd;
	return pmo3D->getSubobjBounds(aiMatID);
}

int mlRMML3DObject::moveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL) return -1;
	return pmo3D->moveTo(aPos3D, aiDuaration, abCheckCollisions);
}

ml3DPosition mlRMML3DObject::getAbsolutePosition(){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL){
		ml3DPosition pos; ML_INIT_3DPOSITION(pos);
		return pos;
	}
	return pmo3D->getAbsolutePosition();
}

ml3DRotation mlRMML3DObject::getAbsoluteRotation(){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL){
		ml3DRotation pos; ML_INIT_3DROTATION(pos);
		return pos;
	}
	return pmo3D->getAbsoluteRotation();
}

bool mlRMML3DObject::attachSound(mlMedia* pAudio, ml3DPosition& pos3D){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL){
		return false;
	}
	return pmo3D->attachSound(pAudio, pos3D);
}

bool mlRMML3DObject::detachSound(mlMedia* pAudio){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL){
		return false;
	}
	return pmo3D->detachSound(pAudio);
}


std::string mlRMML3DObject::getAllTextures(){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
		return "";
	return pmo3D->getAllTextures();
}

int mlRMML3DObject::detectObjectMaterial(int x, int y){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
		return false;
	return pmo3D->detectObjectMaterial(x, y);
}

void mlRMML3DObject::showAxis(){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
		return;
	pmo3D->showAxis();
}

void mlRMML3DObject::hideAxis(){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
		return;
	pmo3D->hideAxis();
}

bool mlRMML3DObject::addPotentialTexture(mlMedia* apVisibleObject){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
		return false;
	return pmo3D->addPotentialTexture(apVisibleObject);
}

bool mlRMML3DObject::addPotentialTexture(const wchar_t* apwcSrc){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
		return false;
	return pmo3D->addPotentialTexture(apwcSrc);
}

void mlRMML3DObject::setMaterialTiling(int textureSlotID, float tilingKoef)
{
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
	{
		return ;
	}

	pmo3D->SetMaterialTiling(textureSlotID, tilingKoef);
}

void mlRMML3DObject::changeCollisionLevel(int level)
{
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
	{
		return ;
	}

	pmo3D->ChangeCollisionLevel(level);
}

void mlRMML3DObject::enableLockPos(bool isLocked)
{
	m_positionLocked = isLocked;
}

void mlRMML3DObject::enableShadows(bool isEnabled)
{
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
	{
		return ;
	}

	pmo3D->EnableShadows(isEnabled);
}

bool mlRMML3DObject::isIntersectedWithOtherObjects()
{
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
	{
		return false;
	}

	return pmo3D->IsIntersectedWithOtherObjects();
}

bool mlRMML3DObject::isFullyLoaded()
{
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
	{
		return false;
	}

	return pmo3D->IsFullyLoaded();
}

void mlRMML3DObject::SetMaterialDescription(int aiMatIdx, const wchar_t* apwcDescr){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
		return;
	pmo3D->SetMaterialDescription(aiMatIdx, apwcDescr);
}

ml3DPosition mlRMML3DObject::GetAbsolutePosition()
{
	ml3DPosition pos = {0, 0, 0};
	moI3DObject* pmo3D = GetmoI3DObject();
	if( pmo3D)
	{
		pos = pmo3D->getAbsolutePosition();
	}else{
		mlRMMLElement* pMLEl = GetElem_mlRMML3DObject();		
	}
	return pos;
}

void mlRMML3DObject::ColorChanged(){
	MOI3D_GET_MOI3D0->ColorChanged();
}

void mlRMML3DObject::OpacityChanged(){
	MOI3D_GET_MOI3D0->OpacityChanged();
}

unsigned char mlRMML3DObject::GetOpacity(){
	return (opacity >> 8);
}

// Получить путь к файлу карты текстуры, если он доступен через ResManager
bool mlRMML3DObject::GetMaterialTextureSrc(int aiMatIdx, const wchar_t* apwcMapName, mlOutString &asSrc){
	moI3DObject* pmo3D = GetmoI3DObject();
	if(pmo3D == NULL)
		return false;
	return pmo3D->GetMaterialTextureSrc(aiMatIdx, apwcMapName, asSrc);
}

}










