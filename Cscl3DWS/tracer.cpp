
#include "stdafx.h"
#include "oms/oms_context.h"
#include "tracer.h"

/*
class wsTracer: public trc::trcITracer
{
	char* mpLog;
	unsigned int miSize;
	unsigned int miRealSize;
	int miSent;
public:
	wsTracer();
	~wsTracer();
	void Send(HWND ahwndTracer, bool abAll);

// реализация trc::trcITracer
public:
	void trace(const char* pszFormat, ...);
};
*/

wsTracer::wsTracer(){
	mpLog=NULL;
	miSize=0;
	miRealSize=0;
	miSent=0;
	m_isWorking = true;
	mpLoggerSender = NULL;
	m_isUserScriptsCompling = false;
}

wsTracer::~wsTracer(){
	if(mpLog!=NULL) MP_DELETE_ARR( mpLog);
}

void wsTracer::Send(HWND ahwndTracer, bool abAll){
	m_isWorking = true;
	COPYDATASTRUCT CopyData;
	if(abAll){
		CopyData.dwData=0;
		CopyData.cbData=miSize;
		CopyData.lpData=mpLog;
	}else{
		CopyData.dwData=1;
		CopyData.cbData=miSize-miSent;
		CopyData.lpData=mpLog+miSent;
	}
	miSent=miSize;
	::SendMessage( ahwndTracer, WM_COPYDATA, NULL, (LPARAM)(LPVOID) &CopyData);
}

void wsTracer::Append(const char* pszText){
	int iLen=strlen(pszText);
	if(miRealSize < (miSize+iLen)){
		miRealSize=miSize+iLen+4000;
		char* pLog = MP_NEW_ARR( char, miRealSize);
		if(mpLog!=NULL){
			memcpy(pLog,mpLog,miSize);
			MP_DELETE_ARR( mpLog);
		}
		mpLog=pLog;
	}
	memcpy(mpLog+miSize,pszText,iLen);
	miSize+=iLen;
	if(miSize > 10000){
		// надо урезать log
		int iDec=2000;
		miSize-=iDec;
		memcpy(mpLog,mpLog+iDec,miSize);
		miSent-=iDec;
		if(miSent<0) miSent=0;
	}
}

void wsTracer::trace(const char* pszFormat, ...)
	{
	CComString sErrorMessage;

	if ((m_isWorking) || (mpLoggerSender))
	{
	va_list ptr;
	va_start(ptr, pszFormat);
	sErrorMessage.FormatV(pszFormat, ptr);
	va_end(ptr);
	}

	if(mpLoggerSender!=NULL)
	{
		mpLoggerSender->WriteLPCSTR( sErrorMessage.GetBuffer());
	}

	if (!m_isWorking)
	{
		return;
	}

	::OutputDebugString(sErrorMessage);
	if (m_isUserScriptsCompling)
	{
		sErrorMessage = "SM: [!OBJ!]" + sErrorMessage;
	}
	
	Append(sErrorMessage);	
}

void wsTracer::StartCompileUserScript()
{
	m_isUserScriptsCompling = true;
}

void wsTracer::EndCompileUserScript()
{
	m_isUserScriptsCompling = false;
}

void wsTracer::setTraceMode(bool isWorking)
{
	m_isWorking = isWorking;
}

void wsTracer::StartSendingToServer( lgs::ILoggerBase* aLoggerSender)
{
	mpLoggerSender = aLoggerSender;
}

void wsTracer::StopSendingToServer()
{
	mpLoggerSender = NULL;
}