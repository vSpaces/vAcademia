
#pragma once

class IDynamicTexture;

class IFlash
{
public:

	// Возвращает динамическую текстуру
	virtual IDynamicTexture* GetDynamicTexture() = 0;
	// установить размер "окна" флэша
	virtual bool SetSize(int aiWidth, int aiHeight) = 0;
/*
	// обработка событий мыши
	virtual bool FireMouseDown(int x, int y)=0; // x,y относительно левого верхнего угла окна браузера
	virtual bool FireMouseMove(int x, int y)=0;
	virtual bool FireMouseUp(int x, int y)=0;
	virtual bool FireMouseWheel(int x, int y, int distance)=0;
	// обработка событий клавиатуры
	virtual bool KeyDown(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode)=0;
	virtual bool KeyUp(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode)=0;
	virtual bool KeyPress(unsigned int charCode, unsigned long scanCode)=0;
*/
	virtual bool SetFocus()=0;
	virtual bool KillFocus()=0;
/*
	virtual bool ExecScript(const wchar_t* scriptCode, VARIANT* result)=0;
	// возвращает критическую секцию, которую надо использовать при обработке сообщений 
	// вспомогательных дочерних окон браузера
	virtual void* GetCSForChildWindows() = 0;
	// передать нэндл главного окна
	virtual bool SetParentWindow(HWND aHwnd) = 0;
*/
	virtual bool LoadMovie(const wchar_t* apwcSrc)=0; 

	virtual bool SetVariable(const wchar_t *var, const wchar_t *value) = 0;

	virtual int GetScaleMode()=0;
	virtual void SetScaleMode(int)=0;

	virtual int GetWMode()=0;
	virtual void SetWMode(int)=0;

	virtual const wchar_t* GetVariable(const wchar_t *var) = 0;
	virtual const wchar_t* CallFunction(const wchar_t *request) = 0;

};

struct IFlashCallBack
{
	virtual void OnAdjustSize(int& aiWidth, int& aiHeight)=0; // получены родные размеры флэшки, уточнить размеры текстуры, на которую она будет отрисовываться

	virtual void onFSCommand(const wchar_t *command, const wchar_t *args) = 0;

/*
	//////////////////////////////////////////////////////////////////////////
	virtual void on_scrolled()=0; // документ проскроллировали html-ку (колесом мыши, выделением...)
	// Fired when a new hyperlink is being navigated to.
	virtual void on_before_navigate(const wchar_t* URL, int flags, const wchar_t* targetFrameName, const wchar_t* postData, const wchar_t* headers, bool &cancel)=0;
	// Fired when the document being navigated to becomes visible and enters the navigation stack
	virtual void on_navigate_complete(const wchar_t* URL)=0;
	virtual void on_document_complete(const wchar_t* URL)=0;
	// Fired when download progress is updated
	virtual void on_progress_change(int progress, int progressMax)=0;
	virtual void on_download_begin()=0; // Download of a page started
	virtual void on_download_complete()=0; // Download of page complete
	virtual void on_command_state_change(int command, bool enable)=0; // The enabled state of a command changed 
	// Fires when an error occurs during navigation
	virtual void on_navigate_error(const wchar_t* URL, const wchar_t* frame, int StatusCode, bool &cancel)=0;
	// вызов из JavaScript через window.external
	virtual void external_callback(const wchar_t* apProcName, int aiArgC, 
		VARIANT* pvarArgs, VARIANT* varRes, bool abEvent)=0;
*/
};
