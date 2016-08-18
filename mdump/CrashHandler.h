#pragma once

class CCrashHandler  
{
public:

    // Constructor
    CCrashHandler();

    // Destructor
    virtual ~CCrashHandler();

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
    static void TerminateHandler();
    static void UnexpectedHandler();

    static void PureCallHandler();

    static void InvalidParameterHandler(const wchar_t* expression, 
        const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);

    static int NewHandler(size_t);

    static void SigabrtHandler(int);
    static void SigfpeHandler(int /*code*/, int subcode);
    static void SigintHandler(int);
    static void SigillHandler(int);
    static void SigsegvHandler(int);
    static void SigtermHandler(int);

	static void TerminateMyProcess(UINT uExitCode);
};


