// MemLeaks.h
#pragma once

#ifdef DESKTOP_DETECT_MEMORY_LEAKS
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>

	#include <stdio.h>
	#include <stdlib.h>

	#ifdef _DEBUG
	#ifdef _CRTDBG_MAP_ALLOC
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#endif /* _CRTDBG_MAP_ALLOC */
	#endif /* _DEBUG */

#endif