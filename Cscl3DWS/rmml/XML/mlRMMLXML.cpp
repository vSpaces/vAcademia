

#include "mlRMML.h"
#include <sstream>
#include "rmmlsafe.h"

namespace rmml {

mlRMMLXML::mlRMMLXML(void)
{
	mType = MLMT_XML;
	mRMMLType = MLMT_XML;
	jsoXMLDocument = NULL;
	mpRes = NULL;
	asynchronous = false;
	
	InitializeCriticalSection(&m_loadAsyncCallback);
}

mlRMMLXML::~mlRMMLXML(void){
	if(mpRes != NULL){
		mpRes->Close();
	}
	DeleteCriticalSection(&m_loadAsyncCallback);
}

#define JSPN_XMLDocument L"XMLDocument"

///// JavaScript Variable Table
JSPropertySpec mlRMMLXML::_JSPropertySpec[] = {
	JSPROP_RO(XMLDocument)
	JSPROP_RW(value)
	JSPROP_RW(asynchronous)	// XML-файл должен загружаться асинхронно?
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLXML::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(XML,mlRMMLXML,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLXML)

///// JavaScript Set Property Wrapper
JSBool mlRMMLXML::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLXML);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:{
			switch(iID){
			case JSPROP_value:
				if(JSVAL_IS_STRING(*vp)){
					JSString* jssVal = JSVAL_TO_STRING(*vp);
					priv->SetValue(wr_JS_GetStringChars(jssVal));
				}else{
					JS_ReportError(cx, "Value of value-property must be a string");
				}
				break;
			case JSPROP_asynchronous:
				ML_JSVAL_TO_BOOL(priv->asynchronous,*vp);
				break;
			}
		}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLXML::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLXML);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:{
			switch(iID){
			case JSPROP_XMLDocument:
				if(priv->jsoXMLDocument==NULL) *vp=JSVAL_NULL;
				else *vp=OBJECT_TO_JSVAL(priv->jsoXMLDocument);
				break;
			case JSPROP_asynchronous:
				*vp = BOOLEAN_TO_JSVAL(priv->asynchronous);
				break;
			}
		}
	GET_PROPS_END;
	return JS_TRUE;
}

void mlRMMLXML::CreateNewXMLDocument(){
	jsoXMLDocument = mlXMLDocument::newJSObject(mcx);
	jsval v = OBJECT_TO_JSVAL(jsoXMLDocument);
	wr_JS_DefineUCProperty(mcx, mjso, JSPN_XMLDocument, -1, v, NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
}

// реализация mlRMMLElement
mlresult mlRMMLXML::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, mpMO==NULL,"mlRMMLXML::CreateMedia");
	mpMO = PMOV_CREATED;
	if(jsoXMLDocument == NULL)
		CreateNewXMLDocument();
	return ML_OK;
}

mlresult mlRMMLXML::Load(){
	if(!SrcChanged())
		return ML_ERROR_FAILURE;
	return ML_OK;
}

bool mlRMMLXML::SrcChanged(){
	const wchar_t* pwcSrc = GetSrc();
	if(pwcSrc == NULL) return true;
	CreateNewXMLDocument();
	mlString msSrcFull = GPSM(mcx)->RefineResPathEl(this, pwcSrc);
	if(asynchronous){
		// защищаем от сборщика мусора
		SAVE_FROM_GC(mcx, GPSM(mcx)->GetPlServerGO(), mjso);

		onLoad(RMML_LOAD_STATE_SRC_LOADING);
		// отсылаем запрос на загрузку XML-файла
		EnterCriticalSection(&m_loadAsyncCallback);
		if(OMS_FAILED(GPSM(mcx)->GetContext()->mpResM->OpenAsynchResource(msSrcFull.c_str(), mpRes, (IResourceNotify*)this, RES_ANYWHERE))){
			mlTraceError(mcx, "Cannot open XML-file (%S)\n",pwcSrc);
			LeaveCriticalSection(&m_loadAsyncCallback);
			onLoad(RMML_LOAD_STATE_LOADERROR);
			return false;
		}
		LeaveCriticalSection(&m_loadAsyncCallback);
		// ??
		return true;
	}
	res::resIResource* pRes=NULL;
	if(OMS_FAILED(GPSM(mcx)->GetContext()->mpResM->OpenResource(msSrcFull.c_str(),pRes))){
		mlTraceError(mcx, "Cannot open XML-file (%S)\n",pwcSrc);
		onLoad(RMML_LOAD_STATE_LOADERROR);
		return false;
	}
	ML_ASSERTION(mcx, pRes!=NULL,"mlRMMLXML::Load()");
	istream* pis=NULL;
	if(OMS_FAILED(pRes->GetIStream(pis))){
		pRes->Close();
		onLoad(RMML_LOAD_STATE_LOADERROR);
		return false;
	}
	ML_ASSERTION(mcx, pis!=NULL,"mlRMMLXML::Load()");
#ifdef RMML_USE_LIBXML2
	if(GPSM(mcx)->GetLibXml2Funcs() != NULL){
		std::auto_ptr<mlXMLBufferRaw> pXMLBufC(new mlXMLBufferRaw(mcx, pis));
		ParseXML(pXMLBufC.get());
		pRes->Close();
		onLoad(RMML_LOAD_STATE_SRC_LOADED);
		return true;
	}
#endif
	std::auto_ptr<mlXMLBuffer> pXMLBuf(new mlXMLBuffer(pis));
	ParseXML(pXMLBuf.get());
	pRes->Close();
	onLoad(RMML_LOAD_STATE_SRC_LOADED);
	return true;
}

long mlRMMLXML::onResourceNotify( IResource* aResource, DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t *aErrorText){
	EnterCriticalSection(&m_loadAsyncCallback);
	if(aResource == NULL || mpRes == NULL)
	{
		LeaveCriticalSection(&m_loadAsyncCallback);
		return OMS_ERROR_INVALID_ARG;
	}
	if(aNotifyCode == RN_ASYNCH_CACHED){ // ?
		// мы не в главном потоке, поэтому складываем все в очередь
		istream* pis=NULL;
		if(OMS_FAILED(mpRes->GetIStream(pis))){
			mpRes->Close();
			mpRes = NULL;
			// разрешаем удаление сборщиком мусора
			FREE_FOR_GC(mcx, GPSM(mcx)->GetPlServerGO(), mjso);

			onLoad(RMML_LOAD_STATE_LOADERROR);
			LeaveCriticalSection(&m_loadAsyncCallback);
			return OMS_ERROR_INVALID_ARG;
		}
		ML_ASSERTION(mcx, pis!=NULL,"mlRMMLXML::Load()");
		std::vector<char> vBuf;
		#define RN_BLOCK_SIZE 512

		char pBuf[RN_BLOCK_SIZE + 1];
		for(;;){
			if(pis->eof())
				break;
			int iRead = pis->readsome(pBuf, RN_BLOCK_SIZE);
			if(iRead <= 0)
				break;
			int iBegin = vBuf.size();
			vBuf.resize(vBuf.size() + iRead);
			memcpy(&vBuf[iBegin], pBuf, iRead);
			if(pis->eof())
				break;
		}
		mlSynchData oData;
		GPSM(mcx)->SaveInternalPath(oData, this);
		oData << (int)vBuf.size();
		if( vBuf.size() > 0)
			oData.put(&vBuf[0], (int)vBuf.size());
		GPSM(mcx)->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_ResourceLoadedAsynchronously, (unsigned char*)oData.data(), oData.size());
		mpRes->Close();
		mpRes = NULL;
	}
	LeaveCriticalSection(&m_loadAsyncCallback);
	return OMS_OK;
}

typedef char_traits<char> _Traits;

class cbuf_streambuf: public streambuf{
	char* mpBuf;
	int miSize;
	char* mpchCurPos;
public:
	cbuf_streambuf(const char* apBuf, int aiSize){
		mpBuf = (char*)apBuf;
		miSize = aiSize;
		mpchCurPos = mpBuf;
	}
	virtual ~cbuf_streambuf(){
	}
	//virtual int showmanyc(){
	virtual streamsize showmanyc(){
		return miSize;
	}	
	virtual int_type underflow(){	// get a character from stream, but don't point past it
		if(mpchCurPos >= mpBuf+miSize) return (_Traits::eof());
		return _Traits::to_int_type(*mpchCurPos);
	}
	virtual int_type uflow(){	// get a character from stream, point past it
		if(mpchCurPos >= mpBuf+miSize) return (_Traits::eof());
		return _Traits::to_int_type(*mpchCurPos++);
	}
};

void mlRMMLXML::ResourceLoadedAsynchronously(mlSynchData &oData){
	int iSize = 0;
	oData >> iSize;
	if( iSize == 0)
	{
		onLoad(RMML_LOAD_STATE_LOADERROR);
		return;
	}
	MP_NEW_P2( pStreamBuf, cbuf_streambuf, (const char*)oData.getDataInPos(), iSize);
	std::istream is(pStreamBuf);
#ifdef RMML_USE_LIBXML2
	if(GPSM(mcx)->GetLibXml2Funcs() != NULL){
		std::auto_ptr<mlXMLBufferRaw> pXMLBufC(new mlXMLBufferRaw(mcx, &is));
		ParseXML(pXMLBufC.get());
	}
#else
	std::auto_ptr<mlXMLBuffer> pXMLBuf(new mlXMLBuffer(&is));
	ParseXML(pXMLBuf.get());
#endif
	MP_DELETE( pStreamBuf);
	pStreamBuf = NULL;
	onLoad(RMML_LOAD_STATE_SRC_LOADED);

	// разрешаем удаление сборщиком мусора
	FREE_FOR_GC(mcx, GPSM(mcx)->GetPlServerGO(), mjso);
}

mlRMMLElement* mlRMMLXML::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLXML* pNewEL=(mlRMMLXML*)GET_RMMLEL(mcx, mlRMMLXML::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

//void mlRMMLXML::JSDestructor(JSContext *cx, JSObject *obj) {
//	mlRMMLXML *p = (mlRMMLXML*)JS_GetPrivate(cx, obj);
//	if (p){
//		 MP_DELETE( p;
//	}
//}

//locale gds_loc("Russian");//English_Britain");//German_Germany
//mbstate_t gds_state=0;
//char* gds_pszNext;
//wchar_t* gds_pwszNext;

__forceinline bool isEqual2(const wchar_t* apch1, const wchar_t* apch2){
	for(;;){
		if(*apch2==L'\0') return true;
		if(*apch1!=*apch2) return false;
		apch1++, apch2++;
	}
}

void mlXMLBuffer::DefineEncoding(wchar_t* pwszStart, wchar_t* pwszEnd){
	int iState=0;
	wchar_t* pwsz = L"";
	for(pwsz=pwszStart; pwsz < pwszEnd-6; pwsz++){
		if(pwsz[0]==L'c' && pwsz[1]==L'o' && isEqual2(pwsz,L"coding")){
			pwsz+=6;
			iState=1;
			break;
		}
		if(pwsz[0]==L'C' && pwsz[1]==L'O' && isEqual2(pwsz,L"CODING")){
			pwsz+=6;
			iState=1;
			break;
		}
	}
	if(iState==0) return;
	for(; pwsz < pwszEnd-4; pwsz++){
		if(*pwsz==L'\"'){
			pwsz++;
			iState=2;
			break;
		}
	}
	if(iState==1) return;
	for(; pwsz < pwszEnd; pwsz++)
		if(*pwsz!=L' ') break;
	if((*pwsz==L'w' || *pwsz==L'W') && (pwszEnd-pwsz) > 7){
		for(; pwsz<(pwszEnd-3); pwsz++){
			if(*pwsz==L'-' && isEqual2(pwsz,L"-1251")){
				enc=WIN1251;
				return;
			}
			if((*pwsz==L'n' || *pwsz==L'N') && (pwsz[1] == L'1') && isEqual2(pwsz+1,L"1251")){
				enc=WIN1251;
				return;
			}
		}
	}
	if((*pwsz==L'u' || *pwsz==L'U') && (pwszEnd-pwsz) >= 5){
		if(isEqual2(pwsz,L"UTF-8") || 
		   isEqual2(pwsz,L"utf-8")){
			enc=UTF8;
			return;
		}
	}
}

unsigned mlXMLBuffer::Read( wchar_t* pszBuffer, unsigned uCount, unsigned* puCount ){
	if(is==NULL) return 0;
	unsigned uLength;
	wchar_t* pwszWC=pszBuffer;
	wchar_t* pwszStart=pwszWC;
	wchar_t* pwszEnd=pwszStart+uCount/sizeof(wchar_t);

	streampos pos=is->tellg();
	if(streamoff(pos)==0){
		// если в самом начале, то кодировка еще неизвестна
		enc=UNKNOWN;
		mbHeader=true;
	}
	char chPrev = ' ';
	char ch;
	while( (!is->eof()) && (pwszWC != pwszEnd) ){
		is->get(ch);
		if(mbHeader && enc == UNKNOWN){
			if(chPrev == '?' && ch == '>'){
				mbHeader = false;
				if((unsigned)(pwszWC - pwszStart) > 6){
					DefineEncoding(pwszStart, pwszWC);
				}
				if(enc==UNKNOWN) enc=DEFAULT; // UTF-8
			}
			chPrev = ch;
		}
		if(enc==WIN1251){
			if((unsigned char)ch > 0x7F){
				mbtowc(pwszWC, &ch, 1);
//				std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >(gds_loc, (std::codecvt<wchar_t, char, mbstate_t> *)0, true).in( 
//					gds_state, 
//					&ch, &ch+1, gds_pszNext, 
//					pwszWC , pwszWC+1, gds_pwszNext);
			}else{
				*pwszWC=ch;
			}
		}else{
			if( (ch & 0x80) == 0 ){
				*pwszWC = ch;
				//pbszUTF8++;
			}else{
				// Calculate bits
				char chTemp;
				if( (ch & 0xF0) == 0xF0 ) { uLength = 4; chTemp = (char)0xF0; }
				else if( (ch & 0xE0) == 0xE0 ) { uLength = 3; chTemp = (char)0xE0; }
				else if( (ch & 0xC0) == 0xC0 ) { uLength = 2; chTemp = (char)0xC0; }
				else 
					return 0;

				// Check if UTF8 string is complete
				if( is->eof() ){
					goto exit;
				}

				// Build character
				// The first byte needs to clear part that has count of bytes for the UTF8 character.
				// This is done by clearing the number bits for each byte + 1 from right that the character
				// are taken.
				// Then we shift the final character one octal bitlenght for each byte that is added.
				uLength--;
				*pwszWC = (ch & ~chTemp) << (uLength * 6);

				is->get(ch); uLength--;
				*pwszWC |= (ch & 0x3F) << (uLength * 6);
				if( uLength ){
					is->get(ch); uLength--;
					*pwszWC |= (ch & 0x3F) << (uLength * 6);
					if( uLength ){
						is->get(ch); uLength--;
						*pwszWC |= (ch & 0x3F);
					}
				}
			}
		}
		pwszWC++;
	}
exit:
//	*pwszWC = L'\0';
//	if(is->fail()){
//		mlTrace(cx, "XML-file read error (%S)\n",L"??");
//	}

	if( puCount != NULL ) *puCount = (unsigned)(pwszWC - pwszStart)*sizeof(wchar_t);
//		*puCount = is->rdbuf()->sgetn(&pszBuffer[0], uCount );
	return *puCount;
}

#define UTF_TRAIL( value, n ) ( (char)(0x80 | (value >> (n * 6))) )
__forceinline unsigned UTF8Char( unsigned uChar, char* pszBuffer ){
   if( uChar <= 0x7F ){
      *pszBuffer = (char)uChar;
      return 1;
   }else if( uChar <= 0x7FF ){
      *pszBuffer = (char)(0xC0 | (uChar >> 6));
      pszBuffer++;
      *pszBuffer = (char)(uChar & 0xBF) | 0x80; // | 0x80 <- ?? (Tandy: 17.12.04)
      return 2;
   }else if( uChar <= 0xFFFF ){
      *pszBuffer = (char)(0xE0 | (uChar >> 12));
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 1 );
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 0 );
      return 3;
   }else if( uChar <= 0x1FFFFF ){
      *pszBuffer = (char)(0xF0 | (uChar >> 18));
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 2 );
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 1 );
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 0 );
      return 4;
   } // if( uChar <= 0x1FFFFF ){
   return 0;                                                                                 
}                                                                                            

void mlXMLBuffer::Write( const wchar_t* pszBuffer, unsigned uCount, unsigned* puCount){
	if(ios==NULL) return;
//mlTrace(cx, "%S",pszBuffer);
char* pszTemp = MP_NEW_ARR( char, uCount+1);
char* pchT=pszTemp;
	for(int ii=0; ii < (int)(uCount/sizeof(wchar_t)); ii++){
		wchar_t wch=pszBuffer[ii];
		if(enc==WIN1251){
			char ch;
			if(wch > 0x7F){
				char mbc[5];
				int iLen =0;
				wctomb_s(&iLen, mbc, 5, wch);
				ios->write(mbc,iLen);
			}else{
				ch=wch;
				ios->put(ch);
			}
		}else{ // UTF-8
		   char pbsz[5];
		   unsigned uLength = UTF8Char(wch, pbsz);
//if(uLength>1)
//int hh=0;
*pchT++=*pbsz;
		   ios->write(pbsz,uLength);
		}
	}
*pchT='\0';
//mlTrace(cx, "%s",pszTemp);
MP_DELETE_ARR( pszTemp);
}

struct mlXMLBufferW:public mlXMLBuf{
	std::wistream* wis;
	std::wiostream* wios;
	mlXMLBufferW(std::wistream* awis){ wis=awis; wios=NULL; }
	mlXMLBufferW(std::wiostream* awis){ wis=awis; wios=NULL; }
	void Write( const wchar_t* pszBuffer, unsigned uCount, unsigned* puCount);
	unsigned Read( wchar_t* pszBuffer, unsigned uCount, unsigned* puCount );
};

unsigned mlXMLBufferW::Read( wchar_t* pszBuffer, unsigned uCount, unsigned* puCount ){
	if(wis==NULL) return 0;
//	unsigned uLength;
	wchar_t* pwszWC=pszBuffer;
	wchar_t* pwszStart=pwszWC;
	wchar_t* pwszEnd=pwszStart+uCount/sizeof(wchar_t)-1;

	wchar_t wch;
	wis->get(wch);
	while( (!wis->eof()) && (pwszWC != pwszEnd) ){
		*pwszWC = wch;
		pwszWC++;
		wis->get(wch);
	}
	*pwszWC = L'\0';

	if( puCount != NULL ) *puCount = (unsigned)(pwszWC - pwszStart)*sizeof(wchar_t);
	return *puCount;
}

void mlXMLBufferW::Write( const wchar_t* pszBuffer, unsigned uCount, unsigned* puCount){
	if(wios==NULL) return;
	wios->write(pszBuffer,uCount);
}

__forceinline bool IsSpace(const wchar_t* apwcStr){
	if(apwcStr==NULL) return true;
	wchar_t* pwcSpaceChars=L" \t\n\r";
	for(;;){
		if(*apwcStr==L'\0') return true;
		for(wchar_t* pwcSC=pwcSpaceChars; ; pwcSC++){
			if(*pwcSC==L'\0') return false;
			if(*apwcStr==*pwcSC) break;
		}
		apwcStr++;
	}
}

//		   char pbsz[5];
//		   unsigned uLength = UTF8Char(wch, pbsz);

mlresult mlRMMLXML::SetValue(wchar_t* apwcValue){
	if(apwcValue == NULL || *apwcValue == L'\0')
		return ML_OK;
	//
#ifdef RMML_USE_LIBXML2
	if(GPSM(mcx)->GetLibXml2Funcs() != NULL){
		int iLen = wcslen(apwcValue);
		std::auto_ptr<char> pcBuf(new char[iLen*4+1]);
		char* pc = pcBuf.get();
		for(wchar_t* pwc = apwcValue; *pwc != L'\0'; pwc++){
			unsigned uLength = UTF8Char(*pwc, pc);
			pc += uLength;
		}
		*pc = '\0';
		std::istringstream iss;
		iss.str(pcBuf.get());
		std::auto_ptr<mlXMLBufferRaw> pXMLBufC(new mlXMLBufferRaw(mcx, &iss));
		return ParseXML(pXMLBufC.get());
	}
#endif
	std::wistringstream wiss;
	wiss.str(apwcValue);
	std::auto_ptr<mlXMLBufferW> pXMLBufW(new mlXMLBufferW(&wiss));
	return ParseXML(pXMLBufW.get());
}

mlresult mlRMMLXML::ParseXML(mlXMLBuf* apXMLBuf){

	gd_xml_stream::CXMLArchive<mlXMLBuf> ar(apXMLBuf);
	//
	CreateNewXMLDocument();
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(mcx,jsoXMLDocument);
//	jsval v=OBJECT_TO_JSVAL(jsoXMLDocument);
//	JS_SetUCProperty(mcx, mjso, JSPN_XMLDocument, wcslen(JSPN_XMLDocument), &v);
	//
	std::vector<JSObject*> vParents;
	vParents.push_back(jsoXMLDocument);

	int iNodeSum=0;
	ar.SkipHeader();
	while(1){
		if(!ar.ReadNext()){
			if(iNodeSum>0) break;
			if(ar.GetName().IsEmpty()) break;
		}
		gd::CString sName=ar.GetName(); // sName.MakeLower();
		iNodeSum++;
		if(ar.IsEndTag()){
//for(int ii=ar.GetDepth()+1; ii>0; ii--) TRACE("  ");
//TRACE("</%S>\n",(LPCWSTR)sName);
			if(vParents.size() > 0){
				vParents.pop_back();
			}
			// берем текст
			gd::CString sNextText=ar.GetTextAfterEndTag();
//			sNextText.Trim(L" \t\r\n",gd::CString::eLeft);
//			sNextText.Trim(L" \t\r\n");
			if(!sNextText.IsEmpty() && !IsSpace(sNextText)){
//for(int ii=ar.GetDepth()+1; ii>0; ii--) TRACE("  ");
//TRACE("%S\n",(LPCWSTR)sNextText);
				JSString* jssText=wr_JS_NewUCStringCopyN(mcx, sNextText.GetBuffer(), sNextText.GetLength());
				JSObject* jsoXMLText=pXMLDoc->createTextNode(jssText);
				// добавляем его родителю
				JSObject* jsoParent=NULL;
				ML_ASSERTION(mcx, vParents.size()>0, "mlRMMLXML::SetValue");
				jsoParent=vParents.back();
				mlIXMLNode* pParent=(mlIXMLNode*)JS_GetPrivate(mcx,jsoParent);
				pParent->appendChild(jsoXMLText);
			}
		}else{
//for(int ii=ar.GetDepth(); ii>0; ii--) TRACE("  ");
//TRACE("<%S",(LPCWSTR)sName);
			// создаем новый элемент
			JSString* jssTagName=wr_JS_NewUCStringCopyN(mcx, sName.GetBuffer(), sName.GetLength());
			JSObject* jsoXMLElement=pXMLDoc->createElement(jssTagName);
			mlXMLElement* pXMLEl=(mlXMLElement*)JS_GetPrivate(mcx,jsoXMLElement);
			// добавляем его родителю
			JSObject* jsoParent=NULL;
			ML_ASSERTION(mcx, vParents.size()>0, "mlRMMLXML::SetValue");
			jsoParent=vParents.back();
			mlIXMLNode* pParent=(mlIXMLNode*)JS_GetPrivate(mcx,jsoParent);
			pParent->appendChild(jsoXMLElement);
			// 
			JSString* jssID=NULL;
			gd::CString sAttrName, sAttrValue;
			unsigned uCount = ar.GetAttributeCount();
			if(uCount>0){
				mlXMLNamedNodeMap* pAttrs=pXMLEl->GetAttributes();
				for(unsigned u=0; u<uCount; u++) {
					sAttrName = ar.GetAttributeName(u); //sAttrName.MakeLower();
					sAttrValue = ar.GetAttribute(u);
//TRACE(" %S=\"%S\"",(LPCWSTR)sAttrName,(LPCWSTR)sAttrValue);
					JSString* jssAttrName=wr_JS_NewUCStringCopyN(mcx, sAttrName.GetBuffer(), sAttrName.GetLength());
					JSObject* jsoAttr = pXMLDoc->createAttribute(jssAttrName);
					SAVE_FROM_GC(mcx, mjso, jsoAttr);
					JSString* jssAttrValue=wr_JS_NewUCStringCopyN(mcx, sAttrValue.GetBuffer(), sAttrValue.GetLength());
					mlXMLAttr* pAttr=(mlXMLAttr*)JS_GetPrivate(mcx,jsoAttr);
					pAttr->SetValue(jssAttrValue);
					pAttrs->setNamedItem(jsoAttr);
					FREE_FOR_GC(mcx, mjso, jsoAttr);
					if(sAttrName.GetLength()==2){
						if(sAttrName[0]==L'i' || sAttrName[0]==L'I'){
							if(sAttrName[1]==L'd' || sAttrName[1]==L'D'){
								// устанавлинвается свойство ID XML-элемента
								jssID=pAttr->nodeValue;
							}
						}
					}
				}
			}
			//
			if(jssID!=NULL)
				pXMLDoc->UpdateIDMap(jssID,jsoXMLElement);
//TRACE(">\n");
			// берем текст
			gd::CString sValue=ar.GetValue();
//			sValue.Trim(L" \t\r\n",gd::CString::eLeft);
//			sValue.Trim(L" \t\r\n");
			if(!sValue.IsEmpty()){
//for(int ii=ar.GetDepth()+1; ii>0; ii--) TRACE("  ");
//TRACE("%S\n",(LPCWSTR)sValue);
				JSString* jssText=wr_JS_NewUCStringCopyN(mcx, sValue.GetBuffer(), sValue.GetLength());
				JSObject* jsoXMLText=pXMLDoc->createTextNode(jssText);
				pXMLEl->appendChild(jsoXMLText);
			}
			if(ar.HasChildren()){
				vParents.push_back(jsoXMLElement);
			}else{
//for(int ii=ar.GetDepth(); ii>0; ii--) TRACE("  ");
//TRACE("</%S>\n",(LPCWSTR)sName);
				// берем текст
				gd::CString sNextText=ar.GetTextAfterEndTag();
//				sNextText.Trim(L" \t\r\n",gd::CString::eLeft);
//				sNextText.Trim(L" \t\r\n");
				if(!sNextText.IsEmpty() && !IsSpace(sNextText)){
//for(int ii=ar.GetDepth(); ii>0; ii--) TRACE("  ");
//TRACE("%S\n",(LPCWSTR)sNextText);
					JSString* jssText=wr_JS_NewUCStringCopyN(mcx, sNextText.GetBuffer(), sNextText.GetLength());
					JSObject* jsoXMLText=pXMLDoc->createTextNode(jssText);
					pParent->appendChild(jsoXMLText);
				}
			}
		}
	}
	vParents.clear();
	return ML_OK;
}

#ifdef RMML_USE_LIBXML2

//xmlSAXHandler mSAX2HandlerStructXE =
//{
//	NULL, // internalSubsetDebug,
//	NULL, // isStandaloneDebug,
//	NULL, // hasInternalSubsetDebug,
//	NULL, // hasExternalSubsetDebug,
//	NULL, // resolveEntityDebug,
//	NULL, // getEntityDebug,
//	NULL, // entityDeclDebug,
//	NULL, // notationDeclDebug,
//	NULL, // attributeDeclDebug,
//	NULL, // elementDeclDebug,
//	NULL, // unparsedEntityDeclDebug,
//	NULL, // setDocumentLocatorDebug,
//	NULL, // startDocumentDebug,
//	NULL, // endDocumentDebug,
//	NULL, 
//	NULL, 
//	NULL, // referenceDebug,
//	mlRMMLBuilder::characters, // charactersDebug,
//	NULL, // ignorableWhitespaceDebug,
//	NULL, // processingInstructionDebug,
//	NULL, // commentDebug,
//	mlRMMLBuilder::warningDebug, // warningDebug,
//	mlRMMLBuilder::errorDebug, // errorDebug,
//	mlRMMLBuilder::fatalErrorDebug, // fatalErrorDebug,
//	NULL, // getParameterEntityDebug,
//	NULL, // cdataBlockDebug,
//	NULL, // externalSubsetDebug,
//	XML_SAX2_MAGIC, 
//	NULL, 
//	mlRMMLBuilder::startElementNs, // startElementNsDebug,
//	mlRMMLBuilder::endElementNs, // endElementNsDebug,
//	NULL
//};

//xmlSAXHandlerPtr mSAX2HandlerXE = &mSAX2HandlerStructXE;

//mlresult mlRMMLXML::ParseXML(mlXMLBuf* apXMLBuf){
//	// ??
//	return ML_OK;
//}

void mlXMLBufferRaw::Write( const char* pszBuffer, unsigned uCount, unsigned* puCount){
	// ??
}

unsigned mlXMLBufferRaw::Read( char* pszBuffer, unsigned uCount, unsigned* puCount ){
	return (*puCount)=(int)(is->readsome(pszBuffer, uCount));
}

#define XMLRDBFMAXLEN 4096
wchar_t cpwcXMLReadBuf[XMLRDBFMAXLEN+1];

int InputReadCallback(void * context, char * buffer, int len){
	mlXMLBufC* pXBuf = (mlXMLBufC*)context;
	unsigned iRealLen = 0;
	pXBuf->Read(buffer, len, &iRealLen);
	return (int)iRealLen;
}

int InputCloseCallback(void * context){
	mlXMLBufC* pXBuf = (mlXMLBufC*)context;
	return 0;
}

char sErr[5000]="";
void myGenericErrorFunc	(void * ctx, const char * msg, ...){
    va_list args;
    va_start(args, msg);
    //vsprintf(sErr, msg, args);
	rmmlsafe_vsprintf(sErr, sizeof(sErr), 0, msg, args);
    va_end(args);
	mlXMLBufC* pXBuf = (mlXMLBufC*)ctx;
	if( pXBuf)
		TRACE(pXBuf->GetJSContext(), "%s", sErr);
	//::OutputDebugString(sErr);
}

mlresult mlRMMLXML::ParseXML(mlXMLBufC* apXMLBuf){
	mlSceneManager::mlLibXml2Funcs* pLibXml2Funcs = GPSM(mcx)->GetLibXml2Funcs();
	xmlTextReaderPtr reader;
	int ret;

	pLibXml2Funcs->xmlSetGenericErrorFunc(NULL, myGenericErrorFunc);
	xmlParserInputBufferPtr inputBuffer = NULL;
	inputBuffer = pLibXml2Funcs->xmlAllocParserInputBuffer(XML_CHAR_ENCODING_NONE);
	if (inputBuffer == NULL){
		mlTrace(mcx, "Cannot allocate XML parser input buffer");
		return ML_ERROR_FAILURE;
	}
	inputBuffer->context = apXMLBuf; // ctxt;
	inputBuffer->readcallback = InputReadCallback;
	inputBuffer->closecallback = InputCloseCallback;

	std::string sSrc = "";
	const wchar_t* pwcSrc = GetLoadable()->GetSrc();
	if(pwcSrc != NULL)
		sSrc = cLPCSTR(pwcSrc);
	else if(miSrcFilesIdx >= 0){
		pwcSrc = GPSM(mcx)->mSrcFileColl[miSrcFilesIdx];
		sSrc = cLPCSTR(pwcSrc);
		char caNum[20];
		sprintf_s(caNum, 20, ":%d", miSrcLine);
		sSrc += caNum;
	}
	reader = pLibXml2Funcs->xmlNewTextReader(inputBuffer, sSrc.c_str());
	if(reader != NULL){
		// создаем новый XMLDocument
		CreateNewXMLDocument();
		mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(mcx,jsoXMLDocument);
//		jsval v=OBJECT_TO_JSVAL(jsoXMLDocument);
//		JS_SetUCProperty(mcx, mjso, JSPN_XMLDocument, wcslen(JSPN_XMLDocument), &v);
		//
		std::vector<JSObject*> vParents;
		vParents.push_back(jsoXMLDocument);

		for(;;){
			ret = pLibXml2Funcs->xmlTextReaderRead(reader);
			if(ret != 1)
				break;
			int iNodeType = pLibXml2Funcs->xmlTextReaderNodeType(reader);
int iNodeDepth = pLibXml2Funcs->xmlTextReaderDepth(reader);
			if(iNodeType == XML_READER_TYPE_END_ELEMENT){
//for(int ii = 0; ii < iNodeDepth; ii++)
//TRACE("  ");
//TRACE("</%s>\n", pLibXml2Funcs->xmlTextReaderName(reader));
				if(vParents.size() > 0)
					vParents.pop_back();
				continue;
			}
			JSObject* jsoParent=NULL;
			ML_ASSERTION(mcx, vParents.size()>0, "mlRMMLXML::SetValue");
			jsoParent=vParents.back();
			mlIXMLNode* pParent=(mlIXMLNode*)JS_GetPrivate(mcx,jsoParent);
			if(iNodeType == XML_READER_TYPE_ELEMENT){
				// создаем новый элемент
//for(int ii = 0; ii < iNodeDepth; ii++)
//TRACE("  ");
//TRACE("<%s", pLibXml2Funcs->xmlTextReaderName(reader));
				const char* pcTagName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);
				JSString* jssTagName = JS_NewStringCopyZ(mcx, pcTagName);
				JSObject* jsoXMLElement = pXMLDoc->createElement(jssTagName);
				mlXMLElement* pXMLEl=(mlXMLElement*)JS_GetPrivate(mcx,jsoXMLElement);
				// добавляем его родителю
				pParent->appendChild(jsoXMLElement);
				JSString* jssID=NULL;
				if(pLibXml2Funcs->xmlTextReaderHasAttributes(reader)){
					mlXMLNamedNodeMap* pAttrs=pXMLEl->GetAttributes();
					while(pLibXml2Funcs->xmlTextReaderMoveToNextAttribute(reader)){
						const char* pcName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);
						const char* pcValue = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader);
						mlString sValue;
						UTF82String(pcValue, -1, sValue);
//TRACE(" %s=\"%S\"", pcName, sValue.c_str());
						JSString* jssAttrName = JS_NewStringCopyZ(mcx, pcName);
						SAVE_STR_FROM_GC(mcx, jsoXMLElement, jssAttrName);
						JSString* jssAttrValue = wr_JS_NewUCStringCopyZ(mcx, sValue.c_str());
						SAVE_STR_FROM_GC(mcx, jsoXMLElement, jssAttrValue);
						JSObject* jsoAttr = pXMLDoc->createAttribute(jssAttrName);
						mlXMLAttr* pAttr = (mlXMLAttr*)JS_GetPrivate(mcx,jsoAttr);
						pAttr->SetValue(jssAttrValue);
						pAttrs->setNamedItem(jsoAttr);
						FREE_STR_FOR_GC(mcx, jsoXMLElement, jssAttrName);
						FREE_STR_FOR_GC(mcx, jsoXMLElement, jssAttrValue);
						if(pcName[0] == 'i' || pcName[0] == 'I'){
							if(pcName[1] == 'd' || pcName[1] == L'D'){
								if(pcName[2] == '\0'){
									// устанавлинвается свойство ID XML-элемента
									jssID=pAttr->nodeValue;
								}
							}
						}
					}
					if(jssID!=NULL)
						pXMLDoc->UpdateIDMap(jssID,jsoXMLElement);
					pLibXml2Funcs->xmlTextReaderMoveToElement(reader);
				}
				if(!pLibXml2Funcs->xmlTextReaderIsEmptyElement(reader)){
//TRACE(">\n");
					vParents.push_back(jsoXMLElement);
				}else{
//TRACE("/>\n");
				}
				continue;
			}
			if(iNodeType == XML_READER_TYPE_TEXT || iNodeType == XML_READER_TYPE_CDATA){
				const char* pcValue = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader);
				mlString sValue;
				UTF82String(pcValue, -1, sValue);
//for(int ii = 0; ii < iNodeDepth; ii++)
//TRACE("  ");
//TRACE("%S\n", sValue.c_str());
				JSString* jssText = wr_JS_NewUCStringCopyZ(mcx, sValue.c_str());
				SAVE_STR_FROM_GC(mcx, mjso, jssText);
				JSObject* jsoXMLText=pXMLDoc->createTextNode(jssText);
				pParent->appendChild(jsoXMLText);
				FREE_STR_FOR_GC(mcx, mjso, jssText);
				continue;
			}
		} // for(;;)
		pLibXml2Funcs->xmlFreeTextReader(reader);
		if(ret != 0){
			mlTraceError(mcx, "Failed to parse %s\n", sSrc.c_str());
		}
	}else{
		mlTraceError(mcx, "Unable to open %s\n", sSrc.c_str());
	}
	pLibXml2Funcs->xmlSetGenericErrorFunc(NULL, NULL);
	return ML_OK;
}

#endif

/*
interface Node {
  // NodeType
  const unsigned short      ELEMENT_NODE       = 1;
  const unsigned short      ATTRIBUTE_NODE     = 2;
  const unsigned short      TEXT_NODE          = 3;
  const unsigned short      CDATA_SECTION_NODE = 4;
  const unsigned short      ENTITY_REFERENCE_NODE = 5;
  const unsigned short      ENTITY_NODE        = 6;
  const unsigned short      PROCESSING_INSTRUCTION_NODE = 7;
  const unsigned short      COMMENT_NODE       = 8;
  const unsigned short      DOCUMENT_NODE      = 9;
  const unsigned short      DOCUMENT_TYPE_NODE = 10;
  const unsigned short      DOCUMENT_FRAGMENT_NODE = 11;
  const unsigned short      NOTATION_NODE      = 12;

  readonly attribute  DOMString            nodeName;
           attribute  DOMString            nodeValue;
                                                 // raises(DOMException) on setting
                                                 // raises(DOMException) on retrieval
  readonly attribute  unsigned short       nodeType;
  readonly attribute  Node                 parentNode;
  readonly attribute  NodeList             childNodes;
  readonly attribute  Node                 firstChild;
  readonly attribute  Node                 lastChild;
  readonly attribute  Node                 previousSibling;
  readonly attribute  Node                 nextSibling;
  readonly attribute  NamedNodeMap         attributes;
  readonly attribute  Document             ownerDocument;
  Node                      insertBefore(in Node newChild, 
                                         in Node refChild)
                                         raises(DOMException);
  Node                      replaceChild(in Node newChild, 
                                         in Node oldChild)
                                         raises(DOMException);
  Node                      removeChild(in Node oldChild)
                                        raises(DOMException);
  Node                      appendChild(in Node newChild)
                                        raises(DOMException);
  boolean                   hasChildNodes();
  Node                      cloneNode(in boolean deep);
};
interface Document : Node {
  readonly attribute  DocumentType         doctype;
  readonly attribute  DOMImplementation    implementation;
  readonly attribute  Element              documentElement;
  Element                   createElement(in DOMString tagName)
                                          raises(DOMException);
  DocumentFragment          createDocumentFragment();
  Text                      createTextNode(in DOMString data);
  Comment                   createComment(in DOMString data);
  CDATASection              createCDATASection(in DOMString data)
                                               raises(DOMException);
  ProcessingInstruction     createProcessingInstruction(in DOMString target, 
                                                        in DOMString data)
                                                        raises(DOMException);
  Attr                      createAttribute(in DOMString name)
                                            raises(DOMException);
  EntityReference           createEntityReference(in DOMString name)
                                                  raises(DOMException);
  NodeList                  getElementsByTagName(in DOMString tagname);
};
interface NodeList {
  Node                      item(in unsigned long index);
  readonly attribute  unsigned long        length;
};
*/

#define DEF_XML_CONST(N,V) \
	JS_DefineProperty(cx,obj,#N,INT_TO_JSVAL(V),NULL,NULL,0);

mlresult mlRMMLXML::InitJSClasses(JSContext* cx , JSObject* obj){
//mlMapJSS2JSO map1;
//JSString* jssKey1=JS_NewStringCopyZ(cx,"key1");
//JSObject* jsoVal1=JS_NewObject(cx,NULL,NULL,NULL);
//map1.insert(jssKey1,jsoVal1);
//JSString* jssKey2Find=JS_NewStringCopyZ(cx,"key1");
//JSObject* jsoFound=map1.find(jssKey2Find);
//map1.erase(jssKey2Find);

	DEF_XML_CONST( XML_ELEMENT_NODE, 1);
	DEF_XML_CONST( XML_ATTRIBUTE_NODE, 2);
	DEF_XML_CONST( XML_TEXT_NODE, 3);
	DEF_XML_CONST( XML_CDATA_SECTION_NODE, 4);
	DEF_XML_CONST( XML_ENTITY_REFERENCE_NODE, 5);
	DEF_XML_CONST( XML_ENTITY_NODE, 6);
	DEF_XML_CONST( XML_PROCESSING_INSTRUCTION_NODE, 7);
	DEF_XML_CONST( XML_COMMENT_NODE, 8);
	DEF_XML_CONST( XML_DOCUMENT_NODE, 9);
	DEF_XML_CONST( XML_DOCUMENT_TYPE_NODE, 10);
	DEF_XML_CONST( XML_DOCUMENT_FRAGMENT_NODE, 11);
	DEF_XML_CONST( XML_NOTATION_NODE, 12);

	mlRMMLXML::InitJSClass(cx,obj);
	mlXMLNodeList::InitJSClass(cx,obj);
	mlXMLAttr::InitJSClass(cx,obj);
	mlXMLText::InitJSClass(cx,obj);
	mlXMLCDATASection::InitJSClass(cx,obj);
	mlXMLComment::InitJSClass(cx,obj);
	mlXMLNamedNodeMap::InitJSClass(cx,obj);
	mlXMLElement::InitJSClass(cx,obj);
	mlXMLDocument::InitJSClass(cx,obj);
	// ??
	return ML_OK;
}

#define CREATE_JSXMLEL(CNM,NM) \
	}else if(isEqual(#CNM,apszClassName)){ \
		return NM::newJSObject(cx);

JSObject* mlRMMLXML::CreateElementByClassName(JSContext* cx, const char* apszClassName){
	if(false){
		CREATE_JSXMLEL(__XMLNodeList_,mlXMLNodeList)
		CREATE_JSXMLEL(__XMLAttr_,mlXMLAttr)
		CREATE_JSXMLEL(XMLText,mlXMLText)
		CREATE_JSXMLEL(XMLCDATASection,mlXMLCDATASection)
		CREATE_JSXMLEL(XMLComment,mlXMLComment)
		CREATE_JSXMLEL(__NamedNodeMap_,mlXMLNamedNodeMap)
		CREATE_JSXMLEL(XMLElement,mlXMLElement)
		CREATE_JSXMLEL(XMLDocument,mlXMLDocument)
	}
	return NULL;
}

// затычка
//bool mlIXMLNode::select(const wchar_t* apwcXPathExpr, mlNodeVect& avNodes){
//	// ??
//	return true;
//}

}
