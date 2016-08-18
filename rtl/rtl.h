#ifdef RTL_STATIC_LIB
#define VA_RTL_API
#else

#ifdef VA_RTL_EXPORTS
#define VA_RTL_API __declspec(dllexport)
#else
#define VA_RTL_API __declspec(dllimport)
#endif

#endif

namespace rtl{
	namespace strings{}
	namespace convert{}
	namespace memory{}
	namespace IO{}
	namespace termination{}
}

using namespace rtl::convert;
using namespace rtl::strings;
using namespace rtl::memory;
using namespace rtl::IO;
using namespace rtl::termination;

#include "rtlErrors.h"
#include "rtlConvert.h"
#include "rtlTermination.h"
#include "rtlStrings.h"
#include "rtlMemory.h"
#include "rtlHelpers.h"
#include "rtlIO.h"

#include "ComString.h"
#include "WComString.h"

#ifndef countof
#define countof(x)	(sizeof(x)/sizeof(x[0]))
#endif