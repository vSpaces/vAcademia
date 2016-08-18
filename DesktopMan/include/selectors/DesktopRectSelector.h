#pragma once

#include "IDesktopSelector.h"
#include "../SharingContext.h"
#include "cursorcapture.h"
#include "selectorbase.h"

class CDesktopFrame;

class CDesktopRectSelector : public CEventSource<IDesktopSelectorEvents>
							,	public IDesktopSelector
							, public CCursorCapture
							, public CSelectorBase
{
	friend class CCaptureWindow;
public:
	CDesktopRectSelector(oms::omsContext* apContext);
	virtual ~CDesktopRectSelector();

	typedef std::vector<MONITORINFOEX>		vecMonitorsInfo;
	typedef MP_VECTOR<CCaptureWindow*>	vecCaptureWindows;

	// реализация IDesktopSelector
public:
	virtual bool Release()
	{
		MP_DELETE_THIS;
		return true;
	}
	// начинает выборку области
	virtual bool StartSourceSelection();
	// заканчивает выборку области
	virtual void CancelSourceSelection();
	// заканчивает выборку области по клавише ESC
	virtual void EscSourceSelection();
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
	virtual void SetShowCursorState(int state);
	virtual bool IsLive()
	{
		return true;
	}
	virtual int GetType();
	void SetInteractiveBoardCapture(int aMonitorID);

	void EmulateSelection( int aLeft, int aRight, int aTop, int aBottom);
	virtual bool Run(){ return true;};
	void Seek( double aSeek){};
	void Play(){};
	void Pause(){};
	double GetDuration(){return 0;};

private:
	void	CreateCaptureWindows();
	void	CreateMonitorCaptureWindow( MONITORINFOEX& pInfo);
	void	UpdateSelectionRect( int aX, int aY, CCaptureWindow* apCaptureWindow);
	void	HideCaptureWindows();
	void	StartSelection( int aX, int aY, CCaptureWindow* apCaptureWindow);
	void	ClearCapturedImage();
	void	CreateCapturedImage();
	void	StopSelection( int aX, int aY, CCaptureWindow* apCaptureWindow);
	void	HandleMouseMove( int aX, int aY, CCaptureWindow* apCaptureWindow);
	void	DestroyCaptureWindows();
	void	ScaleSelectionRect(float dx, float dy);
	void	FindScale( MONITORINFOEX& info, float& dx, float& dy);

	CCaptureWindow*	GetVirtualBoardMonitor();
	void UpdateCurrentSelectingRect();

	void	CreateDCesIfNeeded();
	void	DeleteDCes();

private:
	vecCaptureWindows	captureWindows;
	vecCaptureWindows	captureBuffers;
	//! selection params
	bool				selectionOnProgress;
	RECT				selectionRect;
	POINT				startPoint;
	POINT				currentPoint;
	int					lButtonDownCount;

	//! Captured Image
	HBITMAP				hCapturedImage;
	CDesktopFrame*		currentFrame;

	int					screenWidth;
	int					screenHeight;

	HDC					m_hScreenDC;
	HDC					m_hBackHDC;

	oms::omsContext*	context;
	bool				m_isInteractiveBoardCapture;
	int					m_interactiveBoardMonitorID;
	bool				m_needUpdateInteractiveBoard;
};