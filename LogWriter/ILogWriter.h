#pragma once

#include "LogValue.h"
#include "ILogger.h"

typedef enum LOG_TYPE
{
	ltDefault = 0,
	ltAudiere,
	ltAudioCap,
	ltAvatarEditor,
	ltBrowser,
	ltCal3d,
	ltComMan,
	ltCscl3DWS,
	ltDescktopMan,
	ltFlash,
	ltInfoMan,
	ltLogger,
	ltMdump,
	ltNanimation,
	ltNcommon,
	ltNengine,
	ltNinfo,
	ltNmath,
	ltModelLoader,
	ltNphysics,
	ltRenderTarget,
	ltNtexture,
	ltOMSPlayer,
	ltPathRefiner,
	ltRenderManager,
	ltResLoader,
	ltRmml,
	ltServiceMan,
	ltSoundMan,
	ltSyncMan,
	ltVASEncoder,
	ltVideo,
	ltVoipMan,
	ltReserved1,
	ltReserved2,
	ltReserved3,
	ltReserved4
};

struct IExternalLogWriter
{
	virtual void Trace(const std::string& text, LOG_TYPE logType) = 0;
};

class ILogWriter
{
public:
	virtual void SetMode(bool isWorking) = 0;
	virtual void SetApplicationDataDirectory(const wchar_t* alpApplicationDataDirectory) = 0;
	virtual std::wstring GetApplicationDataDirectory() = 0;

	// включение выключение режима посылки сообщений лога InfoMan-у.
	virtual void StartSendingToInfoMan(IExternalLogWriter* aInfoManager) = 0;
	virtual void StopSendingToInfoMan() = 0;

	// включение выключение режима посылки сообщений лога —ерверу.
	virtual void StartSendingToServer(lgs::ILoggerBase* aLogSender) = 0;
	virtual void StopSendingToServer() = 0;

	virtual std::wstring GetLogFilePath() = 0;

	virtual void StopWriteLog() = 0;
	virtual void ContinueWriteLog() = 0;

	// печать буфера дл€ дальнейшего его использовани€ дл€ отладки
	virtual void WriteBufferAsArray(const unsigned char* buf, unsigned int auSize, LOG_TYPE logType = ltDefault) = 0;

	// печать лога с временем его записи
	virtual void WriteLPCSTR(const char* str, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLnLPCSTR(const char* str, LOG_TYPE logType = ltDefault) = 0;

	// печать лога без времени его записи
	virtual void PrintLPCSTR(const char* str, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLnLPCSTR(const char* str, LOG_TYPE logType = ltDefault) = 0;

	// печать лога с временем его записи
	virtual void WriteLn(CLogValue obj, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType = ltDefault) = 0;
	virtual void WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType = ltDefault) = 0;

	virtual void Write(CLogValue obj, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType = ltDefault) = 0;
	virtual void Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType = ltDefault) = 0;

	// печать лога без времени его записи
	virtual void PrintLn(CLogValue obj, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType = ltDefault) = 0;
	virtual void PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType = ltDefault) = 0;

	virtual void Print(CLogValue obj, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType = ltDefault) = 0;
	virtual void Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType = ltDefault) = 0;
};
