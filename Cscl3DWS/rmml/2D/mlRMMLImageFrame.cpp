#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlSPParser.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLImageFrame::mlRMMLImageFrame(void)
{
	mType=MLMT_IMAGE_FRAME;
	mRMMLType=MLMT_IMAGE_FRAME;

	margin = ML_UNDEF_MARGIN_VALUE;
	marginTop = ML_UNDEF_MARGIN_VALUE;
	marginBottom = ML_UNDEF_MARGIN_VALUE;
	marginLeft = ML_UNDEF_MARGIN_VALUE;
	marginRight = ML_UNDEF_MARGIN_VALUE;
	tile = false;
}

mlRMMLImageFrame::~mlRMMLImageFrame(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLImageFrame::~mlRMMLImageFrame");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLImageFrame::_JSPropertySpec[] = {
	JSPROP_RW(margin) // сколько пикселов от края - рамка
	JSPROP_RW(marginTop)
	JSPROP_RW(marginBottom)
	JSPROP_RW(marginLeft)
	JSPROP_RW(marginRight)
	JSPROP_RW(tile)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLImageFrame::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(ImageFrame,mlRMMLImageFrame,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLImageFrame)

///// JavaScript Set Property Wrapper
JSBool mlRMMLImageFrame::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLImageFrame);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID)	{
			case JSPROP_margin:{
				jsdouble dVal = 0.0;
				if(JS_ValueToNumber(cx, *vp, &dVal)){
					priv->margin = (unsigned int)dVal;
				}
				priv->MarginsChanged();
				}break;
			case JSPROP_marginTop:{
				jsdouble dVal = 0.0;
				if(JS_ValueToNumber(cx, *vp, &dVal)){
					priv->marginTop = (unsigned int)dVal;
				}
				priv->MarginsChanged();
				}break;
			case JSPROP_marginBottom:{
				jsdouble dVal = 0.0;
				if(JS_ValueToNumber(cx, *vp, &dVal)){
					priv->marginBottom = (unsigned int)dVal;
				}
				priv->MarginsChanged();
				}break;
			case JSPROP_marginLeft:{
				jsdouble dVal = 0.0;
				if(JS_ValueToNumber(cx, *vp, &dVal)){
					priv->marginLeft = (unsigned int)dVal;
				}
				priv->MarginsChanged();
				}break;
			case JSPROP_marginRight:{
				jsdouble dVal = 0.0;
				if(JS_ValueToNumber(cx, *vp, &dVal)){
					priv->marginRight = (unsigned int)dVal;
				}
				priv->MarginsChanged();
				}break;
			case JSPROP_tile:
				ML_JSVAL_TO_BOOL(priv->tile, *vp);
				priv->TileChanged();
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLImageFrame::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLImageFrame);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID) {
			case JSPROP_margin:
				*vp = INT_TO_JSVAL(priv->margin);
				break;
			case JSPROP_marginTop:
				*vp = INT_TO_JSVAL(priv->marginTop);
				break;
			case JSPROP_marginBottom:
				*vp = INT_TO_JSVAL(priv->marginBottom);
				break;
			case JSPROP_marginLeft:
				*vp = INT_TO_JSVAL(priv->marginLeft);
				break;
			case JSPROP_marginRight:
				*vp = INT_TO_JSVAL(priv->marginRight);
				break;
			case JSPROP_tile:
				*vp = BOOLEAN_TO_JSVAL(priv->tile);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

mlresult mlRMMLImageFrame::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLImageFrame::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLImageFrame::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	int iOldType=mType;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. Restore
		mType=iOldType;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLImageFrame::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLImageFrame* pNewEL=(mlRMMLImageFrame*)GET_RMMLEL(mcx, mlRMMLImageFrame::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// реализация mlIImageFrame
int mlRMMLImageFrame::GetMargins(int& aiMarginTop, int& aiMarginBottom, int& aiMarginLeft, int& aiMarginRight){
	aiMarginTop = aiMarginBottom = aiMarginLeft = aiMarginRight = margin;
	if(marginTop != ML_UNDEF_MARGIN_VALUE)
		aiMarginTop = marginTop;
	if(marginBottom != ML_UNDEF_MARGIN_VALUE)
		aiMarginBottom = marginBottom;
	if(marginLeft != ML_UNDEF_MARGIN_VALUE)
		aiMarginLeft = marginLeft;
	if(marginRight != ML_UNDEF_MARGIN_VALUE)
		aiMarginRight = marginRight;
	return margin;
}

// реализация moIImageFrame
void mlRMMLImageFrame::MarginsChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImageFrame()->MarginsChanged();
}

void mlRMMLImageFrame::TileChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImageFrame()->TileChanged();
}

} // namespace rmml