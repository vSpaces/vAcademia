// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $Id: oms.h,v 1.2 2004/09/15 10:58:07 Alex Exp $ 
// $Locker:  $
//
// last change: $Author: Alex $ $Date: 2004/09/15 10:58:07 $
//
// Copyright (C) 2004 RNMC
//
// Этот файл является частью ОМС (Открытая мультимедиа система)
//
//////////////////////////////////////////////////////////////////////

#ifndef __OMS_Include__
#define __OMS_Include__

#include "rtl/oms_smartptr.h"

#ifdef OMS_DEBUG
# pragma comment(lib, "oms_rtl_dbg.lib")
#else
# pragma comment(lib, "oms_rtl.lib")
#endif

namespace oms {
  class OMSApplication;

  bool oms_init(shared_ptr<OMSApplication> smpApp);
}


#endif // #ifndef __OMS_Include__