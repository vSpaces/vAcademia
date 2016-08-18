#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLContinuous::mlRMMLContinuous(void)
{
	_playing=false;

	currentFrame=0;
	_length=0;
	currentPos=0; // ms
	duration=-1; // ms

	fps=-1;
	_loop=false;
}

mlRMMLContinuous::~mlRMMLContinuous(void)
{
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLContinuous::_JSPropertySpec[] = {
	JSPROP_RW(_playing)
	JSPROPA_RW(playing, _playing)
	JSPROP_RW(currentFrame)
	JSPROP_RW(_length)
	JSPROPA_RW(totalFrames, _length)
	JSPROPA_RW(_totalframes, _length)
	JSPROP_RW(currentPos)
	JSPROP_RW(_duration)
	JSPROPA_RW(duration, _duration)
	JSPROP_RW(fps)
	JSPROP_RW(_loop)
	JSPROPA_RW(loop, _loop)
	JSPROPA_RW(looped, _loop)

	JSPROP_EV(onEnterFrame)
	JSPROP_EV(onPlayed)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLContinuous::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	JSFUNC(setCheckPoints,1)
	JSFUNC(rewind,0)
	JSFUNC(play,0)
	JSFUNC(stop,0)
	JSFUNC(rewindAndPlay,0)
	JSFUNC(gotoAndPlay,1)
	JSFUNC(gotoAndStop,1)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLContinuous::_EventSpec[] = {
	MLEVENT(onEnterFrame)
	MLEVENT(onPlayed)
	{0, 0, 0}
};

ADDPROPFUNCSPECS_IMPL(mlRMMLContinuous)

//mlresult mlRMMLContinuous::AddPropFuncSpecs(JSPropertySpec* &pJSPropertySpec,JSFunctionSpec* &pJSFunctionSpec){
//	return mlJSClass::AddPropFuncSpecs(pJSPropertySpec, pJSFunctionSpec,
//										_JSPropertySpec, sizeof(_JSPropertySpec)/sizeof(JSPropertySpec)-1,
//										_JSFunctionSpec, sizeof(_JSFunctionSpec)/sizeof(JSFunctionSpec)-1);
//}

///// JavaScript Set Property Wrapper
JSBool mlRMMLContinuous::SetJSProperty(JSContext* cx, int id, jsval *vp) {
	switch(id) {
		case JSPROP__playing:{
			bool bWasPlaying=_playing;
			ML_JSVAL_TO_BOOL(_playing,*vp);
			if(_playing!=bWasPlaying)
				PlayingChanged();
			break;
		}
		case JSPROP_currentFrame:
			ML_JSVAL_TO_INT(currentFrame,*vp);
			FrameChanged();
			break;
		case JSPROP_currentPos:
			ML_JSVAL_TO_INT(currentPos,*vp);
			PosChanged();
			break;
		case JSPROP__loop:
			ML_JSVAL_TO_BOOL(_loop,*vp);
			break;
		case JSPROP__length:
			ML_JSVAL_TO_INT(_length,*vp);
			LengthChanged();
			break;
		case JSPROP__duration:
			ML_JSVAL_TO_INT(duration,*vp);
			DurationChanged();
			break;
		case JSPROP_fps:
			ML_JSVAL_TO_INT(fps,*vp);
			FPSChanged();
			break;
	}
	return JS_TRUE;
}

//#define GET_EV_STRID(N) \
//		case EVID_##N: \
//			GET_EV_STRID(onEnterFrame) \
//			JSString 
//			*vp = STRING_TO_JSVAL(); \
//			break;

///// JavaScript Get Property Wrapper
JSBool mlRMMLContinuous::GetJSProperty(int id, jsval *vp) {
	switch(id) {
		case JSPROP__playing:
			*vp=BOOLEAN_TO_JSVAL(_playing);
			break;
		case JSPROP_currentFrame:
			*vp=INT_TO_JSVAL(currentFrame);
			break;
		case JSPROP__length:
			*vp=INT_TO_JSVAL(_length);
			break;
		case JSPROP_currentPos:
			*vp=INT_TO_JSVAL(GetCurPos());
			break;
		case JSPROP__duration:
			*vp=INT_TO_JSVAL(GetDuration());
			break;
		case JSPROP_fps:
			*vp=INT_TO_JSVAL(fps);
			break;
		case JSPROP__loop:
			*vp=BOOLEAN_TO_JSVAL(_loop);
			break;
		case EVID_onEnterFrame:
		case EVID_onPlayed:
			{
			mlRMMLElement* pMLEl = GetElem_mlRMMLContinuous();
			int iRes = GPSM(pMLEl->mcx)->CheckEvent(id, pMLEl, vp);
			if(iRes >= 0)
				*vp = BOOLEAN_TO_JSVAL(iRes);
			}break;
	}
	return JS_TRUE;
}

void Set_Event_checkPoint(mlRMMLElement* apMLEl,long alCheckPoint){
	JSContext* cx=apMLEl->mcx;
	JS_DefineProperty(cx,GPSM(cx)->GetEventGO(),"checkPoint",INT_TO_JSVAL(alCheckPoint),0,0,0);
}

bool mlRMMLContinuous::Cont_SetEventData(char aidEvent, mlSynchData &Data){
	switch(aidEvent){
	case EVID_onPlayed:{
		int lCheckPoint=-1;
		Data >> lCheckPoint;
		Set_Event_checkPoint(GetElem_mlRMMLContinuous(),lCheckPoint);
		}return true;
	}
	return true;
}

int JSValToMS(JSContext *cx, jsval v){
	int iMs=0;
	if(JSVAL_IS_INT(v)){
		iMs=JSVAL_TO_INT(v);
	}else if(JSVAL_IS_DOUBLE(v)){
		iMs=(int)(*JSVAL_TO_DOUBLE(v));
	}
	if(iMs<0) iMs=0;
	return iMs;
}

JSBool mlRMMLContinuous::JSFUNC_setCheckPoints(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
//	if (argc < 1) return JS_FALSE;
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLContinuous* pCont=pMLEl->GetContinuous();
	std::vector<long> vCheckPoints;
	if(argc==0){
#if _MSC_VER >= 1600
		pCont->setCheckPoints( vCheckPoints.begin()._Ptr,0);
#elif _MSC_VER > 1200
		pCont->setCheckPoints( vCheckPoints.begin()._Myptr,0);
#else
		pCont->setCheckPoints(vCheckPoints.begin(),0);
#endif
		return JS_TRUE;
	}
	for(int ii=0; ii<(int)argc; ii++){
		jsval v=argv[ii];
		if(JSVAL_IS_NUMBER(v)){
			vCheckPoints.push_back(JSValToMS(cx, v));
		}else if(JSVAL_IS_OBJECT(v) && JS_IsArrayObject(cx,JSVAL_TO_OBJECT(v))){
			// Array
			JSObject* jsoArr=JSVAL_TO_OBJECT(v);
			jsuint uiLen=0;
			if(JS_GetArrayLength(cx,jsoArr,&uiLen)){
				for(int jj=0; jj<(int)uiLen; jj++){
					if(JS_GetElement(cx, jsoArr, jj, &v)){
						if(JSVAL_IS_NUMBER(v)){
							vCheckPoints.push_back(JSValToMS(cx, v));
						}else{
							JS_ReportWarning(cx,"setCheckPoints() arguments must be numbers and arrays");
						}
					}
				}
			}
		}else{
			JS_ReportWarning(cx,"setCheckPoints() arguments must be numbers and arrays");
		}
	}
#if _MSC_VER >= 1600
	pCont->setCheckPoints( vCheckPoints.begin()._Ptr,vCheckPoints.size());
#elif _MSC_VER > 1200
	pCont->setCheckPoints(vCheckPoints.begin()._Myptr,vCheckPoints.size());
#else
	pCont->setCheckPoints(vCheckPoints.begin(),vCheckPoints.size());
#endif
	return JS_TRUE;
}

JSBool mlRMMLContinuous::JSFUNC_rewind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLContinuous* pCont=pMLEl->GetContinuous();
	pCont->Rewind();
	*rval = BOOLEAN_TO_JSVAL(JS_TRUE);
	return JS_TRUE;
}

JSBool mlRMMLContinuous::JSFUNC_play(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLContinuous* pCont=pMLEl->GetContinuous();
	pCont->Play();
	*rval = BOOLEAN_TO_JSVAL(JS_TRUE);
	return JS_TRUE;
}

JSBool mlRMMLContinuous::JSFUNC_stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLContinuous* pCont=pMLEl->GetContinuous();
	pCont->Stop();
	*rval = BOOLEAN_TO_JSVAL(JS_TRUE);
	return JS_TRUE;
}

JSBool mlRMMLContinuous::JSFUNC_rewindAndPlay(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLContinuous* pCont=pMLEl->GetContinuous();
	pCont->Rewind();
	pCont->Play();
	*rval = BOOLEAN_TO_JSVAL(JS_TRUE);
	return JS_TRUE;
}

JSBool mlRMMLContinuous::goTo(jsval avArg, const char* alpcFuncName){
	mlRMMLElement* pMLEl=GetElem_mlRMMLContinuous();
	int iFrameNum = 0;
	if(JSVAL_IS_DOUBLE(avArg)){
		jsdouble dVal = *(JSVAL_TO_DOUBLE(avArg));
		avArg = INT_TO_JSVAL((int)dVal);
	}
	if(JSVAL_IS_INT(avArg)){
		if(pMLEl->mRMMLType == MLMT_AUDIO || pMLEl->mRMMLType == MLMT_VIDEO){
			int iPos = JSVAL_TO_INT(avArg);
			if(iPos < 0) iPos = 0;
			unsigned long ulDuration = GetDuration();
			if(ulDuration <= 0) iPos = 0;
			else if(iPos > (int)ulDuration) iPos = ulDuration - 1;
			currentPos = iPos;
			PosChanged();
		}else{
			iFrameNum = JSVAL_TO_INT(avArg);
			if(iFrameNum < 0) iFrameNum = 0;
			if(_length <= 0) iFrameNum = 0;
			else if(iFrameNum > (int)_length) iFrameNum = _length - 1;
			currentFrame = iFrameNum;
			FrameChanged();
		}
	}else if(JSVAL_IS_STRING(avArg) && pMLEl->mRMMLType == MLMT_SEQUENCER){
		JSString* jssFrameName = JSVAL_TO_STRING(avArg);
		iFrameNum = GetFrameByName(wr_JS_GetStringChars(jssFrameName));
		if(iFrameNum < 0){
			JS_ReportError(pMLEl->mcx, "Script with name %s not found", cLPCSTR(JS_GetStringChars(jssFrameName)));
			return JS_FALSE;
		}
		currentFrame = iFrameNum;
		FrameChanged();
	}else{
		JS_ReportError(pMLEl->mcx, "%s function must get one integer or string argument", alpcFuncName);
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlRMMLContinuous::JSFUNC_gotoAndPlay(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLContinuous* pCont=pMLEl->GetContinuous();
	if(argc <= 0){
		JS_ReportError(cx, "gotoAndPlay function must get one integer or string argument");
		return JS_TRUE;
	}
	if(pCont->goTo(argv[0], "gotoAndPlay")){
		pCont->Play();
		*rval = BOOLEAN_TO_JSVAL(JS_TRUE);
	}else
		*rval = BOOLEAN_TO_JSVAL(JS_FALSE);
	return JS_TRUE;
}

JSBool mlRMMLContinuous::JSFUNC_gotoAndStop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLContinuous* pCont=pMLEl->GetContinuous();
	if(argc <= 0){
		JS_ReportError(cx, "gotoAndStop function must get one integer or string argument");
		return JS_TRUE;
	}
	if(pCont->goTo(argv[0], "gotoAndStop")){
		pCont->Stop();
		*rval = BOOLEAN_TO_JSVAL(JS_TRUE);
	}else
		*rval = BOOLEAN_TO_JSVAL(JS_FALSE);
	return JS_TRUE;
}

// moIContinuous
moIContinuous* mlRMMLContinuous::GetmoIContinuous(){
	moMedia* pMO=GetElem_mlRMMLContinuous()->GetMO();
	if(PMO_IS_NULL(pMO)) return NULL;
	return pMO->GetIContinuous();
}

//MLEVENT_IMPL(mlRMMLContinuous,onEnterFrame);
//void mlRMMLContinuous::onEnterFrame(){
//	mlRMMLElement* pMLEl = GetElem_mlRMMLContinuous();
//	GPSM(pMLEl->mcx)->mbDontSendSynchEvents = false;
//	pMLEl->CallListeners(EVID_onEnterFrame);
//}
void mlRMMLContinuous::onEnterFrame(){
	mlRMMLElement* pMLEl = GetElem_mlRMMLContinuous();
	bool bDontSendSynchEvents = (pMLEl->mucSynchFlags4Events & EVSM_onPlayed)?true:false;
	// если в моем update-е, 
mlSceneManager* pSM = GPSM(pMLEl->mcx);
	if(GPSM(pMLEl->mcx)->Updating()){
		// то выполняем сразу же
		Set_Event_checkPoint(pMLEl,getPassedCheckPoint());
		GPSM(pMLEl->mcx)->mbDontSendSynchEvents = bDontSendSynchEvents;
		pMLEl->CallListeners(EVID_onEnterFrame);
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(pMLEl->mcx)->SaveInternalPath(Data,pMLEl);
		Data << bDontSendSynchEvents;
		// сохраняем check point
		int lCheckPoint=getPassedCheckPoint();
		Data << lCheckPoint;
		// ставим в очередь событий
		GPSM(pMLEl->mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onEnterFrame, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(pMLEl->mcx)->GetMode() == smmInitilization);
	}
}

void mlRMMLContinuous::onPlayed(){ 
	if(!_loop) _playing=false;
	mlRMMLElement* pMLEl = GetElem_mlRMMLContinuous();
	bool bDontSendSynchEvents = (pMLEl->mucSynchFlags4Events & EVSM_onPlayed)?true:false;
	// если в моем update-е, 
	if(GPSM(pMLEl->mcx)->Updating()){
		// то выполняем сразу же
		Set_Event_checkPoint(pMLEl,getPassedCheckPoint());
		GPSM(pMLEl->mcx)->mbDontSendSynchEvents = bDontSendSynchEvents;
		pMLEl->CallListeners(EVID_onPlayed);
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(pMLEl->mcx)->SaveInternalPath(Data,pMLEl);
		Data << bDontSendSynchEvents;
		// сохраняем check point
		int lCheckPoint=getPassedCheckPoint();
		Data << lCheckPoint;
		// ставим в очередь событий
		GPSM(pMLEl->mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onPlayed, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(pMLEl->mcx)->GetMode() == smmInitilization);
	}
}

#define MOICONT_EVENT_CALL(E) \
	moIContinuous* pmoIContinuous=GetmoIContinuous(); \
	if(pmoIContinuous) pmoIContinuous->##E(); 

#define MOICONT_EVENT_IMPL(E) \
void mlRMMLContinuous::##E(){ \
	MOICONT_EVENT_CALL(##E); \
}

//MOICONT_EVENT_IMPL(PlayingChanged)
void mlRMMLContinuous::PlayingChanged(){
	// если скрытый видимый элемент запустили на воспроизведение
	if(_playing==true){
		mlRMMLElement* pMLEl=GetElem_mlRMMLContinuous();
		mlRMMLVisible* pVis=pMLEl->GetVisible();
		if(pVis!=NULL && pVis->mbVisible==false){
			// то показать его сначала
			jsval v=BOOLEAN_TO_JSVAL(true);
			JS_SetProperty(pMLEl->mcx,pMLEl->mjso,"_visible",&v);
		}
		//
		SET_SYNCH_FLAG4EVENT(GPSM(pMLEl->mcx), pMLEl, EVSM_onPlayed)
	}
	MOICONT_EVENT_CALL(PlayingChanged);
}
//MOICONT_EVENT_IMPL(FrameChanged)
void mlRMMLContinuous::FrameChanged(){
	Stop();
	MOICONT_EVENT_CALL(FrameChanged);
}
//MOICONT_EVENT_IMPL(PosChanged)
void mlRMMLContinuous::PosChanged(){
	Stop();
	MOICONT_EVENT_CALL(PosChanged);
}

void mlRMMLContinuous::LengthChanged(){
	MOICONT_EVENT_CALL(LengthChanged);
}

void mlRMMLContinuous::DurationChanged(){
	MOICONT_EVENT_CALL(DurationChanged);
}

void mlRMMLContinuous::FPSChanged(){
	MOICONT_EVENT_CALL(FPSChanged);
}

void mlRMMLContinuous::FrameChangedDontStop(){
	MOICONT_EVENT_CALL(FrameChanged);
}

void mlRMMLContinuous::PosChangedDontStop(){
	MOICONT_EVENT_CALL(PosChanged);
}

long mlRMMLContinuous::GetDuration(){
	moIContinuous* pmoIContinuous=GetmoIContinuous();
	if(pmoIContinuous) return (long)pmoIContinuous->GetDuration(); 
	return duration;
}

void mlRMMLContinuous::Rewind(){
//	if(currentPos <= 0 && currentFrame <= 0) return;
	currentPos=0;
	currentFrame=0;
	PosChanged();
//	mlRMMLElement* pMLEl=GetElem_mlRMMLContinuous();
//	JS_SetProperty(pMLEl->mcx,pMLEl->mjso,"currentPos",INT_TO_JSVAL(0));
}

void mlRMMLContinuous::Play(){
	if(_playing) return;
	_playing=true;
	PlayingChanged();
//	mlRMMLElement* pMLEl=GetElem_mlRMMLContinuous();
//	JS_SetProperty(pMLEl->mcx,pMLEl->mjso,"_playing",BOOLEAN_TO_JSVAL(true));
}

void mlRMMLContinuous::Stop(){
	if(!_playing) return;
	_playing=false;
	PlayingChanged();
}

void mlRMMLContinuous::setCheckPoints(long* apfCheckPoints, int aiLength){
	moIContinuous* pmoIContinuous=GetmoIContinuous();
	if(pmoIContinuous) pmoIContinuous->setCheckPoints(apfCheckPoints, aiLength);
}

long mlRMMLContinuous::getPassedCheckPoint(){
	moIContinuous* pmoIContinuous=GetmoIContinuous();
	if(pmoIContinuous) return pmoIContinuous->getPassedCheckPoint();
	return 0;
}

//void mlRMMLContinuous::OnPlayed(mlRMMLElement* apMLEl){
////	TRACE("mlRMMLContinuous::OnPlayed()\n",);
//	apMLEl->CallListeners(EVID_onPlayed);
//}

bool mlRMMLContinuous::Freeze(){
	PlayingChanged();
	return true;
}

bool mlRMMLContinuous::Unfreeze(){
	PlayingChanged();
	return true;
}

}
