#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlSPParser.h"
#include "config/prolog.h"

namespace rmml{

bool ParseStruct(JSContext* cx, const wchar_t* &apwc, mlPropParseInfo* apPInfo){
	apwc=SkipSpaces(apwc);
	if(*apwc!=L'('){
		JS_ReportError(cx,"Structure initialization string must begin from '('");
		return false;
	}
	apwc=SkipSpaces(++apwc);
	for(int ii=0; ; ii++){
		mlPropParseInfo* pPI=&apPInfo[ii];
		if(*(pPI->name)==L'\0') break;
		int iPNLen=GetPropName(apwc);
		if(iPNLen>0){
			if(!isEqual(apwc,pPI->name,iPNLen)){
				for(int jj=0; ; jj++){
					mlPropParseInfo* pPI2=&apPInfo[jj];
					if(*(pPI->name)==L'\0'){
						wchar_t wc=apwc[iPNLen]; ((wchar_t*)apwc)[iPNLen]=0;
						JS_ReportError(cx,"The structure has no property with name '%s'",cLPCSTR(apwc));
						((wchar_t*)apwc)[iPNLen]=wc;
						return false;
					}
					if(isEqual(apwc,pPI2->name,iPNLen)){
						pPI=pPI2;
						break;
					}
				}
			}
			apwc=SkipPropName(apwc,iPNLen);
		}
		switch(pPI->type){
		case MLPT_DOUBLE:{
			if(swscanf_s(apwc,L"%lf",pPI->ptr)==0){
				JS_ReportError(cx,"Cannot read double property '%s'",cLPCSTRq(pPI->name));
				return false;
			}
			apwc=GetNextDelimBrPos(apwc);
			apwc=SkipDelims(apwc);
			}break;
		case MLPT_INT:{
			if(!ParseInt(apwc,(int*)pPI->ptr)){
				JS_ReportError(cx,"Cannot read integer property '%s'",cLPCSTRq(pPI->name));
				return false;
			}
			apwc=GetNextDelimBrPos(apwc);
			apwc=SkipDelims(apwc);
			}break;
		// ??
		default:
			break;
		}
		if(*apwc==L'\0' || *apwc==L')') break;
	}
	if(*apwc!=L')'){
		JS_ReportError(cx,"Structure initialization string must be closed by ')'");
		return false;
	}
	apwc++;
	return true;
}

mlPropParseInfo mlColorParseInfo[]={
	{L"r",MLPT_DOUBLE,NULL},
	{L"g",MLPT_DOUBLE,NULL},
	{L"b",MLPT_DOUBLE,NULL},
	{L"a",MLPT_DOUBLE,NULL},
	{L"",MLPT_UNKNOWN,NULL}
};

mlPropParseInfo mlColorParseInfo2[]={
	{L"r",MLPT_INT,NULL},
	{L"g",MLPT_INT,NULL},
	{L"b",MLPT_INT,NULL},
	{L"a",MLPT_INT,NULL},
	{L"",MLPT_UNKNOWN,NULL}
};

struct mlColorConst{
	const wchar_t* name;
	mlColor color;
};

mlColorConst ColorConsts[]={
	{ L"aliceblue" , {0xF0, 0xF8, 0xFF, 0xFF} },
	{ L"antiquewhite", {0xFA, 0xEB, 0xD7, 0xFF} },
	{ L"aqua", {0x00, 0xFF, 0xFF, 0xFF} },
	{ L"aquamarine", {0x7F, 0xFF, 0xD4, 0xFF} },
	{ L"azure", {0xF0, 0xFF, 0xFF, 0xFF} },
	{ L"beige", {0xF5, 0xF5, 0xDC, 0xFF} },
	{ L"bisque", {0xFF, 0xE4, 0xC4, 0xFF} },
	{ L"black", {0x00, 0x00, 0x00, 0xFF} },
	{ L"blanchedalmond", {0xFF, 0xEB, 0xCD, 0xFF} },
	{ L"blue", {0x00, 0x00, 0xFF, 0xFF} },
	{ L"blueviolet", {0x8A, 0x2B, 0xE2, 0xFF} },
	{ L"brown", {0xA5, 0x2A, 0x2A, 0xFF} },
	{ L"burlywood", {0xDE, 0xB8, 0x87, 0xFF} },
	{ L"cadetblue", {0x5F, 0x9E, 0xA0, 0xFF} },
	{ L"chartreuse", {0x7F, 0xFF, 0x00, 0xFF} },
	{ L"chocolate", {0xD2, 0x69, 0x1E, 0xFF} },
	{ L"coral", {0xFF, 0x7F, 0x50, 0xFF} },
	{ L"cornflowerblue", {0x64, 0x95, 0xED, 0xFF} },
	{ L"cornsilk", {0xFF, 0xF8, 0xDC, 0xFF} },
	{ L"crimson", {0xDC, 0x14, 0x3C, 0xFF} },
	{ L"cyan", {0x00, 0xFF, 0xFF, 0xFF} },
	{ L"darkblue", {0x00, 0x00, 0x8B, 0xFF} },
	{ L"darkcyan", {0x00, 0x8B, 0x8B, 0xFF} },
	{ L"darkgoldenrod", {0xB8, 0x86, 0x0B, 0xFF} },
	{ L"darkgray", {0xA9, 0xA9, 0xA9, 0xFF} },
	{ L"darkgreen", {0x00, 0x64, 0x00, 0xFF} },
	{ L"darkkhaki", {0xBD, 0xB7, 0x6B, 0xFF} },
	{ L"darkmagenta", {0x8B, 0x00, 0x8B, 0xFF} },
	{ L"darkolivegreen", {0x55, 0x6B, 0x2F, 0xFF} },
	{ L"darkorange", {0xFF, 0x8C, 0x00, 0xFF} },
	{ L"darkorchid", {0x99, 0x32, 0xCC, 0xFF} },
	{ L"darkred", {0x8B, 0x00, 0x00, 0xFF} },
	{ L"darksalmon", {0xE9, 0x96, 0x7A, 0xFF} },
	{ L"darkseagreen", {0x8F, 0xBC, 0x8B, 0xFF} },
	{ L"darkslateblue", {0x48, 0x3D, 0x8B, 0xFF} },
	{ L"darkslategray", {0x2F, 0x4F, 0x4F, 0xFF} },
	{ L"darkturquoise", {0x00, 0xCE, 0xD1, 0xFF} },
	{ L"darkviolet", {0x94, 0x00, 0xD3, 0xFF} },
	{ L"deeppink", {0xFF, 0x14, 0x93, 0xFF} },
	{ L"deepskyblue", {0x00, 0xBF, 0xFF, 0xFF} },
	{ L"dimgray", {0x69, 0x69, 0x69, 0xFF} },
	{ L"dodgerblue", {0x1E, 0x90, 0xFF, 0xFF} },
	{ L"firebrick", {0xB2, 0x22, 0x22, 0xFF} },
	{ L"floralwhite", {0xFF, 0xFA, 0xF0, 0xFF} },
	{ L"forestgreen", {0x22, 0x8B, 0x22, 0xFF} },
	{ L"fuchsia", {0xFF, 0x00, 0xFF, 0xFF} },
	{ L"gainsboro", {0xDC, 0xDC, 0xDC, 0xFF} },
	{ L"ghostwhite", {0xF8, 0xF8, 0xFF, 0xFF} },
	{ L"gold", {0xFF, 0xD7, 0x00, 0xFF} },
	{ L"goldenrod", {0xDA, 0xA5, 0x20, 0xFF} },
	{ L"gray", {0x80, 0x80, 0x80, 0xFF} },
	{ L"green", {0x00, 0x80, 0x00, 0xFF} },
	{ L"greenyellow", {0xAD, 0xFF, 0x2F, 0xFF} },
	{ L"honeydew", {0xF0, 0xFF, 0xF0, 0xFF} },
	{ L"hotpink", {0xFF, 0x69, 0xB4, 0xFF} },
	{ L"indianred", {0xCD, 0x5C, 0x5C, 0xFF} },
	{ L"indigo", {0x4B, 0x00, 0x82, 0xFF} },
	{ L"ivory", {0xFF, 0xFF, 0xF0, 0xFF} },
	{ L"khaki", {0xF0, 0xE6, 0x8C, 0xFF} },
	{ L"lavender", {0xE6, 0xE6, 0xFA, 0xFF} },
	{ L"lavenderblush", {0xFF, 0xF0, 0xF5, 0xFF} },
	{ L"lawngreen", {0x7C, 0xFC, 0x00, 0xFF} },
	{ L"lemonchiffon", {0xFF, 0xFA, 0xCD, 0xFF} },
	{ L"lightblue", {0xAD, 0xD8, 0xE6, 0xFF} },
	{ L"lightcoral", {0xF0, 0x80, 0x80, 0xFF} },
	{ L"lightcyan", {0xE0, 0xFF, 0xFF, 0xFF} },
	{ L"lightgoldenrodyellow", {0xFA, 0xFA, 0xD2, 0xFF} },
	{ L"lightgreen", {0x90, 0xEE, 0x90, 0xFF} },
	{ L"lightgrey", {0xD3, 0xD3, 0xD3, 0xFF} },
	{ L"lightpink", {0xFF, 0xB6, 0xC1, 0xFF} },
	{ L"lightsalmon", {0xFF, 0xA0, 0x7A, 0xFF} },
	{ L"lightseagreen", {0x20, 0xB2, 0xAA, 0xFF} },
	{ L"lightskyblue", {0x87, 0xCE, 0xFA, 0xFF} },
	{ L"lightslategray", {0x77, 0x88, 0x99, 0xFF} },
	{ L"lightsteelblue", {0xB0, 0xC4, 0xDE, 0xFF} },
	{ L"lightyellow", {0xFF, 0xFF, 0xE0, 0xFF} },
	{ L"lime", {0x00, 0xFF, 0x00, 0xFF} },
	{ L"limegreen", {0x32, 0xCD, 0x32, 0xFF} },
	{ L"linen", {0xFA, 0xF0, 0xE6, 0xFF} },
	{ L"magenta", {0xFF, 0x00, 0xFF, 0xFF} },
	{ L"maroon", {0x80, 0x00, 0x00, 0xFF} },
	{ L"mediumaquamarine", {0x66, 0xCD, 0xAA, 0xFF} },
	{ L"mediumblue", {0x00, 0x00, 0xCD, 0xFF} },
	{ L"mediumorchid", {0xBA, 0x55, 0xD3, 0xFF} },
	{ L"mediumpurple", {0x93, 0x70, 0xDB, 0xFF} },
	{ L"mediumseagreen", {0x3C, 0xB3, 0x71, 0xFF} },
	{ L"mediumslateblue", {0x7B, 0x68, 0xEE, 0xFF} },
	{ L"mediumspringgreen", {0x00, 0xFA, 0x9A, 0xFF} },
	{ L"mediumturquoise", {0x48, 0xD1, 0xCC, 0xFF} },
	{ L"mediumvioletred", {0xC7, 0x15, 0x85, 0xFF} },
	{ L"midnightblue", {0x19, 0x19, 0x70, 0xFF} },
	{ L"mintcream", {0xF5, 0xFF, 0xFA, 0xFF} },
	{ L"mistyrose", {0xFF, 0xE4, 0xE1, 0xFF} },
	{ L"moccasin", {0xFF, 0xE4, 0xB5, 0xFF} },
	{ L"navajowhite", {0xFF, 0xDE, 0xAD, 0xFF} },
	{ L"navy", {0x00, 0x00, 0x80, 0xFF} },
	{ L"oldlace", {0xFD, 0xF5, 0xE6, 0xFF} },
	{ L"olive", {0x80, 0x80, 0x00, 0xFF} },
	{ L"olivedrab", {0x6B, 0x8E, 0x23, 0xFF} },
	{ L"orange", {0xFF, 0xA5, 0x00, 0xFF} },
	{ L"orangered", {0xFF, 0x45, 0x00, 0xFF} },
	{ L"orchid", {0xDA, 0x70, 0xD6, 0xFF} },
	{ L"palegoldenrod", {0xEE, 0xE8, 0xAA, 0xFF} },
	{ L"palegreen", {0x98, 0xFB, 0x98, 0xFF} },
	{ L"paleturquoise", {0xAF, 0xEE, 0xEE, 0xFF} },
	{ L"palevioletred", {0xDB, 0x70, 0x93, 0xFF} },
	{ L"papayawhip", {0xFF, 0xEF, 0xD5, 0xFF} },
	{ L"peachpuff", {0xFF, 0xDA, 0xB9, 0xFF} },
	{ L"peru", {0xCD, 0x85, 0x3F, 0xFF} },
	{ L"pink", {0xFF, 0xC0, 0xCB, 0xFF} },
	{ L"plum", {0xDD, 0xA0, 0xDD, 0xFF} },
	{ L"powderblue", {0xB0, 0xE0, 0xE6, 0xFF} },
	{ L"purple", {0x80, 0x00, 0x80, 0xFF} },
	{ L"red", {0xFF, 0x00, 0x00, 0xFF} },
	{ L"rosybrown", {0xBC, 0x8F, 0x8F, 0xFF} },
	{ L"royalblue", {0x41, 0x69, 0xE1, 0xFF} },
	{ L"saddlebrown", {0x8B, 0x45, 0x13, 0xFF} },
	{ L"salmon", {0xFA, 0x80, 0x72, 0xFF} },
	{ L"sandybrown", {0xF4, 0xA4, 0x60, 0xFF} },
	{ L"seagreen", {0x2E, 0x8B, 0x57, 0xFF} },
	{ L"seashell", {0xFF, 0xF5, 0xEE, 0xFF} },
	{ L"sienna", {0xA0, 0x52, 0x2D, 0xFF} },
	{ L"silver", {0xC0, 0xC0, 0xC0, 0xFF} },
	{ L"skyblue", {0x87, 0xCE, 0xEB, 0xFF} },
	{ L"slateblue", {0x6A, 0x5A, 0xCD, 0xFF} },
	{ L"slategray", {0x70, 0x80, 0x90, 0xFF} },
	{ L"snow", {0xFF, 0xFA, 0xFA, 0xFF} },
	{ L"springgreen", {0x00, 0xFF, 0x7F, 0xFF} },
	{ L"steelblue", {0x46, 0x82, 0xB4, 0xFF} },
	{ L"tan", {0xD2, 0xB4, 0x8C, 0xFF} },
	{ L"teal", {0x00, 0x80, 0x80, 0xFF} },
	{ L"thistle", {0xD8, 0xBF, 0xD8, 0xFF} },
	{ L"tomato", {0xFF, 0x63, 0x47, 0xFF} },
	{ L"turquoise", {0x40, 0xE0, 0xD0, 0xFF} },
	{ L"violet", {0xEE, 0x82, 0xEE, 0xFF} },
	{ L"wheat", {0xF5, 0xDE, 0xB3, 0xFF} },
	{ L"white", {0xFF, 0xFF, 0xFF, 0xFF} },
	{ L"whitesmoke", {0xF5, 0xF5, 0xF5, 0xFF} },
	{ L"yellow", {0xFF, 0xFF, 0x00, 0xFF} },
	{ L"yellowgreen", {0x9A, 0xCD, 0x32, 0xFF} },
//	{ NULL , {0, 0, 0, 0} },
};

__forceinline bool ccs_less(const wchar_t* apwc1, const wchar_t* apwc2){
	
}

__forceinline int WSCompare(const wchar_t* apch1, const wchar_t* apch2, unsigned int auLen){
	for(; auLen>0; auLen--){
		if(*apch1 != *apch2){
			if(*apch1 < *apch2)
				return -1;
			else
				return 1;
		}
		apch1++, apch2++;
	}
	return 0;
}

bool ParseColor(JSContext* cx, const wchar_t* &apwc, mlColor &aColor){
	apwc=SkipSpaces(apwc);
	if(*apwc==L'#'){
		// #0F0 or #F0F0 (argb) or #00FF00 or #8000FF80 (argb)
		apwc++;
		const wchar_t* pwcDelim=GetNextDelimPos(apwc);
		int iLen=pwcDelim-apwc;
		bool bErr=true;
		if(iLen==3){ // #0F0
			for(;;){
				if(!ParseHexDigit(apwc,aColor.r)) break;
				if(!ParseHexDigit(apwc,aColor.g)) break;
				if(!ParseHexDigit(apwc,aColor.b)) break;
				bErr=false;	break;
			}
			aColor.r<<=4; aColor.g<<=4; aColor.b<<=4; aColor.a=0xFF;
		}else if(iLen==4){ // #F0F0 (argb)
			for(;;){
				if(!ParseHexDigit(apwc,aColor.a)) break;
				if(!ParseHexDigit(apwc,aColor.r)) break;
				if(!ParseHexDigit(apwc,aColor.g)) break;
				if(!ParseHexDigit(apwc,aColor.b)) break;
				bErr=false;	break;
			}
			aColor.r<<=4; aColor.g<<=4; aColor.b<<=4; aColor.a<<=4;
		}else if(iLen==6){ // #00FF00
			for(;;){
				if(!ParseHexByte(apwc,aColor.r)) break;
				if(!ParseHexByte(apwc,aColor.g)) break;
				if(!ParseHexByte(apwc,aColor.b)) break;
				bErr=false;	break;
			}
			aColor.a=0xFF;
		}else if(iLen==8){ // #8000FF80 (argb)
			for(;;){
				if(!ParseHexByte(apwc,aColor.a)) break;
				if(!ParseHexByte(apwc,aColor.r)) break;
				if(!ParseHexByte(apwc,aColor.g)) break;
				if(!ParseHexByte(apwc,aColor.b)) break;
				bErr=false;	break;
			}
		}
		if(bErr){
			JS_ReportError(cx,"Invalid color constant");
			return false;
		}
	}else if(*apwc==L'(' 
			|| (apwc[0]==L'r' && apwc[1]==L'g' && apwc[2]==L'b') 
			|| (apwc[0]==L'R' && apwc[1]==L'G' && apwc[2]==L'B')){
		// '(1 0 0 0)' or 'rgb 0.5 0 0' or 'rgba 1 1 1 0.5' or (0 128 128) ...
		bool bRGB=false;
		bool bRGBA=false;
		if(*apwc!=L'('){
			bRGB=true;
			apwc+=3;
			if(*apwc==L'a' || *apwc==L'A')
				bRGBA=true;
		}
		const wchar_t* pwc=apwc;
		bool bThereIsDots=false;
		if(bRGB){
			while(*pwc!=L'\0'){
				if(*pwc==L'.'){	bThereIsDots=true; break; }
				if(*pwc!=L' ' && *pwc!=L',' && (*pwc<L'0' || *pwc>L'9')) break;
				pwc++;
			}
		}else{
			while(*pwc!=L'\0' && *pwc!=L')'){
				if(*pwc==L'.') bThereIsDots=true;
				pwc++;
			}
		}
		if(!bRGB && *pwc!=L')'){
			JS_ReportError(cx,"Structure initialization string must be closed by ')'");
			return false;
		}
		if(bThereIsDots){
			double r=0,g=0,b=0,a=-1;
			mlColorParseInfo[0].ptr=&r;
			mlColorParseInfo[1].ptr=&g;
			mlColorParseInfo[2].ptr=&b;
			mlColorParseInfo[3].ptr=&a;
			if(!ParseStruct(cx, apwc, mlColorParseInfo)) 
				return false;
			if(r>1 || g>1 || b>1 || a>1){
				aColor.r=(unsigned char)r;
				aColor.g=(unsigned char)g;
				aColor.b=(unsigned char)b;
				if(a==-1) aColor.a=0xFF;
				else aColor.a=(unsigned char)a;
			}else{
				aColor.r=(unsigned char)(r*255);
				aColor.g=(unsigned char)(g*255);
				aColor.b=(unsigned char)(b*255);
				if(a==-1) aColor.a=0xFF;
				else aColor.a=(unsigned char)(a*255);
			}
		}else{
			int r=0,g=0,b=0,a=-1;
			mlColorParseInfo2[0].ptr=&r;
			mlColorParseInfo2[1].ptr=&g;
			mlColorParseInfo2[2].ptr=&b;
			mlColorParseInfo2[3].ptr=&a;
			if(!ParseStruct(cx, apwc, mlColorParseInfo2)) 
				return false;
			if(r<=1 && g<=1 && b<=1 && a<=1){
				aColor.r=(r==0)?0:0xFF;
				aColor.g=(g==0)?0:0xFF;
				aColor.b=(b==0)?0:0xFF;
				aColor.a=(a==0)?0:0xFF;
			}else{
				aColor.r=(r>=255)?0xFF:r;
				aColor.g=(g>=255)?0xFF:g;
				aColor.b=(b>=255)?0xFF:b;
				aColor.a=(a>=255||a<0)?0xFF:a;
			}
		}
	}else{
		// ищем по названию цвета
//		aColor = ColorConsts[0].color;
		const wchar_t* pwcDelim=GetNextDelimPos(apwc);
		int iLen=pwcDelim-apwc;
		int iNumOfConsts = sizeof(ColorConsts)/sizeof(mlColorConst);
		int iFound = -1;
		int iBegin = 0;
		int iEnd = iNumOfConsts-1;
		while(iBegin != iEnd){
			int iMiddle = (iEnd + iBegin) / 2;
			if(iMiddle == iBegin){
				if(isEqual(ColorConsts[iMiddle].name, apwc, iLen)){
					iFound = iMiddle;
					break;
				}
			}
			int result = WSCompare(ColorConsts[iMiddle].name, apwc, iLen);
			if(result == 0){
				iFound = iMiddle;
				break;
			}
			if(result > 0){
				iEnd = iMiddle;
			}else{
				iBegin = iMiddle;
			}
		}
		apwc += iLen;
		if(iFound == -1){
			JS_ReportError(cx,"Invalid color constant");
			return false;
		}else{
			aColor = ColorConsts[iFound].color;
		}
	}
	return true;
}

bool SetColorByJSVal(JSContext* cx, mlColor& aColor, jsval aJSVal){
	if(JSVAL_IS_STRING(aJSVal)){
		JSString* jssColor = JSVAL_TO_STRING(aJSVal);
		wchar_t* pwc=wr_JS_GetStringChars(jssColor);
		ParseColor(cx, (const wchar_t *&) pwc, aColor);
		return true;
	}else if(JSVAL_IS_INT(aJSVal)){
		int iColor = JSVAL_TO_INT(aJSVal);
		aColor.r = iColor & 0xFF;
		aColor.g = (iColor >> 8) & 0xFF;
		aColor.b = (iColor >> 16) & 0xFF;
		return true;
	}else if(JSVAL_IS_REAL_OBJECT(aJSVal)){
		JSObject* jsoColor = JSVAL_TO_OBJECT(aJSVal);
		if(!mlColorJSO::IsInstance(cx, jsoColor))
			return false;
		mlColorJSO* pColor = (mlColorJSO*)JS_GetPrivate(cx, jsoColor);
		aColor = pColor->GetColor();
		return true;
	}
	return false;
}

}