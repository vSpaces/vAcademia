//////////////////////////////////////////////////////////////////////

#pragma once
//их очень много при компил€ции - 4251 - warning о том, что в интерфейсе в длл зависимое от версии студии (std, stl) 
//тк все проекты сейчас под 2010 то отключаю, иначе править все экспортные классы библиотек(мен€ть в них std::string на char* и тд)
#pragma warning(disable: 4251) 

#define _HAS_ITERATOR_DEBUGGING 0
#define _ITERATOR_DEBUG_LEVEL 0
#define _SECURE_SCL		0
#define THREAD_STACK_SIZE 64000

#ifndef _WIN32_WINNT
#if _MSC_VER >= 1600
#define _WIN32_WINNT 0x0502
#else
#define _WIN32_WINNT 0x0500
#endif
#endif

