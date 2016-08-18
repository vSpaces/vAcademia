#pragma once

#include <stack>

namespace rmml{

/**
 * Итератор по иерархии RMML-объектов (обход в глубину)
 */
class RMML_NO_VTABLE mlRMMLIterator
{
	JSContext* cx;
	struct Composition{
		mlRMMLElement* pComposition;
		int iCurChild;
		Composition(mlRMMLElement* apComposition){ pComposition=apComposition; iCurChild=-1; }
	};	
	MP_STACK<Composition> mState;
//typedef Composition* CompositionPtr;
//CompositionPtr* mpaState;
	bool mbChildrenFirst;
	bool mbInclStart;
	void ClearState();
public:
	// abInclStart - включая стартовый элемент
	mlRMMLIterator(JSContext* acx, mlRMMLElement* apElStart, bool abChildrenFirst=false, bool abInclStart=false);
	~mlRMMLIterator(void);
	void Reset(mlRMMLElement* apStartComposition);
	mlRMMLElement* GetNext();
	int GetDepth(){ if(mState.size()==0) return 0; return mState.size()+((mState.size()>0 && mState.top().iCurChild==-1)?0:1)-1; }
};

}