
#pragma once

#include <string>
//#include "ILogWriter.h"
#include "LogValue.h"

//#define	MAIN_LOG_FILENAME	"main.log"

//class CLogWriter : public ILogWriter
class CLogWriter
{
friend class CGlobalSingletonStorage;

public:
	void SetApplicationName(std::string appName);

	void SetMode(bool isWorking);

	// печать лога с временем его записи
	void WriteLPCSTR(const char* str);
	void WriteLnLPCSTR(const char* str);

	// печать лога без времени его записи
	void PrintLPCSTR(const char* str);
	void PrintLnLPCSTR(const char* str);

	// печать лога с временем его записи
	void WriteLn(CLogValue obj);
	void WriteLn(CLogValue obj1, CLogValue obj2);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9);
	void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10);

	void Write(CLogValue obj);
	void Write(CLogValue obj1, CLogValue obj2);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9);
	void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10);

	void PrintLn(CLogValue obj);
	void PrintLn(CLogValue obj1, CLogValue obj2);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9);
	void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10);

	// печать лога без времени его записи
	void Print(CLogValue obj);
	void Print(CLogValue obj1, CLogValue obj2);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9);
	void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10);

private:
	CRITICAL_SECTION m_cs;

	CLogWriter();
	CLogWriter(const CLogWriter&);
	CLogWriter& operator=(const CLogWriter&);
	~CLogWriter();

	FILE* m_file;
	bool m_isWorking;
	std::string m_appName;
	std::string m_logName;
};
