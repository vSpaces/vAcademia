#pragma once

#include "ViskoeThread.h"
#include "EventSource.h"

struct IKeyboardHookEvents
{
	virtual void OnDebugBreakRequest() = 0;
	virtual void OnDumpRequest() = 0;
};

class CGlobalKeyboardHook : public ViskoeThread::CThreadImpl<CGlobalKeyboardHook>
							, public CEventSource<IKeyboardHookEvents>
{
public:
	CGlobalKeyboardHook(HINSTANCE ahInstance, oms::omsContext* apOmsContext);
	~CGlobalKeyboardHook();

public:
	DWORD	Run();

private:
	HINSTANCE hInstance;
};