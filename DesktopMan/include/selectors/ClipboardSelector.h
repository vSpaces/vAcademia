#pragma once

#include "IDesktopSelector.h"

class CClipboardSelector : public CEventSource<IDesktopSelectorEvents>,	public IDesktopSelector
{
public:
	CClipboardSelector(oms::omsContext* apContext);
	virtual ~CClipboardSelector();
	// реализация IDesktopSelector
public:
	virtual bool Release()
	{
		MP_DELETE_THIS;
		return true;
	}

	// начинает выборку области
	virtual bool StartSourceSelection()
	{
		return false;
	}

	// заканчивает выборку области
	virtual void CancelSourceSelection(){}
	// возвращает последний полученный кадр
	virtual IDesktopFrame* GetCurrentFrame();

	// делает снимок нового кадра с текущими параметрами
	virtual IDesktopFrame* RecapFrame()
	{
		return NULL;
	}

	// возвращает текущую область захвата
	virtual RECT GetCurrentSelectingRect();


	//возвращает true если захват возможен
	virtual bool IsCaptureAvailable() 
	{
		return true;
	}

	//возвращает handle окна с которого захватывается изображение или NULL в противном случае
	virtual HWND GetCapturedWindow()
	{
		return NULL;
	}

	//нужно ли показывать область с которой ведётся захват
	virtual bool CanShowCaptureRect()
	{
		return false;
	}

	//установить режим отображения курсора (0-скрыт, 1-отображается, 2-отображается увеличенным)
	virtual void SetShowCursorState(int state){}

	virtual bool IsLive()
	{
		return true;
	}

	virtual int GetType()
	{
		return CLIPBOARD_SELECTOR;
	}
	virtual void EmulateSelection( int aLeft, int aRight, int aTop, int aBottom){};
	virtual bool Run(){ return true;};
	void SetInteractiveBoardCapture(int aMonitorID){};
	void UpdateCurrentSelectingRect(){};
	void Seek( double aSeek){};
	void Play(){};
	void Pause(){};
	double GetDuration(){return 0;};

private:
	void ClearCapturedImage();

	CDesktopFrame*		currentFrame;
	oms::omsContext*	context;
};