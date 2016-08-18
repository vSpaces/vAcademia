#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlRMMLPButPLHA.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLPButton::mlRMMLPButton(void)
{
	enabled = true;
	useHandCursor = 1;
	mbHitAreaIsBoundingBox = false;
	modal = 0;
	mbMouseWasIn = false;
	mbKeyEvListenersIsSet = false;
}

mlRMMLPButton::~mlRMMLPButton(void){
}

//	bool enabled;
//	bool useHandCursor;
//	class mlHitArea{
////		struct 
//	} hitArea;

///// JavaScript Variable Table
JSPropertySpec mlRMMLPButton::_JSPropertySpec[] = {
	JSPROP_RW(enabled)
	JSPROP_RW(useHandCursor)
	JSPROP_RW(hitArea)
	JSPROP_RW(modal)

	JSPROP_EV(onPress)
	JSPROP_EV(onRelease)
	JSPROP_EV(onReleaseOutside)
	JSPROP_EV(onRollOver)
	JSPROP_EV(onRollOut)
	JSPROP_EV(onKeyDown)
	JSPROP_EV(onKeyUp)
	JSPROP_EV(onSetFocus)
	JSPROP_EV(onKillFocus)
	JSPROP_EV(onAltPress)
	JSPROP_EV(onAltRelease)
	JSPROP_EV(onAltReleaseOutside)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLPButton::_JSFunctionSpec[] = {
	JSFUNC(isActivated,0)
	JSFUNC(captureMouse,0) // захватить мышь (все обработчики мыши будут вызываться только у этого объекта)
	JSFUNC(releaseMouse,0) // отпустить мышь
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLPButton::_EventSpec[] = {
	MLEVENT(onPress)
	MLEVENT(onRelease)
	MLEVENT(onReleaseOutside)
	MLEVENT(onRollOver)
	MLEVENT(onRollOut)
	MLEVENT(onKeyDown)
	MLEVENT(onKeyUp)
	MLEVENT(onSetFocus)
	MLEVENT(onKillFocus)
	MLEVENT(onAltPress)
	MLEVENT(onAltRelease)
	MLEVENT(onAltReleaseOutside)
	{0, 0, 0}
};

ADDPROPFUNCSPECS_IMPL(mlRMMLPButton)

#define GetElem GetElem_mlRMMLPButton
#define GetParent GetParent_mlRMMLPButton

///// JavaScript Set Property Wrapper
JSBool mlRMMLPButton::SetJSProperty(JSContext* cx, int id, jsval *vp) {
	switch(id) {
		case JSPROP_enabled:
			ML_JSVAL_TO_BOOL(enabled,*vp);
			AbsEnabledChanged();
			break;
		case JSPROP_useHandCursor:{
			unsigned int uUseHandCursorOld = useHandCursor;
			// ML_JSVAL_TO_BOOL(useHandCursor,*vp);
			if(JSVAL_IS_STRING(*vp)){
				if(isEqual(wr_JS_GetStringChars(JSVAL_TO_STRING(*vp)), (const wchar_t *) L"true"))
					useHandCursor = 1;
				else useHandCursor = 0;
			}else if(JSVAL_IS_BOOLEAN(*vp)){
				if(JSVAL_TO_BOOLEAN(*vp)){
					useHandCursor = 1;
				}else useHandCursor = 0;
			}else if(JSVAL_IS_INT(*vp)){
				useHandCursor = JSVAL_TO_INT(*vp)+2;
			}
			if(useHandCursor != uUseHandCursorOld)
				UseHandCursorChanged();
			}break;
		case JSPROP_hitArea:
			if(JSVAL_IS_STRING(*vp)){
				JSString* jss=JSVAL_TO_STRING(*vp);
				wchar_t* pwc=(wchar_t*)JS_GetStringChars(jss);
				if(isEqual(pwc, L"bound", 5)){
					mbHitAreaIsBoundingBox = true;
					hitArea.Clear();
				}else if(*pwc == L'\0' || isEqual(pwc, L" ") || isEqual(pwc, L"_")){ // isEqual(pwc, L"opaque") || 
					// сбросить в начальное положение
					mbHitAreaIsBoundingBox = false;
					hitArea.Clear();
				}else{
					mbHitAreaIsBoundingBox = false;
					hitArea.Parse(pwc);
				}
				if(GetElem()->GetVisible() != NULL)
					GetElem()->GetVisible()->InvalidBounds();
			}
			break;
		case JSPROP_modal:{
			unsigned short usOldModalVal = modal;
			if(JSVAL_IS_BOOLEAN(*vp)){
				modal = JSVAL_TO_BOOLEAN(*vp);
			}else{
				ML_JSVAL_TO_INT(modal,*vp);
			}
			if(modal != usOldModalVal){
				ModalChanged();
			}
			}break;
		case EVID_onPress:
		case EVID_onRollOver:
		case EVID_onKeyDown:
		case EVID_onKeyUp:
			if(JSVAL_IS_NULL(*vp)){
				GetElem()->RemoveEventListener(id, GetElem());
			}else{
				GetElem()->SetEventListener(id, GetElem());
			}
			break;
	}
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLPButton::GetJSProperty(int id, jsval *vp) {
	switch(id) {
		case JSPROP_enabled:
			*vp=BOOLEAN_TO_JSVAL(enabled);
			break;
		case JSPROP_useHandCursor:
			if(useHandCursor < 0){
				*vp=BOOLEAN_TO_JSVAL(false);
			}else{
				if(useHandCursor >= 2){
					*vp = INT_TO_JSVAL(useHandCursor-2);
				}else{
					if(useHandCursor == 1)
						*vp = JSVAL_TRUE;
					else
						*vp = JSVAL_FALSE;
				}

			}
			break;
		case JSPROP_hitArea:
			break;
		case JSPROP_modal:
			if(modal <= 1){
				*vp=BOOLEAN_TO_JSVAL(modal);
			}else
				*vp=INT_TO_JSVAL(modal);
			break;
		case EVID_onPress:
		case EVID_onRelease:
		case EVID_onReleaseOutside:
		case EVID_onRollOver:
		case EVID_onRollOut:
		case EVID_onKeyDown:
		case EVID_onKeyUp:
		case EVID_onSetFocus:
		case EVID_onKillFocus:
		case EVID_onAltPress:
		case EVID_onAltRelease:
		case EVID_onAltReleaseOutside:
			{
			mlRMMLElement* pMLEl = GetElem();
			int iRes = GPSM(pMLEl->mcx)->CheckEvent(id, pMLEl, vp);
			if(iRes >= 0)
				*vp = BOOLEAN_TO_JSVAL(iRes);
			}break;
	}
	return JS_TRUE;
}

__forceinline bool IsSpaceChar(wchar_t wc){
	switch(wc){
	case L' ':
	case L',':
	case L'\t':
	case L'\n':
		return true;
	}
	return false;
}

__forceinline bool IsCloseBracketChar(wchar_t wc){
	switch(wc){
	case L']':
	case L')':
	case L'}':
		return true;
	}
	return false;
}

__forceinline bool IsDigitChar(wchar_t wc){
	if(wc >= L'0' && wc <= L'9') return true;
	return false;
}

wchar_t* SkipSpace(wchar_t* apwc){
	for(;;){
		if(IsSpaceChar(*apwc)) apwc++;
		else return apwc;
	}
}

mlresult ReadInt(wchar_t* &apwc, int &ai){
	apwc=SkipSpace(apwc);
	ai=0;
	bool bMinus=false;
	if(*apwc=='-'){ bMinus=true; apwc++; }
	for(;;){
		if(*apwc=='\0') return ML_OK;
		if(IsSpaceChar(*apwc) || IsCloseBracketChar(*apwc)) return ML_OK;
		if(!IsDigitChar(*apwc)) return ML_ERROR_INVALID_ARG;
		int iDig=*apwc-L'0';
		ai=ai*10+iDig;
		apwc++;
	}
}

mlresult mlRMMLPButton::mlHitArea::HARect::Parse(wchar_t* &apwc){
	apwc++; // skip '['
	mlresult res;
	if(ML_FAILED(res=ReadInt(apwc,miX))) return res;
	if(ML_FAILED(res=ReadInt(apwc,miY))) return res;
	if(ML_FAILED(res=ReadInt(apwc,miW))) return res;
	if(ML_FAILED(res=ReadInt(apwc,miH))) return res;
	apwc=SkipSpace(apwc);
	if(*apwc!=L']') return ML_ERROR_INVALID_ARG;
	apwc++;
	return ML_OK;
}

bool mlRMMLPButton::mlHitArea::HARect::IsPointIn(mlPoint &aPnt){
	if(aPnt.x < miX) return false;
	if(aPnt.x > (miX+miW)) return false;
	if(aPnt.y < miY) return false;
	if(aPnt.y > (miY+miH)) return false;
	return true;
}

mlresult mlRMMLPButton::mlHitArea::HAPoly::Parse(wchar_t* &apwc){
	apwc++; // skip '{'
	mlresult res;
	Clear();
	PolyPoint* *ppLastPoint=&mpPoints;
	for(;;){
		apwc=SkipSpace(apwc);
		if(*apwc==L'\0') return ML_ERROR_INVALID_ARG;
		if(*apwc==L'}') {++apwc; return ML_OK;}
		int iX=0,iY=0;
		if(*apwc==L'(') apwc++;
		if(!IsDigitChar(*apwc) && *apwc!=L'-') return ML_ERROR_INVALID_ARG;
		if(ML_FAILED(res=ReadInt(apwc,iX))) return res;
		apwc=SkipSpace(apwc);
		if(!IsDigitChar(*apwc) && *apwc!=L'-') return ML_ERROR_INVALID_ARG;
		if(ML_FAILED(res=ReadInt(apwc,iY))) return res;
		apwc=SkipSpace(apwc);
		if(*apwc==L')') apwc++;
		PolyPoint* pNewPoint = MP_NEW( PolyPoint);
		pNewPoint->mPoint.x=iX;
		pNewPoint->mPoint.y=iY;
		*ppLastPoint=pNewPoint;
		ppLastPoint=&(pNewPoint->mpNext);
	}
}

mlRect mlRMMLPButton::mlHitArea::HAPoly::GetBounds(){
	mlRect rc; rc.left=10000000; rc.top=10000000; rc.bottom=-10000000; rc.right=-10000000;
	PolyPoint* pPoint=mpPoints;
	while(pPoint!=NULL){
		int iX=pPoint->mPoint.x;
		int iY=pPoint->mPoint.y;
		if(iX < rc.left) rc.left=iX;
		if(iX > rc.right) rc.right=iX;
		if(iY < rc.top) rc.top=iY;
		if(iY > rc.bottom) rc.bottom=iY;
		pPoint=pPoint->mpNext;
	}
	if(rc.left==10000000){
		ML_INIT_RECT(rc);
	}
	return rc;
}

using namespace rmmlplha;

bool mlRMMLPButton::mlHitArea::HAPoly::IsPointIn(mlPoint &aPnt){
	int i;
	int intersection_count;
	CLine line;
	bool FoundResult;
	CFloatVector v;

	int PointCount=0;
	PolyPoint* pPoint=mpPoints;
	while(pPoint!=NULL){
		PointCount++;
		pPoint=pPoint->mpNext;
	}

	if (PointCount<3) return false;

	CLine *lpLines = MP_NEW_ARR( CLine ,PointCount);

	CLine *pLine = lpLines;
	pPoint=mpPoints;
	while(pPoint!=NULL){
		mlPoint* pp=&(pPoint->mPoint);
		pLine->p0.x=pp->x;
		pLine->p0.y=pp->y;
		if(pPoint->mpNext==NULL){
			pp=&(mpPoints->mPoint);
		}else{
			pp=&(pPoint->mpNext->mPoint);
		}
		pLine->p1.x=pp->x;
		pLine->p1.y=pp->y;
		pPoint=pPoint->mpNext;
		pLine++;
	}

	pLine = lpLines;
	CPoint CheckPoint;
	CheckPoint.x=aPnt.x;
	CheckPoint.y=aPnt.y;
	for (i = 0; i<PointCount;i++,pLine++)
	{
		if (pLine->IsPointOnLine(CheckPoint))
		{
			MP_DELETE_ARR( lpLines);
			return false;
		}
	}

	do
	{
		double t;
		int rv;
		v.MakeSine(rand()); 
		FoundResult = true;
		intersection_count = 0;
		pLine = lpLines;
		for (i = 0; i<PointCount; i++,pLine++)
		{
			if (!Intersection(*pLine,CheckPoint,v,&t,rv))
			{
				FoundResult = false;
				break;
			}
			if (rv == NORMAL_INTERSECTION) intersection_count++;
		}
	}while(!FoundResult);
	
	MP_DELETE_ARR( lpLines);
	return ((intersection_count&1)==1);
}

mlresult mlRMMLPButton::mlHitArea::Parse(wchar_t* apwc){
	if(apwc==NULL) return ML_ERROR_INVALID_ARG;
	Clear();
	wchar_t* pwc=apwc;
	HARegion** ppLast_mpNext=&mpFirst;
	do{
		pwc=SkipSpace(pwc);
		if(*pwc==L'\0') return ML_OK;
		HARegion* pRegNew=NULL;
		switch(*pwc){
		case L'[':
			pRegNew = MP_NEW( HARect);
			break;
		case L'(':
			pRegNew = MP_NEW( HAEllipse);
			break;
		case L'{':
			pRegNew = MP_NEW( HAPoly);
			break;
		default:
			pRegNew = MP_NEW( HAMask);
		}
		mlresult res;
		if(ML_FAILED(res=pRegNew->Parse(pwc))){
			MP_DELETE( pRegNew);
			return res;
		}
		*ppLast_mpNext=pRegNew;
		if(pRegNew) ppLast_mpNext=&(pRegNew->mpNext);
	}while(*pwc!=L'\0');
	return ML_OK;
}

//MLEVENT_IMPL(mlRMMLPButton,onPress);
//MLEVENT_IMPL(mlRMMLPButton,onRelease);
//MLEVENT_IMPL(mlRMMLPButton,onReleaseOutside);
//MLEVENT_IMPL(mlRMMLPButton,onRollOver);
//MLEVENT_IMPL(mlRMMLPButton,onRollOut);

bool mlRMMLPButton::Button_IsPointIn(mlPoint &aPnt){
	if(hitArea.IsDefined()){
		mlPoint pnt=aPnt;
//mlRMMLElement* pMLEl = GetElem();
		mlRMMLVisible* pVis=GetElem()->GetVisible();
		pnt.x-=(pVis->mXY.x);
		pnt.y-=(pVis->mXY.y);
		return hitArea.IsPointIn(pnt);
	}else{
		if(mbHitAreaIsBoundingBox){
			return GetElem()->GetVisible()->Visible_IsPointInBoundingBox(aPnt);
		}else
			return GetElem()->GetVisible()->Visible_IsPointIn(aPnt);
	}
	return false;
}

//void mlRMMLPButton::GenerateEvents(char aidEvent){
//	if(aidEvent==mlRMMLVisible::EVID_onMouseMove){
//		if(IsMouseIn()){
//			if(mbMouseWasIn) return;
//			mbMouseWasIn=true;
//			GetElem()->CallListeners(EVID_onRollOver);
//		}else{
//			if(!mbMouseWasIn) return;
//			mbMouseWasIn=false;
//			GetElem()->CallListeners(EVID_onRollOver);
//		}
//		return;
//	}
//	if(aidEvent==mlRMMLVisible::EVID_onMouseDown){
//		if(IsMouseIn()){
//			GetElem()->CallListeners(EVID_onPress);
//		}
//		return;
//	}
//	if(aidEvent==mlRMMLVisible::EVID_onMouseUp){
//		mlRMMLElement* pMLEl=GetElem();
//		if(IsMouseIn()){
//			pMLEl->CallListeners(EVID_onRelease);
//		}else{
//			pMLEl->CallListeners(EVID_onReleaseOutside);
//		}
//		return;
//	}
//}

bool mlRMMLPButton::CanHandleEvent(char aidEvent, bool abAllEvents, bool &bNeedToHandle){
	if(aidEvent < TEVIDE_mlRMMLPButton && aidEvent > EVID_mlRMMLPButton){
		if(!abAllEvents && !GetAbsEnabled()) return false;
		switch(aidEvent){
		case EVID_onPress:
		case EVID_onRelease:
		case EVID_onReleaseOutside:
		case EVID_onAltPress:
		case EVID_onAltRelease:
		case EVID_onAltReleaseOutside:
			bNeedToHandle=true;
			break;
		case EVID_onKeyDown:
			if(!abAllEvents){
				mlRMMLElement* pMLEl = GetElem();
				if(GPSM(pMLEl->mcx)->GetFocus() != pMLEl) return false;
			}
			bNeedToHandle=true;
			break;
		case EVID_onKeyUp:
			if(!abAllEvents){
				mlRMMLElement* pMLEl = GetElem();
				if(GPSM(pMLEl->mcx)->GetFocus() != pMLEl) return false;
			}
			bNeedToHandle=true;
			break;
		}
	}
	return true;
}

void mlRMMLPButton::HandleEvent(char aidEvent){
	GetElem_mlRMMLPButton()->SetButtonEventData(aidEvent);
	switch(aidEvent){
	case EVID_onPress:
	case EVID_onAltPress:{
		//onSetFocus();
		// если обработчиков клавиатурного ввода у элемента нет, то не надо на него устанавливать фокус 
		if(HandlesKeyEvents())
			GPSM(GetElem_mlRMMLPButton()->mcx)->SetFocus(GetElem_mlRMMLPButton());
		// если ткнули на 3D-объект, 
//		mlRMMLVisible* pVis = GetElem()->GetVisible();
		mlRMMLElement* pMLElThis = GetElem();
		//mlRMMLVisible* pVis = GPSM(pMLElThis->mcx)->GetVisibleUnderMouse();
		//if(pVis == NULL && pMLElThis->GetVisible() != NULL)
		//	pVis = pMLElThis->GetVisible();
		//if(pVis != NULL){
		//	// то вызвать OnPress у viewport-а
		//	mlPoint pnt = GPSM(pMLElThis->mcx)->GetMouseXY();
		//	pnt.x -= (pVis->mXY.x);
		//	pnt.y -= (pVis->mXY.y);
		//	mlRMMLElement* pMLEl = pVis->GetElem_mlRMMLVisible();
		//	if(pMLEl != NULL){
		//		mlRMMLPButton* pBut = pMLEl->GetButton();
		//		if(pBut != NULL)
		//			pBut->OnPress(pnt);
		//	}
		//}
		// 
		mlPoint pnt;
		// если нажали на 3D-объект, 
		mlRMMLElement* p3DObj = GPSM(pMLElThis->mcx)->Get3DObjectUnderMouse();
		if(p3DObj != NULL){
			// значит координаты надо отсчитывать от объекта текстуры
			mlRMMLElement* pTextureObject = GPSM(pMLElThis->mcx)->GetTextureObjectUnderMouse();
			pnt = GPSM(pMLElThis->mcx)->GetTextureObjectMouseXY();
		}else{
			mlRMMLVisible* pVis = pMLElThis->GetVisible();
			if(pVis != NULL){
				pnt = pVis->GetMouseXY();
			}else{
				pnt = GPSM(pMLElThis->mcx)->GetMouseXY(); // ?? возможно это неправильно
			}
		}
		if(aidEvent == EVID_onAltPress)
			OnAltPress(pnt);
		else
			OnPress(pnt);
		}break;
	case EVID_onRelease:
	case EVID_onReleaseOutside:
		OnRelease();
		break;
	case EVID_onAltRelease:
		OnAltRelease();
		break;
	case EVID_onKeyDown:
		onKeyDown();
		break;
	case EVID_onKeyUp:
		onKeyUp();
		break;
	}
}


JSBool mlRMMLPButton::JSFUNC_isActivated(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLPButton* pBut=pMLEl->GetButton();
	*rval = JSVAL_FALSE;
	if(pMLEl->mbBtnEvListenersIsSet)
		*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// поддержка синхронизации

void mlRMMLPButton::But_GetData4Synch(JSContext* cx, char aidEvent, mlSynchData &avSynchData){
	if(aidEvent <= EVID_mlRMMLPButton || aidEvent >= TEVIDE_mlRMMLPButton)
		return;
	if(aidEvent >= EVID_onPress && aidEvent <= EVID_onRollOut){
		// сохраняем положение мыши
		mlPoint MXY = GPSM(cx)->GetMouseXY();
		avSynchData.put(&MXY,sizeof(mlPoint));
	}else if(aidEvent >= EVID_onKeyDown && aidEvent <= EVID_onKeyUp){
		// сохранить key_code
		unsigned short uKeyCode = GPSM(cx)->GetKeyCode();
		avSynchData << uKeyCode;
	}
}

void mlRMMLPButton::But_SetData4Synch(JSContext* cx, char aidEvent, mlSynchData &avSynchData){
	if(aidEvent <= EVID_mlRMMLPButton || aidEvent >= TEVIDE_mlRMMLPButton)
		return;
	if(aidEvent >= EVID_onPress && aidEvent <= EVID_onRollOut){
		// устанавливаем положение мыши
		mlPoint MXY;
		avSynchData.get(&MXY,sizeof(mlPoint));
		GPSM(cx)->SetMouseXY(MXY);
	}else if(aidEvent >= EVID_onKeyDown && aidEvent <= EVID_onKeyUp){
		// устанавливаем key_code
		unsigned short uKeyCode;
		avSynchData >> uKeyCode;
		GPSM(cx)->SetKeyCode(uKeyCode);
	}
}
/*
void mlRMMLPButton::But_SetData4Synch(char aidEvent, unsigned char* apSynchData, int &aiIdx){
	if(aidEvent <= EVID_mlRMMLPButton || aidEvent >= TEVIDE_mlRMMLPButton)
		return;
	if(aidEvent >= EVID_onPress && aidEvent <= EVID_onRollOut){
		// устанавливаем положение мыши
		mlPoint MXY;
		get_int(apSynchData, aiIdx, MXY.x);
		get_int(apSynchData, aiIdx, MXY.y);
		gpSM->SetMouseXY(MXY);
	}else if(aidEvent >= EVID_onKeyDown && aidEvent <= EVID_onKeyUp){
		// устанавливаем key_code
		unsigned short uKeyCode;
		get_word(apSynchData, aiIdx, uKeyCode);
		gpSM->SetKeyCode(uKeyCode);
	}
}
*/

void mlRMMLPButton::UseHandCursorChanged(){
	mlRMMLElement* pMLEl = GetElem_mlRMMLPButton();
	GPSM(pMLEl->mcx)->UseHandCursorChanged(pMLEl);
}

void mlRMMLPButton::ModalChanged(){
	mlRMMLElement* pMLEl = GetElem_mlRMMLPButton();
	GPSM(pMLEl->mcx)->UpdateModalButList(pMLEl);
}

// реализация mlIButton
unsigned int mlRMMLPButton::GetKeyCode(){
	return GPSM(GetElem_mlRMMLPButton()->mcx)->GetKeyCode();
}

unsigned int mlRMMLPButton::GetKeyStates(){
	return GPSM(GetElem_mlRMMLPButton()->mcx)->GetKeyStates();
}

unsigned int mlRMMLPButton::GetKeyScanCode(){
	return GPSM(GetElem_mlRMMLPButton()->mcx)->GetKeyScanCode();
}

unsigned int mlRMMLPButton::GetKeyUnicode(){
	return GPSM(GetElem_mlRMMLPButton()->mcx)->GetUnicodeKeyCode();
}

bool mlRMMLPButton::GetAbsEnabled(){
	if(!enabled) return false;
	mlRMMLElement* pParent=GetParent();
	if(pParent!=NULL){
		mlRMMLPButton* pBut=pParent->GetButton();
		if(pBut==NULL) return true;
		if(!pBut->GetAbsEnabled())
			return false;
	}
	return true;
}

void mlRMMLPButton::Button_ParentChanged(){
	AbsEnabledChanged();
}

// реализация moIButton
moIButton* mlRMMLPButton::GetmoIButton(){
	moMedia* pMO=GetElem_mlRMMLPButton()->GetMO();
	if(PMO_IS_NULL(pMO)) return NULL;
	return pMO->GetIButton();
}

#define MOIBUT_EVENT_CALL(E) \
	moIButton* pmoIButton=GetmoIButton(); \
	if(pmoIButton) pmoIButton->##E(); 


#define MOIBUT_EVENT_IMPL(E) \
void mlRMMLPButton::##E(){ \
	MOIBUT_EVENT_CALL(E); \
}

void mlRMMLPButton::onSetFocus(){
	//GPSM(GetElem_mlRMMLPButton()->mcx)->SetFocus(GetElem_mlRMMLPButton());
	MOIBUT_EVENT_CALL(onSetFocus);
}

void mlRMMLPButton::onKillFocus(){
	MOIBUT_EVENT_CALL(onKillFocus);
}

void mlRMMLPButton::onKeyDown(){ 
//	mlRMMLElement* pMLEl=gpSM->GetFocus();
//	if(pMLEl!=GetElem()) return;
	MOIBUT_EVENT_CALL(onKeyDown);
}

void mlRMMLPButton::onKeyUp(){ 
//	mlRMMLElement* pMLEl=gpSM->GetFocus();
//	if(pMLEl!=GetElem()) return;
	MOIBUT_EVENT_CALL(onKeyUp);
}

//#define MOIBUT_EVENT_IMPL2(E) \
//void mlRMMLPButton::##E(){ \
//	MOIBUT_EVENT_CALL(E); \
//	mlRMMLElement* pMLEl=GetElem_mlRMMLPButton(); \
//	if(pMLEl->IsComposition()){ \
//		pv_elems pChildren=pMLEl->mpChildren; \
//		if(pChildren!=NULL){ \
//			v_elems::iterator vi; \
//			for(vi=pChildren->begin(); vi != pChildren->end(); vi++ ){ \
//				mlRMMLElement* pMLEl=*vi; \
//				mlRMMLPButton* pBut=pMLEl->GetButton(); \
//				if(pBut!=NULL) pBut->##E(); \
//			} \
//		} \
//	}else if(pMLEl->mRMMLType == MLMT_GROUP){ \
//		pv_elems pChildren=pMLEl->mpChildren; \
//		if(pChildren!=NULL){ \
//			v_elems::iterator vi; \
//			for(vi=pChildren->begin(); vi != pChildren->end(); vi++ ){ \
//				mlRMMLElement* pMLEl=*vi; \
//				mlRMMLPButton* pBut=pMLEl->GetButton(); \
//				if(pBut!=NULL) pBut->##E(); \
//			} \
//		} \
//	} \
//}

//MOIBUT_EVENT_IMPL2(AbsEnabledChanged)

void mlRMMLPButton::AbsEnabledChanged(){
	MOIBUT_EVENT_CALL(AbsEnabledChanged);
	mlRMMLElement* pMLEl=GetElem_mlRMMLPButton();
	if(pMLEl->IsComposition()){
		pv_elems pChildren=pMLEl->mpChildren;
		if(pChildren!=NULL){
			v_elems::iterator vi;
			for(vi=pChildren->begin(); vi != pChildren->end(); vi++ ){
				mlRMMLElement* pMLEl=*vi;
				mlRMMLPButton* pBut=pMLEl->GetButton();
				if(pBut!=NULL) pBut->AbsEnabledChanged();
			}
		}
	}else if(pMLEl->mRMMLType == MLMT_GROUP){
		pv_elems pChildren=pMLEl->mpChildren;
		if(pChildren!=NULL){
			v_elems::iterator vi;
			for(vi=pChildren->begin(); vi != pChildren->end(); vi++ ){
				mlRMMLElement* pMLEl=*vi;
				mlRMMLPButton* pBut=pMLEl->GetButton();
				if(pBut!=NULL) pBut->AbsEnabledChanged();
			}
		}
	}
}

// захватить мышь (все обработчики мыши будут вызываться только у этого объекта)
JSBool mlRMMLPButton::JSFUNC_captureMouse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLPButton* pBut = pMLEl->GetButton();
	bool bRet = GPSM(cx)->CaptureMouse(pMLEl);
	*rval = BOOLEAN_TO_JSVAL(bRet);
//	*rval = JSVAL_FALSE;
//	if(pMLEl->mbBtnEvListenersIsSet)
//		*rval = JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLPButton::JSFUNC_releaseMouse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx,obj);
	mlRMMLPButton* pBut = pMLEl->GetButton();
	bool bRet = GPSM(cx)->ReleaseMouse(pMLEl);
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

}
