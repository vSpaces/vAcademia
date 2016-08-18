// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: mlRMMLBrowser.h,v $
// $Revision: 1.10 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2009/03/23 12:32:03 $
//              $Author: tandy $
//
// Copyright (C) 2004 РМЦ
//
// Этот файл является частью ОМС (Открытая мультимедиа система)
//
//////////////////////////////////////////////////////////////////////

#ifndef __rmml_Browser_Included__
#define __rmml_Browser_Included__


#define RMMLTN_Browser L"browser"

namespace rmml
{
/** !!!! ВАЖНО !!!!
 *  Перед тем как начать использовать этот класс
 *  с JavaScript'ом необходимо вызвать
 *  ф-цю mlRMMLBrowser::InitJSClass(cx,obj);
 */

#define EVNM_onScrolled L"onScrolled"
#define EVNM_onBeforeNavigate L"onBeforeNavigate"
#define EVNM_onNavigateComplete L"onNavigateComplete"
#define EVNM_onDocumentComplete L"onDocumentComplete"
#define EVNM_onProgressChange L"onProgressChange"
#define EVNM_onDownloadBegin L"onDownloadBegin"
#define EVNM_onDownloadComplete L"onDownloadComplete"
#define EVNM_onCommandStateChange L"onCommandStateChange"
#define EVNM_onNavigateError L"onNavigateError"

/** Класс для поддержки системного приложения Browser */
  class mlRMMLBrowser : 
                      public mlRMMLElement,
                      public mlJSClass,
                      public mlRMMLLoadable,
                      public mlRMMLVisible,
					  public mlRMMLPButton,
					  public mlIActiveX,
					  public mlIBrowser,
					  public moIBrowser
  {
	  bool mbInMyCall;
  protected:
	  class CInMyCall
	  {
//friend class mlRMMLBrowser;
		  mlRMMLBrowser* mpThis;
		  bool mbOldInMyCall;
	  public:
		  CInMyCall(mlRMMLBrowser* apThis){
			  mpThis = apThis;
			  mbOldInMyCall = mpThis->mbInMyCall;
			  mpThis->mbInMyCall = true;
		  }
		  ~CInMyCall(){
			  mpThis->mbInMyCall = mbOldInMyCall;
		  }
	  };
	friend class CInMyCall;

  public:
    mlRMMLBrowser();
	void destroy() { MP_DELETE_THIS }
    virtual ~mlRMMLBrowser();

	bool IsInMyCall(){ return mbInMyCall; }

// реализация mlJSClass
MLJSCLASS_DECL
private:
	enum {
		JSPROP_documentType = 1, // (RO) Type of the contained document object
		JSPROP_locationName,	// (RO) Short (UI-friendly) name of the URL/file currently viewed
		JSPROP_locationURL,		// (RO) Full URL/path currently viewed
		JSPROP_busy,			// (RO) If something is still in progress
		JSPROP_readyState,		// (RO) Ready state of the object (UNINITIALIZED = 0, LOADING = 1, LOADED = 2, INTERACTIVE = 3, COMPLETE = 4)
		JSPROP_offline,			// Controls if the frame is offline (read from cache)
		JSPROP_silent,			// Controls if any dialog boxes can be shown
		JSPROP_scrollTop,		// distance between the top of the object and the topmost portion of the content currently visible in the window
		JSPROP_scrollLeft,		// distance between the left edge of the object and the leftmost portion of the content currently visible in the window
		JSPROP_bkgColor,		// цвет фона ('#RGB' '#RRGGBB' '#AARRGGBB')
		JSPROP_defaultCharset, // кодировка по-умолчанию
		JSPROP_AXFrame,			// включение/выключение стандартной рамки (по умолчанию включена)
		JSPROP_value,			// HTML-контент
		EVID_onScrolled,		// документ проскроллировали html-ку (колесом мыши, выделением...)
		EVID_onBeforeNavigate,	// Fired when a new hyperlink is being navigated to
		EVID_onNavigateComplete, // Fired when the document being navigated to becomes visible and enters the navigation stack
		EVID_onDocumentComplete, // 
		EVID_onProgressChange,	// Fired when download progress is updated
		EVID_onDownloadBegin,	// Download of a page started
		EVID_onDownloadComplete, // Download of page complete
		EVID_onCommandStateChange, // The enabled state of a command changed 
		EVID_onNavigateError,	// Fires when an error occurs during navigation
	};

	bool AXFrame;
	mlColor bkgColor;
	MP_WSTRING defaultCharset;
	JSString* value;

	JSFUNC_DECL(navigate) // Navigates to a URL or file
	JSFUNC_DECL(goBack) // Navigates to the previous item in the history list
	JSFUNC_DECL(goForward) // Navigates to the next item in the history list
	JSFUNC_DECL(goHome) // Go home/start page (src)
	JSFUNC_DECL(refresh) // Refresh the currently viewed page (NORMAL = 0, IFEXPIRED = 1, CONTINUE = 2, COMPLETELY = 3)
	JSFUNC_DECL(stop) // Stops opening a file
	JSFUNC_DECL(scrollTo) // Scrolls the window to the specified x- and y-offset
	JSFUNC_DECL(scrollBy) // Causes the window to scroll relative to the current scrolled position by the specified x- and y-pixel offset
	// JSFUNC_DECL(scrollToPrevPage) // 
	// JSFUNC_DECL(scrollToNextPage) // 
	JSFUNC_DECL(scrollToAnchor) // 
	JSFUNC_DECL(execScript) // 
	JSFUNC_DECL(execCommand) // 
	JSFUNC_DECL(queryCommandEnabled) // 
	JSFUNC_DECL(fireMouseDown) // эмулирует нажатие левой кнопки мыши на браузере (x,y - относительно левого верхнего угла браузера)
	JSFUNC_DECL(fireMouseMove) // эмулирует перемещение мыши в браузере
	JSFUNC_DECL(fireMouseUp) // эмулирует отпускание левой кнопки мыши на браузере

/*
	void navigate(
		string URL, 
		[optional] int Flags, // 2 - NoHistory, 4 - NoHistory, 8 - NoWriteToCache
		[optional] string TargetFrameName, 
		[optional] string PostData, 
		[optional] string Headers); // Navigates to a URL or file
	void goBack(); // Navigates to the previous item in the history list
	void goForward(); // Navigates to the next item in the history list
	void goHome(); // Go home/start page (src)
	void refresh([optional] int level); // Refresh the currently viewed page (NORMAL = 0, IFEXPIRED = 1, CONTINUE = 2, COMPLETELY = 3)
	void stop(); // Stops opening a file
	bool scrollTo(int x, int y); // абсолютный
	bool scrollBy(int x, int y); // относительный 
	?scrollToPrevPage)
	?scrollToNextPage)
	void scrollToAnchor(string anchor);
	VARIANT execScript(string code, [optional, defaultvalue("JScript")] string language);
	bool execCommand(string cmdID, [optional, defaultvalue(0)] VARIANT_BOOL showUI, [optional] VARIANT value);
	bool queryCommandEnabled(string cmdID);
*/

  // реализация mlRMMLElement
    MLRMMLELEMENT_IMPL
    virtual mlresult CreateMedia(omsContext* amcx);
    virtual mlresult Load();
    virtual mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	virtual bool SetEventData(char aidEvent, mlSynchData &Data);

  // реализация mlILoadable
    MLILOADABLE_IMPL

  // реализация mlIVisible
    MLIVISIBLE_IMPL

// реализация  mlIButton
	// пока реалимзован только enabled. Если enabled == false, то браузер не реагирует ни на какой внешний ввод
	MLIBUTTON_IMPL
	virtual void OnPress(mlPoint apntXY);

// реализация mlIActiveX
  public:
	mlIActiveX* GetIActiveX(){ return this; }
	omsresult CallExternal(const wchar_t* apProcName, int aiArgC, 
			ML_VARIANT* pvarArgs, ML_VARIANT* varRes, bool abEvent);

// реализация mlIBrowser
  public:
	mlIBrowser* GetIBrowser(){ return this; }
	// Свойства
	virtual mlColor GetBkgColor();
	virtual const wchar_t* GetDefaultCharset();
	virtual bool GetAXFrame();
	virtual const wchar_t* GetValue();
	// Методы
	// События
	virtual void onScrolled(); // документ проскроллировали html-ку (колесом мыши, выделением...)
		// Fired when a new hyperlink is being navigated to.
	virtual void onBeforeNavigate(const wchar_t* URL, int flags, const wchar_t* targetFrameName, 
				const wchar_t* postData, const wchar_t* headers, bool &cancel);
		// Fired when the document being navigated to becomes visible and enters the navigation stack
	virtual void onNavigateComplete(const wchar_t* URL);
		// Fired when a document has been completely loaded and initialized.
	virtual void onDocumentComplete(const wchar_t* URL);
		// Fired when download progress is updated
	virtual void onProgressChange(int progress, int progressMax);
	virtual void onDownloadBegin(); // Download of a page started
	virtual void onDownloadComplete(); // Download of page complete
	virtual void onCommandStateChange(int command, bool enable); // The enabled state of a command changed 
		// Fires when an error occurs during navigation
	virtual void onNavigateError(const wchar_t* URL, const wchar_t* frame, int StatusCode, bool &cancel);

// реализация moIBrowser
	// Свойства
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
  };
}

#endif // __rmml_Browser_Included__