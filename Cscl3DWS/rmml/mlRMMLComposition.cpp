#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include "rmmlsafe.h"

namespace rmml {

mlRMMLComposition::mlRMMLComposition(void):
	MP_VECTOR_INIT(mvTimeouts)
{
	// mlRMMLElement
	mRMMLType=MLMT_COMPOSITION;
	mType=MLMT_COMPOSITION;
	// mlRMMLPButton
	enabled=true;
	ML_INIT_RECT(mBounds);
	bInvalidBounds=true;
	bTimeoutsChanged = false;
	mbDisableChildButtons = true;
}

mlRMMLComposition::~mlRMMLComposition(void){
	ClearTimeouts(true);
	if(mRMMLType==MLMT_SCENE){
		GPSM(mcx)->RemoveScene(this);
	}
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLComposition::_JSPropertySpec[] = {
	JSPROP_RW(_final) // JSPROP_PERMANENT
	JSPROPA_RW(final, _final)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLComposition::_JSFunctionSpec[] = {
	JSFUNC(getBounds,1)
	JSFUNC(setTimeout,2)
	JSFUNC(setInterval,2)
	JSFUNC(clearTimeout,2)
	JSFUNC(clearInterval,2)
	JSFUNC(handleButtonEvents,1) // сбросить или установить флаг обработки кнопочных событий
	JSFUNC(disableChildButtons,1) // сбросить или установить флаг игнорирования дочерних кнопок
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLComposition::_EventSpec[] = {
	MLEVENT(onBoundsChange)	// изменились размеры композиции или разброс глубины видимых объектов (getBounds())
};

//ADDPROPFUNCSPECS_IMPL(mlRMMLComposition)

MLJSCLASS_IMPL_BEGIN(Composition,mlRMMLComposition,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLComposition)

///// JavaScript Set Property Wrapper
JSBool mlRMMLComposition::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
//mlRMMLElement* pML = (mlRMMLElement*)JS_GetPrivate(cx, obj);
////if(isEqual(pML->GetName(), L"ui_COrderedCompositionBase")){
//	jsval vClss;
//	JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_CLASSES, &vClss);
//	jsval v;
//	JS_GetProperty(cx, JSVAL_TO_OBJECT(vClss), "ui_COrderedCompositionBase", &v);
//	if(JSVAL_IS_OBJECT(v)){
//		JSObject* jso = JSVAL_TO_OBJECT(v);
//		JSObject* jsoProto = JS_GetPrototype(cx, jso);
//		if(jso == obj)
//		int hh=0;
//		if(jsoProto == obj)
//		int hh=0;
//	}
////}
//if((mlRMMLComposition*)JS_GetPrivate(cx, obj) == GPSM(mcx)->GetUI())
//int hh=0;
	SET_PROPS_BEGIN(mlRMMLComposition);
		SET_PROTO_PROP(mlRMMLElement);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
			case JSPROP__final:{
				bool bFinal=true;
				ML_JSVAL_TO_BOOL(bFinal,*vp);
				if(!bFinal){
					priv->mType=MLMT_SCENECLASS;
				}
				}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLComposition::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLComposition);
		GET_PROTO_PROP(mlRMMLElement);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLPButton);
		default:;
			switch(iID){
			case JSPROP__final:{
				*vp=BOOLEAN_TO_JSVAL(true);
				}break;
			}
	GET_PROPS_END;
//	if (JSVAL_IS_STRING(id)) {
//		int hh=0;
//	}
	return JS_TRUE;
}

//void mlRMMLComposition::JSDestructor(JSContext *cx, JSObject *obj) {
//	mlRMMLComposition *p = (mlRMMLComposition*)JS_GetPrivate(cx, obj);
//	if (p)  MP_DELETE( p;
//}

mlRMMLElement* mlRMMLComposition::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLComposition* pNewEL=(mlRMMLComposition*)GET_RMMLEL(mcx, mlRMMLComposition::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

void mlRMMLComposition::InvalidBounds(){
	bInvalidBounds=true;
	mlRMMLVisible::InvalidBounds();
}

void mlRMMLComposition::UpdateBounds(){
	if(bInvalidBounds){
		bInvalidBounds=false;
		if(mpChildren!=NULL){
			bool bFirst=true;
			v_elems::iterator vi;
			for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
				mlRMMLElement* pMLEl=*vi;
				if (pMLEl->GetType() == MLMT_SCRIPT)
					continue;
				mlRMMLVisible* pMLElVis=pMLEl->GetVisible();
				if(pMLElVis){
					if(!pMLElVis->mbVisible) continue;
					mlRect rc=pMLElVis->GetBounds();
					if(rc.left < rc.right){
						if(bFirst){
							mBounds=rc;
							bFirst=false;
						}else{
							if(rc.left < mBounds.left) mBounds.left=rc.left;
							if(rc.top < mBounds.top) mBounds.top=rc.top;
							if(rc.right > mBounds.right) mBounds.right=rc.right;
							if(rc.bottom > mBounds.bottom) mBounds.bottom=rc.bottom;
						}
					}
				}
				mlRMMLPButton* pBut=pMLEl->GetButton();
				if(pBut){
					mlRect rc=pBut->GetBounds();
					if(pMLElVis){
						rc.left += pMLElVis->mXY.x;
						rc.right += pMLElVis->mXY.x;
						rc.top += pMLElVis->mXY.y;
						rc.bottom += pMLElVis->mXY.y;
					}
					if(rc.left < rc.right){
						if(bFirst){
							mBounds=rc;
							bFirst=false;
						}else{
							if(rc.left < mBounds.left) mBounds.left=rc.left;
							if(rc.top < mBounds.top) mBounds.top=rc.top;
							if(rc.right > mBounds.right) mBounds.right=rc.right;
							if(rc.bottom > mBounds.bottom) mBounds.bottom=rc.bottom;
						}
					}
				}
			}
			mBounds.left+=mXY.x;
			mBounds.right+=mXY.x;
			mBounds.top+=mXY.y;
			mBounds.bottom+=mXY.y;
		}
		// CallListeners(EVID_onBoundsChange); // Tandy 02.09.2010: отключил, потому что вроде как нигде не используется, а работу rmml существенно замедляет
	}
}

// выдает границы композиции относительно координат родителя
mlRect mlRMMLComposition::GetBounds(){
	UpdateBounds();
	return mBounds;
}

int mlRMMLComposition::IsPointIn(const mlPoint aPnt){
	// если мышка на одном из дочерних элементов
	if(mpChildren==NULL) return 0;
	for(v_elems::iterator vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		mlRMMLVisible* pVis = pMLEl->GetVisible();
		if(pVis == NULL) continue;
		if(!pVis->GetAbsVisible()) continue; 
		mlPoint pnt = aPnt;
		if(pMLEl->IsPointIn(pnt)){
			return 1;
		}
	}
	return 0;
}

mlRMMLElement* mlRMMLComposition::GetButtonUnderMouse(mlPoint &aMouseXY, mlButtonUnderMouse &aBUM){ // , mlRMMLVisible** appVisible, mlRMMLElement** app3DObject, mlRMMLElement** appTextureObject, mlPoint* apLocalXY){
	if(!enabled || !mbVisible) return NULL;
	UpdateBounds();
	if(!ML_POINT_IN_RECT(aMouseXY,mBounds)) return NULL;
	mlPoint MouseXY=aMouseXY;
	MouseXY.x=aMouseXY.x-mXY.x;
	MouseXY.y=aMouseXY.y-mXY.y;
	// если у меня есть обработчики событий кнопки
	if(mbBtnEvListenersIsSet && mbDisableChildButtons){
		// тогда я буду сам обрабатывать кнопочные события
		if(hitArea.IsDefined()){
			if(hitArea.IsPointIn(MouseXY)){
				return this;
			}
		}else{
			// если мышка на одном из дочерних элементов
			if(mpChildren==NULL) return NULL;
			for(v_elems::iterator vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
				mlRMMLElement* pMLEl=*vi;
				mlRMMLVisible* pVis = pMLEl->GetVisible();
				if(pVis == NULL) continue;
				if(!pVis->GetAbsVisible()) continue; 
				if(pMLEl->IsPointIn(MouseXY)){
					return this;
				}
			}
		}
		return NULL;
	}
	if(mpChildren==NULL) return NULL;
	int iMinDepth=32000;
	mlRMMLElement* pMLElMinDepth = NULL;
	v_elems::iterator vi;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		if(pMLEl->GetButton()==NULL) continue;
		mlButtonUnderMouse oBUM;
		//mlRMMLVisible* pVisible = NULL;
		//mlRMMLElement* p3DObject = NULL;
		//mlRMMLElement* pTextureObject = NULL;
		//mlPoint pntLocalXY; ML_INIT_POINT(pntLocalXY);
		mlRMMLElement* pMLElBut=pMLEl->GetButtonUnderMouse(MouseXY, oBUM); // , &pVisible, &p3DObject, &pTextureObject, &pntLocalXY);
		if(pMLElBut){
			mlRMMLVisible* pMLElVis = pMLElBut->GetVisible();
			if(oBUM.mpVisible != NULL){
				pMLElVis = oBUM.mpVisible;
			}
			//if(pMLElVis == NULL){
			//	if(pVisible == NULL){
			//		if(pMLEl->mRMMLType == MLMT_VIEWPORT)
			//			pVisible = pMLEl->GetVisible();
			//	}
			//	pMLElVis = pVisible;
			//}
			ML_ASSERTION(mcx, pMLElVis!=NULL,"mlRMMLComposition::GetButtonUnderMouse");
			int iDepth = pMLElVis->GetAbsDepth();
			if(iDepth < iMinDepth){
				pMLElMinDepth = pMLElBut;
				aBUM = oBUM;
				//if(appVisible != NULL)
				//	*appVisible = pVisible;
				//if(app3DObject != NULL)
				//	*app3DObject = p3DObject;
				//if(appTextureObject != NULL)
				//	*appTextureObject = pTextureObject;
				//if(apLocalXY != NULL)
				//	*apLocalXY = pntLocalXY;
				iMinDepth=iDepth;
			}
		}
	}
//if(pMLElMinDepth != NULL){
//mlString s = pMLElMinDepth->GetSrcFileRef();
//int hh=0;
//}
	if(mbBtnEvListenersIsSet && (pMLElMinDepth != NULL && !(pMLElMinDepth->mbBtnEvListenersIsSet)))
		return this;
	return pMLElMinDepth;
}

mlRMMLElement* mlRMMLComposition::GetVisibleAt(const mlPoint aPnt, mlIVisible** appVisible){
	if(!mbVisible) return NULL;
	UpdateBounds();
	if(!ML_POINT_IN_RECT(aPnt,mBounds)) return NULL;
	mlPoint pnt = aPnt;
	pnt.x = aPnt.x - mXY.x;
	pnt.y = aPnt.y - mXY.y;
	//
	if(mpChildren == NULL) return NULL;
	int iMinDepth = 32000;
	mlRMMLElement* pMLElMinDepth = NULL;
	v_elems::iterator vi;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pMLEl = *vi;
		mlRMMLVisible* pVisible = pMLEl->GetVisible();
		if(pVisible == NULL || !pVisible->mbVisible) continue;
		mlRMMLElement* pMLElVisible = pVisible->GetVisibleAt(pnt, appVisible);
		if(pMLElVisible == NULL) continue;
		if(pMLElVisible->IsComposition()) continue;
		//int iDepth = pVisible->GetAbsDepth();
		int iDepth = 0;
		if( pMLElVisible->GetIVisible())
			iDepth = pMLElVisible->GetIVisible()->GetAbsDepth();
		if(iDepth <= iMinDepth){
			pMLElMinDepth = pMLElVisible;
			iMinDepth=iDepth;
		}
	}
	return pMLElMinDepth;
}

void mlRMMLComposition::SetMask(const mlMask* apMask){
	const mlMask* pMask = mpMask;
	if(apMask != NULL)
		pMask = apMask;
	if(pMask == NULL)
		return;
	if(mpChildren == NULL || mpChildren->empty()) return;
	mlPoint pntCompXY = GetAbsXY();
	v_elems::iterator vi;
	for(vi = mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pMLEl = *vi;
		if(!(pMLEl->IsVisible())) continue;
		mlRMMLVisible* pVis = pMLEl->GetVisible();
		if(pVis == NULL) continue;
		mlPoint pnt = pVis->GetAbsXY();
		if(pMask->meType == mlMask::MASKT_ELEM){
			pVis->SetMask(pMask->mpElem, pMask->miX - pnt.x, pMask->miY - pnt.y);
		}else if(pMask->meType == mlMask::MASKT_RECT){
			int iDX = pnt.x - pntCompXY.x;
			int iDY = pnt.y - pntCompXY.y;
			//int iLMX = aiX + iDX;
			//int iLMY = aiY + iDY;
			//int iLMW = aiW - iDX;
			//int iLMH = aiH - iDY;
			//pVis->SetMask(iLMX, iLMY, iLMW, iLMH);
			//pVis->SetMask(mpMask->miX, mpMask->miY, mpMask->miW, mpMask->miH);
			//if(pMLEl->mRMMLType == MLMT_COMPOSITION){
				//pVis->SetMask(mpMask->miX + iDX, mpMask->miY + iDY, mpMask->miW - iDX, mpMask->miH - iDY);
				//mlRMMLComposition* pCompos = (mlRMMLComposition*)pMLEl;
				mlMask oMask;
				oMask.meType = mlMask::MASKT_RECT;
				oMask.miX = pMask->miX;
				oMask.miY = pMask->miY;
				oMask.mbLocal = pMask->mbLocal;
				if(pMask->mbLocal){
					oMask.miX -= iDX;
					oMask.miY -= iDY;
				}
				oMask.miW = pMask->miW;
				oMask.miH = pMask->miH;
				pVis->SetMask(&oMask);
			//}else{
			//	pVis->SetMask(pMask->miX + pntCompXY.x, pMask->miY + pntCompXY.y, pMask->miW, pMask->miH);
			//}
//			pVis->SetMask(mpMask->miX, mpMask->miY, mpMask->miW, mpMask->miH);
		}
	}
}

//struct mlTimeout
//{
//	enum ETimeoutType{
//		TMOT_EVAL,
//		TMOT_FUNC,
//		TMOT_METHOD
//	} meType;
//	unsigned short musID;
//	mlString msStr;
//	JSObject* mjsoObj;
//	std::vector<jsval> mvArgs;
//	unsigned int miInterval;
//	bool mbInterval;
//	__int64 mlStartTime;
//	bool mbChecked;
//	mlTimeout(){
//		mjsoObj = NULL;
//		mlStartTime = 0;
//	}
//};

void mlRMMLComposition::GetState(mlSynchData &aData){
	aData << mRMMLType;
	// сохранить все timeout-ы
	unsigned short usCnt = mvTimeouts.size();
	aData << usCnt;
	std::vector<mlTimeout>::iterator vi;
	for(vi = mvTimeouts.begin(); vi!=mvTimeouts.end(); vi++){
		aData << (unsigned char)(vi->meType);
		aData << vi->musID;
		aData << vi->msStr.c_str();
		// mjsoObj
		if(vi->meType == mlTimeout::TMOT_FUNC){
			aData << vi->mjsoObj;
		}else if(vi->meType == mlTimeout::TMOT_METHOD){
			aData << vi->mjsoObj;
		}
		// mvArgs
		unsigned short usACnt = vi->mvArgs.size();
		aData << usACnt;
		std::vector<jsval>::iterator vi2;
		for(vi2 = vi->mvArgs.begin(); vi2 != vi->mvArgs.end(); vi2++){
			aData << *vi2;
		}
		aData << vi->miInterval;
		aData << vi->mbInterval;
		// __int64 mlStartTime;
		int iStartTime = (GPSM(mcx)->GetTime() - vi->mlStartTime)/1000;
		aData << iStartTime;
	}
	if(mRMMLType==MLMT_SCENE){
		GPSM(mcx)->GetSceneData(this,aData);
		// ??
	}
}

void mlRMMLComposition::SetState(mlSynchData &aData){	aData >> mRMMLType;
	// восстановить все timeout-ы
	ClearTimeouts();
	unsigned short usCnt;
	aData >> usCnt;
	for(unsigned short us = 0; us < usCnt; us++){
		mlTimeout timeout;
		unsigned char ucType;
		aData >> ucType; timeout.meType = (mlTimeout::ETimeoutType)ucType;
		aData >> timeout.musID;
		wchar_t* pwc = NULL;
		aData >> pwc;
		timeout.msStr = pwc;
		// mjsoObj
		if(timeout.meType == mlTimeout::TMOT_FUNC){
			aData >> timeout.mjsoObj;
		}else if(timeout.meType == mlTimeout::TMOT_METHOD){
			aData >> timeout.mjsoObj;
		}
		// mvArgs
		unsigned short usACnt;
		aData >> usACnt;
		for(unsigned short usA = 0; usA < usACnt; usA++){
			jsval vA;
			aData >> vA;
			timeout.mvArgs.push_back(vA);
		}
		aData >> timeout.miInterval;
		aData >> timeout.mbInterval;
		// __int64 mlStartTime;
		int iStartTime;
		aData >> iStartTime;
		timeout.mlStartTime = ((__int64)iStartTime)*1000 + GPSM(mcx)->GetTime();
		mvTimeouts.push_back(timeout);
	}
	if(mRMMLType==MLMT_SCENE){
		GPSM(mcx)->SetSceneData(this,aData);
		// ??
	}
}

JSBool mlRMMLComposition::JSFUNC_getBounds(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLComposition* pMLElThis = (mlRMMLComposition*)JS_GetPrivate(cx, obj);
	mlRect rcBounds; ML_INIT_RECT(rcBounds);
	// iDepthMin и iDepthMax определяем по глубинам всех видимых дочерних элементов
	int iAbsDepthMin = 0xFFFFFF;
	int iAbsDepthMax = -0xFFFFFF;
	int iCompAbsDepth = pMLElThis->GetAbsDepth();
	mlRMMLElement* pElem;
	mlRMMLIterator Iter(cx,pMLElThis);
	while((pElem = Iter.GetNext()) != NULL){
		if(pElem == pMLElThis) continue;
		mlRMMLVisible* pVis = pElem->GetVisible();
		if(pVis == NULL) continue;
		if(!pVis->mbVisible) continue;
		int iAbsDepth = pVis->GetAbsDepth();
		if(iAbsDepth < iAbsDepthMin) iAbsDepthMin = iAbsDepth;
		if(iAbsDepth > iAbsDepthMax) iAbsDepthMax = iAbsDepth;
	}
	if(argc == 0){
		rcBounds = pMLElThis->GetBounds();
		rcBounds.left -= pMLElThis->mXY.x;
		rcBounds.right -= pMLElThis->mXY.x;
		rcBounds.top -= pMLElThis->mXY.y;
		rcBounds.bottom -= pMLElThis->mXY.y;
	}else{
		if(argc > 1){
			mlTraceWarning(cx, "getBounds must get only one visible argument\n");
		}
		mlRMMLVisible* pArgVis = NULL;
		if(JSVAL_IS_OBJECT(argv[0])){
			JSObject* objArg=JSVAL_TO_OBJECT(argv[0]);
			if(JSO_IS_MLEL(cx, objArg)){
				mlRMMLElement* pMLElArg = (mlRMMLElement*)JS_GetPrivate(cx,objArg);
				pArgVis = pMLElArg->GetVisible();
			}
		}
		if(pArgVis == NULL){
			mlTraceError(cx, "getBounds must get a visible argument\n");
			return JS_TRUE;
		}else{
			iCompAbsDepth = pArgVis->GetAbsDepth();
			mlPoint pntTargetSpace = pArgVis->GetAbsXY();
			rcBounds = pMLElThis->GetAbsBounds();
			rcBounds.left -= pntTargetSpace.x;
			rcBounds.right -= pntTargetSpace.x;
			rcBounds.top -= pntTargetSpace.y;
			rcBounds.bottom -= pntTargetSpace.y;
		}
	}
	if( iAbsDepthMin == 0xFFFFFF)
	{
		iAbsDepthMin = iCompAbsDepth;
	}
	if( iAbsDepthMax == -0xFFFFFF)
	{
		iAbsDepthMax = iCompAbsDepth;
	}
	int iDepthMin = iAbsDepthMin - iCompAbsDepth;
	int iDepthMax = iAbsDepthMax - iCompAbsDepth;
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, obj, jso);
	JS_DefineProperty(cx,jso,"xMin",INT_TO_JSVAL(rcBounds.left),0,0,0);
	JS_DefineProperty(cx,jso,"xMax",INT_TO_JSVAL(rcBounds.right),0,0,0);
	JS_DefineProperty(cx,jso,"yMin",INT_TO_JSVAL(rcBounds.top),0,0,0);
	JS_DefineProperty(cx,jso,"yMax",INT_TO_JSVAL(rcBounds.bottom),0,0,0);
	JS_DefineProperty(cx,jso,"depthMin",INT_TO_JSVAL(iDepthMin),0,0,0);
	JS_DefineProperty(cx,jso,"depthMax",INT_TO_JSVAL(iDepthMax),0,0,0);
	*rval=OBJECT_TO_JSVAL(jso);
	FREE_FOR_GC(cx, obj, jso);
	return JS_TRUE;
}

JSBool mlRMMLComposition::JSFUNC_handleButtonEvents(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bHandle = true;
	if(argc > 0 && JSVAL_IS_BOOLEAN(argv[0])){
		bHandle = JSVAL_TO_BOOLEAN(argv[0]);
	}
	mlRMMLComposition* pMLElThis = (mlRMMLComposition*)JS_GetPrivate(cx, obj);
	pMLElThis->mbBtnEvListenersIsSet = bHandle;
	return JS_TRUE;
}

JSBool mlRMMLComposition::JSFUNC_disableChildButtons(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bDisable = true;
	if(argc > 0 && JSVAL_IS_BOOLEAN(argv[0])){
		bDisable = JSVAL_TO_BOOLEAN(argv[0]);
	}
	mlRMMLComposition* pMLElThis = (mlRMMLComposition*)JS_GetPrivate(cx, obj);
	pMLElThis->mbDisableChildButtons = bDisable;
	return JS_TRUE;
}


JSBool mlRMMLComposition::JSFUNC_setTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLComposition* pMLElThis = (mlRMMLComposition*)JS_GetPrivate(cx, obj);
	return pMLElThis->setTimeout(argc, argv, rval, false, "setTimeout");
}

JSBool mlRMMLComposition::JSFUNC_setInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLComposition* pMLElThis = (mlRMMLComposition*)JS_GetPrivate(cx, obj);
	return pMLElThis->setTimeout(argc, argv, rval, true, "setInterval");
}

JSBool mlRMMLComposition::JSFUNC_clearTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLComposition* pMLElThis = (mlRMMLComposition*)JS_GetPrivate(cx, obj);
	return pMLElThis->clearTimeout(argc, argv, rval, false, "clearTimeout");
}

JSBool mlRMMLComposition::JSFUNC_clearInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLComposition* pMLElThis = (mlRMMLComposition*)JS_GetPrivate(cx, obj);
	return pMLElThis->clearTimeout(argc, argv, rval, true, "clearInterval");
}

JSBool mlRMMLComposition::setTimeout(uintN argc, jsval *argv, jsval *rval, bool abInterval, const char* apcFuncName) {
	if(argc < 2){
		JS_ReportError(mcx, "%s function must get two arguments at least", apcFuncName);
		return JS_TRUE;
	}
	int iArg = 0;
	mlTimeout timeout;
	if(JSVAL_IS_STRING(argv[iArg])){
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		timeout.msStr = wr_JS_GetStringChars(jss);
		timeout.mjsoObj = NULL;
		timeout.meType = mlTimeout::TMOT_EVAL;
		iArg++;
	}else if(!JSVAL_IS_NULL(argv[iArg]) && !JSVAL_IS_VOID(argv[iArg]) && JSVAL_IS_OBJECT(argv[iArg])){
		JSObject* jso = JSVAL_TO_OBJECT(argv[iArg]);
		timeout.mjsoObj = jso;
		iArg++;
		if(JS_ObjectIsFunction(mcx, jso)){
			timeout.meType = mlTimeout::TMOT_FUNC;
		}else{
			if(!JSVAL_IS_STRING(argv[iArg])){
				JS_ReportError(mcx, "Second argument of %s function must be a method name", apcFuncName);
				return JS_TRUE;
			}
			JSString* jssMethod = JSVAL_TO_STRING(argv[iArg]);
			timeout.meType = mlTimeout::TMOT_METHOD;
			timeout.msStr = wr_JS_GetStringChars(jssMethod);
			iArg++;
			if(iArg == argc){
				JS_ReportError(mcx, "First argument of %s function must be a string, a function or an object", apcFuncName);
				return JS_TRUE;
			}
		}
	}else{
		JS_ReportError(mcx, "First argument of %s function must be a string, a function or an object", apcFuncName);
		return JS_TRUE;
	}
	timeout.miInterval = -1;
	if(iArg < (int)argc){
		if(!JSVAL_IS_INT(argv[iArg])){
			JS_ReportError(mcx, "interval argument of %s function must be an integer", apcFuncName);
			return JS_TRUE;
		}
		timeout.miInterval = JSVAL_TO_INT(argv[iArg]);
		iArg++;
	}
	if(mvTimeouts.size() >= 32767){
		JS_ReportError(mcx, "Number of timeout/interval for the composition is reached a limit", apcFuncName);
		return JS_FALSE;
	}
	if(timeout.meType == mlTimeout::TMOT_METHOD){
		// собираем аргументы функции
		for(; iArg < (int)argc; iArg++){
			jsval vArg = argv[iArg];
			timeout.mvArgs.push_back(vArg);
			SAVE_VAL_FROM_GC(mcx, mjso, vArg);
		}
	}
	timeout.mbInterval = abInterval;
	// подбираем уникальный ID
	timeout.musID = 1;
	std::vector<mlTimeout>::iterator vi;
	for(vi = mvTimeouts.begin(); vi!=mvTimeouts.end(); vi++){
		if(vi->musID >= timeout.musID){
			timeout.musID = vi->musID + 1;
		}
	}
	if(timeout.musID > 255 && mvTimeouts.size() < 255){
		std::vector<mlTimeout>::iterator vi;
		for(unsigned short us = 1; us <= 255; us++){
			for(vi = mvTimeouts.begin(); vi!=mvTimeouts.end(); vi++){
				if(vi->musID == us)
					break;
			}
			if(vi==mvTimeouts.end()){
				timeout.musID = us;
				break;
			}
		}
	}
	//
	timeout.mlStartTime = GPSM(mcx)->GetTime();
	timeout.mbChecked = false;
	if(timeout.mjsoObj != NULL)
		SAVE_FROM_GC(mcx, mjso, timeout.mjsoObj);
	mvTimeouts.push_back(timeout);
	GPSM(mcx)->AddUpdatableElement( this, true);
	*rval = INT_TO_JSVAL(timeout.musID);
	return JS_TRUE;
}

JSBool mlRMMLComposition::clearTimeout(uintN argc, jsval *argv, jsval *rval, bool abInterval, const char* apcFuncName) {
	if(argc < 1 || (!JSVAL_IS_INT(argv[0]))){
		JS_ReportError(mcx, "%s function must get one integer argument", apcFuncName);
		return JS_TRUE;
	}
	unsigned short usID = (unsigned short)JSVAL_TO_INT(argv[0]);
	std::vector<mlTimeout>::iterator vi;
	for(vi = mvTimeouts.begin(); vi!=mvTimeouts.end(); vi++){
		if(vi->musID == usID){
			FreeTimeoutForGC(*vi);
			mvTimeouts.erase(vi);
			bTimeoutsChanged = true;
			return JS_TRUE;
		}
	}
	return JS_TRUE;
}

void mlRMMLComposition::FreeTimeoutForGC(mlTimeout &aTimeout){
	if(aTimeout.mjsoObj != NULL)
		FREE_FOR_GC(mcx, mjso, aTimeout.mjsoObj);
	// освободить аргументы
	if(!aTimeout.mvArgs.empty()){
		std::vector<jsval>::iterator vi;
		for(vi = aTimeout.mvArgs.begin(); vi != aTimeout.mvArgs.end(); vi++){
			FREE_VAL_FOR_GC(mcx, mjso, *vi)
		}
	}
}

void mlRMMLComposition::ClearTimeouts(bool abDontFreeForGC){
	if(mvTimeouts.empty()) return;
	if(!abDontFreeForGC){
		std::vector<mlTimeout>::iterator vi;
		for(vi = mvTimeouts.begin(); vi!=mvTimeouts.end(); vi++){
			FreeTimeoutForGC(*vi);
		}
	}
	mvTimeouts.clear();
	bTimeoutsChanged = true;
}

mlresult mlRMMLComposition::Update(const __int64 alTime){
	mlresult res = mlRMMLElement::Update(alTime);
	if(mvTimeouts.empty())
		return res;
	// проверить все timeout-ы
	std::vector<mlTimeout>::iterator vi;
	for(vi = mvTimeouts.begin(); vi!=mvTimeouts.end(); vi++){
		vi->mbChecked = false;
	}
	bTimeoutsChanged = false;
	for(vi = mvTimeouts.begin(); vi!=mvTimeouts.end();){
		if(vi->mbChecked){
			vi++;
			continue;
		}
		vi->mbChecked = true;
		bool bCall = (vi->miInterval < 0);
		if(!bCall && ((alTime - vi->mlStartTime)/1000 >= vi->miInterval)){
			if(vi->mbInterval)
				vi->mlStartTime += ((__int64)vi->miInterval)*1000;
			bCall = true;
		}
		if(!bCall){
			vi++;
			continue;
		}
		unsigned short usCurTimeoutID = vi->musID;
		jsval v = JSVAL_NULL;
		switch(vi->meType){
		case mlTimeout::TMOT_EVAL:{
			char pcBuf[1000] = "interval/timeout";
			/*sprintf(pcBuf, "%s (ID=%d) of %S (%s: %d)", vi->mbInterval?"interval":"timeout", vi->musID,
				GetStringID().c_str(), cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]), miSrcLine);*/
			rmmlsafe_sprintf(pcBuf, sizeof(pcBuf), 0, "%s (ID=%d) of %S (%s: %d)", vi->mbInterval?"interval":"timeout", vi->musID,
				GetStringID().c_str(), cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]), miSrcLine);

			wr_JS_EvaluateUCScript(mcx, mjso, vi->msStr.c_str(), vi->msStr.length(), pcBuf, 1, &v);
			}break;
		case mlTimeout::TMOT_FUNC:
		case mlTimeout::TMOT_METHOD:{
			jsval vFunc = JSVAL_VOID;
			if(vi->meType == mlTimeout::TMOT_FUNC){
				vFunc = OBJECT_TO_JSVAL(vi->mjsoObj);
			}else{
				wr_JS_GetUCProperty(mcx, vi->mjsoObj, vi->msStr.c_str(), vi->msStr.length(), &vFunc);
				if(vFunc==JSVAL_VOID || !JSVAL_IS_OBJECT(vFunc) || !JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc))){
					mlTrace(mcx, "%S property is not a function (%s (ID=%d) of %S (%S: %d))\n", vi->msStr.c_str(),
						vi->mbInterval?"interval":"timeout", vi->musID,
						GetStringID().c_str(), GPSM(mcx)->mSrcFileColl[miSrcFilesIdx], miSrcLine);
					vFunc = JSVAL_VOID;
				}
			}
			jsval* argv = NULL;
			#if _MSC_VER > 1200
				argv = (jsval*)(*vi->mvArgs.begin());
			#else
				argv = (jsval*)vi->mvArgs.begin();
			#endif
			if(vFunc != JSVAL_VOID)
				JS_CallFunctionValue(mcx, mjso, vFunc, vi->mvArgs.size(), argv, &v);
			}break;
		}
		if(bTimeoutsChanged){
			// если timeout, который выполнялся еще остался
			for(vi = mvTimeouts.begin(); vi!=mvTimeouts.end(); vi++){
				if(vi->musID == usCurTimeoutID){
					// то если он был timeout-ом, а не interval-ом,то прибить его
					if(!vi->mbInterval)
						mvTimeouts.erase(vi);
					break;
				}
			}
			vi = mvTimeouts.begin();
			continue;
		}
		if(!vi->mbInterval)
			mvTimeouts.erase(vi);
		else
			vi++;
	}
	return ML_OK;
}

}
