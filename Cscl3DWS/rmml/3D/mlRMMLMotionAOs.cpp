
#include "mlRMML.h"

namespace rmml {

//////////////////////////////////////////////
// mlMotionTrackFrame
//

mlMotionTrackFrame::mlMotionTrackFrame(void)
{
	muIndex = 0;
	mpMotionTrack = NULL;
	_pos = NULL;
	_scale = NULL;
	_rotation = NULL;
}

mlMotionTrackFrame::~mlMotionTrackFrame(void){
}
		
///// JavaScript Variable Table
JSPropertySpec mlMotionTrackFrame::_JSPropertySpec[] = {
	{ "_pos", JSPROP__pos, JSPROP_ENUMERATE, 0, 0},
	{ "_scale", JSPROP__scale, JSPROP_ENUMERATE, 0, 0},
	{ "_rotation", JSPROP__rotation, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlMotionTrackFrame::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(MotionTrackFrame,mlMotionTrackFrame,0)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlMotionTrackFrame)

JSBool mlMotionTrackFrame::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	mlMotionTrackFrame* priv = (mlMotionTrackFrame*)JS_GetPrivate(cx, obj);
	if(JSVAL_IS_INT(id)){
		switch(JSVAL_TO_INT(id)){
		case JSPROP__pos:{
			ml3DPosition pos;
			bool bSet = false;
			if(JSVAL_IS_STRING(*vp)){
				JSString* jssVal=JSVAL_TO_STRING(*vp);
				if(mlRMML3DObject::ParsePos(wr_JS_GetStringChars(jssVal),&pos))
					bSet = true;
			}else if(JSVAL_IS_OBJECT(*vp)){
				JSObject* jso=JSVAL_TO_OBJECT(*vp);
				if(mlPosition3D::IsInstance(cx, jso)){
					mlPosition3D* pSrcPos=(mlPosition3D*)JS_GetPrivate(cx,jso);
					pos = pSrcPos->GetPos();
					bSet = true;
				}
			}
			if(bSet)
				priv->SetPos(pos);
			}break;
		case JSPROP__scale:{
			ml3DScale scl;
			bool bSet = false;
			if(JSVAL_IS_STRING(*vp)){
				JSString* jssVal=JSVAL_TO_STRING(*vp);
				if(mlRMML3DObject::ParseScl(wr_JS_GetStringChars(jssVal),&scl))
					bSet = true;
			}else if(JSVAL_IS_OBJECT(*vp)){
				JSObject* jso=JSVAL_TO_OBJECT(*vp);
				if(mlScale3D::IsInstance(cx, jso)){
					mlScale3D* pSrcScl=(mlScale3D*)JS_GetPrivate(cx,jso);
					scl = pSrcScl->GetScl();
					bSet = true;
				}
			}
			if(bSet)
				priv->SetScl(scl);
			}break;
		case JSPROP__rotation:{
			ml3DRotation rot;
			bool bSet = false;
			if(JSVAL_IS_STRING(*vp)){
				JSString* jssVal=JSVAL_TO_STRING(*vp);
				if(mlRMML3DObject::ParseRot(wr_JS_GetStringChars(jssVal),&rot))
					bSet = true;
			}else if(JSVAL_IS_OBJECT(*vp)){
				JSObject* jso=JSVAL_TO_OBJECT(*vp);
				if(mlRotation3D::IsInstance(cx, jso)){
					mlRotation3D* pSrcRot=(mlRotation3D*)JS_GetPrivate(cx,jso);
					rot = pSrcRot->GetRot();
					bSet = true;
				}
			}
			if(bSet)
				priv->SetRot(rot);
			}break;
		}
	}
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlMotionTrackFrame::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	mlMotionTrackFrame* priv = (mlMotionTrackFrame*)JS_GetPrivate(cx, obj);
	if(JSVAL_IS_INT(id)){
		switch(JSVAL_TO_INT(id)){
		case JSPROP__pos:
			priv->CreatePos();
			*vp = OBJECT_TO_JSVAL(priv->_pos->mjso);
			break;
		case JSPROP__scale:
			priv->CreateScale();
			*vp = OBJECT_TO_JSVAL(priv->_scale->mjso);
			break;
		case JSPROP__rotation:
			priv->CreateRotation();
			*vp = OBJECT_TO_JSVAL(priv->_rotation->mjso);
			break;
		}
	}
	return JS_TRUE;
}

void mlMotionTrackFrame::CreatePos(){
	if(_pos !=NULL) return;
	JSObject* jsoPos = mlPosition3D::newJSObject(mcx);
	SAVE_FROM_GC(mcx, mjso, jsoPos)
	_pos = (mlPosition3D*)JS_GetPrivate(mcx, jsoPos);
	_pos->Set3DPosRef(this);
}
void mlMotionTrackFrame::CreateScale(){
	if(_scale !=NULL) return;
	JSObject* jsoScl = mlScale3D::newJSObject(mcx);
	SAVE_FROM_GC(mcx, mjso, jsoScl)
	_scale = (mlScale3D*)JS_GetPrivate(mcx, jsoScl);
	_scale->Set3DSclRef(this);
}
void mlMotionTrackFrame::CreateRotation(){
	if(_rotation !=NULL) return;
	JSObject* jsoRot = mlRotation3D::newJSObject(mcx);
	SAVE_FROM_GC(mcx, mjso, jsoRot)
	_rotation = (mlRotation3D*)JS_GetPrivate(mcx, jsoRot);
	_rotation->Set3DRotRef(this);
}

__forceinline ml3DPosition mlMotionTrackFrame::GetPos(){
	if(mpMotionTrack->msBoneName.empty())
		return mpMotionTrack->mpMotion->GetFramePos(mpMotionTrack->miBoneNum, muIndex);
	else
		return mpMotionTrack->mpMotion->GetFramePos(mpMotionTrack->msBoneName.c_str(), muIndex);
}
__forceinline void mlMotionTrackFrame::SetPos(ml3DPosition &pos){
	if(mpMotionTrack->msBoneName.empty())
		mpMotionTrack->mpMotion->SetFramePos(mpMotionTrack->miBoneNum,	muIndex, pos);
	else
		mpMotionTrack->mpMotion->SetFramePos(mpMotionTrack->msBoneName.c_str(),	muIndex, pos);
}
__forceinline ml3DScale mlMotionTrackFrame::GetScl(){
	if(mpMotionTrack->msBoneName.empty())
		return mpMotionTrack->mpMotion->GetFrameScale(mpMotionTrack->miBoneNum, muIndex);
	else
		return mpMotionTrack->mpMotion->GetFrameScale(mpMotionTrack->msBoneName.c_str(), muIndex);
}
__forceinline void mlMotionTrackFrame::SetScl(ml3DScale &scl){
	if(mpMotionTrack->msBoneName.empty())
		mpMotionTrack->mpMotion->SetFrameScale(mpMotionTrack->miBoneNum, muIndex, scl);
	else
		mpMotionTrack->mpMotion->SetFrameScale(mpMotionTrack->msBoneName.c_str(), muIndex, scl);
}
__forceinline ml3DRotation mlMotionTrackFrame::GetRot(){
	if(mpMotionTrack->msBoneName.empty())
		return mpMotionTrack->mpMotion->GetFrameRotation(mpMotionTrack->miBoneNum, muIndex);
	else
		return mpMotionTrack->mpMotion->GetFrameRotation(mpMotionTrack->msBoneName.c_str(), muIndex);
}
__forceinline void mlMotionTrackFrame::SetRot(ml3DRotation &rot){
	if(mpMotionTrack->msBoneName.empty())
		mpMotionTrack->mpMotion->SetFrameRotation(mpMotionTrack->miBoneNum, muIndex, rot);
	else
		mpMotionTrack->mpMotion->SetFrameRotation(mpMotionTrack->msBoneName.c_str(), muIndex, rot);
}

//////////////////////////////////////////////
// mlMotionTrackFrames
//

mlMotionTrackFrames::mlMotionTrackFrames(void)
{
	mpMotionTrack = NULL;
}

mlMotionTrackFrames::~mlMotionTrackFrames(void){
}

///// JavaScript Variable Table
JSPropertySpec mlMotionTrackFrames::_JSPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlMotionTrackFrames::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(MotionTrackFrames,mlMotionTrackFrames,0)
MLJSCLASS_IMPL_END(mlMotionTrackFrames)

JSBool mlMotionTrackFrames::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlMotionTrackFrames::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	mlMotionTrackFrames* priv = (mlMotionTrackFrames*)JS_GetPrivate(cx, obj);
	if(JSVAL_IS_INT(id)){
		int iIndex = JSVAL_TO_INT(id);
		if(iIndex < 0){
			JS_ReportError(cx, "Motion track frame index must not be negative");
			return JS_FALSE;
		}
		wchar_t pwcIndex[20];
		swprintf_s(pwcIndex, 20, L"%d", iIndex);
		mlString sPropName = L"_"; sPropName += pwcIndex;
		jsval vProp;
		wr_JS_GetUCProperty(cx, obj, sPropName.c_str(), -1, &vProp);
		if(JSVAL_IS_NULL(vProp) || JSVAL_IS_VOID(vProp)){
			JSObject* jsoTrackFrame = mlMotionTrackFrame::newJSObject(cx);
			mlMotionTrackFrame* pMotionTrackFrame = (mlMotionTrackFrame*)JS_GetPrivate(cx, jsoTrackFrame);
			pMotionTrackFrame->muIndex = (unsigned)iIndex;
			pMotionTrackFrame->mpMotionTrack = priv->mpMotionTrack;
			vProp = OBJECT_TO_JSVAL(jsoTrackFrame);
			wr_JS_SetUCProperty(cx, obj, sPropName.c_str(), -1, &vProp);
		}
		*vp=vProp;
	}
	return JS_TRUE;
}

//////////////////////////////////////////////
// mlMotionTrack
//

mlMotionTrack::mlMotionTrack(void):
	MP_WSTRING_INIT(msBoneName)
{
	miBoneNum = -1;
	mpMotion = NULL;
	frames = NULL;
}

mlMotionTrack::~mlMotionTrack(void){
}

///// JavaScript Variable Table
JSPropertySpec mlMotionTrack::_JSPropertySpec[] = {
    JSPROP_RO(frames)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlMotionTrack::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(MotionTrack,mlMotionTrack,0)
	MLJSCLASS_ADDPROPFUNC
//	MLJSCLASS_ADDPROPENUMOP
MLJSCLASS_IMPL_END(mlMotionTrack)

JSBool mlMotionTrack::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlMotionTrack::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlMotionTrack);
		default:
			switch(iID){
			case JSPROP_frames:
				if(priv->frames == NULL){
					JSObject* jsoFrames = mlMotionTrackFrames::newJSObject(cx);
					priv->frames = (mlMotionTrackFrames*)JS_GetPrivate(cx, jsoFrames);
					SAVE_FROM_GC(cx, obj, jsoFrames)
					priv->frames->mpMotionTrack = priv;
				}
				*vp=OBJECT_TO_JSVAL(priv->frames->mjso);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

//////////////////////////////////////////////
// mlMotionTracks
//

mlMotionTracks::mlMotionTracks(void)
{
	mpMotion=NULL;
}

mlMotionTracks::~mlMotionTracks(void){
}

///// JavaScript Variable Table
JSPropertySpec mlMotionTracks::_JSPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlMotionTracks::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(MotionTracks,mlMotionTracks,0)
MLJSCLASS_IMPL_END(mlMotionTracks)

JSBool mlMotionTracks::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlMotionTracks::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	mlMotionTracks* priv = (mlMotionTracks*)JS_GetPrivate(cx, obj);
	if(JSVAL_IS_STRING(id)){
		JSString* jssBoneName = JSVAL_TO_STRING(id);
		const wchar_t* jscBoneName = wr_JS_GetStringChars(jssBoneName);
		if(*jscBoneName == L'_' || isEqual(jscBoneName, (const wchar_t *) L"constructor"))
			return JS_TRUE;
		mlString sPropName = L"_"; sPropName += jscBoneName;
		jsval vProp;
		wr_JS_GetUCProperty(cx, obj, sPropName.c_str(), -1, &vProp);
		if(JSVAL_IS_NULL(vProp) || JSVAL_IS_VOID(vProp)){
			JSObject* jsoTrack = mlMotionTrack::newJSObject(cx);
			mlMotionTrack* pMotionTrack = (mlMotionTrack*)JS_GetPrivate(cx, jsoTrack);
			pMotionTrack->msBoneName = jscBoneName;
			pMotionTrack->mpMotion = priv->mpMotion;
			vProp = OBJECT_TO_JSVAL(jsoTrack);
			wr_JS_SetUCProperty(cx, obj, sPropName.c_str(), -1, &vProp);
		}
		*vp=vProp;
	}else if(JSVAL_IS_INT(id)){
		int iBoneNum = JSVAL_TO_INT(id);
		mlString sPropName = L"__";
		wchar_t wsBoneNum[20];
		swprintf_s(wsBoneNum, 20, L"%d", iBoneNum);
		sPropName += wsBoneNum;
		jsval vProp;
		wr_JS_GetUCProperty(cx, obj, sPropName.c_str(), -1, &vProp);
		if(JSVAL_IS_NULL(vProp) || JSVAL_IS_VOID(vProp)){
			JSObject* jsoTrack = mlMotionTrack::newJSObject(cx);
			mlMotionTrack* pMotionTrack = (mlMotionTrack*)JS_GetPrivate(cx, jsoTrack);
			pMotionTrack->miBoneNum = iBoneNum;
			pMotionTrack->mpMotion = priv->mpMotion;
			vProp = OBJECT_TO_JSVAL(jsoTrack);
			wr_JS_SetUCProperty(cx, obj, sPropName.c_str(), -1, &vProp);
		}
		*vp=vProp;
	}
	return JS_TRUE;
}

}