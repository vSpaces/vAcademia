

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0499 */
/* at Wed Jul 08 12:09:43 2009
 */
/* Compiler settings for .\ScreenCapture.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ScreenCapture_h__
#define __ScreenCapture_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMLScreenCap_FWD_DEFINED__
#define __IMLScreenCap_FWD_DEFINED__
typedef interface IMLScreenCap IMLScreenCap;
#endif 	/* __IMLScreenCap_FWD_DEFINED__ */


#ifndef __CoMLScreenCap_FWD_DEFINED__
#define __CoMLScreenCap_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoMLScreenCap CoMLScreenCap;
#else
typedef struct CoMLScreenCap CoMLScreenCap;
#endif /* __cplusplus */

#endif 	/* __CoMLScreenCap_FWD_DEFINED__ */


#ifndef __CoMLScreenCapFile_FWD_DEFINED__
#define __CoMLScreenCapFile_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoMLScreenCapFile CoMLScreenCapFile;
#else
typedef struct CoMLScreenCapFile CoMLScreenCapFile;
#endif /* __cplusplus */

#endif 	/* __CoMLScreenCapFile_FWD_DEFINED__ */


#ifndef __MLScreenPage_FWD_DEFINED__
#define __MLScreenPage_FWD_DEFINED__

#ifdef __cplusplus
typedef class MLScreenPage MLScreenPage;
#else
typedef struct MLScreenPage MLScreenPage;
#endif /* __cplusplus */

#endif 	/* __MLScreenPage_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "strmif.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_ScreenCapture_0000_0000 */
/* [local] */ 

typedef 
enum eSC_MouseCapture
    {	eSCMC_None	= 0,
	eSCMC_Pointer	= ( eSCMC_None + 1 ) ,
	eSCMC_Hilight	= ( eSCMC_Pointer + 1 ) 
    } 	eSC_MouseCapture;



extern RPC_IF_HANDLE __MIDL_itf_ScreenCapture_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ScreenCapture_0000_0000_v0_0_s_ifspec;

#ifndef __IMLScreenCap_INTERFACE_DEFINED__
#define __IMLScreenCap_INTERFACE_DEFINED__

/* interface IMLScreenCap */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IMLScreenCap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1F3E1C26-3939-4FFC-AA03-EEE317286726")
    IMLScreenCap : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetFrameRate( 
            /* [in] */ double _dblFPS) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetFrameRate( 
            /* [out] */ double *_pdblFPS) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetOutputSize( 
            /* [in] */ int _nWidth,
            /* [in] */ int _nHeight) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetOutputSize( 
            /* [out] */ int *_pnWidth,
            /* [out] */ int *_pnHeight) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetSourceRect( 
            /* [in] */ RECT _rcSource) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSourceRect( 
            /* [out] */ RECT *_prcSource,
            /* [in] */ BOOL _bFromHwnd) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetSourceWindow( 
            /* [in] */ LONG_PTR _hWnd) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSourceWindow( 
            /* [out] */ LONG_PTR *_phWnd) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OverlappedWindowCapture( 
            /* [in] */ BOOL _bCapture) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsOverlappedWindowCapture( 
            /* [out] */ BOOL *_pbCapture) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CaptureMousePointer( 
            /* [in] */ eSC_MouseCapture _eType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsCaptureMousePointer( 
            /* [out] */ eSC_MouseCapture *_peType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TrackActiveWindow( 
            /* [in] */ BOOL _bTrack) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsTrackActiveWindow( 
            /* [out] */ BOOL *_pbTrack) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE MaintainAspectRatio( 
            /* [in] */ BOOL _bEnable,
            /* [in] */ double _dblSetAR) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsMaintainAspectRatio( 
            /* [out] */ BOOL *_pbEnable,
            /* [out] */ double *_pdblSetAR) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnableBackground( 
            /* [in] */ BOOL _bEnable,
            /* [in] */ int _nColor) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsBackgroundEnabled( 
            /* [out] */ BOOL *_pbEnable,
            /* [out] */ int *_pnColor) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetPanAndZoom( 
            /* [in] */ int _nMsecTime) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPanAndZoom( 
            /* [out] */ int *_nMsecTime) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMLScreenCapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMLScreenCap * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMLScreenCap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMLScreenCap * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetFrameRate )( 
            IMLScreenCap * This,
            /* [in] */ double _dblFPS);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetFrameRate )( 
            IMLScreenCap * This,
            /* [out] */ double *_pdblFPS);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetOutputSize )( 
            IMLScreenCap * This,
            /* [in] */ int _nWidth,
            /* [in] */ int _nHeight);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetOutputSize )( 
            IMLScreenCap * This,
            /* [out] */ int *_pnWidth,
            /* [out] */ int *_pnHeight);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetSourceRect )( 
            IMLScreenCap * This,
            /* [in] */ RECT _rcSource);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetSourceRect )( 
            IMLScreenCap * This,
            /* [out] */ RECT *_prcSource,
            /* [in] */ BOOL _bFromHwnd);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetSourceWindow )( 
            IMLScreenCap * This,
            /* [in] */ LONG_PTR _hWnd);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetSourceWindow )( 
            IMLScreenCap * This,
            /* [out] */ LONG_PTR *_phWnd);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *OverlappedWindowCapture )( 
            IMLScreenCap * This,
            /* [in] */ BOOL _bCapture);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsOverlappedWindowCapture )( 
            IMLScreenCap * This,
            /* [out] */ BOOL *_pbCapture);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CaptureMousePointer )( 
            IMLScreenCap * This,
            /* [in] */ eSC_MouseCapture _eType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsCaptureMousePointer )( 
            IMLScreenCap * This,
            /* [out] */ eSC_MouseCapture *_peType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TrackActiveWindow )( 
            IMLScreenCap * This,
            /* [in] */ BOOL _bTrack);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsTrackActiveWindow )( 
            IMLScreenCap * This,
            /* [out] */ BOOL *_pbTrack);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MaintainAspectRatio )( 
            IMLScreenCap * This,
            /* [in] */ BOOL _bEnable,
            /* [in] */ double _dblSetAR);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsMaintainAspectRatio )( 
            IMLScreenCap * This,
            /* [out] */ BOOL *_pbEnable,
            /* [out] */ double *_pdblSetAR);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EnableBackground )( 
            IMLScreenCap * This,
            /* [in] */ BOOL _bEnable,
            /* [in] */ int _nColor);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsBackgroundEnabled )( 
            IMLScreenCap * This,
            /* [out] */ BOOL *_pbEnable,
            /* [out] */ int *_pnColor);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetPanAndZoom )( 
            IMLScreenCap * This,
            /* [in] */ int _nMsecTime);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPanAndZoom )( 
            IMLScreenCap * This,
            /* [out] */ int *_nMsecTime);
        
        END_INTERFACE
    } IMLScreenCapVtbl;

    interface IMLScreenCap
    {
        CONST_VTBL struct IMLScreenCapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMLScreenCap_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMLScreenCap_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMLScreenCap_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMLScreenCap_SetFrameRate(This,_dblFPS)	\
    ( (This)->lpVtbl -> SetFrameRate(This,_dblFPS) ) 

#define IMLScreenCap_GetFrameRate(This,_pdblFPS)	\
    ( (This)->lpVtbl -> GetFrameRate(This,_pdblFPS) ) 

#define IMLScreenCap_SetOutputSize(This,_nWidth,_nHeight)	\
    ( (This)->lpVtbl -> SetOutputSize(This,_nWidth,_nHeight) ) 

#define IMLScreenCap_GetOutputSize(This,_pnWidth,_pnHeight)	\
    ( (This)->lpVtbl -> GetOutputSize(This,_pnWidth,_pnHeight) ) 

#define IMLScreenCap_SetSourceRect(This,_rcSource)	\
    ( (This)->lpVtbl -> SetSourceRect(This,_rcSource) ) 

#define IMLScreenCap_GetSourceRect(This,_prcSource,_bFromHwnd)	\
    ( (This)->lpVtbl -> GetSourceRect(This,_prcSource,_bFromHwnd) ) 

#define IMLScreenCap_SetSourceWindow(This,_hWnd)	\
    ( (This)->lpVtbl -> SetSourceWindow(This,_hWnd) ) 

#define IMLScreenCap_GetSourceWindow(This,_phWnd)	\
    ( (This)->lpVtbl -> GetSourceWindow(This,_phWnd) ) 

#define IMLScreenCap_OverlappedWindowCapture(This,_bCapture)	\
    ( (This)->lpVtbl -> OverlappedWindowCapture(This,_bCapture) ) 

#define IMLScreenCap_IsOverlappedWindowCapture(This,_pbCapture)	\
    ( (This)->lpVtbl -> IsOverlappedWindowCapture(This,_pbCapture) ) 

#define IMLScreenCap_CaptureMousePointer(This,_eType)	\
    ( (This)->lpVtbl -> CaptureMousePointer(This,_eType) ) 

#define IMLScreenCap_IsCaptureMousePointer(This,_peType)	\
    ( (This)->lpVtbl -> IsCaptureMousePointer(This,_peType) ) 

#define IMLScreenCap_TrackActiveWindow(This,_bTrack)	\
    ( (This)->lpVtbl -> TrackActiveWindow(This,_bTrack) ) 

#define IMLScreenCap_IsTrackActiveWindow(This,_pbTrack)	\
    ( (This)->lpVtbl -> IsTrackActiveWindow(This,_pbTrack) ) 

#define IMLScreenCap_MaintainAspectRatio(This,_bEnable,_dblSetAR)	\
    ( (This)->lpVtbl -> MaintainAspectRatio(This,_bEnable,_dblSetAR) ) 

#define IMLScreenCap_IsMaintainAspectRatio(This,_pbEnable,_pdblSetAR)	\
    ( (This)->lpVtbl -> IsMaintainAspectRatio(This,_pbEnable,_pdblSetAR) ) 

#define IMLScreenCap_EnableBackground(This,_bEnable,_nColor)	\
    ( (This)->lpVtbl -> EnableBackground(This,_bEnable,_nColor) ) 

#define IMLScreenCap_IsBackgroundEnabled(This,_pbEnable,_pnColor)	\
    ( (This)->lpVtbl -> IsBackgroundEnabled(This,_pbEnable,_pnColor) ) 

#define IMLScreenCap_SetPanAndZoom(This,_nMsecTime)	\
    ( (This)->lpVtbl -> SetPanAndZoom(This,_nMsecTime) ) 

#define IMLScreenCap_GetPanAndZoom(This,_nMsecTime)	\
    ( (This)->lpVtbl -> GetPanAndZoom(This,_nMsecTime) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMLScreenCap_INTERFACE_DEFINED__ */



#ifndef __MLSCREENCAPTURELib_LIBRARY_DEFINED__
#define __MLSCREENCAPTURELib_LIBRARY_DEFINED__

/* library MLSCREENCAPTURELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MLSCREENCAPTURELib;

EXTERN_C const CLSID CLSID_CoMLScreenCap;

#ifdef __cplusplus

class DECLSPEC_UUID("10360012-2216-4246-B3AF-5CB99BE58D62")
CoMLScreenCap;
#endif

EXTERN_C const CLSID CLSID_CoMLScreenCapFile;

#ifdef __cplusplus

class DECLSPEC_UUID("10360013-2216-4246-B3AF-5CB99BE58D62")
CoMLScreenCapFile;
#endif

EXTERN_C const CLSID CLSID_MLScreenPage;

#ifdef __cplusplus

class DECLSPEC_UUID("E74A095B-BDA7-4570-A4D9-AA18E8025ADE")
MLScreenPage;
#endif
#endif /* __MLSCREENCAPTURELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


