#include "mlRMML.h"
#include "config/oms_config.h"
#include <locale>

#include "config/prolog.h"
#include "rmmlsafe.h"

#include <vector>
#include <iostream>
using namespace std;

namespace rmml {

#define IS_EQUAL(S1, S2) \
	(*(S1) == *(S2) && (*(S1) == 0 || isEqual((S1)+1, (S2)+1)))

mlRMMLBuilder::mlRMMLBuilder(JSContext* acx, bool abUI):
	MP_VECTOR_INIT(mvParents),
	MP_VECTOR_INIT(mvIncludes),
	MP_WSTRING_INIT(msIncludeSrc),
    MP_WSTRING_INIT(msValue),
	MP_WSTRING_INIT(msUnkonownTagName),
	mpRootElement(NULL)
{
	cx = acx;
	mbIn3DScene = false;
	mbInViewport = false;
	mpCurAr = NULL;
	mpLibXml2Funcs = GPSM(cx)->GetLibXml2Funcs();
	mpBuildEvalCtxt = GPSM(cx)->GetBuildEvalCtxt();
	mpBuildEvalCtxt->Clear();
	miElemCount = 0;

//mpLibXml2Funcs = NULL;
}

mlRMMLBuilder::~mlRMMLBuilder(void)
{
}

gd::CString GenerateRandomName(){
	gd::CString str("__");
	int iNumOfDig=rand()%4+4;
	for(int ii=0; ii<iNumOfDig; ii++){
		int iDig=rand()%10;
		char chDig='0'+iDig;
		str+=chDig;
	}
	str+='_';
	return str;
}

bool ParseVersion(const wchar_t* apwcVer,unsigned &auMajor,unsigned &auMinor){
	if(apwcVer==NULL) return false;
	auMajor=0;
	auMinor=0;
	for(; *apwcVer!=L'\0'; apwcVer++){
		if(*apwcVer!=L' ') break;
	}
	if(*apwcVer==L'\0') return false;
	for(; *apwcVer!=L'\0'; apwcVer++){
		if(*apwcVer==L'.') break;
		if(*apwcVer<L'0' || *apwcVer>L'9') return false;
		auMajor=auMajor*10+(*apwcVer-L'0');
	}
	if(auMajor==0) return false;
	if(*apwcVer==L'\0') return true;
	apwcVer++;
	for(; *apwcVer!=L'\0'; apwcVer++){
		if(*apwcVer==L' ') break;
		if(*apwcVer<L'0' || *apwcVer>L'9') return false;
		auMinor=auMinor*10+(*apwcVer-L'0');
	}
	return true;
}

#define CREATE_JSMLEL(NM) \
	}else if(isEqual(pwcTagName, RMMLTN_##NM)){ \
		apElem = GET_RMMLEL(cx, mlRMML##NM::newJSObject(cx));

mlresult mlRMMLBuilder::CreateElementByTag(JSContext* cx, const wchar_t* apwcTagName, mlRMMLElement* &apElem){
	if(apwcTagName == NULL || *apwcTagName == L'\0')
		return ML_ERROR_INVALID_ARG;
	// make lower
	wchar_t* pwcTagName = MP_NEW_ARR(wchar_t, (wcslen(apwcTagName)+1));
	wchar_t* pwc;
	for(pwc = pwcTagName; *apwcTagName != L'\0'; ){
		wchar_t wch = *(apwcTagName++);
		if((wch >= L'A') && (wch <= L'Z'))
			wch = wch - L'A' + L'a';
		*(pwc++) = wch;
	}
	*pwc = L'\0';
	//
	if(isEqual(pwcTagName, RMMLTN_Script)){
		apElem = MP_NEW( mlRMMLScript);
		apElem->mcx=cx;
	CREATE_JSMLEL(Image);
	CREATE_JSMLEL(Text);
	CREATE_JSMLEL(Animation);
	}else if(isEqual(pwcTagName, RMMLTN_Composition)){
		apElem=GET_RMMLEL(cx, mlRMMLComposition::newJSObject(cx));
//mlRMMLComposition* pIm=(mlRMMLComposition*)JS_GetPrivate(cx, ajsoElem);
//mlRMMLElement* pElem=GET_RMMLEL(cx, ajsoElem);
//int hh=0;
	}else if(isEqual(pwcTagName, RMMLTN_Scene)){
		apElem=GET_RMMLEL(cx, mlRMMLComposition::newJSObject(cx));
		apElem->mRMMLType=MLMT_SCENE;
	CREATE_JSMLEL(Button);
	CREATE_JSMLEL(Sequencer);
	CREATE_JSMLEL(Object);
	}else if(isEqual(pwcTagName, RMMLTN_Object3D)){
		apElem=GET_RMMLEL(cx, mlRMMLObject::newJSObject(cx));
	CREATE_JSMLEL(Motion);
	CREATE_JSMLEL(Group);
	CREATE_JSMLEL(Speech);
	CREATE_JSMLEL(Audio);
	CREATE_JSMLEL(Camera);
	CREATE_JSMLEL(Light);
	CREATE_JSMLEL(Video);
	CREATE_JSMLEL(Input);
	CREATE_JSMLEL(ActiveX);
	CREATE_JSMLEL(Flash);
	CREATE_JSMLEL(Browser);
	}else if(isEqual(pwcTagName, RMMLTN_QT)){
		apElem=GET_RMMLEL(cx, mlRMMLQT::newJSObject(cx));
	CREATE_JSMLEL(Plugin);
	CREATE_JSMLEL(Scene3D);
	CREATE_JSMLEL(Character);
	CREATE_JSMLEL(Viewport);
	CREATE_JSMLEL(Movement);
	CREATE_JSMLEL(Visemes);
	CREATE_JSMLEL(Cloud);
	CREATE_JSMLEL(XML); 
	CREATE_JSMLEL(Styles);
	CREATE_JSMLEL(Hint);
	CREATE_JSMLEL(Path3D);
	CREATE_JSMLEL(Idles);
	CREATE_JSMLEL(Idle);
	CREATE_JSMLEL(Map);
	CREATE_JSMLEL(Material);
	CREATE_JSMLEL(Materials);
	}else if(isEqual(pwcTagName, RMMLTN_Shadows)){
		if(mbIn3DScene)
			apElem=GET_RMMLEL(cx, mlRMMLShadows3D::newJSObject(cx));
		else return ML_ERROR_FAILURE;
	}else if(isEqual(pwcTagName, RMMLTN_Shadow)){
		if(mbIn3DScene)
			apElem=GET_RMMLEL(cx, mlRMMLShadow3D::newJSObject(cx));
		else return ML_ERROR_FAILURE;
	CREATE_JSMLEL(Resource);
	CREATE_JSMLEL(ImageFrame);
	CREATE_JSMLEL(Line);
	}else{
		apElem=NULL;
		return ML_ERROR_INVALID_ARG;
	}
	return ML_OK;
}

typedef gd_xml_stream::CXMLArchive<mlXMLBuffer> gdmlXMLArchive;

mlresult mlRMMLBuilder::Build(mlXMLArchive* apAr, mlSceneManager* apSceneManager, bool abUI){
	ML_ASSERTION(cx, apAr!=NULL && apSceneManager!=NULL,"mlRMMLBuilder::Build");
	mpRootElement = NULL;
	mpSceneManager = apSceneManager;
	cx=mpSceneManager->cx;
	mlRMMLComposition* pLastScene=NULL;
	mlDontSendSynchEvents oDSSE(apSceneManager);
	mbIn3DScene=false;
	mbInViewport=false;
	std::vector<mlRMMLElement*> vParents;
	std::vector<mlXMLArchive*> vXMLs;
	mlString sUnknownObject;
	mlXMLArchive* pmlAr=apAr;
	gdmlXMLArchive* pAr=apAr->mpAr;
	pAr->SkipHeader();
	for(;;){
		while(!pAr->ReadNext()){
			if(vXMLs.size()==0){
				pmlAr=NULL; pAr=NULL;
				break;
			}
			apSceneManager->CloseXMLArchive(pmlAr);
			pmlAr=vXMLs.back();
			vXMLs.pop_back();
			pAr=pmlAr->mpAr;
		}
		if(pmlAr==NULL) break;
		gd::CString sName = pAr->GetName(); // sName.MakeLower();
		if(pAr->IsEndTag()){
//for(int ii=pAr->GetDepth()+1; ii>0; ii--) TRACE("  ");
//TRACE("</%S>\n",(LPCWSTR)sName);
			if(sName==RMMLTN_RMML){
				continue; // skip <rmml ...
			}
			if(vParents.size() > 0){
//				JSObject* jso=vParents.back();
//				JS_RemoveRoot(cx, jso);
				if(vParents.back()->mRMMLType==MLMT_SCENE3D)
					mbIn3DScene=false;
				if(vParents.back()->mRMMLType==MLMT_VIEWPORT)
					mbInViewport=false;
				vParents.pop_back();
			}
//			iNodeDepth--;
		}else{
//for(int ii=pAr->GetDepth(); ii>0; ii--) TRACE("  ");
//TRACE("<%S",(LPCWSTR)sName);
			if(sName==RMMLTN_RMML){
				gd::CString sAttrName, sAttrValue;
				unsigned uCount = pAr->GetAttributeCount();
				for(unsigned u=0; u<uCount; u++) {
					sAttrName = pAr->GetAttributeName(u); sAttrName.MakeLower();
					sAttrValue = pAr->GetAttribute(u);
					if(apSceneManager->meMode==smmAuthoring && !abUI){
						if(sAttrName==L"ui"){
							apSceneManager->GetContext()->mpResM->SetUIBase(sAttrValue);
							apSceneManager->LoadUI(L"ui:ui.xml");
							apSceneManager->GetContext()->mpResM->SetModuleBase(apAr->msSrc.c_str());
						}
					}
					if(sAttrName==L"version"){
						sAttrValue = pAr->GetAttribute(u);
						unsigned uMajor,uMinor;
						if(!ParseVersion(sAttrValue,uMajor,uMinor)){
							mlTraceError(cx, "Version attribute value is not valid");
						}else{
							// ѕроверить на версию €зыка в XML-файле, 
							unsigned uRMMLVMajor=1, uRMMLVMinor=0;
							apSceneManager->GetRMMLVersion(uRMMLVMajor, uRMMLVMinor);
							if(uMajor > uRMMLVMajor || ((uMajor == uRMMLVMajor) && uMinor > uRMMLVMinor)){
								// если больше текущей, 
								// то поругатьс€
								// ??
								mlTraceWarning(cx, "RMML-script %S require higher RMML version %S (current is %d.%d)\n", 
									pmlAr->msSrc.c_str(), (const wchar_t*)sAttrValue, uRMMLVMajor, uRMMLVMinor);
							}
						}
					}
				}
				continue; // skip <rmml ...
			}
			if(sName == "define"){
				mlString sName;
				mlString sValue;
				for(unsigned u=0; u<pAr->GetAttributeCount(); u++) {
					if(pAr->GetAttributeName(u) == L"name"){
						sName=pAr->GetAttribute(u);
					}else if(pAr->GetAttributeName(u) == L"value"){
						sValue=pAr->GetAttribute(u);
					}
				}
				mpBuildEvalCtxt->DefineConstant(sName.c_str(), sValue.c_str());
				continue;
			}
			if(sName == RMMLTN_INCLUDE && !mbInViewport){
				mlString sSrc;
				for(unsigned u = 0; u < pAr->GetAttributeCount(); u++) {
					if(pAr->GetAttributeName(u) == L"src")
						sSrc = pAr->GetAttribute(u);
				}
				if(!sSrc.empty()){
					mlXMLArchive* pmlArNew = NULL;
					mlString sPath = mpSceneManager->RefineResPath(pmlAr->msSrc, sSrc.c_str());
					mpSceneManager->OpenXMLArchive(sPath.c_str(), pmlArNew);
					if(pmlArNew != NULL){
						vXMLs.push_back(pmlAr);
						pmlAr = pmlArNew;
						pAr = pmlArNew->mpAr;
						pAr->SkipHeader();
					}
				}
				continue; 
			}
			JSObject* jsoParent=NULL;
			mlRMMLElement* pParent = NULL;
			if(vParents.size() > 0){
				pParent = vParents.back();
				jsoParent = pParent->mjso;
			}
			if(mbInViewport && pParent!=NULL && pParent->mRMMLType == MLMT_VIEWPORT){
				mlRMMLViewport* pViewport = (mlRMMLViewport*)pParent;
				if(sName == L"include"){
					gd::CString sValue=pAr->GetValue();
					pViewport->SetProperty(L"include", (wchar_t*)(LPCWSTR)sValue);
					continue;
				}
				if(sName == L"exclude"){
					gd::CString sValue=pAr->GetValue();
					pViewport->SetProperty(L"exclude", (wchar_t*)(LPCWSTR)sValue);
					continue;
				}
			}
			{
				int iSrcFilesIdx = pmlAr->miSrcIdx;
				int iSrcLine = pAr->GetTagLine();
				bool bFinal = true;
				mlRMMLElement* pNewElem=NULL;
				if(ML_SUCCEEDED(CreateElementByTag(cx, sName, pNewElem))){
					if(pNewElem->mRMMLType==MLMT_SCENE3D)
						mbIn3DScene=true;
					if(pNewElem->mRMMLType==MLMT_VIEWPORT)
						mbInViewport=true;
					pNewElem->SetSrcFileIdx(iSrcFilesIdx, iSrcLine);
					JSObject* jsoNewElem=pNewElem->mjso;
					bool bWasNoName=true;
					bool bRefining=false;
//					JS_AddRoot(cx, jsoNewElem);
					// устанавливаем/уточн€ем свойства
					gd::CString sAttrName, sAttrValue;
					unsigned uCount = pAr->GetAttributeCount();
					for(unsigned u=0; u<uCount; u++) {
						sAttrName = pAr->GetAttributeName(u); //sAttrName.MakeLower();
						sAttrValue = pAr->GetAttribute(u);
//						const char* smbAttrName=(const char*)sAttrName;
						// convert value to Unicode string
//						wchar_t* wsAttrValue=MB2WC(sAttrValue);
						if(isEqual(sAttrValue, L"eval(", 5)){
							mlString sAttrEvalExpr = sAttrValue+5;
							int iLen = sAttrEvalExpr.length();
							if(iLen > 0)
								sAttrEvalExpr.erase(iLen-1);
							if(mpBuildEvalCtxt != NULL)
								sAttrValue = mpBuildEvalCtxt->Eval(sAttrEvalExpr.c_str());
						}
						//
//if(isEqual((wchar_t*)(LPCWSTR)sAttrValue,L"aud6"))
//int hh=0;
						const wchar_t* pwcPropName = (LPCWSTR)sAttrName;
						if( *pwcPropName==L'f' && 
							isEqual(pwcPropName+1, L"inal") && 
							isEqual((LPCWSTR)sAttrValue, L"false") &&
							pNewElem->mRMMLType != MLMT_SCENE
						){
							bFinal = false;
							continue;
						}
						if(IS_EQUAL(pwcPropName, L"userProperties")){
							pNewElem->SetUserProperties((LPCWSTR)sAttrValue);
							continue;
						}
						if (bRefining && 
							*pwcPropName==L's' && 
							pNewElem->IsLoadable() &&
							isEqual(pwcPropName+1, L"rc")
						){
							pNewElem->SetSrcFileIdx(iSrcFilesIdx, iSrcLine);
						}
						bool bSetParentToNULL = false;
						if(pNewElem->mpParent == NULL && 
							(IS_EQUAL(pwcPropName, L"class") || IS_EQUAL(pwcPropName, L"scriptProviders"))
						){
							pNewElem->mpParent = pParent;
							bSetParentToNULL = true;
						}
						pNewElem->SetProperty(pwcPropName, (wchar_t*)(LPCWSTR)sAttrValue);
						if(bSetParentToNULL)
							pNewElem->mpParent = NULL;
						if(bWasNoName && pParent!=NULL && pNewElem->_name!=NULL){
							bWasNoName=false;
							// ищем в родителе ребенка с таким же именем
							jsval v;
							jschar* pjsch=JS_GetStringChars(pNewElem->_name);
							size_t jssLen=JS_GetStringLength(pNewElem->_name);
							if(pParent->mjso == NULL){
								mlTraceError(cx, "Internal error (mlRMMLBuilder::Build: pParent->mjso == NULL)");
								break;
							}
							if(JS_GetUCProperty(cx, pParent->mjso, pjsch, jssLen, &v)){
								if(JSVAL_IS_REAL_OBJECT(v)){
									JSObject* jsoProp = JSVAL_TO_OBJECT(v);
									if(JSO_IS_MLEL(cx, jsoProp)){
										mlRMMLElement* pMLEl=GET_RMMLEL(cx, jsoProp);
										if(pMLEl){
											// если типы разные, то ругаемс€
											if(pMLEl->mRMMLType!=pNewElem->mRMMLType){
//												ML_WARNING("Type mismatch while refining");
												mlTrace(cx, "Type mismatch while refining element %s (line:%d)\n",cLPCSTR(pjsch),pNewElem->miSrcLine);
											}
											// если нашли, то берем у него прочитанные уже свойства
											pMLEl->GetPropsAndChildrenCopyFrom(pNewElem);
											// , удал€ем новый 
											if(pMLEl->IsJSO()){ // сам умрет
//													cx,pNewElem->mjso
											}else  MP_DELETE( pNewElem);
											// и считаем найденный текущим
											iSrcFilesIdx = pNewElem->miSrcFilesIdx;
											iSrcLine = pNewElem->miSrcLine;
											pNewElem=pMLEl;
											bRefining=true;
										}
									}
								}
							}
						}
//TRACE(" %S=\"%S\"",(LPCWSTR)sAttrName,(LPCWSTR)sAttrValue);
					}
//TRACE(">\n");
					if(!bRefining){
						// устанавливаем св€зь с родительским элементом
						pNewElem->SetParent(pParent);
					}
					// берем данные (например, текст скрипта)
					gd::CString sValue=pAr->GetValue();
		//			sValue.TrimLeft("\t\r\n ");
		//			sValue.TrimRight("\t\r\n ");
					if(!sValue.IsEmpty()){
//						wchar_t* wsValue=MB2WC(sValue);
						pNewElem->SetValue((wchar_t*)(LPCWSTR)sValue);
//						 MP_DELETE( wsValue);
//for(ii=pAr->GetDepth()+1; ii>0; ii--) TRACE("  ");
//TRACE("%S\n",(LPCWSTR)sValue.Left(250));
					}
					if(!bFinal && pParent != NULL){
						// новый элемент - подкласс родительского элемента или класса
						if(pNewElem->_name){
							mlString sSubClassName = wr_JS_GetStringChars(pNewElem->_name);
							mlRMMLElement* pParent2 = pParent;
							while(pParent2 != NULL){
								mlString sParentName = pParent2->GetName();
								mlString sOldSNName = sSubClassName;
								sSubClassName = sParentName;
								sSubClassName += L"_";
								sSubClassName += sOldSNName;
								pParent2 = pParent2->mpParent;
							}
							mpSceneManager->UpdateNotInScenesElems(pNewElem, true);
							jsval v = OBJECT_TO_JSVAL(pNewElem->mjso);
							wr_JS_DefineUCProperty(cx, mpSceneManager->mjsoClasses, 
								sSubClassName.c_str(), -1, v, NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
							jsval vName = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sSubClassName.c_str()));
							JS_SetProperty(cx, pNewElem->mjso, MLELPN_NAME, &vName);
						}
					}else{
						if(pParent==NULL){
							// добавл€ем корневой элемент в классы
							if(pNewElem->IsScene() && pNewElem->mType!=MLMT_SCENECLASS){
								pLastScene=(mlRMMLComposition*)pNewElem;
							}else{
								if(pNewElem->_name){
									mpSceneManager->UpdateNotInScenesElems(pNewElem, true);
									mpSceneManager->InitClassObject(pNewElem);
									jsval v=OBJECT_TO_JSVAL(pNewElem->mjso);
									JS_DefineUCProperty(cx, mpSceneManager->mjsoClasses, 
											JS_GetStringChars(pNewElem->_name), 
											JS_GetStringLength(pNewElem->_name), v, 
											NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
								}
							}
						}
					}
					// обновл€ем список родительских элементов
					if(pAr->HasChildren()){
						vParents.push_back(pNewElem);
					}
				}else{ 
					sUnknownObject = sName;
					mlTraceError(cx, "Unknown RMML tag %S (%S:%d)\n", (const wchar_t*)sName, GPSM(cx)->mSrcFileColl[iSrcFilesIdx], iSrcLine);
				} // dif(ML_SUCCEEDED(CreateElementByTag(sName, pNewElem)))
			}
//if(!pAr->HasChildren()){
//for(int ii=pAr->GetDepth(); ii>0; ii--) TRACE("  ");
//TRACE("</%S>\n",(LPCWSTR)sName);
//}
			if(pAr->HasChildren()){
//				pParentVals[++iNodeDepth]=pNewVal;
			}
		}
	}

	SetScene(pLastScene, abUI, apAr->msSrc.c_str());

	vParents.clear();
	vXMLs.clear();
	return ML_OK;
}

void mlRMMLBuilder::SetScene(mlRMMLComposition* apNewScene, bool abUI, const wchar_t* apwcSrc){
	if(apNewScene == NULL) return;
	GUARD_JS_CALLS(mpSceneManager->cx);
	MP_NEW_P( pSceneData, mlSceneDatas::mlSceneData, apNewScene);
	pSceneData->msPath = apwcSrc;
	mpSceneManager->mSceneDatas.AddSceneData(pSceneData);
	bool bAuthScene = isEqual(apwcSrc, AUTH_SCENE_FILE_NAME);
	if(abUI){
		if(bAuthScene){
			apNewScene->GetIVisible()->Show();
			mpSceneManager->SetAuthScene(apNewScene);
		}else{
			apNewScene->GetIVisible()->Show();
			mpSceneManager->SetUIScene(apNewScene);
			mpBuildEvalCtxt->CopyAllConstantsTo(mpSceneManager->cx);
		}
	}else{
		mpSceneManager->SetScene(apNewScene);
		mpBuildEvalCtxt->CopyAllConstantsTo(mpSceneManager->cx);
		mlSynchData oData;
		oData << 2;
		GPSM(cx)->GetContext()->mpInput->AddCustomEvent(mpSceneManager->CEID_onShowInfoState, (unsigned char*)oData.data(), oData.size());
	}

	// создаем и грузим медиа-объекты сцены
	if(mpSceneManager->mpScene!=NULL || abUI){
		mlRMMLElement* pScene=mpSceneManager->mpScene;
		if(abUI){ 
			if(bAuthScene)
				pScene = mpSceneManager->mpAuthScene;
			else
				pScene = mpSceneManager->mpUI;
			if(pScene != NULL){
				mlTrace(cx, "Creating elements...\n");
				pScene->CreateAndLoadMOs();
				mlTrace(cx, "Done. (Creating elements...)\n");
				mlTrace(cx, "Executing and activating scripts...\n");
				pScene->ActivateScripts();
				mlTrace(cx, "Done. (Executing and activating scripts...)\n");
				mpSceneManager->SetLoadingProgress(1.0);
				JS_GC(cx);
			}
		}else{
			// будем закачивать постепенно
			mlTrace(cx, "Creating elements...\n");
			// считаем общее количество элементов, 
			// которые надо загрузить
			unsigned int iElToLoadCnt=0;
			mlRMMLIterator Iter(cx,pScene);
			mlRMMLElement* pElem=NULL;
			while((pElem=Iter.GetNext())!=NULL){
				if(!pElem->IsComposition())
					iElToLoadCnt++;
			}
			mpSceneManager->muElToLoadCnt=iElToLoadCnt;
			mpSceneManager->muElLoadedCnt=0;
			mpSceneManager->SetMode(smmLoading);
			mpSceneManager->SetLoadingProgress(0.0);
		}
	}
}

///////////////////////////////////////////////////////////////////
// Build by LibXML2
// 

bool UTF82String(const char* autf8Str, int anLen, mlString &asStr){
	asStr.erase();

	if (anLen == -1){
		anLen = strlen(autf8Str);
	}

	unsigned uLength;
	char chTemp;
	for (; *autf8Str != '\0' && anLen > 0; anLen--){
		wchar_t wch;
		if ((*autf8Str & 0x80) == 0){
			wch = *autf8Str;
			autf8Str++;
		}else{
			// Calculate bits
			//chTemp = (char)0xF0;
			if ((*autf8Str & 0xF0) == 0xF0){
				uLength = 4; chTemp = (char) 0xF0;
			}
			else if ((*autf8Str & 0xE0) == 0xE0){
				uLength = 3; chTemp = (char) 0xE0;
			}
			else if ((*autf8Str & 0xC0) == 0xC0){
				uLength = 2; chTemp = (char) 0xC0;
			}
			else
				return false;

			// Build character
			// The first byte needs to clear part that has count of bytes for the UTF8 character.
			// This is done by clearing the number bits for each byte + 1 from right that the character
			// are taken.
			// Then we shift the final character one octal bitlenght for each byte that is added.
			uLength--;
			wch = (*autf8Str & ~chTemp) << (uLength * 6);

			autf8Str++; uLength--;
			wch |= (*autf8Str & 0x3F) << (uLength * 6);
			if (uLength){
				autf8Str++; uLength--;
				wch |= (*autf8Str & 0x3F) << (uLength * 6);
				if (uLength){
					autf8Str++; uLength--;
					wch |= (*autf8Str & 0x3F);
				}
			}
			autf8Str++;
		}
		asStr += wch;
	}
	return true;
}

#ifdef RMML_USE_LIBXML2

xmlSAXHandler mlSAX2HandlerStruct =
{
	NULL, // internalSubsetDebug,
	NULL, // isStandaloneDebug,
	NULL, // hasInternalSubsetDebug,
	NULL, // hasExternalSubsetDebug,
	NULL, // resolveEntityDebug,
	NULL, // getEntityDebug,
	NULL, // entityDeclDebug,
	NULL, // notationDeclDebug,
	NULL, // attributeDeclDebug,
	NULL, // elementDeclDebug,
	NULL, // unparsedEntityDeclDebug,
	NULL, // setDocumentLocatorDebug,
	NULL, // startDocumentDebug,
	NULL, // endDocumentDebug,
	NULL, 
	NULL, 
	NULL, // referenceDebug,
	mlRMMLBuilder::characters, // charactersDebug,
	NULL, // ignorableWhitespaceDebug,
	NULL, // processingInstructionDebug,
	NULL, // commentDebug,
	mlRMMLBuilder::warningDebug, // warningDebug,
	mlRMMLBuilder::errorDebug, // errorDebug,
	mlRMMLBuilder::fatalErrorDebug, // fatalErrorDebug,
	NULL, // getParameterEntityDebug,
	NULL, // cdataBlockDebug,
	NULL, // externalSubsetDebug,
	XML_SAX2_MAGIC, 
	NULL, 
	mlRMMLBuilder::startElementNs, // startElementNsDebug,
	mlRMMLBuilder::endElementNs, // endElementNsDebug,
	NULL
};

xmlSAXHandlerPtr mlSAX2Handler = &mlSAX2HandlerStruct;

/*
* SAX callbacks
*/

__forceinline bool IsSpaceChar(char ach){
	switch(ach){
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return true;
	}
	return false;
}

void mlRMMLBuilder::characters(void* ctx ATTRIBUTE_UNUSED, const xmlChar* ch, int len)
{
	mlRMMLBuilder* pRMB = (mlRMMLBuilder*) ctx;
	if(pRMB->mvParents.size() == 0)
		return;
	mlRMMLElement* pNewElem = pRMB->mvParents.back();
	if(pNewElem != NULL){
		const char* pch = (const char*)ch;
		/* чтобы номера строк не сбивались, не будем ничего пропускать
		if(pRMB->msValue.empty()){
			// skip leading spaces
			for(; len > 0; len--, pch++){
				if(!IsSpaceChar(*pch))
					break;
			}
		}
		*/
		// skip ending spaces
//		for(; len > 0; len--){
//			if(!IsSpaceChar(pch[len-1]))
//				break;
//		}
		if(len == 0)
			return;
		mlString sVal;
		UTF82String(pch, len, sVal);
		pRMB->msValue += sVal;
	}
}

/*
* SAX2 specific callbacks
*/
void mlRMMLBuilder::startElementNs(void* ctx ATTRIBUTE_UNUSED,
								   const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
								   int nb_namespaces, const xmlChar** namespaces, int nb_attributes,
								   int nb_defaulted, const xmlChar** attributes){
//TRACE("<%s>\n",localname);
	mlRMMLBuilder* pRMB = (mlRMMLBuilder*) ctx;

	if(isEqual((const char*)localname, "rmml")){
		return; // skip <rmml ...
	}
	if(isEqual((const char*)localname, "define")){
		mlString sName;
		mlString sValue;
		for (int i = 0; i < nb_attributes*5; i += 5){
			const char* pcAttrName = (const char*)attributes[i];
			int iValLen = (int) (attributes[i + 4] - attributes[i + 3]);
			const char* pcAttrVal = (const char*)attributes[i + 3];
			if(isEqual(pcAttrName, "name")){
				UTF82String(pcAttrVal, iValLen, sName);
			}else if(isEqual(pcAttrName, "value")){
				UTF82String(pcAttrVal, iValLen, sValue);
			}
		}
		pRMB->mpBuildEvalCtxt->DefineConstant(sName.c_str(), sValue.c_str());
		return;
	}
	if(isEqual((const char*)localname, "include") && !pRMB->mbInViewport){
		pRMB->msIncludeSrc.erase();
		for (int i = 0; i < nb_attributes*5; i += 5){
			if(isEqual((const char*)attributes[i], "src")){
				int iLen = (int) (attributes[i + 4] - attributes[i + 3]);
				UTF82String((const char*)attributes[i + 3], iLen, pRMB->msIncludeSrc);
				break;
			}
		}
		return;
	}
	JSObject* jsoParent=NULL;
	mlRMMLElement* pParent=NULL;
	if(pRMB->mvParents.size() > 0){
		pParent=pRMB->mvParents.back();
		jsoParent=pParent->mjso;
	}
	if(pRMB->mbInViewport && pParent!=NULL && pParent->mRMMLType == MLMT_VIEWPORT){
		if(isEqual((const char*)localname, "include"))
			return;
		if(isEqual((const char*)localname, "exclude"))
			return;
	}
	GUARD_JS_CALLS(pRMB->cx);
	{
		int iSrcFilesIdx = ((mlXMLArchive*)(pRMB->mpParserCtxt->input->buf->context))->miSrcIdx; // pRMB->mpCurAr->miSrcIdx;
//if(isEqual(pRMB->mpParserCtxt->input->filename, ":rmsc:rmcomscr:labbase.xml"))
//int hh=0;
		int iSrcLine = pRMB->mpParserCtxt->input->line; // begin_line; // 
		bool bFinal = true;
		mlRMMLElement* pNewElem=NULL;
//		gd::CString sName((const char*)localname);
		mlString sName = cLPWCSTRq((const char*)localname);
		if(ML_SUCCEEDED(pRMB->CreateElementByTag(pRMB->cx, sName.c_str(), pNewElem))){
			if(pNewElem->IsJSO())
				SAVE_FROM_GC(pRMB->cx, JS_GetGlobalObject(pRMB->cx), pNewElem->mjso); // uncomment by Tandy 03.06.2010
			if(pNewElem->mRMMLType==MLMT_SCENE3D)
				pRMB->mbIn3DScene=true;
			if(pNewElem->mRMMLType == MLMT_VIEWPORT)
				pRMB->mbInViewport = true;
			pNewElem->SetSrcFileIdx(iSrcFilesIdx, iSrcLine);
			JSObject* jsoNewElem=pNewElem->mjso;
			bool bWasNoName=true;
			bool bRefining=false;
			// устанавливаем/уточн€ем свойства
			for (int i = 0; i < nb_attributes*5; i += 5){
				mlString sAttrName;
				UTF82String((const char*)attributes[i], -1, sAttrName);
				mlString sAttrValue;
				int iLen = (int) (attributes[i + 4] - attributes[i + 3]);
				UTF82String((const char*)attributes[i + 3], iLen, sAttrValue);
				//
				if(isEqual(sAttrValue.c_str(), L"eval(", 5)){
					mlString sAttrEvalExpr = sAttrValue.c_str()+5;
					int iLen = sAttrEvalExpr.length();
					if(iLen > 0)
						sAttrEvalExpr.erase(iLen-1);
					sAttrValue = pRMB->mpBuildEvalCtxt->Eval(sAttrEvalExpr.c_str());
				}
				const wchar_t* pwcPropName = sAttrName.c_str();
				if( IS_EQUAL(pwcPropName, L"final") && 
					isEqual(sAttrValue.c_str(), L"false") &&
					pNewElem->mRMMLType != MLMT_SCENE
				){
					bFinal = false;
					continue;
				}
				if(IS_EQUAL(pwcPropName, L"userProperties")){
					pNewElem->SetUserProperties(sAttrValue.c_str());
					continue;
				}
				if (bRefining && 
					*pwcPropName==L's' && 
					pNewElem->IsLoadable() &&
					isEqual(pwcPropName+1, L"rc")
				){
					pNewElem->SetSrcFileIdx(iSrcFilesIdx, iSrcLine);
				}
				bool bSetParentToNULL = false;
				if(pNewElem->mpParent == NULL && 
					(IS_EQUAL(pwcPropName, L"class") || IS_EQUAL(pwcPropName, L"scriptProviders"))
				){
					pNewElem->mpParent = pParent;
					bSetParentToNULL = true;
				}
				pNewElem->SetProperty(pwcPropName, (wchar_t*)sAttrValue.c_str());
				if(bSetParentToNULL)
					pNewElem->mpParent = NULL;
				if(bWasNoName && pParent!=NULL && pNewElem->_name!=NULL){
					bWasNoName=false;
					// ищем в родителе ребенка с таким же именем
					jsval v;
					jschar* pjsch=JS_GetStringChars(pNewElem->_name);
					size_t jssLen=JS_GetStringLength(pNewElem->_name);
					if(pParent->mjso == NULL){
						mlTrace(pRMB->cx, "Internal error (mlRMMLBuilder::Build: pParent->mjso == NULL)");
						break;
					}
					if(JS_GetUCProperty(pRMB->cx, pParent->mjso, pjsch, jssLen, &v)){
						if(JSVAL_IS_REAL_OBJECT(v)){
							mlRMMLElement* pMLEl=GET_RMMLEL(pRMB->cx,JSVAL_TO_OBJECT(v));
							if(pMLEl){
								// если типы разные, то ругаемс€
								if(pMLEl->mRMMLType!=pNewElem->mRMMLType){
									mlTraceError(pRMB->cx, "Type mismatch while refining element %s (line:%d)\n",cLPCSTR(pjsch),pNewElem->miSrcLine);
								}
								// если нашли, то берем у него прочитанные уже свойства
								pMLEl->GetPropsAndChildrenCopyFrom(pNewElem);
								// , удал€ем новый 
								if(pNewElem->IsJSO()){
									FREE_FOR_GC(pRMB->cx, JS_GetGlobalObject(pRMB->cx), pNewElem->mjso);
								}else  MP_DELETE( pNewElem);
								// и считаем найденный текущим
								pNewElem=pMLEl;
								bRefining=true;
							}
						}
					}
				}
			}
//if(pNewElem->_name != NULL){
//if(isEqual(pNewElem->_name, L"photocamera"))
//int hh=0;
//}
			if(pNewElem->mRMMLType == MLMT_SCENE)
				pNewElem->GetIVisible()->Hide();
			if(!bFinal && pParent != NULL){
				// новый элемент - подкласс родительского элемента или класса
				if(pNewElem->_name){
					mlString sSubClassName = wr_JS_GetStringChars(pNewElem->_name);
					mlRMMLElement* pParent2 = pParent;
					while(pParent2 != NULL){
						mlString sParentName = pParent2->GetName();
						mlString sOldSNName = sSubClassName;
						sSubClassName = sParentName;
						sSubClassName += L"_";
						sSubClassName += sOldSNName;
						pParent2 = pParent2->mpParent;
					}
					pRMB->mpSceneManager->UpdateNotInScenesElems(pNewElem, true, true);
					jsval v = OBJECT_TO_JSVAL(pNewElem->mjso);
					wr_JS_DefineUCProperty(pRMB->cx, pRMB->mpSceneManager->mjsoClasses, 
						sSubClassName.c_str(), -1, v, NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
					jsval vName = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(pRMB->cx, sSubClassName.c_str()));
					JS_SetProperty(pRMB->cx, pNewElem->mjso, MLELPN_NAME, &vName);
				}
			}else{
				if(!bRefining){
					// устанавливаем св€зь с родительским элементом
					pNewElem->SetParent(pParent);
				}
				if(pParent == NULL){
					if(pRMB->mpRootElement != NULL){
//						pNewElem->SetParent(pRMB->mpRootElement);
					}else{
						if(pNewElem->IsScene() && pNewElem->mType!=MLMT_SCENECLASS){
							if(pRMB->mpLastScene != NULL){
								FREE_FOR_GC(pRMB->cx, JS_GetGlobalObject(pRMB->cx), pRMB->mpLastScene->mjso);
							}
							pRMB->mpLastScene=(mlRMMLComposition*)pNewElem;
							SAVE_FROM_GC(pRMB->cx, JS_GetGlobalObject(pRMB->cx), pRMB->mpLastScene->mjso);
						}else{
							if(pNewElem->_name){
								pRMB->mpSceneManager->UpdateNotInScenesElems(pNewElem, true, true);
								size_t iClassNameLength = JS_GetStringLength(pNewElem->_name);
								jschar* jscClassName = JS_GetStringChars(pNewElem->_name);
								jsval v2 = JSVAL_NULL;
								JS_GetUCProperty(pRMB->cx, pRMB->mpSceneManager->mjsoClasses, jscClassName, iClassNameLength, &v2);
								if(JSVAL_IS_REAL_OBJECT(v2)){
									mlXMLArchive* pAr = NULL;
									if(pRMB->mpParserCtxt->inputNr-2 >= 0){
										xmlParserInputPtr pInput = pRMB->mpParserCtxt->inputTab[pRMB->mpParserCtxt->inputNr-2];
										xmlParserInputBufferPtr pInputBuffer = pInput->buf;
										pAr = (mlXMLArchive*)pInputBuffer->context;
									}

									mlTraceWarning(pRMB->cx, "Redefinition of class %S (may cause a bug) (%S:%d in %S)\n", jscClassName, GPSM(pRMB->cx)->mSrcFileColl[iSrcFilesIdx], iSrcLine, (pAr!=NULL)?pRMB->mpSceneManager->mSrcFileColl[pAr->miSrcIdx]:L"?");
								}
								pRMB->mpSceneManager->InitClassObject(pNewElem);
								jsval v = OBJECT_TO_JSVAL(pNewElem->mjso);
								JS_DefineUCProperty(pRMB->cx, pRMB->mpSceneManager->mjsoClasses, 
									jscClassName, 
									iClassNameLength, v, 
									NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
							}
						}
					}
				}
			}
			// обновл€ем список родительских элементов
			pRMB->mvParents.push_back(pNewElem);
			if((pRMB->miElemCount++) > 20){
				pRMB->miElemCount = 0;
//				JS_GC(pRMB->cx);
			}
			if(pNewElem->IsJSO())
				FREE_FOR_GC(pRMB->cx, JS_GetGlobalObject(pRMB->cx), pNewElem->mjso);
		}else{
			pRMB->msUnkonownTagName = sName;
			mlTraceError(pRMB->cx, "Unknown RMML tag %S (%S:%d)\n", sName.c_str(), GPSM(pRMB->cx)->mSrcFileColl[iSrcFilesIdx], iSrcLine);
		}
	}
	pRMB->msValue.erase();
}

#define ASX_MAX_ERROR_STR_LEN 3000

void mlRMMLBuilder::endElementNs(void* ctx ATTRIBUTE_UNUSED,
								 const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI){
//TRACE("</%s>\n",localname);
	mlRMMLBuilder* pRMB = (mlRMMLBuilder*) ctx;
	if(isEqual((const char*)localname, "rmml")){
		return; // skip </rmml>
	}
	if(isEqual((const char*)localname, "define")){
		return;
	}
	if(!pRMB->msUnkonownTagName.empty()){
		if(isEqual((const char*)localname, cLPCSTRq(pRMB->msUnkonownTagName.c_str())))
			pRMB->msUnkonownTagName = L"";
		return;
	}
	if(pRMB->mbInViewport){
		mlRMMLElement* pParent = pRMB->mvParents.back();
		if(pParent!=NULL && pParent->mRMMLType == MLMT_VIEWPORT){
			mlRMMLViewport* pViewport = (mlRMMLViewport*)pParent;
			if(isEqual((const char*)localname, "include")){
				pViewport->SetProperty(L"include", pRMB->msValue.c_str());
				return;
			}
			if(isEqual((const char*)localname, "exclude")){
				pViewport->SetProperty(L"exclude", pRMB->msValue.c_str());
				return;
			}
		}
	}
	if(isEqual((const char*)localname, "include")){
		if(!pRMB->msIncludeSrc.empty()){
			if(ML_FAILED(pRMB->Include(pRMB->msIncludeSrc.c_str())))
				return;
		}
		return;
	}
	if (pRMB->mvParents.size() == 0)
		return;
	mlRMMLElement* pNewElem = pRMB->mvParents.back();
	if(pRMB->mpRootElement != NULL && pNewElem->mpParent == NULL){
		pNewElem->SetParent(pRMB->mpRootElement);
	}
	if (pNewElem != NULL){
		GUARD_JS_CALLS(pRMB->cx);
		if(!pRMB->msValue.empty())
			pNewElem->SetValue((wchar_t*)pRMB->msValue.c_str());
	}
	if(!pRMB->mvParents.empty()){
		if(pRMB->mvParents.back()->mRMMLType == MLMT_SCENE3D)
			pRMB->mbIn3DScene = false;
		if(pRMB->mvParents.back()->mRMMLType == MLMT_VIEWPORT)
			pRMB->mbInViewport = false;
		pRMB->mvParents.pop_back();
	}
}

void mlRMMLBuilder::FormatErrorString(char* pszErr, unsigned int bufferSize)
{
	xmlErrorPtr pErr = mpLibXml2Funcs->xmlGetLastError();
	char sErr[ASX_MAX_ERROR_STR_LEN];
	//strcpy(sErr, pszErr);
	rmmlsafe_strcpy(sErr, sizeof(sErr), 0, pszErr);
	size_t len = strlen(sErr);
	if(len > 1){
		if(sErr[len-1] == '\n'){
			sErr[len-1] = '\0';
		}
	}
	const char* pszFile = pErr->file;
	if(pszFile == NULL) pszFile = "";
	// , pErr->int2 <- ~column дл€ некоторых кодов ошибок
	//sprintf(pszErr, "%s (%s:%d)\n", sErr, pszFile, pErr->line); 
	rmmlsafe_sprintf(pszErr, bufferSize, 0, "%s (%s:%d)\n", sErr, pszFile, pErr->line);
}

void mlRMMLBuilder::warningDebug(void* ctx ATTRIBUTE_UNUSED, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	char sErr[ASX_MAX_ERROR_STR_LEN];
	//vsprintf(sErr, msg, args);
	rmmlsafe_vsprintf(sErr, ASX_MAX_ERROR_STR_LEN, 0, msg, args);
	mlRMMLBuilder* pRMB = (mlRMMLBuilder*) ctx;
	pRMB->FormatErrorString(sErr, ASX_MAX_ERROR_STR_LEN);
	mlTraceWarning(pRMB->cx, sErr);
	va_end(args);
}

void mlRMMLBuilder::errorDebug(void* ctx ATTRIBUTE_UNUSED, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	char sErr[ASX_MAX_ERROR_STR_LEN];
	//vsprintf(sErr, msg, args);
	rmmlsafe_vsprintf(sErr, ASX_MAX_ERROR_STR_LEN, 0, msg, args);
	mlRMMLBuilder* pRMB = (mlRMMLBuilder*) ctx;
	pRMB->FormatErrorString(sErr, ASX_MAX_ERROR_STR_LEN);
	mlTraceError(pRMB->cx, sErr);
	va_end(args);
}

void mlRMMLBuilder::fatalErrorDebug(void* ctx ATTRIBUTE_UNUSED, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	char sErr[ASX_MAX_ERROR_STR_LEN];
	//vsprintf(sErr, msg, args);
	rmmlsafe_vsprintf(sErr, ASX_MAX_ERROR_STR_LEN, 0, msg, args);
	mlRMMLBuilder* pRMB = (mlRMMLBuilder*) ctx;
	pRMB->FormatErrorString(sErr, ASX_MAX_ERROR_STR_LEN);
	mlTraceError(pRMB->cx, sErr);
	va_end(args);
}

int mlRMMLBuilder::InputReadCallback(void * context, char * buffer, int len){
	mlXMLArchive* pAr = (mlXMLArchive*)context;
	std::istream* stream = pAr->mpIS;
//	pAr->mpRes->GetIStream(stream);
	if(stream == NULL) return -1;
	return (int)(stream->readsome(buffer, len));
}

int mlRMMLBuilder::InputCloseCallback(void * context){
	if(context == NULL)
		return 0;
	mlXMLArchive* pAr = (mlXMLArchive*)context;
	mlRMMLBuilder* pBuilder = pAr->mpBuilder;
	pBuilder->mpSceneManager->CloseXMLArchive(pAr);
	xmlParserCtxtPtr pParserCtxt = pBuilder->mpParserCtxt;
	if(pParserCtxt->inputNr > 0){
		xmlParserInputPtr pInput = pParserCtxt->inputTab[pParserCtxt->inputNr-1];
		xmlParserInputBufferPtr pInputBuffer = pInput->buf;
		pAr = (mlXMLArchive*)pInputBuffer->context;
		pBuilder->mpBuildEvalCtxt->SetXMLFullPath(pBuilder->mpSceneManager->mSrcFileColl[pAr->miSrcIdx]);
	}
//	TRACE( "close include\n");
	return 0;
}

mlString GetFileName(const wchar_t* apwcPathAndFileName){
  mlString sFileName = apwcPathAndFileName;  // Copy to make modifications.

  // Find the last "\" in the string and return everything after it.
  mlString::size_type pos = sFileName.find('\\');
  while(pos != mlString::npos){
	sFileName.erase(0, pos+1);
	pos = sFileName.find('\\');
  } // while
  return sFileName;
} // GetFileName

xmlParserInputPtr mlRMMLBuilder::CreateInputStream(xmlParserCtxtPtr ctxt, mlXMLArchive* apAr, xmlCharEncoding encoding){
	xmlParserInputBufferPtr inputBuffer;
	inputBuffer = mpLibXml2Funcs->xmlAllocParserInputBuffer(encoding);
	if (inputBuffer == NULL)
	{
		mlTrace(cx, "Cannot allocate XML parser input buffer");
		return NULL;
	}
	inputBuffer->context = apAr; // ctxt;
	inputBuffer->readcallback = InputReadCallback;
	inputBuffer->closecallback = InputCloseCallback;
	// 
	xmlParserInputPtr pParserInput = mpLibXml2Funcs->xmlNewIOInputStream(ctxt, inputBuffer, encoding);
	if(mpLibXml2Funcs->xmlSetInputStreamFilePath && !apAr->msSrc.empty()){
//		mlString sFileName = GetFileName(apAr->msSrc.c_str());
		mpLibXml2Funcs->xmlSetInputStreamFilePath(pParserInput, cLPCSTR(apAr->msSrc.c_str()), NULL);
	}
	return pParserInput;
}

xmlParserCtxtPtr mlRMMLBuilder::CreateStreamParserCtxt(mlXMLArchive* apAr){
	xmlParserCtxtPtr ctxt;
	char* directory = NULL;

	ctxt = mpLibXml2Funcs->xmlNewParserCtxt();
	if (ctxt == NULL){
		mlTrace(cx, "Cannot allocate XML parser context");
		return NULL;
	}

	ctxt->linenumbers = 1;

	xmlParserInputPtr inputStream = CreateInputStream(ctxt, apAr);
	if (inputStream == NULL){
		mpLibXml2Funcs->xmlFreeParserCtxt(ctxt);
		return(NULL);
	}

	// inputPush(ctxt, inputStream);
	ctxt->inputTab[ctxt->inputNr] = inputStream;
	ctxt->input = inputStream;
	ctxt->inputNr++;

	//	if ((ctxt->directory == NULL) && (directory == NULL))
	//		directory = xmlParserGetDirectory(filename);
	//	if ((ctxt->directory == NULL) && (directory != NULL))
	//		ctxt->directory = directory;

	return(ctxt);
}

int mlRMMLBuilder::SAXUserParseStream(xmlSAXHandlerPtr sax, mlXMLArchive* apAr){
	int ret = 0;

	mpParserCtxt = CreateStreamParserCtxt(apAr);
	if (mpParserCtxt == NULL)
		return -1;
	// mpLibXml2Funcs->xmlFree(mpParserCtxt->sax);
	xmlSAXHandlerPtr pOldSaxHandler = mpParserCtxt->sax;
	mpParserCtxt->sax = sax;
	//mpParserCtxt->begin_line = 0;
	//mpParserCtxt->end_line = 0;
	mpParserCtxt->userData = this;

	int iResult = mpLibXml2Funcs->xmlParseDocument(mpParserCtxt);	

	if (mpParserCtxt->wellFormed)
		ret = 0;
	else{
		if (mpParserCtxt->errNo != 0)
			ret = mpParserCtxt->errNo;
		else
			ret = -1;
	}
	if (sax != NULL)
		mpParserCtxt->sax = NULL;
	if (mpParserCtxt->myDoc != NULL){
		mpLibXml2Funcs->xmlFreeDoc(mpParserCtxt->myDoc);
		mpParserCtxt->myDoc = NULL;
	}
	mpParserCtxt->sax = pOldSaxHandler;
	mpLibXml2Funcs->xmlFreeParserCtxt(mpParserCtxt);
	return ret;
}

mlresult mlRMMLBuilder::Include(const wchar_t* apwcSrc){
//		TRACE( "include %S\n", apwcSrc);
	int iCurXMLSrcIdx = ((mlXMLArchive*)(mpParserCtxt->input->buf->context))->miSrcIdx;
	mlString sCurXMLSrc = mpSceneManager->mSrcFileColl[iCurXMLSrcIdx];
	mlString sPath = mpSceneManager->RefineResPath(sCurXMLSrc, apwcSrc);
	mlString sLCPath = sPath;
	wchar_t* pwc = (wchar_t*)sLCPath.c_str();
	for(; *pwc != L'\0'; pwc++){ *pwc = tolower(*pwc); }
if(sPath.find(L"recordsServiceTest.xml") != mlString::npos){
sLCPath = sPath;
for(; *pwc != L'\0'; pwc++){ *pwc = tolower(*pwc); }
}
//if(sLCPath.find(L"resimage.xml") != mlString::npos)
//int kk=0;
//if(sPath.find(L"notesViewBase.xml") != mlString::npos)
//int hh=0;
//if(isEqual(sPath.c_str(), L":rmcommon:scripts\include\notesViewBase.xml"))
//int hh=0;
	// если файл уже подключен 
	if(mvParents.size() == 0){ // и подключаетс€ не в элементе, 
		std::vector<MP_WSTRING>::const_iterator cvi = mvIncludes.begin();
		bool bAlreadyIncluded = false;
		for(; cvi != mvIncludes.end(); cvi++){
const wchar_t* pwcTemp = (*cvi).c_str();
			if(isEqual((*cvi).c_str(), sLCPath.c_str())){
				bAlreadyIncluded = true;
			}
		}
		if(!bAlreadyIncluded){
			bAlreadyIncluded = mpSceneManager->mScriptPreloader->IsAlreadyIncluded(sLCPath.c_str());
		}
		if(bAlreadyIncluded){
			// то не подключать его повторно
			int iSrcFilesIdx = ((mlXMLArchive*)(mpParserCtxt->input->buf->context))->miSrcIdx; // pRMB->mpCurAr->miSrcIdx;
			int iSrcLine = mpParserCtxt->input->line; 
			mlTraceWarning(cx, "Reincluding of file %S (skiped in %S:%d)\n", sPath.c_str(), GPSM(cx)->mSrcFileColl[iSrcFilesIdx], iSrcLine);
			return ML_FALSE;
		}
		mvIncludes.push_back(MAKE_MP_WSTRING(sLCPath.c_str()));
		mpSceneManager->mScriptPreloader->RegIncludedScript(sLCPath.c_str());
	}else{
int hh=0;
	}

	mlXMLArchive* pAr = NULL;
	if(ML_FAILED(mpSceneManager->OpenXMLArchive(sPath.c_str(), pAr, false))){
		return ML_ERROR_NOT_AVAILABLE;
	}
	pAr->mpBuilder = this;
	mpCurAr = pAr;
	mpBuildEvalCtxt->SetXMLFullPath(sPath.c_str());
	//
	xmlParserInputPtr inputStream = CreateInputStream(mpParserCtxt, pAr);
	if(inputStream == NULL){
		TRACE(cx, "Cannot create input stream (%S)\n",apwcSrc);
		return ML_ERROR_FAILURE;
	}
	if(mpParserCtxt->inputNr >= mpParserCtxt->inputMax){
		mlTrace(cx, "Maximum include depth (%d) is riched\n", mpParserCtxt->inputMax);
		return ML_ERROR_FAILURE;
	}
	mpParserCtxt->inputTab[mpParserCtxt->inputNr] = inputStream;
	mpParserCtxt->input = inputStream;
	mpParserCtxt->inputNr++;
	//
	// skip header <?xml version="1.0" encoding="UTF-8"?>
	xmlCharEncoding encoding = XML_CHAR_ENCODING_NONE;
	std::istream* strm = pAr->mpIS;
//	pAr->mpRes->GetIStream(strm);
	if(strm){
		char pcHeader[500];
		char* pcHdr = pcHeader;
		char prevch = ' ';
		unsigned short overflowCounter = 0;
		unsigned short overflowMaxValue = sizeof(pcHeader)/sizeof(pcHeader[0]) - 1;
		for(char ch = strm->get(); overflowCounter<overflowMaxValue && !strm->eof(); ch = strm->get(), ++overflowCounter){
			*(pcHdr++) = ch;
			if(prevch == '?' && ch == '>')
				break;
			prevch = ch;
		}
		*pcHdr = '\0';
		std::string sHeader = pcHeader;
		int pos = sHeader.find("encoding");
		if(pos != std::string::npos){
			int posQt1 = sHeader.find('\"', pos);
			int posQt2 = sHeader.find('\"', posQt1+1);
			std::string sEncoding = sHeader.substr(posQt1+1, posQt2-posQt1-1);
			xmlCharEncodingHandlerPtr handler = mpLibXml2Funcs->xmlFindCharEncodingHandler(sEncoding.c_str());
			if(handler != NULL)
				mpLibXml2Funcs->xmlSwitchInputEncoding(mpParserCtxt, inputStream, handler);
		}
	}
	return ML_OK;
}

#endif

mlresult mlRMMLBuilder::Build(const wchar_t* apwcSrc, mlSceneManager* apSceneManager, bool abUI, mlRMMLElement* apRootElem)
{
	ML_ASSERTION(cx, apwcSrc!=NULL && apSceneManager!=NULL,"mlRMMLBuilder::Build");
	mpRootElement = apRootElem;
	//mvIncludes.clear();
//	if(true){
	if(mpLibXml2Funcs == NULL){
		mlXMLArchive* pXMLArchive=NULL;
		if(ML_FAILED(apSceneManager->OpenXMLArchive(apwcSrc,pXMLArchive)))
			return ML_ERROR_NOT_AVAILABLE;
		Build(pXMLArchive,apSceneManager,abUI);
		apSceneManager->CloseXMLArchive(pXMLArchive);
		return ML_OK;
	}

#ifdef RMML_USE_LIBXML2
	mlXMLArchive* pAr = NULL;
	if(ML_FAILED(apSceneManager->OpenXMLArchive(apwcSrc, pAr, false))){
		return ML_ERROR_NOT_AVAILABLE;
	}
	pAr->mpBuilder = this;
	mpCurAr = pAr;

	mpSceneManager = apSceneManager;
	cx = mpSceneManager->cx;
	mbIn3DScene = false;
	mbInViewport = false;
	mpLastScene = NULL;
	mpBuildEvalCtxt->SetXMLFullPath(mpSceneManager->mSrcFileColl[pAr->miSrcIdx]);
	if ( abUI) // Vladimir 2011.11.24
		mpSceneManager->GetContext()->mpApp->SendMessage2Launcher( APP_PERCENT, 82);

	int res = SAXUserParseStream(mlSAX2Handler, pAr);
	if (res != 0){ // error
		xmlErrorPtr pErr = mpLibXml2Funcs->xmlGetLastError();
		if (pErr != NULL && pErr->message != NULL){ //  && pErr->code == XML_IO_LOAD_ERROR
//			mlTraceError(cx, "XML load error: %s\n",pErr->message);
		}
	}
//	xmlCleanupParser();

	if ( abUI) // Vladimir 2011.11.24
		mpSceneManager->GetContext()->mpApp->SendMessage2Launcher( APP_PERCENT, 89);

	SetScene(mpLastScene, abUI, apwcSrc);
	if(mpLastScene != NULL){
		GUARD_JS_CALLS(cx);
		FREE_FOR_GC(cx, JS_GetGlobalObject(cx), mpLastScene->mjso);
	}
#endif
//onShowInfoState
	return ML_OK;
}

using namespace gd_xml_stream;

mlresult mlRMMLBuilder::Save(mlXMLArchive* apAr, mlSceneManager* apSceneManager){
	gdmlXMLArchive* pAr=apAr->mpAr;
//*pAr << Child << _U("rmml");
//*pAr << Child << _U("image") << Attribute << _U("CHAR") << (char)'A' << EndNode;
//*pAr << Child << _U("image") << EndNode;
//*pAr << Child << _U("DATA") << Attribute << _U("CHAR") << (char)'A' 
//                              << Attribute << _U("SHORT") << (short)100
//                              << Attribute << _U("LONG") << (long)12345678
//                              << Attribute << _U("DOUBLE") << (double)1234.00998877 << EndNode;
//*pAr << Child << _U("DATA") << Attribute << _U("CHAR") << (char)'A' 
//                              << Attribute << _U("DOUBLE") << (double)1234.00998877;
//*pAr << Child << _U("TEXT") << _U("This is a sample how you can serialize data like the ")
//                                 _U("MFC CArchive class");
//*pAr << End;
//return OMS_OK;
	*pAr << Child << L"rmml";
		*pAr << Attribute << L"xmlns:xsi" << L"http://www.w3.org/2001/XMLSchema-instance";
		// сохран€ем параметры сцены
		mlRMMLElement* pMLElScene=apSceneManager->mpScene;
		*pAr << Child << L"scene";
		*pAr << Attribute << L"_name" << pMLElScene->GetName();
		mlPoint pnt=pMLElScene->GetVisible()->GetAbsXY();
		if(pnt.x != 0){	*pAr << Attribute << L"_x" << pnt.x; }
		if(pnt.y != 0){	*pAr << Attribute << L"_y" << pnt.y; }
		// перебираем все элементы
		JSContext* mcx=apSceneManager->cx;
		mlRMMLIterator Iter(mcx,pMLElScene);
		int iOldDepth=Iter.GetDepth();
		mlRMMLElement* pElem=NULL;
		while((pElem=Iter.GetNext())!=NULL){
			mlRMMLElement* pTempElem=apSceneManager->CreateElementByType(pElem->GetType());
			int iDepth=Iter.GetDepth();
			if(iDepth > iOldDepth){
				*pAr << Child; 
			}else{
				*pAr << EndNode;
				*pAr << Child; 
			}
			iOldDepth=iDepth;
			*pAr << pElem->GetTagName().c_str();
mlTrace(cx, "%S\n",pElem->GetFullPath().c_str());
//mlString wsTN=pElem->GetTagName();
			// сохран€ем атрибуты
			{	mlSynchData DataProps;
				if(pElem->GetPropertiesInfo(DataProps)){
					DataProps.reset();
					mldPropertiesInfo PropsInfo;
					DataProps.get(&PropsInfo,sizeof(mldPropertiesInfo));
					unsigned short us;
					DataProps >> us;
					for(int ij=0; ij<PropsInfo.numGroup; ij++){
						mldPropGroupInfo PropGroupInfo;
						DataProps.get(&PropGroupInfo,sizeof(mldPropGroupInfo));
						for(int iProp=0; iProp<PropGroupInfo.numPropeties; iProp++){
							char* pszPropName=NULL;
							DataProps >> pszPropName;
							mlString wsPropName=cLPWCSTRq(pszPropName);
							char cPropType=0;
							DataProps >> cPropType;
							wchar_t* pwcVal=NULL;
							DataProps >> pwcVal;
							// сравниваем со значением по умолчанию
							if(!pElem->NeedToSave(PropGroupInfo.type,wsPropName.c_str(),pwcVal,pTempElem))
								continue;
							//
							*pAr << Attribute << wsPropName.c_str();
							*pAr << pwcVal;
						}
					}
				}
			}
			if(pElem->mRMMLType==MLMT_SCRIPT){
				mlRMMLScript* pScript=(mlRMMLScript*)pElem;
				if(pElem->mType==MLST_EVENT || pElem->mType==MLST_WATCH){
					mlString wsEvOrW=pScript->GetEventOrWatchName();
					*pAr << Attribute;
					if(pElem->mType==MLST_EVENT)
						*pAr << L"event";
					else
						*pAr << L"watch";
					*pAr << wsEvOrW.c_str();
				}
			}
			// сохран€ем значение (скрипта или текста)
			mlString sVal=pElem->GetValue((iDepth+2)*8);
			if(!sVal.empty()){
				if(sVal[sVal.length()-1]==L'\n'){
					sVal.resize(sVal.length()-1);
				}
				sVal.insert(0,L"\n");
				*pAr << sVal.c_str();
			}
			if(pTempElem!=NULL && !pTempElem->IsJSO())
				 MP_DELETE( pTempElem);
		}
	*pAr << End;
	apSceneManager->CallGC();
	return ML_OK;
}

}
