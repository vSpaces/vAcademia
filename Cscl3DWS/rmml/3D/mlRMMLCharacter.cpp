
#include "mlRMML.h"
#include "ILogWriter.h"

namespace rmml {

mlRMMLCharacter::mlRMMLCharacter(void)
{
	mRMMLType=MLMT_CHARACTER;
	mType=MLMT_CHARACTER;
	mbChildrenMLElsCreated=false;
	mpSayingSpeech=NULL;
	miSayingSpeechIdx=-1;
	movement=NULL;
	mjssMovement=NULL;
	visemes=NULL;
	mjssVisemes=NULL;
	idles=NULL;
	mjssIdles=NULL;
	useZCoordinateSynchonization = true;
	
	faceIdles = NULL;
	mjssFaceIdles = NULL;

	unpickable = false;
}

mlRMMLCharacter::~mlRMMLCharacter(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
//GPSM(mcx)->CheckBug();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLCharacter::~mlRMMLCharacter");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

// добавить:
// interlocutors - массив собеседников 
// addInterlocutor(character) - добавить собеседника
// removeInterlocutor(character) - удалить собеседника

// lookByEyes(dest, time, koef)

///// JavaScript Variable Table
JSPropertySpec mlRMMLCharacter::_JSPropertySpec[] = {
	JSPROP_RW(movement)
	JSPROP_RW(visemes)
	JSPROP_RW(idles)
	JSPROP_RW(faceIdles)
	JSPROP_RW(shadow)
	JSPROP_RW(materials)
	JSPROP_RW(motion)
	JSPROP_RW(speech)
	JSPROP_RW(gravitation)
	JSPROP_RW(intangible)
	JSPROP_RW(unpickable)
//	{ "_camera", JSPROP__camera, JSPROP_ENUMERATE, 0, 0},
	JSPROP_RW(color)
	JSPROP_RW(opacity)
	JSPROP_RW(useZSynchronization)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLCharacter::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	JSFUNC(say, 1)
	JSFUNC(goTo, 1)
	JSFUNC(turnTo, 1)
	JSFUNC(lookAt, 3)
	JSFUNC(addInterlocutor, 1)
	JSFUNC(removeInterlocutor, 1)
	JSFUNC(lookAtByEyes, 1)
	JSFUNC(setViseme, 1) // установить выражение лица
	JSFUNC(unsetViseme, 1) // убрать с лица определенное выражение
	JSFUNC(doVisemes, 1) // последовательно сменить все выражения лица
	JSFUNC(setToRandomFreePlace, 1)
	JSFUNC(goForward, 0) // иди вперед
	JSFUNC(goBackward, 0) // пяться
	JSFUNC(strafeLeft, 0) // иди боком влево
	JSFUNC(strafeRight, 0) // иди боком вправо
	JSFUNC(stopGo, 0) // перестань идти (как goTo(NULL))
	JSFUNC(turnLeft, 0) // поворачивайся влево
	JSFUNC(turnRight, 0) // поворачивайся вправо
	JSFUNC(stopTurn, 0) // перестань поворачиваться
	JSFUNC(sayRecordedVoice, 1) // проиграть записанный голос
	JSFUNC(setLexeme, 2) // установить положение губ
	JSFUNC(goLeft, 1) // иди вперед и налево
	JSFUNC(goRight, 1) // иди вперед и направо
	JSFUNC(goBackLeft, 1) // иди назад и налево
	JSFUNC(goBackRight, 1) // иди назад и направо
	JSFUNC(getFaceVector, 0) //получить FaceVector аватара
	JSFUNC(toInitialPose, 0) // поставить аватара в начальную позу
	JSFUNC(fromInitialPose, 0) // вернуть аватара из начальной позы
	JSFUNC(pointByHandTo, 1) // показать на заданную точку или объект
	JSFUNC(setSleepingState, 1) // установить спящее/неспящее состояние
	JSFUNC(setRightHandTracking, 1) // установить вкл/выкл для отслеживания положения правой руки
	JSFUNC(setLeftHandTracking, 1) // установить вкл/выкл для отслеживания положения левой руки
	JSFUNC(setKinectDeskPoints, 3)
	JSFUNC(disableKinectDesk, 0)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLCharacter::_EventSpec[] = {
	MLEVENT(onTurned)
	MLEVENT(onSaid)
	MLEVENT(onFalling)
	MLEVENT(onFallAndStop)
	MLEVENT(onWreck)
	MLEVENT(onNeedToChangePosition)
	MLEVENT(onRightHandPosChanged)
	MLEVENT(onLeftHandPosChanged)
	// ?? Tandy: возможно стоит ввести событие, что перестал идти, например потому что уперся во что-то 
	{0, 0, 0}
};


MLJSCLASS_IMPL_BEGIN(Character,mlRMMLCharacter,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMML3DObject);
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLCharacter)

#define SET_REF_JSPROP(prop,mjss,cls,type,stype) \
					if(JSVAL_IS_NULL(*vp)){ \
						prop=NULL; \
					}else if(JSVAL_IS_OBJECT(*vp)){ \
						JSObject* jso=JSVAL_TO_OBJECT(*vp); \
						if(cls::IsInstance(cx, jso)){ \
							prop=(cls*)JS_GetPrivate(cx,jso); \
							if(prop->GetParent()!=priv){ \
								if(prop->GetParent() == NULL) \
									JS_ReportWarning(cx,"Trying to set no parent %s-object for '%s'", stype, cLPCSTR(priv->GetName())); \
								else \
									JS_ReportWarning(cx,"%s-object for '%s' is of another character-object '%s'", stype, cLPCSTR(priv->GetName()), cLPCSTR(prop->GetParent()->GetName())); \
							} \
						}else{ \
							JS_ReportError(cx, "Parameter is not a movement/visemes/idles-object"); \
							return JS_FALSE; \
						} \
					}else if(JSVAL_IS_STRING(*vp)){ \
						JSString* jss=JSVAL_TO_STRING(*vp); \
						if(!priv->mbChildrenMLElsCreated){  \
							mjss=jss; \
							SAVE_STR_FROM_GC(cx,obj,mjss); \
							return JS_TRUE; \
						} \
						mlRMMLElement* pMLEl=priv->GetChild(jss); \
						if(pMLEl==NULL){ \
							JS_ReportError(cx, "%s-object '%s' for '%s' not found", stype, JS_GetStringBytes(jss), cLPCSTR(priv->GetName())); \
							return JS_FALSE; \
						} \
						if(pMLEl->mRMMLType!=type){ \
							JS_ReportError(cx, "Named object is not a %s", stype); \
							return JS_FALSE; \
						} \
						prop=(cls*)pMLEl; \
					}

//							JS_ReportError(cx, "Movement/visemes-object (%S) for '%S' not found",JS_GetStringChars(jss),priv->GetName()); 

///// JavaScript Set Property Wrapper
JSBool mlRMMLCharacter::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLCharacter);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMML3DObject);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
				case JSPROP_movement:{
					SET_REF_JSPROP(priv->movement,priv->mjssMovement,mlRMMLMovement,MLMT_MOVEMENT, "movement");
/*
					if(JSVAL_IS_NULL(*vp)){
						priv->movement=NULL;
						return JS_FALSE;
					}else if(JSVAL_IS_OBJECT(*vp)){
						JSObject* jsoMovement=JSVAL_TO_OBJECT(*vp);
						if(mlRMMLMovement::IsInstance(cx,jsoMovement)){
							priv->movement=(mlRMMLMovement*)JS_GetPrivate(cx,jsoMovement);
							if(priv->movement->GetParent()!=priv){
								JS_ReportWarning(cx,"movement-object is of another character-object");
							}
						}else{
//							mlTrace("Error: parameter is not a movement-object\n");
							JS_ReportError(cx, "Parameter is not a movement-object");
							return JS_FALSE;
						}
					}else if(JSVAL_IS_STRING(*vp)){
						JSString* jssMovement=JSVAL_TO_STRING(*vp);
						if(!priv->mbChildrenMLElsCreated){ 
							priv->mjssMovement=jssMovement;
							SAVE_STR_FROM_GC(cx,obj,priv->mjssMovement);
							return JS_TRUE;
						}
						mlRMMLElement* pMLEl=priv->GetChild(jssMovement);
						if(pMLEl==NULL){
							JS_ReportError(cx, "Movement-object not found");
							return JS_FALSE;
						}
						if(pMLEl->mRMMLType!=MLMT_MOVEMENT){
							JS_ReportError(cx, "Named object is not a movement");
							return JS_FALSE;
						}
						priv->movement=(mlRMMLMovement*)pMLEl;
					}
*/
					priv->MovementChanged();
					}break;
				case JSPROP_visemes:{
					SET_REF_JSPROP(priv->visemes,priv->mjssVisemes,mlRMMLVisemes,MLMT_VISEMES, "visemes");
					priv->VisemesChanged();
					}break;
				case JSPROP_idles:{
					SET_REF_JSPROP(priv->idles,priv->mjssIdles,mlRMMLIdles,MLMT_IDLES, "idles");
					priv->IdlesChanged();
					}break;
				case JSPROP_faceIdles:{
					SET_REF_JSPROP(priv->faceIdles,priv->mjssFaceIdles,mlRMMLIdles,MLMT_IDLES, "faceIdles");
					priv->FaceIdlesChanged();
					}break;
				case JSPROP_shadow:{
					if(priv->SetShadowRefJSProp(cx,vp))
						priv->ShadowChanged();
					}break;
				case JSPROP_materials:{
					if(priv->SetMaterialsRefJSProp(cx,vp))
						priv->MaterialsChanged();
					}break;
				case JSPROP_motion:{
					if(priv->SetMotionRefJSProp(cx,vp))
						priv->MotionChanged();
					}break;
				case JSPROP_speech:{
					if(*vp == JSVAL_NULL || *vp == JSVAL_VOID){
						priv->say();
					}else{
						if(!JSVAL_IS_REAL_OBJECT(*vp)){
							JS_ReportError(cx, "Character speech property value must be a Speech or Audio object");
						}else{
							JSObject* jsoSpeech = JSVAL_TO_OBJECT(*vp);
							if(mlRMMLSpeech::IsInstance(cx, jsoSpeech) || mlRMMLAudio::IsInstance(cx, jsoSpeech)){
								mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, jsoSpeech);
								priv->say(pMLEl);
							}else{
								JS_ReportError(cx, "Character speech property value must be a Speech or Audio object");
							}
						}
					}
					*vp = JSVAL_NULL; // ??
					}break;
				case JSPROP_interlocutors:{
					// если дали строку, то надо перебрать все имена персонажей, найти их и добавить их в качестве 
					// собеседников (если сцена уже создана)
					// ??
					}break;
				case JSPROP_gravitation:{
					ML_JSVAL_TO_BOOL(priv->gravitation,*vp);
					priv->GravitationChanged();
					}break;
				case JSPROP_intangible:{
					ML_JSVAL_TO_BOOL(priv->intangible,*vp);
					priv->IntangibleChanged();
					}break;
				case JSPROP_unpickable:{
					ML_JSVAL_TO_BOOL(priv->unpickable,*vp);
					priv->UnpickableChanged();
									   }break;
				case JSPROP_color:{
					priv->Get3DObject()->SetColorJSProp(cx, *vp);
					}break;
				case JSPROP_opacity:{
					priv->Get3DObject()->SetOpacityJSProp(cx, *vp);
					}break;
				case JSPROP_useZSynchronization:{
					ML_JSVAL_TO_BOOL(priv->useZCoordinateSynchonization,*vp);
					priv->SynchronizeZPosition();
					}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

#define GET_REF_JSPROP(OBJ,JSS) \
					if(OBJ!=NULL){ \
						*vp=OBJECT_TO_JSVAL(OBJ->mjso); \
						break; \
					} \
					if(JSS){ \
						*vp=STRING_TO_JSVAL(JSS); \
						break; \
					} \
					*vp=JSVAL_NULL; 

#define GET_REF_JSPROP2(OBJ,JSS,FLG) \
					*vp=JSVAL_NULL; \
					if(priv->btRefFlags & FLG){ \
						if(OBJ!=NULL) \
							*vp=OBJECT_TO_JSVAL(OBJ->mjso); \
					}else{ \
						if(JSS!=NULL) \
							*vp=STRING_TO_JSVAL(JSS); \
					}

///// JavaScript Get Property Wrapper
JSBool mlRMMLCharacter::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLCharacter);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMML3DObject);
		GET_PROTO_PROP(mlRMMLPButton);
		default:;
			switch(iID){
				case JSPROP_movement:{
					GET_REF_JSPROP(priv->movement,priv->mjssMovement)
					}break;
				case JSPROP_visemes:{
					GET_REF_JSPROP(priv->visemes,priv->mjssVisemes)
					}break;
				case JSPROP_idles:{
					GET_REF_JSPROP(priv->idles,priv->mjssIdles)
					}break;
				case JSPROP_shadow:{
					GET_REF_JSPROP2(priv->shadow,priv->mjssShadow,REFF_SHADOW)
					}break;
				case JSPROP_materials:{
					*vp=JSVAL_NULL;
					if(priv->btRefFlags & REFF_MATERIALS){
						if(priv->materials!=NULL)
							*vp=OBJECT_TO_JSVAL(priv->materials->mjso);
					}else{
						if(priv->mjssMaterials!=NULL)
							*vp=STRING_TO_JSVAL(priv->mjssMaterials);
						else{
							priv->CreateDefaultMaterials();
							if(priv->materials!=NULL)
								*vp=OBJECT_TO_JSVAL(priv->materials->mjso);
						}
					}
					}break;
				case JSPROP_motion:{
					GET_REF_JSPROP2(priv->motion,priv->mjssMotion,REFF_MOTION)
					}break;
				case JSPROP_speech:{
					mlMedia* pSpeech = priv->GetSpeech();
					*vp = JSVAL_NULL;
					if(pSpeech != NULL){
						mlRMMLElement* pMLElSpeech = (mlRMMLElement*)pSpeech;
						*vp = OBJECT_TO_JSVAL(pMLElSpeech->mjso);
					}
					}break;
				case EVID_onTurned:
				case EVID_onSaid:
				case EVID_onFalling:
				case EVID_onFallAndStop:
				case EVID_onWreck:
				case EVID_onNeedToChangePosition:
				case EVID_onRightHandPosChanged:
					{
					int iRes = GPSM(cx)->CheckEvent(iID, priv, vp);
					if(iRes >= 0)
						*vp = BOOLEAN_TO_JSVAL(iRes);
					}break;
				case EVID_onLeftHandPosChanged:
					{
					int iRes = GPSM(cx)->CheckEvent(iID, priv, vp);
					if(iRes >= 0)
						*vp = BOOLEAN_TO_JSVAL(iRes);
					}break;
				case JSPROP_interlocutors:{
					mlICharacter** appInterlocutors = priv->GetInterlocutors();
					if(appInterlocutors != NULL){
						mlICharacter** ppInterlocutors = appInterlocutors;
						int iLen = 0;
						for(; *ppInterlocutors != NULL; iLen++, ppInterlocutors++);
						jsval* pjsvals = MP_NEW_ARR( jsval, iLen + 1);
						ppInterlocutors = appInterlocutors;
						for(int ii=0; ii < iLen; ii++, ppInterlocutors++){
							mlRMMLCharacter* pCharMLEl = (mlRMMLCharacter*)(*ppInterlocutors);
							pjsvals[ii] = OBJECT_TO_JSVAL(pCharMLEl->mjso);
						}
						JSObject* jsarr = JS_NewArrayObject(cx, iLen, pjsvals);
						MP_DELETE_ARR( pjsvals);
						*vp = OBJECT_TO_JSVAL(jsarr);
					}
					}break;
				case JSPROP_gravitation:{
					*vp = BOOLEAN_TO_JSVAL(priv->gravitation);
					}break;
				case JSPROP_intangible:{
					*vp = BOOLEAN_TO_JSVAL(priv->intangible);
					}break;
				case JSPROP_unpickable:{
					*vp = BOOLEAN_TO_JSVAL(priv->unpickable);
					}break;
				case JSPROP_color:{
					priv->GetColorJSProp(cx, vp);
				    }break;
				case JSPROP_opacity:{
					priv->GetOpacityJSProp(cx, vp);
					}break;
				case JSPROP_useZSynchronization:{
					*vp = BOOLEAN_TO_JSVAL(priv->useZCoordinateSynchonization);
				}break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_say(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc > 1){
		JS_ReportError(cx,"say-method must got only one parameter");
		return JS_TRUE;
	}
//	if(argv[0]==JSVAL_NULL) return JS_FALSE;
	mlRMMLCharacter* pCharacter=(mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	//
	mlRMMLElement* pSpchMLEl=NULL;
	if(argc > 0 && argv[0]!=JSVAL_NULL && argv[0]!=JSVAL_VOID){
		if(JSVAL_IS_OBJECT(argv[0])){
			JSObject* jsoSpeech=JSVAL_TO_OBJECT(argv[0]);
			pSpchMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jsoSpeech);
			if(pSpchMLEl==NULL){
				JS_ReportError(cx,"say-method parameter must be a speech or an audio element");
				return JS_TRUE;
			}
		}else if(JSVAL_IS_STRING(argv[0])){
			JSString* jssName=JSVAL_TO_STRING(argv[0]);
			pSpchMLEl=pCharacter->FindElemByName(jssName);
			if(pSpchMLEl==NULL){
				JS_ReportError(cx,"Speech/audio element for say-method not found");
				return JS_TRUE;
			}
		}else{
			JS_ReportError(cx,"say-method parameter must an object or a string");
			return JS_TRUE;
		}
	}
	if(pSpchMLEl==NULL){
		bool bRet=pCharacter->say();
		*rval=BOOLEAN_TO_JSVAL(bRet);
		*rval=JS_FALSE;
		return JS_TRUE;
	}
	if(pSpchMLEl->mRMMLType!=MLMT_SPEECH && pSpchMLEl->mRMMLType!=MLMT_AUDIO){
		JS_ReportError(cx,"say-method parameter must be a speech or an audio element");
		return JS_TRUE;
	}
	if(pSpchMLEl->mRMMLType==MLMT_SPEECH){
		mlRMMLSpeech* pMLElSpch=(mlRMMLSpeech*)pSpchMLEl;
		pMLElSpch->ResetActions();
		pCharacter->mpSayingSpeech=pMLElSpch;
		pCharacter->miSayingSpeechIdx=pCharacter->GetChildIdx(pSpchMLEl);
	}
	mlMedia* pSpeech=(mlMedia*)pSpchMLEl;
	//
	SET_SYNCH_FLAG4EVENT(GPSM(cx), pCharacter, mlRMMLCharacter::EVSM_onSaid)
	SET_SYNCH_FLAG4EVENT(GPSM(cx), (mlRMMLElement*)pSpeech, mlRMMLSpeech::EVSM_onSaid)
	int iDurationMS = -1;
	bool bRet=pCharacter->say(pSpeech, &iDurationMS);
	if(iDurationMS > 0)
		GPSM(cx)->UpdateNextUpdateDeltaTime(iDurationMS);
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

bool mlRMMLCharacter::ZShouldBeSynched()
{
	if (mpSynch != NULL && useZCoordinateSynchonization && !mpSynch->IsMyOwn())
		return true;
	return false;
}

void mlRMMLCharacter::CorrectPositionWithSynchronization(ml3DPosition &aPos)
{
	if( ZShouldBeSynched())
	{
		// #1597
		// Alex, BDima 2010-06-07. Добавлена проверка на аттач. Сделана для того чтобы если при перемотке
		// мы попадает на состояние сразу после attach, то сервер хранит Z-координату не зааттаченного объекта
		// и если она большая, то аватар взлетает над креслом.  
		moI3DObject* pmoI3DObject=GetmoI3DObject(); 
		if (pmoI3DObject)
		{
			if( !pmoI3DObject->HasAttachParent())
				aPos.z = mpSynch->GetZ();
		}
	}
}

// Set position from syncronization
void mlRMMLCharacter::SynchronizeZPosition()
{
	ml3DPosition position = GetPos();
	if( ZShouldBeSynched())
	{
		CorrectPositionWithSynchronization( position);
		moI3DObject* pmoI3DObject=GetmoI3DObject();
		if(pmoI3DObject)
		{
			pmoI3DObject->PosChanged(position); 
		}
	}
}

// Для персонажей, которые не являются моим аватаром, позицию Z берем из синхронизации
void mlRMMLCharacter::PosChanged(ml3DPosition &aPos)
{
	CorrectPositionWithSynchronization( aPos);
	mlRMML3DObject::PosChanged(aPos);
}

void mlRMMLCharacter::Said(mlRMMLSpeech* apSpeech){
	mpSayingSpeech=NULL;
	miSayingSpeechIdx=-1;
}

// вызывается из mlRMMLIdles
void mlRMMLCharacter::IdlesSrcChanged(const wchar_t* apwcIdlesName, const wchar_t* apwcSrc){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetICharacter()->IdlesSrcChanged(apwcIdlesName, apwcSrc);
}

JSBool mlRMMLCharacter::JSFUNC_goTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc == 0) return JS_FALSE;
	mlRMMLCharacter* pCharacter=(mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	//
	mlRMMLElement* pObjMLEl=NULL;
	mlRMMLElement* pTurnToObjMLEl=NULL;
	ml3DPosition posTurnTo;
	ml3DPosition* pTurnToPos=NULL;

	unsigned int shiftTime = 0;
	if (argc >= 3 && argv[2]!=JSVAL_NULL && JSVAL_IS_INT(argv[2]))
	{
		shiftTime = (unsigned int)JSVAL_TO_INT(argv[2]);
	}

	if(argc >= 2){
		if(JSVAL_IS_OBJECT(argv[1])){
			JSObject* jso=JSVAL_TO_OBJECT(argv[1]);
			if(mlPosition3D::IsInstance(cx, jso)){
				mlPosition3D* pPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
				posTurnTo=pPos->GetPos();
				pTurnToPos=&posTurnTo;
			}else if(JSO_IS_MLEL(cx, jso)){
				pTurnToObjMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jso);
				if(!(pTurnToObjMLEl->mRMMLType & MLMPT_3DOBJECT)){
					pTurnToObjMLEl=NULL;
				}
			}
		}
	}
	if(argv[0]==JSVAL_NULL){
	}else if(JSVAL_IS_OBJECT(argv[0])){
		JSObject* jso=JSVAL_TO_OBJECT(argv[0]);
		if(mlPosition3D::IsInstance(cx, jso)){
			mlPosition3D* pPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
			if(pPos==NULL) return JS_FALSE;
			ml3DPosition pos=pPos->GetPos();
			bool bRet=pCharacter->goTo(pos,pTurnToPos, (mlMedia*)pTurnToObjMLEl, shiftTime);
			*rval=BOOLEAN_TO_JSVAL(bRet);
			return JS_TRUE;
		}
		pObjMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jso);
	}else if(JSVAL_IS_STRING(argv[0])){
		JSString* jssName=JSVAL_TO_STRING(argv[0]);
		pObjMLEl=pCharacter->FindElemByName(jssName);
	}
	if(pObjMLEl!=NULL){
		if(!(pObjMLEl->mRMMLType & MLMPT_3DOBJECT)){
			JS_ReportError(cx,"Character can go to 3D-object only");
			return JS_FALSE;
		}
	}
	mlMedia* pObject=(mlMedia*)pObjMLEl;
	//

	bool bRet=pCharacter->goTo(pObject,pTurnToPos, (mlMedia*)pTurnToObjMLEl, shiftTime);
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_turnTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc != 1) return JS_FALSE;
	mlRMMLCharacter* pCharacter=(mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	//
	mlRMMLElement* pObjMLEl=NULL;
	SET_SYNCH_FLAG4EVENT(GPSM(cx), pCharacter, mlRMMLCharacter::EVSM_onTurned)
	if(argv[0]==JSVAL_NULL){
	}else if(JSVAL_IS_OBJECT(argv[0])){
		JSObject* jso=JSVAL_TO_OBJECT(argv[0]);
		if(mlPosition3D::IsInstance(cx, jso)){
			mlPosition3D* pPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
			if(pPos==NULL) return JS_FALSE;
			ml3DPosition pos=pPos->GetPos();
			bool bRet=pCharacter->turnTo(pos);
			*rval=BOOLEAN_TO_JSVAL(bRet);
			return JS_TRUE;
		}
		if(!JSO_IS_MLEL(cx, jso)){
			JS_ReportError(cx,"Character can turn to 3D-object only");
			return JS_FALSE;
		}
		pObjMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jso);
	}else if(JSVAL_IS_INT(argv[0])){
		int iFlags=JSVAL_TO_INT(argv[0]);
		bool bRet=pCharacter->turnTo(iFlags);
		*rval=BOOLEAN_TO_JSVAL(bRet);
		return JS_TRUE;
	}else if(JSVAL_IS_STRING(argv[0])){
		JSString* jssName=JSVAL_TO_STRING(argv[0]);
		pObjMLEl=pCharacter->FindElemByName(jssName);
	}
	if(pObjMLEl!=NULL){
		if(!(pObjMLEl->mRMMLType & MLMPT_3DOBJECT)){
			JS_ReportError(cx,"Character can turn to 3D-object only");
			return JS_FALSE;
		}
	}
	mlMedia* pObject=(mlMedia*)pObjMLEl;
	//
	bool bRet=pCharacter->turnTo(pObject);
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_lookAt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 1) return JS_FALSE;
	mlRMMLCharacter* pCharacter=(mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	int iMs=-1;
	if(argc >= 2){
		if(JSVAL_IS_NUMBER(argv[1])){
			long lMs=-1;
			JS_ValueToInt32(cx,argv[1],&lMs);
			iMs=(int)lMs;
		}
	}
	double dKoef=1.0;
	if(argc >= 3){
		if(JSVAL_IS_NUMBER(argv[2])){
			JS_ValueToNumber(cx,argv[2],&dKoef);
		}
	}
	//
	mlRMMLElement* pObjMLEl=NULL;
	if(argv[0]==JSVAL_NULL){
	}else if(JSVAL_IS_OBJECT(argv[0])){
		JSObject* jso=JSVAL_TO_OBJECT(argv[0]);
		if(mlPosition3D::IsInstance(cx, jso)){
			mlPosition3D* pPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
			if(pPos==NULL) return JS_FALSE;
			ml3DPosition pos=pPos->GetPos();
			bool bRet=pCharacter->lookAt(pos, iMs, dKoef);
			*rval=BOOLEAN_TO_JSVAL(bRet);
			return JS_TRUE;
		}
		pObjMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jso);
	}else if(JSVAL_IS_STRING(argv[0])){
		JSString* jssName=JSVAL_TO_STRING(argv[0]);
		pObjMLEl=pCharacter->FindElemByName(jssName);
	}
	if(pObjMLEl!=NULL){
		if(!(pObjMLEl->mRMMLType & MLMPT_3DOBJECT)){
			JS_ReportError(cx,"Character can look at 3D-object only");
			return JS_FALSE;
		}
	}
	if(pObjMLEl != NULL && PMO_IS_NULL(pObjMLEl->GetMO())){
		JS_ReportWarning(cx,"Character can't look at deleted 3D-object");
		*rval = BOOLEAN_TO_JSVAL(false);
		return JS_TRUE;
	}
	mlMedia* pObject=(mlMedia*)pObjMLEl;
	//
	bool bRet = pCharacter->lookAt(pObject, iMs, dKoef);
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_lookAtByEyes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 1){
		JS_ReportError(cx, "lookAtByEyes-method must get one argument at least");
		return JS_TRUE;
	}
	mlRMMLCharacter* pCharacter=(mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	int iMs=-1;
	if(argc >= 2){
		if(JSVAL_IS_NUMBER(argv[1])){
			long lMs=-1;
			JS_ValueToInt32(cx,argv[1],&lMs);
			iMs=(int)lMs;
		}
	}
	double dKoef=1.0;
	if(argc >= 3){
		if(JSVAL_IS_NUMBER(argv[2])){
			JS_ValueToNumber(cx,argv[2],&dKoef);
		}
	}
	//
	mlRMMLElement* pObjMLEl=NULL;
	if(argv[0]==JSVAL_NULL){
	}else if(JSVAL_IS_OBJECT(argv[0])){
		JSObject* jso=JSVAL_TO_OBJECT(argv[0]);
		if(mlPosition3D::IsInstance(cx, jso)){
			mlPosition3D* pPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
			if(pPos==NULL) return JS_FALSE;
			ml3DPosition pos=pPos->GetPos();
			bool bRet=pCharacter->lookAtByEyes(pos, iMs, dKoef);
			*rval=BOOLEAN_TO_JSVAL(bRet);
			return JS_TRUE;
		}
		pObjMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jso);
	}else if(JSVAL_IS_STRING(argv[0])){
		JSString* jssName=JSVAL_TO_STRING(argv[0]);
//		pObjMLEl = pCharacter->FindElemByName(jssName);
		bool bRet = pCharacter->lookAtByEyes(wr_JS_GetStringChars(jssName), iMs, dKoef);
		*rval = BOOLEAN_TO_JSVAL(bRet);
		return JS_TRUE;
	}
	if(pObjMLEl!=NULL){
		if(!(pObjMLEl->mRMMLType & MLMPT_3DOBJECT)){
			JS_ReportError(cx,"Character can look at 3D-object only");
			return JS_FALSE;
		}
	}
	mlMedia* pObject=(mlMedia*)pObjMLEl;
	//
	bool bRet = pCharacter->lookAtByEyes(pObject, iMs, dKoef);
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_addInterlocutor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc ==0 || argc > 1){
		JS_ReportError(cx,"addInterlocutor-method must get only one parameter");
		return JS_TRUE;
	}
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	mlRMMLCharacter* pInterlocutor = NULL;

	mlRMMLElement* pInterlocutorMLEl=NULL;
	if(argv[0]==JSVAL_NULL){
	}else if(JSVAL_IS_REAL_OBJECT(argv[0])){
		JSObject* jso = JSVAL_TO_OBJECT(argv[0]);
		pInterlocutorMLEl = (mlRMMLElement*)JS_GetPrivate(cx, jso);
	}else if(JSVAL_IS_STRING(argv[0])){
		JSString* jssName=JSVAL_TO_STRING(argv[0]);
		pInterlocutorMLEl = pCharacter->FindElemByName2(jssName);
	}
	if(pInterlocutorMLEl != NULL){
		if(!(pInterlocutorMLEl->mRMMLType & MLMT_CHARACTER)){
			JS_ReportError(cx,"addInterlocutor-method must get a character object as argument");
			return JS_TRUE;
		}
		pInterlocutor = (mlRMMLCharacter*)pInterlocutorMLEl;
	}

	bool bRet = pCharacter->addInterlocutor(pInterlocutor);
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_removeInterlocutor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc ==0 || argc > 1){
		JS_ReportError(cx,"removeInterlocutor-method must get only one parameter");
		return JS_TRUE;
	}
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	mlRMMLCharacter* pInterlocutor = NULL;

	mlRMMLElement* pInterlocutorMLEl=NULL;
	if(argv[0]==JSVAL_NULL){
	}else if(JSVAL_IS_REAL_OBJECT(argv[0])){
		JSObject* jso = JSVAL_TO_OBJECT(argv[0]);
		pInterlocutorMLEl = (mlRMMLElement*)JS_GetPrivate(cx, jso);
	}else if(JSVAL_IS_STRING(argv[0])){
		JSString* jssName=JSVAL_TO_STRING(argv[0]);
		pInterlocutorMLEl = pCharacter->FindElemByName2(jssName);
	}
	if(pInterlocutorMLEl != NULL){
		if(!(pInterlocutorMLEl->mRMMLType & MLMT_CHARACTER)){
			JS_ReportError(cx,"removeInterlocutor-method must got a character object as argument");
			return JS_TRUE;
		}
		pInterlocutor = (mlRMMLCharacter*)pInterlocutorMLEl;
	}

	bool bRet = pCharacter->removeInterlocutor(pInterlocutor);
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

mlRMMLElement* mlRMMLCharacter::FindVisemes(jsval ajsvVisemes, const char* apcMethodName){
	mlRMMLElement* pVisemesMLEl = NULL;
	if(ajsvVisemes == JSVAL_NULL || ajsvVisemes == JSVAL_VOID)
		return NULL;
	if(JSVAL_IS_OBJECT(ajsvVisemes)){
		JSObject* jsoVisemes = JSVAL_TO_OBJECT(ajsvVisemes);
		pVisemesMLEl = (mlRMMLElement*)JS_GetPrivate(mcx, jsoVisemes);
		if(pVisemesMLEl == NULL || pVisemesMLEl->mRMMLType != MLMT_VISEMES){
			JS_ReportError(mcx,"First setViseme-method parameter must be visemes object", apcMethodName);
			return NULL;
		}
	}else if(JSVAL_IS_STRING(ajsvVisemes)){
		JSString* jssName = JSVAL_TO_STRING(ajsvVisemes);
		pVisemesMLEl = FindElemByName(jssName);
		if(pVisemesMLEl == NULL){
			jschar* jscName = JS_GetStringChars(jssName);
			JS_ReportError(mcx,"Visemes \"%s\" for %s-method not found, cLPCSTR(jscName), apcMethodName");
			return NULL;
		}
	}else{
		JS_ReportError(mcx,"First %s-method parameter must be visemes object or a string", apcMethodName);
		return NULL;
	}
	return pVisemesMLEl;
}

JSBool mlRMMLCharacter::JSFUNC_setViseme(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 2){
		JS_ReportError(cx,"setViseme-method must get two arguments at least");
		return JS_TRUE;
	}
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	mlRMMLVisemes* pVisemes = NULL;

	mlRMMLElement* pVisemesMLEl = pCharacter->FindVisemes(argv[0], "setViseme");
	if(pVisemesMLEl != NULL)
		pVisemes = (mlRMMLVisemes*)pVisemesMLEl;

	if(!JSVAL_IS_INT(argv[1])){
		JS_ReportError(cx,"Second setViseme-method must be an integer");
		return JS_TRUE;
	}
	unsigned int iIndex = JSVAL_TO_INT(argv[1]);

	int iTime = -1;
	if(argc > 2){
		if(!JSVAL_IS_INT(argv[2])){
			JS_ReportError(cx,"Third setViseme-method must be an integer");
			return JS_TRUE;
		}
		iTime = JSVAL_TO_INT(argv[2]);
	}
	int iKeepupTime = -1;
	if(argc > 3){
		if(!JSVAL_IS_INT(argv[3])){
			JS_ReportError(cx,"Fourth setViseme-method must be an integer");
			return JS_TRUE;
		}
		iKeepupTime = JSVAL_TO_INT(argv[3]);
	}

	// установить выражение лица с индексом auiIndex, взятое из apVisemes.
	// aiTime - период времени установки/сброса, 
	// aiKeepupTime - период времени, сколько "держать" это выражение на лице
	bool bRet = pCharacter->setViseme(pVisemes, iIndex, iTime, iKeepupTime);

	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_setLexeme(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if(argc < 2){
			JS_ReportError(cx,"setLexeme-method must get two arguments");
			return JS_TRUE;
		}
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

		unsigned int iIndex = JSVAL_TO_INT(argv[0]);
		unsigned int iTime = 0;
		if(JSVAL_IS_NUMBER(argv[1]))
		{
			double dTime=0.0;
			JS_ValueToNumber(cx,argv[1],&dTime);
			iTime = dTime;
		}
		else
		{
			JS_ReportError(cx,"setLexeme-method second parameter must be an integer");
			return JS_TRUE;
		}
		

		// установить выражение лица с индексом auiIndex, взятое из apVisemes.
		// aiTime - период времени установки/сброса, 
		// aiKeepupTime - период времени, сколько "держать" это выражение на лице
		bool bRet = pCharacter->setLexeme( iIndex, iTime);

		*rval=BOOLEAN_TO_JSVAL(bRet);
		return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_setSleepingState(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if(argc < 1){
			JS_ReportError(cx,"JSFUNC_setSleepingState-method must get 1 argument");
			return JS_TRUE;
		}
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

		bool bSleep = JSVAL_TO_BOOLEAN(argv[0]);
		
		pCharacter->setSleepingState(bSleep);

		*rval=BOOLEAN_TO_JSVAL(true);
		return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_setRightHandTracking(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 1)
	{
		JS_ReportError(cx,"JSFUNC_setRightHandTracking-method must get 1 argument");
		return JS_TRUE;
	}
	
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	bool bTracking = JSVAL_TO_BOOLEAN(argv[0]);
	pCharacter->SetTrackingRightHand(bTracking);

	*rval=BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_setLeftHandTracking(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 1)
	{
		JS_ReportError(cx,"JSFUNC_setLeftHandTracking-method must get 1 argument");
		return JS_TRUE;
	}
	
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	bool bTracking = JSVAL_TO_BOOLEAN(argv[0]);
	pCharacter->SetTrackingLeftHand(bTracking);

	*rval=BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_unsetViseme(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 2){
		JS_ReportError(cx,"unsetViseme-method must get two arguments at least");
		return JS_TRUE;
	}
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	mlRMMLVisemes* pVisemes = NULL;

	mlRMMLElement* pVisemesMLEl = pCharacter->FindVisemes(argv[0], "unsetViseme");
	if(pVisemesMLEl != NULL)
		pVisemes = (mlRMMLVisemes*)pVisemesMLEl;

	if(!JSVAL_IS_INT(argv[1])){
		JS_ReportError(cx,"Second argument of unsetViseme-method must be an integer");
		return JS_TRUE;
	}
	unsigned int iIndex = JSVAL_TO_INT(argv[1]);

	int iTime = -1;
	if(argc > 2){
		if(!JSVAL_IS_INT(argv[2])){
			JS_ReportError(cx,"Third argument of unsetViseme-method must be an integer");
			return JS_TRUE;
		}
		iTime = JSVAL_TO_INT(argv[2]);
	}

	// убрать с лица выражение с индексом auiIndex, взятое из apVisemes.
	// aiTime - период времени "убирания"
	bool bRet = pCharacter->unsetViseme(pVisemes, iIndex, iTime);

	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_doVisemes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 1){
		JS_ReportError(cx,"doVisemes-method must get one argument at least");
		return JS_TRUE;
	}
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	mlRMMLVisemes* pVisemes = NULL;

	mlRMMLElement* pVisemesMLEl = pCharacter->FindVisemes(argv[0], "doVisemes");
	if(pVisemesMLEl != NULL)
		pVisemes = (mlRMMLVisemes*)pVisemesMLEl;

	int iTime = -1;
	if(argc > 1){
		if(!JSVAL_IS_INT(argv[1])){
			JS_ReportError(cx,"Second doVisemes-method must be an integer");
			return JS_TRUE;
		}
		iTime = JSVAL_TO_INT(argv[1]);
	}

	// убрать с лица выражение с индексом auiIndex, взятое из apVisemes.
	// aiTime - период времени "убирания"
	bool bRet = pCharacter->doVisemes(pVisemes, iTime);

	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

// установить персонажа в свободное место в области, заданной аргументами:
// apStartPos - начальная позиция, adRadius - радиус области
// abInRoom - поместить в пределах комнаты
JSBool mlRMMLCharacter::JSFUNC_setToRandomFreePlace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	ml3DPosition* pStartPos = &pos;
	if(argc > 0){
		if(JSVAL_IS_REAL_OBJECT(argv[0])){
			JSObject* jsoPos = JSVAL_TO_OBJECT(argv[0]);
			if(mlPosition3D::IsInstance(cx,jsoPos)){
				mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(cx, jsoPos);
				pos = pPos3D->GetPos();
			}else{
				JS_ReportError(cx, "First argument of setToRandomFreePlace-method must be a Position3D-object");
				return JS_TRUE;
			}
		}else if(JSVAL_IS_STRING(argv[0])){
			JSString* jssPos = JSVAL_TO_STRING(argv[0]);
			if(!mlRMML3DObject::ParsePos(wr_JS_GetStringChars(jssPos), &pos)){
				JS_ReportError(cx, "Wrong first argument of setToRandomFreePlace-method");
				return JS_TRUE;
			}
		}else{
			JS_ReportError(cx, "First argument of setToRandomFreePlace-method must be a Position3D-object");
			return JS_TRUE;
		}
	}
	double dRadius = 0;
	if(argc > 1){
		if(JSVAL_IS_INT(argv[1])){
			dRadius = (double)(JSVAL_TO_INT(argv[1]));
		}else if(JSVAL_IS_DOUBLE(argv[1])){
			dRadius = *JSVAL_TO_DOUBLE(argv[1]);
		}else{
			JS_ReportError(cx, "Second argument of setToRandomFreePlace-method must be a number");
			return JS_TRUE;
		}
	}
	bool bInRoom = false;
	if(argc > 2){
		if(JSVAL_IS_BOOLEAN(argv[2])){
			bInRoom = JSVAL_TO_BOOLEAN(argv[2]);
		}else{
			JS_ReportError(cx, "Third argument of setToRandomFreePlace-method must be a boolean");
			return JS_TRUE;
		}
	}

	bool bRet = pCharacter->setToRandomFreePlace(pStartPos, dRadius, bInRoom);

	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

bool GetShiftTimeArg(JSContext *cx, uintN argc, jsval *argv, unsigned int &aiShiftTime, const char* apcFuncName){
	aiShiftTime = 0;
	if(argc > 0){
		if(JSVAL_IS_INT(argv[0])){
			aiShiftTime = (unsigned int)(JSVAL_TO_INT(argv[0]));
		}else{
			JS_ReportError(cx, "First argument of %s-method must be a integer number", apcFuncName);
			return false;
		}
	}
	return true;
}

//JSFUNC(goForward, 0) // иди вперед
JSBool mlRMMLCharacter::JSFUNC_goForward(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "goForward"))
		return JS_TRUE;

	bool bRet = pCharacter->goForward(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//
JSBool mlRMMLCharacter::JSFUNC_toInitialPose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	pCharacter->toInitialPose();

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

//
JSBool mlRMMLCharacter::JSFUNC_fromInitialPose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	pCharacter->fromInitialPose();

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}


//JSFUNC(goLeft, 1) // иди вперед и налево
JSBool mlRMMLCharacter::JSFUNC_goLeft(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "goLeft"))
		return JS_TRUE;

	bool bRet = pCharacter->goLeft(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(goRight, 1) // иди вперед и направо
JSBool mlRMMLCharacter::JSFUNC_goRight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "goRight"))
		return JS_TRUE;

	bool bRet = pCharacter->goRight(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(goBackward, 0) // пяться
JSBool mlRMMLCharacter::JSFUNC_goBackward(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "goBackward"))
		return JS_TRUE;

	bool bRet = pCharacter->goBackward(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(goBackLeft, 1) // иди назад и налево
JSBool mlRMMLCharacter::JSFUNC_goBackLeft(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "goBackLeft"))
		return JS_TRUE;

	bool bRet = pCharacter->goBackLeft(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(goBackRight, 1) // иди назад и направо
JSBool mlRMMLCharacter::JSFUNC_goBackRight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "goBackRight"))
		return JS_TRUE;

	bool bRet = pCharacter->goBackRight(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(strafeLeft, 0) // иди боком влево
JSBool mlRMMLCharacter::JSFUNC_strafeLeft(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "strafeLeft"))
		return JS_TRUE;

	bool bRet = pCharacter->strafeLeft(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(strafeRight, 0) // иди боком вправо
JSBool mlRMMLCharacter::JSFUNC_strafeRight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "strafeRight"))
		return JS_TRUE;

	bool bRet = pCharacter->strafeRight(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(stopGo, 0) // перестань идти (как goTo(NULL))
JSBool mlRMMLCharacter::JSFUNC_stopGo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	bool bRet = pCharacter->stopGo();

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(turnLeft, 0) // поворачивайся влево
JSBool mlRMMLCharacter::JSFUNC_turnLeft(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "turnLeft"))
		return JS_TRUE;

	bool bRet = pCharacter->turnLeft(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(turnRight, 0) // поворачивайся вправо
JSBool mlRMMLCharacter::JSFUNC_turnRight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	unsigned int shiftTime = 0;
	if(!GetShiftTimeArg(cx, argc, argv, shiftTime, "turnRight"))
		return JS_TRUE;

	bool bRet = pCharacter->turnRight(shiftTime);

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(stopTurn, 0) // перестань поворачиваться
JSBool mlRMMLCharacter::JSFUNC_stopTurn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	bool bRet = pCharacter->stopTurn();

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(sayRecordedVoice, 0) // проиграть записанный голос
JSBool mlRMMLCharacter::JSFUNC_sayRecordedVoice(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	if(argc < 2)
	{
		JS_ReportError(cx,"sayRecordedVoice-method must two or more parameters");
		return JS_FALSE;
	}

	if( argv[0]!=JSVAL_NULL && argv[0]!=JSVAL_VOID && !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"First parameter of sayRecordedVoice(..) must be a string (name of the file) or null");
		return JS_FALSE;
	}

	if( argv[1]!=JSVAL_NULL && argv[1]!=JSVAL_VOID && !JSVAL_IS_STRING(argv[1]))
	{
		JS_ReportError(cx,"Second parameter of sayRecordedVoice(..) must be a string (name communicationArea) or null");
		return JS_FALSE;
	}

	JSString* jssFileName = NULL;
	JSString* jssCommunicationAreaName = NULL;

	if( JSVAL_IS_STRING(argv[0]))
		jssFileName = JSVAL_TO_STRING(argv[0]);

	if( JSVAL_IS_STRING(argv[1]))
		jssCommunicationAreaName = JSVAL_TO_STRING(argv[1]);	

	unsigned int shiftTime = 0;
	if (argc > 2 && argv[2]!=JSVAL_NULL && JSVAL_IS_INT(argv[2]))
	{
		shiftTime = JSVAL_TO_INT(argv[2]);
	}	

	wchar_t* lpwcUserName = pCharacter->GetStringProp("name");
	bool bRet = pCharacter->sayRecordedVoice( lpwcUserName? cLPCSTR(lpwcUserName) : (const char*)NULL
											, jssFileName? cLPCSTR(jssFileName) : (const char*)NULL
											, jssCommunicationAreaName? cLPCSTR(jssCommunicationAreaName) : (const char*)""
											, shiftTime
											, pCharacter->GetServerVersion());

	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

//JSFUNC(getFaceVector, 0) получить FaceVector аватара
JSBool mlRMMLCharacter::JSFUNC_getFaceVector(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	float x=0, y=0, z=0;
	pCharacter->getFaceVector(x, y, z);
	
	ml3DPosition p3dRet;
	p3dRet.x = x;
	p3dRet.y = y;
	p3dRet.z = z;
	JSObject* jsoPos = mlPosition3D::newJSObject(cx);
	mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx,jsoPos);
	pPos->SetPos(p3dRet);
	
	*rval = OBJECT_TO_JSVAL(jsoPos);
	return JS_TRUE;
}

// pointByHandTo([point3D[,object3D]]) - показать рукой на заданную точку
JSBool mlRMMLCharacter::JSFUNC_pointByHandTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL;
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;

	mlRMMLElement* pObjMLEl = NULL;
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);

	if(argc == 0 || argv[0] == JSVAL_NULL){
	}else{
		for(int i = 0; i < 2; i++){
			if(i >= (int)argc)
				break;
			jsval varg = argv[i];
			if(JSVAL_IS_OBJECT(varg)){
				JSObject* jso = JSVAL_TO_OBJECT(varg);
				if(mlPosition3D::IsInstance(cx, jso)){
					mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx,jso);
					if(pPos == NULL) return JS_FALSE;
					pos = pPos->GetPos();
				}else if(JSO_IS_MLEL(cx, jso)){
					pObjMLEl = (mlRMMLElement*)JS_GetPrivate(cx, jso);
				}else{
					JS_ReportWarning(cx,"Character can point to 3D-object only");
					return JS_TRUE;
				}
			}else if(JSVAL_IS_STRING(varg)){
				JSString* jssName = JSVAL_TO_STRING(varg);
				pObjMLEl = pCharacter->FindElemByName(jssName);
			}
		}
	}
	if(pObjMLEl != NULL){
		if(!(pObjMLEl->mRMMLType & MLMPT_3DOBJECT)){
			JS_ReportWarning(cx,"Character can point to 3D-object only");
			return JS_TRUE;
		}
		if(PMO_IS_NULL(pObjMLEl->GetMO())){
			JS_ReportWarning(cx,"Character can't point to deleted 3D-object");
			return JS_TRUE;
		}
	}
	mlMedia* pObject = (mlMedia*)pObjMLEl;
	//
	bool bRet = pCharacter->pointByHandTo(pos, pObject);
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_setKinectDeskPoints(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL;
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;

	mlRMMLElement* pObjMLEl = NULL;
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	ml3DPosition pos2; ML_INIT_3DPOSITION(pos2);
	ml3DRotation rot; ML_INIT_3DROTATION(rot);
	mlRMMLObject* p3DObj = NULL;

	if(argc < 3 || argv[0] == JSVAL_NULL || argv[1] == JSVAL_NULL || argv[2] == JSVAL_NULL){
	}else{
		for(int i = 0; i < 2; i++){
			jsval varg = argv[i];
			if(JSVAL_IS_OBJECT(varg)){
				JSObject* jso = JSVAL_TO_OBJECT(varg);
				if(mlPosition3D::IsInstance(cx, jso))
				{
					mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx,jso);
					if(pPos == NULL) return JS_FALSE;
					if (i == 0)
					{
						pos = pPos->GetPos();
					}
					else
					{
						pos2 = pPos->GetPos();
					}
				}
				else
				{
					JS_ReportWarning(cx,"setKinectDeskPoints has two 3d positions and 3d rotation as parameters");
					return JS_TRUE;
				}
			}else if(JSVAL_IS_STRING(varg)){
				JSString* jssName = JSVAL_TO_STRING(varg);
				pObjMLEl = pCharacter->FindElemByName(jssName);
			}
		}
		if (JSVAL_IS_REAL_OBJECT(argv[2]))
		{
			JSObject* jso = JSVAL_TO_OBJECT(argv[2]);
			if(mlRMMLObject::IsInstance(cx, jso))
			{
				p3DObj = (mlRMMLObject*)JS_GetPrivate(cx,jso);
				if (p3DObj == NULL) return JS_FALSE;
				moMedia* p3DO_MO = p3DObj->GetMO();
				if(PMO_IS_NULL(p3DO_MO)){
					JS_ReportWarning(cx,"setKinectDeskPoints must get a live object argument");
					return JS_TRUE;
				}
			}
		}
	}

	if(p3DObj == NULL){
		JS_ReportWarning(cx,"setKinectDeskPoints must get a live object argument");
		return JS_TRUE;
	}
	bool bRet = pCharacter->setKinectDeskPoints(pos, pos2, p3DObj->GetMO());
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLCharacter::JSFUNC_disableKinectDesk(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL;
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)JS_GetPrivate(cx, obj);

	if (pCharacter)
	{
		pCharacter->disableKinectDesk();
	}

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

#define MLCH_SET_REF(jss,var,MLELPN) \
if(jss!=NULL && var==NULL){ \
		jsval v=STRING_TO_JSVAL(jss); \
		JS_SetProperty(mcx,mjso,MLELPN,&v); \
		FREE_STR_FOR_GC(mcx,mjso,jss); \
		jss=NULL; \
	}

// Реализация mlRMMLElement
mlresult mlRMMLCharacter::CreateMedia(omsContext* amcx){
	// set movement
	mbChildrenMLElsCreated=true;
	MLCH_SET_REF(mjssMovement,movement,MLELPN_MOVEMENT);
	MLCH_SET_REF(mjssVisemes,visemes,MLELPN_VISEMES);
	MLCH_SET_REF(mjssIdles,idles,MLELPN_IDLES);
//	MLCH_SET_REF(mjssShadow,shadow,MLELPN_SHADOW);
	mpMO=PMOV_CREATED;
	if(mjssShadow!=NULL && !(btRefFlags & REFF_SHADOW)){
		jsval v=STRING_TO_JSVAL(mjssShadow);
		JS_SetProperty(mcx,mjso,MLELPN_SHADOW,&v); 
	}
	if(mjssMaterials!=NULL && !(btRefFlags & REFF_MATERIALS)){
		jsval v=STRING_TO_JSVAL(mjssMaterials);
		JS_SetProperty(mcx,mjso,MLELPN_MATERIALS,&v); 
	}
	if(mjssMotion!=NULL && !(btRefFlags & REFF_MOTION)){
		jsval v=STRING_TO_JSVAL(mjssMotion);
		JS_SetProperty(mcx,mjso,MLELPN_MOTION,&v); 
	}
	mpMO=NULL; // ??
	//
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLCharacter::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(PMO_IS_NULL(mpMO)) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLCharacter::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded
		return ML_ERROR_FAILURE;
	}else{
		SetPSR2MO();
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLCharacter::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLCharacter* pNewEL=(mlRMMLCharacter*)GET_RMMLEL(mcx, mlRMMLCharacter::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

mlresult mlRMMLCharacter::Update(const __int64 alTime){
	// если персонаж говорит
	if(mpSayingSpeech!=NULL){
		// проверка mpSayingSpeech на валидность
		if(miSayingSpeechIdx >= 0){
			if(GetChildByIdx(miSayingSpeechIdx)!=mpSayingSpeech){
				if(!IsChild(mpSayingSpeech)) return ML_ERROR_FAILURE;
			}
		}else{
			if(!IsChild(mpSayingSpeech)) return ML_ERROR_FAILURE;
		}
		//
		mpSayingSpeech->PerformCurActions();
	}
	return ML_OK;
}

bool mlRMMLCharacter::Freeze(){
	if(!mlRMMLElement::Freeze()) return false; 
	// ??
	return true;
}

bool mlRMMLCharacter::Unfreeze(){
	if(!mlRMMLElement::Unfreeze()) return false; 
	// ??
	return true;
}

// реализация mlICharacter
mlMedia* mlRMMLCharacter::GetCloud(){
	return (mlMedia*)GetChild(MLMT_CLOUD);
}

mlIMovement* mlRMMLCharacter::GetMovement(){ 
	return (mlIMovement*)movement; 
}

bool mlRMMLCharacter::GetUnpickable()
{
	return unpickable;
}

mlMedia* mlRMMLCharacter::GetVisemes(){ 
	return (mlMedia*)visemes; 
}

mlMedia* mlRMMLCharacter::GetIdles(){ 
	return (mlMedia*)idles; 
}

mlIIdles* mlRMMLCharacter::GetFaceIdles(){ 
	return (mlIIdles*)faceIdles; 
}

MLEVENT_IMPL(mlRMMLCharacter,onTurned);
MLEVENT_IMPL(mlRMMLCharacter,onFalling);
MLEVENT_IMPL(mlRMMLCharacter,onFallAndStop);
MLEVENT_IMPL(mlRMMLCharacter,onWreck);
//MLEVENT_IMPL(mlRMMLCharacter,onNeedToChangePosition);
void mlRMMLCharacter::onNeedToChangePosition(){
	GUARD_JS_CALLS(mcx);
	GPSM(mcx)->mbDontSendSynchEvents = false;
	GetElem_mlRMMLCharacter()->CallListeners(EVID_onNeedToChangePosition);
}

//MLEVENT_IMPL(mlRMMLCharacter,onSaid);
void mlRMMLCharacter::onSaid(){
	GUARD_JS_CALLS(mcx);
	GPSM(mcx)->mbDontSendSynchEvents = (mucSynchFlags4Events & EVSM_onSaid)?true:false;
	CallListeners(EVID_onSaid);
}

void mlRMMLCharacter::onRightHandPosChanged(ml3DPosition aRightHandPos, ml3DPosition aRightHandDir, 
		ml3DPosition aRightHandDirExt)
{
	GUARD_JS_CALLS(mcx);

	JSObject* jsoPos = mlPosition3D::newJSObject(mcx);
	mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(mcx, jsoPos);
	pPos3D->SetPos(aRightHandPos);
	jsval v = OBJECT_TO_JSVAL(jsoPos);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "rightHandPosition", &v);

	JSObject* jsoPos2 = mlPosition3D::newJSObject(mcx);
	mlPosition3D* pPos3D2 = (mlPosition3D*)JS_GetPrivate(mcx, jsoPos2);
	pPos3D2->SetPos(aRightHandDir);
	v = OBJECT_TO_JSVAL(jsoPos2);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "rightHandDirection", &v);

	if ((aRightHandDirExt.x != 0.0) || (aRightHandDirExt.y != 0.0) || (aRightHandDirExt.z != 0.0))
	{
		JSObject* jsoPos3 = mlPosition3D::newJSObject(mcx);
		mlPosition3D* pPos3D3 = (mlPosition3D*)JS_GetPrivate(mcx, jsoPos3);
		pPos3D3->SetPos(aRightHandDirExt);
		v = OBJECT_TO_JSVAL(jsoPos3);
	}
	else
	{
		v = JSVAL_NULL;
	}
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "rightHandDirectionExtended", &v);
	//
	CallListeners(EVID_onRightHandPosChanged);
	//
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "rightHandPosition");
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "rightHandDirection");
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "rightHandDirectionExtended");
}

void mlRMMLCharacter::onLeftHandPosChanged(ml3DPosition aLeftHandPos, ml3DPosition aLeftHandDir, 
		ml3DPosition aLeftHandDirExt)
{
	GUARD_JS_CALLS(mcx);

	JSObject* jsoPos = mlPosition3D::newJSObject(mcx);
	mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(mcx, jsoPos);
	pPos3D->SetPos(aLeftHandPos);
	jsval v = OBJECT_TO_JSVAL(jsoPos);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "leftHandPosition", &v);

	JSObject* jsoPos2 = mlPosition3D::newJSObject(mcx);
	mlPosition3D* pPos3D2 = (mlPosition3D*)JS_GetPrivate(mcx, jsoPos2);
	pPos3D2->SetPos(aLeftHandDir);
	v = OBJECT_TO_JSVAL(jsoPos2);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "leftHandDirection", &v);

	if ((aLeftHandDirExt.x != 0.0) || (aLeftHandDirExt.y != 0.0) || (aLeftHandDirExt.z != 0.0))
	{
		JSObject* jsoPos3 = mlPosition3D::newJSObject(mcx);
		mlPosition3D* pPos3D3 = (mlPosition3D*)JS_GetPrivate(mcx, jsoPos3);
		pPos3D3->SetPos(aLeftHandDirExt);
		v = OBJECT_TO_JSVAL(jsoPos3);
	}
	else
	{
		v = JSVAL_NULL;
	}
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "leftHandDirectionExtended", &v);
	//
	CallListeners(EVID_onLeftHandPosChanged);
	//
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "leftHandPosition");
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "leftHandDirection");
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "leftHandDirectionExtended");
}

// реализация moICharacter
mlMedia* mlRMMLCharacter::GetSpeech(){
	if(PMO_IS_NULL(mpMO)) return NULL;
	return mpMO->GetICharacter()->GetSpeech();
}
void mlRMMLCharacter::MovementChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetICharacter()->MovementChanged();
}
void mlRMMLCharacter::UnpickableChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetICharacter()->UnpickableChanged();
}
void mlRMMLCharacter::VisemesChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetICharacter()->VisemesChanged();
}
void mlRMMLCharacter::IdlesChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetICharacter()->IdlesChanged();
}
void mlRMMLCharacter::FaceIdlesChanged(){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetICharacter()->FaceIdlesChanged();
}
mlICharacter** mlRMMLCharacter::GetInterlocutors(){
	if(PMO_IS_NULL(mpMO)) return NULL;
	return mpMO->GetICharacter()->GetInterlocutors();
}

#define MOICH_METH_BODY_IMPL \
	if(PMO_IS_NULL(mpMO)) return false; \
	return mpMO->GetICharacter()
bool mlRMMLCharacter::say(mlMedia* apSpeech, int* apDuration){
	MOICH_METH_BODY_IMPL->say(apSpeech, apDuration);
}
bool mlRMMLCharacter::goTo(mlMedia* apObject, ml3DPosition* apTurnToPos, mlMedia* apTurnToObject, unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->goTo(apObject, apTurnToPos, apTurnToObject, shiftTime);
}
bool mlRMMLCharacter::goTo(ml3DPosition &aPos, ml3DPosition* apTurnToPos, mlMedia* apTurnToObject, unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->goTo(aPos, apTurnToPos, apTurnToObject, shiftTime);
}
bool mlRMMLCharacter::turnTo(mlMedia* apObject){
	MOICH_METH_BODY_IMPL->turnTo(apObject);
}
bool mlRMMLCharacter::turnTo(ml3DPosition &aPos){
	MOICH_METH_BODY_IMPL->turnTo(aPos);
}
bool mlRMMLCharacter::turnTo(int aiFlags){
	MOICH_METH_BODY_IMPL->turnTo(aiFlags);
}
bool mlRMMLCharacter::lookAt(mlMedia* apObject, int aiMs, double adKoef){
	MOICH_METH_BODY_IMPL->lookAt(apObject, aiMs, adKoef);
}
bool mlRMMLCharacter::lookAt(ml3DPosition &aPos, int aiMs, double adKoef){
	MOICH_METH_BODY_IMPL->lookAt(aPos, aiMs, adKoef);
}
bool mlRMMLCharacter::lookAtByEyes(mlMedia* apObject, int aiMs, double adKoef){
	MOICH_METH_BODY_IMPL->lookAtByEyes(apObject, aiMs, adKoef);
}
bool mlRMMLCharacter::lookAtByEyes(ml3DPosition &aPos, int aiMs, double adKoef){
	MOICH_METH_BODY_IMPL->lookAtByEyes(aPos, aiMs, adKoef);
}
bool mlRMMLCharacter::lookAtByEyes(const wchar_t* apDest, int aiMs, double adKoef){
	MOICH_METH_BODY_IMPL->lookAtByEyes(apDest, aiMs, adKoef);
}
bool mlRMMLCharacter::addInterlocutor(mlICharacter* apCharacter){
	MOICH_METH_BODY_IMPL->addInterlocutor(apCharacter);
}
bool mlRMMLCharacter::removeInterlocutor(mlICharacter* apCharacter){
	MOICH_METH_BODY_IMPL->removeInterlocutor(apCharacter);
}
bool mlRMMLCharacter::setViseme(mlMedia* apVisemes, unsigned int auiIndex, int aiTime, int aiKeepupTime){
	MOICH_METH_BODY_IMPL->setViseme(apVisemes, auiIndex, aiTime, aiKeepupTime);
}
bool mlRMMLCharacter::setLexeme( unsigned int auiIndex, int aiTime)
{
	MOICH_METH_BODY_IMPL->setLexeme(auiIndex, aiTime);
}

bool mlRMMLCharacter::setSleepingState( bool abSleep)
{
	MOICH_METH_BODY_IMPL->setSleepingState(abSleep);
}

void mlRMMLCharacter::SetTrackingRightHand( bool abTracking)
{
	if(PMO_IS_NULL(mpMO)) return; 
	mpMO->GetICharacter()->SetTrackingRightHand(abTracking);
}

void mlRMMLCharacter::SetTrackingLeftHand( bool abTracking)
{
	if(PMO_IS_NULL(mpMO)) return; 
	mpMO->GetICharacter()->SetTrackingLeftHand(abTracking);
}

bool mlRMMLCharacter::unsetViseme(mlMedia* apVisemes, unsigned int auiIndex, int aiTime){
	MOICH_METH_BODY_IMPL->unsetViseme(apVisemes, auiIndex, aiTime);
}
bool mlRMMLCharacter::doVisemes(mlMedia* apVisemes, int aiTime){
	MOICH_METH_BODY_IMPL->doVisemes(apVisemes, aiTime);
}
bool mlRMMLCharacter::setToRandomFreePlace(ml3DPosition* apStartPos, double adRadius, bool abInRoom){
	MOICH_METH_BODY_IMPL->setToRandomFreePlace(apStartPos, adRadius, abInRoom);
}

void mlRMMLCharacter::toInitialPose(){
	if (!PMO_IS_NULL(mpMO))
	{
		mpMO->GetICharacter()->toInitialPose();
	}
}

void mlRMMLCharacter::fromInitialPose(){
	if (!PMO_IS_NULL(mpMO))
	{
		mpMO->GetICharacter()->fromInitialPose();
	}	
}

bool mlRMMLCharacter::goForward(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->goForward(shiftTime);
}

bool mlRMMLCharacter::goLeft(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->goLeft(shiftTime);
}

bool mlRMMLCharacter::goRight(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->goRight(shiftTime);
}

bool mlRMMLCharacter::goBackLeft(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->goBackLeft(shiftTime);
}

bool mlRMMLCharacter::goBackRight(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->goBackRight(shiftTime);
}

bool mlRMMLCharacter::goBackward(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->goBackward(shiftTime);
}
bool mlRMMLCharacter::strafeLeft(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->strafeLeft(shiftTime);
}
bool mlRMMLCharacter::strafeRight(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->strafeRight(shiftTime);

}
bool mlRMMLCharacter::stopGo(){
	MOICH_METH_BODY_IMPL->stopGo();
}
bool mlRMMLCharacter::turnLeft(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->turnLeft(shiftTime);
}
bool mlRMMLCharacter::turnRight(unsigned int shiftTime){
	MOICH_METH_BODY_IMPL->turnRight(shiftTime);
}
bool mlRMMLCharacter::stopTurn(){
	MOICH_METH_BODY_IMPL->stopTurn();
}

bool mlRMMLCharacter::sayRecordedVoice( const char* alpcUserName, const char* alpcFileName, LPCSTR alpcCommunicationAreaName, unsigned int auPosition, unsigned int auSyncVersion)
{
	MOICH_METH_BODY_IMPL->sayRecordedVoice( alpcUserName, alpcFileName, alpcCommunicationAreaName, auPosition, auSyncVersion);
}
bool mlRMMLCharacter::getFaceVector(float& x, float& y, float& z)
{
	MOICH_METH_BODY_IMPL->getFaceVector(x, y, z);
}

// показать на заданную точку или объект
bool mlRMMLCharacter::pointByHandTo(const ml3DPosition& aPos, mlMedia* apObject){
	MOICH_METH_BODY_IMPL->pointByHandTo(aPos, apObject);
}

bool mlRMMLCharacter::setKinectDeskPoints(const ml3DPosition& aPos, const ml3DPosition& aPos2, const moMedia* moObj){
	MOICH_METH_BODY_IMPL->setKinectDeskPoints(aPos, aPos2, moObj);
}

void mlRMMLCharacter::disableKinectDesk(){
	if (PMO_IS_NULL(mpMO)) return; 
	mpMO->GetICharacter()->disableKinectDesk();	
}

}
