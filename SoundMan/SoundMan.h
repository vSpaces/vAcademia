#ifndef __SOUNDMAN_H_
#define __SOUNDMAN_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SOUNDMAN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SOUNDMAN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SOUNDMAN_EXPORTS
#define SOUNDMAN_API __declspec(dllexport)
#else
#define SOUNDMAN_API __declspec(dllimport)
#endif

// This class is exported from the SoundMan.dll
class SOUNDMAN_API CSoundMan {
public:
	CSoundMan(void);
	// TODO: add your methods here.
};

extern SOUNDMAN_API int nSoundMan;

SOUNDMAN_API int fnSoundMan(void);

#include "oms/oms_context.h"

namespace sm
{
	SOUNDMAN_API oms::omsresult CreateSoundManager( oms::omsContext* aContext);
	SOUNDMAN_API void DestroySoundManager( oms::omsContext* aContext);
}
#endif