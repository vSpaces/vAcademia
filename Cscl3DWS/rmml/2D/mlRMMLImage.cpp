// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $Id: mlRMMLImage.cpp,v 1.42 2009/10/09 13:15:51 nikitin Exp $ 
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

mlRMMLImage::mlRMMLImage(void)
{
	mType=MLMT_IMAGE;
	mRMMLType=MLMT_IMAGE;
	
	antialiasing = AA_LINEAR;
	mbEditable = false;
	mbEditableNow = false;
	mbRemoteControlNow = false;
	ML_INIT_COLOR(color);
	color.r = color.g = color.b = 0xFF;
	compressed = false;
	angle = 0;
	maxDecreaseKoef = 4;
	mjsoColor = NULL;
	tile = false;
	smooth = false;

	crop = NULL;
	sysmem = false;
}

mlRMMLImage::~mlRMMLImage(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLImage::~mlRMMLImage");
	if(crop != NULL)
		MP_DELETE( crop);
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

/*
JSClass mlRMMLImage::_jsClass={
	"Image", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub,
	mlRMMLImage::JSGetProperty, mlRMMLImage::JSSetProperty,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub, mlRMMLImage::JSDestructor,
	0, 0, 0, 0, 
	0, 0, 0, 0
};
*/

///// JavaScript Variable Table
JSPropertySpec mlRMMLImage::_JSPropertySpec[] = {
//	JSPROP_RW(antialiasing)
	JSPROP_RW(editable)
	JSPROP_RW(editableNow)
	JSPROP_RW(color)
	JSPROP_RW(tile)
	JSPROP_RW(smooth)

	JSPROP_RW(crop)	// декларативное задание crop-а в формате "x, y, w, h" или "x y w h"
	JSPROP_RW(compressed)
	JSPROP_RW(angle)
	JSPROP_RW(maxDecreaseKoef)
	JSPROP_RW(sysmem) // если картинка невидимая, то не распаковывать в видео память, а оставить в сжатом виде в системной памяти
	JSPROP_RW(remoteControlNow)
	JSPROP_RW(editToolUsed)
	JSPROP_RW(firstSharingFrameDrawed)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLImage::_JSFunctionSpec[] = {
	JSFUNC(makeSnapshot,1)
	JSFUNC(drawLine,4)
	JSFUNC(fillRectangle,5)
	JSFUNC(setCanvasSize,2) // setCanvasSize(w,h) - устанавливает размер текстуры (свойства width и height не меняет)
	JSFUNC(drawEllipse,4)
	JSFUNC(drawRectangle,4)
	JSFUNC(getPixel,2)	// color getPixel(x,y) - возвращает цвет пиксела по указанным координатам
	JSFUNC(setActiveTool,1)
	JSFUNC(setLineWidth,1)
	JSFUNC(setLineColor,1)
	JSFUNC(setBgColor,1)
	JSFUNC(setToolOpacity, 1)
	JSFUNC(setTextFontName,1)
	JSFUNC(setTextBold,1)
	JSFUNC(setTextItalic,1)
	JSFUNC(setTextUnderline,1)
	JSFUNC(setTextAntialiasing,1)
	JSFUNC(setToolMode,1)
	JSFUNC(setLineDirection,1)
	JSFUNC(resample,2)
	JSFUNC(undoAction,1)
	JSFUNC(pasteText,1)
	JSFUNC(clearOldScreen,1)
	JSFUNC(copyToClipboard,0)	
	JSFUNC(isBinaryStateReceived,0)
	JSFUNC(save,0)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Image,mlRMMLImage,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLImage)

/*
mlresult mlRMMLImage::InitJSClass(JSContext* cx , JSObject* obj){

	JSPropertySpec* pJSPropertySpec=NULL;
	JSFunctionSpec* pJSFunctionSpec=NULL;
	mlJSClass::CreatePropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
	mlRMMLElement::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
	mlRMMLLoadable::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
	mlRMMLVisible::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
//	mlRMMLPlayable::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
//	mlRMMLPButton::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);

	JSObject* pImageProto=JS_InitClass(cx, obj, NULL, &_jsClass,
		mlRMMLImage::JSConstructor, 0,
		pJSPropertySpec, pJSFunctionSpec,
		NULL, NULL);
	mlJSClass::DestroyPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);
	return ML_OK;
}
*/

mlPropParseInfo mlCropParseInfo[]={
	{L"x",MLPT_INT,NULL},
	{L"y",MLPT_INT,NULL},
	{L"w",MLPT_INT,NULL},
	{L"h",MLPT_INT,NULL},
	{L"",MLPT_UNKNOWN,NULL}
};

///// JavaScript Set Property Wrapper
JSBool mlRMMLImage::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLImage);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID)	{
			case JSPROP_editable:
				ML_JSVAL_TO_BOOL(priv->mbEditable, *vp);
				priv->EditableChanged();
				break;
			case JSPROP_editableNow:
				ML_JSVAL_TO_BOOL(priv->mbEditableNow, *vp);
				priv->EditableNowChanged();
				break;
			case JSPROP_color:
				if(SetColorByJSVal(cx, priv->color, *vp)){
					priv->ColorChanged();
				}else{
					mlTrace(cx, "color property value is of unexpected type");
				}
				break;
			case JSPROP_tile:
				ML_JSVAL_TO_BOOL(priv->tile, *vp);
				priv->TileChanged();
				break;
			case JSPROP_smooth:
				ML_JSVAL_TO_BOOL(priv->smooth, *vp);
				priv->SmoothChanged();
				break;
			case JSPROP_crop:{
				if(JSVAL_IS_STRING(*vp)){
					JSString* jssCrop = JSVAL_TO_STRING(*vp);
					wchar_t* jscCrop = wr_JS_GetStringChars(jssCrop);
					if(priv->crop == NULL){
						priv->crop = MP_NEW( mlBounds); ML_INIT_BOUNDS((*(priv->crop)));
					}
					mlCropParseInfo[0].ptr = &(priv->crop->x);
					mlCropParseInfo[1].ptr = &(priv->crop->y);
					mlCropParseInfo[2].ptr = &(priv->crop->width);
					mlCropParseInfo[3].ptr = &(priv->crop->height);
					mlString wsCrop = L"("; wsCrop += jscCrop; wsCrop += L")";
					const wchar_t* pwc = wsCrop.c_str();
					if(ParseStruct(cx, pwc, mlCropParseInfo)){
						priv->CropChanged();
					}else{
						mlTraceWarning(cx, "'crop' property value is invalid");
						MP_DELETE( priv->crop);
						priv->crop = NULL;
					}
				}else{
					mlTraceWarning(cx, "'crop' property value is of unexpected type");
				}
				}break;
			case JSPROP_compressed:
				ML_JSVAL_TO_BOOL(priv->compressed, *vp);
				priv->CompressedChanged();
				break;
			case JSPROP_angle:
				ML_JSVAL_TO_INT(priv->angle, *vp);
				priv->AngleChanged();
				break;
			case JSPROP_maxDecreaseKoef:{
				ML_JSVAL_TO_INT(priv->maxDecreaseKoef, *vp);
				int _maxDecreaseKoef = priv->maxDecreaseKoef;
				if (_maxDecreaseKoef != 1 && _maxDecreaseKoef != 2 && _maxDecreaseKoef != 4)
				{
					mlTraceWarning(cx, "'maxDecreaseKoef' property value is in unexpected range. Expected is 1,2 or 4");
					priv->maxDecreaseKoef = NULL;
				}
				else
					priv->MaxDecreaseKoefChanged();
				break;}
			case JSPROP_sysmem:
				ML_JSVAL_TO_BOOL(priv->sysmem, *vp);
				priv->SysmemChanged();
				break;
			case JSPROP_remoteControlNow:
				ML_JSVAL_TO_BOOL(priv->mbRemoteControlNow, *vp);
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

void mlRMMLImage::CreateColorJSO(JSObject* &jso, mlColor* apColor){
	jso = mlColorJSO::newJSObject(mcx);
	mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(mcx,jso);
	pColor->SetColorRef(apColor, this);
	SAVE_FROM_GC(mcx,mjso,jso)
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLImage::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLImage);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID) {
			case JSPROP_editable:
				*vp = BOOLEAN_TO_JSVAL(priv->GetEditable());
				break;
			case JSPROP_editableNow:
				*vp = BOOLEAN_TO_JSVAL(priv->GetEditableNow());
				break;
			case JSPROP_color:
				if(priv->mjsoColor == NULL)
					priv->CreateColorJSO(priv->mjsoColor, &priv->color);
				*vp=OBJECT_TO_JSVAL(priv->mjsoColor);
				break;
			case JSPROP_tile:
				*vp = BOOLEAN_TO_JSVAL(priv->tile);
				break;
			case JSPROP_smooth:
				*vp = BOOLEAN_TO_JSVAL(priv->smooth);
				break;
			case JSPROP_crop:{
				if(priv->crop == NULL){
					*vp = JS_GetEmptyStringValue(cx);
				}else{
					char buf[255];
					sprintf_s(buf, 255, "%d, %d, %d, %d", priv->crop->x, priv->crop->y, priv->crop->width, priv->crop->height);
					JSString* jss = JS_NewStringCopyZ(cx, buf);
					*vp = STRING_TO_JSVAL(jss);
				}
				}break;
			case JSPROP_compressed:
				*vp = BOOLEAN_TO_JSVAL(priv->GetCompressed());
				break;
			case JSPROP_angle:
				*vp = INT_TO_JSVAL(priv->GetAngle());
				break;
			case JSPROP_maxDecreaseKoef:
				*vp = INT_TO_JSVAL(priv->GetMaxDecreaseKoef());
				break;
			case JSPROP_sysmem:
				*vp = BOOLEAN_TO_JSVAL(priv->GetSysmem());
				break;
			case JSPROP_remoteControlNow:
				*vp = BOOLEAN_TO_JSVAL(priv->GetRemoteControlNow());
				break;
			case JSPROP_editToolUsed:
				*vp = BOOLEAN_TO_JSVAL(priv->isPaintToolUsed());
				break;
			case JSPROP_firstSharingFrameDrawed:
				*vp = BOOLEAN_TO_JSVAL(priv->isFirstSharingFrameDrawed());
				break;

				
			}
	GET_PROPS_END;
	return JS_TRUE;
}

/**********************************
 * реализация  mlILoadable
 */
//const wchar_t* mlRMMLImage::GetSrc(){
//	jsval v;
//	if(JS_GetProperty(mcx, mjso, MLELPN_SRC, &v)){
//		if(JSVAL_IS_STRING(v)){
//			JSString* jss=JS_ValueToString(mcx, v);
//			return JS_GetStringChars(jss);
//		}
//	}
//	// ??
//	return L"";
//}

// реализация mlRMMLElement
mlresult mlRMMLImage::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLImage::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLImage::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	int iOldType=mType;
	mType=MLMT_IMAGE;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. Restore
		mType=iOldType;
	}
	return ML_OK;
}

void mlRMMLImage::DuplicateMedia(mlRMMLElement* apSource)
{
	DuplicateFrom(apSource);
}

mlRMMLElement* mlRMMLImage::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLImage* pNewEL=(mlRMMLImage*)GET_RMMLEL(mcx, mlRMMLImage::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	pNewEL->DuplicateFrom(apNewParent);
	return pNewEL;
}

//void mlRMMLImage::JSDestructor(JSContext *cx, JSObject *obj) {
//	mlRMMLImage *p = (mlRMMLImage*)JS_GetPrivate(cx, obj);
//	if (p){
//		 MP_DELETE( p;
//	}
//}


JSBool mlRMMLImage::JSFUNC_makeSnapshot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	/*if (argc < 1){
		JS_ReportError(cx,"makeSnapshot method must get one argument at least");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}*/
	typedef std::vector<mlRMMLElement*> v_elems_s;
	v_elems_s vVisibles;
	mlRect rcCrop; ML_INIT_RECT(rcCrop);
	for(int iArg = 0; iArg < (int)argc; iArg++){
		mlRMMLElement* pMLEL=NULL;
		jsval vArg = argv[iArg];
		if(!JSVAL_IS_VOID(vArg) && JSVAL_IS_OBJECT(vArg)){
			JSObject* jso = JSVAL_TO_OBJECT(vArg);
			if(JSO_IS_MLEL(cx, jso)){
				pMLEL=(mlRMMLElement*)JS_GetPrivate(cx,jso);
				if(!pMLEL->IsVisible()) pMLEL=NULL;
			}else if(JS_IsArrayObject(cx, jso)){
				jsuint uiLen=0;
				if(JS_GetArrayLength(cx, jso, &uiLen)){
					for(int jj=0; jj<(int)uiLen; jj++){
						jsval v;
						if(JS_GetElement(cx, jso, jj, &v)){
							if(JSVAL_IS_REAL_OBJECT(v)){
								JSObject* jsoElem = JSVAL_TO_OBJECT(v);
								if(JSO_IS_MLEL(cx, jsoElem)){
									pMLEL = (mlRMMLElement*)JS_GetPrivate(cx,jsoElem);
									if(pMLEL->IsComposition()){
										mlRMMLIterator iter(cx,pMLEL);
										mlRMMLElement* pElem = NULL;
										while((pElem = iter.GetNext()) != NULL){
											if(pElem->GetMO() == NULL){
												JS_ReportWarning(cx,"makeSnapshot argument composition is not completely loaded yet");
												*rval = JSVAL_FALSE;
												return JS_TRUE;
											}
											if(pElem->IsVisible() && !PMO_IS_NULL(pElem->GetMO()))
												vVisibles.push_back(pElem);
										}
									}else{
										//для снятия снимка 3D объекта
										//if(pMLEL->IsVisible())
											vVisibles.push_back(pMLEL);
									}
									pMLEL = NULL;
								}
							}
						}
					}
				}
			}else if(mlRectangleJSO::IsInstance(cx, jso)){
				mlRectangleJSO* pRectJSO = (mlRectangleJSO*)JS_GetPrivate(cx, jso);
				rcCrop = pRectJSO->GetRect();
				break;
			}else{
				if(iArg == (argc-1))
					JS_ReportWarning(cx,"makeSnapshot arguments is not valid for the crop rectangle");
			}
		}else if(JSVAL_IS_NUMBER(vArg)){
			for(int ii=0; iArg < (int)argc; iArg++, ii++){
				jsval vArg = argv[iArg];
				if(!JSVAL_IS_NUMBER(vArg)){
					JS_ReportWarning(cx,"makeSnapshot arguments is not valid for the crop rectangle");
					break;
				}
				jsdouble dArg;
				if(!JS_ValueToNumber(cx,vArg,&dArg)){
					JS_ReportWarning(cx,"makeSnapshot arguments is not valid for the crop rectangle");
					break;
				}
				if(ii == 4)
					break;
				switch(ii){
				case 0: // left
					rcCrop.left = (int)dArg;
					break;
				case 1: // top
					rcCrop.top = (int)dArg;
					break;
				case 2: // right
					rcCrop.right = (int)dArg;
					break;
				case 3: // bottom
					rcCrop.bottom = (int)dArg;
					break;
				}
			}
			break;
		}
		/*if(pMLEL == NULL && vVisibles.empty()){
			JS_ReportError(cx,"makeSnapshot method must get a visible RMML-object");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}*/
		if(pMLEL != NULL){
			if(pMLEL->IsComposition()){
				mlRMMLIterator iter(cx,pMLEL);
				mlRMMLElement* pElem=NULL;
				while((pElem=iter.GetNext())!=NULL){
					if(pElem->GetMO() == NULL){
						JS_ReportWarning(cx,"makeSnapshot argument composition is not completely loaded yet");
						*rval=JSVAL_FALSE;
						return JS_TRUE;
					}
					if(pElem->IsVisible() && !PMO_IS_NULL(pElem->GetMO()))
						vVisibles.push_back(pElem);
				}
			}else{
				if(pMLEL->GetMO() == NULL){
					JS_ReportWarning(cx,"makeSnapshot argument element is not created yet");
					*rval=JSVAL_FALSE;
					return JS_TRUE;
				}
				vVisibles.push_back(pMLEL);
			}
		}
	}
	vVisibles.push_back(NULL);
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	size_t cnt = vVisibles.size();
#if _MSC_VER >= 1600
	mlRMMLElement **p = vVisibles.begin()._Ptr;
#elif _MSC_VER > 1200
	mlRMMLElement **p = vVisibles.begin()._Myptr;
#else
	mlRMMLElement **p = vVisibles.begin();
#endif
	 if (!pImage->MakeSnapshot( (mlMedia **)p, rcCrop))
	 {
		 JS_ReportWarning(cx,"makeSnapshot is not correctly render");
		 *rval=JSVAL_FALSE;
		 return JS_TRUE;
	 }

	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_drawLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 1){
		JS_ReportError(cx,"Invalid arguments for drawLine method.\n Expected format: drawLine(x1, y1, x2, y2, color, weight)");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}
	int iX1 = 0, iY1 = 0, iX2 = 0, iY2 = 0;
	mlColor color; ML_INIT_COLOR(color);
	int iWeight = 1;
	bool bError = true;
	int argi = 0;
	for(;;){
		jsdouble jsd;
		if((int)argc <= argi) break;
		if(!JS_ValueToNumber(cx, argv[argi], &jsd))
			break;
		iX1 = jsd;
		argi++;
		if((int)argc <= argi) break;
		if(!JS_ValueToNumber(cx, argv[argi], &jsd))
			break;
		iY1 = jsd;
		argi++;
		if((int)argc <= argi) break;
		if(!JS_ValueToNumber(cx, argv[argi], &jsd))
			break;
		iX2 = jsd;
		argi++;
		if((int)argc <= argi) break;
		if(!JS_ValueToNumber(cx, argv[argi], &jsd))
			break;
		iY2 = jsd;
		argi++;
		if((int)argc >= argi+1){
			if(JSVAL_IS_STRING(argv[argi])){
				JSString* jssColor = JSVAL_TO_STRING(argv[argi]);
				wchar_t* pwc=wr_JS_GetStringChars(jssColor);
				ParseColor(cx,(const wchar_t *&) pwc,color);
			}else if(JSVAL_IS_REAL_OBJECT(argv[argi])){
				JSObject* jsoColor = JSVAL_TO_OBJECT(argv[argi]);
				if(!mlColorJSO::IsInstance(cx, jsoColor))
					break;
				mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
				color = pColor->GetColor();
			}else
				break;
			argi++;
		}
		if((int)argc >= argi+1){
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iWeight = jsd;
		}
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx,"Invalid arguments for drawLine method.\n Expected format: drawLine(x1, y1, x2, y2, color, weight)");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->DrawLine(iX1, iY1, iX2, iY2, color, iWeight);
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_fillRectangle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 1){
		JS_ReportError(cx,"Invalid arguments for fillRectangle method.\n Expected format: fillRectangle(x1, y1, x2, y2, color)");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}
	int iX1 = 0, iY1 = 0, iX2 = 0, iY2 = 0;
	mlColor color; ML_INIT_COLOR(color);
	bool bError = true;
	int argi = 0;
	for(;;){
		if(JSVAL_IS_REAL_OBJECT(argv[argi])){
			JSObject* jsoRect = JSVAL_TO_OBJECT(argv[argi]);
			if(!mlRectangleJSO::IsInstance(cx, jsoRect))
				break;
			mlRectangleJSO* pRect = (mlRectangleJSO*)JS_GetPrivate(cx, jsoRect);
			mlRect rect = pRect->GetRect();
			iX1 = rect.left;
			iY1 = rect.top;
			iX2 = rect.right;
			iY2 = rect.bottom;
			argi++;
		}else{
			jsdouble jsd;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iX1 = jsd;
			argi++;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iY1 = jsd;
			argi++;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iX2 = jsd;
			argi++;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iY2 = jsd;
			argi++;
		}
		if((int)argc >= argi+1){
			if(JSVAL_IS_STRING(argv[argi])){
				JSString* jssColor = JSVAL_TO_STRING(argv[argi]);
				wchar_t* pwc=wr_JS_GetStringChars(jssColor);
				ParseColor(cx, (const wchar_t *&) pwc,color);
			}else if(JSVAL_IS_REAL_OBJECT(argv[argi])){
				JSObject* jsoColor = JSVAL_TO_OBJECT(argv[argi]);
				if(!mlColorJSO::IsInstance(cx, jsoColor))
					break;
				mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
				color = pColor->GetColor();
			}else
				break;
			argi++;
		}
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx,"Invalid arguments for fillRectangle method.\n Expected format: fillRectangle(x1, y1, x2, y2, color)");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->FillRectangle(iX1, iY1, iX2, iY2, color);
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

// setCanvasSize(w,h) - устанавливает размер текстуры (свойства width и height, т.е. отображаемый размер картинки не меняет)
JSBool mlRMMLImage::JSFUNC_setCanvasSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL;
	*rval = JSVAL_FALSE;
	if(argc < 2){
		JS_ReportWarning(cx,"Invalid arguments for setCanvasSize method.\n Expected format: setCanvasSize(width, height)");
		return JS_TRUE;
	}
	int iWidth = 0;
	int iHeight = 0;
	jsval v = argv[0];
	if(!JSVAL_IS_INT(v)){
		JS_ReportWarning(cx,"First argument of setCanvasSize method must be an integer");
		return JS_TRUE;
	}
	iWidth = JSVAL_TO_INT(v);
	v = argv[1];
	if(!JSVAL_IS_INT(v)){
		JS_ReportWarning(cx,"Second argument of setCanvasSize method must be an integer");
		return JS_TRUE;
	}
	iHeight = JSVAL_TO_INT(v);
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->SetCanvasSize(iWidth, iHeight);
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// drawEllipse(x1, y1, x2, y2, color, weight, fillColor)
//  x1, y1, x2, y2 - координаты прямоугольника, в который должен вписаться эллипс (толщина линии не учитывается)
//  color - цвет линии
//  weight - толщина линии
//  fillColor - цвет заливки
JSBool mlRMMLImage::JSFUNC_drawEllipse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 1){
		JS_ReportWarning(cx,"Invalid arguments for drawEllipse method.\n Expected format: drawEllipse(x1, y1, x2, y2, color, weight, fillColor)");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}
	int iX1 = 0, iY1 = 0, iX2 = 0, iY2 = 0;
	mlColor color; ML_INIT_COLOR(color);
	mlColor fillColor; ML_INIT_COLOR(fillColor);
	int iWeight = 1;
	bool bError = true;
	int argi = 0;
	for(;;){
		jsdouble jsd;
		if((int)argc <= argi) break;
		if(!JS_ValueToNumber(cx, argv[argi], &jsd))
			break;
		iX1 = jsd;
		argi++;
		if((int)argc <= argi) break;
		if(!JS_ValueToNumber(cx, argv[argi], &jsd))
			break;
		iY1 = jsd;
		argi++;
		if((int)argc <= argi) break;
		if(!JS_ValueToNumber(cx, argv[argi], &jsd))
			break;
		iX2 = jsd;
		argi++;
		if((int)argc <= argi) break;
		if(!JS_ValueToNumber(cx, argv[argi], &jsd))
			break;
		iY2 = jsd;
		argi++;
		if((int)argc >= argi+1){
			if(JSVAL_IS_STRING(argv[argi])){
				JSString* jssColor = JSVAL_TO_STRING(argv[argi]);
				wchar_t* pwc=wr_JS_GetStringChars(jssColor);
				ParseColor(cx,(const wchar_t *&) pwc,color);
			}else if(JSVAL_IS_REAL_OBJECT(argv[argi])){
				JSObject* jsoColor = JSVAL_TO_OBJECT(argv[argi]);
				if(!mlColorJSO::IsInstance(cx, jsoColor))
					break;
				mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
				color = pColor->GetColor();
			}else
				break;
			argi++;
		}
		if((int)argc >= argi+1){
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iWeight = jsd;
		}
		argi++;
		if((int)argc >= argi+1){
			if(JSVAL_IS_STRING(argv[argi])){
				JSString* jssColor = JSVAL_TO_STRING(argv[argi]);
				wchar_t* pwc=wr_JS_GetStringChars(jssColor);
				ParseColor(cx,(const wchar_t *&)pwc,fillColor);
			}else if(JSVAL_IS_REAL_OBJECT(argv[argi])){
				JSObject* jsoColor = JSVAL_TO_OBJECT(argv[argi]);
				if(!mlColorJSO::IsInstance(cx, jsoColor))
					break;
				mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
				fillColor = pColor->GetColor();
			}else
				break;
			argi++;
		}
		bError = false;
		break;
	}
	if(bError){
		JS_ReportWarning(cx,"Invalid arguments for drawEllipse method.\n Expected format: drawEllipse(x1, y1, x2, y2, color, weight, fillColor)");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->DrawEllipse(iX1, iY1, iX2, iY2, color, iWeight, fillColor);
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

// drawRectangle(x1, y1, x2, y2, color, weight, fillColor)
//  x1, y1, x2, y2 - координаты прямоугольника, (толщина линии не учитывается)
//  color - цвет линии
//  weight - толщина линии
//  fillColor - цвет заливки
JSBool mlRMMLImage::JSFUNC_drawRectangle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if(argc < 1){
			JS_ReportWarning(cx,"Invalid arguments for drawRectangle method.\n Expected format: drawRectangle(x1, y1, x2, y2, color, weight, fillColor)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		int iX1 = 0, iY1 = 0, iX2 = 0, iY2 = 0;
		mlColor color; ML_INIT_COLOR(color);
		mlColor fillColor; ML_INIT_COLOR(fillColor);
		int iWeight = 1;
		bool bError = true;
		int argi = 0;
		for(;;){
			jsdouble jsd;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iX1 = jsd;
			argi++;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iY1 = jsd;
			argi++;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iX2 = jsd;
			argi++;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			iY2 = jsd;
			argi++;
			if((int)argc >= argi+1){
				if(JSVAL_IS_STRING(argv[argi])){
					JSString* jssColor = JSVAL_TO_STRING(argv[argi]);
					wchar_t* pwc=wr_JS_GetStringChars(jssColor);
					ParseColor(cx,(const wchar_t *&) pwc,color);
				}else if(JSVAL_IS_REAL_OBJECT(argv[argi])){
					JSObject* jsoColor = JSVAL_TO_OBJECT(argv[argi]);
					if(!mlColorJSO::IsInstance(cx, jsoColor))
						break;
					mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
					color = pColor->GetColor();
				}else
					break;
				argi++;
			}
			if((int)argc >= argi+1){
				if(!JS_ValueToNumber(cx, argv[argi], &jsd))
					break;
				iWeight = jsd;
			}
			argi++;
			if((int)argc >= argi+1){
				if(JSVAL_IS_STRING(argv[argi])){
					JSString* jssColor = JSVAL_TO_STRING(argv[argi]);
					wchar_t* pwc=wr_JS_GetStringChars(jssColor);
					ParseColor(cx,(const wchar_t *&)pwc,fillColor);
				}else if(JSVAL_IS_REAL_OBJECT(argv[argi])){
					JSObject* jsoColor = JSVAL_TO_OBJECT(argv[argi]);
					if(!mlColorJSO::IsInstance(cx, jsoColor))
						break;
					mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
					fillColor = pColor->GetColor();
				}else
					break;
				argi++;
			}
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for drawRectangle method.\n Expected format: drawRectangle(x1, y1, x2, y2, color, weight, fillColor)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->DrawRectangle(iX1, iY1, iX2, iY2, color, iWeight, fillColor);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_getPixel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if(argc < 2){
		JS_ReportWarning(cx,"Invalid arguments for getPixel method.\n Expected format: getPixel(x, y)");
		*rval=JSVAL_NULL;
		return JS_TRUE;
	}
	int iX = 0, iY = 0;
	//
	bool bError = true;
	for(;;){
		jsdouble jsd;
		if(!JS_ValueToNumber(cx, argv[0], &jsd))
			break;
		iX = jsd;
		if(!JS_ValueToNumber(cx, argv[1], &jsd))
			break;
		iY = jsd;
		bError = false;
		break;
	}
	if(bError){
		JS_ReportWarning(cx,"Invalid arguments for getPixel method.\n Expected format: getPixel(x, y)");
		*rval=JSVAL_NULL;
		return JS_TRUE;
	}
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	if(iX < 0 || iY < 0 || iX > pImage->mSize.width || iY > pImage->mSize.height){
		JS_ReportWarning(cx,"Out of bounds in arguments of getPixel method.");
		*rval=JSVAL_NULL;
		return JS_TRUE;
	}
	mlColor color = pImage->GetPixel(iX, iY);
	JSObject* jso = mlColorJSO::newJSObject(cx);
	mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jso);
	pColor->SetColor(color);
	*rval = OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_undoAction(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc > 0)
		{
			JS_ReportWarning(cx,"Invalid arguments for undoAction method.\n Expected format: undoAction()");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->UndoAction();
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_isBinaryStateReceived(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc > 0)
		{
			JS_ReportWarning(cx,"Invalid arguments for isBinaryStateReceived method.\n Expected format: isBinaryStateReceived()");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	bool isReceived = pImage->IsBinaryStateReceived();
	*rval = BOOLEAN_TO_JSVAL(isReceived);
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_pasteText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc > 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for pasteText method.\n Expected format: pasteText()");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);

		if (argc == 1)
		{
			if (!JSVAL_IS_BOOLEAN(argv[0])) 
			{
				JS_ReportWarning(cx,"Invalid arguments for pasteText method.\n Expected format: pasteText(true/false)");
				*rval=JSVAL_NULL;
				return JS_TRUE;
			}
			else
			{
				bool isPasteOnIconClick = JSVAL_TO_BOOLEAN(argv[0]);
				pImage->PasteText(isPasteOnIconClick);
			}
		}
		else
		{
			pImage->PasteText();
		}

		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setActiveTool(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setActiveTool method.\n Expected format: setActiveTool(num)");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		int toolNum = 0;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			jsdouble jsd;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			toolNum = jsd;
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setActiveTool method.\n Expected format: setActiveTool(num)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetActiveTool(toolNum);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setLineWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setLineWidth method.\n Expected format: setLineWidth(num)");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		int lineWidth = 0;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			jsdouble jsd;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			lineWidth = jsd;
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setLineWidth method.\n Expected format: setLineWidth(num)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetLineWidth(lineWidth);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setLineColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setLineColor method.\n Expected format: setLineColor(\"#ffffff\")");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		mlColor color; ML_INIT_COLOR(color);
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			if((int)argc <= argi) break;
			if(JSVAL_IS_STRING(argv[argi])){
				JSString* jssColor = JSVAL_TO_STRING(argv[argi]);
				wchar_t* pwc=wr_JS_GetStringChars(jssColor);
				ParseColor(cx,(const wchar_t *&)pwc,color);
			}else if(JSVAL_IS_REAL_OBJECT(argv[argi])){
				JSObject* jsoColor = JSVAL_TO_OBJECT(argv[argi]);
				if(!mlColorJSO::IsInstance(cx, jsoColor))
					break;
				mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
				color = pColor->GetColor();
			}else
				break;
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setLineColor method.\n Expected format: setLineColor(\"#ffffff\")");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetLineColor(color);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setBgColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setBgColor method.\n Expected format: setBgColor(\"#ffffff\")");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		mlColor color; ML_INIT_COLOR(color);
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			if((int)argc <= argi) break;
			if(JSVAL_IS_STRING(argv[argi])){
				JSString* jssColor = JSVAL_TO_STRING(argv[argi]);
				wchar_t* pwc=wr_JS_GetStringChars(jssColor);
				ParseColor(cx,(const wchar_t *&)pwc,color);
			}else if(JSVAL_IS_REAL_OBJECT(argv[argi])){
				JSObject* jsoColor = JSVAL_TO_OBJECT(argv[argi]);
				if(!mlColorJSO::IsInstance(cx, jsoColor))
					break;
				mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
				color = pColor->GetColor();
			}else
				break;
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setBgColor method.\n Expected format: setBgColor(\"#ffffff\")");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetBgColor(color);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setToolOpacity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setToolOpacity method.\n Expected format: setToolOpacity(num)");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		int toolOpacity = 0;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			jsdouble jsd;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			toolOpacity = jsd;
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setToolOpacity method.\n Expected format: setToolOpacity(num)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetToolOpacity(toolOpacity);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setTextFontName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLImage* pThis = (mlRMMLImage*)JS_GetPrivate(cx, obj);
	*rval = BOOLEAN_TO_JSVAL(false);
	if(argc <= 0){
		JS_ReportError(cx, "setTextFontName-method must get one argument!");
		return JS_TRUE;
	}
	mlString sText;
	int iarg = 0;
	if(!JSVAL_IS_STRING(argv[iarg])){
		JS_ReportError(cx, "First argument (text) of setTextFontName-method must be a string");
		return JS_TRUE;
	}
	sText = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[iarg]));

	pThis->SetTextFontName((wchar_t *)sText.c_str());

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setTextBold(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setTextBold method.\n Expected format: setTextBold(bool)");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		bool isBold = true;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
//			jsdouble jsd;
			if((int)argc <= argi) break;
			if (!JSVAL_IS_BOOLEAN(argv[argi])) break;
			isBold = JSVAL_TO_BOOLEAN(argv[argi]);//ML_JSVAL_TO_BOOL(isBold, *cx)
				bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setTextBold method.\n Expected format: setTextBold(bool)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetTextBold(isBold);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setTextItalic(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setTextItalic method.\n Expected format: setTextItalic(bool)");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		bool isItalic = true;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			if((int)argc <= argi) break;
			if (!JSVAL_IS_BOOLEAN(argv[argi])) break;
			isItalic = JSVAL_TO_BOOLEAN(argv[argi]);//ML_JSVAL_TO_BOOL(isItalic, *cx)
				bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setTextItalic method.\n Expected format: setTextItalic(bool)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetTextItalic(isItalic);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setTextUnderline(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setTextUnderline method.\n Expected format: setTextUnderline(bool)");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		bool isUnderline = true;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			if((int)argc <= argi) break;
			if (!JSVAL_IS_BOOLEAN(argv[argi])) break;
			isUnderline = JSVAL_TO_BOOLEAN(argv[argi]);//ML_JSVAL_TO_BOOL(isUnderline, *cx)
				bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setTextUnderline method.\n Expected format: setTextUnderline(bool)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetTextUnderline(isUnderline);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setTextAntialiasing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setTextAntialiasing method.\n Expected format: setTextAntialiasing(bool)");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		bool isAntialiasing = true;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			if((int)argc <= argi) break;
			if (JSVAL_IS_BOOLEAN(argv[argi])) break;
			isAntialiasing = JSVAL_TO_BOOLEAN(argv[argi]);//ML_JSVAL_TO_BOOL(isAntialiasing, *cx)
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setTextAntialiasing method.\n Expected format: setTextAntialiasing(bool)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetTextAntialiasing(isAntialiasing);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_clearOldScreen(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc > 0)
	{
		JS_ReportWarning(cx,"Invalid arguments for clearOldScreen() should be no arguments");
		*rval=JSVAL_NULL;
		return JS_TRUE;
	}

	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->ClearOldScreen();
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_copyToClipboard(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->CopyToClipboard();
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->Save();
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_resample(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc != 2)
	{
		JS_ReportWarning(cx,"Invalid arguments for resample method.\n Expected format: resample(windth, height)");
		*rval=JSVAL_NULL;
		return JS_TRUE;
	}
	int iWidth=0, iHeight=0;
	if(!JSVAL_IS_INT(argv[0])){
		JS_ReportError(cx,"resample-method first parameter must be an integer");
		return JS_TRUE;
	}
	iWidth=JSVAL_TO_INT(argv[0]);
	if( iWidth <= 0)
	{
		JS_ReportError(cx,"resample-method first parameter must be greater than 0");
		return JS_TRUE;
	}

	if(!JSVAL_IS_INT(argv[1])){
		JS_ReportError(cx,"resample-method second parameter must be an integer");
		return JS_TRUE;
	}
	iHeight=JSVAL_TO_INT(argv[1]);
	if( iHeight <= 0)
	{
		JS_ReportError(cx,"resample-method second parameter must be greater than 0");
		return JS_TRUE;
	}

	mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
	pImage->Resample(iWidth, iHeight);
	*rval=JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setToolMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setMode method.\n Expected format: setMode(num)");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		int toolMode = 0;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			jsdouble jsd;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			toolMode = jsd;
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setMode method.\n Expected format: setMode(num)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetToolMode(toolMode);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

JSBool mlRMMLImage::JSFUNC_setLineDirection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CHECK_DONT_IMPL
		if (argc < 1)
		{
			JS_ReportWarning(cx,"Invalid arguments for setLineDirection method.\n Expected format: setLineDirection(num) 0 - no direction 1 - up 2 - down");
			*rval=JSVAL_NULL;
			return JS_TRUE;
		}
		int lineDirection = 0;
		bool bError = true;
		int argi = 0;
		for (;;)
		{
			jsdouble jsd;
			if((int)argc <= argi) break;
			if(!JS_ValueToNumber(cx, argv[argi], &jsd))
				break;
			lineDirection = jsd;
			bError = false;
			break;
		}
		if(bError){
			JS_ReportWarning(cx,"Invalid arguments for setLineDirection method.\n Expected format: setLineDirection(num)");
			*rval=JSVAL_FALSE;
			return JS_TRUE;
		}
		mlRMMLImage* pImage=(mlRMMLImage*)JS_GetPrivate(cx, obj);
		pImage->SetLineDirection(lineDirection);
		*rval=JSVAL_TRUE;
		return JS_TRUE;
}

void mlRMMLImage::EditableChanged()
{
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->EditableChanged();
}

void mlRMMLImage::EditableNowChanged()
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->EditableNowChanged();
}

bool mlRMMLImage::isPaintToolUsed()
{
	return mpMO->GetIImage()->isPaintToolUsed();
}

bool mlRMMLImage::isFirstSharingFrameDrawed()
{
	return mpMO->GetIImage()->isFirstSharingFrameDrawed();
}

void mlRMMLImage::ColorChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->ColorChanged();
}

void mlRMMLImage::TileChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->TileChanged();
}

void mlRMMLImage::SmoothChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SmoothChanged();
}

void mlRMMLImage::CropChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->CropChanged();
}

void mlRMMLImage::CompressedChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->CompressedChanged();
}

void mlRMMLImage::AngleChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->AngleChanged();
}

void mlRMMLImage::MaxDecreaseKoefChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->MaxDecreaseKoefChanged();
}

void mlRMMLImage::SysmemChanged(){
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SysmemChanged();
}

void mlRMMLImage::SetActiveTool(int aToolNumber)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetActiveTool(aToolNumber);
}

void mlRMMLImage::PasteText(bool isPasteOnIconClick)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->PasteText(isPasteOnIconClick);
}

void mlRMMLImage::UndoAction()
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->UndoAction();
}

void mlRMMLImage::SetLineWidth(int aLineWidth)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetLineWidth(aLineWidth);
}

void mlRMMLImage::SetLineColor(const mlColor aColor)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetLineColor(aColor);
}

void mlRMMLImage::SetBgColor(const mlColor aColor)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetBgColor(aColor);
}

void mlRMMLImage::SetToolOpacity(int aOpacity)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetToolOpacity(aOpacity);
}

void mlRMMLImage::SetTextFontName(wchar_t* aFontName)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetTextFontName(aFontName);
}

void mlRMMLImage::SetTextBold(bool isBold)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetTextBold(isBold);
}

void mlRMMLImage::SetTextItalic(bool isItalic)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetTextItalic(isItalic);
}

void mlRMMLImage::SetTextUnderline(bool isUnderline)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetTextUnderline(isUnderline);
}

void mlRMMLImage::SetTextAntialiasing(bool isAntialiasing)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetTextAntialiasing(isAntialiasing);
}

void mlRMMLImage::SetToolMode(int aMode)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetToolMode(aMode);
}

void mlRMMLImage::SetLineDirection(int aDirection)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->SetLineDirection(aDirection);
}

void mlRMMLImage::Resample(int aiWidth, int aiHeight)
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->Resample( aiWidth, aiHeight);
}

void mlRMMLImage::ClearOldScreen()
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->ClearOldScreen();
}

void mlRMMLImage::CopyToClipboard()
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->CopyToClipboard();
}

void mlRMMLImage::Save()
{
	if (PMO_IS_NULL(mpMO)) return;
	mpMO->GetIImage()->Save();
}

bool mlRMMLImage::ChangeState( unsigned char type, unsigned char *aData, unsigned int aDataSize)
{
	if (PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIImage()->ChangeState( type, aData,  aDataSize);
}

bool mlRMMLImage::GetBinState( BYTE*& aBinState, int& aBinStateSize)
{
	if (PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIImage()->GetBinState( aBinState, aBinStateSize);
}

bool mlRMMLImage::SetIPadDrawing( bool abIPadDrawing)
{
	if (PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIImage()->SetIPadDrawing( abIPadDrawing);
}

bool mlRMMLImage::ApplyIPadState()
{
	if (PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIImage()->ApplyIPadState();
}

bool mlRMMLImage::IsBinaryStateReceived()
{
	if (PMO_IS_NULL(mpMO)) return false;
	return mpMO->GetIImage()->IsBinaryStateReceived();
}

}