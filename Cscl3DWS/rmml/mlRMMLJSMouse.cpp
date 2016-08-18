#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "Mouse"
 */

#define JSMC_MASK 0x40

#define JSMCONST(NM,VL) \
	{ #NM, VL | JSMC_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},

enum{
	JSPROP_bigCursor = 1,
	JSPROP_x,
	JSPROP_y,
	JSPROP_target,
	JSPROP_modalButtons,
	JSPROP_skipMoves,
	JSPROP_skipedMoves
};

JSPropertySpec _JSMousePropertySpec[] = {
	JSPROP_RW(bigCursor)
	JSPROP_RO(x)
	JSPROP_RO(y)
	JSPROP_RO(target)
	JSPROP_RO(modalButtons)
	JSPROP_RO(skipMoves)	// пропускать или нет события MouseMove (по умолчанию пропускать) (рекомендуется выключать только для реализации рисования и подобных функций)
	JSPROP_RO(skipedMoves)	// массив координат пропущенных событий MouseMove в формате [x1,y1, x2,y2, ...]
	JSMCONST(NO, oms::NoCursor)
	JSMCONST(ARROW, oms::NormalCursor)
	JSMCONST(HAND, oms::ActiveCursor)
	JSMCONST(WAIT, oms::WaitCursor)
	JSMCONST(BIG_ARROW, oms::NormalCursor | CT_BIG_CURSOR_MASK)
	JSMCONST(BIG_HAND, oms::ActiveCursor | CT_BIG_CURSOR_MASK)
	JSMCONST(BIG_WAIT, oms::WaitCursor | CT_BIG_CURSOR_MASK)
	JSMCONST(BEAM, oms::BeamCursor)
	JSMCONST(DRAG, oms::DragCursor)
	JSMCONST(SIZE1, oms::Size1Cursor)
	JSMCONST(SIZE2, oms::Size2Cursor)
	JSMCONST(SIZE3, oms::Size3Cursor)
	JSMCONST(SIZE4, oms::Size4Cursor)
	// для режима редактирования
	JSMCONST(CREATE, oms::CreateCursor)	// для создания объектов
	JSMCONST(COPY, oms::CopyCursor)	// для создания объектов
	JSMCONST(DELETE, oms::DeleteCursor)	// для удаления объектов
	JSMCONST(MOVE_3D_PLANE, oms::Move3DPlaneCursor) // для перемещения по земле
	JSMCONST(MOVE_3D_PLANE_IMPOSSIBILITY, oms::Move3DPlaneImpossibilityCursor) // для запрета перемещения по земле
	JSMCONST(ROTATE_3D_PLANE, oms::Rotate3DPlaneCursor) // для вращения в горизонтали земли
	JSMCONST(MOVE_3D, oms::Move3DCursor) // для перемещения в 3D пространстве
	JSMCONST(ROTATE_3D, oms::Rotate3DPlaneCursor) // для вращения в 3D пространстве
	JSMCONST(ROTATE_3D_IMPOSSIBILITY, oms::Rotate3DImpossibilityCursor) // для показа запрещающей индикации вращения объектов
	JSMCONST(SELECT_3D, oms::Select3DCursor) // для выделения объектов в 3D пространстве
	JSMCONST(PLACE_3D, oms::Place3DCursor) // для помещения объектов на поверхность в 3D пространстве
	JSMCONST(PLACE_3D_IMPOSSIBILITY, oms::Place3DImpossibilityCursor) // для показа запрещающей индикации помещения объектов на поверхность в 3D пространстве
	JSMCONST(COPY_3D, oms::Copy3DCursor) // для дублирования объектов в 3D пространстве
	JSMCONST(SCALE_3D, oms::Scale3DCursor) // для масштабирования объектов в 3D пространстве
	JSMCONST(SCALE_3D_IMPOSSIBILITY, oms::Scale3DImpossibilityCursor) // для масштабирования объектов в 3D пространстве
	JSMCONST(SITPLACE_UPDOWN, oms::SitplaceUpDown3DCursor) // для размещения сидячих мест
	JSMCONST(WB_PEN, oms::PenCursor) // карандаш для доски
	JSMCONST(WB_ERASE_2, oms::EraseCursorSize2) // ластик для доски
	JSMCONST(WB_ERASE_4, oms::EraseCursorSize4)
	JSMCONST(WB_ERASE_6, oms::EraseCursorSize6)
	JSMCONST(WB_ERASE_8, oms::EraseCursorSize8)
	JSMCONST(WB_ERASE_10, oms::EraseCursorSize10)
	JSMCONST(WB_ERASE_12, oms::EraseCursorSize12) 
	JSMCONST(WB_ERASE_16, oms::EraseCursorSize16) 
	JSMCONST(WB_ERASE_20, oms::EraseCursorSize20) 
	JSMCONST(WB_ERASE_26, oms::EraseCursorSize26) 
	JSMCONST(WB_ERASE_32, oms::EraseCursorSize32) 
	JSMCONST(TEXT_STICKER1, oms::TextStickerCursor1) 
	JSMCONST(TEXT_STICKER2, oms::TextStickerCursor2) 
	JSMCONST(TEXT_STICKER3, oms::TextStickerCursor3) 
	JSMCONST(TEXT_STICKER4, oms::TextStickerCursor4) 
	JSMCONST(TEXT_STICKER5, oms::TextStickerCursor5) 
	JSMCONST(TEXT_STICKER6, oms::TextStickerCursor6) 
	JSMCONST(TEXT_STICKER7, oms::TextStickerCursor7) 
	JSMCONST(TEXT_STICKER8, oms::TextStickerCursor8) 
	JSMCONST(TEXT_STICKER9, oms::TextStickerCursor9) 
	JSMCONST(TEXT_STICKER10, oms::TextStickerCursor10) 
	JSMCONST(IMAGE_STICKER1, oms::ImageStickerCursor1) 
	JSMCONST(IMAGE_STICKER2, oms::ImageStickerCursor2)
	JSMCONST(IMAGE_STICKER3, oms::ImageStickerCursor3)
	JSMCONST(IMAGE_STICKER4, oms::ImageStickerCursor4)
	JSMCONST(IMAGE_STICKER5, oms::ImageStickerCursor5)
	JSMCONST(IMAGE_STICKER6, oms::ImageStickerCursor6) 
	JSMCONST(IMAGE_STICKER7, oms::ImageStickerCursor7)
	JSMCONST(IMAGE_STICKER8, oms::ImageStickerCursor8)
	JSMCONST(IMAGE_STICKER9, oms::ImageStickerCursor9)
	JSMCONST(IMAGE_STICKER10, oms::ImageStickerCursor10)
	JSMCONST(HEAL_CURSOR, oms::HealCursor)
	JSMCONST(FIREOFF_CURSOR, oms::FireOffCursor)
	JSMCONST(REMOVEBOX_CURSOR, oms::RemoveBoxCursor)
	JSMCONST(STRETCHER_CURSOR, oms::StretcherCursor)
	JSMCONST(DRAGPEOPLE_CURSOR, oms::DragPeople)
	JSMCONST(TRIAGE_CURSOR, oms::TriageCursor)
	{ 0, 0, 0, 0, 0 }
};

JSBool JSMouseSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id)){
		int iID = JSVAL_TO_INT(id);
		switch(iID){
		case JSPROP_bigCursor:{
			JSBool bVal = JSVAL_FALSE;
			if(JS_ValueToBoolean(cx, *vp, &bVal))
				GPSM(cx)->SetBigCursor(bVal);
			}break;
		case JSPROP_skipMoves:{
			JSBool bVal = JSVAL_FALSE;
			if(JS_ValueToBoolean(cx, *vp, &bVal))
				GPSM(cx)->GetContext()->mpInput->SetSkipMouseMovesMode(bVal);
			}break;
		}
	}
	return JS_TRUE;
}

bool bNullifyRefProps = false;

JSBool JSMouseGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id)){
		int iID = JSVAL_TO_INT(id);
		if(iID & JSMC_MASK){
			*vp = INT_TO_JSVAL(iID & ~JSMC_MASK);
			return JS_TRUE;
		}
		switch(iID){
		case JSPROP_bigCursor:{
			*vp = BOOLEAN_TO_JSVAL(GPSM(cx)->IsCursorBig());
			}break;
		case JSPROP_x:{
			*vp = INT_TO_JSVAL(GPSM(cx)->GetMouseXY().x);
			}break;
		case JSPROP_y:{
			*vp = INT_TO_JSVAL(GPSM(cx)->GetMouseXY().y);
			}break;
		case JSPROP_target:{
			*vp = JSVAL_NULL;
			if(!bNullifyRefProps){
				mlPoint pnt = GPSM(cx)->GetMouseXY();
				mlRMMLElement* pMLEl = GPSM(cx)->GetVisibleAt(pnt);
				if(pMLEl != NULL)
					*vp = OBJECT_TO_JSVAL(pMLEl->mjso);
			}
			}break;
		case JSPROP_modalButtons:{
			*vp = JSVAL_NULL;
			if(!bNullifyRefProps){
				int iLen = GPSM(cx)->mlModalButs.size();
				jsval* pjsvals = MP_NEW_ARR( jsval, iLen+1);
				std::list<mlSceneManager::mlModalButton>::iterator li;
				int ii = 0;
				for(li = GPSM(cx)->mlModalButs.begin(); li != GPSM(cx)->mlModalButs.end(); li++, ii++){
					mlSceneManager::mlModalButton& but = *li;
					mlRMMLElement* pMLEl = but.mpButton;
					pjsvals[ii] = OBJECT_TO_JSVAL(pMLEl->mjso);
				}
				JSObject* jso = JS_NewArrayObject(cx,iLen,pjsvals);
				MP_DELETE_ARR( pjsvals);
				*vp = OBJECT_TO_JSVAL(jso);
			}
			}break;
		case JSPROP_skipMoves:{
			*vp = BOOLEAN_TO_JSVAL(GPSM(cx)->GetContext()->mpInput->GetSkipMouseMovesMode());
			}break;
		case JSPROP_skipedMoves:{
			*vp = JSVAL_NULL;
			int iCnt = GPSM(cx)->mvSkipedMouseMoves.size();
			jsval* pjsvals = MP_NEW_ARR( jsval, iCnt + 1);
			std::vector<unsigned short>::const_iterator cvi = GPSM(cx)->mvSkipedMouseMoves.begin();
			for(int i = 0; cvi != GPSM(cx)->mvSkipedMouseMoves.end(); cvi++, i++){
				pjsvals[i] = INT_TO_JSVAL(*cvi);
			}
			JSObject* jso = JS_NewArrayObject(cx, iCnt, pjsvals);
			MP_DELETE_ARR( pjsvals);
			*vp = OBJECT_TO_JSVAL(jso);
			}break;
		}
	}
	return JS_TRUE;
}

JSClass JSRMMLMouseClass = {
	"_Mouse", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSMouseGetProperty, JSMouseSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

__forceinline int GetMouseEventIdx(char aidEvent){
	switch(aidEvent){
	case 0:
		return 0;
	case mlRMMLVisible::EVID_onMouseMove:
	case mlRMML3DObject::EVID_onMouseMove:
	case mlRMMLButton::EVID_onRollOver:
	case mlRMMLButton::EVID_onRollOut:
		return 1;
	case mlRMMLVisible::EVID_onMouseDown:
	case mlRMML3DObject::EVID_onMouseDown:
	case mlRMMLButton::EVID_onPress:
		return 2;
	case mlRMMLVisible::EVID_onMouseUp:
	case mlRMML3DObject::EVID_onMouseUp:
	case mlRMMLButton::EVID_onRelease:
	case mlRMMLButton::EVID_onReleaseOutside:
		return 3;
	case mlRMMLVisible::EVID_onRMouseDown:
	case mlRMML3DObject::EVID_onRMouseDown:
		return 5;
	case mlRMMLVisible::EVID_onRMouseUp:
	case mlRMML3DObject::EVID_onRMouseUp:
		return 6;
	case mlRMMLVisible::EVID_onMouseWheel:
		return 7;
	case JSPROP_modalButtons:
		return 8;
	}
	return 0;
}

void mlSceneManager::NotifyMouseListeners(char chEvID){
	v_elems* pv=&(maMouseListeners[GetMouseEventIdx(chEvID)]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Mouse);
	}
}

void mlSceneManager::NotifyMouseModalButtons(){
	NotifyMouseListeners(JSPROP_modalButtons);
}

char GetMouseEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"modalButtons"))
		return (char)JSPROP_modalButtons;
	return '\0';
}

wchar_t* GetMouseEventName(char aidEvent){
	switch(aidEvent){
	case (char)JSPROP_modalButtons:
		return L"modalButtons";
	}
	return L"???";
}

bool mlSceneManager::addMouseListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	v_elems* pv=&(maMouseListeners[GetMouseEventIdx(aidEvent)]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

JSBool JSFUNC_addMouseListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	mlRMMLElement* pMLEl=GET_RMMLEL(cx,JSVAL_TO_OBJECT(argv[0]));
	if(pSM->addMouseListener(0,pMLEl)) return JS_TRUE;
	return JS_FALSE;
}

bool mlSceneManager::removeMouseListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		if(mpButUnderMouse == apMLEl){
			mpButUnderMouse = (mlRMMLElement*)1;
			if (mpBUMParents != NULL)
				MP_DELETE(mpBUMParents);
		}
		if(mpButPressed == apMLEl){ mpButPressed = NULL; if (mpBPParents != NULL) MP_DELETE(mpBPParents); }
		if(mpButRPressed == apMLEl){ mpButRPressed = NULL; if(mpBRPParents != NULL) MP_DELETE(mpBRPParents); }
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_MOUSE_EVENTS_COUNT; ii++){
			v_elems* pv=&maMouseListeners[ii];
			v_elems::iterator vi;
			for(vi=pv->begin(); vi != pv->end(); vi++ ){
				if(*vi==apMLEl){
					pv->erase(vi);
					break;
				}
			}
		}
		return true;
	}
	v_elems* pv=&(maMouseListeners[GetMouseEventIdx(aidEvent)]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

JSBool JSFUNC_removeMouseListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc < 1 || !JSVAL_IS_REAL_OBJECT(argv[0])){
		mlTraceWarning(cx, "Mouse.removeMouseListener must get a RMML-element\n");
		return JS_TRUE;
	}
	mlRMMLElement* pMLEl = GET_RMMLEL(cx, JSVAL_TO_OBJECT(argv[0]));
	if(!pSM->removeMouseListener(0, pMLEl)){
		mlTraceWarning(cx, "Mouse.removeMouseListener hasn't work for unknown reason\n");
	}
	return JS_TRUE;
}

JSBool JSFUNC_addMouseMoveListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	mlRMMLElement* pMLEl = GET_RMMLEL(cx,JSVAL_TO_OBJECT(argv[0]));
	if(pSM->addMouseListener(mlRMMLVisible::EVID_onMouseMove, pMLEl)) return JS_TRUE;
	return JS_FALSE;
}

JSBool JSFUNC_removeMouseMoveListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	mlRMMLElement* pMLEl=GET_RMMLEL(cx,JSVAL_TO_OBJECT(argv[0]));
	if(pSM->removeMouseListener(mlRMMLVisible::EVID_onMouseMove,pMLEl)) return JS_TRUE;
	return JS_FALSE;
}

JSBool JSFUNC_hideMouse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->hideMouse();
	return JS_TRUE;
}

JSBool JSFUNC_showMouse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->showMouse();
	return JS_TRUE;
}

JSBool JSFUNC_setCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc > 0){
		int iCursor = oms::NormalCursor;
		double dCursor = 0;
		if(JS_ValueToNumber(cx, argv[0], &dCursor )){
			iCursor = (int)dCursor;
		}
		pSM->miCursorType = iCursor;
	}else{
		pSM->miCursorType = oms::NormalCursor;
	}
	pSM->ForceSetCursor();
	return JS_TRUE;
}

JSBool JSFUNC_setHandCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(argc > 0){
		int iCursor = oms::ActiveCursor;
		double dCursor = 0;
		if(JS_ValueToNumber(cx, argv[0], &dCursor )){
			iCursor = (int)dCursor;
		}
		pSM->miHandCursorType = iCursor;
	}else{
		pSM->miHandCursorType = oms::ActiveCursor;
	}
	pSM->ForceSetCursor();
	return JS_TRUE;
}

JSBool JSFUNC_mouseLeftDown(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = INT_TO_JSVAL(0);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	//::SetCapture(ghWnd);
	//pSM->OnAppActivated();
	return pSM->SendMouseEvent(WM_LBUTTONDOWN, MK_LBUTTON, argc, argv, rval);
}

JSBool JSFUNC_mouseLeftUp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = INT_TO_JSVAL(0);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	return pSM->SendMouseEvent(WM_LBUTTONUP, MK_LBUTTON, argc, argv, rval);	
}

JSBool JSFUNC_mouseRightDown(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = INT_TO_JSVAL(0);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	return pSM->SendMouseEvent(WM_RBUTTONDOWN, MK_RBUTTON, argc, argv, rval);		
}

JSBool JSFUNC_mouseRightUp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	*rval = INT_TO_JSVAL(0);
	return pSM->SendMouseEvent(WM_RBUTTONUP, MK_RBUTTON, argc, argv, rval);
}

JSBool JSFUNC_mouseDblLeftDown(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = INT_TO_JSVAL(0);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	return pSM->SendMouseEvent(WM_LBUTTONDBLCLK, MK_LBUTTON, argc, argv, rval);		
}

JSBool JSFUNC_mouseDblRightDown(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = INT_TO_JSVAL(0);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	return pSM->SendMouseEvent(WM_RBUTTONDBLCLK, MK_RBUTTON, argc, argv, rval);		
}

JSBool JSFUNC_mouseMove(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = INT_TO_JSVAL(0);
	if(argc < 2) return JS_FALSE;
	short x = 0;
	double dValueX = 0;
	if(JS_ValueToNumber( cx, argv[0], &dValueX )){
		x = (short)dValueX;
	}
	short y = 0;
	double dValueY = 0;
	if(JS_ValueToNumber( cx, argv[1], &dValueY )){
		y = (short)dValueY;
	}
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	//::SetCapture(ghWnd);
	//pSM->OnAppActivated();
	pSM->GetContext()->mpWindow->MouseMoveToPos( x, y);
	//return pSM->SetMouseEvent(WM_LBUTTONDOWN, MK_LBUTTON, argc, argv, rval);
	return JS_TRUE;
}

///// JavaScript Function Table
JSFunctionSpec _JSMouseFunctionSpec[] = {
	{ "addListener", JSFUNC_addMouseListener, 1, 0, 0 },
	{ "removeListener", JSFUNC_removeMouseListener, 1, 0, 0 },
	{ "addMouseMoveListener", JSFUNC_addMouseMoveListener, 1, 0, 0 },
	{ "removeMouseMoveListener", JSFUNC_removeMouseMoveListener, 1, 0, 0 },
	{ "hide", JSFUNC_hideMouse, 0, 0, 0 },
	{ "show", JSFUNC_showMouse, 0, 0, 0 },
	{ "setCursor", JSFUNC_setCursor, 0, 0, 0 }, // устанавливает вид курсора, который обычно стрелкой
	{ "setHandCursor", JSFUNC_setHandCursor, 0, 0, 0 },
	{ "mouseLeftDown", JSFUNC_mouseLeftDown, 2, 0, 0}, // эмулировать нажатие левой клавиши
	{ "mouseRightDown", JSFUNC_mouseRightDown, 2, 0, 0}, // эмулировать нажатие правой клавиши
	{ "mouseDblLeftDown", JSFUNC_mouseDblLeftDown, 2, 0, 0}, // эмулировать двойной левой щелчок
	{ "mouseLeftUp", JSFUNC_mouseLeftUp, 2, 0, 0}, // эмулировать отпускание левой клавиши
	{ "mouseRightUp", JSFUNC_mouseRightUp, 2, 0, 0}, // эмулировать отпускание правой клавиши
	{ "mouseDblRightDown", JSFUNC_mouseDblRightDown, 2, 0, 0}, // эмулировать двойной правой щелчок
	{ "mouseMove", JSFUNC_mouseMove, 2, 0, 0}, // переместить курсор мыши
	{ 0, 0, 0, 0, 0 }
};

bool mlSceneManager::SendMouseEvent(unsigned int eventId, unsigned int wparam, uintN argc, jsval *argv, jsval *rval){	
	if(argc < 2) return JS_FALSE;
	short x = 0;
	double dValueX = 0;
	if(JS_ValueToNumber( cx, argv[0], &dValueX )){
		x = (short)dValueX;
	}
	short y = 0;
	double dValueY = 0;
	if(JS_ValueToNumber( cx, argv[1], &dValueY )){
		y = (short)dValueY;
	}
	/*long lParam = (y << 16) + x;
	GetContext()->mpInput->AddEvent(omsUserEvent(eventId, wparam,lParam));*/
	mpContext->mpWindow->SendMouseMessage( eventId, wparam, x, y);
	return JS_TRUE;
}

// сбросить все ссылки на элемент, связанные c мышиными событиями 
// (например, если элемент удаляется)
// Если apMLEl==NULL, сбрасываются ссылки на любые элементы
void mlSceneManager::ResetMouse(mlRMMLElement* apMLEl){
	if(apMLEl == NULL){
		if(mpButUnderMouse != NULL)
			mpButUnderMouse = (mlRMMLElement*)1;
		if (mpBUMParents != NULL)
			MP_DELETE(mpBUMParents);
		mBUM.Reset();
		mpButPressed = NULL;
		if (mpBPParents != NULL)
			MP_DELETE(mpBPParents);
		mpButRPressed = NULL;
		if (mpBRPParents != NULL)
			MP_DELETE(mpBRPParents);
		ResetMouseTarget();
	}else{
		if(mpButUnderMouse == apMLEl){
			mpButUnderMouse = (mlRMMLElement*)1;
			if (mpBUMParents != NULL)
				MP_DELETE(mpBUMParents);
		}
		if(mBUM.mp3DObject == apMLEl)
			mBUM.Reset();

		if(mBUM.mpTextureObject == apMLEl)
			mBUM.mpTextureObject = NULL;
		if(mpButPressed == apMLEl){
			mpButPressed = NULL;
			if (mpBPParents != NULL)
				MP_DELETE(mpBPParents);
		}
		if(mpButRPressed == apMLEl){
			mpButRPressed = NULL;
			if (mpBRPParents != NULL)
				MP_DELETE(mpBRPParents);
		}
	}
	ReleaseMouse(apMLEl);
}

void mlSceneManager::ResetMouseTarget(){
	// free "target" link for GC
	bNullifyRefProps = true;
	jsval v;
	JS_GetProperty(cx, mjsoMouse, "target", &v);
	JS_GetProperty(cx, mjsoMouse, "modalButtons", &v);
	bNullifyRefProps = false;
}

//void mlSceneManager::DisableMouse(){
//}
//
//void mlSceneManager::EnableMouse(){
//}

void mlSceneManager::GetMouseState(mlSynchData &aData){
	// ??
}

void mlSceneManager::SetMouseState(mlSynchData &aData){
	// ??
}

void mlSceneManager::hideMouse(){
#ifdef CSCL
	mpContext->mpWindow->SetCursor(oms::NoCursor);
#else
	mpContext->mpApp->set_cursor(oms::NoCursor);
#endif
}

void mlSceneManager::showMouse(){ 
	SetNormalCursor();
}

void mlSceneManager::SetNormalCursor(int aiCursor, bool abForce){ 
#ifdef CSCL
	int iBigMask = mbBigCursor?CT_BIG_CURSOR_MASK:0;
	//miCursorType = oms::NormalCursor | iBigMask;
	if(aiCursor >= 0) miCursorType = (aiCursor);
	if(mbDragging && !abForce) return;
	mpContext->mpWindow->SetCursor(miCursorType);
#else
	miCursorType = oms::NormalCursor;
	if(mbDragging && !abForce) return;
	mpContext->mpApp->set_cursor((oms::CursorType)miCursorType);
#endif
}

void mlSceneManager::SetHandCursor(int auCursor, bool abForce){ 
#ifdef CSCL
	int iBigMask = mbBigCursor?CT_BIG_CURSOR_MASK:0;
	//miCursorType = oms::ActiveCursor | iBigMask;
	if (miHandCursorType == oms::HealCursor 
	||	miHandCursorType == oms::RemoveBoxCursor
	||	miHandCursorType == oms::StretcherCursor
	||  miHandCursorType == oms::DragPeople
	||	miHandCursorType == oms::TriageCursor)
	mpContext->mpWindow->SetCursor(miHandCursorType);
	else
	{
		if(auCursor > 1){
			miHandCursorType = (auCursor-2);
		}else if(auCursor == 1){
			miHandCursorType = oms::ActiveCursor | iBigMask;
		}else if(auCursor == 0){
			miHandCursorType = oms::NormalCursor | iBigMask;
		}
		if(mbDragging && !abForce) return;
		mpContext->mpWindow->SetCursor(miHandCursorType);
	}
#else
	miCursorType = oms::ActiveCursor;
	if(auCursor > 1)
		miCursorType = oms::CursorType(auCursor-2);
	if(mbDragging && !abForce) return;
	mpContext->mpApp->set_cursor(miCursorType);
#endif
}

void mlSceneManager::HideCursor(){ 
	hideMouse();
}

void mlSceneManager::ShowCursor(){
	showMouse();
}

void mlSceneManager::SetBigCursor(bool abBig){
	mbBigCursor = abBig;
}

}
