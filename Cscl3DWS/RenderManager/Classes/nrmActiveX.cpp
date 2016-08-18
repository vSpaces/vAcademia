// nrmActiveX.cpp: implementation of the nrmActiveX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmActiveX.h"
//#include "n3dpluginsupport.h"

#include <locale>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

nrmActiveX::nrmActiveX(mlMedia* apMLMedia) : nrmObject(apMLMedia)
{
	m_type = AXT_UNKNOWN;
/*	piwindow = NULL;
	piactivex = NULL;
	pivisibleobject = NULL;*/
}

nrmActiveX::~nrmActiveX()
{
}

void nrmActiveX::FrozenChanged()
{
	//if( !piwindow)	return;
//	BOOL bFrozen = GetMLMedia()->GetFrozen();
	//piwindow->freeze(bFrozen);
}

using std::string;
using std::wstring;

wstring GetFileExtension(const wchar_t* apwcSrc){
	// находим последнюю точку
	wchar_t* pwcDot=(wchar_t*)apwcSrc+wcslen(apwcSrc)-1;
	for(; pwcDot!=apwcSrc; pwcDot--)
		if(*pwcDot==L'.') break;
	if(pwcDot==apwcSrc) return L"";
	// выделяем расширение из строки
	wstring wsExt=pwcDot+1;
	// удаляем конечные пробельные символы
	for(;;){
		if(wsExt.empty()) return L"";
		wchar_t wc=*wsExt.end();
		if(!(wc==L' ' || wc==L'\t')) break;
		wsExt.erase(wsExt.end());
	}
	// русские символы не трогаем (быстрое преобразование)
/*#if _MSC_VER > 1200
	//assert( FALSE);	// не компилится под VC2003
	std::use_facet<std::ctype<wchar_t> >(	\
			std::locale::locale(),NULL,true).\
			tolower((std::ctype<wchar_t>::_Elem*)wsExt.begin()._Myptr, 
			(std::ctype<wchar_t>::_Elem*)wsExt.end()._Myptr);
#else
	std::use_facet<std::ctype<wchar_t> >(std::locale::locale(),NULL,true).tolower(wsExt.begin(), wsExt.end());
#endif*/
	return wsExt;
}

wstring nrmActiveX::DefCLSIDBySrc(const wchar_t* /*apwcSrc*/){
	/*if(apwcSrc==NULL) return L"";
	m_type = AXT_UNKNOWN;
	wstring wsExt=GetFileExtension(apwcSrc);
	if(wsExt==L"html" || wsExt==L"htm"|| wsExt==L"ppt"){
		m_type = AXT_WEBBROWSER;
		return L"{8856F961-340A-11D0-A96B-00C04FD705A2}"; // WebBrowser control
	}
	if(wsExt==L"swf"){
		m_type = AXT_FLASH;
		return L"{D27CDB6E-AE6D-11cf-96B8-444553540000}"; // Flash control 
	}
	assert( FALSE);*/
	return L"";
}

// загрузка нового ресурса
bool nrmActiveX::SrcChanged()
{
	/*const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	if(pwcSrc==NULL) return false;
	AFX_TYPES	a_type = m_type;
	wstring wsCLSID=DefCLSIDBySrc(pwcSrc);
	if(wsCLSID.empty()) return false;
	assert(m_pRenderManager);
	assert(m_pRenderManager->GetResourceManager());
	if( !m_pRenderManager->GetResourceManager())
	{
		rm_trace("Error! nrmActiveX::SrcChanged() - resource manager is not set.");
		return false;
	}
	wstring sSRC=m_pRenderManager->GetResourceManager()->GetFullPath(pwcSrc);
//	if(sSRC.empty()) return false;
	mlSize sz=mpMLMedia->GetIVisible()->GetSize();
	if(sz.width==0)
	{
		sz.width=100;
		mpMLMedia->GetIVisible()->SetSize(sz);
	}
	if(sz.height==0)
	{
		sz.height=100;
		mpMLMedia->GetIVisible()->SetSize(sz);
	}
	mlPoint pnt=mpMLMedia->GetIVisible()->GetAbsXY();
	//
	if( !piactivex || m_type!=a_type)
	{
		if( pibase)
		{
			//delete n3d_obj;
			get_iobjslib()->destroy_object(pibase);
			pibase = NULL;
			piactivex = NULL;
		}
		RECT	controlWidth={pnt.x,pnt.y,pnt.x+sz.width,pnt.y+sz.height};
		if( m_type==AXT_WEBBROWSER)
		{
			//n3d_obj = new CWebBrowser(get_engine()->m_hWnd,this);
			//((CWebBrowser*)n3d_obj)->create( get_engine()->m_hWnd, &controlWidth, FALSE);
			piactivex = get_iobjslib()->create_webbrowser(get_iengine()->get_device_window(), this, &controlWidth, FALSE);
		}
		else
		{
			//n3d_obj = new CActiveXHostWnd(get_engine()->m_hWnd,this);
			//n3d_obj->create( wsCLSID.c_str(), NULL, get_engine()->m_hWnd, &controlWidth, FALSE);
			piactivex = get_iobjslib()->create_activex(wsCLSID.c_str(), get_iengine()->get_device_window(), this, &controlWidth, FALSE);
		}
	}
	if( piactivex)
	{
		GET_INTERFACE(ibase, piactivex, &pibase);
		GET_INTERFACE(iwindow, piactivex, &piwindow);
		GET_INTERFACE(ivisibleobject, piactivex, &pivisibleobject);
		assert(pibase);
		assert(piwindow);
		assert(pivisibleobject);
		USES_CONVERSION;
		wchar_t*	wcName = GetMLMedia()->GetStringProp("name");
		if( wcName != NULL)
			pivisibleobject->set_name(W2A(wcName));
	}
	else
	{
		return false;
	}
	if( m_pRenderManager->GetResourceManager())
	{
		sSRC = m_pRenderManager->GetResourceManager()->GetFullPath(pwcSrc);
	}
	//n3d_obj->set_wh(0, sz.width, sz.height);
	//n3d_obj->set_xy(0, pnt.x, pnt.y);
	piwindow->move(pnt.x, pnt.y);
	piwindow->resize(sz.width, sz.height);
	if(m_type==AXT_WEBBROWSER)
	{
		static BYTE parms[] =
			VTS_BSTR VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
		piactivex->invoke_helper(0x68, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 cLPCSTR(sSRC.c_str()), NULL, NULL, NULL, NULL);
	}
	else if(m_type==AXT_FLASH)
	{
		static BYTE parms[] = VTS_BSTR;
		piactivex->invoke_helper(0x66, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
			 cLPCSTR(sSRC.c_str()));
	}

	AbsDepthChanged();
	AbsVisibleChanged();*/
	return true;
}

// изменилось абсолютное положение 
void nrmActiveX::AbsXYChanged()
{
	/*if(!piwindow)	return;
	mlPoint pnt=mpMLMedia->GetIVisible()->GetAbsXY();
	piwindow->move(pnt.x, pnt.y);*/
} 

void nrmActiveX::SizeChanged()
{
	/*if(!piwindow)	return;
	mlSize sz=mpMLMedia->GetIVisible()->GetSize();
	piwindow->resize(sz.width, sz.height);*/
}

// изменилась абсолютная глубина
void nrmActiveX::AbsDepthChanged()
{
	/*if( !pibase)	return;
	isprite*	pisprite=NULL;
	GET_INTERFACE(isprite, pibase, &pisprite);
	if( !pisprite)	return;
	pisprite->set_depth(GetMLMedia()->GetIVisible()->GetAbsDepth());*/
}

// изменилась абсолютная видимость
void nrmActiveX::AbsVisibleChanged()
{
	/*if(pivisibleobject== NULL) return;
	pivisibleobject->visible(GetMLMedia()->GetIVisible()->GetAbsVisible());*/
}

mlSize nrmActiveX::GetMediaSize(){
	mlSize size; ML_INIT_SIZE(size);
	return size;
}

__forceinline omsresult HResult2OMSResult(HRESULT hr){
	if(FAILED(hr)){
		switch(hr){
		case E_NOINTERFACE: return OMS_ERROR_NOT_INITIALIZED;
		case E_INVALIDARG: return OMS_ERROR_INVALID_ARG;
		}
		return OMS_ERROR_FAILURE;
	}
	return OMS_OK;
}

omsresult nrmActiveX::Call(wchar_t* /*apProcName*/, int /*aiArgC*/, 
						   ML_VARIANT* /*pvarArgs*/, ML_VARIANT* /*varRes*/){
	//if(piactivex==NULL) return OMS_ERROR_NOT_INITIALIZED;
	//HRESULT hr=piactivex->invoke(apProcName, aiArgC, (VARIANT*)pvarArgs, (VARIANT*)varRes);
	HRESULT hr = S_OK;
	return HResult2OMSResult(hr);
}

omsresult nrmActiveX::Set(wchar_t* /*apPropName*/,ML_VARIANT* /*vVar*/){
	//if(piactivex==NULL) return OMS_ERROR_NOT_INITIALIZED;
	//HRESULT hr=piactivex->put_prop(apPropName,(VARIANT*)vVar);
	HRESULT hr = S_OK;
	return HResult2OMSResult(hr);
}

omsresult nrmActiveX::Get(wchar_t* /*apPropName*/,ML_VARIANT* /*varRes*/){
	//if(piactivex==NULL) return OMS_ERROR_NOT_INITIALIZED;
	//HRESULT hr=piactivex->get_prop(apPropName,(VARIANT*)varRes);
	HRESULT hr = S_OK;
	return HResult2OMSResult(hr);
}

HRESULT nrmActiveX::CallExternal(const wchar_t* apProcName, 
								 DISPPARAMS *apDispParams, VARIANT *apVarResult, bool abEvent){
	int iArgC=0;
	ML_VARIANT* pArgV=NULL;
	if(apDispParams!=NULL){
		iArgC=apDispParams->cArgs;
		pArgV=(ML_VARIANT*)apDispParams->rgvarg;
	}
	mpMLMedia->GetIActiveX()->CallExternal(apProcName, iArgC, pArgV, (ML_VARIANT*)apVarResult, abEvent);
	return S_OK;
}