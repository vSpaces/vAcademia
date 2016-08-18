#pragma once

#include "IDesktopSelector.h"
#include "../SharingContext.h"
#include "selectorbase.h"

class CDesktopFrame;
class CDevicesCtrl;
class CVideoInputSelectorDialog;
class CVideoInputRunChecker;
class CVideoInputGraphRunning;

class CVideoInputSelector: public CEventSource<IDesktopSelectorEvents>
						, public IDesktopSelector, public IWebCameraSelector
						, public CSelectorBase
{
public:
	CVideoInputSelector(oms::omsContext* apContext, CDevicesCtrl* aDeviceController);
	~CVideoInputSelector(void);

	// реализация IDesktopSelector
public:
	virtual bool Release();
	// начинает выборку области
	virtual bool StartSourceSelection();
	// заканчивает выборку области
	virtual void CancelSourceSelection();
	// возвращает последний полученный кадр
	virtual IDesktopFrame* GetCurrentFrame();
	// делает снимок нового кадра с текущими параметрами
	virtual IDesktopFrame* RecapFrame();
	// возвращает текущую область захвата
	virtual RECT GetCurrentSelectingRect();
	//возвращает true если захват возможен
	virtual bool IsCaptureAvailable(); 
	//возвращает handle окна с которого захватывается изображение или NULL в противном случае
	virtual HWND GetCapturedWindow();
	//нужно ли показывать область с которой ведётся захват
	virtual bool CanShowCaptureRect();
	//установить режим отображения курсора (0-скрыт, 1-отображается, 2-отображается увеличенным)
	virtual void SetShowCursorState(int state){}
	virtual bool IsLive();
	virtual int GetType();
	virtual void EmulateSelection( int aLeft, int aRight, int aTop, int aBottom){};
	virtual bool Run(){ return true;};
	void SetInteractiveBoardCapture(int aMonitorID){};
	void UpdateCurrentSelectingRect(){};
	void Seek( double aSeek){};
	void Play(){};
	void Pause(){};
	double GetDuration(){return 0;};
	//реализация IWebCameraSelector
public:
	//получение числа вебкамер
	virtual int GetWebNumsCameraDevices();
	//получение разрешений вебкамеры
	virtual std::vector<Resolution> GetWebCameraResolutions(int numDevice);
	virtual bool SetResolution(Resolution newResolution, int numDevice, bool reRun);
	virtual std::string GetWebCameraName(int numDevice);
	virtual bool SetActiveDevice(int numDevice);

public:
	CDevicesCtrl* GetDeviceController();
	void SelectionComplete(int resultCode);
	void SetResolutionComplete(int resultCode);
	void VideoInputGraphRunEnd();

private:
	void ClearCapturedImage();
	void Destroy();
	void CleanUp();

public:
	bool				m_firstCaptureFrame;
	CRect				m_currentRect;
	int					m_numActiveDevice;
	MP_STRING			m_deviceDisplayName;
	CCriticalSection	csDestroy;
	bool				m_deviceInitializing;
	bool				m_needDeleteFromDeviceRunner;
	bool				m_deviceWasStarted;
	bool				m_isDeleted; // можно удалять объект поток инициализации камеры завершился

private:
	CDesktopFrame*		m_currentFrame;
	CDevicesCtrl*		m_deviceController;
	int					m_size;
	CVideoInputSelectorDialog* m_viDialog;
	CVideoInputGraphRunning*	m_deviceRunner;
	CVideoInputRunChecker* m_runChecker;

	oms::omsContext*	context;
};

class CVideoInputRunChecker: public ViskoeThread::CThreadImpl<CVideoInputRunChecker>
{
public:
	CVideoInputRunChecker(CVideoInputSelector* selector);
	~CVideoInputRunChecker(void);
	// реализация CThreadImpl<CVideoInputGraphRunning>
	DWORD Run();

private:
	CVideoInputSelector*		m_VISelector;
	CVideoInputGraphRunning*	m_graphRunningThread;
};

class CVideoInputGraphRunning: public ViskoeThread::CThreadImpl<CVideoInputGraphRunning> 
{
public:
	CVideoInputGraphRunning(CVideoInputSelector* selector, bool notifyAboutComplete);
	~CVideoInputGraphRunning(void);
	// реализация CThreadImpl<CVideoInputGraphRunning>
	DWORD Run();

private:
	CVideoInputSelector*		m_VISelector;
	bool						m_notifyAboutComplete;
};
