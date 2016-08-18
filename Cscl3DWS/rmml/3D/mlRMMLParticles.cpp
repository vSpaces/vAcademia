
#include "mlRMML.h"

namespace rmml {

mlRMMLParticles::mlRMMLParticles(void)
{
	mType=mRMMLType=MLMT_PARTICLES;
}

mlRMMLParticles::~mlRMMLParticles(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLParticles::~mlRMMLParticles");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLParticles::_JSPropertySpec[] = {
	JSPROP_RW(maxParticles)
	JSPROP_RW(density)
	JSPROP_RW(minSpeed)
	JSPROP_RW(maxSpeed)
	JSPROP_RW(minSize)
	JSPROP_RW(maxSize)
	JSPROP_RW(minOpacity)
	JSPROP_RW(maxOpacity)
	JSPROP_RW(dirChangeFactor)
	JSPROP_RW(speedChangeFactor)
	JSPROP_RW(defPointSize)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLParticles::_JSFunctionSpec[] = {
	JSFUNC(setType, 1)
	JSFUNC(enableSortVerts, 0)
	JSFUNC(disableSortVerts, 0)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Particles,mlRMMLParticles,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMML3DObject)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLParticles)

///// JavaScript Set Property Wrapper
JSBool mlRMMLParticles::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLParticles);
		SET_PROTO_PROP(mlRMMLElement);
		SET_PROTO_PROP(mlRMML3DObject);
		default:
			if (priv->GetSafeMO()) {
				moIParticles* particles = priv->mpMO->GetIParticles();
				switch(iID){
				case JSPROP_maxParticles:
					if(JSVAL_IS_INT(*vp)){
						int val = JSVAL_TO_INT(*vp);
						if (val >= 0)
						{
							particles->SetMaxParticles(val);
						} else 
						{
							JS_ReportError(cx,"maxParticles value must be positive integer");
							return JS_FALSE;
						}
					}
					break;
				case JSPROP_density: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							if (val >= 0.0f && val <= 1.0f) {
								particles->SetDensity(val);
							} else
							{
								JS_ReportError(cx,"density value must be from 0 to 1");
								return JS_FALSE;
							}							
						}
					} break;
				case JSPROP_minSpeed: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							particles->SetMinSpeed(val);
						}
					} break;
				case JSPROP_maxSpeed: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							particles->SetMaxSpeed(val);
						}
					} break;
				case JSPROP_minSize: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							particles->SetMinSize(val);
						}
					} break;
				case JSPROP_maxSize: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							particles->SetMaxSize(val);
						}
					} break;
				case JSPROP_minOpacity: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							if (val >= 0.0f && val <= 1.0f) {
								particles->SetMinOpacity(val);
							} else
							{
								JS_ReportError(cx,"opacity value must be from 0 to 1");
								return JS_FALSE;
							}
						}
					} break;
				case JSPROP_maxOpacity: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							if (val >= 0.0f && val <= 1.0f) {
								particles->SetMaxOpacity(val);
							} else
							{
								JS_ReportError(cx,"opacity value must be from 0 to 1");
								return JS_FALSE;
							}
						}
					} break;
				case JSPROP_dirChangeFactor: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							particles->SetDirChangeFactor(val);
						}
					} break;
				case JSPROP_speedChangeFactor: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							particles->SetSpeedChangeFactor(val);
						}
					} break;
				case JSPROP_defPointSize: {
						double val = FLT_MAX;
						ML_JSVAL_TO_DOUBLE(val,*vp);
						if(val != FLT_MAX){
							particles->SetDefPointSize(val);
						}
					} break;
				}
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLParticles::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLParticles);
		GET_PROTO_PROP(mlRMMLElement);
		GET_PROTO_PROP(mlRMML3DObject);
		default:
			if (priv->GetSafeMO()) {
				moIParticles* particles = priv->mpMO->GetIParticles();
				switch(iID){
				case JSPROP_maxParticles:
					*vp = INT_TO_JSVAL(particles->GetMaxParticles());
					break;
				case JSPROP_density: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetDensity());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_minSpeed: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetMinSpeed());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_maxSpeed: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetMaxSpeed());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_minSize: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetMinSize());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_maxSize: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetMaxSize());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_minOpacity: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetMinOpacity());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_maxOpacity: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetMaxOpacity());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_dirChangeFactor: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetDirChangeFactor());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_speedChangeFactor: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetSpeedChangeFactor());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				case JSPROP_defPointSize: {
					jsdouble* jsdbl = JS_NewDouble(cx, particles->GetDefPointSize());
					*vp = DOUBLE_TO_JSVAL(jsdbl);
					} break;
				}
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLParticles::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLParticles::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLParticles::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. 
		return ML_ERROR_FAILURE;
	}else{
		SetPSR2MO();
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLParticles::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLParticles* pNewEL=(mlRMMLParticles*)GET_RMMLEL(mcx, mlRMMLParticles::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

JSBool mlRMMLParticles::JSFUNC_setType(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc != 1)
	{
		JS_ReportError(cx,"Method setType must get one argument");
		return JS_FALSE;
	}
	jsval v = argv[0];
	if (!JSVAL_IS_STRING(v)) {
		JS_ReportError(cx, "Argument of setType must be string");
		return JS_FALSE;
	}

	mlRMMLParticles* rmml = (mlRMMLParticles*)JS_GetPrivate(cx, obj);
	if (rmml->GetSafeMO()) {
		moIParticles* particles = rmml->mpMO->GetIParticles();
		const WCHAR* wname = wr_JS_GetStringChars(JSVAL_TO_STRING(v));
		size_t len = wcslen(wname);
		char* name = new char[len+1];
		wcstombs (name, wname, len);
		name[len] = '\0';

		bool success = particles->SetType(name);

		if (!success) {
			std::string mess = "Particles type '";
			mess += name;
			mess += "' is undefined";
			JS_ReportError(cx, mess.c_str());
		}
		delete name;
		return (success) ? JS_TRUE: JS_FALSE;
	} else {
		return JS_TRUE;
	}
}

JSBool mlRMMLParticles::JSFUNC_enableSortVerts(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	moIParticles* particles = ((mlRMMLParticles*)JS_GetPrivate(cx, obj))->mpMO->GetIParticles();
	particles->SetSortVerts(true);
	return JS_TRUE;
}

JSBool mlRMMLParticles::JSFUNC_disableSortVerts(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	moIParticles* particles = ((mlRMMLParticles*)JS_GetPrivate(cx, obj))->mpMO->GetIParticles();
	particles->SetSortVerts(false);
	return JS_TRUE;
}

}
