#pragma once

const int RECT_SELECTOR = 1;
const int WND_SELECTOR = 2;
const int VIDEOIN_SELECTOR = 3;
const int INTERACTIVEBOARD_SELECTOR = 4;
const int DROPFILE_SELECTOR = 5;
const int CLIPBOARD_SELECTOR = 6;
const int VIDEOFILE_SELECTOR = 7;

struct IDesktopFrame
{
	virtual void			Release() = 0;
	virtual IDesktopFrame*	Clone() = 0;
	virtual unsigned int	GetWidth() = 0;
	virtual unsigned int	GetHeight() = 0;
	virtual unsigned int	GetDataSize() = 0;
	virtual HBITMAP	GetHBitmap() = 0;
	virtual void*	GetData() = 0;
};

struct IDesktopSelectorEvents
{
	virtual void OnSelectionCompleted(int resultCode) = 0;
	virtual void OnSelectionEscape() = 0;
	virtual void OnSetBarPosition( LPCSTR aTarget, std::vector<double> v1) = 0;
	virtual void OnVideoEnd( LPCSTR aTarget) = 0;
};

struct Resolution
{
	unsigned int width;
	unsigned int height;
}; 

struct IWebCameraSelector
{
	//получение числа вебкамер
	virtual int GetWebNumsCameraDevices() = 0;
	//получение разрешений вебкамеры
	virtual std::vector<Resolution> GetWebCameraResolutions(int numDevice) = 0;
	virtual std::string GetWebCameraName(int numDevice) = 0;
	virtual bool SetResolution(Resolution newResolution, int numDevice, bool reRun) = 0;
	virtual bool SetActiveDevice(int numDevice) = 0;
};

struct IWebCameraSelectorEvents
{
	virtual void OnSetResolutionCompleted(int resultCode) = 0;
};

struct IDesktopSelector
{
	// начинает выборку области
	virtual bool Release() = 0;
	// начинает выборку области
	virtual bool StartSourceSelection() = 0;
	// заканчивает выборку области
	virtual void CancelSourceSelection() = 0;
	// возвращает последний полученный кадр
	virtual IDesktopFrame* GetCurrentFrame() = 0;
	// делает снимок нового кадра с текущими параметрами
	virtual IDesktopFrame* RecapFrame() = 0;
	// возвращает текущую область захвата и координаты окна
	virtual RECT GetCurrentSelectingRect() = 0;
	//возвращает true если захват возможен
	virtual bool IsCaptureAvailable() = 0; 
	//возвращает handle окна с которого захватывается изображение или NULL в противном случае
	virtual HWND GetCapturedWindow() = 0;
	//нужно ли показывать область с которой ведётся захват
	virtual bool CanShowCaptureRect() = 0;
	//установить режим отображения курсора (0-скрыт, 1-отображается, 2-отображается увеличенным)
	virtual void SetShowCursorState(int state) = 0;
	//установить режим захвата с интерактивной доски
	virtual void SetInteractiveBoardCapture(int aMonitorID) = 0;
	//
	virtual void UpdateCurrentSelectingRect() = 0;
	virtual bool IsLive() = 0;
	//возвращает тип селектора - область / окно и тд
	virtual int GetType() = 0;

	virtual void EmulateSelection( int aLeft, int aRight, int aTop, int aBottom) = 0;

	virtual bool Run() = 0;

	virtual void Seek( double aSeek)=0;
	virtual void Play()=0;
	virtual void Pause()=0;
	virtual double GetDuration() = 0;
};