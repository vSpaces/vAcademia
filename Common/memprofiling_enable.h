#pragma once

#include "std_define.h"

/////////////////////////////////////////////////////////////////////
// Глобальные настройки
/////////////////////////////////////////////////////////////////////
// В релизе сбросим флаг
//#ifndef DEBUG
//	#ifndef _DEBUG
		#define	NO_PROFILING
//	#endif
//#endif

#ifdef AUDIERE_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef BIGFILEMAN_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef CAL3D_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef COMMAN_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef CSCL_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef DESKTOPMAN_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef INFOMAN_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef JSONSPIRIT_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef LOGGER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef LOGWRITER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NAVATAR_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NANIMATION_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NCOMMON_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NENGINE_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NINFO_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NMATH_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NMODELLOADER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NPHYSICS_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NRENDERTARGET_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef NTEXTURE_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef RENDERMANAGER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef OMSPLAYER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef PATHREFINER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef RESLOADER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef RMML_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef RTL_BUILD
	#define MEMORY_PROFILING	0
#endif

#ifdef SERVICEMAN_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef SOUNDMAN_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef SYNCMAN_BUILD
	#define MEMORY_PROFILING
#endif


#ifdef AUDIOCAPTURE_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef VOIPMAN_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef VASENCODER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef UPDATERDLL_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef VCSETTINGS_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef WEBCAMGRABBER_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef JS_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef FTGL_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef VOIPMANCLIENT_BUILD
	#define MEMORY_PROFILING
#endif

#ifdef VOIPMANSERVER_BUILD
	#define MEMORY_PROFILING
#endif

// Глобальные настройки
#ifdef	NO_PROFILING
	#undef MEMORY_PROFILING
#endif