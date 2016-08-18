#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include "ILogWriter.h"

namespace rmml {

mlRMMLVisible::mlRMMLVisible(void)
{
	ML_INIT_POINT(mXY);
	ML_INIT_SIZE(mSize);
	miDepth = 0;
	mbVisible = true;
	musOpacity = 0xFFFF;
	mpVisibleNotifications = NULL;
	mpMask = NULL;
}

mlRMMLVisible::~mlRMMLVisible(void)
{
	ClearMask(true);
	if(mpVisibleNotifications != NULL){
		 MP_DELETE( mpVisibleNotifications);
	}
}

mlRMMLVisible::mlVisibleNotifications::~mlVisibleNotifications(){
	// уведомить всех слушателей, что уведомитель удаляется
	mlVisibleNotifyListeners::const_iterator si = mVNListeners.begin();
	for(; si != mVNListeners.end(); si++){
		mlIVisibleNotifyListener* pListener = *si;
		pListener->VisibleDestroyed(mpThis);
	}
}

mlRMMLVisible::mlVisibleNotifications::mlVisibleNotifications(mlMedia* apThis){
	mbVisualRepresentationChanged = false;
	mpThis = apThis;
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLVisible::_JSPropertySpec[] = {
	JSPROP_RW(_x)
	JSPROPA_RW(x, _x)
	JSPROP_RW(_y)
	JSPROPA_RW(y, _y)
	JSPROP_RW(_width)
	JSPROPA_RW(width, _width)
	JSPROP_RW(_height)
	JSPROPA_RW(height, _height)
	JSPROP_RW(_depth)
	JSPROPA_RW(depth, _depth)
	JSPROP_RW(_visible)
	JSPROPA_RW(visible, _visible)
	JSPROP_RW(opacity)
	JSPROP_RO(_xmouse)
	JSPROPA_RO(xMouse, _xmouse)
	JSPROP_RO(_ymouse)
	JSPROPA_RO(yMouse, _ymouse)

	JSPROP_EV(onMouseDown)
	JSPROP_EV(onRMouseDown)
	JSPROP_EV(onMouseMove)
	JSPROP_EV(onRMouseUp)
	JSPROP_EV(onMouseUp)
	JSPROP_EV(onMouseWheel)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLVisible::_JSFunctionSpec[] = {
	JSFUNC(getMediaSize,0)
	JSFUNC(crop,1)
	JSFUNC(setMask,1)
	JSFUNC(show,0)
	JSFUNC(hide,0)
	JSFUNC(localToGlobal,1)
	JSFUNC(globalToLocal,1)
	JSFUNC(getAbsVisible,1)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLVisible::_EventSpec[] = {
	MLEVENT(onMouseDown)
	MLEVENT(onRMouseDown)
	MLEVENT(onMouseMove)
	MLEVENT(onRMouseUp)
	MLEVENT(onMouseUp)
	MLEVENT(onMouseWheel)
	{0, 0, 0}
};

ADDPROPFUNCSPECS_IMPL(mlRMMLVisible)
//mlresult mlRMMLVisible::AddPropFuncSpecs(JSPropertySpec* &pJSPropertySpec,JSFunctionSpec* &pJSFunctionSpec){
//	return mlJSClass::AddPropFuncSpecs(pJSPropertySpec, pJSFunctionSpec,
//										_JSPropertySpec, sizeof(_JSPropertySpec)/sizeof(JSPropertySpec)-1,
//										_JSFunctionSpec, sizeof(_JSFunctionSpec)/sizeof(JSFunctionSpec)-1);
//}

///// JavaScript Get Property Wrapper
JSBool mlRMMLVisible::GetJSProperty(int id, jsval *vp) {
	switch(id) {
		case JSPROP__x:
			*vp=INT_TO_JSVAL(mXY.x);
			break;
		case JSPROP__y:
			*vp=INT_TO_JSVAL(mXY.y);
			break;
		case JSPROP__width:
			*vp=INT_TO_JSVAL(GetSize().width);
			break;
		case JSPROP__height:
			*vp=INT_TO_JSVAL(GetSize().height);
			break;
		case JSPROP__depth:
			*vp=INT_TO_JSVAL(miDepth);
			break;
		case JSPROP__visible:
			*vp=BOOLEAN_TO_JSVAL(mbVisible);
			break;
		case JSPROP__xmouse:
			*vp=INT_TO_JSVAL(GetMouseXY().x);
			break;
		case JSPROP__ymouse:
			*vp=INT_TO_JSVAL(GetMouseXY().y);
			break;
		case JSPROP_opacity:
		{
			double	opacity = musOpacity/65535.0;
			jsdouble* jsdbl=JS_NewDouble(GetElem_mlRMMLVisible()->mcx, opacity);
			*vp=DOUBLE_TO_JSVAL(jsdbl);
		}	break;
//			*vp = __float_TO_JSVal(cx,priv->getX());
	}
	return JS_TRUE;
}

///// JavaScript Set Property Wrapper
JSBool mlRMMLVisible::SetJSProperty(JSContext* cx, int id, jsval *vp) {
	switch(id) {
		case JSPROP__x:
			ML_JSVAL_TO_INT(mXY.x,*vp);
			InvalidBounds();
//if(GetElem_mlRMMLVisible()->mRMMLType == MLMT_COMPOSITION)
//GetAbsBounds();
			AbsXYChanged();
			break;
		case JSPROP__y:
			ML_JSVAL_TO_INT(mXY.y,*vp);
			InvalidBounds();
			AbsXYChanged();
			break;
		case JSPROP__width:
			ML_JSVAL_TO_INT(mSize.width,*vp);
			InvalidBounds();
			SizeChanged();
			break;
		case JSPROP__height:
			ML_JSVAL_TO_INT(mSize.height,*vp);
			InvalidBounds();
			SizeChanged();
			break;
		case JSPROP__depth:
//if(JSVAL_IS_INT(*vp) && JSVAL_TO_INT(*vp)==-1)
//int hh=0;

			ML_JSVAL_TO_INT(miDepth,*vp);
			AbsDepthChanged();
			break;
		case JSPROP__visible:
//return JS_FALSE;
			ML_JSVAL_TO_BOOL_TDF(mbVisible,*vp);
//if(isEqual(GetElem_mlRMMLVisible()->GetName(), L"login")){
//int hh=0;
//}
			InvalidBounds();
			AbsVisibleChanged();
			break;
		case JSPROP_opacity:
		{
			double opacity;
			ML_JSVAL_TO_DOUBLE(opacity,*vp);
			if( opacity>=0.0 && opacity<=1.0)
			{
				musOpacity = (unsigned short)(opacity*0xFFFF);
				AbsOpacityChanged();
			}
			else
			{
				JS_ReportError(cx,"Incorrect bounds for opacity");
			}
		}	break;
		case EVID_onMouseDown:
		case EVID_onRMouseDown:
		case EVID_onMouseMove:
		case EVID_onRMouseUp:
		case EVID_onMouseUp:
		case EVID_onMouseWheel:
			if(JSVAL_IS_NULL(*vp)){
				GetElem_mlRMMLVisible()->RemoveEventListener(id, GetElem_mlRMMLVisible());
			}else{
				GetElem_mlRMMLVisible()->SetEventListener(id, GetElem_mlRMMLVisible());
			}
			{
			int iRes = GPSM(cx)->CheckEvent(id, GetElem_mlRMMLVisible(), vp);
			if(iRes >= 0)
				*vp = BOOLEAN_TO_JSVAL(iRes);
			}break;
			break;
	}
//return JS_FALSE;
	return JS_TRUE;
}

#define GetParent GetParent_mlRMMLVisible

mlPoint mlRMMLVisible::GetAbsXY(){
	mlRMMLElement* pParent=GetParent();
	mlPoint pnt; ML_INIT_POINT(pnt);
	if(pParent!=NULL && pParent->GetIVisible() ){
		pnt=pParent->GetIVisible()->GetAbsXY();
	}
	pnt.x+=mXY.x;
	pnt.y+=mXY.y;
	return pnt;
}

int mlRMMLVisible::GetAbsDepth(){
	mlRMMLElement* pParent=GetParent();
	int iDepth=0;
	if(pParent!=NULL && pParent->GetIVisible()){
		iDepth=pParent->GetIVisible()->GetAbsDepth();
	}
	return iDepth+miDepth;
}

bool mlRMMLVisible::GetAbsVisible(){
	//GPSM(cx)->GetContext()->mpLogWriter->WriteLn((const char *)cLPCSTR(GetElem_mlRMMLVisible()->GetName()));
	mlRMMLElement* pParent=GetParent();
	if(pParent != NULL){
		mlIVisible* pParentVis = pParent->GetIVisible();
		if(pParentVis != NULL){
			if(!pParentVis->GetAbsVisible())
			{
		//		GPSM(cx)->GetContext()->mpLogWriter->WriteLn("false");
				return false;
			}
		}
	}else{
		// если самый верхний родитель не является сценой,
		// то этот элемент считать невидимым
		if(!GetElem_mlRMMLVisible()->IsScene())
			return false;
	}
	//GPSM(cx)->GetContext()->mpLogWriter->WriteLn(mbVisible);
	return mbVisible;
}

float mlRMMLVisible::GetAbsOpacity(){
	mlRMMLElement* pParent=GetParent();
	float fOpacity=1.0f;
	if(pParent!=NULL && pParent->GetIVisible()){
		fOpacity=pParent->GetIVisible()->GetAbsOpacity();
	}
	return fOpacity*(musOpacity/65535.0);
}

//mlRect mlRMMLVisible::GetAbsBounds(mlRMMLElement* apMLEl){
////	mlRMMLElement* pParent=apMLEl->mpParent;
//	mlRect rc; 
//	mlPoint pnt=GetAbsXY(apMLEl);
//	rc.left=pnt.x; rc.top=pnt.y;
//	rc.right=rc.left+mSize.width;
//	rc.bottom=rc.top+mSize.height;
//	return rc;
//}

mlRect mlRMMLVisible::GetBounds(){
	mlRect rc; 
	rc.left=mXY.x; rc.top=mXY.y;
	rc.right=rc.left+mSize.width;
	rc.bottom=rc.top+mSize.height;
	return rc;
}

mlRect mlRMMLVisible::GetAbsBounds(){
	mlRMMLElement* pParent=GetParent();
	mlPoint pnt;
	if(pParent){
		pnt=pParent->GetIVisible()->GetAbsXY();
	}else{
		pnt=GetAbsXY(); // ??
	}
	mlRect rc=GetBounds();
	rc.left+=pnt.x;
	rc.right+=pnt.x;
	rc.top+=pnt.y;
	rc.bottom+=pnt.y;
	return rc;
}

void mlRMMLVisible::SetAbsXY(const mlPoint apntXY){
	int iNewX = apntXY.x;
	int iNewY = apntXY.y;
	mlRMMLElement* pParent=GetParent();
	if(pParent){
		mlPoint pnt = pParent->GetIVisible()->GetAbsXY();
		iNewX -= pnt.x;
		iNewY -= pnt.y;
	}
	mlRMMLElement* pMLEl =GetElem_mlRMMLVisible();
	bool bPosChanged = false;
	if(mXY.x != iNewX){
//		mXY.x = iNewX;
		jsval v = INT_TO_JSVAL(iNewX);
		JS_SetProperty(pMLEl->mcx, pMLEl->mjso, "x", &v);
		bPosChanged = true;
	}
	if(mXY.y != iNewY){
//		mXY.y = iNewY;
		jsval v = INT_TO_JSVAL(iNewY);
		JS_SetProperty(pMLEl->mcx, pMLEl->mjso, "y", &v);
		bPosChanged = true;
	}
//	if(bPosChanged)
//		AbsXYChanged();
//	InvalidBounds();
}

void mlRMMLVisible::SetAbsBounds(const mlRect arcBounds){
	mlRMMLElement* pMLEl = GetElem_mlRMMLVisible();
	if(pMLEl->IsComposition())
		return;
	int iNewWidth = arcBounds.right - arcBounds.left;
	int iNewHeight = arcBounds.bottom - arcBounds.top;
	bool bSizeChanged = false;
	if(mSize.width != iNewWidth){
//		mSize.width = iNewWidth;
		jsval v = INT_TO_JSVAL(iNewWidth);
		JS_SetProperty(pMLEl->mcx, pMLEl->mjso, "width", &v);
		bSizeChanged = true;
	}
	if(mSize.height != iNewHeight){
//		mSize.height = iNewHeight;
		jsval v = INT_TO_JSVAL(iNewHeight);
		JS_SetProperty(pMLEl->mcx, pMLEl->mjso, "height", &v);
		bSizeChanged = true;
	}
//	if(bSizeChanged)
//		SizeChanged();
//	InvalidBounds();
}


void mlRMMLVisible::InvalidBounds(){
//	if(mpMask != NULL && mpMask->mbLocal)
//		SetMask();
	mlRMMLElement* pParent=GetParent();
	if(pParent!=NULL){
		if(pParent->IsComposition())
			((mlRMMLComposition*)pParent)->InvalidBounds();
	}
}

mlSize mlRMMLVisible::GetSize(){ 
	mlRect rc=GetBounds();
	mSize.width=rc.right-rc.left;
	mSize.height=rc.bottom-rc.top;
	return mSize;
}

// выдает относительные координаты мыши
__forceinline mlPoint mlRMMLVisible::GetMouseXY(){ 
	mlPoint MXY = GPSM(GetElem_mlRMMLVisible()->mcx)->GetMouseXY();
	mlPoint AbsXY=GetAbsXY();
	MXY.x-=AbsXY.x;
	MXY.y-=AbsXY.y;
	return MXY;
}

bool mlRMMLVisible::Visible_IsPointInBoundingBox(const mlPoint aPnt){
	mlRect rc=GetBounds();
	return ML_POINT_IN_RECT(aPnt,rc);
}

bool mlRMMLVisible::Visible_IsPointIn(const mlPoint aPnt){
	mlRect rc=GetBounds();
	if(ML_POINT_IN_RECT(aPnt,rc)){
		mlPoint pnt = aPnt;
		pnt.x -= rc.left;
		pnt.y -= rc.top;
		return IsPointIn(pnt)==0?false:true;
	}
	return false;
}

void mlRMMLVisible::Visible_NotifyParentChanged(){
	AbsVisibleChanged();
	AbsDepthChanged();
}

JSBool mlRMMLVisible::JSFUNC_getMediaSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc > 0) return JS_FALSE;
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlSize sz=pMLEl->GetVisible()->GetMediaSize();
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, obj, jso);
	JS_DefineProperty(cx,jso,"width",INT_TO_JSVAL(sz.width),0,0,0);
	JS_DefineProperty(cx,jso,"height",INT_TO_JSVAL(sz.height),0,0,0);
	*rval=OBJECT_TO_JSVAL(jso);
	FREE_FOR_GC(cx, obj, jso);
	return JS_TRUE;
}

#define GET_RECT_OBJ_PROP(NM1,NM2,VL) \
		if((wr_JS_GetUCProperty(cx,objPar,NM1,-1,&v) && (v != JSVAL_VOID)) || wr_JS_GetUCProperty(cx,objPar,NM2,-1,&v)){ \
			int32 i32; \
			if(JS_ValueToInt32(cx,v,&i32)){ \
				VL=(short)i32; \
			}else{ \
				JS_ReportWarning(cx,"crop-function parameter object must have a numeric properties"); \
			} \
		}

#define GET_RECT_OBJ_PROP4(NM1, NM2, NM3, NM4, VL, PN) \
		if((wr_JS_GetUCProperty(cx,objPar,NM1,-1,&v) && (v != JSVAL_VOID)) || \
		   (wr_JS_GetUCProperty(cx,objPar,NM2,-1,&v) && (v != JSVAL_VOID)) || \
		   (wr_JS_GetUCProperty(cx,objPar,NM3,-1,&v) && (v != JSVAL_VOID)) || \
		    wr_JS_GetUCProperty(cx,objPar,NM4,-1,&v)  \
		){ \
			int32 i32; \
			if(JS_ValueToInt32(cx,v,&i32)){ \
				VL=(short)i32; \
			}else{ \
				JS_ReportWarning(cx,#PN "-function parameter object must have a numeric properties"); \
			} \
		}

#define GET_CROP_PARAM(ARG,VL) \
		if(JS_ValueToInt32(cx,ARG,&i32)){ \
			VL=(short)i32; \
		}else{ \
			JS_ReportWarning(cx,"crop-function parameter must be an integer"); \
		}

JSBool mlRMMLVisible::JSFUNC_crop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	short wX = 0, wY = 0;
	MLWORD wW = 0, wH = 0;
	if(argc == 0){
		// сброс обрезания
		mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
		mlSize sz = pMLEl->GetVisible()->GetMediaSize();
		wW = sz.width;
		wH = sz.height;
	}else if(argc==1 && JSVAL_IS_OBJECT(argv[0])){
		// аргументом является объект со свойствами x,y,w,h (или width,height)
		JSObject* objPar=JSVAL_TO_OBJECT(argv[0]);
		jsval v;
		GET_RECT_OBJ_PROP(L"x",L"X",wX)
		GET_RECT_OBJ_PROP(L"y",L"Y",wY)
		GET_RECT_OBJ_PROP(L"w",L"W",wW)
		GET_RECT_OBJ_PROP(L"h",L"H",wH)
		GET_RECT_OBJ_PROP(L"width",L"Width",wW)
		GET_RECT_OBJ_PROP(L"height",L"Height",wH)
	}else if(argc==4){
		// параметры функции - x,y,w,h
		int32 i32;
		GET_CROP_PARAM(argv[0],wX);
		GET_CROP_PARAM(argv[1],wY);
		GET_CROP_PARAM(argv[2],wW);
		GET_CROP_PARAM(argv[3],wH);
	}else{
		JS_ReportWarning(cx,"crop-function parameters must be four integer numbers");
	}
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	pMLEl->GetVisible()->Crop(wX, wY, wW, wH);
	return JS_TRUE;
}

void mlRMMLVisible::SetMask(const mlMask* apMask){
	const mlMask* pMask = mpMask;
	if(apMask != NULL)
		pMask = apMask;
	if(pMask == NULL)
		return;
	if(pMask->meType == mlMask::MASKT_ELEM){
		SetMask(pMask->mpElem);
	}else if(pMask->meType == mlMask::MASKT_RECT){
		mlPoint pnt = GetAbsXY();
		if(pMask->mbLocal)
			SetMask(pMask->miX + pnt.x, pMask->miY + pnt.y, pMask->miW, pMask->miH);
		else
			SetMask(pMask->miX, pMask->miY, pMask->miW, pMask->miH);
	}
}

// setMask(obj[,local])
JSBool mlRMMLVisible::JSFUNC_setMask(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 2){
		JS_ReportError(cx,"setMask-function argument must be an Image object or a Rect object");
		return JS_TRUE;
	}
	mlRMMLElement* pMLElMask=NULL;
	mlRect rcMask; ML_INIT_RECT(rcMask);
	bool bParError=false;
	if(argc >= 1){
		if(JSVAL_IS_OBJECT(argv[0])){
			JSObject* objMask=JSVAL_TO_OBJECT(argv[0]);
			if(JSO_IS_MLEL(cx, objMask)){
				pMLElMask=(mlRMMLElement*)JS_GetPrivate(cx,objMask);
				if(pMLElMask->mRMMLType != MLMT_IMAGE){
					pMLElMask=NULL;
					bParError=true;
				}
			}
			else
			{
				JSObject* objPar = objMask;
				MLWORD wX=0, wY=0, wW=0xFFFF, wH=0;
				jsval v;

				GET_RECT_OBJ_PROP(L"x",L"X",wX)
				GET_RECT_OBJ_PROP(L"y",L"Y",wY)
				GET_RECT_OBJ_PROP4(L"w",L"W",L"width",L"Width",wW, setMask);
				GET_RECT_OBJ_PROP4(L"h",L"H",L"height",L"Height",wH, setMask);

				if(wW == 0xFFFF){
					bParError=true;
				}else{
					rcMask.left = wX;
					rcMask.right = wX + wW;
					rcMask.top = wY;
					rcMask.bottom = wY + wH;
				}
			}
		}else bParError=true;
	}
	if(bParError){
		JS_ReportError(cx,"setMask-function parameter must be an Image object or Rect object");
		return JS_FALSE;
	}
	bool bLocal = false;
	if(argc >= 2){
		if(JSVAL_IS_BOOLEAN(argv[1])){
			bLocal = JSVAL_TO_BOOLEAN(argv[1]);
		}
	}
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLVisible* pVis = pMLEl->GetVisible();
	pVis->ClearMask();
	pVis->mpMask = MP_NEW( mlMask);
	mlMask* pMask = pVis->mpMask;
	pMask->mbLocal = bLocal;
	if (pMLElMask != NULL) {
		pMask->meType = mlMask::MASKT_ELEM;
		pMask->mpElem = pMLElMask;
		SAVE_FROM_GC(cx, obj, pMask->mpElem->mjso);
		mlPoint pntMask = pMLElMask->GetVisible()->GetAbsXY();
		pMask->miX = pntMask.x;
		pMask->miY = pntMask.y;
//		pMLEl->GetVisible()->SetMask(pMLElMask);
	} else {
		pMask->meType = mlMask::MASKT_RECT;
		pMask->miX = rcMask.left;
		pMask->miY = rcMask.top;
		pMask->miW = rcMask.right - rcMask.left;
		pMask->miH = rcMask.bottom - rcMask.top;
//		pMLEl->GetVisible()->SetMask(rcMask.left, rcMask.top, rcMask.right - rcMask.left, rcMask.bottom - rcMask.top);
	}
	pVis->SetMask();
	return JS_TRUE;
}

JSBool mlRMMLVisible::JSFUNC_show(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLVisible* pVis=pMLEl->GetVisible();
	pVis->Show();
	return JS_TRUE;
}

JSBool mlRMMLVisible::JSFUNC_hide(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLVisible* pVis=pMLEl->GetVisible();
	pVis->Hide();
	return JS_TRUE;
}

#define GET_POINT_OBJ_PROP(NM1,NM2,VL) \
		if(wr_JS_GetUCProperty(cx,objPar,NM1,0,&v) || wr_JS_GetUCProperty(cx,objPar,NM2,0,&v)){ \
			int32 i32; \
			if(JS_ValueToInt32(cx,v,&i32)){ \
				VL=i32; \
			}else{ \
				JS_ReportWarning(cx,"Method argument object must have a numeric properties"); \
			} \
		}

#define GET_LTG_PARAM(ARG,VL) \
		if(JS_ValueToInt32(cx,ARG,&i32)){ \
			VL = i32; \
		}else{ \
			JS_ReportWarning(cx,"Method argument must be an integer"); \
		}

JSBool mlRMMLVisible::JSFUNC_localToGlobal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLVisible* pVis=pMLEl->GetVisible();
	mlPoint pnt; ML_INIT_POINT(pnt);
	if(argc==1 && JSVAL_IS_OBJECT(argv[0])){
		// аргументом является объект со свойствами x,y
		JSObject* objPar=JSVAL_TO_OBJECT(argv[0]);
		jsval v;
		GET_POINT_OBJ_PROP(L"x",L"X", pnt.x)
		GET_POINT_OBJ_PROP(L"y",L"Y", pnt.y)
	}else if(argc==2){
		// параметры функции - x,y
		int32 i32;
		GET_LTG_PARAM(argv[0], pnt.x);
		GET_LTG_PARAM(argv[1], pnt.y);
	}else{
		JS_ReportWarning(cx,"localToGlobal-method arguments must be two integer numbers");
	}
	pVis->LocalToGlobal(pnt);
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	JS_DefineProperty(cx,jso,"x",INT_TO_JSVAL(pnt.x),0,0,0);
	JS_DefineProperty(cx,jso,"y",INT_TO_JSVAL(pnt.y),0,0,0);
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSBool mlRMMLVisible::JSFUNC_getAbsVisible(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLVisible* pVis=pMLEl->GetVisible();
	*rval=BOOLEAN_TO_JSVAL( pVis->GetAbsVisible());
	return JS_TRUE;
}

JSBool mlRMMLVisible::JSFUNC_globalToLocal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLVisible* pVis=pMLEl->GetVisible();
	mlPoint pnt; ML_INIT_POINT(pnt);
	if(argc==1 && JSVAL_IS_OBJECT(argv[0])){
		// аргументом является объект со свойствами x,y
		JSObject* objPar=JSVAL_TO_OBJECT(argv[0]);
		jsval v;
		GET_POINT_OBJ_PROP(L"x",L"X", pnt.x)
			GET_POINT_OBJ_PROP(L"y",L"Y", pnt.y)
	}else if(argc==2){
		// параметры функции - x,y
		int32 i32;
		GET_LTG_PARAM(argv[0], pnt.x);
		GET_LTG_PARAM(argv[1], pnt.y);
	}else{
		JS_ReportWarning(cx,"globalToLocal-method arguments must be two integer numbers");
	}
	pVis->GlobalToLocal(pnt);
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	JS_DefineProperty(cx,jso,"x",INT_TO_JSVAL(pnt.x),0,0,0);
	JS_DefineProperty(cx,jso,"y",INT_TO_JSVAL(pnt.y),0,0,0);
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

void mlRMMLVisible::LocalToGlobal(mlPoint &aPnt){
	mlPoint pnt = GetAbsXY();
	aPnt.x += pnt.x;
	aPnt.y += pnt.y;
}

void mlRMMLVisible::GlobalToLocal(mlPoint &aPnt){
	mlPoint pnt = GetAbsXY();
	aPnt.x -= pnt.x;
	aPnt.y -= pnt.y;
}

// реализация moIVisible

moIVisible* mlRMMLVisible::GetmoIVisible(){
	moMedia* pMO=GetElem_mlRMMLVisible()->GetMO();
	if(PMO_IS_NULL(pMO)) return NULL;
	return pMO->GetIVisible();
}

#define MOIVIS_EVENT_CALL(E) \
	moIVisible* pmoIVisible=GetmoIVisible(); \
	if(pmoIVisible) pmoIVisible->##E(); 

#define MOIVIS_EVENT_IMPL2(E) \
void mlRMMLVisible::##E(){ \
	MOIVIS_EVENT_CALL(E); \
	mlRMMLElement* pMLEl=GetElem_mlRMMLVisible(); \
	if(mpMask != NULL && mpMask->mbLocal) \
		SetMask(); \
	if(pMLEl->IsComposition()){ \
		pv_elems pChildren=pMLEl->mpChildren; \
		if(pChildren!=NULL){ \
			v_elems::iterator vi; \
			for(vi=pChildren->begin(); vi != pChildren->end(); vi++ ){ \
				mlRMMLElement* pMLEl=*vi; \
				mlRMMLVisible* pVis=pMLEl->GetVisible(); \
				if(pVis!=NULL) pVis->##E(); \
			} \
		} \
	} \
	pMLEl->VisualRepresentationChanged(); \
}

#define MOIVIS_EVENT_IMPL(E) \
void mlRMMLVisible::##E(){ \
	MOIVIS_EVENT_CALL(E); \
	GetElem_mlRMMLVisible()->VisualRepresentationChanged(); \
}

MOIVIS_EVENT_IMPL2(AbsXYChanged);
MOIVIS_EVENT_IMPL(SizeChanged);
MOIVIS_EVENT_IMPL2(AbsDepthChanged);
MOIVIS_EVENT_IMPL2(AbsVisibleChanged);
MOIVIS_EVENT_IMPL2(AbsOpacityChanged);

mlSize mlRMMLVisible::GetMediaSize(){
	moIVisible* pmoIVisible=GetmoIVisible();
	if(pmoIVisible) return pmoIVisible->GetMediaSize(); 
	mlSize size; ML_INIT_SIZE(size);
	return size;
}

void mlRMMLVisible::Crop(short awX, short awY, MLWORD awWidth, MLWORD awHeight){
	moIVisible* pmoIVisible=GetmoIVisible();
	if(pmoIVisible) pmoIVisible->Crop(awX, awY, awWidth, awHeight); 
}

mlRect mlRMMLVisible::GetCropRect(){
	moIVisible* pmoIVisible=GetmoIVisible();
	if(pmoIVisible) return pmoIVisible->GetCropRect(); 
	mlRect rect; ML_INIT_RECT(rect);
	return rect;
}

void mlRMMLVisible::SetMask(mlMedia* aMask, int aiX, int aiY){
	moIVisible* pmoIVisible=GetmoIVisible();
	if(pmoIVisible) pmoIVisible->SetMask(aMask, aiX, aiY); 
}

void mlRMMLVisible::SetMask(int aiX, int aiY, int aiW, int aiH){
	moIVisible* pmoIVisible=GetmoIVisible();
	if(pmoIVisible) pmoIVisible->SetMask(aiX, aiY, aiW, aiH); 
}

// -1 - not handled, 0 - point is out, 1 - point is in
int mlRMMLVisible::IsPointIn(const mlPoint aPnt){
	moIVisible* pmoIVisible=GetmoIVisible();
	if(pmoIVisible) return pmoIVisible->IsPointIn(aPnt); 
	return -1;
}

void mlRMMLVisible::Show(){
	if(mbVisible) return;
	mbVisible=true;
	InvalidBounds();
	AbsVisibleChanged();
	mlRMMLElement::PropIsSet(GetElem_mlRMMLVisible(), JSPROP__visible);
}

void mlRMMLVisible::Hide(){
	if(!mbVisible) return;
	mbVisible=false;
	InvalidBounds();
	AbsVisibleChanged();
	mlRMMLElement::PropIsSet(GetElem_mlRMMLVisible(), JSPROP__visible);
}

// поддержка синхронизации 

void mlRMMLVisible::Vis_GetData4Synch(JSContext* cx, char aidEvent, mlSynchData &avSynchData){
	if(aidEvent <= EVID_mlRMMLVisible || aidEvent >= TEVIDE_mlRMMLVisible)
		return;
	// сохраняем положение мыши
	mlPoint MXY = GPSM(cx)->GetMouseXY();
	avSynchData.put(&MXY,sizeof(mlPoint));
}

void mlRMMLVisible::Vis_SetData4Synch(JSContext* cx, char aidEvent, mlSynchData &avSynchData){
	if(aidEvent <= EVID_mlRMMLVisible || aidEvent >= TEVIDE_mlRMMLVisible)
		return;
	// устанавливаем положение мыши
	mlPoint MXY;
	avSynchData.get(&MXY,sizeof(mlPoint));
	GPSM(cx)->SetMouseXY(MXY);
}
/*
void mlRMMLVisible::Vis_SetData4Synch(char aidEvent, unsigned char* apSynchData, int &aiIdx){
	if(aidEvent <= EVID_mlRMMLVisible || aidEvent >= TEVIDE_mlRMMLVisible)
		return;
	// устанавливаем положение мыши
	mlPoint MXY;
	get_int(apSynchData, aiIdx, MXY.x);
	get_int(apSynchData, aiIdx, MXY.y);
	gpSM->SetMouseXY(MXY);
}
*/

void mlRMMLVisible::Vis_VisualRepresentationChanged(){
	if(mpVisibleNotifications == NULL)
		return;
	mpVisibleNotifications->mbVisualRepresentationChanged = true;
	// mucVisualRepresentationChanged |= MLVRFL_THIS_UPDATE;
}

mlresult mlRMMLVisible::UpdateVisualRepresentationChanged(){
	if(mpVisibleNotifications == NULL)
		return ML_ERROR_NOT_IMPLEMENTED;
	if(!mpVisibleNotifications->mbVisualRepresentationChanged)
		return ML_OK;
	mpVisibleNotifications->NotifyVisualRepresentationChanged();
	//if(mucVisualRepresentationChanged == 0)
	//	return;
	//// если были изменения на этом update-е, 
	//if(mucVisualRepresentationChanged & MLVRFL_THIS_UPDATE){
	//	// то установить флаг для RenderManager-а, а флаг этого update-а сбросить
	//	mucVisualRepresentationChanged = MLVRFL_LAST_UPDATE;
	//	NotifyVisualRepresentationChanged(); // пока MLVRFL_LAST_UPDATE реально не нужен
	//}else{
	//	// если на прошлом update-е изменения были, 
	//	// а на этом нет, то сбросить флаг для RenderManager-а
	//	mucVisualRepresentationChanged = 0;
	//}
	return ML_OK;
}

void mlRMMLVisible::SetVListener(mlIVisibleNotifyListener* apListener){
	if(mpVisibleNotifications == NULL){
		MP_NEW_V( mpVisibleNotifications, mlVisibleNotifications, GetElem_mlRMMLVisible());
	}
	mpVisibleNotifications->mVNListeners.insert(apListener);
}

void mlRMMLVisible::RemoveVListener(mlIVisibleNotifyListener* apListener){
	if(mpVisibleNotifications == NULL)
		return;
	mpVisibleNotifications->mVNListeners.erase(apListener);
}

void mlRMMLVisible::mlVisibleNotifications::NotifyVisualRepresentationChanged(){
	mlVisibleNotifyListeners::const_iterator si = mVNListeners.begin();
	for(; si != mVNListeners.end(); si++){
		mlIVisibleNotifyListener* pListener = *si;
		if(pListener != NULL)
			pListener->VisualRepresentationChanged(mpThis);
	}
	mbVisualRepresentationChanged = false;
}

// обрабатывает уведомление от mlRMMLElement о том, что parent был не null, а стал null
//void mlRMMLVisible::ParentNulled(){
//	// возможно объект выпал из сцены: пусть станет невидимым
//	InvalidBounds();
//	AbsVisibleChanged();
//}

void mlRMMLVisible::ClearMask(bool abDestroying){
	if(mpMask != NULL){
		if(mpMask->mpElem != NULL){
			if(!abDestroying){
				mlRMMLElement* pMLEl = GetElem_mlRMMLVisible();
				FREE_FOR_GC(pMLEl->mcx, pMLEl->mjso, mpMask->mpElem->mjso);
			}
		}
		 MP_DELETE( mpMask);
	}
	mpMask = NULL;
}

void mlRMMLVisible::Vis_SetVisibleEventData(char aidEvent){
	GPSM(GetElem_mlRMMLVisible()->mcx)->SetTextureXYToEventGO();
}

}