// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $Id: mlRMMLLine.cpp,v 1.42 2009/10/09 13:15:51 nikitin Exp $ 
// $Locker:  $
//
// last change: $Date: 2009/10/09 13:15:51 $ $Author: nikitin $ 
//
//////////////////////////////////////////////////////////////////////
#include "mlRMML.h"
#include "config/oms_config.h"

#include "mlSPParser.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLLine::mlRMMLLine(void)
{
	mType=MLMT_LINE;
	mRMMLType=MLMT_LINE;
	
	ML_INIT_POINT(mXY2)

	smooth = true;
	thickness = 1;

	ML_INIT_COLOR(color);
	color.r = color.g = color.b = 0xFF;
	mjsoColor = NULL;
}

mlRMMLLine::~mlRMMLLine(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLLine::~mlRMMLLine");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

/*
JSClass mlRMMLLine::_jsClass={
	"Image", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub,
	mlRMMLLine::JSGetProperty, mlRMMLLine::JSSetProperty,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub, mlRMMLLine::JSDestructor,
	0, 0, 0, 0, 
	0, 0, 0, 0
};
*/

///// JavaScript Variable Table
JSPropertySpec mlRMMLLine::_JSPropertySpec[] = {
	JSPROP_RW(x2)
	JSPROP_RW(y2)
	JSPROP_RW(color)
	JSPROP_RW(thickness)
	JSPROP_RW(smooth)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLLine::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Line,mlRMMLLine,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLLine)

/*
mlresult mlRMMLLine::InitJSClass(JSContext* cx , JSObject* obj){

	JSPropertySpec* pJSPropertySpec=NULL;
	JSFunctionSpec* pJSFunctionSpec=NULL;
	mlJSClass::CreatePropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
	mlRMMLElement::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
	mlRMMLVisible::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
//	mlRMMLPButton::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);

	JSObject* pImageProto=JS_InitClass(cx, obj, NULL, &_jsClass,
		mlRMMLLine::JSConstructor, 0,
		pJSPropertySpec, pJSFunctionSpec,
		NULL, NULL);
	mlJSClass::DestroyPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
	return ML_OK;
}
*/

///// JavaScript Set Property Wrapper
JSBool mlRMMLLine::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLLine);
		SET_PROTO_PROP(mlRMMLElement);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID)	{
			case JSPROP_x2:
				ML_JSVAL_TO_INT(priv->mXY2.x,*vp);
				priv->GetVisible()->InvalidBounds();
				priv->GetVisible()->SizeChanged();
				break;
			case JSPROP_y2:
				ML_JSVAL_TO_INT(priv->mXY2.y,*vp);
				priv->GetVisible()->InvalidBounds();
				priv->GetVisible()->SizeChanged();
				break;
			case JSPROP_thickness:
				ML_JSVAL_TO_INT(priv->thickness,*vp);
				priv->ThicknessChanged();
				priv->GetVisible()->InvalidBounds();
				break;
			case JSPROP_color:
				if(SetColorByJSVal(cx, priv->color, *vp)){
					priv->ColorChanged();
				}else{
					mlTrace(cx, "color property value is of unexpected type");
				}
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

void mlRMMLLine::CreateColorJSO(JSObject* &jso, mlColor* apColor){
	jso = mlColorJSO::newJSObject(mcx);
	mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(mcx,jso);
	pColor->SetColorRef(apColor, this);
	SAVE_FROM_GC(mcx,mjso,jso)
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLLine::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLLine);
		GET_PROTO_PROP(mlRMMLElement);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID) {
			case JSPROP_x2:
				*vp = INT_TO_JSVAL(priv->mXY2.x);
				break;
			case JSPROP_y2:
				*vp = INT_TO_JSVAL(priv->mXY2.y);
				break;
			case JSPROP_thickness:
				*vp = INT_TO_JSVAL(priv->thickness);
				break;
			case JSPROP_color:
				if(priv->mjsoColor == NULL)
					priv->CreateColorJSO(priv->mjsoColor, &priv->color);
				*vp=OBJECT_TO_JSVAL(priv->mjsoColor);
				break;
				
			}
	GET_PROPS_END;
	return JS_TRUE;
}

// реализация mlRMMLElement
mlresult mlRMMLLine::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLLine::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLLine::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	return ML_OK;
}

void mlRMMLLine::DuplicateMedia(mlRMMLElement* apSource)
{
	DuplicateFrom(apSource);
}

mlRMMLElement* mlRMMLLine::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLLine* pNewEL=(mlRMMLLine*)GET_RMMLEL(mcx, mlRMMLLine::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	pNewEL->DuplicateFrom(apNewParent);
	return pNewEL;
}

mlRect mlRMMLLine::GetBounds(){
	mlRect rc; 
	if(mXY.x < mXY2.x){
		rc.left = mXY.x; 
		rc.right = mXY2.x; 
	}else{
		rc.left = mXY2.x; 
		rc.right = mXY.x; 
	}
	if(mXY.y < mXY2.y){
		rc.top = mXY.y;
		rc.bottom = mXY2.y;
	}else{
		rc.top = mXY2.y;
		rc.bottom = mXY.y;
	}
	mSize.width = rc.right - rc.left;
	mSize.height = rc.bottom - rc.top;
	return rc;
}

mlPoint mlRMMLLine::GetAbsXY2(){
	mlPoint pntAbsXY = GetAbsXY();
	pntAbsXY.x -= mXY.x;
	pntAbsXY.y -= mXY.y;
	pntAbsXY.x += mXY2.x;
	pntAbsXY.y += mXY2.y;
	return pntAbsXY;
}

// реализация moILine
#define MOILINEEVENT_IMPL(M) \
	void mlRMMLLine::M(){ \
		if(PMO_IS_NULL(mpMO)) return; \
		mpMO->GetILine()->M(); \
	}

MOILINEEVENT_IMPL(ColorChanged)
MOILINEEVENT_IMPL(ThicknessChanged)
MOILINEEVENT_IMPL(SmoothChanged)

}