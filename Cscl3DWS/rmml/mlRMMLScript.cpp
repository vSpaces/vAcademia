#include "mlRMML.h"
#include "config/oms_config.h"
#include <strstream>
#include "config/prolog.h"
#include <windows.h>
#include "ILogWriter.h"

//#define PROFILE_RMML_EXECUTING
#define PROFILE_RMML_EXECUTING_SC

#ifdef PROFILE_RMML_EXECUTING

__int64 m_freq;

void tp_Init()
{
	static bool init = false;
	if (init)
	{
		return;
	}
	init = true;
	LARGE_INTEGER s;
	QueryPerformanceFrequency(&s);
	m_freq = s.QuadPart;
}

__int64 tp_GetTime()
{
	LARGE_INTEGER s;
	QueryPerformanceCounter(&s);

	return s.QuadPart;
}

__int64 tp_GetMicroTickCount()
{
	if (m_freq == 0)
	{
		m_freq = 1;
	}

	return ((tp_GetTime()) * 1000000 / m_freq);
}
#endif

namespace rmml {

// скомпилировать скрипт, если он еще не скомпилирован
//#define COMPILE_IF_NOT_YET \
//	if(mucFlags & SCRF_NOT_COMPILED){ \
//		mucFlags &= ~SCRF_NOT_COMPILED; \ 
//	}

struct mlRMMLScript::EventExpressionStruct{
	mlString msExpr;
	std::vector<EventSender> mvWaitingEvents;
	std::vector<EventSender> mvFiredEvents;
};

wchar_t* CopyWStr(const wchar_t* apwc){
	if(apwc==NULL) return NULL;
	int iLen=wcslen(apwc);
	wchar_t* pwc = MP_NEW_ARR( wchar_t, iLen+1);
	//wcscpy(pwc,apwc);
	rmmlsafe_wcscpy(pwc, iLen+1, 0, apwc);
	return pwc;
}

mlRMMLScript::mlRMMLScript(void)
{
//TRACE("%d\n",sizeof(mlRMMLScript));
	mRMMLType = MLMT_SCRIPT;
	mType = MLMT_SCRIPT; // onLoad
	mbNeedToLoadScript=false;

	//mbRef=false;
	mucFlags = 0;
	mpData=NULL;
#ifdef PROFILE_RMML_EXECUTING
	tp_Init();
#endif
}

// Когда объект исключается из иерархии сцены (parent=null), то надо, чтобы он перестал обрабатывать и события
void mlRMMLScript::UnlinkFromEventSenders(bool abWithChildren){
	if(mType & MLSTF_NOTSET){
		if(mpData!=NULL){
			wchar_t* pwc=(wchar_t*)mpData;
			MP_DELETE_ARR( pwc);
			mpData = NULL;
		}
	}else if(mType==MLST_EVENT || mType==MLST_WATCH || mType==MLST_SYNCHMS || mType==MLST_EVEXPR || mType==MLST_SYNCHPROP){
		if(mpData!=NULL){
			// удаляем себя из списка слушателей 
			RemoveMeFromEventListeners();
			if(mType==MLST_EVEXPR){
				EventExpressionStruct* pEvExprStruct = (EventExpressionStruct*)mpData;
				if(pEvExprStruct!=NULL)  MP_DELETE( pEvExprStruct);
				mpData = NULL;
			}else if(mType==MLST_SYNCHMS){
				SynchEventSender* pSESenderStruct=(SynchEventSender*)mpData;
				if(pSESenderStruct!=NULL)  MP_DELETE( pSESenderStruct);
				mpData = NULL;
			}else if(mType==MLST_SYNCHPROP){
				SynchPropEventSender* pSPESenderStruct = (SynchPropEventSender*)mpData;
				if(pSPESenderStruct!=NULL)  MP_DELETE( pSPESenderStruct);
				mpData = NULL;
			}else{
				EventSender* pSenderStruct=(EventSender*)mpData;
				if(pSenderStruct!=NULL)	 MP_DELETE( pSenderStruct);
				mpData = NULL;
			}
		}
	}
	mType |= MLSTF_NOTSET;
}

mlRMMLScript::~mlRMMLScript(void)
{
	UnlinkFromEventSenders();
	//	ML_ASSERTION(mpData == NULL, "mlRMMLScript::~mlRMMLScript");
	// если скрипт так и не скомпиляли
	if(mucFlags & SCRF_NOT_COMPILED){
		wchar_t* pwcScript = (wchar_t*)mjss;
		mjss = NULL;
		MP_DELETE_ARR( pwcScript);
	}
	// если скрипт так и не загрузили
	if(mbNeedToLoadScript && mjss!=NULL){
		wchar_t* pwcSrc = (wchar_t*)mjss;
		mjss = NULL;
		MP_DELETE_ARR( pwcSrc);
	}
	// (mucFlags & SCRF_REF)
	//	if(mjss!=NULL && !mbRef) JS_DestroyScript(mcx,mjss);
}

void mlRMMLScript::Activate(){
//if(isEqual((const char*)mpData, "enabled"))
//int h=0;
	if(SetMeAsEventListener()==ML_ERROR_INVALID_ARG){
		const wchar_t* pwcEvent = L"";
		mlString sPropName;
		if(!(mType & MLSTF_NOTSET)){
			pwcEvent = GetEventOrWatchName().c_str();
		}else{
//			if(mType == (MLST_WATCH | MLSTF_NOTSET)){
//				sPropName = cLPWCSTR((const char*)mpData);
//				pwcEvent = sPropName.c_str();
//			}else{
				pwcEvent = (const wchar_t*)mpData;
//			}
		}
		std::wstring fileSrc = GetSrcFileRef();
		const wchar_t* wcFileSrc = fileSrc.c_str();
		mlTraceError(mcx, "Event '%S' not found %S\n",pwcEvent,wcFileSrc);
	}
}

mlresult ParseSynchValue(JSContext* cx, wchar_t* apwc, SynchEventSender* pSESenderStruct){
	pSESenderStruct->eValueType=SynchEventSender::ms;
	pSESenderStruct->lValue=0;
	while(*apwc==L' ' || *apwc==L'\t') apwc++;
	wchar_t* pwc=apwc;
	while(*pwc>=L'0' && *pwc<=L'9'){
		pSESenderStruct->lValue=pSESenderStruct->lValue*10+(int)(*pwc-L'0');
		pwc++;
	}
	if(*pwc==L'\0' || *pwc==L'm' || *pwc==L'M'){
	}else if(*pwc==L's' || *pwc==L'S'){
		pSESenderStruct->eValueType=SynchEventSender::sec;
	}else if(*pwc==L'f' || *pwc==L'F'){
		pSESenderStruct->eValueType=SynchEventSender::frame;
	}else{
		mlTraceError(cx, "Unknown synchronize value type in '%s'\n",apwc);
		return ML_ERROR_INVALID_ARG;
	}
	return ML_OK;
}

class mlGetWaitingEventsMode
{
	mlSceneManager* mpSM;
public:
	mlGetWaitingEventsMode(mlRMMLElement* apSELMLEl){
		mpSM = GPSM(apSELMLEl->mcx);
		mpSM->mpvFiredEvents = NULL;
		mpSM->mpSELMLEl = apSELMLEl;
		mpSM->mbCheckingEvents = true;
	}
	~mlGetWaitingEventsMode(){
		mpSM->RestoreEventJSVals();
		mpSM->mbCheckingEvents = false;
		mpSM->mpSELMLEl = NULL;
	}
};

void mlRMMLScript::GetWaitingEvents(){
	mlString sExpr(((EventExpressionStruct*)mpData)->msExpr);
	mlString::size_type andpos = 0;
	while((andpos = sExpr.find(L"&&",andpos)) != mlString::npos){
		sExpr.replace(andpos, 2, L"||");
	}
	mlGetWaitingEventsMode oCWEM(this);
	jsval v;
	wr_JS_EvaluateUCScript(mcx, mpParent->mjso, sExpr.c_str(), sExpr.length(), 
		cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]), miSrcLine, &v);
}

mlresult mlRMMLScript::SetMeAsEventListener(){
	if(!(mType & MLSTF_NOTSET)) return ML_FALSE;
	ML_ASSERTION(mcx, mpData!=NULL,"mlRMMLScript::SetEventListener");
	//
//if(mpParent == NULL)
//int hh=0;
	ML_ASSERTION(mcx, mpParent!=NULL,"mlRMMLScript::SetEventListener");
	void* pSndrStruct=NULL;
	if(mType == (MLST_EVEXPR | MLSTF_NOTSET)){
		EventExpressionStruct* pEvExprStruct = MP_NEW( EventExpressionStruct);
		pEvExprStruct->msExpr = (const wchar_t*)mpData;
		pSndrStruct = pEvExprStruct;
	}else if(mType == (MLST_SYNCHMS | MLSTF_NOTSET)){
		wchar_t* pwc=(wchar_t*)mpData; while(*pwc==L' ' || *pwc==L'\t') pwc++;
		wchar_t* pwcColon=pwc;
		// если есть ':'
		for(; *pwcColon!=L'\0'; pwcColon++)
			if(*pwcColon==L':'){ if(pwcColon==pwc) pwc=++pwcColon; break;}
		mlRMMLElement* pSynchEvSender=mpParent;
		// и до ':' что-то есть
		if(*pwcColon==L':'){
			// то ищем этот элемент
			*pwcColon=L'\0';
			pSynchEvSender=mpParent->FindElemByName(pwc);
			*pwcColon=L':'; pwcColon++;
		}
		if(*pwcColon==L'\0') pwcColon=pwc;
		if(pSynchEvSender==NULL) return ML_ERROR_INVALID_ARG;
		if(!(pSynchEvSender->mRMMLType & MLMPT_CONTINUOUS || 
			pSynchEvSender->mRMMLType==MLMT_SPEECH)){
			mlTrace(mcx, "Event sender for synchronizing (%s) must be continuous or a speech\n",cLPCSTRq(pwc));
			return ML_ERROR_INVALID_ARG;
		}
		SynchEventSender* pSESenderStruct = MP_NEW( SynchEventSender);
		pSESenderStruct->pSender=pSynchEvSender;
		mlresult res;
		if((res = ParseSynchValue(mcx, pwcColon,pSESenderStruct))!=ML_OK) return res;
		pSynchEvSender->SetSynchEventListener(this,pSESenderStruct);
		pSndrStruct=pSESenderStruct;
	}else if(mType == (MLST_SYNCHPROP | MLSTF_NOTSET)){
		SynchPropEventSender* pSPSenderStruct = mpParent->GetSynchPropEventSender((wchar_t*)mpData);
		if(pSPSenderStruct==NULL) return ML_ERROR_INVALID_ARG;
		pSPSenderStruct->pSender->SetSynchPropEventListener(pSPSenderStruct->msSynchProp.c_str(),this);
		pSndrStruct = pSPSenderStruct;
	}else if(mType == (MLST_SYNCHEVENT | MLSTF_NOTSET)){
		SynchPropEventSender* pSPSenderStruct = mpParent->GetSynchPropEventSender((wchar_t*)mpData);
		if(pSPSenderStruct==NULL) return ML_ERROR_INVALID_ARG;
		pSPSenderStruct->pSender->SetSynchPropEventListener(pSPSenderStruct->msSynchProp.c_str(),this);
		pSndrStruct = pSPSenderStruct;
	}else{
		EventSender* pSenderStruct=mpParent->GetEventSender((wchar_t*)mpData);
		if(pSenderStruct==NULL) return ML_ERROR_INVALID_ARG;
		if(pSenderStruct->pSender < (mlRMMLElement*)0xFF){
			GPSM(mcx)->SetEventListener((char)(pSenderStruct->pSender),pSenderStruct->idEvent,this);
		}else{
			pSenderStruct->pSender->SetEventListener(pSenderStruct->idEvent,this);
		}
		pSndrStruct=pSenderStruct;
	}
	wchar_t *pData = (wchar_t*)mpData;
	MP_DELETE_ARR( pData);
	mpData=(void*)pSndrStruct;
	mType &= ~MLSTF_NOTSET;
	if(mType == MLST_EVEXPR){
		GetWaitingEvents();
	}
	return ML_OK;
}

mlresult mlRMMLScript::RemoveMeFromEventListeners(){
	if(mType & MLSTF_NOTSET) return ML_FALSE;
	ML_ASSERTION(mcx, mpData!=NULL,"mlRMMLScript::SetEventListener");
	//
	ML_ASSERTION(mcx, mpParent!=NULL,"mlRMMLScript::SetEventListener");
	if(mType == MLST_EVEXPR){
		EventExpressionStruct* pEvExprStruct = (EventExpressionStruct*)mpData;
		std::vector<EventSender>::iterator vi;
		for(vi = pEvExprStruct->mvWaitingEvents.begin(); vi != pEvExprStruct->mvWaitingEvents.end(); vi++){
			if((*vi).pSender < (mlRMMLElement*)0xFF){
				GPSM(mcx)->RemoveEventListener((char)((*vi).pSender),this);
			}else{
				(*vi).pSender->RemoveEventListener((*vi).idEvent,this);
			}
		}
		pEvExprStruct->mvWaitingEvents.clear();
	}else if(mType==MLST_SYNCHMS){
		SynchEventSender* pSESenderStruct=(SynchEventSender*)mpData;
		if(pSESenderStruct==NULL) return ML_ERROR_FAILURE;
		pSESenderStruct->pSender->RemoveSynchEventListener(this);
	}else if(mType==MLST_SYNCHPROP){
		SynchPropEventSender* pSPSenderStruct = (SynchPropEventSender*)mpData;
		if(pSPSenderStruct == NULL) return ML_ERROR_FAILURE;
		pSPSenderStruct->pSender->RemoveSynchPropEventListener(pSPSenderStruct->msSynchProp.c_str(), this);
	}else{
		EventSender* pSenderStruct=(EventSender*)mpData;
		if(pSenderStruct==NULL) return ML_ERROR_FAILURE;
		if(pSenderStruct->pSender < (mlRMMLElement*)0xFF){
			GPSM(mcx)->RemoveEventListener((char)(pSenderStruct->pSender),this);
		}else{
			pSenderStruct->pSender->RemoveEventListener(pSenderStruct->idEvent,this);
		}
	}
	return ML_OK;
}

mlresult mlRMMLScript::Load(const wchar_t* apwcSrc){
	res::resIResource* pRes=NULL;
	mlString msSrcFull = GPSM(mcx)->RefineResPathEl(this,apwcSrc);

	if(OMS_FAILED(GPSM(mcx)->GetContext()->mpResM->OpenResource(msSrcFull.c_str(),pRes))){
		mlTraceError(mcx, "Cannot open MJS-file (%S)\n",apwcSrc);
		return ML_ERROR_FILE_NOT_FOUND;
	}
//	ML_ASSERTION(pRes!=NULL,"mlRMMLScript::Load()");
	istream* pis=NULL;
	if(OMS_FAILED(pRes->GetIStream(pis))){
		pRes->Close();
		return ML_ERROR_FAILURE;
	}
//	ML_ASSERTION(pis!=NULL,"mlRMMLXML::Load()");
	string str;
	for(;;){
		int ic=pis->rdbuf()->sbumpc();
		if(ic==-1) 
			break;
		str.append(1,(char)ic);
	}
//	strstream ss;
//	while(!pis->eof()){
//		ss << pis->get();
//	}
//	pis->get(*ss.rdbuf(),'\0');
//{
//fstream fs("c:\\tmp.mjs",ios_base::out); 
//fs << ss.rdbuf();
//}
//	ss<<'\0';
	pRes->Close();
	//
	Compile(cLPWCSTR(str.c_str()),cLPCSTR(apwcSrc),1);
//	Compile((char*)str.c_str(),cLPCSTR(apwcSrc),1);
//	Compile(ss.rdbuf()->str());
//	ss.rdbuf()->freeze(false);
//	ss.clear();
	return ML_OK;
}

bool IsEventRef(const wchar_t* apwc){
	while(*apwc){
		switch(*apwc){
		case L' ':
		case L'\t':
			if(apwc[1]==L'\0')
				return true;
		case L',':
		case L':':
		case L';':
		case L'-':
		case L'+':
		case L'*':
		case L'|':
		case L'\'':
		case L'\"':
		case L'(':
		case L')':
		case L'[':
		case L']':
		case L'{':
		case L'}':
		case L'<':
		case L'>':
		case L'=':
		case L'/':
		case L'`':
		case L'~':
		case L'!':
		case L'@':
		case L'#':
		case L'$':
		case L'%':
		case L'^':
		case L'&':
		case L'\\':
		case L'?':
		case L'':
			return false;
		}
		apwc++;
	}
	return true;
}

mlresult mlRMMLScript::SetProperty(const wchar_t* apwcName, const wchar_t* apwcValue){
	if(isEqual(apwcName,L"event")){
		while(*apwcValue==L' ' || *apwcValue==L'\t') apwcValue++;
		bool bSynch=false;
		if(*apwcValue>=L'0' && *apwcValue<=L'9')
			bSynch=true;
		for(const wchar_t* pwc=apwcValue; *pwc!=L'\0'; pwc++){
			if(*pwc==L':'){
				while(*pwc==L' ' || *pwc==L'\t') pwc++;
				if(*pwc>=L'0' && *pwc<=L'9')
					bSynch=true;
				break;
			}
		}
		if(bSynch){
			mType = MLST_SYNCHMS | MLSTF_NOTSET;
		}else{
			if(IsEventRef(apwcValue)){
				mType = MLST_EVENT | MLSTF_NOTSET;
				mlString sEventVal = apwcValue;
				mlString::size_type pos = sEventVal.find(L"synch");
				if(pos != mlString::npos){
					if(pos == 0 || sEventVal[pos - 1] == L'.'){
						if((pos + 5) == sEventVal.size() || sEventVal[pos + 5] == L'.'){
							mType = MLST_SYNCHEVENT | MLSTF_NOTSET;
						}
					}
				}
			}else
				mType = MLST_EVEXPR | MLSTF_NOTSET;
		}
	}else if(isEqual(apwcName,L"watch")){
		mType = MLST_WATCH | MLSTF_NOTSET;
		// если есть ".synch.", значит это свойство подобъекта synch
		mlString sWatchVal = apwcValue;
		mlString::size_type pos = sWatchVal.find(L"synch");
		if(pos != mlString::npos){
			if(pos == 0 || sWatchVal[pos - 1] == L'.'){
				if((pos + 5) == sWatchVal.size() || sWatchVal[pos + 5] == L'.'){
					mType = MLST_SYNCHPROP | MLSTF_NOTSET;
				}
			}
		}
	}else if(isEqual(apwcName,L"name")){
	}else if(isEqual(apwcName,L"src")){
		int bufferLength = wcslen(apwcValue)+1;
		wchar_t* pwcSrc = MP_NEW_ARR( wchar_t, bufferLength);
		//wcscpy(pwcSrc,apwcValue);
		rmmlsafe_wcscpy(pwcSrc, bufferLength, 0, apwcValue);
		mjss=(JSScript*)(void*)pwcSrc;
		mbNeedToLoadScript=true; // load and compile later
		return ML_OK;
	}else if(isEqual(apwcName,L"synch")){
		if(mType != MLMT_SCRIPT){
			mlTraceError(mcx, "Only onLoad-script can be synchronized (%S)",GetSrcFileRef().c_str());
			return ML_ERROR_FAILURE;
		}
		mType = MLST_SYNCH | MLSTF_NOTSET;
	}else {
		return ML_ERROR_INVALID_ARG;
	}
	int bufferLength = wcslen(apwcValue)+1;
	wchar_t* pwc = MP_NEW_ARR( wchar_t, bufferLength);
	//wcscpy(pwc,apwcValue);
	rmmlsafe_wcscpy(pwc, bufferLength, 0, apwcValue);
	mpData=(void*) pwc;
	return ML_OK;
}

__int64 giAvatOnSST = 0;

// by JSScript
void mlRMMLScript::Compile(const wchar_t* apwcData, const char* apszSrcName, int aiLine){
	if(mpParent==NULL) return;
	if(apszSrcName==NULL) apszSrcName="_script_";

#if (defined PROFILE_RMML_EXECUTING || defined PROFILE_RMML_EXECUTING_SC)
	SetName(apszSrcName);
	assert( aiLine < 0xFFFF);
	m_line = (unsigned short) aiLine;
#ifdef PROFILE_RMML_EXECUTING
	std::string m_srcName = apszSrcName;
	int t1 = tp_GetMicroTickCount();
#endif
#endif

	mjss=JS_CompileUCScript(mcx, mpParent->mjso,(const jschar*)apwcData,
		wcslen(apwcData), apszSrcName, aiLine);
	if(mjss!=NULL){
		// защищаем от сборщика мусора
		JSObject* jsso=JS_NewScriptObject(mcx, mjss);
		jsval v=OBJECT_TO_JSVAL(jsso);
		jschar* pjsch=(jschar*)(void*)(&mjss);
		JSBool bR=JS_SetUCProperty(mcx, mpParent->mjso, pjsch, sizeof(void*)/sizeof(jschar), &v);
//jsval v2;
//JSBool bR2=JS_GetUCProperty(mcx, mpParent->mjso, pjsch, sizeof(void*)/sizeof(jschar), &v2);
//JSFunction* jsf=JS_ValueToFunction(mcx,v2);
//int hh=0;
	}else{
		mlTraceError(mcx, "Error: script is not compiled (%s line %d)\n",apszSrcName,aiLine);
	}

#ifdef PROFILE_RMML_EXECUTING
	int t2 = tp_GetMicroTickCount();
	GPSM(mcx)->GetContext()->mpLogWriter->WriteLn("Compile ", m_srcName, " line ", m_line, " in ", t2 - t1, " ms  giAvatOnSST:", (int)(t2-giAvatOnSST));
#endif
}

void mlRMMLScript::SaveToCompileLater(const wchar_t* apwcData, const char* apszSrcName, int aiLine){
	size_t iLen = wcslen(apwcData);
	wchar_t* buf = MP_NEW_ARR( wchar_t, iLen+2);
	//wcscpy(buf, apwcData);
	rmmlsafe_wcscpy(buf, iLen+2, 0, apwcData);
	mjss = (JSScript*)buf;
	mucFlags |= SCRF_NOT_COMPILED;
}

// скомпилировать скрипт, если он еще не скомпилирован
void mlRMMLScript::Compile(){
	if(mucFlags & SCRF_NOT_COMPILED){
		wchar_t* pwcScript = (wchar_t*)mjss;
		mjss = NULL;
		mucFlags &= ~SCRF_NOT_COMPILED;
		Compile(pwcScript, cLPCSTR(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]), miSrcLine);
		MP_DELETE_ARR( pwcScript);
	}
}

void mlRMMLScript::Compile(const char* apszData, const char* apszSrcName, int aiLine){
	if(mpParent==NULL) return;
	if(apszSrcName==NULL) apszSrcName="_script_";

#if (defined PROFILE_RMML_EXECUTING || defined PROFILE_RMML_EXECUTING_SC)
	//m_srcName = apszSrcName;
	SetName(apszSrcName);
	assert( aiLine < 0xFFFF);
	m_line = (unsigned short)aiLine;


#ifdef PROFILE_RMML_EXECUTING
	GPSM(mcx)->GetContext()->mpLogWriter->WriteLn("Compile ", GetName(), " line ", m_line);
#endif
#endif

	mjss=JS_CompileScript(mcx, mpParent->mjso, apszData, strlen(apszData), apszSrcName, aiLine);
	if(mjss!=NULL){
		// защищаем от сборщика мусора
		JSObject* jsso=JS_NewScriptObject(mcx, mjss);
		jsval v=OBJECT_TO_JSVAL(jsso);
		jschar* pjsch=(jschar*)(void*)(&mjss);
		JSBool bR=JS_SetUCProperty(mcx, mpParent->mjso, pjsch, sizeof(void*)/sizeof(jschar), &v);
	}else{
		mlTraceError(mcx, "Error: script is not compiled (%s line %d)\n",apszSrcName,aiLine);
	}
}

/* by JSFunction
void mlRMMLScript::Compile(wchar_t* apwcData){
	if(mpParent==NULL) return;
//JSScript* jss=JS_CompileUCScript(mcx, mpParent->mjso,(const jschar*)apwcData,
//	wcslen(apwcData), "_script_", 1);
////JSObject* jso=JS_NewScriptObject(mcx, jss);
	mjss=JS_CompileUCFunction(mcx, mpParent->mjso,
    "fun", 0, NULL,
    (const jschar*)apwcData, wcslen(apwcData), "_script_", 1);
	if(mjss!=NULL){
//		JSFunction* jsf=JS_ValueToFunction(mcx, vFunc);
//jsval vv;
//jsval argv;
//JSBool bRR=JS_CallFunction(mcx, mpParent->mjso, mjss, 0, &argv, &vv);
		// защищаем от сборщика мусора
		jsval v=OBJECT_TO_JSVAL(JS_GetFunctionObject(mjss));
//		jsval v=OBJECT_TO_JSVAL(mjss);
//JSFunction* jsf=JS_ValueToFunction(mcx,v);
////		jsval v;
////		JSBool bRRR=JS_ConvertValue(mcx, jsval v, JSType type,
////			jsval *vp);
//
		jschar* pjsch=(jschar*)(void*)(&mjss);
		JSBool bR=JS_SetUCProperty(mcx, mpParent->mjso, pjsch, sizeof(void*)/sizeof(jschar), &v);
//jsval v2;
//JSBool bR2=JS_GetUCProperty(mcx, mpParent->mjso, pjsch, sizeof(void*)/sizeof(jschar), &v2);
//JSFunction* jsf=JS_ValueToFunction(mcx,v2);
//int hh=0;
	}
}
*/

mlresult mlRMMLScript::SetValue(wchar_t* apwcValue){
	if(mbNeedToLoadScript && mjss!=NULL){
		wchar_t* pwcSrc=(wchar_t*)mjss;
		mjss=NULL;
		Load(pwcSrc);
		MP_DELETE_ARR( pwcSrc);
		mbNeedToLoadScript=false;
	}
	const wchar_t* pwcSrcName=L"_script_";
	if(miSrcFilesIdx > 0)
		pwcSrcName = GPSM(mcx)->mSrcFileColl[miSrcFilesIdx];
	wchar_t* pwcValue0 = apwcValue;
	int iStartLine=1;
	if(miSrcLine > 0) iStartLine=miSrcLine;
	for(; *apwcValue != L'\0'; apwcValue++){
		switch(*apwcValue){
		case L'\n':
			iStartLine++;
		case L' ':
		case L'\t':
		case L'\r':
			continue;
		}
		break;
	}
	if(*apwcValue == L'\0')
		return ML_OK;
	apwcValue = pwcValue0;
	SaveToCompileLater(apwcValue,cLPCSTR(pwcSrcName),iStartLine);
	//Compile(apwcValue,cLPCSTR(pwcSrcName),iStartLine);
	return ML_OK;
}

struct SFuncProp{
	const jschar* name;
	jsval val;
	SFuncProp(const jschar* aName, jsval aVal){
		name = aName;
		val = aVal;
	}
};

#define SUPER_PREFIX L"super_" 
#define SUPER_PREFIX_LEN (sizeof(SUPER_PREFIX)/sizeof(jschar)-1)

mlresult mlRMMLScript::Execute(jsval* apjsvResult){
	if(mbNeedToLoadScript && mjss!=NULL){
		wchar_t* pwcSrc = (wchar_t*)mjss;
		mjss = NULL;
		Load(pwcSrc);
		MP_DELETE_ARR( pwcSrc);
		mbNeedToLoadScript=false;
	}
	if(mucFlags & SCRF_NOT_COMPILED)
		Compile();
	if(mpParent==NULL || mjss==NULL) 
	{
		return ML_ERROR_NOT_INITIALIZED;
	}
		
#ifdef PROFILE_RMML_EXECUTING
	//int uiAllocatedBytes = JS_GetGCParameter(JS_GetRuntime(mcx), JSGC_BYTES);
	__int64 time1 = tp_GetMicroTickCount();
	GPSM(mcx)->GetContext()->mpLogWriter->WriteLn("Executing ", GetName(), " at line ", m_line, " microseconds giAvatOnSST:",(int)(time1 - giAvatOnSST));
	if(m_line == 863){
		giAvatOnSST = time1;
	}
#endif

#ifdef PROFILE_RMML_EXECUTING_SC
	GPSM(mcx)->GetContext()->mpLogWriterLastMsgAsync->WriteLn("Executing ", GetName(), " at line ", m_line);
#endif

#ifdef ENABLE_OBJECT_STAT
	GPSM(mcx)->SetCurrentExecutingScriptParams( GetName(), (int)m_line);
#endif
	GPSM(mcx)->SetCurrentExecutingScriptLine( (int)m_line);
	

//{mlPoint pnt=mpParent->GetIVisible()->GetAbsXY();
//TRACE("%d\n",pnt.x);}
	// если это скрипт класса или это скрипт с синхронизируемыми методами,
	// то запомнить все методы, которые уже есть
	std::vector<SFuncProp> vFuncNames;
	if(mpClass != NULL || mType == (MLST_SYNCH | MLSTF_NOTSET)){
		JSIdArray* idArr=JS_Enumerate(mcx, mpParent->mjso);
		for(int ij=0; ij<idArr->length; ij++){
			jsid id=idArr->vector[ij];
			jsval v;
			if(!JS_IdToValue(mcx,id,&v)) continue;
			if(!JSVAL_IS_STRING(v)) continue;
			JSString* jss=JSVAL_TO_STRING(v);
			jschar* pjsch=JS_GetStringChars(jss);
			size_t jssLen=JS_GetStringLength(jss);
			jsval vProp;
			JS_GetUCProperty(mcx, mpParent->mjso, pjsch, jssLen, &vProp);
			if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
			// если SAVE_FROM_GC-свойство (script)
			if(jssLen==2 && ML_JS_GETCLASS(mcx, JSVAL_TO_OBJECT(vProp))->flags==1) continue;
			if(JS_TypeOfValue(mcx,vProp)!=JSTYPE_FUNCTION) continue;
			vFuncNames.push_back(SFuncProp(pjsch, vProp));
		}
		JS_DestroyIdArray(mcx, idArr);
	}
	// 
	JSObject* jsoContext=mpParent->mjso;
	if(mpParent->mRMMLType==MLMT_SEQUENCER){
		mlRMMLElement* pParentParent=mpParent->mpParent;
		if(pParentParent==NULL) 
		{
#ifdef PROFILE_RMML_EXECUTING
	int uiAllocatedBytes = JS_GetGCParameter(JS_GetRuntime(mcx), JSGC_BYTES);
	__int64 time2 = tp_GetMicroTickCount();
	GPSM(mcx)->GetContext()->mpLogWriter->WriteLn("Executed ", GetName(), " at line ", m_line, " in ", (int)(time2 - time1), " microseconds  Gsize:",uiAllocatedBytes);
#endif
#ifdef PROFILE_RMML_EXECUTING_SC
	GPSM(mcx)->GetContext()->mpLogWriterLastMsgAsync->WriteLn("Executed ", GetName(), " at line ", m_line);
#endif
			return ML_ERROR_FAILURE;
		}
		jsoContext=pParentParent->mjso;
		jsval vSeq = OBJECT_TO_JSVAL(mpParent->mjso);
		wr_JS_SetUCProperty(mcx, jsoContext, L"this_seq", -1, &vSeq);
	}
	GPSM(mcx)->mbWasExitFromScript=false;
	GPSM(mcx)->mjsvalExit=JSVAL_NULL;
//mlString sScript = GetValue(0);
	jsval v;
#ifdef PROFILE_RMML_EXECUTING
	__int64 time4 = tp_GetMicroTickCount();
#endif
	GPSM(mcx)->JSScriptExecutionStarted((void*)this);
	if(!JS_ExecuteScript(mcx, jsoContext,mjss,&v)){
		if(!GPSM(mcx)->mbWasExitFromScript){
#ifdef PROFILE_RMML_EXECUTING
	int uiAllocatedBytes = JS_GetGCParameter(JS_GetRuntime(mcx), JSGC_BYTES);
	__int64 time2 = tp_GetMicroTickCount();
	GPSM(mcx)->GetContext()->mpLogWriter->WriteLn("Executed ", GetName(), " at line ", m_line, " in ", (int)(time2 - time1), " microseconds  Gsize:",uiAllocatedBytes);
#endif
#ifdef PROFILE_RMML_EXECUTING_SC
	GPSM(mcx)->GetContext()->mpLogWriterLastMsgAsync->WriteLn("Executed ", GetName(), " at line ", m_line);
#endif

#ifdef ENABLE_OBJECT_STAT
	GPSM(mcx)->SetCurrentExecutingScriptParams( "", 0);
#endif
	GPSM(mcx)->SetCurrentExecutingScriptLine( -1);
			GPSM(mcx)->JSScriptExecuted((void*)this);
			return ML_ERROR_FAILURE;
		}
		GPSM(mcx)->mbWasExitFromScript=false;
		v = GPSM(mcx)->mjsvalExit;
	}
	GPSM(mcx)->JSScriptExecuted((void*)this);
#ifdef PROFILE_RMML_EXECUTING
	__int64 time5 = tp_GetMicroTickCount();
	GPSM(mcx)->GetContext()->mpLogWriter->WriteLn("Executed JS ", GetName(), " at line ", m_line, " in ", (int)(time5 - time4), " microseconds ");
#endif
#ifdef PROFILE_RMML_EXECUTING_SC
	GPSM(mcx)->GetContext()->mpLogWriterLastMsgAsync->WriteLn("Executed JS ", GetName(), " at line ", m_line);
#endif
	if(mpParent->mRMMLType==MLMT_SEQUENCER){
		JS_DeleteProperty(mcx, jsoContext, "this_seq");
	}
	// если это скрипт с синхронизируемыми методами
	if(mType == (MLST_SYNCH | MLSTF_NOTSET)){
		ML_ASSERTION(mcx, mpData!=NULL, "mlRMMLScript::Execute");
		JSString* jssSP = JS_NewUCStringCopyZ(mcx, (const jschar*)mpData);
		jsval vSPScript = STRING_TO_JSVAL(jssSP);
		// то устанавливаем вновь созданным методам свойство synch
		JSIdArray* idArr=JS_Enumerate(mcx, mpParent->mjso);
		for(int ij=0; ij<idArr->length; ij++){
			jsid id=idArr->vector[ij];
			jsval v;
			if(!JS_IdToValue(mcx,id,&v)) continue;
			if(!JSVAL_IS_STRING(v)) continue;
			JSString* jss=JSVAL_TO_STRING(v);
			wchar_t* pjsch=wr_JS_GetStringChars(jss);
			size_t jssLen=JS_GetStringLength(jss);
			jsval vProp;
			wr_JS_GetUCProperty(mcx, mpParent->mjso, pjsch, jssLen, &vProp);
			if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
			// если SAVE_FROM_GC-свойство (script)
			if(jssLen==2 && ML_JS_GETCLASS(mcx, JSVAL_TO_OBJECT(vProp))->flags==1) 
				continue; // то не ищем
			if(JS_TypeOfValue(mcx,vProp)!=JSTYPE_FUNCTION) continue;
			jsval vSPClassMethod = JSVAL_NULL;
			std::vector<SFuncProp>::iterator vi;
			for(vi=vFuncNames.begin(); vi!=vFuncNames.end(); vi++){
				if(isEqual( (const wchar_t *) (*vi).name,pjsch)){
					if((*vi).val == vProp)
						break;
					// если метод уже был унаследован от класса, 
					// то запоминаем значение его свойства synch
					wr_JS_GetUCProperty(mcx, JSVAL_TO_OBJECT((*vi).val), L"synch", -1, &vSPClassMethod);
					vi = vFuncNames.end();
					break;
				}
			}
			if(vi != vFuncNames.end()) continue;
			// Метода не было или перекрылся унаследованный метод
			jsval vSP = vSPScript;
			if(vSPClassMethod != JSVAL_NULL && JSVAL_IS_STRING(vSPClassMethod)){
				// объединяем свойства, унаследованные от класса со свойствами, указанными в атрибуте synch
				mlString sSPClassMethod = wr_JS_GetStringChars(JSVAL_TO_STRING(vSPClassMethod));
				mlString sSPScript = wr_JS_GetStringChars(JSVAL_TO_STRING(vSPScript));
				// ?? надо объединять с удалением повторений и с минусами
				mlString sSP = sSPClassMethod + L" " + sSPScript;
				vSP = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx,sSP.c_str()));
			}
			JSObject* jsoMethod = JSVAL_TO_OBJECT(vProp);
			wr_JS_SetUCProperty(mcx, jsoMethod, L"synch", -1, &vSP);
//TRACE("func:%S", pjsch);
		}
		JS_DestroyIdArray(mcx, idArr);
		//
		if(mpData!=NULL){
			wchar_t* pwc=(wchar_t*)mpData;
			MP_DELETE_ARR( pwc);
			mpData = NULL;
		}
		mType = MLST_SYNCH;
	}
	// если это скрипт класса, то делаем копию всех вновь созданных методов с префиксом "super_"
	if(mpClass != NULL){
		JSIdArray* idArr=JS_Enumerate(mcx, mpParent->mjso);
		for(int ij=0; ij<idArr->length; ij++){
			jsid id=idArr->vector[ij];
			jsval v;
			if(!JS_IdToValue(mcx,id,&v)) continue;
			if(!JSVAL_IS_STRING(v)) continue;
			JSString* jss=JSVAL_TO_STRING(v);
			wchar_t* pjsch=wr_JS_GetStringChars(jss);
			size_t jssLen=JS_GetStringLength(jss);
			jsval vProp;
			wr_JS_GetUCProperty(mcx, mpParent->mjso, pjsch, jssLen, &vProp);
			if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
			// если SAVE_FROM_GC-свойство (script)
			if(jssLen==2 && ML_JS_GETCLASS(mcx, JSVAL_TO_OBJECT(vProp))->flags==1) 
				continue; // то не ищем
			if(JS_TypeOfValue(mcx,vProp)!=JSTYPE_FUNCTION) continue;
			// если начинается на "super_", то тоже не ищем
//				if(jssLen > SUPER_PREFIX_LEN){
//					if(isEqual(pjsch, SUPER_PREFIX, SUPER_PREFIX_LEN))
//						continue;
//				}
			std::vector<SFuncProp>::iterator vi;
			for(vi=vFuncNames.begin(); vi!=vFuncNames.end(); vi++){
				if(isEqual( (const wchar_t *) (*vi).name,pjsch)){
					if((*vi).val == vProp)
						break;
					vi = vFuncNames.end(); // перекрылся метод, унаследованный от класса0
					break;
				}
			}
			if(vi != vFuncNames.end()) continue;
			// Метода не было или перекрылся унаследованный метод
			// Создаем копию метода класса
			mlString sFuncName;
//						for(int ii=0; ii<(int)mpClass; ii++){
//							sFuncName+=SUPER_PEFIX;
//						}
			if(mpClass->_name != NULL){
				wchar_t* jscName = wr_JS_GetStringChars(mpClass->_name);
				if(jscName != NULL)
					sFuncName += (wchar_t *) jscName;
				sFuncName+=L'_';
				sFuncName+=pjsch;
				wr_JS_SetUCProperty(mcx, mpParent->mjso, sFuncName.c_str(), -1, &vProp);
			}
//TRACE("func:%S", pjsch);
//if(mpParent->_name != NULL)
//TRACE(" | mpParent:%S", mpParent->mpName);
//if(mpParent->mpClass != NULL && mpParent->mpClass->_name != NULL)
//TRACE(" | mpParent->mpClass:%S", mpParent->mpClass->mpName);
//else 
//TRACE(" | mpParent->mpClass:NULL");
//if(mpClass->_name != NULL)
//TRACE(" | mpClass:%S", mpClass->mpName);
//TRACE("\n");
			if(mpClass == mpParent->mpClass){
				sFuncName = SUPER_PREFIX;
				sFuncName += pjsch;	
				wr_JS_SetUCProperty(mcx, mpParent->mjso, sFuncName.c_str(), -1, &vProp);
			}
		}
		JS_DestroyIdArray(mcx, idArr);
	}
/* by JSFunction
	jsval argv;
//jschar* pjsch=(jschar*)(void*)(&mjss);
//jsval v2;
//JSBool bR2=JS_GetUCProperty(mcx, mpParent->mjso, pjsch, sizeof(void*)/sizeof(jschar), &v2);
////JSFunction* jsf=JS_ValueToFunction(mcx,v2);
//if(JS_ValueToFunction(mcx,v2)!=mjss)
//int hh=0;
	if(!JS_CallFunction(mcx, jsoContext, mjss, 0, &argv, &v)) return ML_ERROR_FAILURE;
*/
	if(apjsvResult!=NULL) *apjsvResult=v;

#ifdef PROFILE_RMML_EXECUTING
	__int64 time2 = tp_GetMicroTickCount();
	GPSM(mcx)->GetContext()->mpLogWriter->WriteLn("Executed ", GetName(), " at line ", m_line, " in ", (int)(time2 - time1), " microseconds   giAvatOnSST:",(int)(time1 - giAvatOnSST));
#endif
#ifdef PROFILE_RMML_EXECUTING_SC
	GPSM(mcx)->GetContext()->mpLogWriterLastMsgAsync->WriteLn("Executed ", GetName(), " at line ", m_line);
#endif
#ifdef ENABLE_OBJECT_STAT
	GPSM(mcx)->SetCurrentExecutingScriptParams( "", 0);
#endif
	GPSM(mcx)->SetCurrentExecutingScriptLine( -1);

//{mlPoint pnt=mpParent->GetIVisible()->GetAbsXY();
//TRACE("%d\n",pnt.x);}
	return ML_OK;
}

mlRMMLElement* mlRMMLScript::Duplicate(mlRMMLElement* apNewParent){
	Compile();
	mlRMMLScript* pNewScript = MP_NEW( mlRMMLScript);
	pNewScript->mcx = mcx;
	pNewScript->SetParent(apNewParent);
//	pNewScript->mpParent=apNewParent;
	pNewScript->mpData = NULL;
	pNewScript->mjss = mjss; // ?? may need to copy JS-script
	//pNewScript->mbRef=true;
	pNewScript->mucFlags |= SCRF_REF;
	pNewScript->mType=mType;
	pNewScript->mbNeedToLoadScript=mbNeedToLoadScript;
	pNewScript->SetSrcFileIdx(miSrcFilesIdx, miSrcLine);

#if (defined PROFILE_RMML_EXECUTING || defined PROFILE_RMML_EXECUTING_SC)
	pNewScript->m_srcID = m_srcID;
	pNewScript->m_line = m_line;
#endif


	if(mbNeedToLoadScript){
		int bufferLength = wcslen((wchar_t*)mjss)+1;
		wchar_t* pwcSrc = MP_NEW_ARR( wchar_t, bufferLength);
		//wcscpy(pwcSrc,(wchar_t*)mjss);
		rmmlsafe_wcscpy(pwcSrc, bufferLength, 0, (wchar_t*)mjss);
		pNewScript->mjss=(JSScript*)(void*)pwcSrc;
	}else{
		if(apNewParent != NULL && mjss != NULL){ // защищаем от сборщика мусора на случай если класс будет удален
			JSObject* jsso=JS_GetScriptObject(pNewScript->mjss);
			if(jsso == NULL)
				jsso=JS_NewScriptObject(mcx, pNewScript->mjss);
			SAVE_FROM_GC(mcx, apNewParent->mjso, jsso)
		}
	}
	if(mType & MLSTF_NOTSET){
		pNewScript->mpData=(void*)CopyWStr((wchar_t*)mpData);
	}else if(mType==MLST_EVENT || mType==MLST_WATCH || mType==MLST_SYNCHMS || mType==MLST_EVEXPR){
		pNewScript->mType |= MLSTF_NOTSET;
		pNewScript->mpData=(void*)CopyWStr(GetEventOrWatchName().c_str());
	}
	return pNewScript;
}

void mlRMMLScript::YouAreSetAsEventListener(char aidEvent, mlRMMLElement* apSender){ 
	if(mType != MLST_EVEXPR) return;
	EventExpressionStruct* pEvExprStruct = (EventExpressionStruct*)mpData;
	if(GPSM(mcx)->mpvFiredEvents == NULL && GPSM(mcx)->mpSELMLEl != NULL)
		pEvExprStruct->mvWaitingEvents.push_back(EventSender(aidEvent, apSender));
}

class mlCheckEventsMode
{	
	mlSceneManager* mpSM;
public:
	mlCheckEventsMode(std::vector<EventSender>* pvFiredEvents, mlRMMLElement* apSELMLEl){
		mpSM = GPSM(apSELMLEl->mcx);
		mpSM->mpvFiredEvents = pvFiredEvents;
		mpSM->mpSELMLEl = apSELMLEl;
		mpSM->mbCheckingEvents = true;
	}
	~mlCheckEventsMode(){
		mpSM->RestoreEventJSVals();
		mpSM->mbCheckingEvents = false;
		mpSM->mpvFiredEvents = NULL;
		mpSM->mpSELMLEl = NULL;
	}
};

bool mlRMMLScript::CheckEventExpr(EventExpressionStruct* apEvExprStruct){
	if(apEvExprStruct==NULL) return false;
	mlCheckEventsMode oCheckEventMode(&(apEvExprStruct->mvFiredEvents), this);
	jsval v;
	JSBool bR=wr_JS_EvaluateUCScript(mcx, mpParent->mjso, 
		apEvExprStruct->msExpr.c_str(), apEvExprStruct->msExpr.size(), 
		cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]), miSrcLine, &v);
	if(!bR){
		mlTraceError(mcx, "Script \'%S\' cannot evaluate \'%S\'\n",GetName(),apEvExprStruct->msExpr.c_str());
		return false;
	}
	if(v==JSVAL_VOID || v==JSVAL_NULL) return false;
	if(JSVAL_IS_BOOLEAN(v)){
		if(v==JSVAL_FALSE) return false;
	}else if(JSVAL_IS_INT(v)){
		int iVal=JSVAL_TO_INT(v);
		if(iVal==0) return false;
	}else if(JSVAL_IS_STRING(v)){
		JSString* jss=JSVAL_TO_STRING(v);
		if(JS_GetStringLength(jss)==0) return false;
	}else if(JSVAL_IS_DOUBLE(v)){
		jsdouble dVal=(*JSVAL_TO_DOUBLE(v));
		if(dVal==0.0) return false;
	}
	return true; // may execute
}

bool mlRMMLScript::EventNotify(char aidEvent, mlRMMLElement* apSender){
	if(mType == MLST_EVEXPR){
		if(mpData == NULL) return false;
		EventExpressionStruct* pEvExprStruct = (EventExpressionStruct*)mpData;
		pEvExprStruct->mvFiredEvents.push_back(EventSender(aidEvent, apSender));
		if(!CheckEventExpr(pEvExprStruct))
			return false;
		// если выражение == true,
		// то выполняем и ждем снова
		pEvExprStruct->mvFiredEvents.clear();
	}
//	bool bDontSendSynchEvents = gpSM->mbDontSendSynchEvents;
//	if(mpParent != NULL && mpParent->mchSynchronized == 2){
//		gpSM->mbDontSendSynchEvents = true;
//	}
	Execute();
//	gpSM->mbDontSendSynchEvents = bDontSendSynchEvents;
	return false;
}

void mlRMMLScript::EventSenderDestroyed(mlRMMLElement* apSender){
	if(mType & MLSTF_NOTSET) return;
	if(mType == MLST_EVEXPR){
		EventExpressionStruct* pEvExprStruct = (EventExpressionStruct*)mpData;
		std::vector<EventSender>::iterator vi;
		for(vi = pEvExprStruct->mvWaitingEvents.begin(); vi != pEvExprStruct->mvWaitingEvents.end(); ){
			if((*vi).pSender == apSender)
				pEvExprStruct->mvWaitingEvents.erase(vi);
			else
				vi++;
		}
	}else if(mType==MLST_SYNCHMS){
		SynchEventSender* pSESenderStruct=(SynchEventSender*)mpData;
		if(pSESenderStruct==NULL) return;
		if(pSESenderStruct->pSender==apSender){
			MP_DELETE( pSESenderStruct); pSESenderStruct=NULL;
			mpData=NULL;
		}
	}else if(mType==MLST_SYNCHPROP){
		SynchPropEventSender* pSPSenderStruct = (SynchPropEventSender*)mpData;
		if(pSPSenderStruct == NULL) return;
		if(pSPSenderStruct->pSender == apSender){
			MP_DELETE( pSPSenderStruct); pSPSenderStruct = NULL;
			mpData=NULL;
		}
	}else{
		EventSender* pSenderStruct=(EventSender*)mpData;
		if(pSenderStruct==NULL) return;
		if(pSenderStruct->pSender==apSender){
			MP_DELETE( pSenderStruct); pSenderStruct=NULL;
			mpData=NULL;
		}
	}
}

mlString mlRMMLScript::GetEventOrWatchName(){
	mlString wsEWName=L"???";
	if(mType & MLSTF_NOTSET){
		wsEWName=(wchar_t*)mpData;
	}else{
		mlRMMLElement* pMLSender=NULL;
		if(mType==MLST_EVEXPR){
			EventExpressionStruct* pEvExprStruct = (EventExpressionStruct*)mpData;
			return pEvExprStruct->msExpr;
		}else if(mType==MLST_SYNCHMS){
			SynchEventSender* pSenderStruct=(SynchEventSender*)mpData;
			if(pSenderStruct==NULL) return wsEWName; // "???"
			pMLSender=pSenderStruct->pSender;
		}else{
			EventSender* pSenderStruct=(EventSender*)mpData;
			if(pSenderStruct==NULL) return wsEWName; // "???"
			if(pSenderStruct->pSender < (mlRMMLElement*)0xFF)
				return GPSM(mcx)->GetEventName((char)(pSenderStruct->pSender),pSenderStruct->idEvent);
			pMLSender=pSenderStruct->pSender;
			if(pMLSender==mpParent){
				return pMLSender->GetEventName(pSenderStruct->idEvent);
			}
		}
		// находим ближайшего общего предка 
		std::vector<mlRMMLElement*> vSParents; // предки источника событий
		mlRMMLElement* pParent=pMLSender;
		while(pParent!=NULL){
			vSParents.insert(vSParents.begin(),pParent);
			pParent=pParent->mpParent;
		}
		std::vector<mlRMMLElement*> vLParents; // мои предки
		pParent=mpParent;
		while(pParent!=NULL){
			vLParents.insert(vLParents.begin(),pParent);
			pParent=pParent->mpParent;
		}
		if(vSParents.size()==0 || vLParents.size()==0) 
			return wsEWName;
		if(vSParents[0]!=vLParents[0]) 
			return wsEWName;

		int iIdx = 0;
		for(iIdx=0; vSParents[iIdx]==vLParents[iIdx]; iIdx++);
		iIdx--;
		pParent=vSParents[iIdx]; // ближайший общий предок
		mlString wsPath;
		int iIdx2=vLParents.size()-1;
		while(vLParents[iIdx2] != pParent){
			wsPath+=L"_parent";
			iIdx2--;
		}
		iIdx++;
		while(iIdx < (int)vSParents.size()){
			wchar_t* wcsName=vSParents[iIdx]->GetName();
			if(wcsName==NULL)
				wsPath+=L"???";
			else
				wsPath+=wcsName;
			wsPath+=L'.';
			iIdx++;
		}
		if(mType==MLST_SYNCHMS){
			// удаляем послежнюю точку (если он есть)
			if(*wsPath.end()==L'.'){
				*wsPath.end()=L'\0';
			}
			SynchEventSender* pSESenderStruct=(SynchEventSender*)mpData;
			wsPath+=L':';
			wchar_t* pwcCP = MP_NEW_ARR( wchar_t, 20);
			swprintf_s(pwcCP, 20, L"%d",pSESenderStruct->lValue);
			wsPath+=pwcCP;
			if(pSESenderStruct->eValueType==SynchEventSender::ms){
				wsPath+=L"ms";
			}else if(pSESenderStruct->eValueType==SynchEventSender::sec){
				wsPath+=L"sec";
			}else if(pSESenderStruct->eValueType==SynchEventSender::frame){
				wsPath+=L"fr";
			}
		}else if(mType==MLST_SYNCHPROP){
			SynchPropEventSender* pSPSenderStruct = (SynchPropEventSender*)mpData;
			wsPath += pSPSenderStruct->msSynchProp;
		}else{
			EventSender* pSenderStruct=(EventSender*)mpData;
			wsPath += pMLSender->GetEventName(pSenderStruct->idEvent);
		}
		return wsPath;
	}
	return wsEWName;
}

mlString mlRMMLScript::GetScriptName(){
	if(mpData == NULL || mType != MLMT_SCRIPT) return L"";
	return (const wchar_t*)mpData;
}

mlString mlRMMLScript::GetValue(int aiIndent){
	if(mjss==NULL) return L""; 
	JSString* jss=JS_DecompileScript(mcx, mjss, "script", aiIndent);
	if(jss==NULL) return L"";
	return wr_JS_GetStringChars(jss);
}

std::string mlRMMLScript::GetName(){
	return  CFileNameStorage::GetInstance()->GetFileNameById(m_srcID);
}

void mlRMMLScript::SetName(std::string fileName){
	m_srcID = CFileNameStorage::GetInstance()->AddFileName(fileName);
}

// получить размер буфера в байтах, в котором хранится нескомпилированный скрипт
int mlRMMLScript::GetNonCompiledScriptSize(){
	if(IsCompiled())
		return 0;
	wchar_t* pwcScript = (wchar_t*)mjss;
	if(pwcScript == NULL)
		return 0;
	return (wcslen(pwcScript) + 1) * sizeof(wchar_t);
}

}
