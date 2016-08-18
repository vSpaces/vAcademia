#pragma once

#ifdef WIN32
#define LOGGER_NO_VTABLE __declspec(novtable)
#else
#define LOGGER_NO_VTABLE
#endif

#define SEND_LOGWRITER_LOG 1
#define SEND_TRACER_LOG 2

namespace lgs
{
	struct LOGGER_NO_VTABLE ILoggerBase
	{
		virtual void SetMode(bool isWorking) = 0;
	
		virtual void WriteLPCSTR(const char * str) = 0;
		virtual void WriteLnLPCSTR(const char * str) = 0;
		// печать лога с временем его записи
		virtual void WriteLPCSTR(const char* time, const char * str) = 0;
		virtual void WriteLnLPCSTR(const char* time, const char * str) = 0;
		virtual void PrintLPCSTR(const char * str) = 0;
		virtual void PrintLnLPCSTR(const char * str) = 0;
		virtual void Destroy();
		
	};
}