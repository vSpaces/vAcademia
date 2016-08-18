/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Jul 20 12:57:29 2004
 */
/* Compiler settings for C:\user\alex\programming\RM\Common\LogEventIntr.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __LogEventIntr_h__
#define __LogEventIntr_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_LogEventIntr_0000 */
/* [local] */ 

#pragma pack(1)
typedef struct  CLogEvent
    {
    BYTE btChannel;
    DWORD dwChannelSq;
    WORD wEvent;
    WORD wDataSize;
    /* [size_is] */ BYTE btData[ 1 ];
    }	CLogEvent;

#pragma pack()


extern RPC_IF_HANDLE __MIDL_itf_LogEventIntr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_LogEventIntr_0000_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
