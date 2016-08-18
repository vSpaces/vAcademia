
#pragma once

#include <string>
#include "ILogWriter.h"
#include "LogValue.h"
#include "ILogger.h"

#define	MAIN_LOG_FILENAME	"main.log"

class LOGWRITER_API CLogWriter : public ILogWriter
{
friend class CGlobalSingletonStorage;
//friend class ILogger;

public:
	void SetApplicationName(std::string appName);

	void SetMode(bool isWorking);

	// включение выключение режима посылки сообщений лога InfoMan-у.
	void StartSendingToInfoMan(IExternalLogWriter* aInfoManager);
	void StopSendingToInfoMan();

	void StartSendingToServer( lgs::ILoggerBase* aLoggerSender);
	void StopSendingToServer();

	virtual std::wstring GetLogFilePath();
	//virtual std::string GetLogFileData();

	void StopWriteLog();
	void ContinueWriteLog();

	inline void SendToInfoMan(const std::string& str, LOG_TYPE logType, const char * time = NULL);

	// печать буфера для дальнейшего его использования для отладки
	void WriteBufferAsArray(const unsigned char* buf, unsigned int auSize, LOG_TYPE logType = ltDefault);

	// печать лога с временем его записи
	void WriteLPCSTR(const char* str, LOG_TYPE logType = ltDefault);
	void WriteLnLPCSTR(const char* str, LOG_TYPE logType = ltDefault);

	// печать лога без времени его записи
	void PrintLPCSTR(const char* str, LOG_TYPE logType = ltDefault);
	void PrintLnLPCSTR(const char* str, LOG_TYPE logType = ltDefault);

	// печать лога с временем его записи
	void WriteLn(CLogValue obj, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType = ltDefault);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType = ltDefault);

	void Write(CLogValue obj, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType = ltDefault);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType = ltDefault);

	// печать лога без времени его записи
	void PrintLn(CLogValue obj, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType = ltDefault);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType = ltDefault);

	void Print(CLogValue obj, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType = ltDefault);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType = ltDefault);

	void SetApplicationDataDirectory(LPCWSTR alpApplicationDataDirectory);
	std::wstring GetApplicationDataDirectory();

protected:
	std::wstring m_applicationDataDirectory;

	CRITICAL_SECTION m_cs;

	CLogWriter(const CLogWriter&);
	CLogWriter& operator=(const CLogWriter&);

	bool OpenFileIfNeeded();
	
	FILE* m_file;
	bool m_isWorking;
	std::string m_appName;
	std::string m_logName;
	IExternalLogWriter* mpInfoMan;
	lgs::ILoggerBase* mpLoggerSender;

public:
	CLogWriter();
	~CLogWriter();
};
