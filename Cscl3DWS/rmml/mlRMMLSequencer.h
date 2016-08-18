#pragma once

namespace rmml {

struct WaitJSExpr
{
	JSString* mjssExpr;
	MP_STRING msSrcFileName;
	int miSrcLine;

	WaitJSExpr():
		MP_STRING_INIT(msSrcFileName)
	{
	}

	WaitJSExpr(const WaitJSExpr& other):
		MP_STRING_INIT(msSrcFileName)
	{
		mjssExpr = other.mjssExpr;
		msSrcFileName = other.msSrcFileName;
		miSrcLine = other.miSrcLine;
	}
};

/**
 * Класс последовательного выполнения RMML
 */

class mlRMMLSequencer :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLContinuous
{
friend class nlKeepCurrentFrameAfterUpdate;
	__int64 mlTimeOfWaitingEnd;
	__int64 mlTimeToWaitAfterUnfreez; // сколько времени надо будет еще ждать после разморозки
	EventSender* mpEventSender;
	WaitJSExpr* mpWaitJSExpr;
	// список событий, которые мы ждем в WaitJSExpr
	MP_VECTOR<EventSender> mvWaitingEvents;
	MP_VECTOR<EventSender> mvFiredEvents;
//	short miCurrentFrameAfterUpdate;
public:
	mlRMMLSequencer(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLSequencer(void);
MLJSCLASS_DECL
protected:
	mlresult SetMeAsEventListener(wchar_t* apwcEvent);
	void YouAreSetAsEventListener(char aidEvent, mlRMMLElement* apSender);
	void FreeWaitJSExpr();
	bool CheckWaitJSExpr();
	void GetWaitingEvents();
	void Reset();

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult Update(const __int64 alTime);
	bool EventNotify(char aidEvent, mlRMMLElement* apSender);
	void EventSenderDestroyed(mlRMMLElement* apSender);
	bool Freeze();
	bool Unfreeze();
	void GlobalTimeShifted(const __int64 alTime, const __int64 alTimeShift, mlEModes aePrevMode);

// реализация  mlIContinuous
MLICONTINUOUS_IMPL
	void PlayingChanged();
	void FrameChanged();
	void PosChanged();
	int GetFrameByName(const wchar_t* apwcName);

};

}