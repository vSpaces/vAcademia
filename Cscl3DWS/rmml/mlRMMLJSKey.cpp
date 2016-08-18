#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include <XInput.h>
#include "XBOXController.h"

namespace rmml {

#define JSPROPKC_ID_MASK 0x40
#define JSPROP_KC(NM, CD) \
	{ #NM, CD | JSPROPKC_ID_MASK, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT, 0, 0},

enum{
	JSPROP_keyCode,
	JSPROP_gamepadKey,
	JSPROP_leftThumbState,
	JSPROP_rightThumbState,
	JSPROP_leftThumbValue,
	JSPROP_rightThumbValue,
	JSPROP_leftTrigger,
	JSPROP_rightTrigger,
	JSPROP_shiftKey,
	JSPROP_shiftLeft,
	JSPROP_ctrlKey,
	JSPROP_ctrlLeft,
	JSPROP_altKey,
	JSPROP_altLeft,
};

JSPropertySpec _JSKeyPropertySpec[] = {
	JSPROP_KC(BACKSPACE, 8)
	JSPROP_KC(CAPSLOCK, 20)
	JSPROP_KC(ALT, 18)
	JSPROP_KC(CONTROL, 17)
	JSPROP_KC(DELETEKEY, 46)
	JSPROP_KC(DOWN, 40)
	JSPROP_KC(END, 35)
	JSPROP_KC(ENTER, 13)
	JSPROP_KC(ESCAPE, 27)
	JSPROP_KC(HOME, 36)
	JSPROP_KC(INSERT, 45)
	JSPROP_KC(LEFT, 37)
	JSPROP_KC(PGDN, 34)
	JSPROP_KC(PGUP, 33)
	JSPROP_KC(RIGHT, 39)
	JSPROP_KC(SHIFT, 16)
	JSPROP_KC(SPACE, 32)
	JSPROP_KC(TAB, 9)
	JSPROP_KC(UP, 38)
	JSPROP_RO(keyCode)
	JSPROP_KC(XBOX_A, XBOXButtons::XBOX_A) 
	JSPROP_KC(XBOX_B, XBOXButtons::XBOX_B)
	JSPROP_KC(XBOX_X, XBOXButtons::XBOX_X)
	JSPROP_KC(XBOX_Y, XBOXButtons::XBOX_Y)
	JSPROP_KC(XBOX_DPAD_LEFT, XBOXButtons::XBOX_DPAD_LEFT)
	JSPROP_KC(XBOX_DPAD_RIGHT, XBOXButtons::XBOX_DPAD_RIGHT)
	JSPROP_KC(XBOX_DPAD_DOWN, XBOXButtons::XBOX_DPAD_DOWN)
	JSPROP_KC(XBOX_DPAD_UP, XBOXButtons::XBOX_DPAD_UP)
	JSPROP_KC(XBOX_LEFT_THUMB, XBOXButtons::XBOX_LEFT_THUMB) 
	JSPROP_KC(XBOX_RIGHT_THUMB, XBOXButtons::XBOX_RIGHT_THUMB)
	JSPROP_KC(XBOX_LEFT_SHOULDER, XBOXButtons::XBOX_LEFT_SHOULDER)
	JSPROP_KC(XBOX_RIGHT_SHOULDER, XBOXButtons::XBOX_RIGHT_SHOULDER)
	JSPROP_KC(XBOX_START, XBOXButtons::XBOX_START)
	JSPROP_KC(XBOX_BACK, XBOXButtons::XBOX_BACK) 
	JSPROP_KC(XBOX_FAILED, XBOXButtons::XBOX_UNKNOWN)
	JSPROP_KC(XBOX_NONE, XBOXButtons::XBOX_NONE)
	JSPROP_KC(XBOX_THUMB_LEFT, ThumbState::LEFT)
	JSPROP_KC(XBOX_THUMB_RIGHT, ThumbState::RIGHT)
	JSPROP_KC(XBOX_THUMB_DOWN, ThumbState::DOWN)
	JSPROP_KC(XBOX_THUMB_UP, ThumbState::UP)
	JSPROP_KC(XBOX_THUMB_NEUTRAL, ThumbState::NEUTRAL)
	JSPROP_RO(gamepadKey)
	JSPROP_RO(leftThumbState)
	JSPROP_RO(rightThumbState)
	JSPROP_RO(leftThumbValue)
	JSPROP_RO(rightThumbValue)
	JSPROP_RO(leftTrigger)
	JSPROP_RO(rightTrigger)
	JSPROP_RO(shiftKey)
	JSPROP_RO(shiftLeft)
	JSPROP_RO(ctrlKey)
	JSPROP_RO(ctrlLeft)
	JSPROP_RO(altKey)
	JSPROP_RO(altLeft)
	{ 0, 0, 0, 0, 0 }
};

JSBool JSKeyGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id)){
		int iID = JSVAL_TO_INT(id);
		if(iID & JSPROPKC_ID_MASK){
			*vp = INT_TO_JSVAL((iID & ~JSPROPKC_ID_MASK));
			return JS_TRUE;
		}
		switch(iID){
		case JSPROP_keyCode:
			*vp = INT_TO_JSVAL(GPSM(cx)->GetKeyCode());
			break;
		case JSPROP_gamepadKey:
			*vp = INT_TO_JSVAL(GPSM(cx)->GetGamepadKey());
			break;
		case JSPROP_leftThumbState:
			*vp = INT_TO_JSVAL(GPSM(cx)->GetLeftThumbState());
			break;
		case JSPROP_rightThumbState:
			*vp = INT_TO_JSVAL(GPSM(cx)->GetRightThumbState());
			break;
		case JSPROP_leftThumbValue:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, GPSM(cx)->GetLeftThumbValue()));
			break;
		case JSPROP_rightThumbValue:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, GPSM(cx)->GetRightThumbValue()));
			break;
		case JSPROP_leftTrigger:
			*vp = INT_TO_JSVAL(GPSM(cx)->GetLeftTrigger());
			break;
		case JSPROP_rightTrigger:
			*vp = INT_TO_JSVAL(GPSM(cx)->GetRightTrigger());
			break;
		case JSPROP_shiftKey:
			*vp = BOOLEAN_TO_JSVAL(GPSM(cx)->GetKeyStates() & 0x1);
			break;
		case JSPROP_shiftLeft:
			*vp = BOOLEAN_TO_JSVAL(GPSM(cx)->GetKeyStates() & 0x2);
			break;
		case JSPROP_ctrlKey:
			*vp = BOOLEAN_TO_JSVAL(GPSM(cx)->GetKeyStates() & 0x4);
			break;
		case JSPROP_ctrlLeft:
			*vp = BOOLEAN_TO_JSVAL(GPSM(cx)->GetKeyStates() & 0x8);
			break;
		case JSPROP_altKey:
			*vp = BOOLEAN_TO_JSVAL(GPSM(cx)->GetKeyStates() & 0x10);
			break;
		case JSPROP_altLeft:
			*vp = BOOLEAN_TO_JSVAL(GPSM(cx)->GetKeyStates() & 0x20);
			break;
		}
	}
	return JS_TRUE;
}

JSClass JSRMMLKeyClass = {
	"_Key", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSKeyGetProperty, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

__forceinline int GetKeyEventIdx(char aidEvent){
	switch(aidEvent){
	case mlRMMLButton::EVID_onKeyDown:	return 1;
	case mlRMMLButton::EVID_onKeyUp:	return 2;
	}
	return 0;
}

bool mlSceneManager::addKeyListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	v_elems* pv=&(maKeyListeners[GetKeyEventIdx(aidEvent)]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

bool mlSceneManager::addGamepadListener(char aidEvent,mlRMMLElement* apMLEl){
	if (!m_ctrl || !m_ctrl->IsConnected())
	{
		return false;
	}

	return addKeyListener(aidEvent, apMLEl);
}

JSBool JSFUNC_addKeyListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	mlRMMLElement* pMLEl=GET_RMMLEL(cx,JSVAL_TO_OBJECT(argv[0]));
	if(pSM->addKeyListener(0,pMLEl)) return JS_TRUE;
	return JS_FALSE;
}

JSBool JSFUNC_addGamepadListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	mlRMMLElement* pMLEl=GET_RMMLEL(cx,JSVAL_TO_OBJECT(argv[0]));
	if(pSM->addGamepadListener(0,pMLEl)) return JS_TRUE;
	return JS_FALSE;
}

bool mlSceneManager::removeKeyListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		if(mpButFocused==apMLEl) mpButFocused=NULL;
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_KEY_EVENTS_COUNT; ii++){
			v_elems* pv=&maKeyListeners[ii];
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
	v_elems* pv=&(maKeyListeners[GetKeyEventIdx(aidEvent)]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

bool mlSceneManager::removeGamepadListener(char aidEvent,mlRMMLElement* apMLEl){
	if (!m_ctrl)
	{
		return false;
	}

	return removeKeyListener(aidEvent, apMLEl);
}

JSBool JSFUNC_removeKeyListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	mlRMMLElement* pMLEl=GET_RMMLEL(cx,JSVAL_TO_OBJECT(argv[0]));
	if(pSM->removeKeyListener(0,pMLEl)) return JS_TRUE;
	return JS_FALSE;
}

JSBool JSFUNC_removeGamepadListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	mlRMMLElement* pMLEl=GET_RMMLEL(cx,JSVAL_TO_OBJECT(argv[0]));
	if(pSM->removeGamepadListener(0,pMLEl)) return JS_TRUE;
	return JS_FALSE;
}

JSBool JSFUNC_getAscii(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	*rval=INT_TO_JSVAL(pSM->GetAsciiKeyCode());
	return JS_TRUE;
}

JSBool JSFUNC_getUnicode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	*rval=INT_TO_JSVAL(pSM->GetUnicodeKeyCode());
	return JS_TRUE;
}

JSBool JSFUNC_getCode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	*rval=INT_TO_JSVAL(pSM->GetKeyCode());
	return JS_TRUE;
}

JSBool JSFUNC_isDown(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
//	*rval = INT_TO_JSVAL(pSM->GetKeyCode());
	return JS_TRUE;
}

JSBool JSFUNC_isToggled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
//	*rval = INT_TO_JSVAL(pSM->GetKeyCode());
	return JS_TRUE;
}

JSBool JSFUNC_getThumbX(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	*rval = INT_TO_JSVAL(pSM->GetXThumbValue(JSVAL_TO_STRING(argv[0])));
	return JS_TRUE;
}

JSBool JSFUNC_getThumbY(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	*rval = INT_TO_JSVAL(pSM->GetYThumbValue(JSVAL_TO_STRING(argv[0])));
	return JS_TRUE;
}

short mlSceneManager::GetXThumbValue(JSString* str)
{
	unsigned int value = (unsigned int) std::atoi(JS_GetStringBytes(str));
	unsigned int res = value >> 16;
	if ((short)(value) < 0) res++; // костыль 10/10
	return res;
}

short mlSceneManager::GetYThumbValue(JSString* str)
{
	unsigned int value = (unsigned int) std::atoi(JS_GetStringBytes(str));
	return value;
}
///// JavaScript Function Table
JSFunctionSpec _JSKeyFunctionSpec[] = {
	{ "addListener", JSFUNC_addKeyListener, 1, 0, 0 },
	{ "removeListener", JSFUNC_removeKeyListener, 1, 0, 0 },
	{ "addGamepadListener", JSFUNC_addGamepadListener, 1, 0, 0 },
	{ "removeGamepadListener", JSFUNC_removeGamepadListener, 1, 0, 0 },
	{ "getAscii", JSFUNC_getAscii, 0, 0, 0 },
	{ "getUnicode", JSFUNC_getUnicode, 0, 0, 0 },
	{ "getCode", JSFUNC_getCode, 0, 0, 0 },
	{ "isDown", JSFUNC_isDown, 1, 0, 0 },
	{ "isToggled", JSFUNC_isToggled, 1, 0, 0 },
	{ "getThumbX", JSFUNC_getThumbX, 1, 0, 0},
	{ "getThumbY", JSFUNC_getThumbY, 1, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

void mlSceneManager::ResetKey(bool bUI){
	mpButFocused=NULL;
	// если перегружается UI
	if(bUI){ 
		// то чистим полностью
		for(int ii=0; ii<ML_KEY_EVENTS_COUNT; ii++){
			maKeyListeners[ii].clear();
		}
	}else{
		// иначе удаляем только слушателей сцены
		for(int ii=0; ii<ML_KEY_EVENTS_COUNT; ii++){
			v_elems& v = maKeyListeners[ii];
			bool bAgain = true;
			while(bAgain){
				bAgain = false;;
				for(v_elems_iter vi = v.begin(); vi != v.end(); vi++ ){
					mlRMMLElement* pElem = (mlRMMLElement*)*vi;
					if(pElem == NULL) continue;
					if(pElem->GetScene() == mpScene){
						v.erase(vi);
						bAgain = true;
						break;
					}
				}
			};
		}
	}
}

void mlSceneManager::GetKeyState(mlSynchData &aData){
	// ??
}

void mlSceneManager::SetKeyState(mlSynchData &aData){
	// ??
}

}