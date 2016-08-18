// browser.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Browser.h"
#include <Assert.h>
#include "bstr.h"
#include <shlwapi.h>
#include <wininet.h>

#include "savebmp.h"

#ifndef CComPtr
#include "comptr.h"
#endif

//#import "lib/WebKit.tlb"
#include "webkit_i.c"

HMODULE ghModule = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	ghModule = (HMODULE)hModule;
    return TRUE;
}

HRESULT STDMETHODCALLTYPE CBrowserWebHost::QueryInterface(REFIID riid, void** ppvObject)
{
	*ppvObject = 0;
	if (IsEqualGUID(riid, IID_IUnknown))
		*ppvObject = static_cast<IWebFrameLoadDelegate*>(this);
	else if (IsEqualGUID(riid, IID_IWebFrameLoadDelegate))
		*ppvObject = static_cast<IWebFrameLoadDelegate*>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CBrowserWebHost::AddRef(void)
{
	return ++m_refCount;
}

ULONG STDMETHODCALLTYPE CBrowserWebHost::Release(void)
{
	ULONG newRef = --m_refCount;
	if (!newRef)
		delete(this);

	return newRef;
}

bool MakePathToComponents(CHAR* lpcModuleFileName, CHAR* lpcExeFullPathName, LPSTR &lpFileName, BOOL &bByPathOnly){
	::GetModuleFileName( ghModule, lpcModuleFileName, MAX_PATH+150);
	::GetFullPathName(lpcModuleFileName,MAX_PATH+150,lpcExeFullPathName,&lpFileName);
	return true;
}

typedef HRESULT (__stdcall *TGetClassObjectProc)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

HRESULT CBrowser::CreateWebKitObject(REFCLSID rclsid, REFIID riid, LPVOID * ppv){
	HRESULT hr = S_OK;
	if(m_webKit == NULL)
		return E_FAIL;
	TGetClassObjectProc pfnGetClassObjectProc = (TGetClassObjectProc)GetProcAddress(m_webKit, "DllGetClassObject");
	if (pfnGetClassObjectProc == NULL)
		return E_FAIL;
	IClassFactory* pCF = NULL;
	hr = (*pfnGetClassObjectProc)(rclsid, IID_IClassFactory, (void**)&pCF);
	if(FAILED(hr) || pCF == NULL)
		return hr;
	hr = pCF->CreateInstance(NULL, riid, ppv);
	pCF->Release();
	return hr;
}

CBrowser::CBrowser(IBrowserCallBack* apCallBack, HWND ahwndParentWindow):
m_callBack(apCallBack),
m_initialized(false),
m_colorChannelCount(0),
m_frameData(NULL),
m_height(0),
m_width(0),
m_textureWidth(0),
m_textureHeight(0),
m_bitmap(0),
m_hdc(0),
m_oldBitmap(NULL),
m_paintAll(true),
m_cs(NULL),
m_parentWindow(NULL)
{
//	USES_CONVERSION;

//	InitializeCriticalSection(&m_cs);
//	CGuardByCS oGCS(&m_cs);

	HRESULT hr = S_OK;

	m_hdc = CreateCompatibleDC(0);

	// Init COM
	OleInitialize(NULL);

	// поднимаем WebKit.dll
	m_webKit = NULL;
	CHAR lpcModuleFileName[MAX_PATH+200];
	CHAR lpcExeFullPathName[MAX_PATH+200];
	LPSTR lpFileName=NULL;
	BOOL bByPathOnly = FALSE;
	MakePathToComponents(lpcModuleFileName, lpcExeFullPathName, lpFileName, bByPathOnly);
	if(lpFileName!=NULL){
		*lpFileName='\0';
		strcpy(lpFileName, "WebKit.dll");
		BSTR bstr = SysAllocString(cLPWCSTR(lpFileName));
		m_webKit = CoLoadLibrary(bstr, TRUE);
		SysFreeString(bstr);
		*lpFileName='\0';
	}
	if(m_webKit != NULL){
		TGetClassObjectProc pfnGetClassObjectProc = (TGetClassObjectProc)GetProcAddress(m_webKit, "DllGetClassObject");
		IClassFactory* pClassFactory = NULL;
		if (pfnGetClassObjectProc != NULL)
			hr=(*pfnGetClassObjectProc)(CLSID_WebView, IID_IClassFactory, (void**)&pClassFactory);
		if(SUCCEEDED(hr) && pClassFactory!=NULL){
			hr = pClassFactory->CreateInstance(NULL, IID_IWebView, (void**)&m_webView);
			pClassFactory->Release();
		}
	}
	// если из WebKit.dll непосредственно создать не удалось, создаем стандартным COM-овским способом
	if(m_webView == NULL && !bByPathOnly){
		if(FAILED(CoCreateInstance( CLSID_WebView, NULL, CLSCTX_ALL, IID_IWebView,(void**)&m_webView)))
			m_webView = NULL;
	}
	//
//	HRESULT hr = CoCreateInstance(CLSID_WebView, 0, CLSCTX_ALL, IID_IWebView, (void**)&m_webView);
	if (m_webView == NULL){
		return;
	}

	m_webHost = new CBrowserWebHost(this);
	m_webHost->AddRef();
	hr = m_webView->setFrameLoadDelegate(m_webHost);
	if (FAILED(hr))
		return;

	m_webView->setApplicationNameForUserAgent(CBSTR(L"VUClient/0.1"));

	hr = m_webView->SetSharedTimerCS((IUnknown**)&m_cs);
	if (FAILED(hr))
		return;

/*
	hr = m_webView->setHostWindow((OLE_HANDLE) ahwndParentWindow);
	if (FAILED(hr))
		return;
RECT clientRect;
GetClientRect(ahwndParentWindow, &clientRect);
clientRect.right = 300;
clientRect.bottom = 650;
hr = m_webView->initWithFrame(clientRect, 0, 0);
*/
/*
	RECT clientRect;
	clientRect.left = 0;
	clientRect.top = 0;
	clientRect.right = m_updateWidth;
	clientRect.bottom = m_updateHeight;
//	GetClientRect(hMainWnd, &clientRect);
	hr = m_webView->initWithFrame(clientRect, 0, 0);
	if (FAILED(hr))
		goto exit;

	IWebFrame* frame;
	hr = m_webView->mainFrame(&frame);
	if (FAILED(hr))
		goto exit;
	static BSTR defaultHTML = 0;
	if (!defaultHTML)
		defaultHTML = SysAllocString(L"<p style=\"background-color: #00FF00\">Testing</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul>");
	frame->loadHTMLString(defaultHTML, 0);
	frame->Release();

	IWebViewPrivate* viewExt;
	hr = m_webView->QueryInterface(IID_IWebViewPrivate, (void**)&viewExt);
	if (FAILED(hr))
		goto exit;
	hr = viewExt->viewWindow((OLE_HANDLE*) &m_ViewWindow);
	viewExt->Release();
	if (FAILED(hr) || !m_ViewWindow)
		goto exit;
*/

	for (int k = 0; k < 4; k++)
	{
		m_userData[k] = NULL;
	}
}

void* 	CBrowser::GetUserData(unsigned int userDataID)
{
	if (userDataID < 4)
	{
		return m_userData[userDataID];
	}
	else
	{
		return NULL;
	}
}

void 	CBrowser::SetUserData(unsigned int userDataID, void* data)
{
	if (userDataID < 4)
	{
		m_userData[userDataID] = data;
	}
}

bool CBrowser::SetParentWindow(HWND aHwnd){ 
	m_parentWindow = aHwnd; 
	if(!m_webView)
		return false;
	HRESULT hr = m_webView->setHostWindow((OLE_HANDLE)m_parentWindow);
	return true; 
}

CBrowser::~CBrowser()
{
	if(m_webView != NULL){
		m_webView->SetSharedTimerCS(NULL);
		m_webView->Destroy();
		int irefs = m_webView->Release();
//		if(irefs > 0)
//			m_webView->Release();
int hh=0;
	}

	if(m_webHost != NULL)
		delete m_webHost;

	if(m_webKit != NULL)
		CoFreeLibrary(m_webKit);

	if(m_oldBitmap != NULL)
		::SelectObject(m_hdc, m_oldBitmap);

	::DeleteObject(m_bitmap);
	m_frameData = NULL;

	::DeleteDC(m_hdc);

//	::DeleteCriticalSection(&m_cs);
}

// Возвращает динамическую текстуру
IDynamicTexture* CBrowser::GetDynamicTexture(){
	return this;
}

// установить размер "окна" браузера
bool CBrowser::SetSize(int aiWidth, int aiHeight){
	CGuardByCS oGCS(m_cs);
	m_width = aiWidth;
	m_height = aiHeight;
	SetTextureSize(aiWidth, aiHeight);
	return true;
}

bool CBrowser::FireMouseDown(int x, int y){ // x,y относительно левого верхнего угла окна браузера
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
//	m_webView->SetActiveState(true); // ?? надо вывести из активного состояния другие браузеры
	m_webView->HandleMouseEvent(WM_LBUTTONDOWN, 0, (y << 16) + x);
	return true;
}

bool CBrowser::FireMouseMove(int x, int y){
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	m_webView->HandleMouseEvent(WM_MOUSEMOVE, 0, (y << 16) + x);
	return true;
}

bool CBrowser::FireMouseUp(int x, int y){
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	m_webView->HandleMouseEvent(WM_LBUTTONUP, 0, (y << 16) + x);
	return true;
}

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

bool CBrowser::FireMouseWheel(int x, int y, int distance){
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	m_webView->MouseWheel(distance << 16, (y << 16) + x, FALSE);
	return true;
}

bool CBrowser::KeyDown(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode){
	if(m_webView == NULL || !m_initialized)
		return false;
	//muKeyCode = aEvent.key_code; // wParam//  
	//muKeyStates = aEvent.mousexy.mouse_x; (alParam & 0xffff); 
	//muScanCode = aEvent.mousexy.mouse_y; (alParam >> 16); 
	CGuardByCS oGCS(m_cs);
	HRESULT hr = m_webView->KeyDown(keyCode, scanCode << 16 | keyStates, FALSE); // wParam, lParam
	return SUCCEEDED(hr);
}

bool CBrowser::KeyUp(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode){
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	HRESULT hr = m_webView->KeyUp(keyCode, scanCode << 16 | keyStates, FALSE); // wParam, lParam
	return SUCCEEDED(hr);
}

bool CBrowser::KeyPress(unsigned int charCode, unsigned long scanCode){
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	HRESULT hr = m_webView->KeyPress(charCode, scanCode, FALSE);
	return SUCCEEDED(hr);
}

bool CBrowser::SetFocus(){
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	HRESULT hr = m_webView->SetActiveState(true);
	return SUCCEEDED(hr);
}

bool CBrowser::KillFocus(){
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	HRESULT hr = m_webView->SetActiveState(false);
	return SUCCEEDED(hr);
}

class CStringOStream: public IStream
{
	CBSTR m_bstrStr;
public:
	CStringOStream(CBSTR& abstrStr){
		m_bstrStr = abstrStr;
	}

// IUnknown
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject)
	{
		return E_NOTIMPL;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef( void){
		return 0;
	}

	virtual ULONG STDMETHODCALLTYPE Release( void){
		return 0;
	}

// ISequentialStream
public:
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read( 
		/* [length_is][size_is][out] */ void *pv,
		/* [in] */ ULONG cb,
		/* [out] */ ULONG *pcbRead)
	{
		memcpy(pv, m_bstrStr.m_str, m_bstrStr.ByteLength());
		if(pcbRead)
			*pcbRead = m_bstrStr.ByteLength();
		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write( 
	/* [size_is][in] */ const void *pv,
	/* [in] */ ULONG cb,
	/* [out] */ ULONG *pcbWritten)
	{
		return E_NOTIMPL;
	}

// IStream
public:
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek( 
		/* [in] */ LARGE_INTEGER dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [out] */ ULARGE_INTEGER *plibNewPosition)
	{
		if(dwOrigin == STREAM_SEEK_END){
			if(plibNewPosition != NULL)
				plibNewPosition->QuadPart = m_bstrStr.ByteLength();
			return S_OK;
		}
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE SetSize( 
	/* [in] */ ULARGE_INTEGER libNewSize)
	{
		return E_NOTIMPL;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo( 
	/* [unique][in] */ IStream *pstm,
	/* [in] */ ULARGE_INTEGER cb,
	/* [out] */ ULARGE_INTEGER *pcbRead,
	/* [out] */ ULARGE_INTEGER *pcbWritten)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Commit( 
	/* [in] */ DWORD grfCommitFlags)
	{
		
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Revert( void)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE LockRegion( 
	/* [in] */ ULARGE_INTEGER libOffset,
	/* [in] */ ULARGE_INTEGER cb,
	/* [in] */ DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE UnlockRegion( 
	/* [in] */ ULARGE_INTEGER libOffset,
	/* [in] */ ULARGE_INTEGER cb,
	/* [in] */ DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Stat( 
	/* [out] */ STATSTG *pstatstg,
	/* [in] */ DWORD grfStatFlag)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Clone( 
	/* [out] */ IStream **ppstm)
	{
		return E_NOTIMPL;
	}
};

std::wstring trimWStr(std::wstring& aSrcStr){
	std::wstring aRetStr = aSrcStr;
	while(aRetStr.size() > 0 && aRetStr[0] == L' ')
		aRetStr = aRetStr.substr(1, aRetStr.size() - 1);
	while(aRetStr.size() > 0 && aRetStr[aRetStr.size()-1] == L' ')
		aRetStr = aRetStr.substr(0, aRetStr.size() - 1);
	return aRetStr;
}

bool CBrowser::Navigate(const wchar_t* apwcURL, int aiFlags, const wchar_t* apwcPostData, const wchar_t* apwcHeaders){
	if(m_webView == NULL || !m_initialized){
		m_urlAfterInit = apwcURL;
		return false;
	}
	IWebFrame* frame = 0;
	IWebMutableURLRequest* request = 0;

	CBSTR urlBStr = apwcURL;

	CBSTR methodBStr = L"GET";

	CBSTR postData = L"";
	if(apwcPostData != NULL && *apwcPostData != L'\0'){
		methodBStr = L"POST";
		postData = apwcPostData;
	}
	CStringOStream streamPostData(postData);

	std::wstring wsHeaders = apwcHeaders==NULL?L"":apwcHeaders;

	if (urlBStr && urlBStr[0] && (PathFileExistsW(urlBStr) || PathIsUNCW(urlBStr))) {
		wchar_t fileURL[INTERNET_MAX_URL_LENGTH];
		DWORD fileURLLength = sizeof(fileURL)/sizeof(fileURL[0]);
		if (SUCCEEDED(UrlCreateFromPathW(urlBStr, fileURL, &fileURLLength, 0)))
			urlBStr = fileURL;
	}

	CGuardByCS oGCS(m_cs);

	HRESULT hr = m_webView->mainFrame(&frame);
	if (FAILED(hr))
		goto exit;

	//hr = CoCreateInstance(CLSID_WebMutableURLRequest, 0, CLSCTX_ALL, IID_IWebMutableURLRequest, (void**)&request);
	hr = CreateWebKitObject(CLSID_WebMutableURLRequest, IID_IWebMutableURLRequest, (void**)&request);
	//hr = m_webView->CreateMutableURLRequest(&request);
	if (FAILED(hr))
		goto exit;

	hr = request->initWithURL(urlBStr, WebURLRequestUseProtocolCachePolicy, 0);
	if (FAILED(hr))
		goto exit;

	hr = request->setHTTPMethod(methodBStr);
	if (FAILED(hr))
		goto exit;

	hr = request->setHTTPBody(&streamPostData);
	if (FAILED(hr))
		goto exit;

	if(wsHeaders.size() > 0){
		std::wstring::size_type posHPairBegin = 0;
		for(;;){
			std::wstring::size_type posSemic = wsHeaders.find(L';', posHPairBegin);
			if(posSemic == std::wstring::npos)
				posSemic = wsHeaders.size();
			std::wstring wsPair = wsHeaders.substr(posHPairBegin, posSemic);
			std::wstring::size_type posComma = wsPair.find(L':');
			if(posComma != std::wstring::npos){
				std::wstring wsField = wsPair.substr(0, posComma);
				wsField = trimWStr(wsField);
				std::wstring wsValue = wsPair.substr(posComma + 1, wsPair.size() - posComma);
				wsValue = trimWStr(wsValue);
				if(!wsField.empty())
					hr = request->setValue(CBSTR(wsValue.c_str()), CBSTR(wsField.c_str()));
			}
			if(posSemic == wsHeaders.size())
				break;
			posHPairBegin = posSemic + 1;
		}
//	hr = request->setValue(CBSTR(L"vuclient@virtuniver.com"), CBSTR(L"From"));
//	if (FAILED(hr))
//		goto exit;
	}

	hr = frame->loadRequest(request);
	if (FAILED(hr))
		goto exit;

	//SetFocus(gViewWindow);

exit:
	if (frame)
		frame->Release();
	if (request)
		request->Release();
	return true;
}

bool CBrowser::ExecScript(const wchar_t* scriptCode, VARIANT* result){
	CGuardByCS oGCS(m_cs);
//	CComPtr<IWebFrame> frame;

//	HRESULT hr = m_webView->mainFrame(&frame);
//	if (FAILED(hr))
//		return false;

	CBSTR bstrCode = scriptCode;
	BSTR bstrResult = NULL;
	m_webView->stringByEvaluatingJavaScriptFromString(bstrCode, &bstrResult);
	if(bstrResult != NULL){
		if(result != NULL && bstrResult != NULL){
			result->vt = VT_BSTR;
			result->bstrVal = bstrResult;
		}else
			SysFreeString(bstrResult);
	}
	return true;
}

// возвращает критическую секцию, которую надо использовать при обработке сообщений 
// вспомогательных дочерних окон браузера
void* CBrowser::GetCSForChildWindows(){
	return m_cs;
}

// Обновить состояние объекта (не текстуру)
void CBrowser::Update(){
	// сохранить текущие значения свойств, чтобы использовать их в UpdateFrame()
	m_updateWidth = m_width;
	m_updateHeight = m_height;
	m_updateTextureWidth = m_textureWidth;
	m_updateTextureHeight = m_textureHeight;
	//
	m_updateBitmap = m_bitmap;
	m_updateHDC = m_hdc;
	// ??
	if(!m_initialized){
		CGuardByCS oGCS(m_cs);

		// первый раз отрисовываем в главном потоке, 
		// чтобы m_webView инициализировался (а надо чтобы он инициализировался именно в главном потоке)
		UpdateFrame();

		IWebFrame* frame;
		HRESULT hr = m_webView->mainFrame(&frame);
		if (FAILED(hr))
			return;
		static BSTR defaultHTML = 0;
		if (!defaultHTML){
			defaultHTML = SysAllocString(L"Loading...");
//			defaultHTML = SysAllocString(L"<p style=\"background-color: #00FF00\">Testing</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul>");
		}
		frame->loadHTMLString(defaultHTML, 0);
		frame->Release();

		m_initialized = true;
		if(!m_urlAfterInit.empty()){
			Navigate(m_urlAfterInit.c_str());
		}
	}else{
		if(m_webView != NULL){
			CGuardByCS oGCS(m_cs);
			m_webView->LayoutIfNeeded();
		}
	}
}

// загрузить в качестве HTML-страницы строку
bool CBrowser::LoadHTMLString(const wchar_t* apwcHTMLString, const wchar_t* apwcEncoding){
	if(apwcHTMLString == NULL)
		apwcHTMLString = L"";
	IWebFrame* frame;
	HRESULT hr = m_webView->mainFrame(&frame);
	if (FAILED(hr))
		return false;
	frame->loadHTMLString(CBSTR(apwcHTMLString), 0);
	frame->Release();
	return true;
}

// Обновить текстуру
void CBrowser::UpdateFrame(){
//	::SelectObject(m_updateHDC, m_updateBitmap);
static bool gbOnceCalled = false;

//static int giCntr = 0;
//if(giCntr > 0){
//giCntr--;
//return;
//}
//giCntr = 20;
	if(m_webView != NULL){
		CGuardByCS oGCS(m_cs);

		IViewObject* viewObj;
		HRESULT hr = m_webView->QueryInterface(IID_IViewObject, (void**)&viewObj);
		if (SUCCEEDED(hr)){
			RECT rc;
			rc.left = 0;
			rc.top = 0;
			rc.right = m_width;
			rc.bottom = m_height;
			RECTL rect = {rc.left, rc.top, rc.right, rc.bottom};
//if(!gbOnceCalled){
//m_paintAll = true;
//			::FillRect(m_updateHDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			::SelectClipRgn(m_hdc, NULL);
			viewObj->Draw(
				DVASPECT_CONTENT,
				1,
				NULL,
				NULL, 
				(HDC)m_parentWindow, // сюда передаем HWND главного окна плеера
				m_updateHDC,
				&rect,
				NULL,
				NULL,
				m_paintAll?0:1
			);
//gbOnceCalled = true;
//}
			viewObj->Release();
			m_paintAll = false;
		}
	}

//CreateBMPFile(NULL, "c:\\bmp.bmp", m_updateBitmap, m_updateHDC);

/*
	HPEN pen = ::CreatePen(PS_SOLID, 2, RGB(255,128,128));
	HPEN penOld = (HPEN)::SelectObject(m_updateHDC, pen);
	::MoveToEx(m_updateHDC, 10, 10,NULL);
	::LineTo(m_updateHDC, 100, 100);
	::SelectObject(m_updateHDC, penOld);
	::DeleteObject(pen);
*/
	// ??
}

// Вернуть ссылку на массив с построчным содержимым кадра	
void* CBrowser::GetFrameData(){
	return m_frameData;
}

// Вернуть ширину исходной картинки в пикселях
unsigned short CBrowser::GetSourceWidth(){
	return m_width;
}

// Вернуть высоту исходной картинки в пикселях
unsigned short CBrowser::GetSourceHeight(){
	return m_height;
}

// Вернуть ширину результирующей текстуры в пикселях
unsigned short CBrowser::GetTextureWidth(){
	return m_textureWidth;
}

// Вернуть высоту результирующей текстуры в пикселях
unsigned short CBrowser::GetTextureHeight(){
	return m_textureHeight;
}

// Установить размер результирующей текстуры
void CBrowser::SetTextureSize(int width, int height){
	assert(width > 0);
	assert(height > 0);

	if ((width <= 0) || (height <= 0))
	{
		return;
	}

	if ((m_textureWidth != width) || (m_textureHeight != height))
	{
		if (m_bitmap)
		{
			if(m_oldBitmap != NULL)
				::SelectObject(m_hdc, m_oldBitmap);
			DeleteObject(m_bitmap);
		}

		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof (BITMAPINFOHEADER); 
		bmih.biPlanes = 1;       
		bmih.biBitCount = 24;      
		bmih.biWidth = width;     
		bmih.biHeight = height;     
		bmih.biCompression = BI_RGB;  
		m_bitmap = CreateDIBSection(m_hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&m_frameData), NULL, NULL);
		::SelectClipRgn(m_hdc, NULL);
		m_oldBitmap = ::SelectObject(m_hdc, m_bitmap);
		m_paintAll = true;
		UpdateFrame();
	}

	m_textureWidth = width;
	m_textureHeight = height;

	m_callBack->on_texture_size_changed(m_textureWidth, m_textureHeight);
}

// Вернуть количество цветовых каналов, 3 для RGB, 4 для RGBA
unsigned char CBrowser::GetColorChannelCount(){
	return m_colorChannelCount;
}

// Порядок цветовых каналов прямой (RGB) или обратный (BGR)
bool CBrowser::IsDirectChannelOrder(){
	// ??
	return false;
}

// Нужно ли обновлять текстуру (или сейчас её содержимое не изменяется?)
bool CBrowser::IsNeedUpdate(){
	// ??
	return true;
}

void CBrowser::Destroy(){
	delete this;
}

void CBrowser::ShowErrorPage(const wchar_t* apwcURL, int aiCode, const wchar_t* apwcError){
	std::wstring wsErrorHTML;
	if(apwcURL != NULL){
		wsErrorHTML += apwcURL;
		wsErrorHTML += L"<br>";
	}
	wsErrorHTML += L"<br><b>ERROR</b> ";
	wchar_t wcCode[20] = L"?";
	swprintf(wcCode, L"%d", aiCode);
	wsErrorHTML += wcCode;
	wsErrorHTML += L"<br>";
	if(apwcError != NULL)
		wsErrorHTML += apwcError;
	LoadHTMLString(wsErrorHTML.c_str());
}
