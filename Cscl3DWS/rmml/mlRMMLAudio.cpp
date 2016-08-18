#include "mlRMML.h"
#include "config/oms_config.h"

#include "config/prolog.h"

namespace rmml {

mlRMMLAudio::mlRMMLAudio(void):
	MP_WSTRING_INIT(msSplineSrcFull)
{
	mRMMLType=MLMT_AUDIO;
	mType=MLMT_AUDIO;
	m_volume = 100;
	m_locationID = NULL;
	m_emitterType = NULL;
	m_splineSrc = NULL;
}

mlRMMLAudio::~mlRMMLAudio(void){
	omsContext* pContext=GPSM(mcx)->GetContext();
#ifdef CSCL
	ML_ASSERTION(mcx, pContext->mpRM != NULL, "mlRMMLAudio::~mlRMMLAudio");
	if (pContext->mpSndM && !PMO_IS_NULL(mpMO))
		pContext->mpSndM->DestroyMO(this);
#else
	ML_ASSERTION(mcx, pContext->mpRM != NULL, "mlRMMLAudio::~mlRMMLAudio");
	if (pContext->mpRM && !PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
#endif
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLAudio::_JSPropertySpec[] = {
	JSPROP_RW(emitterType)
	JSPROP_RW(splineSrc)
	JSPROP_RW(volume)
	JSPROP_RW(location)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLAudio::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Audio,mlRMMLAudio,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLContinuous)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLAudio)


///// JavaScript Set Property Wrapper
JSBool mlRMMLAudio::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLAudio);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMMLContinuous);
		default:
			switch(iID)	
			{
			case JSPROP_emitterType:
				{
					if ( JSVAL_IS_STRING( *vp))
					{
						priv->m_emitterType = JSVAL_TO_STRING(*vp);
						priv->EmitterTypeChanged();
					}
				}break;
			case JSPROP_splineSrc:
				{					
					if ( JSVAL_IS_STRING( *vp))
					{
						priv->m_splineSrc = JSVAL_TO_STRING(*vp);
						priv->SplineSrcChanged();
					}					
				}break;			
			case JSPROP_volume:
				{					
					if ( JSVAL_IS_INT( *vp))
					{
						priv->m_volume = JSVAL_TO_INT(*vp);
						priv->VolumeChanged();
					}					
				}break;
			case JSPROP_location:
				{					
					if ( JSVAL_IS_STRING( *vp))
					{
						priv->m_locationID = JSVAL_TO_STRING(*vp);
						priv->LocationChanged();
					}					
				}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLAudio::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLAudio);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMMLContinuous);
		default:
			switch(iID)	
			{
			case JSPROP_emitterType:
				{
					if ( priv->m_emitterType == NULL)
					{
						*vp = JS_GetEmptyStringValue(cx);
					}else
					{						
						*vp = STRING_TO_JSVAL(priv->m_emitterType);
					}
				}break;				
			case JSPROP_splineSrc:
				{
					if ( priv->m_splineSrc == NULL)
					{
						*vp = JS_GetEmptyStringValue(cx);
					}else
					{						
						*vp = STRING_TO_JSVAL(priv->m_splineSrc);
					}
				}break;
			case JSPROP_volume:
				{
					if ( priv->m_volume == NULL)
					{
						*vp = JS_GetEmptyStringValue(cx);
					}else
					{						
						*vp = INT_TO_JSVAL(priv->m_volume);
					}
				}break;
			case JSPROP_location:
				{
					if ( priv->m_locationID== NULL)
					{
						*vp = JS_GetEmptyStringValue(cx);
					}else
					{						
						*vp = STRING_TO_JSVAL(priv->m_locationID);
					}
				}break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLAudio::CreateMedia(omsContext* amcx){
#ifdef CSCL
	if ( amcx->mpSndM == NULL)
	{
		amcx->mpRM->CreateMO(this);
		//return ML_OK;//ML_ERROR_NOT_INITIALIZED;
	}
	else
	{
		ML_ASSERTION(mcx, amcx->mpSndM!=NULL,"mlRMMLAudio::CreateMedia");
		amcx->mpSndM->CreateMO(this);
	}
	
#else
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLAudio::CreateMedia");
	amcx->mpRM->CreateMO(this);
#endif
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	mlRMMLContinuous::PosChangedDontStop(); // currentPos attribute added 21.02.06 (patched 06.05.06)
	return ML_OK;
}

mlresult mlRMMLAudio::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded.
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLAudio::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLAudio* pNewEL=(mlRMMLAudio*)GET_RMMLEL(mcx, mlRMMLAudio::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

bool mlRMMLAudio::Freeze(){
	if(!mlRMMLElement::Freeze()) return false; 
	// ??
	return true;
}

bool mlRMMLAudio::Unfreeze(){
	if(!mlRMMLElement::Unfreeze()) return false; 
	// ??
	return true;
}

unsigned long mlRMMLAudio::GetCurPos()
{ 
	if (PMO_IS_NULL(mpMO))
	{
		return 0;
	}
	currentPos =  mpMO->GetIContinuous()->GetCurPos();
	return currentPos;
}

unsigned long mlRMMLAudio::GetRMCurPos()
{ 	
	return currentPos;
}

#define MOICONT_EVENT_CALL(E) \
	moIContinuous* pmoIContinuous=GetmoIContinuous(); \
	if(pmoIContinuous) pmoIContinuous->##E(); 

void mlRMMLAudio::PosChanged()
{	
	MOICONT_EVENT_CALL(PosChanged);
}

bool mlRMMLAudio::EmitterTypeChanged()
{
	if ( PMO_IS_NULL( mpMO)) return false;
	return mpMO->GetIAudible()->EmitterTypeChanged();
}


bool mlRMMLAudio::SplineSrcChanged()
{
	if ( PMO_IS_NULL( mpMO)) return false;
	return mpMO->GetIAudible()->SplineSrcChanged();
}

bool mlRMMLAudio::VolumeChanged()
{
	if ( PMO_IS_NULL( mpMO)) return false;
	return mpMO->GetIAudible()->VolumeChanged();
}

bool mlRMMLAudio::LocationChanged()
{
	if ( PMO_IS_NULL( mpMO)) return false;
	return mpMO->GetIAudible()->LocationChanged();
}

const wchar_t* mlRMMLAudio::GetEmitterType()
{
	if ( m_emitterType == NULL) 
		return NULL;
	wchar_t* ws_emitterType = wr_JS_GetStringChars(m_emitterType);
	if ( *ws_emitterType == L'\0') 
		return NULL;

	if ( *(ws_emitterType+1) == L'\0' && (*ws_emitterType == L'\\' || *ws_emitterType == L'/')) 
		return NULL;

	return ws_emitterType;
}

const wchar_t* mlRMMLAudio::GetSplineSrc()
{
	if ( m_splineSrc == NULL) 
		return NULL;
	wchar_t* pwcSrc = wr_JS_GetStringChars(m_splineSrc);
	if ( *pwcSrc == L'\0') 
		return NULL;

	if ( *(pwcSrc+1) == L'\0' && (*pwcSrc == L'\\' || *pwcSrc == L'/')) 
		return NULL;

	mlRMMLElement* pMLEl = GetElem_mlRMMLLoadable();
	msSplineSrcFull = GPSM(pMLEl->mcx)->RefineResPathEl(pMLEl, pwcSrc);
	return msSplineSrcFull.c_str();
}

const int mlRMMLAudio::GetVolume()
{
	if ( m_volume < 0) 
		return -1;
	
	return m_volume;	
}

const wchar_t* mlRMMLAudio::GetLocation()
{
	if ( m_locationID == NULL) 
		return NULL;
	wchar_t* ws_location = wr_JS_GetStringChars(m_locationID);
	if ( *ws_location == L'\0') 
		return NULL;

	if ( *(ws_location+1) == L'\0' && (*ws_location == L'\\' || *ws_location == L'/')) 
		return NULL;

	return ws_location;	
}

}