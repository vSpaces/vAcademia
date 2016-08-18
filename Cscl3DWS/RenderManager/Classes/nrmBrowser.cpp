
#include "stdafx.h"
#include <locale>
#include <stdlib.h>
#include "nrmBrowser.h"
#include "BrowserPlugin.h"
#include "IDynamicTexture.h"
#include "HelperFunctions.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

nrmBrowser::nrmBrowser(mlMedia* apMLMedia) : 
	MP_WSTRING_INIT(m_lastExecScriptResult),
	nrmObject(apMLMedia), 
	m_browser(NULL),
	m_sprite(NULL)
{
}

nrmBrowser::~nrmBrowser()
{
	if (m_sprite){
		m_sprite->RemoveDynamicTextures();
	}

//	if(m_browser != NULL)
//		DeleteBrowserObject(m_browser);

	recreate_sprite(false);

}

// изменилось абсолютное положение 
void nrmBrowser::AbsXYChanged()
{
	if (!m_sprite)
	{
		return;
	}

	mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
	m_sprite->SetCoords(pnt.x, pnt.y);
} 

bool IsPowerOfTwo(int x)
{
    return x && ((x & (x - 1)) == 0);
}

void nrmBrowser::SetBrowserSize(mlSize& sz)
{
	int browserWidth = sz.width;
	int browserHeight = sz.height;

	if ((!IsPowerOfTwo(browserWidth)) && (!g->gi.IsNPOTSupport()))
	{
		browserWidth = GetMinBigger2st(browserWidth);
	}

	if ((!IsPowerOfTwo(browserHeight)) && (!g->gi.IsNPOTSupport()))
	{
		browserHeight = GetMinBigger2st(browserHeight);
	}

	m_browser->SetSize(browserWidth, browserHeight);
}

void nrmBrowser::SizeChanged()
{
	if(!m_browser)
		return;
	mlSize sz=mpMLMedia->GetIVisible()->GetSize();
	if(sz.width <= 0)
		sz.width = 2;
	if(sz.height <= 0)
		sz.height = 2;

	SetBrowserSize(sz);

	if (m_sprite)
	{
		m_sprite->SetSize(sz.width, sz.height);
	}
	
	//recreate_sprite();
	
	AbsDepthChanged();
	AbsXYChanged();
	AbsVisibleChanged();
}

void nrmBrowser::on_texture_size_changed(int width, int height)
{
	if (m_sprite)
	{
		mlSize sz = mpMLMedia->GetIVisible()->GetSize();
		m_sprite->SetWorkRect(0, 0, (width <= sz.width) ? width : sz.width, (height <= sz.height) ? height : sz.height);
	}
}

// изменилась абсолютная глубина
void nrmBrowser::AbsDepthChanged()
{
	if(m_sprite)
		m_sprite->SetDepth(GetMLMedia()->GetIVisible()->GetAbsDepth());
}

mlSize nrmBrowser::GetMediaSize(){
	mlSize size; ML_INIT_SIZE(size);
	return size;
}

bool nrmBrowser::recreate_sprite(bool create){
if (m_sprite)
return true;
	if (m_sprite)
	{
		m_sprite->DeleteChangesListener(this);
		gRM->scene2d->DeleteSprite(m_sprite->GetName());
		g->sprm.DeleteObject(m_sprite->GetID());
		m_sprite = NULL;
	}
	if(!create)
		return true;

	CComString name;
	name = "browser_";
	name += IntToStr(g->sprm.GetCount()).c_str();

	int spriteID = g->sprm.AddObject(name.GetBuffer(0));
	m_sprite = g->sprm.GetObjectPointer(spriteID);
	gRM->scene2d->AddSprite(spriteID, 0, 0);
//	m_sprite->SetVisibilityProvider(this);

	// IDynamicTexture* dynamicTexture = NULL;
	m_sprite->LoadFromDynamicTexture(m_browser->GetDynamicTexture());

	return true;
}

// загрузка нового ресурса
bool nrmBrowser::SrcChanged()
{
	const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
/*
	if (pwcSrc == NULL) 
	{
		return false;
	}

	USES_CONVERSION;
//	std::string sSrc = W2A(pwcSrc);

	CComString name;
	if (mpMLMedia->GetStringProp("name"))
	{
		name = W2A(mpMLMedia->GetStringProp("name"));
	}
	CComString src(W2A(pwcSrc));

	if (!m_sprite)
	{
		name += "_";
		name += src;
		name += "_";
		name += IntToStr(g->sprm.GetCount()).c_str();

		int spriteID = g->sprm.AddObject(name.GetBuffer(0));
		m_sprite = g->sprm.GetObjectPointer(spriteID);
		m_sprite->AddChangesListener(this);
		gRM->scene2d->AddSprite(spriteID, false, 0, 0);
	}

	if(m_browser == NULL)
		m_browser = CreateBrowserObject();

	mlSize sz = mpMLMedia->GetIVisible()->GetSize();
	m_sprite->SetSize(sz.width, sz.height);

	//if (!m_browser->Open((char *)path.c_str()))
	//{
	//	DeleteVideoObject(m_video);
	//	m_browser = NULL;
	//	return false;
	//}

//	mlSize sz;
//	sz.width = m_browser->GetDynamicTexture()->GetSourceWidth();
//	sz.height = m_browser->GetDynamicTexture()->GetSourceHeight();
//	mpMLMedia->GetIVisible()->SetSize(sz);

	AbsDepthChanged();
	AbsXYChanged();
	AbsVisibleChanged();

	return true;
*/

	/////////////////////////////////////////////////////////////////
	// jedi 2011.06.14
	// закомментирована подключение плагина браузера
	//USES_CONVERSION;

	//if(m_browser == NULL)
	//	m_browser = CreateBrowserObject(this, m_pRenderManager->GetMainWindow());

	//if(m_browser == NULL)
	//	return false;

	//m_browser->SetParentWindow(m_pRenderManager->GetMainWindow());

	//mlSize sz = mpMLMedia->GetIVisible()->GetSize();
	//SetBrowserSize(sz);

	//recreate_sprite();

	//AbsDepthChanged();
	//AbsXYChanged();
	//AbsVisibleChanged();

	//std::wstring wsSrc;
	//if(pwcSrc != NULL)
	//	wsSrc = pwcSrc;

	//if (!wsSrc.empty()){
	//	wsSrc = m_pRenderManager->GetResourceManager()->GetFullPath(wsSrc.c_str());
	//	m_browser->Navigate(wsSrc.c_str());
	//}else{
	//	ValueChanged();
	//}

	//m_pRenderManager->SetCSForChildWindows(GetCSForChildWindows());
	/////////////////////////////////////////////////////////////////

/*	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	if(pwcSrc==NULL) return false;
	AFX_TYPES	a_type = m_type;
	assert(m_pRenderManager);
	assert(m_pRenderManager->GetResourceManager());
	if( !m_pRenderManager->GetResourceManager())
	{
		rm_trace("Error! nrmBrowser::SrcChanged() - resource manager is not set.");
		return false;
	}
	wstring sSRC=m_pRenderManager->GetResourceManager()->GetFullPath(pwcSrc);
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
	if( !pibrowser)
	{
		//if( pibase)
		//{
			//get_iobjslib()->destroy_object(pibase);
			//pibase = NULL;
			//pibrowser = NULL;
		//}
		RECT	controlWidth={pnt.x,pnt.y,pnt.x+sz.width,pnt.y+sz.height};
		piactivex = get_iobjslib()->create_webbrowser(get_iengine()->get_device_window(), this, &controlWidth, FALSE);
	}
	if( piactivex)
	{
		GET_INTERFACE(ibase, piactivex, &pibase);
		GET_INTERFACE(iwindow, piactivex, &piwindow);
		GET_INTERFACE(ibrowser, piactivex, &pibrowser);
		GET_INTERFACE(ivisibleobject, piactivex, &pivisibleobject);
		GET_INTERFACE(ipluginsupport, piactivex, &pipluginsupport);
		assert(pibase);
		assert(piwindow);
		assert(pibrowser);
		assert(pivisibleobject);
		assert(pipluginsupport);
		USES_CONVERSION;
		wchar_t*	wcName = GetMLMedia()->GetStringProp("name");
		if( wcName != NULL)
			pivisibleobject->set_name(W2A(wcName));

		if( pipluginsupport)
			pipluginsupport->register_plugin(iid_iplugbrowser, (ibaseplugin*)this);
	}
	else
	{
		return false;
	}
	if( m_pRenderManager->GetResourceManager())
	{
		sSRC = m_pRenderManager->GetResourceManager()->GetFullPath(pwcSrc);
	}
	piwindow->move(pnt.x, pnt.y);
	piwindow->resize(sz.width, sz.height);
	pibrowser->navigate( sSRC.c_str());*/

	// tandy: вынесено в recreate_sprite
	//AbsDepthChanged();
	//AbsXYChanged();
	//AbsVisibleChanged();

	return true;
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

// реализация  iplugbrowser для компонента BROWSER
// документ проскроллировали html-ку (колесом мыши, выделением...)
void nrmBrowser::on_scrolled()
{
}

// Fired when a new hyperlink is being navigated to.
void nrmBrowser::on_before_navigate(const wchar_t* URL, int flags, const wchar_t* targetFrameName, const wchar_t* postData, const wchar_t* headers, bool &cancel)
{
	assert( mpMLMedia->GetIBrowser());
	mpMLMedia->GetIBrowser()->onBeforeNavigate( URL, flags, targetFrameName, postData, headers, cancel);
}

// Fired when the document being navigated to becomes visible and enters the navigation stack
void nrmBrowser::on_navigate_complete(const wchar_t* URL)
{
	assert( mpMLMedia->GetIBrowser());
	mpMLMedia->GetIBrowser()->onNavigateComplete( URL);
}

void nrmBrowser::on_document_complete(const wchar_t* URL)
{
	assert( mpMLMedia->GetIBrowser());
	mpMLMedia->GetIBrowser()->onDocumentComplete( URL);
}

// Fired when download progress is updated
void nrmBrowser::on_progress_change(int /*progress*/, int /*progressMax*/)
{
}

// Download of a page started
void nrmBrowser::on_download_begin()
{
}

// Download of page complete
void nrmBrowser::on_download_complete()
{
}

// The enabled state of a command changed 
void nrmBrowser::on_command_state_change(int /*command*/, bool /*enable*/)
{
}
// Fires when an error occurs during navigation
void nrmBrowser::on_navigate_error(const wchar_t* /*URL*/, const wchar_t* /*frame*/, int /*StatusCode*/, bool& /*cancel*/)
{
}

// вызов из JavaScript через window.external
void nrmBrowser::external_callback(const wchar_t* apProcName, int aiArgC, 
								   VARIANT* pvarArgs, VARIANT* varRes, bool abEvent)
{
/*
	if(wcscmp(apProcName, L"__getVUClientSessionID")){
		if(varRes != NULL){
		
			BSTR bstrSessionID = SysAllocString();
			varRes->bstrVal = bstrSessionID; // Tandy: ?? проверить, освобождается ли строка
		}
		return;
	}
*/
	mpMLMedia->GetIActiveX()->CallExternal(apProcName, aiArgC, (rmml::ML_VARIANT*)pvarArgs, (rmml::ML_VARIANT*)varRes, abEvent);
}

/*bool	nrmBrowser::check_support(ibase* apibase)
{
	return true;
}

void	nrmBrowser::is_registered(ibase* apibase)
{
	//??
}

int		nrmBrowser::get_supported_interfaces(N3DGUID* pinterfaces, int maxCount)
{
	if( maxCount < 1)	return 0;
	pinterfaces[0] = iid_iplugbrowser;
	return 1;
}*/

// реализация moIBrowser

const wchar_t* nrmBrowser::GetDocumentType(){
	//if(!pibrowser) return L"";
	//	  _ret_str = pibrowser->
	// ??
	return L"";
}

const wchar_t* nrmBrowser::GetLocationName(){
	/*if(!pibrowser) return L"";
	return  pibrowser->get_location_name();*/
	return L"";
}

const wchar_t* nrmBrowser::GetLocationURL(){
	/*if(!pibrowser) return L"";
	return pibrowser->get_location_url();*/
	return L"";
}

bool nrmBrowser::GetBusy(){
	/*if(!pibrowser) */return false;
	//return pibrowser->get_busy();
}

int nrmBrowser::GetReadyState(){
	// ??
	return 0;
}
bool nrmBrowser::GetOffline(){
	// ??
	return true;
}
void nrmBrowser::SetOffline(bool /*abValue*/){
	// ??
	return;
}
bool nrmBrowser::GetSilent(){
	// ??
	return true;
}
void nrmBrowser::SetSilent(bool /*abValue*/){
	// ??
	return;
}
int nrmBrowser::GetScrollTop(){
	/*if(!pibrowser) */return 0;
	//return pibrowser->get_top();
}
void nrmBrowser::SetScrollTop(int /*aiValue*/){
	// ??
	return;
}
int nrmBrowser::GetScrollLeft(){
	assert( FALSE);	// непонятно с названиями методов. У pibrowser есть get_scroll_left, но он почему то не вызывается
	/*if(!pibrowser) */return 0;
	//return pibrowser->get_left();
}
void nrmBrowser::SetScrollLeft(int /*aiValue*/){
	// ??
	return;
}
void nrmBrowser::BkgColorChanged(){
	//mlColor color = GetBrowser()->GetBkgColor();
	// ??
}

void nrmBrowser::ValueChanged(){
	if(m_browser){
		assert( mpMLMedia->GetIBrowser());
		const wchar_t* pwcHTMLString = mpMLMedia->GetIBrowser()->GetValue();
		if(pwcHTMLString == NULL || *pwcHTMLString == L'\0')
			return;
		m_browser->LoadHTMLString(pwcHTMLString);
	}
}

// flags: 2 - NoHistory, 4 - NoHistory, 8 - NoWriteToCache
omsresult nrmBrowser::Navigate(const wchar_t* URL, int flags, const wchar_t* postData, const wchar_t* headers){
	if(m_browser){
		wstring wsPostData(postData);
		wstring wsURL(URL);
		// добавляем sessionID
		oms::omsContext* pOMSContext = m_pRenderManager->GetContext();
		long lSessionID = pOMSContext->mpComMan->GetSessionID();
		wchar_t wsSessionID[40] = L"";
		if(lSessionID >= 0)
			_ltow(lSessionID, wsSessionID, 10);
		if(postData == NULL || *postData == L'\0'){
			// если надо, дополняем адрес до полного
			if(!(wsURL.find(L"http:") == 0
				|| wsURL.find(L"https:") == 0
				|| wsURL.find(L"javascript:") == 0)
			){
				// если есть GET-параметры, ты вытаскиваем их
				wstring wsParams;
				int idx = wstring::npos;
				if ((unsigned int)(idx = wsURL.find(L"?")) != wstring::npos){
					wsParams = wsURL.substr(idx);
					wsURL.erase(idx);
				}
				wsURL = m_pRenderManager->GetResourceManager()->GetFullPath(wsURL.c_str());
				wsURL += wsParams;
			}
			// добавляем sessionID в URL
			wstring::size_type pos = wsURL.find(L'?');
			wsURL += (pos == wstring::npos)?L'?':L'&';
			wsURL += wsSessionID;
		}else{
			// если надо, дополняем адрес до полного
			if(!(wsURL.find(L"http:") == 0
				|| wsURL.find(L"https:") == 0
				|| wsURL.find(L"javascript:") == 0)
			){
				wsURL = m_pRenderManager->GetResourceManager()->GetFullPath(wsURL.c_str());
			}
			// добавляем sessionID в POST-данные
			if(wsSessionID[0] != L'\0'){
				wsPostData += L"&sessionID=";
				wsPostData += wsSessionID;
			}
		}
		m_browser->Navigate(wsURL.c_str(), flags, wsPostData.c_str(), headers);
	}
	return OMS_OK;
}

// Navigates to the previous item in the history list
void nrmBrowser::GoBack(){
	//if(!pibrowser) 
	return;
	//pibrowser->go_back();
}
// Navigates to the next item in the history list
void nrmBrowser::GoForward(){
	/*if(!pibrowser) 	return;
	pibrowser->go_forward();*/
}
// Go home/start page (src)
void nrmBrowser::GoHome(){
	/*if(!pibrowser) return;
	pibrowser->go_home();*/
}
// Refresh the currently viewed page (NORMAL = 0, IFEXPIRED = 1, CONTINUE = 2, COMPLETELY = 3)
void nrmBrowser::Refresh(int /*level*/){
	/*if(!pibrowser) return;
	pibrowser->refresh();*/
}
// Stops opening a file
void nrmBrowser::Stop(){
	/*if(!pibrowser) return;
	pibrowser->stop();*/
} 
// абсолютный
bool nrmBrowser::ScrollTo(int /*x*/, int /*y*/){
	// ??
	assert( FALSE);	// сделать реализацию
	return false;
} 
// относительный 
bool nrmBrowser::ScrollBy(int /*x*/, int /*y*/){
	// ??
	assert( FALSE);	// сделать реализацию
	return false;
}
void nrmBrowser::ScrollToAnchor(const wchar_t* /*anchor*/){
	// ??
	assert( FALSE);	// сделать реализацию
}
omsresult nrmBrowser::ExecScript(const wchar_t* code, const wchar_t* /*language*/, ML_VARIANT* result){
	m_browser->ExecScript(code, (VARIANT*) result);
	if(result != NULL){
		if(result->vt == VT_BSTR && result->bstrVal != NULL){
			m_lastExecScriptResult = result->bstrVal;
			SysFreeString(result->bstrVal);
			result->bstrVal = (wchar_t*)m_lastExecScriptResult.c_str();
		}
	}
	return OMS_OK;
}
bool nrmBrowser::ExecCommand(const wchar_t* /*cmdID*/, bool /*showUI*/, ML_VARIANT /*value*/){
	// ??
	assert( FALSE);	// сделать реализацию
	return false;
}
bool nrmBrowser::QueryCommandEnabled(const wchar_t* /*cmdID*/){
	// ??
	assert( FALSE);	// сделать реализацию
	return false;
}

bool nrmBrowser::FireMouseDown(int x, int y){
	if(!m_browser) return false;
	m_browser->FireMouseDown(x, y);
	/*if(!pibrowser) return false;
	iwindow* piwindow = NULL;
	GET_INTERFACE(iwindow, pibrowser, &piwindow);
	assert(piwindow);
	piwindow->handle_mouse_down(MHB_LEFT, x, y);*/
	return true;
	//return pibrowser->fire_mouse_down(x, y);
}

bool nrmBrowser::FireMouseMove(int x, int y){
	if(!m_browser) return false;
	m_browser->FireMouseMove(x, y);
	/*if(!pibrowser) return false;
	iwindow* piwindow = NULL;
	GET_INTERFACE(iwindow, pibrowser, &piwindow);
	assert(piwindow);
	piwindow->handle_mouse_move(x, y);*/
	return true;
	//return pibrowser->fire_mouse_move(x, y);
}

bool nrmBrowser::FireMouseUp(int x, int y){
	if(!m_browser) return false;
	m_browser->FireMouseUp(x, y);
	/*if(!pibrowser) return false;
	iwindow* piwindow = NULL;
	GET_INTERFACE(iwindow, pibrowser, &piwindow);
	assert(piwindow);
	piwindow->handle_mouse_up(MHB_LEFT, x, y);*/
	return true;
//	return pibrowser->fire_mouse_up(x, y);
}

bool nrmBrowser::FireMouseWheel(int x, int y, int distance){
	if(!m_browser) return false;
	return m_browser->FireMouseWheel(x, y, distance);
}

bool nrmBrowser::KeyDown(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode){
	if(!m_browser) return false;
	return m_browser->KeyDown(keyCode, keyStates, scanCode);
}

bool nrmBrowser::KeyUp(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode){
	if(!m_browser) return false;
	return m_browser->KeyUp(keyCode, keyStates, scanCode);
}

bool nrmBrowser::KeyPress(unsigned int charCode, unsigned long scanCode){
	if(!m_browser) return false;
	return m_browser->KeyPress(charCode, scanCode);
}

void nrmBrowser::onSetFocus(){
	if(!m_browser) return;
	m_browser->SetFocus();
}

void nrmBrowser::onKillFocus(){
	if(!m_browser) return;
	m_browser->KillFocus();
}

void nrmBrowser::onKeyDown(){
	if(!m_browser) return;
	unsigned int keyCode = mpMLMedia->GetIButton()->GetKeyCode();
	unsigned int keyStates = mpMLMedia->GetIButton()->GetKeyStates();
	unsigned int scanCode = mpMLMedia->GetIButton()->GetKeyScanCode();
	m_browser->KeyDown(keyCode, keyStates, scanCode);
	unsigned int unicode = mpMLMedia->GetIButton()->GetKeyUnicode();
	m_browser->KeyPress(unicode, scanCode);
}

void nrmBrowser::onKeyUp(){
	if(!m_browser) return;
	unsigned int keyCode = mpMLMedia->GetIButton()->GetKeyCode();
	unsigned int keyStates = mpMLMedia->GetIButton()->GetKeyStates();
	unsigned int scanCode = mpMLMedia->GetIButton()->GetKeyScanCode();
	m_browser->KeyUp(keyCode, keyStates, scanCode);
}

void nrmBrowser::AbsEnabledChanged(){
	/*BOOL	bEnabled = */GetMLMedia()->GetIButton()->GetAbsEnabled();
	// ??
}

void nrmBrowser::AbsVisibleChanged()
{
	bool isVisible = GetMLMedia()->GetIVisible()->GetAbsVisible();
	if (m_sprite)
	{
#ifndef USE_VISIBLE_DESTINATION
		m_sprite->SetVisible(isVisible);
#else
		m_sprite->SetVisible(isVisible, 5);
#endif
	}
}

// возвращает критическую секцию, которую надо использовать при обработке сообщений 
// вспомогательных дочерних окон браузера
LPCRITICAL_SECTION nrmBrowser::GetCSForChildWindows(){
	if(m_browser)
		return (LPCRITICAL_SECTION)m_browser->GetCSForChildWindows();
	return NULL;
}

IDynamicTexture* nrmBrowser::GetDynamicTexture(){
	if(!m_browser)
		return NULL;
	return m_browser->GetDynamicTexture();
}
