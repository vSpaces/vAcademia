
#ifndef rmError_h__
#define rmError_h__

#include "jsapi.h"

namespace rmml{

#ifndef TRACE
void TRACE(JSContext* cx, const char* pszFormat, ...);
#endif
void mlConcat(JSContext* cx, std::string& aDest, const char* pszFormat, ...);
void mlTrace(JSContext* cx, const char* pszFormat, ...);
void mlTraceNew(JSContext* cx, const char* pszFormat, ...);
void mlTraceError(JSContext* cx, const char* pszFormat, ...);
void mlTraceWarning(JSContext* cx, const char* pszFormat, ...);
void mlSetSynchTracesEnabled( bool abEnabled, unsigned int auLevel = 0);
bool mlIsSynchTracesEnabled();

void mlTraceSyncMode3(JSContext* cx, const char* pszFormat, ...);

typedef unsigned int mlresult;
#define mlnull 0

#define ML_FAILED(_mlresult) ((_mlresult) & 0x80000000)
#define ML_SUCCEEDED(_mlresult) (!((_mlresult) & 0x80000000))

#define ML_ERROR_SEVERITY_SUCCESS       0
#define ML_ERROR_SEVERITY_ERROR         1
#define ML_ERROR_MODULE_RMML			1
#define ML_ERROR_MODULE_BASE			2
#define ML_ERROR_MODULE_NETWORK			6
#define ML_ERROR_MODULE_FILES			13
#define ML_ERROR_MODULE_BASE_OFFSET		0x45
#define ML_ERROR_GENERATE_SUCCESS(module,code) \
    ((mlresult) ((ML_ERROR_SEVERITY_SUCCESS<<31) | ((module+ML_ERROR_MODULE_BASE_OFFSET)<<16) | ((code))) )
#define ML_ERROR_GENERATE_FAILURE(module,code) \
    ((mlresult) ((ML_ERROR_SEVERITY_ERROR<<31) | ((module+ML_ERROR_MODULE_BASE_OFFSET)<<16) | ((code))) )
#define ML_FALSE	ML_ERROR_GENERATE_SUCCESS(ML_ERROR_MODULE_RMML, 0)
#define ML_TRUE		ML_ERROR_GENERATE_SUCCESS(ML_ERROR_MODULE_RMML, 1)


#define ML_OK                              0

#define ML_ERROR_BASE                      ((mlresult) 0xC1F30000)

/* Returned when an instance is not initialized */
#define ML_ERROR_NOT_INITIALIZED           (ML_ERROR_BASE + 1)

/* Returned when an instance is already initialized */
#define ML_ERROR_ALREADY_INITIALIZED       (ML_ERROR_BASE + 2)

/* Returned by a not implemented function */
#define ML_ERROR_NOT_IMPLEMENTED           ((mlresult) 0x80004001L)

/* Returned when a given interface is not supported. */
#define ML_NOINTERFACE                     ((mlresult) 0x80004002L)
#define ML_ERROR_NO_INTERFACE              ML_NOINTERFACE

#define ML_ERROR_INVALID_POINTER           ((mlresult) 0x80004003L)
#define ML_ERROR_NULL_POINTER              ML_ERROR_INVALID_POINTER

/* Returned when a function aborts */
#define ML_ERROR_ABORT                     ((mlresult) 0x80004004L)

/* Returned when a function fails */
#define ML_ERROR_FAILURE                   ((mlresult) 0x80004005L)

/* Returned when an unexpected error occurs */
#define ML_ERROR_UNEXPECTED                ((mlresult) 0x8000ffffL)

/* Returned when a memory allocation fails */
#define ML_ERROR_OUT_OF_MEMORY             ((mlresult) 0x8007000eL)

/* Returned when an illegal value is passed */
#define ML_ERROR_ILLEGAL_VALUE             ((mlresult) 0x80070057L)
#define ML_ERROR_INVALID_ARG               ML_ERROR_ILLEGAL_VALUE

/* Returned when a class doesn't allow aggregation */
#define ML_ERROR_NO_AGGREGATION            ((mlresult) 0x80040110L)

/* Returned when an operation can't complete due to an unavailable resource */
#define ML_ERROR_NOT_AVAILABLE             ((mlresult) 0x80040111L)

/* Returned when a class is not registered */
#define ML_ERROR_FACTORY_NOT_REGISTERED    ((mlresult) 0x80040154L)

/* Returned when a class cannot be registered, but may be tried again later */
#define ML_ERROR_FACTORY_REGISTER_AGAIN    ((mlresult) 0x80040155L)

/* Returned when a dynamically loaded factory couldn't be found */
#define ML_ERROR_FACTORY_NOT_LOADED        ((mlresult) 0x800401f8L)

/* Returned when a factory doesn't support signatures */
#define ML_ERROR_FACTORY_NO_SIGNATURE_SUPPORT \
                                           (ML_ERROR_BASE + 0x101)

/* Returned when a factory already is registered */
#define ML_ERROR_FACTORY_EXISTS            (ML_ERROR_BASE + 0x100)


/* For COM compatibility reasons, we want to use exact error code numbers
   for ML_ERROR_PROXY_INVALID_IN_PARAMETER and ML_ERROR_PROXY_INVALID_OUT_PARAMETER.
   The first matches:

     #define RPC_E_INVALID_PARAMETER          _HRESULT_TYPEDEF_(0x80010010L)
   
   Errors returning this mean that the xpcom proxy code could not create a proxy for
   one of the in paramaters.

   Because of this, we are ignoring the convention if using a base and offset for
   error numbers.

*/

/* Returned when a proxy could not be create a proxy for one of the IN parameters
   This is returned only when the "real" meathod has NOT been invoked. 
*/

#define ML_ERROR_PROXY_INVALID_IN_PARAMETER        ((mlresult) 0x80010010L)

/* Returned when a proxy could not be create a proxy for one of the OUT parameters
   This is returned only when the "real" meathod has ALREADY been invoked. 
*/

#define ML_ERROR_PROXY_INVALID_OUT_PARAMETER        ((mlresult) 0x80010011L)


/*@}*/

 /* I/O Errors */

 /*  Stream closed */
#define ML_BASE_STREAM_CLOSED         ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_BASE, 2)
 /*  Error from the operating system */
#define ML_BASE_STREAM_OSERROR        ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_BASE, 3)
 /*  Illegal arguments */
#define ML_BASE_STREAM_ILLEGAL_ARGS   ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_BASE, 4)
 /*  For unichar streams */
#define ML_BASE_STREAM_NO_CONVERTER   ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_BASE, 5)
 /*  For unichar streams */
#define ML_BASE_STREAM_BAD_CONVERSION ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_BASE, 6)

#define ML_BASE_STREAM_WOULD_BLOCK    ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_BASE, 7)


#define ML_ERROR_FILE_UNRECOGNIZED_PATH         ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 1)
#define ML_ERROR_FILE_UNRESOLVABLE_SYMLINK      ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 2)
#define ML_ERROR_FILE_EXECUTION_FAILED          ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 3)
#define ML_ERROR_FILE_UNKNOWN_TYPE              ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 4)
#define ML_ERROR_FILE_DESTINATION_NOT_DIR       ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 5)
#define ML_ERROR_FILE_TARGET_DOES_NOT_EXIST     ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 6)
#define ML_ERROR_FILE_COPY_OR_MOVE_FAILED       ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 7)
#define ML_ERROR_FILE_ALREADY_EXISTS            ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 8)
#define ML_ERROR_FILE_INVALID_PATH              ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 9)
#define ML_ERROR_FILE_DISK_FULL                 ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 10)
#define ML_ERROR_FILE_CORRUPTED                 ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 11)
#define ML_ERROR_FILE_NOT_DIRECTORY             ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 12)
#define ML_ERROR_FILE_IS_DIRECTORY              ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 13)
#define ML_ERROR_FILE_IS_LOCKED                 ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 14)
#define ML_ERROR_FILE_TOO_BIG                   ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 15)
#define ML_ERROR_FILE_NO_DEVICE_SPACE           ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 16)
#define ML_ERROR_FILE_NAME_TOO_LONG             ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 17)
#define ML_ERROR_FILE_NOT_FOUND                 ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 18)
#define ML_ERROR_FILE_READ_ONLY                 ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 19)
#define ML_ERROR_FILE_DIR_NOT_EMPTY             ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 20)
#define ML_ERROR_FILE_ACCESS_DENIED             ML_ERROR_GENERATE_FAILURE(ML_ERROR_MODULE_FILES, 21)

#define ML_SUCCESS_FILE_DIRECTORY_EMPTY         ML_ERROR_GENERATE_SUCCESS(ML_ERROR_MODULE_FILES, 1)

#ifdef _DEBUG

class mlDebug {
public:

  /**
   * Log a warning message to the debug log.
   */
  static void Warning(JSContext* cx, const char* aMessage,
                             const char* aFile, int aLine);

  /**
   * Abort the executing program. This works on all architectures.
   */
  static void Abort(JSContext* cx, const char* aFile, int aLine);

  /**
   * Break the executing program into the debugger. 
   */
  static void Break(JSContext* cx, const char* aFile, int aLine);

  /**
   * Log an assertion message to the debug log
   */
  static void Assertion(JSContext* cx, const char* aStr, const char* aExpr,
                               const char* aFile, int aLine);
};

#define ML_BEGIN_MACRO {
#define ML_END_MACRO }

/**
 * Abort the execution of the program if the expression evaluates to
 * false.
 *
 * There is no status value returned from the macro.
 *
 * Note that the non-debug version of this macro does <b>not</b>
 * evaluate the expression argument. Hence side effect statements
 * as arguments to the macro will yield improper execution in a
 * non-debug build. For example:
 *
 *      ML_ABORT_IF_FALSE(0 == foo++, "yikes foo should be zero");
 *
 * Note also that the non-debug version of this macro does <b>not</b>
 * evaluate the message argument.
 */
#define ML_ABORT_IF_FALSE(_expr, _msg)                        \
  ML_BEGIN_MACRO                                              \
    if (!(_expr)) {                                           \
      mlDebug::Assertion(_msg, #_expr, __FILE__, __LINE__);   \
    }                                                         \
  ML_END_MACRO

/**
 * Warn if a given condition is false.
 *
 * Program execution continues past the usage of this macro.
 *
 * Note also that the non-debug version of this macro does <b>not</b>
 * evaluate the message argument.
 */
#define ML_WARN_IF_FALSE(_expr,_msg)                          \
  ML_BEGIN_MACRO                                              \
    if (!(_expr)) {                                           \
      mlDebug::Assertion(_msg, #_expr, __FILE__, __LINE__);   \
    }                                                         \
  ML_END_MACRO

/**
 * Test a precondition for truth. If the expression is not true then
 * trigger a program failure.
 */
#define ML_PRECONDITION(cx, expr, str)                            \
  ML_BEGIN_MACRO                                              \
    if (!(expr)) {                                            \
      mlDebug::Assertion(cx, str, #expr, __FILE__, __LINE__);     \
    }                                                         \
  ML_END_MACRO

/**
 * Test an assertion for truth. If the expression is not true then
 * trigger a program failure.
 */
#define ML_ASSERTION(cx, expr, str)                               \
  ML_BEGIN_MACRO                                              \
    if (!(expr)) {                                            \
      mlDebug::Assertion(cx, str, #expr, __FILE__, __LINE__);     \
    }                                                         \
  ML_END_MACRO

/**
 * Test a post-condition for truth. If the expression is not true then
 * trigger a program failure.
 */
#define ML_POSTCONDITION(cx, expr, str)                           \
  ML_BEGIN_MACRO                                              \
    if (!(expr)) {                                            \
      mlDebug::Assertion(cx, str, #expr, __FILE__, __LINE__);     \
    }                                                         \
  ML_END_MACRO

/**
 * This macros triggers a program failure if executed. It indicates that
 * an attempt was made to execute some unimplimented functionality.
 */
#define ML_NOTYETIMPLEMENTED(cx, str)                             \
  mlDebug::Assertion(cx, str, "NotYetImplemented", __FILE__, __LINE__)

/**
 * This macros triggers a program failure if executed. It indicates that
 * an attempt was made to execute some unimplimented functionality.
 */
#define ML_NOTREACHED(cx, str)                                    \
  mlDebug::Assertion(cx, str, "Not Reached", __FILE__, __LINE__)

/**
 * Log an error message.
 */
#define ML_ERROR(cx, str)                                         \
  mlDebug::Assertion(cx, str, "Error", __FILE__, __LINE__)

/**
 * Log a warning message.
 */
#define ML_WARNING(cx, str)                                       \
  mlDebug::Warning(cx, str, __FILE__, __LINE__)

/**
 * Trigger an abort
 */
#define ML_ABORT()                                            \
  mlDebug::Abort(__FILE__, __LINE__)

/**
 * Cause a break
 */
#define ML_BREAK()                                            \
  mlDebug::Break(__FILE__, __LINE__)

#else /* ML_DEBUG */

/**
 * The non-debug version of these macros do not evaluate the
 * expression or the message arguments to the macro.
 */
#define ML_ABORT_IF_FALSE(_expr, _msg) /* nothing */
#define ML_WARN_IF_FALSE(_expr, _msg)  /* nothing */
#define ML_PRECONDITION(cx, expr, str)     /* nothing */
#define ML_ASSERTION(cx, expr, str)        /* nothing */
#define ML_POSTCONDITION(cx, expr, str)    /* nothing */
#define ML_NOTYETIMPLEMENTED(cx, str)      /* nothing */
#define ML_NOTREACHED(str)             /* nothing */
#define ML_ERROR(cx, str)                  /* nothing */
#define ML_WARNING(cx, str)                /* nothing */
#define ML_ABORT()                     /* nothing */
#define ML_BREAK()                     /* nothing */

#endif /* ! ML_DEBUG */

//static 
JSString* mlFormatJSError(JSContext *cx, const char *message, JSErrorReport *report, JSBool reportWarnings);

#ifdef _DEBUG
#define ML_ASSERT_PTR(PTR) \
	  ML_ASSERTION((PTR!=NULL),"_"); \
	  ML_ASSERTION((PTR!=(void*)0xdddddddd),"_"); \
	  ML_ASSERTION((PTR!=(void*)0xcdcdcdcd),"_"); \
	  ML_ASSERTION((PTR!=(void*)0xcccccccc),"_"); \
	  ML_ASSERTION((PTR!=(void*)0xdcdcdcdc),"_");
#else
#define ML_ASSERT_PTR(PTR) 
#endif // _DEBUG

}

#endif // rmError_h__
