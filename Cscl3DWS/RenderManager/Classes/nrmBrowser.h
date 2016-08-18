// nrmBrowser.h: interface for the nrmBrowser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmBrowser_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmBrowser_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "../CommonRenderManagerHeader.h"

#include "nrmanager.h"
#include "nrmActiveX.h"
#include "Sprite.h"
#include "nrmMOIVisibleStub.h"
#include "IBrowser.h"

#include "Sprite.h"

//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class nrmBrowser :	// public nrmActiveX,
					public nrmObject,
					public nrmMOIVisibleStub,
					public moIButton,
					public moIBrowser,
					public IChangesListener,
					public IBrowserCallBack
					//public ibaseplugin,
					//public iplugbrowser
{
	wstring DefCLSIDBySrc(const wchar_t* apwcSrc);
public:
	nrmBrowser(mlMedia* apMLMedia);
	virtual ~nrmBrowser();

	LPCRITICAL_SECTION GetCSForChildWindows();

	/*IMPLEMENT_RELEASE
	BEGIN_INTERFACE_IMPLEMENTATION
		IMPLEMENT_INTERFACE(ibaseplugin)
		IMPLEMENT_INTERFACE(iplugbrowser)
	END_INTERFACE_IMPLEMENTATION*/

	// реализация natura3d::ibaseplugin
public:
/*	bool	check_support(ibase* apibase);
	void	is_registered(ibase* apibase);
	int		get_supported_interfaces(N3DGUID* pinterfaces, int maxCount);*/

	// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }
	moIVisible* GetIVisible(){ return this; }
	moIButton* GetIButton(){ return this; }
	moIBrowser* GetIBrowser(){ return this; }

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
	void Crop(short /*awX*/, short /*awY*/, MLWORD /*awWidth*/, MLWORD /*awHeight*/){}
	mlRect GetCropRect(){ mlRect rc; ML_INIT_RECT(rc); return rc; }
	void SetMask(int /*aiX*/, int /*aiY*/, int /*aiW*/, int /*aiH*/){}
	int IsPointIn(const mlPoint /*aPnt*/){ return -1; }

	// реализация  moIButton
public:
	// Свойства
	void AbsEnabledChanged();
	// События
	virtual void onSetFocus();
	virtual void onKillFocus();
	virtual void onKeyDown();
	virtual void onKeyUp();

	// реализация moIBrowser
public:
	virtual const wchar_t* GetDocumentType();
	virtual const wchar_t* GetLocationName();
	virtual const wchar_t* GetLocationURL();
	virtual bool GetBusy();
	virtual int GetReadyState();
	virtual bool GetOffline();
	virtual void SetOffline(bool abValue);
	virtual bool GetSilent();
	virtual void SetSilent(bool abValue);
	virtual int GetScrollTop();
	virtual void SetScrollTop(int aiValue);
	virtual int GetScrollLeft();
	virtual void SetScrollLeft(int aiValue);
	virtual void BkgColorChanged();
	virtual void ValueChanged();
	// Методы
	// flags: 2 - NoHistory, 4 - NoHistory, 8 - NoWriteToCache
	virtual omsresult Navigate(const wchar_t* URL, int flags, const wchar_t* postData, const wchar_t* headers);
	virtual void GoBack(); // Navigates to the previous item in the history list
	virtual void GoForward(); // Navigates to the next item in the history list
	virtual void GoHome(); // Go home/start page (src)
	virtual void Refresh(int level); // Refresh the currently viewed page (NORMAL = 0, IFEXPIRED = 1, CONTINUE = 2, COMPLETELY = 3)
	virtual void Stop(); // Stops opening a file
	virtual bool ScrollTo(int x, int y); // абсолютный
	virtual bool ScrollBy(int x, int y); // относительный 
	virtual void ScrollToAnchor(const wchar_t* anchor);
	virtual omsresult ExecScript(const wchar_t* code, const wchar_t* language, ML_VARIANT* result);
	virtual bool ExecCommand(const wchar_t* cmdID, bool showUI, ML_VARIANT value);
	virtual bool QueryCommandEnabled(const wchar_t* cmdID);
	virtual bool FireMouseDown(int x, int y); // x,y относительно левого верхнего угла окна браузера
	virtual bool FireMouseMove(int x, int y);
	virtual bool FireMouseUp(int x, int y);
	virtual bool FireMouseWheel(int x, int y, int distance);
	virtual bool KeyDown(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode);
	virtual bool KeyUp(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode);
	virtual bool KeyPress(unsigned int charCode, unsigned long scanCode);
//	virtual bool SetFocus();
//	virtual bool KillFocus();

// реализация IChangeListener
public:
	virtual void OnChanged(int /*eventID*/){}

	// реализация  iplugbrowser
public:
	void on_scrolled(); // документ проскроллировали html-ку (колесом мыши, выделением...)
	// Fired when a new hyperlink is being navigated to.
	void on_before_navigate(const wchar_t* URL, int flags, const wchar_t* targetFrameName, 
				const wchar_t* postData, const wchar_t* headers, bool &cancel);
	// Fired when the document being navigated to becomes visible and enters the navigation stack
	void on_navigate_complete(const wchar_t* URL);
	void on_document_complete(const wchar_t* URL);
	// Fired when download progress is updated
	void on_progress_change(int progress, int progressMax);
	void on_download_begin(); // Download of a page started
	void on_download_complete(); // Download of page complete
	void on_command_state_change(int command, bool enable); // The enabled state of a command changed 
	// Fires when an error occurs during navigation
	void on_navigate_error(const wchar_t* URL, const wchar_t* frame, int StatusCode, bool &cancel);
	// вызов из JavaScript через window.external
	void external_callback(const wchar_t* apProcName, int aiArgC, 
		VARIANT* pvarArgs, VARIANT* varRes, bool abEvent);

	void on_texture_size_changed(int width, int height);


public:
	IDynamicTexture* GetDynamicTexture();
	CSprite* m_sprite;

private:
	bool recreate_sprite(bool create = true);
	void SetBrowserSize(mlSize& sz);

	IBrowser*				m_browser;

	MP_WSTRING m_lastExecScriptResult;
};

#endif // !defined(AFX_nrmBrowser_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
