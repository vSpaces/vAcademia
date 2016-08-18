/* js-config.h.  Generated automatically by configure.  */
/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sw=4 et tw=78:
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef js_config_h___
#define js_config_h___

#include "mp.h"

#include "js-confdefs.h" // added by Tandy

/* Definitions set at build time that affect SpiderMonkey's public API.
   This header file is generated by the SpiderMonkey configure script,
   and installed along with jsapi.h.  */

/* Define to 1 if SpiderMonkey should support multi-threaded clients.  */
/* #undef JS_THREADSAFE */

/* Define to 1 if SpiderMonkey should support the ability to perform
   entirely too much GC.  */
/* #undef JS_GC_ZEAL */

/* Define to 1 if the standard <stdint.h> header is present and
   useable.  See jstypes.h and jsstdint.h.  */
/* #undef JS_HAVE_STDINT_H */

/* Define to 1 if the <sys/types.h> defines int8_t, etc. */
/* #undef JS_SYS_TYPES_H_DEFINES_EXACT_SIZE_TYPES */

/* Define to 1 if the N-byte __intN types are defined by the
   compiler.  */
#define JS_HAVE___INTN 1

/* Define to 1 if #including <stddef.h> provides definitions for
   intptr_t and uintptr_t.  */
#define JS_STDDEF_H_HAS_INTPTR_T 1

/* Define to 1 if #including <crtdefs.h> provides definitions for
   intptr_t and uintptr_t.  */
/* #undef JS_CRTDEFS_H_HAS_INTPTR_T */

/* The configure script defines these if it doesn't #define
   JS_HAVE_STDINT_H.  */
/* #undef JS_INT8_TYPE */
/* #undef JS_INT16_TYPE */
/* #undef JS_INT32_TYPE */
/* #undef JS_INT64_TYPE */
/* #undef JS_INTPTR_TYPE */
/* #undef JS_BYTES_PER_WORD */

/* Some mozilla code uses JS-friend APIs that depend on JS_TRACER being
   correct. */
#define JS_TRACER 1

#endif /* js_config_h___ */
