#ifndef __INFOMAN_H_
#define __INFOMAN_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SYNCMAN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SYNCMAN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef INFOMAN_EXPORTS
	#define INFOMAN_API __declspec(dllexport)
#else
	#define INFOMAN_API __declspec(dllimport)
#endif

#ifdef WIN32
	#define INFOMAN_NO_VTABLE __declspec(novtable)
#else
	#define INFOMAN_NO_VTABLE
#endif

#include "oms_context.h"
#include "rmml.h"

namespace info
{

	struct INFOMAN_NO_VTABLE IInfoManager
	{		
		virtual void CreateWnd( unsigned long m_hParentWnd) = 0;
		virtual void ShowWindow( bool bShow) = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void TraceTestResult(char* resStr) = 0;
		virtual void ShowObject(const rmml::mlString& aTarget, bool aShowWindow) = 0;
		virtual void TraceSessionPacketInfo(std::string& asSessionName, BYTE packetType, DWORD size, bool isOut) = 0;
		virtual void SetAutoTestMode( int aTestMode) = 0;
		virtual bool IsTestMode();
		virtual void StartAutoTesting();
		virtual void TestEnded( int aiCompleteTest, int aiFailTest);
	};

	//typedef syncISyncManager ISyncManager;

	INFOMAN_API omsresult CreateInfoManager( oms::omsContext* aContext);
	INFOMAN_API void DestroyInfoManager( oms::omsContext* aContext);
}
#endif
