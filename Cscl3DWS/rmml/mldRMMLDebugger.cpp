
#include "mlRMML.h" // for precompile heades only
#include "mldRMMLDebugger.h"

namespace rmmld{

using namespace rmml;

	std::auto_ptr<mldRMMLDebugger> gpDebugger;

mlIDebugger* CreateDebugger(mlISceneManager* apSM){
	if(gpDebugger.get()==NULL)
		gpDebugger = std::auto_ptr<mldRMMLDebugger>(new mldRMMLDebugger(apSM));
	return (mlIDebugger*)gpDebugger.get();
}

void mldRMMLDebugger::Reset(){
	if(mpIter) MP_DELETE( mpIter);
	MP_NEW_V2( mpIter, mlRMMLIterator, mpSM->cx, mpSM->mpScene);
}

char* mldRMMLDebugger::GetSceneName(){
	if(mpSM->mpScene == NULL) return NULL;
	if(mpSM->mpScene->_name == NULL) return NULL;
	return JS_GetStringBytes(mpSM->mpScene->_name);
}

int mldRMMLDebugger::GetSceneChildNum(){
	if(mpSM->mpScene==NULL) return 0;
	if(mpSM->mpScene->mpChildren==NULL) return 0;
	return mpSM->mpScene->mpChildren->size();
}

bool mldRMMLDebugger::Next(){
	if(!mpIter) return false;
	mpCurMLEl=mpIter->GetNext();
	if(mpCurMLEl==NULL) return false;
	return true;
}

int mldRMMLDebugger::GetTreeDepth(){
	if(!mpIter) return 0;
	return mpIter->GetDepth();
}

// информация о текущем элементе
char* mldRMMLDebugger::GetName(){
	if(mpCurMLEl->mRMMLType==MLMT_SCRIPT) return "script";
	if(mpCurMLEl->_name==NULL) return "undef";
	return JS_GetStringBytes(mpCurMLEl->_name);
}

bool mldRMMLDebugger::IsComposition(){
	return mpCurMLEl->IsComposition();
}

bool mldRMMLDebugger::IsVisible(){
	return mpCurMLEl->IsVisible();
}

bool mldRMMLDebugger::IsText(){
	return mpCurMLEl->mRMMLType==MLMT_TEXT;
}

// информация о видимом элементе
mlPoint mldRMMLDebugger::GetXY(){
	return mpCurMLEl->GetIVisible()->GetAbsXY();
}

mlSize mldRMMLDebugger::GetSize(){
	return mpCurMLEl->GetIVisible()->GetSize();
}

int mldRMMLDebugger::GetDepth(){
	return mpCurMLEl->GetIVisible()->GetAbsDepth();
}

bool mldRMMLDebugger::GetVisible(){
	return mpCurMLEl->GetIVisible()->GetAbsVisible();
}

mlRect mldRMMLDebugger::GetBounds(){
	return mpCurMLEl->GetVisible()->GetAbsBounds();
}

// информаци о тексте
mlString mldRMMLDebugger::GetValue(){
	return ((mlRMMLText*)mpCurMLEl)->GetValue();
}


int mldRMMLDebugger::GetChildNum(){
	if(mpCurMLEl->mpChildren==NULL) return 0;
	return mpCurMLEl->mpChildren->size();
}

bool mldRMMLDebugger::GetTree(mlSynchData &aData){
	aData.reset();
	mldElementInfo elInfSc;
	elInfSc.type=MLMT_SCENE;
	char* pszSceneName = GetSceneName();
	elInfSc.name[0] = '\0';
	if(pszSceneName != NULL)
	{
		//strcpy(elInfSc.name,pszSceneName);
		rmmlsafe_strcpy(elInfSc.name, sizeof(elInfSc.name), 0, pszSceneName);
	}
	elInfSc.id=0;
	elInfSc.numChild = GetSceneChildNum();
//mldElementInfo {
//	int type;
//	char name[100];
//	unsigned int id;
//	unsigned short numChild;
//};
	int iScenePos=aData.getPos();
	aData.put(&elInfSc,sizeof(mldElementInfo));
//	elInfSc.id=mID2Path.addPath(cLPWCSTR(GetSceneName()));
	elInfSc.id = mID2Path.setPath((int)mpSM->GetActiveScene(), cLPWCSTR(GetSceneName()));
	if(elInfSc.id < 0) return false;
	Reset();
	while(Next()){
		mldElementInfo elInf;
		elInf.type=mpCurMLEl->mRMMLType;
		if(mpCurMLEl->IsJSO()){
			JSClass* pClass=ML_JS_GETCLASS(mpSM->GetJSContext(),mpCurMLEl->mjso);
			const char* pszClassName=pClass->name;
			//strcpy(elInf.typen,pszClassName);
			rmmlsafe_strcpy(elInf.typen, sizeof(elInf.typen), 0, pszClassName);
		}else{
			if(mpCurMLEl->mRMMLType==MLMT_SCRIPT){
				//strcpy(elInf.typen,"Script");
				rmmlsafe_strcpy(elInf.typen, sizeof(elInf.typen), 0, "Script");
			}else{
				//strcpy(elInf.typen,"???");
				rmmlsafe_strcpy(elInf.typen, sizeof(elInf.typen), 0, "???");
			}
		}
		//
		if(mpCurMLEl->mRMMLType==MLMT_SCRIPT){
			if(mpCurMLEl->mType==MLMT_SCRIPT){
				//strcpy(elInf.name,"onLoad");
				rmmlsafe_strcpy(elInf.name, sizeof(elInf.name), 0, "onLoad");
			}else{
				if(mpCurMLEl->mType & MLSTF_NOTSET){
					elInf.type=mpCurMLEl->mType & ~MLSTF_NOTSET;
				}else{
					elInf.type=mpCurMLEl->mType;
				}
				mlRMMLScript* pMLElScr=(mlRMMLScript*)mpCurMLEl;
				//strcpy(elInf.name,cLPCSTRq(pMLElScr->GetEventOrWatchName().c_str()));
				rmmlsafe_strcpy(elInf.name, sizeof(elInf.name), 0, cLPCSTRq(pMLElScr->GetEventOrWatchName().c_str()));
			}
		}else if(mpCurMLEl->_name==NULL){
			//strcpy(elInf.name,"<unnamed>");
			rmmlsafe_strcpy(elInf.name, sizeof(elInf.name), 0, "onLoad");
//			elInf.name[0]='\0';
		}else{
			int iNameLen=JS_GetStringLength(mpCurMLEl->_name);
			if(iNameLen < 99){
				//strcpy(elInf.name,JS_GetStringBytes(mpCurMLEl->_name));
				rmmlsafe_strcpy(elInf.name, sizeof(elInfSc.name), 0, JS_GetStringBytes(mpCurMLEl->_name));
			}else{
				memcpy(elInf.name,JS_GetStringBytes(mpCurMLEl->_name),99);
				elInf.name[99]='\0';
				elInf.name[98]='.';
				elInf.name[97]='.';
				elInf.name[96]='.';
			}
		}
		elInf.numChild=GetChildNum();
		elInf.id = mID2Path.setPath((int)mpCurMLEl, mpCurMLEl->GetStringID().c_str());
		aData.put(&elInf,sizeof(mldElementInfo));
	}
	return true;
}

bool mldRMMLDebugger::GetProperties(mlSynchData &aData, unsigned int aiElemID){
	aData.reset();
	const wchar_t* pwcPath=mID2Path.getPath(aiElemID);
	if(pwcPath==NULL) return false;
	mlRMMLElement* pMLEl;
	if(aiElemID==0) pMLEl = mpSM->mpScene;
	else{
		wchar_t* pwcDot=(wchar_t*)findDot(pwcPath);
		pMLEl=mpSM->mpScene->FindElemByStringID(pwcDot+1);
	}
	if(pMLEl==NULL) return false;
	pMLEl->GetPropertiesInfo(aData);
	return true;
}

bool mldRMMLDebugger::GetJSTree(mlSynchData &aData){
	aData.reset();
	aData.setJSOutMode(mlSynchData::JSOM_TREE);
	aData << mpSM->cx;
	aData.setJSOutMode(mlSynchData::JSOM_STATE);
	return true;
}

bool mldRMMLDebugger::GetUITree(mlSynchData &aData){
	aData.reset();
	jsval v = JSVAL_VOID;
	JS_GetProperty(mpSM->cx, mpSM->mjsoPlayer, "UI", &v);
	if(JSVAL_IS_NULL(v) || JSVAL_IS_VOID(v) || !(JSVAL_IS_OBJECT(v)))
		return false;
	JSObject* jsoUI = JSVAL_TO_OBJECT(v);
	aData.setJSOutMode(mlSynchData::JSOM_TREE);
	aData << jsoUI;
	aData.setJSOutMode(mlSynchData::JSOM_STATE);
	return true;
}

void mldRMMLDebugger::mldID2Path::buildPath2ID(){
	if(mpPath2ID != NULL) return;
	mpPath2ID = MP_NEW( mlMapWCS2Int);
	MapID2Path::iterator it;
	for(it = mmapID2Path.begin(); it != mmapID2Path.end(); it++){
		mpPath2ID->insert((wchar_t*)it->second.c_str(),it->first);
	}
}

//void mldRMMLDebugger::mldID2Path::Dump(){
//	std::vector<wchar_t*>::iterator vi;
//	for(vi = mvID2Path.begin(); vi != mvID2Path.end(); vi++){
//		wchar_t* pwc = *vi;
//		mlTrace("%S\n", pwc);
//	}
//	
//}

//int mldRMMLDebugger::GetIDBySource(int aiStr, const wchar_t* apszSrcFileName){
//	// ??
//	return 0;
//}


}
 