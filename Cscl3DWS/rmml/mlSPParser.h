
#ifndef _mlSPParser_h__
#define _mlSPParser_h__

namespace rmml{

struct mlPropParseInfo{
	const wchar_t* name;
	unsigned char type;
	void* ptr;
};

__forceinline bool ParseHexDigit(const wchar_t* &apwc, unsigned char &aucDig){
	if(*apwc>=L'0' && *apwc<=L'9'){ aucDig=*apwc-L'0'; apwc++; return true; }
	if(*apwc>=L'A' && *apwc<=L'F'){ aucDig=(*apwc-L'A')+10; apwc++; return true; }
	if(*apwc>=L'a' && *apwc<=L'f'){ aucDig=(*apwc-L'a')+10; apwc++; return true; }
	return false;
}

__forceinline bool ParseHexByte(const wchar_t* &apwc, unsigned char &aucByte){
	unsigned char ucHi;
	if(!ParseHexDigit(apwc,ucHi)) return false;
	if(!ParseHexDigit(apwc,aucByte)) return false;
	aucByte+=(ucHi<<4);
	return true;
}

__forceinline bool IsSpace(wchar_t awc){
	switch(awc){
		case L'\0':
		case L' ':
		case L'\t':
		case L'\r':
		case L'\n':
			return true;
	}
	return false;
}

__forceinline bool IsDelim(wchar_t awc){
	switch(awc){
		case L'\0':
		case L' ':
		case L'\t':
		case L'\r':
		case L'\n':
		case L',':
		case L';':
		case L':':
		case L'=':
			return true;
	}
	return false;
}

__forceinline bool IsDelimBr(wchar_t awc){
	switch(awc){
		case L'\0':
		case L' ':
		case L'\t':
		case L'\r':
		case L'\n':
		case L',':
		case L';':
		case L':':
		case L'=':
		case L'(':
		case L')':
			return true;
	}
	return false;
}

__forceinline bool IsNotSpaceDelim(wchar_t awc){
	switch(awc){
		case L'\0':
		case L',':
		case L';':
		case L':':
		case L'=':
			return true;
	}
	return false;
}
__forceinline const wchar_t* GetNextDelimPos(const wchar_t* apwc){
	while(!IsDelim(*apwc)) apwc++;
	return apwc;
}

__forceinline const wchar_t* GetNextDelimBrPos(const wchar_t* apwc){
	while(!IsDelimBr(*apwc)) apwc++;
	return apwc;
}

__forceinline const wchar_t* GetNextNotSpaceDelimPos(const wchar_t* apwc){
	while(!IsNotSpaceDelim(*apwc)) apwc++;
	return apwc;
}

__forceinline const wchar_t* SkipSpaces(const wchar_t* apwc){
	while(*apwc!=L'\0' && (*apwc==L' ' || *apwc==L'\t' || *apwc==L'\n' || *apwc==L'\r')) apwc++;
	return apwc;
}

__forceinline const wchar_t* SkipDelims(const wchar_t* apwc){
	while(IsDelim(*apwc)) apwc++;
	return apwc;
}

__forceinline int GetPropName(const wchar_t* apwc){
	const wchar_t* pwc=GetNextDelimPos(apwc);
	const wchar_t* pwc2=SkipSpaces(pwc);
	if(*pwc2==L'='){
		return pwc-apwc;
	}
	return 0;
}

__forceinline const wchar_t* SkipPropName(const wchar_t* apwc, int aiLen){
	if(aiLen==0) return apwc;
	apwc+=aiLen;
	return SkipDelims(apwc);
}

__forceinline bool ParseInt(const wchar_t* &apwc, int* apInt){
	apwc=SkipSpaces(apwc);
	bool bMinus=false;
	if(*apwc==L'-'){
		bMinus=true; apwc++;
	}
	if(*apwc>=L'0' && *apwc<=L'9'){
		*apInt=*apwc-L'0';
		apwc++;
	}else return false;
	while(*apwc>=L'0' && *apwc<=L'9'){
		*apInt=*apInt*10+(*apwc-L'0');
		apwc++;
	}
	if(bMinus) *apInt=-(*apInt);
	return true;
}

bool ParseStruct(JSContext* cx, const wchar_t* &apwc, mlPropParseInfo* apPInfo);

bool ParseColor(JSContext* cx, const wchar_t* &apwc, mlColor &aColor);

bool SetColorByJSVal(JSContext* cx, mlColor& aColor, jsval aJSVal);

}

#endif
