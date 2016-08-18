
#pragma once

#include <string>
//#include "ILogWriter.h"
//#include "LogValue.h"
//#include "ILogger.h"
#include "LogWriter.h"

#define	MAIN_LOG_SC_FILENAME	"mainSC.log"

class CLogMessage
{
public:
	std::string text;
	char time[4 * MAX_INT_LENGTH];
	bool bNewStr;

public:
	CLogMessage()
	{
		time[0] ='\0';
	}

	CLogMessage( const char *apText, bool bTime, bool abNewStr)
	{
		SetMessage( apText, bTime, abNewStr);
	}	

	void SetMessage( const char *apText, bool bTime, bool abNewStr);

	bool IsEmpty()
	{
		return (text.size() == 0);
	}
};

class LOGWRITER_API CLogWriterLastMsgAsync : public CLogWriter
{
friend class CGlobalSingletonStorage;
//friend class ILogger;

public:	

	// печать лога с временем его записи
	void WriteLPCSTR(const char* str, LOG_TYPE logType = ltDefault);
	void WriteLnLPCSTR(const char* str, LOG_TYPE logType = ltDefault);

	// печать лога без времени его записи
	void PrintLPCSTR(const char* str, LOG_TYPE logType = ltDefault);
	void PrintLnLPCSTR(const char* str, LOG_TYPE logType = ltDefault);
	

private:	

	CLogWriterLastMsgAsync(const CLogWriterLastMsgAsync&);
	CLogWriterLastMsgAsync& operator=(const CLogWriterLastMsgAsync&);		

	static DWORD WINAPI writeThread_(LPVOID param);
	int writeThread();
	CLogMessage message;
	bool bThread;
	HANDLE hThread;
	DWORD dwThreadID;
	HANDLE hEvent;

public:
	CLogWriterLastMsgAsync();
	~CLogWriterLastMsgAsync();
};
