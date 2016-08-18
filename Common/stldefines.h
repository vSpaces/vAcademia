#pragma once

#ifdef AUDIERE_BUILD
	#define MEMORY_PROFILING	1
#endif

#ifdef BIGFILEMAN_BUILD
	#define MEMORY_PROFILING	1
#endif

#ifdef CAL3D_BUILD
	#define MEMORY_PROFILING	1
#endif

#include "MemoryProfilerCreate.h"