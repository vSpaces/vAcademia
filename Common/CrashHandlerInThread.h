#pragma once

#include "std_define.h"

#include "SimpleThreadStarter.h"

class CCrashHandlerInThread  
{
public:

    // Constructor
    CCrashHandlerInThread();

    // Destructor
    virtual ~CCrashHandlerInThread();

    // Sets exception handlers that work on per-process basis
    void SetProcessExceptionHandlers();

    // Installs C++ exception handlers that function on per-thread basis
    void SetThreadExceptionHandlers();

	// Collects current process state.
    static void GetExceptionPointers(
        DWORD dwExceptionCode, 
        EXCEPTION_POINTERS** pExceptionPointers);

	// This method creates minidump of the process
	static LONG CreateMiniDump(EXCEPTION_POINTERS* pExcPtrs);

    /* Exception handler functions. */

    static LONG WINAPI SehHandler(PEXCEPTION_POINTERS pExceptionPtrs);
    static void __cdecl TerminateHandler();
    static void __cdecl UnexpectedHandler();

    static void __cdecl PureCallHandler();

    static void __cdecl InvalidParameterHandler(const wchar_t* expression, 
        const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);

    static int __cdecl NewHandler(size_t);

    static void SigabrtHandler(int);
    static void SigfpeHandler(int /*code*/, int subcode);
    static void SigintHandler(int);
    static void SigillHandler(int);
    static void SigsegvHandler(int);
    static void SigtermHandler(int);

	static void TerminateMyProcess(UINT uExitCode);
};


