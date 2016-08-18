#pragma once

#include "CommonCommonHeader.h"

#include <string>
#include "ILogWriter.h"
#include "LogValue.h"

class CInternalLogWriter
{
friend class CGlobalSingletonStorage;

public:
	ILogWriter*	GetILogWriter();

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

private:
	CInternalLogWriter();
	~CInternalLogWriter();
	ILogWriter*	m_pLogWriter;
};
