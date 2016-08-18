#pragma once

#include <vector>

namespace rmml {

struct mlTimeout
{
	enum ETimeoutType{
		TMOT_EVAL,
		TMOT_FUNC,
		TMOT_METHOD
	} meType;
	unsigned short musID;
	MP_WSTRING msStr;
	JSObject* mjsoObj;
	MP_VECTOR<jsval> mvArgs;
	unsigned int miInterval;
	bool mbInterval;
	__int64 mlStartTime;
	bool mbChecked;
	mlTimeout():
		MP_VECTOR_INIT(mvArgs),
		MP_WSTRING_INIT(msStr)
	{
		mjsoObj = NULL;
		mlStartTime = 0;
		mbChecked = false;
	}

	mlTimeout(const mlTimeout& other):
		MP_VECTOR_INIT(mvArgs),
		MP_WSTRING_INIT(msStr)
	{
		musID = other.musID;
		msStr = other.msStr;
		mjsoObj = other.mjsoObj;
		mvArgs = other.mvArgs;
		miInterval = other.miInterval;
		mbInterval = other.mbInterval;
		mlStartTime = other.mlStartTime;
		mbChecked = other.mbChecked;
	}
};

#define EVNM_onBoundsChange L"onBoundsChange"

/**
 * Класс композиций RMML
 */
class mlRMMLComposition :	
							public mlRMMLElement,
							public mlJSClass, 
							public mlRMMLVisible,
							public mlRMMLPButton
{
//friend class mlRMMLIterator;
friend class mlRMMLBuilder;
friend class mlRMMLVisible;
	mlRect mBounds;
	bool bInvalidBounds;
	MP_VECTOR<mlTimeout> mvTimeouts;
	bool bTimeoutsChanged;
	bool mbDisableChildButtons;	// =true: если у композиции выставлены кнопочные обработчики, то выдавать ее в качестве кнопки, а не дочернюю кнопку
public:
	mlRMMLComposition(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLComposition(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP__final = 1,
		EVID_onBoundsChange
	};
	JSFUNC_DECL(getBounds)
	JSFUNC_DECL(setTimeout)
	JSFUNC_DECL(setInterval)
	JSBool setTimeout(uintN argc, jsval *argv, jsval *rval, bool abInterval, const char* apcFuncName);
	JSFUNC_DECL(clearTimeout)
	JSFUNC_DECL(clearInterval)
	JSFUNC_DECL(handleButtonEvents)
	JSFUNC_DECL(disableChildButtons)
	JSBool clearTimeout(uintN argc, jsval *argv, jsval *rval, bool abInterval, const char* apcFuncName);
	void ClearTimeouts(bool abDontFreeForGC = false);
	void FreeTimeoutForGC(mlTimeout &aTimeout);
protected:
	void InvalidBounds();
	void UpdateBounds();
	void SetMask(const mlMask* apMask = NULL);		// обновить маску композиции у дочерних элементов
public:
	bool IsChildButtonsDisabled(){ return mbDisableChildButtons; }

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx){ return ML_OK; }
	mlresult Load(){ return ML_OK; }
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult Update(const __int64 alTime);
	mlRMMLElement* GetButtonUnderMouse(mlPoint &aMouseXY, mlButtonUnderMouse &aBUM);
	bool Freeze(){ 
		if(!mlRMMLElement::Freeze()) return false; 
		return FreezeChildren(); 
	}
	bool Unfreeze(){ 
		if(!mlRMMLElement::Unfreeze()) return false; 
		return UnfreezeChildren(); 
	}
	void GetState(mlSynchData &aData);
	void SetState(mlSynchData &aData);

// реализация mlRMMLVisible
public:
	mlRMMLElement* GetVisibleAt(const mlPoint aPnt, mlIVisible** appVisible=NULL);
	//void SetMask(mlMedia* aMask, int aiX=0, int aiY=0);
	//void SetMask(int aiX, int aiY, int aiW, int aiH);

// реализация  mlIVisible
MLIVISIBLE_IMPL
	mlRect GetBounds();
	virtual int IsPointIn(const mlPoint aPnt); // -1 - not handled, 0 - point is out, 1 - point is in

// реализация  mlIButton
MLIBUTTON_IMPL

};

}
