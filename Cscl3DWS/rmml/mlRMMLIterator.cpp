#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml{

mlRMMLIterator::mlRMMLIterator(JSContext* acx, mlRMMLElement* apElStart, bool abChildrenFirst, bool abInclStart):
	MP_STACK_INIT(mState)
{
	cx = acx;
	mbChildrenFirst = abChildrenFirst;
	mbInclStart = abInclStart;
	Reset(apElStart);
}

void mlRMMLIterator::Reset(mlRMMLElement* apElStart){
	ClearState();
	if(apElStart == NULL) return;
	if(!mbChildrenFirst && !mbInclStart)
		if(apElStart->mpChildren == NULL) return;
	mState.push(Composition(apElStart));
//mpaState = mState.begin();
}

mlRMMLIterator::~mlRMMLIterator(){
	ClearState();
	//mState.clear();
}

void mlRMMLIterator::ClearState(){
	//mState.clear();
	while(!mState.empty()) mState.pop();
	//mState.reserve(100);
}

mlRMMLElement* mlRMMLIterator::GetNext(){
	if(mState.size() == 0) return NULL;
	// берем элемент с вершины стека
	Composition* pComp = &(mState.top());
	mlRMMLElement* pMLComp = pComp->pComposition;
	if(mbChildrenFirst){
		// включая стартовый элемент
		if(pMLComp->mpChildren==NULL){
			mState.pop();
//mpaState = mState.begin();
			return pMLComp;
		}
		while(1){
			// если обошли уже все дочерние элементы
			if((++(pComp->iCurChild)) >= (int)(pMLComp->mpChildren->size())){
				// то удаляем элемент из стека
				mState.pop();
				// и возвращаем его
				return pMLComp;
			}
			// берем текущий дочерний элемент
			mlRMMLElement* pMLEl = (*(pMLComp->mpChildren))[pComp->iCurChild];
			// если у него нет детей, то его и возвращаем
			if(pMLEl->mpChildren == NULL) return pMLEl;
			// если есть, то ложим его на стек
			mState.push(Composition(pMLEl));
//mpaState = mState.begin();
			pComp = &(mState.top());
			// и повторяем процедуру
			pMLComp = pComp->pComposition;
		}
		
	}
	// если обошли уже все дочерние элементы
	while((++(pComp->iCurChild)) >= (int)(pMLComp->mpChildren->size())){
		// то удаляем элемент из стека
		mlRMMLElement* pMLEl = pComp->pComposition;
		mState.pop();
//mpaState = mState.begin();
		// если стек пуст, то обход закончен
		if(mState.size() == 0){
			if(mbInclStart)
				return pMLEl;
			return NULL;
		}
		// проверяем следующий элемент с вешины стека
		pComp = &(mState.top());
		pMLComp = pComp->pComposition;
	}
	// берем текущий дочерний элемент
	mlRMMLElement* pMLEl = (*(pMLComp->mpChildren))[pComp->iCurChild];
	// если у него нет детей, то его и возвращаем
	if(pMLEl->mpChildren == NULL) return pMLEl;
	// если есть, то ложим его на стек и возвращаем
	mState.push(Composition(pMLEl));
//mpaState = mState.begin();
	return pMLEl;
}

}
