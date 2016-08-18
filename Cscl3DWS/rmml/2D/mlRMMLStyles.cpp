#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlSPParser.h"
#include <sstream>
#include "config/prolog.h"

namespace rmml {

mlRMMLStyles::stlStyles::~stlStyles(){
	MP_VECTOR<stlStyle*>::iterator vi; 
	for(vi=mvStyles.begin(); vi != mvStyles.end(); vi++ ){
//		ML_ASSERTION(mcx, *vi!=NULL,"mlRMMLStyles::stlStyles::~stlStyles()");
		MP_DELETE( *vi);
	}
}

mlRMMLStyles::stlStyles::stlStyle* mlRMMLStyles::stlStyles::GetStyle(const wchar_t* apwcTag){
	if(apwcTag==NULL) return mvStyles[0];
	// ??
	// if apwcTag not found then return default style
	return mvStyles[0];
}

mlRMMLStyles::stlStyles::stlStyle* mlRMMLStyles::stlStyles::CreateStyle(JSString* ajssTag){
	if(ajssTag==NULL) return mvStyles[0];
	// ??
	return NULL; // ??
}

mlRMMLStyles::mlRMMLStyles(void)
{
	mType=MLMT_STYLES;
	mRMMLType=MLMT_STYLES;

//	mbCreateMediaCalled=false;
	htmlTag=NULL;
	mpStyle = MP_NEW( stlStyles::stlStyle);
}

mlRMMLStyles::~mlRMMLStyles(void){
	if(mpMO == NULL){
		MP_DELETE( mpStyle);
	}
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLStyles::_JSPropertySpec[] = {
	{ "value", JSPROP_value, JSPROP_ENUMERATE, 0, 0},
	JSPROP_RW(htmlTag) // current HTML tag name (following properties for)
	JSPROP_RW(font) // example: 'italic small-caps bold 12pt serif'
	JSPROP_RW(fontStyle) // 'normal' 'italic'
	JSPROP_RW(fontWeight) // 'normal' 'bold'
	JSPROP_RW(fontSize) // example: '14pt'
	JSPROP_RW(lineHeight) // 'normal' '5pt' '10%'
	JSPROP_RW(fontFamily) // example: 'Times New Roman'
	JSPROP_RW(textAlign) // 'left' 'right' 'center' 'justify'
	JSPROP_RW(textIndent) // example: '10px'
	JSPROP_RW(textDecoration) // 'none' 'underline'
	JSPROP_RW(color) // 'red' '#F00' '#FF0000' 'rgb 1.0 0.0 0.0'
	JSPROP_RW(antialiasing) // 'none' 'smooth' (по умолчанию - как установлено движком - smooth)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLStyles::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Styles,mlRMMLStyles,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLStyles)

JSBool ParseIntWithUnits(JSContext* cx, jsval *vp,mlStyle::IntWithUnits& aIWU);
JSString* IntWithUnitsToString(JSContext* cx, mlStyle::IntWithUnits& aIWU);


#define SET_EPROP_BEGIN(PN,UND) \
				if(JSVAL_IS_NULL(*vp)){ \
					priv->mpStyle->PN=mlStyle::UND; \
				}else if(JSVAL_IS_STRING(*vp)){ \
					JSString* jss=JSVAL_TO_STRING(*vp); \
					if(false){ 
#define SET_EPROP_VALUE(PN,ENC,VAL) \
					}else if(isEqual(jss, L#VAL)){ \
						priv->mpStyle->PN=mlStyle::ENC;
#define SET_EPROP_END(ERRMES) \
					}else{ \
						JS_ReportError(cx,#ERRMES); \
						return JS_FALSE; \
					} \
				}else{ \
					JS_ReportError(cx,"type missmatch"); \
					return JS_FALSE; \
				}


#define GET_EPROP_BEGIN(PN,UND) \
				if(priv->mpStyle->PN==mlStyle::UND){ \
					*vp=JS_GetEmptyStringValue(cx); 
#define GET_EPROP_VALUE(PN,ENC,VAL) \
				}else if(priv->mpStyle->PN==mlStyle::ENC){ \
					*vp=STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx,L#VAL));
#define GET_EPROP_END(ERRMES) \
				}else{ \
					ML_ASSERTION(cx, FALSE,"mlRMMLStyles::JSGetProperty"); \
				}

JSBool ParseColor(JSContext* cx, jsval *vp, mlStyle::Color& aColor);

///// JavaScript Set Property Wrapper
JSBool mlRMMLStyles::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLStyles);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_value:
				// ??
				break;
			case JSPROP_htmlTag: // current HTML tag name (following properties for)
				if(JSVAL_IS_NULL(*vp)){
					priv->htmlTag=NULL;
				}else if(JSVAL_IS_STRING(*vp)){
					priv->htmlTag=JSVAL_TO_STRING(*vp);
				}
				if(!PMO_IS_NULL(priv->mpMO))
					priv->mpStyle=priv->mStyles.CreateStyle(priv->htmlTag);
				break;
			case JSPROP_font: // example: 'italic small-caps bold 12pt serif'
				// ??
				break;
			case JSPROP_fontStyle: // 'normal' 'italic'
				SET_EPROP_BEGIN(fontStyle,FS_undef)
				SET_EPROP_VALUE(fontStyle,FS_normal,normal)
				SET_EPROP_VALUE(fontStyle,FS_italic,italic)
				SET_EPROP_END(unknown font style value)
				break;
			case JSPROP_fontWeight: // 'normal' 'bold'
				SET_EPROP_BEGIN(fontWeight,FW_undef)
				SET_EPROP_VALUE(fontWeight,FW_normal,normal)
				SET_EPROP_VALUE(fontWeight,FW_bold,bold)
				SET_EPROP_END(unknown font weight value)
				break;
			case JSPROP_fontSize: // example: '14pt'
				if(ParseIntWithUnits(cx,vp,priv->mpStyle->fontSize)==JS_FALSE)
					return JS_FALSE;
				break;
			case JSPROP_lineHeight:{ // 'normal' '5pt' '10%'
				bool bDontParse=false;
					if(JSVAL_IS_STRING(*vp)){
						if(isEqual(JSVAL_TO_STRING(*vp),L"normal")){
							priv->mpStyle->lineHeight.units=mlStyle::U_undef;
							priv->mpStyle->lineHeight.val=-1;
							bDontParse=true;
						}
					}
					if(!bDontParse && ParseIntWithUnits(cx,vp,priv->mpStyle->lineHeight)==JS_FALSE)
						return JS_FALSE;
				}
				break;
			case JSPROP_fontFamily: // example: 'Times New Roman'
				if(JSVAL_IS_NULL(*vp)){
					ML_RELEASE(priv->mpStyle->fontFamily);
				}else if(JSVAL_IS_STRING(*vp)){
					ML_RELEASE(priv->mpStyle->fontFamily);
					JSString* jss=JSVAL_TO_STRING(*vp);
					int iLen=JS_GetStringLength(jss);
					jschar* jsc=JS_GetStringChars(jss);
					char* psz = MP_NEW_ARR( char, iLen+1); psz[iLen]='\0';
					priv->mpStyle->fontFamily=psz;
					for(int ii=0; ii<iLen; ii++){
						jschar wch=*jsc++;
						char ch=(char)wch;
						*psz++=ch;
					}
				}
				break;
			case JSPROP_textAlign: // 'left' 'right' 'center' 'justify'
				SET_EPROP_BEGIN(textAlign,TA_undef)
				SET_EPROP_VALUE(textAlign,TA_left,left)
				SET_EPROP_VALUE(textAlign,TA_right,right)
				SET_EPROP_VALUE(textAlign,TA_center,center)
				SET_EPROP_VALUE(textAlign,TA_justify,justify)
				SET_EPROP_END(unknown text align value)
				break;
			case JSPROP_textIndent: // example: '10px'
				if(ParseIntWithUnits(cx,vp,priv->mpStyle->fontSize)==JS_FALSE)
					return JS_FALSE;
				break;
			case JSPROP_textDecoration: // 'none' 'underline'
				SET_EPROP_BEGIN(textDecoration,TD_undef)
				SET_EPROP_VALUE(textDecoration,TD_none,none)
				SET_EPROP_VALUE(textDecoration,TD_underline,underline)
				SET_EPROP_END(unknown text decoration value)
				break;
			case JSPROP_color: // 'red' '#F00' '#FF0000' 'rgb 1.0 0.0 0.0'
				if(!ParseColor(cx, vp, priv->mpStyle->color)) return JS_FALSE;
				priv->mpStyle->color.def=true;
				break;
			case JSPROP_antialiasing: // 'none' 'smooth'
				SET_EPROP_BEGIN(antialiasing,AA_undef)
				SET_EPROP_VALUE(antialiasing,AA_none,none)
				SET_EPROP_VALUE(antialiasing,AA_smooth,smooth)
				SET_EPROP_END(unknown antialiasing value)
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLStyles::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLStyles);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_value:
				// ??
				break;
			case JSPROP_htmlTag: // current HTML tag name (following properties for)
				if(priv->htmlTag==NULL) *vp=JSVAL_NULL;
				else *vp=STRING_TO_JSVAL(priv->htmlTag);
				break;
			case JSPROP_font: // example: 'italic small-caps bold 12pt serif'
				// ??
				break;
			case JSPROP_fontStyle: // 'normal' 'italic'
				GET_EPROP_BEGIN(fontStyle,FS_undef)
				GET_EPROP_VALUE(fontStyle,FS_normal,normal)
				GET_EPROP_VALUE(fontStyle,FS_italic,italic)
				GET_EPROP_END(unknown font style value)
				break;
			case JSPROP_fontWeight: // 'normal' 'bold'
				GET_EPROP_BEGIN(fontWeight,FW_undef)
				GET_EPROP_VALUE(fontWeight,FW_normal,normal)
				GET_EPROP_VALUE(fontWeight,FW_bold,bold)
				GET_EPROP_END(unknown font weight value)
				break;
			case JSPROP_fontSize: // example: '14pt'
				*vp=STRING_TO_JSVAL(IntWithUnitsToString(cx, priv->mpStyle->fontSize));
				break;
			case JSPROP_lineHeight: // 'normal' '5pt' '10%'
				if(priv->mpStyle->lineHeight.val==-1){
					*vp=STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx,L"normal"));
				}else{
					*vp=STRING_TO_JSVAL(IntWithUnitsToString(cx, priv->mpStyle->lineHeight));
				}
				break;
			case JSPROP_fontFamily: // example: 'Times New Roman'
				if(priv->mpStyle->fontFamily==NULL) *vp=JSVAL_NULL;
				else{
					JSString* jssFontFamily=JS_NewStringCopyZ(cx,priv->mpStyle->fontFamily);
					*vp=STRING_TO_JSVAL(jssFontFamily);
				}
				break;
			case JSPROP_textAlign: // 'left' 'right' 'center' 'justify'
				GET_EPROP_BEGIN(textAlign,TA_undef)
				GET_EPROP_VALUE(textAlign,TA_left,left)
				GET_EPROP_VALUE(textAlign,TA_right,right)
				GET_EPROP_VALUE(textAlign,TA_center,center)
				GET_EPROP_VALUE(textAlign,TA_justify,justify)
				GET_EPROP_END(unknown text align value)
				break;
			case JSPROP_textIndent: // example: '10px'
				*vp=STRING_TO_JSVAL(IntWithUnitsToString(cx, priv->mpStyle->fontSize));
				break;
			case JSPROP_textDecoration: // 'none' 'underline'
				GET_EPROP_BEGIN(textDecoration,TD_undef)
				GET_EPROP_VALUE(textDecoration,TD_none,none)
				GET_EPROP_VALUE(textDecoration,TD_underline,underline)
				GET_EPROP_END(unknown text decoration value)
				break;
			case JSPROP_color: // 'red' '#F00' '#FF0000' 'rgb 1.0 0.0 0.0'
				// ??
				break;
			case JSPROP_antialiasing: // 'none' 'smooth'
				GET_EPROP_BEGIN(antialiasing,AA_undef)
				GET_EPROP_VALUE(antialiasing,AA_none,none)
				GET_EPROP_VALUE(antialiasing,AA_smooth,smooth)
				GET_EPROP_END(unknown antialiasing value)
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

JSBool ParseIntWithUnits(JSContext* cx, const wchar_t* &jsc, mlStyle::IntWithUnits& IWU){
	if(*jsc == L'\0'){
		IWU.units=mlStyle::U_undef;
		IWU.val=-1;
		return JS_TRUE;
	}
	IWU.val=0;
	while(*jsc!=L'\0' && *jsc>=L'0' && *jsc<=L'9'){
		IWU.val*=10;
		IWU.val+=*jsc-L'0';
		jsc++;
	}
	IWU.units=mlStyle::U_undef;
	if(*jsc == L'\0')
		return JS_TRUE;
	if(*jsc == L'%'){
		IWU.units=mlStyle::U_percent;
		jsc++;
	}else if(*jsc==L'p' && *(jsc+1)==L't'){
		IWU.units=mlStyle::U_pt;
		jsc+=2;
	}else if(*jsc==L'p' && *(jsc+1)==L'x'){
		IWU.units=mlStyle::U_px;
		jsc+=2;
	}else if(!IsDelim(*jsc)){
		JS_ReportError(cx, "Unknown units");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool ParseIntWithUnits(JSContext* cx, jsval *vp,mlStyle::IntWithUnits& aIWU){
	mlStyle::IntWithUnits IWU;
	if(JSVAL_IS_NULL(*vp)){
		IWU.units=mlStyle::U_undef;
		IWU.val=-1;
	}else if(JSVAL_IS_INT(*vp)){
		IWU.units=mlStyle::U_undef;
		IWU.val=JSVAL_TO_INT(*vp);
	}else if(JSVAL_IS_STRING(*vp)){
		JSString* jss = JSVAL_TO_STRING(*vp);
		wchar_t* jsc = wr_JS_GetStringChars(jss);
		if(!ParseIntWithUnits(cx, (const wchar_t *&) jsc, IWU))
			return JS_FALSE;
	}else 
		return JS_FALSE;
	aIWU=IWU;
	return JS_TRUE;
}

JSString* IntWithUnitsToString(JSContext* cx, mlStyle::IntWithUnits& aIWU){
//	wstring str=L"";
//	wsprintf();
	std::wstringstream wss;
	if(aIWU.val!=-1){
		wss << aIWU.val;
		if(aIWU.units==mlStyle::U_pt){
			wss << L"pt";
		}else if(aIWU.units==mlStyle::U_px){
			wss << L"px";
		}else if(aIWU.units==mlStyle::U_percent){
			wss << L"%";
		}
	}
	return wr_JS_NewUCStringCopyZ(cx, wss.str().c_str());
}

__forceinline bool ParseDigit(const jschar jsc, unsigned char &aDig){
	if(jsc >= L'0' && jsc <= L'9'){
		aDig=jsc - L'0';
		return true;
	}
	if(jsc >= L'A' && jsc <= L'F'){
		aDig=jsc - L'A' + 10;
		return true;
	}
	return false;
}

__forceinline bool ParseByte(const jschar* jsc, unsigned char &aByte){
	unsigned char chHi,chLo;
	if(!ParseDigit(*jsc,chHi)) return false;
	if(*(jsc+1)==L'\0') return false;
	if(!ParseDigit(*(jsc+1),chLo)) return false;
	aByte=chHi*16+chLo;
	return true;
}

// 'red' '#F00' '#FF0000' 'rgb 1.0 0.0 0.0'
JSBool ParseColor(JSContext* cx, jsval *vp, mlStyle::Color& aColor){
//	mlStyle::Color Color;
	mlColor color;
	if(JSVAL_IS_STRING(*vp)){
		wchar_t* jscVal=wr_JS_GetStringChars(JSVAL_TO_STRING(*vp));
		if(!ParseColor(cx, (const wchar_t *&) jscVal, color))
			return JS_FALSE;
//		while(*jscVal && *jscVal==L' ') jscVal++; // skip leading spaces
//		if(*jscVal==L'#'){
//			jscVal++;
//			int iLen=wcslen(jscVal);
//			if(iLen < 3) return JS_FALSE;
//			if(iLen == 3){ // '#F00'
//				// ??
//			}else{ // '#FF0000'
//				if(iLen < 6) return JS_FALSE;
//				if(!ParseByte(jscVal,Color.r)) return JS_FALSE;
//				if(!ParseByte(jscVal+2,Color.g)) return JS_FALSE;
//				if(!ParseByte(jscVal+4,Color.b)) return JS_FALSE;
//			}
//		}else if(wcslen(jscVal) > 3){
//			// ??
//		}
	}else
		return JS_FALSE;
	aColor.def = true;
	aColor.r = color.r;
	aColor.g = color.g;
	aColor.b = color.b;
	return JS_TRUE;
}

// реализация mlRMMLElement
mlresult mlRMMLStyles::CreateMedia(omsContext* amcx){
	if(!PMO_IS_NULL(mpMO)) return ML_OK;
	stlStyles::stlStyle* pStyle = mStyles.CreateStyle(htmlTag);
	if(mpStyle != pStyle){
		*pStyle=*mpStyle;
		MP_DELETE( mpStyle);
		mpStyle=pStyle;
	}
	mpMO = PMOV_CREATED;
	return ML_OK;
}

mlresult mlRMMLStyles::Load(){
	// ??
	return ML_OK;
}

mlRMMLStyles::stlStyles::stlStyle& 
mlRMMLStyles::stlStyles::stlStyle::operator=(const stlStyle& aStyle){
	*((mlStyle*)this) = *((mlStyle*)&aStyle);
	if(pwcTag != NULL) MP_DELETE_ARR( pwcTag); pwcTag = NULL;
	if(aStyle.pwcTag != NULL){
		int bufferLength = wcslen(aStyle.pwcTag)+1;
		pwcTag = MP_NEW_ARR( wchar_t, bufferLength);
		//wcscpy(pwcTag, aStyle.pwcTag);
		rmmlsafe_wcscpy(pwcTag, bufferLength, 0, aStyle.pwcTag);
	}else
		pwcTag = aStyle.pwcTag;
	return *this;
}

mlRMMLStyles::stlStyles& mlRMMLStyles::stlStyles::operator=(const stlStyles& aStyles){
	std::vector<stlStyle*>::iterator vi; 
	for(vi=mvStyles.begin(); vi != mvStyles.end(); vi++ ){
//		ML_ASSERTION(*vi!=NULL,"mlRMMLStyles::stlStyles::operator=");
		MP_DELETE( *vi);
	}
	mvStyles.clear();
	stlStyles& Styles = (stlStyles&)aStyles;
	for(vi=Styles.mvStyles.begin(); vi != Styles.mvStyles.end(); vi++ ){
//		ML_ASSERTION(*vi!=NULL,"mlRMMLStyles::stlStyles::operator=");
		stlStyle* pStyle = MP_NEW( stlStyle);
		*pStyle = *(*vi);
		mvStyles.push_back(pStyle);
	}
	return *this;
}

mlRMMLElement* mlRMMLStyles::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLStyles* pNewEL=(mlRMMLStyles*)GET_RMMLEL(mcx, mlRMMLStyles::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	pNewEL->htmlTag = htmlTag;
	*(pNewEL->mpStyle) = *mpStyle;
	return pNewEL;
}

bool mlRMMLStyles::ParseFontStyleValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle){
	if(isEqual(apwcValue, L"normal", aiLen)){
		aStyle.fontStyle = mlStyle::FS_normal;
	}else if(isEqual(apwcValue, L"italic", aiLen)){
		aStyle.fontStyle = mlStyle::FS_italic;
	}else
		return false;
	return true;
}

bool mlRMMLStyles::ParseFontWeightValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle){
	if(isEqual(apwcValue, L"normal", aiLen)){
		aStyle.fontWeight = mlStyle::FW_normal;
	}else if(isEqual(apwcValue, L"italic", aiLen)){
		aStyle.fontWeight = mlStyle::FW_bold;
	}else
		return false;
	return true;
}

bool mlRMMLStyles::ParseTextDecorationValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle){
	if(isEqual(apwcValue, L"none", aiLen)){
		aStyle.textDecoration = mlStyle::TD_none;
	}else if(isEqual(apwcValue, L"underline", aiLen)){
		aStyle.textDecoration = mlStyle::TD_underline;
	}else
		return false;
	return true;
}

bool mlRMMLStyles::ParseTextAlignValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle){
	if(isEqual(apwcValue, L"left", aiLen)){
		aStyle.textAlign = mlStyle::TA_left;
	}else if(isEqual(apwcValue, L"right", aiLen)){
		aStyle.textAlign = mlStyle::TA_right;
	}else if(isEqual(apwcValue, L"center", aiLen)){
		aStyle.textAlign = mlStyle::TA_center;
	}else if(isEqual(apwcValue, L"justify", aiLen)){
		aStyle.textAlign = mlStyle::TA_justify;
	}else
		return false;
	return true;
}

bool mlRMMLStyles::ParseAntialiasingValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle){
	if(isEqual(apwcValue, L"none", aiLen)){
		aStyle.antialiasing = mlStyle::AA_none;
	}else if(isEqual(apwcValue, L"smooth", aiLen)){
		aStyle.antialiasing = mlStyle::AA_smooth;
	}else
		return false;
	return true;
}

mlresult mlRMMLStyles::SetValue(wchar_t* apwcValue){
	// парсим значение
	stlStyles::stlStyle* pStyle = NULL;
	if(PMO_IS_NULL(mpMO))
		pStyle = mpStyle;
	else{
		pStyle = mStyles.CreateStyle(htmlTag); 
	}
	ML_ASSERTION(mcx, pStyle != NULL, "mlRMMLStyles::SetValue");
	const wchar_t* pwc = apwcValue;
	if(pwc == NULL || *pwc == L'\0')
		return ML_ERROR_INVALID_ARG;
	for(;;){
		if(*pwc == L'\0') break;
		pwc = SkipDelims(pwc);
		if(*pwc == L'\0') break;
		const wchar_t* pwcParamName = pwc;
		pwc = GetNextDelimPos(pwc);
		int iParamNameLen = pwc - pwcParamName;
		if(iParamNameLen == 0) break;
		if(*pwc == L'\0') break;
		pwc = SkipSpaces(pwc);
		if(*pwc == L'\0') break;
		if(*pwc != L':' && *pwc != L';'){
			JS_ReportError(mcx, "Colon or semicolon expected");
			return OMS_ERROR_INVALID_ARG;
		}
		if(*pwc == L';')
			continue;
		pwc++; // skip ':'
		pwc = SkipSpaces(pwc);
		if(*pwc == L';'){
			pwc++; // skip ';'
			continue;
		}
		if(IsDelim(*pwc)){
			JS_ReportError(mcx, "After colon parameter value expected");
			return OMS_ERROR_INVALID_ARG;
		}
		if(isEqual(pwcParamName, L"font", iParamNameLen)){
			// ??
		}else if(isEqual(pwcParamName, L"fontStyle", iParamNameLen)){
			const wchar_t* pwcStyle = pwc;
			pwc = GetNextDelimPos(pwc);
			int iLen = pwc - pwcStyle;
			ML_ASSERTION(mcx, iLen != 0, "mlRMMLStyles::SetValue");
			if(!ParseFontStyleValue(pwcStyle, iLen, *pStyle)){
				JS_ReportError(mcx, "Invalid fontStyle value");
				return OMS_ERROR_INVALID_ARG;
			}
		}else if(isEqual(pwcParamName, L"fontWeight", iParamNameLen)){
			const wchar_t* pwcWeight = pwc;
			pwc = GetNextDelimPos(pwc);
			int iLen = pwc - pwcWeight;
			ML_ASSERTION(mcx, iLen != 0, "mlRMMLStyles::SetValue");
			if(!ParseFontWeightValue(pwcWeight, iLen, *pStyle)){
				JS_ReportError(mcx, "Invalid fontWeight value");
				return OMS_ERROR_INVALID_ARG;
			}
		}else if(isEqual(pwcParamName, L"fontSize", iParamNameLen)){
			mlStyle::IntWithUnits IWU;
			if(!ParseIntWithUnits(mcx, pwc, IWU))
				return OMS_ERROR_INVALID_ARG;
			pStyle->fontSize = IWU;
		}else if(isEqual(pwcParamName, L"lineHeight", iParamNameLen)){
			const wchar_t* pwcHeight = pwc;
			pwc = GetNextDelimPos(pwc);
			int iLen = pwc - pwcHeight;
			ML_ASSERTION(mcx, iLen != 0, "mlRMMLStyles::SetValue");
			if(isEqual(pwcHeight, L"normal", iLen)){
				pStyle->lineHeight.val = -1;
			}else{
				pwc = pwcHeight;
				mlStyle::IntWithUnits IWU;
				if(!ParseIntWithUnits(mcx, pwc, IWU))
					return OMS_ERROR_INVALID_ARG;
				pStyle->lineHeight = IWU;
			}
		}else if(isEqual(pwcParamName, L"fontFamily", iParamNameLen)){
			const wchar_t* pwcFontFamily = pwc;
			pwc = GetNextNotSpaceDelimPos(pwc);
			const wchar_t* pwcEnd = pwc;
			if(IsSpace(*(pwcEnd-1))){
				pwcEnd--;
				while (IsSpace(*pwcEnd)) pwcEnd--;
				pwcEnd++;
			}
			int iLen = pwcEnd - pwcFontFamily;
			ML_RELEASE(pStyle->fontFamily)
			char* psz = MP_NEW_ARR( char, iLen+1); psz[iLen]='\0';
			pStyle->fontFamily=psz;
			for(int ii=0; ii<iLen; ii++){
				jschar wch=*pwcFontFamily++;
				char ch=(char)wch;
				*psz++=ch;
			}
		}else if(isEqual(pwcParamName, L"textAlign", iParamNameLen)){
			const wchar_t* pwcAlign = pwc;
			pwc = GetNextDelimPos(pwc);
			int iLen = pwc - pwcAlign;
			ML_ASSERTION(mcx, iLen != 0, "mlRMMLStyles::SetValue");
			if(!ParseTextAlignValue(pwcAlign, iLen, *pStyle)){
				JS_ReportError(mcx, "Invalid textAlign value");
				return OMS_ERROR_INVALID_ARG;
			}
		}else if(isEqual(pwcParamName, L"textIndent", iParamNameLen)){
			mlStyle::IntWithUnits IWU;
			if(!ParseIntWithUnits(mcx, pwc, IWU))
				return OMS_ERROR_INVALID_ARG;
			pStyle->textIndent = IWU;
		}else if(isEqual(pwcParamName, L"textDecoration", iParamNameLen)){
			const wchar_t* pwcDecoration = pwc;
			pwc = GetNextDelimPos(pwc);
			int iLen = pwc - pwcDecoration;
			ML_ASSERTION(mcx, iLen != 0, "mlRMMLStyles::SetValue");
			if(!ParseTextDecorationValue(pwcDecoration, iLen, *pStyle)){
				JS_ReportError(mcx, "Invalid textDecoration value");
				return OMS_ERROR_INVALID_ARG;
			}
		}else if(isEqual(pwcParamName, L"color", iParamNameLen)){
			mlColor color;
			if(!ParseColor(mcx, pwc, color)){
				JS_ReportError(mcx, "Invalid color value");
				return OMS_ERROR_INVALID_ARG;
			}
			pStyle->color.def = true;
			pStyle->color.r = color.r;
			pStyle->color.g = color.g;
			pStyle->color.b = color.b;
		}else if(isEqual(pwcParamName, L"antialiasing", iParamNameLen)){
			const wchar_t* pwcDecoration = pwc;
			pwc = GetNextDelimPos(pwc);
			int iLen = pwc - pwcDecoration;
			ML_ASSERTION(mcx, iLen != 0, "mlRMMLStyles::SetValue");
			if(!ParseAntialiasingValue(pwcDecoration, iLen, *pStyle)){
				JS_ReportError(mcx, "Invalid antialiasing value");
				return OMS_ERROR_INVALID_ARG;
			}
		}
	}
	return ML_OK;
}

//	JSPROP_RW(font) // example: 'italic small-caps bold 12pt serif'
//!	JSPROP_RW(fontStyle) // 'normal' 'italic'
//!	JSPROP_RW(fontWeight) // 'normal' 'bold'
//!	JSPROP_RW(fontSize) // example: '14pt'
//!	JSPROP_RW(lineHeight) // 'normal' '5pt' '10%'
//	JSPROP_RW(fontFamily) // example: 'Times New Roman'
//!	JSPROP_RW(textAlign) // 'left' 'right' 'center' 'justify'
//!	JSPROP_RW(textIndent) // example: '10px'
//!	JSPROP_RW(textDecoration) // 'none' 'underline'
//	JSPROP_RW(color) // 'red' '#F00' '#FF0000' 'rgb 1.0 0.0 0.0'

//			JS_ReportError(mcx, "Style parameter expected");
//			return OMS_ERROR_INVALID_ARG;

//
mlStyle* mlRMMLStyles::GetStyle(const char* apszTag){
	if(PMO_IS_NULL(mpMO))
		CreateMedia(NULL);
	if(apszTag==NULL){
		return mStyles.GetStyle();
	}else{
		return mStyles.GetStyle(cLPWCSTRq(apszTag));
	}
}

}
