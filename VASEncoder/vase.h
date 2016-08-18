#pragma once

// VASE - virtual academy sharing encoder

#ifdef VASE_EXPORTS
#define VASE_API __declspec(dllexport)
#else
#define VASE_API __declspec(dllimport)
#endif

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

// TODO: reference additional headers your program requires here
#include "VASEEncoder.h"
#include "vaswaveletencoder.h"
