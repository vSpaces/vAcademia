#pragma once

#include <windows.h>
#include "webhost.h"
//#include "WebKit.h"
#include "IBrowser.h"
#include "IDynamicTexture.h"
#include <string>

class CGuardByCS
{
public:
	CGuardByCS::CGuardByCS(LPCRITICAL_SECTION apCS){
		m_pCS = apCS;
		if(m_pCS == NULL)
			return;
		EnterCriticalSection(m_pCS);
	}
	CGuardByCS::~CGuardByCS(){
		if(m_pCS == NULL)
			return;
		LeaveCriticalSection(m_pCS);
	}
private:
	LPCRITICAL_SECTION m_pCS;
};

class CBrowser : public IBrowser, public IDynamicTexture
{
friend class CBrowserWebHost;
public:
	CBrowser(IBrowserCallBack* apCallBack, HWND ahwndParentWindow);
	~CBrowser();

// реализаци€ IBrowser
public: 
	// ¬озвращает динамическую текстуру
	virtual IDynamicTexture* GetDynamicTexture();
	// установить размер "окна" браузера
	virtual bool SetSize(int aiWidth, int aiHeight);
	// обработка событий мыши
	virtual bool FireMouseDown(int x, int y); // x,y относительно левого верхнего угла окна браузера
	virtual bool FireMouseMove(int x, int y);
	virtual bool FireMouseUp(int x, int y);
	virtual bool FireMouseWheel(int x, int y, int distance);
	// обработка событий клавиатуры
	virtual bool KeyDown(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode);
	virtual bool KeyUp(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode);
	virtual bool KeyPress(unsigned int charCode, unsigned long scanCode);
	virtual bool SetFocus();
	virtual bool KillFocus();
	// 
	virtual bool Navigate(const wchar_t* apwcURL, int flags=0, const wchar_t* postData=NULL, const wchar_t* headers=NULL);
	//
	virtual bool ExecScript(const wchar_t* scriptCode, VARIANT* result);
	// возвращает критическую секцию, которую надо использовать при обработке сообщений 
	// вспомогательных дочерних окон браузера
	virtual void* GetCSForChildWindows();
	// передать нэндл главного окна
	virtual bool SetParentWindow(HWND aHwnd);
	// загрузить в качестве HTML-страницы строку
	virtual bool LoadHTMLString(const wchar_t* apwcHTMLString, const wchar_t* apwcEncoding = NULL);

// реализаци€ IDynamicTexture
public: 

	// ќбновить состо€ние объекта (не текстуру)
	virtual void Update();
	// ќбновить текстуру
	virtual void UpdateFrame();

	// ¬ернуть ссылку на массив с построчным содержимым кадра	
	virtual void* GetFrameData();

	// ¬ернуть ширину исходной картинки в пиксел€х
	virtual unsigned short GetSourceWidth();
	// ¬ернуть высоту исходной картинки в пиксел€х
	virtual unsigned short GetSourceHeight();
	// ¬ернуть ширину результирующей текстуры в пиксел€х
	virtual unsigned short GetTextureWidth();
	// ¬ернуть высоту результирующей текстуры в пиксел€х
	virtual unsigned short GetTextureHeight();
	// ”становить размер результирующей текстуры
	virtual void SetTextureSize(int width, int height);
	// ¬ернуть количество цветовых каналов, 3 дл€ RGB, 4 дл€ RGBA
	virtual unsigned char GetColorChannelCount();
	// ѕор€док цветовых каналов пр€мой (RGB) или обратный (BGR)
	virtual bool IsDirectChannelOrder();

	// Ќужно ли обновл€ть текстуру (или сейчас еЄ содержимое не измен€етс€?)
	virtual bool IsNeedUpdate();

	// ”ничтож себ€
	// (требуетс€ дл€ того, чтобы nengine мог сам решить когда его безопасно удалить)
	virtual void Destroy();

	void* 	GetUserData(unsigned int userDataID);
	void 	SetUserData(unsigned int userDataID, void* data);

	void SetFreezState(bool /*isFreezed*/){};
	bool IsFreezed(){return false;};
	bool IsFirstFrameSetted(){return false;};

private:
	// выполнить инициализацию
//	bool init();
	HRESULT CreateWebKitObject(REFCLSID rclsid, REFIID riid, LPVOID * ppv);
	void ShowErrorPage(const wchar_t* apwcURL, int aiCode, const wchar_t* apwcError);

private:
	IBrowserCallBack* m_callBack;
	bool m_initialized;
	bool m_paintAll; // отрисовывать всю страницу (true) или только изменени€ (false)

	unsigned char m_colorChannelCount;
	unsigned char* m_frameData;
	//	
	unsigned short m_width;
	unsigned short m_height;
	unsigned short m_textureWidth;
	unsigned short m_textureHeight;
	//
	HBITMAP m_bitmap;
	HDC m_hdc;

	// копии свойств, которые должны быть использованы в UpdateFrame()
	unsigned int m_updateWidth;
	unsigned int m_updateHeight;
	unsigned int m_updateTextureWidth;
	unsigned int m_updateTextureHeight;
	//
	HDC m_updateHDC;
	HBITMAP m_updateBitmap;
	//
	HMODULE m_webKit;
	CBrowserWebHost* m_webHost;
	IWebView* m_webView;
	//HWND m_ViewWindow;
	HGDIOBJ m_oldBitmap;

	// критическа€ секци€, гарантирующа€, что обработка событий и отрисовка не будут происходить одновременно
	LPCRITICAL_SECTION m_cs;

	HWND m_parentWindow;

	// url, который надо загрузить после инициализации
	std::wstring m_urlAfterInit;

	void* m_userData[4];
};
