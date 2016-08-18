#pragma once

#ifdef LOGGER_EXPORTS
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif

#include "iclientsession.h"
#include "oms_context.h"
#include "..\LogWriter\ILogger.h"

namespace lgs
{
	struct LOGGER_NO_VTABLE ILogger :  public ILoggerBase
	{
		virtual void SetClientSession(cs::iclientsession* apClientSession) = 0;

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


	LOGGER_API unsigned int CreateLogger( oms::omsContext* aContext);
	LOGGER_API void DestroyLogger( oms::omsContext* aContext);
}