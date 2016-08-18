#ifndef __COMMAN_H_
#define __COMMAN_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SYNCMAN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// COMMAN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef COMMAN_EXPORTS
#define COMMAN_API __declspec(dllexport)
#else
#define COMMAN_API __declspec(dllimport)
#endif

#include "oms_context.h"
#include "CommanRes.h"

namespace cm
{
	struct COMMAN_NO_VTABLE IDataHandler
	{
		virtual long handleData( DWORD aDataSize, BYTE *aData) = 0;
	};

	struct COMMAN_NO_VTABLE IAsyncConnect
	{
		virtual long sendData(DWORD aDataSize, BYTE* aData)=0;
		//virtual long setDataHandler(IUnknown* aDataHandler);
		virtual long getConnectName(wchar_t ** aName)=0;
	};


	COMMAN_API unsigned int CreateComManager( oms::omsContext* aContext);
	COMMAN_API void DestroyComManager( oms::omsContext* aContext);
}

#endif