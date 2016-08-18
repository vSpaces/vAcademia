
#ifndef _tracer_h___
#define _tracer_h___

#include "../LogWriter/ILogger.h"

class wsTracer: public trc::trcITracer
{
	char* mpLog;
	unsigned int miSize;
	unsigned int miRealSize;
	bool m_isWorking;
	bool m_isUserScriptsCompling;
	int miSent;
	FILE* m_file;
	lgs::ILoggerBase* mpLoggerSender;
	void Append(const char* pszText);
public:
	wsTracer();
	~wsTracer();
	void Send(HWND ahwndTracer, bool abAll);

public:
	void trace(const char* pszFormat, ...);
	void setTraceMode(bool isWorking);
	void StartSendingToServer( lgs::ILoggerBase* aLoggerSender);
	void StopSendingToServer();
	void StartCompileUserScript();
	void EndCompileUserScript();
};

#endif