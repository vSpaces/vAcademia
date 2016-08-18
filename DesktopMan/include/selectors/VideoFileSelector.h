#pragma once

#include "IDesktopSelector.h"
#include "../SharingContext.h"
#include "selectorbase.h"
#include "../movie.h"

#define VCD_PLAYING          0x0001
#define VCD_STOPPED          0x0002
#define VCD_PAUSED           0x0004

class CDesktopFrame;
class CVideoFileGraphRunning;

class CVideoFileSelector: public CEventSource<IDesktopSelectorEvents>
						, public IDesktopSelector, public CSelectorBase
{
public:
	CVideoFileSelector(oms::omsContext* apContext, LPCWSTR aFileName);
	~CVideoFileSelector(void);

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
	virtual bool Run();

	void SetInteractiveBoardCapture(int aMonitorID){}
	void UpdateCurrentSelectingRect(){}
	void Seek( double aSeek);
	void Play();
	void Pause();
	double GetDuration();
	void SetBoardName( LPCSTR alpTarget);

public:
	void SelectionComplete(int resultCode);
	//void SetResolutionComplete(int resultCode);
	void VideoFileGraphRunEnd();
	CMovie *GetNewMovie();
	void CloseMovie();
	void SetPlayMode( bool aPlaying);

private:
	void ClearCapturedImage();
	void Destroy();
	void CleanUp();	
	void SetBarPosition( bool aForce);
	bool VideoIsEnd();

public:
	bool				m_firstCaptureFrame;
	CRect				m_currentRect;
	MP_WSTRING			m_fileName;
	CCriticalSection	csSeek;
	bool				m_isDeleted; // можно удалять объект поток инициализации видео завершился

private:
	CDesktopFrame*		m_currentFrame;
	CDevicesCtrl*		m_deviceController;
	int					m_size;	
	CVideoFileGraphRunning*	m_videoRunner;

	oms::omsContext*	context;
	CMovie              *m_pMovie;

	int					m_sizeBar;
	REFTIME				m_currPos;
	REFTIME				m_duration;
	CComString			m_target;
	bool				m_playing;
	int					m_seek;
	bool				m_needVideoRunner;
	bool				m_videoWasStarted;
	CCriticalSection	csDestroy;
};

/*class CVideoFileRunChecker: public ViskoeThread::CThreadImpl<CVideoInputRunChecker>
{
public:
	CVideoFileRunChecker(CVideoInputSelector* selector);
	~CVideoFileRunChecker(void);
	// реализация CThreadImpl<CVideoInputGraphRunning>
	DWORD Run();

private:
	CVideoFileSelector*		m_VideoFileSelector;
	CVideoFileGraphRunning*	m_videoRunning;
};*/

class CVideoFileGraphRunning: public ViskoeThread::CThreadImpl<CVideoFileGraphRunning> 
{
public:
	CVideoFileGraphRunning(CVideoFileSelector* selector, bool notifyAboutComplete, LPCWSTR aFileName);
	~CVideoFileGraphRunning(void);
	// реализация CThreadImpl<CVideoInputGraphRunning>
	DWORD Run();

private:
	CVideoFileSelector*		m_VideoSelector;
	bool					m_notifyAboutComplete;
	MP_WSTRING			m_fileName;	
};
