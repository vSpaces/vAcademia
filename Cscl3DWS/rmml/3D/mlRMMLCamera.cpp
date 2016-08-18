
#include "mlRMML.h"
#include <float.h>

namespace rmml {

mlRMMLCamera::mlRMMLCamera(void)
{
	mType=mRMMLType=MLMT_CAMERA;
	fov=0;
	ML_INIT_3DPOSITION(dest);
	ML_INIT_3DPOSITION(destOculus);
	ML_INIT_3DPOSITION(up);
	mpJSDest=NULL;
	mpJSDestOculus=NULL;
	mpJSUp=NULL;
	linkTo=NULL;
	mp3DOlinkTo=NULL;
	mbMLElsCreated=false;
	fixedPosition=false;
	maxLinkDistance = 0.0;
}

mlRMMLCamera::~mlRMMLCamera(void){
	if(mpJSDest) MP_DELETE( mpJSDest);
	if(mpJSDestOculus) MP_DELETE( mpJSDestOculus);
	if(mpJSUp) MP_DELETE( mpJSUp);
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLCamera::~mlRMMLCamera");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLCamera::_JSPropertySpec[] = {
	JSPROP_RW(fov)
	JSPROP_RW(dest)
	JSPROP_RW(linkTo)
	JSPROP_RW(maxLinkDistance)
	JSPROP_RW(fixedPosition)
	JSPROP_RW(up)
	JSPROP_RW(destOculus) //точка, на которую пользователь в окулус смотрит в данный момент
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLCamera::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	JSFUNC(destMoveTo, 2) // переместить объект в определенную позицию за интервал времени в ms
	JSFUNC(setNearPlane, 1)
	JSFUNC(getZLevel, 1)
	JSFUNC(rotate, 2)
	JSFUNC(move, 2)
	JSFUNC(clearOffset, 0)
	JSFUNC(hasOffset, 0)
	JSFUNC(correctPosition, 0)
	JSFUNC(playTrackFile, 0) // воспроизводит движение камеры по треку из файла
	JSFUNC(resetDestOculus, 0) //сбрасывает дополнительный поворот, выставленный изменением destOculus
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Camera,mlRMMLCamera,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMML3DObject)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLCamera)

///// JavaScript Set Property Wrapper
JSBool mlRMMLCamera::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLCamera);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMML3DObject);
		default:
			switch(iID){
			case JSPROP_fov:
				if(JSVAL_IS_INT(*vp)){
					priv->fov=JSVAL_TO_INT(*vp);
					priv->FOVChanged();
				}
				break;
			case JSPROP_fixedPosition:
				if(JSVAL_IS_BOOLEAN(*vp)){
					priv->fixedPosition=JSVAL_TO_BOOLEAN(*vp);
					priv->FixedPositionChanged();
				}
				break;
			case JSPROP_dest:
				priv->CreateJSDest();
				if(JSVAL_IS_STRING(*vp)){
					JSString* jssVal=JSVAL_TO_STRING(*vp);
					ParsePos(wr_JS_GetStringChars(jssVal),&(priv->dest));
					priv->DestChanged();
				}else if(JSVAL_IS_OBJECT(*vp)){
					JSObject* jso=JSVAL_TO_OBJECT(*vp);
					if(mlPosition3D::IsInstance(cx, jso)){
						mlPosition3D* pSrcPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
						priv->dest=pSrcPos->GetPos();
						priv->DestChanged();
					}else{
						JS_ReportError(cx, "Parameter is not 3D-position-object");
						return JS_FALSE;
					}
				}
				break;
			case JSPROP_linkTo:
				if(*vp==JSVAL_NULL || *vp==JSVAL_VOID){
					priv->linkTo=NULL;
					priv->mp3DOlinkTo=NULL;
					priv->LinkToChanged();
				}else if(JSVAL_IS_OBJECT(*vp)){
					JSObject* jso=JSVAL_TO_OBJECT(*vp);
					mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,jso);
					if(pMLEl!=NULL && pMLEl->Get3DObject()!=NULL){
						priv->linkTo=NULL;
						priv->mp3DOlinkTo=pMLEl;
						priv->LinkToChanged();
					}else{
						JS_ReportError(cx, "Parameter is not 3D-object");
						return JS_FALSE;
					}
				}else if(JSVAL_IS_STRING(*vp)){
					priv->SetLinkTo(JSVAL_TO_STRING(*vp));
					priv->LinkToChanged();
				}
				break;
			case JSPROP_maxLinkDistance:{
				double maxLinkDistance = FLT_MAX;
				ML_JSVAL_TO_DOUBLE(maxLinkDistance,*vp);
				if(maxLinkDistance != FLT_MAX){
					if(maxLinkDistance >= 0.0){
						priv->maxLinkDistance = maxLinkDistance;
						priv->MaxLinkDistanceChanged();
					}else{
						JS_ReportError(cx,"maxLinkDistance value must be positive");
					}
				}else{
					JS_ReportError(cx,"maxLinkDistance value must be a double");
				}
				}break;
			case JSPROP_up:
				{
					priv->CreateJSUp();
					if(JSVAL_IS_STRING(*vp)){
						JSString* jssVal=JSVAL_TO_STRING(*vp);
						ParsePos(wr_JS_GetStringChars(jssVal),&(priv->up));
						priv->UpChanged();
					}else if(JSVAL_IS_OBJECT(*vp)){
						JSObject* jso=JSVAL_TO_OBJECT(*vp);
						if(mlPosition3D::IsInstance(cx, jso)){
							mlPosition3D* pSrcPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
							priv->up=pSrcPos->GetPos();
							priv->UpChanged();
						}else{
							JS_ReportError(cx, "Parameter is not 3D-position-object");
							return JS_FALSE;
						}
					}
					break;
				}
				break;
			case JSPROP_destOculus:
				priv->CreateJSDestOculus();
				if(JSVAL_IS_STRING(*vp)){
					JSString* jssVal=JSVAL_TO_STRING(*vp);
					ParsePos(wr_JS_GetStringChars(jssVal),&(priv->destOculus));
					priv->DestOculusChanged();
				}else if(JSVAL_IS_OBJECT(*vp)){
					JSObject* jso=JSVAL_TO_OBJECT(*vp);
					if(mlPosition3D::IsInstance(cx, jso)){
						mlPosition3D* pSrcPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
						priv->destOculus=pSrcPos->GetPos();
						priv->DestOculusChanged();
					}else{
						JS_ReportError(cx, "Parameter is not 3D-position-object");
						return JS_FALSE;
					}
				}
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLCamera::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLCamera);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMML3DObject);
		default:
			switch(iID){
			case EVID_onTrackFilePlayed:
			{
				int iRes = GPSM(cx)->CheckEvent(iID, priv, vp);
				if(iRes >= 0)
					*vp = BOOLEAN_TO_JSVAL(iRes);
			}break;
			case JSPROP_fov:
				*vp=INT_TO_JSVAL(priv->fov);
				break;
			case JSPROP_dest:
				priv->CreateJSDest();
				*vp=OBJECT_TO_JSVAL(priv->mpJSDest->mjsoDest);
				break;
			case JSPROP_linkTo:
				if(priv->mp3DOlinkTo != NULL){
					*vp = OBJECT_TO_JSVAL(priv->mp3DOlinkTo->mjso);
				}else if(priv->linkTo != NULL){
					*vp = STRING_TO_JSVAL(priv->linkTo);
				}else{
					*vp = JSVAL_NULL;
				}
				break;
			case JSPROP_maxLinkDistance:{
				jsdouble* jsdbl = JS_NewDouble(cx, priv->maxLinkDistance);
				*vp = DOUBLE_TO_JSVAL(jsdbl);
				}break;
			case JSPROP_fixedPosition:
				*vp = BOOLEAN_TO_JSVAL(priv->fixedPosition);
				break;
			case JSPROP_up:
				priv->CreateJSUp();
				*vp=OBJECT_TO_JSVAL(priv->mpJSUp->mjsoUp);
				break;
			case JSPROP_destOculus:
				priv->CreateJSDestOculus();
				*vp=OBJECT_TO_JSVAL(priv->mpJSDestOculus->mjsoDestOculus);
				break;	
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLCamera::CreateMedia(omsContext* amcx){
	mbMLElsCreated=true;
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLCamera::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLCamera::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. 
		return ML_ERROR_FAILURE;
	}else{
		SetPSR2MO();
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLCamera::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLCamera* pNewEL=(mlRMMLCamera*)GET_RMMLEL(mcx, mlRMMLCamera::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// 

mlRMMLCamera::mlJS3DDest::mlJS3DDest(mlRMMLCamera* apCamera){
//	ML_ASSERTION(mcx, apCamera!=NULL,"mlRMMLCamera::mlJS3DDest");
	mpCamera=apCamera;
	JSContext* cx=mpCamera->mcx;
	JSObject* obj=mpCamera->mjso;
	mjsoDest=mlPosition3D::newJSObject(cx);
	SAVE_FROM_GC(cx,obj,mjsoDest);
	mlPosition3D* pPos3D=(mlPosition3D*)JS_GetPrivate(cx,mjsoDest);
	pPos3D->Set3DPosRef(this);
}

mlRMMLCamera::mlJS3DDestOculus::mlJS3DDestOculus(mlRMMLCamera* apCamera){
	mpCamera=apCamera;
	JSContext* cx=mpCamera->mcx;
	JSObject* obj=mpCamera->mjso;
	mjsoDestOculus=mlPosition3D::newJSObject(cx);
	SAVE_FROM_GC(cx,obj,mjsoDestOculus);
	mlPosition3D* pPos3D=(mlPosition3D*)JS_GetPrivate(cx,mjsoDestOculus);
	pPos3D->Set3DPosRef(this);
}

mlRMMLCamera::mlJS3DUp::mlJS3DUp(mlRMMLCamera* apCamera){
	mpCamera=apCamera;
	JSContext* cx=mpCamera->mcx;
	JSObject* obj=mpCamera->mjso;
	mjsoUp=mlPosition3D::newJSObject(cx);
	SAVE_FROM_GC(cx,obj,mjsoUp);
	mlPosition3D* pPos3D=(mlPosition3D*)JS_GetPrivate(cx,mjsoUp);
	pPos3D->Set3DPosRef(this);
}

void mlRMMLCamera::CreateJSDest(){
	if(!mpJSDest)
	MP_NEW_V( mpJSDest, mlJS3DDest, this);

	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;
	moICamera* pmoCamera = pMO->GetICamera();

	if(pmoCamera==NULL){ ML_INIT_3DPOSITION(dest);}
	else 	mpJSDest->SetPos(pmoCamera->GetDest());
}

void mlRMMLCamera::CreateJSDestOculus(){
	if(!mpJSDestOculus)
	MP_NEW_V( mpJSDestOculus, mlJS3DDestOculus, this);

	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;
	moICamera* pmoCamera = pMO->GetICamera();

	if(pmoCamera==NULL){ML_INIT_3DPOSITION(destOculus);}
	else    mpJSDestOculus->SetPos(pmoCamera->GetDestOculus());
}

void mlRMMLCamera::CreateJSUp(){
	if(!mpJSUp)
	MP_NEW_V( mpJSUp, mlJS3DUp, this);

	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;
	moICamera* pmoCamera = pMO->GetICamera();

	if(pmoCamera==NULL){ML_INIT_3DPOSITION(up);}
	else    mpJSUp->SetPos(pmoCamera->GetUp());
}

// moICamera
#define MOICAMEVENT_IMPL(M) \
void mlRMMLCamera::M(){ \
	if(PMO_IS_NULL(mpMO)) return; \
	mpMO->GetICamera()->M(); \
}

MOICAMEVENT_IMPL(FOVChanged); 
MOICAMEVENT_IMPL(DestChanged);
MOICAMEVENT_IMPL(DestOculusChanged);
MOICAMEVENT_IMPL(UpChanged);
MOICAMEVENT_IMPL(LinkToChanged);
MOICAMEVENT_IMPL(MaxLinkDistanceChanged);
MOICAMEVENT_IMPL(FixedPositionChanged);
MOICAMEVENT_IMPL(ResetDestOculus);

//
void mlRMMLCamera::SetLinkTo(JSString* ajssName){
	linkTo=ajssName;
	mp3DOlinkTo=NULL;
	if(!mbMLElsCreated) return;
	mp3DOlinkTo=NULL;
	if(ajssName==NULL) return;
	wchar_t* pwcName=wr_JS_GetStringChars(linkTo);
	if(mpParent==NULL) return;
	mlRMMLElement* pMLEl=mpParent->FindElemByName(pwcName);
	if(pMLEl->GetI3DObject()==NULL) return;
	mp3DOlinkTo=pMLEl;
}

mlMedia* mlRMMLCamera::GetLinkTo(){ 
	if(mp3DOlinkTo==NULL)
		SetLinkTo(linkTo); 
	return (mlMedia*)mp3DOlinkTo; 
}

JSBool mlRMMLCamera::JSFUNC_destMoveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc <= 0){
		JS_ReportError(cx,"Method moveTo must get one argument at least");
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}

	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
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
	int iID = pCamera->destMoveTo(pDestPos->GetPos(), iDuration, bCheckCollisions);
	*rval = INT_TO_JSVAL(iID);
	return JS_TRUE;
}

int  mlRMMLCamera::destMoveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions)
{
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return JS_FALSE;
	moICamera* pmoCamera = pMO->GetICamera();
	if(!pmoCamera) return -1;
	pmoCamera->destMoveTo(aPos3D, aiDuaration, abCheckCollisions);
	return 0;
}

JSBool mlRMMLCamera::JSFUNC_setNearPlane(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if ((argc <= 0) || (argc > 1))
	{
		JS_ReportError(cx,"Method setNearPlane must get one argument");
		return JS_FALSE;
	}
	double nearPlane = 0;
	jsval v = argv[0];
	if(!JSVAL_IS_NUMBER(v)){
		JS_ReportError(cx, "Argument of setNearPlane must be number");
		return JS_FALSE;
	}
	JS_ValueToNumber(cx, v, &(nearPlane));

	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	pCamera->SetNearPlane((float)nearPlane);
	
	return JS_TRUE;
}

JSBool mlRMMLCamera::JSFUNC_getZLevel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc != 0)
	{
		JS_ReportError(cx,"Method setNearPlane must get 0 arguments");
		return JS_FALSE;
	}

	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	double zLevel = pCamera->GetZLevel();

	jsdouble* jsdbl=JS_NewDouble(cx, zLevel); 
	*rval = DOUBLE_TO_JSVAL(jsdbl);
	
	return JS_TRUE;
}

JSBool mlRMMLCamera::JSFUNC_rotate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc == 0)
	{
		JS_ReportError(cx,"Method rotate must get 2 arguments");
		return JS_FALSE;
	}

	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	int iDuration = 0, iTime = 0;

	jsval vDur = argv[0];

	if(!JSVAL_IS_INT(vDur)){
		JS_ReportError(cx,"1st parameter of rotate(..) must be an integer");
		return JS_TRUE;
	}

	iDuration = JSVAL_TO_INT(vDur);

	jsval vTime = argv[1];

	if(!JSVAL_IS_INT(vTime)){
		JS_ReportError(cx,"2nd parameter of rotate(..) must be an integer");
		return JS_TRUE;
	}

	iTime = JSVAL_TO_INT(vTime);

	pCamera->Rotate(iDuration, iTime);

	return JS_TRUE;
}

JSBool mlRMMLCamera::JSFUNC_move(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc != 2)
	{
		JS_ReportError(cx,"Method move must get 2 arguments");
		return JS_FALSE;
	}

	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	int iDuration = 0, iTime = 0;

	jsval vDur = argv[0];

	if(!JSVAL_IS_INT(vDur)){
		JS_ReportError(cx,"1st parameter of move(..) must be an integer");
		return JS_TRUE;
	}

	iDuration = JSVAL_TO_INT(vDur);

	jsval vTime = argv[1];

	if(!JSVAL_IS_INT(vTime)){
		JS_ReportError(cx,"2nd parameter of move(..) must be an integer");
		return JS_TRUE;
	}

	iTime = JSVAL_TO_INT(vTime);

	pCamera->Move(iDuration, iTime);

	return JS_TRUE;
}

JSBool mlRMMLCamera::JSFUNC_clearOffset(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	pCamera->ClearOffset();

	return JS_TRUE;
}

JSBool mlRMMLCamera::JSFUNC_correctPosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	pCamera->CorrectPosition();

	return JS_TRUE;
}

JSBool mlRMMLCamera::JSFUNC_playTrackFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);
	if ( !pSM->GetContext() || !pSM->GetContext()->mpRM)
	{
		JS_ReportError(cx, "RenderManager is NULL in playCameraTrackFile");
		return JS_TRUE;
	}

	// должен быть один строковый аргумент
	if(argc != 1){
		JS_ReportError(cx, "playCameraTrackFile method must get one and only one string argument");
		return JS_TRUE;
	}

	const wchar_t* pwcFilePath = NULL;
	if(JSVAL_IS_STRING(argv[0]))
	{
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		pwcFilePath = wr_JS_GetStringChars(jss);
	}
	else
	{
		JS_ReportError(cx, "playCameraTrackFile argument must be a string");
		return JS_TRUE;
	}

	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	pCamera->PlayTrackFile( pwcFilePath);

	return JS_TRUE;
}

JSBool mlRMMLCamera::JSFUNC_hasOffset(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(pCamera->HasOffset());
	
	return JS_TRUE;
}

JSBool mlRMMLCamera::JSFUNC_resetDestOculus( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{

	mlRMMLCamera* pCamera = (mlRMMLCamera*)JS_GetPrivate(cx, obj);
	pCamera->ResetDestOculus();
	return JS_TRUE;
}

void mlRMMLCamera::Rotate(int aiDuration, int aiTime)
{
	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;

	moICamera* pmoCamera = pMO->GetICamera();
	if(!pmoCamera) return;

	pmoCamera->Rotate(aiDuration, aiTime);
}

void mlRMMLCamera::Move(int aiDuration, int aiTime)
{
#define MUL_KOEF		0.25f

	moMedia* pMO=GetMO();
	if(PMO_IS_NULL(pMO)) return;

	moICamera* pmoCamera = pMO->GetICamera();
	if(!pmoCamera) return;

	pmoCamera->Move(aiDuration, (int)(aiTime * MUL_KOEF));
}

void mlRMMLCamera::ClearOffset()
{
	moMedia* pMO = GetMO();
	if (PMO_IS_NULL(pMO)) return;

	moICamera* pmoCamera = pMO->GetICamera();
	if (!pmoCamera) return;

	pmoCamera->ClearOffset();
}

void mlRMMLCamera::CorrectPosition()
{
	moMedia* pMO = GetMO();
	if (PMO_IS_NULL(pMO)) return;

	moICamera* pmoCamera = pMO->GetICamera();
	if (!pmoCamera) return;

	pmoCamera->CorrectPosition();
}

void mlRMMLCamera::SetNearPlane(float nearPlane)
{
	moMedia* pMO = GetMO();
	if (PMO_IS_NULL(pMO)) return;
	moICamera* pmoCamera = pMO->GetICamera();
	if (!pmoCamera) return;
	pmoCamera->SetNearPlane(nearPlane);
}

float mlRMMLCamera::GetZLevel()
{
	moMedia* pMO = GetMO();
	if (PMO_IS_NULL(pMO)) 
	{
		return 0;
	}

	moICamera* pmoCamera = pMO->GetICamera();
	if (!pmoCamera) 
	{
		return 0;
	}

	return pmoCamera->GetZLevel();
}

bool mlRMMLCamera::HasOffset()
{
	moMedia* pMO = GetMO();
	if (PMO_IS_NULL(pMO)) 
	{
		return 0;
	}

	moICamera* pmoCamera = pMO->GetICamera();
	if (!pmoCamera) 
	{
		return 0;
	}

	return pmoCamera->HasOffset();
}

void mlRMMLCamera::PlayTrackFile(const wchar_t* apwcFilePath)
{
	moMedia* pMO = GetMO();
	if( pMO)
	{
		moICamera* pmoCamera = pMO->GetICamera();
		if( pmoCamera)
		{
			pmoCamera->PlayTrackFile( apwcFilePath);
		}
	}
}

void mlRMMLCamera::onTrackFilePlayed(){
//	GPSM(mcx)->mbDontSendSynchEvents = true;
//	CallListeners(EVID_onTrackFilePlayed);
}


}
