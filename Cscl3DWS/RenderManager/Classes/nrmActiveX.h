// nrmActiveX.h: interface for the nrmActiveX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmActiveX_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmActiveX_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nrmanager.h"
#include "nrmObject.h"
#include "nrmMOIVisibleStub.h"

//////////////////////////////////////////////////////////////////////////
class nrmActiveX :	public nrmObject,
					public nrmMOIVisibleStub,
					public moIActiveX//,
					//public IActiveXHostWndExternal
{
	wstring DefCLSIDBySrc(const wchar_t* apwcSrc);
public:
	nrmActiveX(mlMedia* apMLMedia);
	virtual ~nrmActiveX();

	// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }
	moIVisible* GetIVisible(){ return this; }
//	moIButton* GetIButton(){ return this; }
	moIActiveX* GetIActiveX(){ return this; }
	void FrozenChanged();

	// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

	// реализация  moIVisible
public:
	// Свойства
	void AbsXYChanged(); // изменилось абсолютное положение 
	void SizeChanged();
	void AbsDepthChanged(); // изменилась абсолютная глубина
	void AbsVisibleChanged(); // изменилась абсолютная видимость
	void AbsOpacityChanged(){}; // изменилась абсолютная видимость
	mlSize GetMediaSize(); 
	int IsPointIn(const mlPoint /*aPnt*/){ return -1; }

	// реализация  moIButton
public:
	// реализация moIActiveX
	omsresult Call(wchar_t* apProcName, int aiArgC, 
						   ML_VARIANT* pvarArgs, ML_VARIANT* varRes);
	omsresult Set(wchar_t* apPropName,ML_VARIANT* vVar);
	omsresult Get(wchar_t* apPropName,ML_VARIANT* varRes);
	// реализация IActiveXHostWndExternal
	HRESULT CallExternal(const wchar_t* apProcName, 
						 DISPPARAMS *apDispParams, VARIANT *apVarResult, bool abEvent);

protected:
	enum AFX_TYPES{
		AXT_UNKNOWN,
		AXT_WEBBROWSER,
		AXT_FLASH
	}	m_type;
};

#endif // !defined(AFX_nrmActiveX_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
