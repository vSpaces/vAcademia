//////////////////////////////////////////////////////////////////////

#pragma once

char* WC2MB(const wchar_t* apwc);
wchar_t* MB2WC(const char* apch);

class cLPCSTR{
	char* mpchStr;
public:
	cLPCSTR(const wchar_t* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MB(apwcStr);
	}
	cLPCSTR(wchar_t* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MB(apwcStr);
	}
	operator char*(){ return mpchStr; }
	~cLPCSTR(){
		if(mpchStr != NULL) 
			MP_DELETE_ARR( mpchStr); 
	};
};

class cLPWCSTR{
	wchar_t* mpwcStr;
public:
	cLPWCSTR(const char* apszStr){
		if(apszStr==NULL){ mpwcStr=NULL; return; }
		mpwcStr=MB2WC(apszStr);
	}
	cLPWCSTR(char* apszStr){
		if(apszStr==NULL){ mpwcStr=NULL; return; }
		mpwcStr=MB2WC(apszStr);
	}
	operator wchar_t*(){ return mpwcStr; }
	~cLPWCSTR(){ 
		if(mpwcStr != NULL) 
			MP_DELETE_ARR( mpwcStr); 

	};
};