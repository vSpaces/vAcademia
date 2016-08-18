//----------------------------------------------------------------------------//
// StdAfx.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef STDAFX_H
#define STDAFX_H


// Windows Header Files:
#include <windows.h>
#include <atlbase.h>

#include "std_define.h"
#include "..\rtl\rtl.h"

#define CAL3D_BUILD
#include "..\common\memprofiling.h"
#undef CAL3D_BUILD

//----------------------------------------------------------------------------//
// Defines                                                                    //
//----------------------------------------------------------------------------//

#ifdef _WIN32
#define VC_EXTRALEAN
#pragma warning(disable : 4786)
#endif

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// MSVC++ stuff                                                               //
//----------------------------------------------------------------------------//

//{{AFX_INSERT_LOCATION}}

#endif

//----------------------------------------------------------------------------//
