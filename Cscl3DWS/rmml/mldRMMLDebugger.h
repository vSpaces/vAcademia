#pragma once

namespace rmmld{
class mldRMMLDebugger;
}
#define RMML_DEBUG

#include "mlMapWCS2Int.h"
//#include "bimap.h"
#include "rmmld.h"

using namespace rmml;

namespace rmmld{

class mldRMMLDebugger: public mlIDebugger{
	mlSceneManager* mpSM;
	mlRMMLIterator* mpIter;
	mlRMMLElement* mpCurMLEl;
	MP_STRING msPropVal;
	mlSynchData mDataProps;
	mlSynchData mDataEvents;
	mlSynchData mDataTree;
	mlSynchData mDataTextFormatting;
	MP_STRING msTagName;
	class mldID2Path{
	private:
		typedef MP_MAP<int, MP_WSTRING> MapID2Path;
		MapID2Path mmapID2Path;
		mlMapWCS2Int* mpPath2ID;
		void buildPath2ID();
		void destroyPath2ID(){
			if(mpPath2ID!=NULL) MP_DELETE( mpPath2ID);
			mpPath2ID=NULL;
		}
	public:
		mldID2Path():
			MP_MAP_INIT(mmapID2Path)
		{ 
			mpPath2ID=NULL; 
		}
		~mldID2Path(){
			clear();
		}
		void clear(){
			destroyPath2ID();
		}
		int setPath(int aiID, const wchar_t* apwcPath){
			if(apwcPath == NULL) return -1;
			mmapID2Path.insert(MapID2Path::value_type(aiID, MAKE_MP_WSTRING(apwcPath)));
			destroyPath2ID();
			return aiID;
		}
		const wchar_t* getPath(int aiID){
			MapID2Path::iterator mi = mmapID2Path.find(aiID);
			if(mi == mmapID2Path.end()) return NULL;
			return mi->second.c_str();
		}
		int getIDByPath(const wchar_t* apwcPath){
			buildPath2ID();
			return mpPath2ID->find((wchar_t*)apwcPath);
		}
		void Dump();
	}mID2Path;
public:
	mldRMMLDebugger(mlISceneManager* apSM):
		MP_STRING_INIT(msPropVal),
		MP_STRING_INIT(msTagName)
	{
		mpSM=(mlSceneManager*)apSM;
		mpIter=NULL;
		mpCurMLEl=NULL;
		Reset();
	}
	~mldRMMLDebugger(){
		if(mpIter) MP_DELETE( mpIter);
	}
// реализация mlIDebugger
public:
	char* GetSceneName();
	int GetSceneChildNum();
	void Reset();
	bool Next();
	int GetTreeDepth();
	// информация о текущем элементе
	char* GetName();
	int GetChildNum();
	bool IsComposition();
	bool IsVisible();
	bool IsText();
	// информация о видимом элементе
	mlPoint GetXY();
	mlSize GetSize();
	int GetDepth();
	bool GetVisible();
	mlRect GetBounds();
	// информаци о тексте
	mlString GetValue();
	//
	bool GetTree(mlSynchData &aData);
	bool GetProperties(mlSynchData &aData, unsigned int aiElemID);
	bool GetJSTree(mlSynchData &aData);
	bool GetUITree(mlSynchData &aData);

};

}

