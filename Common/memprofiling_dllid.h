#pragma once

/////////////////////////////////////////////////////////////////////
// Настройки проектов
// ВНИМАНИЕ!!! НЕ МЕНЯТЬ НОМЕРА MP_DLL_ID, ТОЛЬКО ДОБАВЛЯТЬ НОВЫЕ
// Добавляя, не забываем добавлять в std::string CMemoryProfiler::GetDLLNameByID(int id)
/////////////////////////////////////////////////////////////////////
#ifdef AUDIERE_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		1
#endif

#ifdef BIGFILEMAN_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		2
#endif

#ifdef CAL3D_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		3
#endif

#ifdef COMMAN_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		4
#endif

#ifdef CSCL_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		5
#endif

#ifdef DESKTOPMAN_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		6
#endif

#ifdef INFOMAN_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		7
#endif

#ifdef JSONSPIRIT_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		8
#endif

#ifdef LOGGER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		9
#endif

#ifdef LOGWRITER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		10
#endif

#ifdef NAVATAR_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NANIMATION_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NCOMMON_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NENGINE_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NINFO_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NMATH_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NMODELLOADER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NPHYSICS_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NRENDERTARGET_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef NTEXTURE_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef RENDERMANAGER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		12
#endif

#ifdef OMSPLAYER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		13
#endif

#ifdef PATHREFINER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		14
#endif

#ifdef RESLOADER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		16
#endif

#ifdef RMML_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		17
#endif

#ifdef RTL_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		18
#endif

#ifdef SERVICEMAN_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		19
#endif

#ifdef SOUNDMAN_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		20
#endif

#ifdef SYNCMAN_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		21
#endif


#ifdef AUDIOCAPTURE_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		22
#endif

#ifdef VOIPMAN_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		23
#endif

#ifdef VASENCODER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		24
#endif

#ifdef UPDATERDLL_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		25
#endif

#ifdef VCSETTINGS_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		26
#endif

#ifdef WEBCAMGRABBER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		27
#endif

#ifdef JS_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		28
#endif

#ifdef FTGL_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID		32
#endif

#ifdef VOIPMANCLIENT_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID			33
#endif

#ifdef VOIPMANSERVER_BUILD
	#undef MP_DLL_ID
	#define MP_DLL_ID			34
#endif

