

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Jun 09 17:49:11 2009
 */
/* Compiler settings for \Users\Vladimir\Programming\vu\common\ResManIntr.idl:
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

#ifndef __ResManIntr_h__
#define __ResManIntr_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IResourceNotify_FWD_DEFINED__
#define __IResourceNotify_FWD_DEFINED__
typedef interface IResourceNotify IResourceNotify;
#endif 	/* __IResourceNotify_FWD_DEFINED__ */


#ifndef __IResource_FWD_DEFINED__
#define __IResource_FWD_DEFINED__
typedef interface IResource IResource;
#endif 	/* __IResource_FWD_DEFINED__ */


#ifndef __IResModuleMetadata_FWD_DEFINED__
#define __IResModuleMetadata_FWD_DEFINED__
typedef interface IResModuleMetadata IResModuleMetadata;
#endif 	/* __IResModuleMetadata_FWD_DEFINED__ */


#ifndef __IResModule_FWD_DEFINED__
#define __IResModule_FWD_DEFINED__
typedef interface IResModule IResModule;
#endif 	/* __IResModule_FWD_DEFINED__ */


#ifndef __IResMan_FWD_DEFINED__
#define __IResMan_FWD_DEFINED__
typedef interface IResMan IResMan;
#endif 	/* __IResMan_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_ResManIntr_0000 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_ResManIntr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ResManIntr_0000_v0_0_s_ifspec;

#ifndef __IResourceNotify_INTERFACE_DEFINED__
#define __IResourceNotify_INTERFACE_DEFINED__

/* interface IResourceNotify */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IResourceNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("90CB08CD-7156-48ca-9EA0-4375A79AF39D")
    IResourceNotify : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE onResourceNotify( 
            /* [in] */ IResource *aResource,
            /* [in] */ DWORD aNotifyCode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResourceNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IResourceNotify * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IResourceNotify * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IResourceNotify * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *onResourceNotify )( 
            IResourceNotify * This,
            /* [in] */ IResource *aResource,
            /* [in] */ DWORD aNotifyCode);
        
        END_INTERFACE
    } IResourceNotifyVtbl;

    interface IResourceNotify
    {
        CONST_VTBL struct IResourceNotifyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResourceNotify_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResourceNotify_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResourceNotify_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResourceNotify_onResourceNotify(This,aResource,aNotifyCode)	\
    (This)->lpVtbl -> onResourceNotify(This,aResource,aNotifyCode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResourceNotify_onResourceNotify_Proxy( 
    IResourceNotify * This,
    /* [in] */ IResource *aResource,
    /* [in] */ DWORD aNotifyCode);


void __RPC_STUB IResourceNotify_onResourceNotify_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResourceNotify_INTERFACE_DEFINED__ */


#ifndef __IResource_INTERFACE_DEFINED__
#define __IResource_INTERFACE_DEFINED__

/* interface IResource */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IResource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("814F5759-E271-4288-9148-F2F4A3693C04")
    IResource : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE close( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE read( 
            /* [size_is][out] */ BYTE *aBuffer,
            /* [in] */ DWORD aBufferSize,
            /* [out] */ DWORD *aReadSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE write( 
            /* [size_is][in] */ const BYTE *aBuffer,
            /* [in] */ DWORD aBufferSize,
            /* [out] */ DWORD *aWroteSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE seek( 
            /* [in] */ DWORD aPos,
            /* [in] */ BYTE aFrom) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getPos( 
            /* [out] */ DWORD *aPos) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getSize( 
            /* [out] */ DWORD *aSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getCachedSize( 
            /* [out] */ DWORD *aSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getCacheProgress( 
            /* [out] */ BYTE *aPercent) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getStatus( 
            /* [out] */ DWORD *aStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setSize( 
            /* [in] */ DWORD aSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE unpack( 
            /* [in] */ BSTR aPath,
            /* [in] */ BOOL aWait) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getUpackProgress( 
            /* [out] */ BYTE *aPercent) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getURL( 
            BSTR *a_ppURL) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE unpackToMemory( 
            /* [in] */ BSTR aFileName,
            /* [out] */ BOOL *aSuccessful,
            /* [out][in] */ DWORD *aDataSize,
            /* [size_is][out] */ BYTE **aData) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getDownloadedSize( 
            /* [out] */ DWORD *aSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE download( 
            /* [in] */ IResourceNotify *aNotify) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getDownloadProcess( 
            /* [out] */ BYTE *aPercent) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE unpack2( 
            /* [in] */ BSTR aPath,
            /* [in] */ IResourceNotify *aNotify) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE downloadToCache( 
            /* [in] */ IResourceNotify *aNotify) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE unpackToCache( 
            /* [in] */ BSTR aPath,
            /* [in] */ IResourceNotify *aNotify) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE upload( 
            /* [in] */ IResource *aSrcRes,
            /* [in] */ IResourceNotify *aNotify) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getUploadedSize( 
            /* [out] */ DWORD *aSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IResource * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IResource * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IResource * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *close )( 
            IResource * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *read )( 
            IResource * This,
            /* [size_is][out] */ BYTE *aBuffer,
            /* [in] */ DWORD aBufferSize,
            /* [out] */ DWORD *aReadSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *write )( 
            IResource * This,
            /* [size_is][in] */ const BYTE *aBuffer,
            /* [in] */ DWORD aBufferSize,
            /* [out] */ DWORD *aWroteSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *seek )( 
            IResource * This,
            /* [in] */ DWORD aPos,
            /* [in] */ BYTE aFrom);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getPos )( 
            IResource * This,
            /* [out] */ DWORD *aPos);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getSize )( 
            IResource * This,
            /* [out] */ DWORD *aSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getCachedSize )( 
            IResource * This,
            /* [out] */ DWORD *aSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getCacheProgress )( 
            IResource * This,
            /* [out] */ BYTE *aPercent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getStatus )( 
            IResource * This,
            /* [out] */ DWORD *aStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setSize )( 
            IResource * This,
            /* [in] */ DWORD aSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *unpack )( 
            IResource * This,
            /* [in] */ BSTR aPath,
            /* [in] */ BOOL aWait);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getUpackProgress )( 
            IResource * This,
            /* [out] */ BYTE *aPercent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getURL )( 
            IResource * This,
            BSTR *a_ppURL);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *unpackToMemory )( 
            IResource * This,
            /* [in] */ BSTR aFileName,
            /* [out] */ BOOL *aSuccessful,
            /* [out][in] */ DWORD *aDataSize,
            /* [size_is][out] */ BYTE **aData);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getDownloadedSize )( 
            IResource * This,
            /* [out] */ DWORD *aSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *download )( 
            IResource * This,
            /* [in] */ IResourceNotify *aNotify);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getDownloadProcess )( 
            IResource * This,
            /* [out] */ BYTE *aPercent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *unpack2 )( 
            IResource * This,
            /* [in] */ BSTR aPath,
            /* [in] */ IResourceNotify *aNotify);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *downloadToCache )( 
            IResource * This,
            /* [in] */ IResourceNotify *aNotify);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *unpackToCache )( 
            IResource * This,
            /* [in] */ BSTR aPath,
            /* [in] */ IResourceNotify *aNotify);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *upload )( 
            IResource * This,
            /* [in] */ IResource *aSrcRes,
            /* [in] */ IResourceNotify *aNotify);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getUploadedSize )( 
            IResource * This,
            /* [out] */ DWORD *aSize);
        
        END_INTERFACE
    } IResourceVtbl;

    interface IResource
    {
        CONST_VTBL struct IResourceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResource_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResource_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResource_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResource_close(This)	\
    (This)->lpVtbl -> close(This)

#define IResource_read(This,aBuffer,aBufferSize,aReadSize)	\
    (This)->lpVtbl -> read(This,aBuffer,aBufferSize,aReadSize)

#define IResource_write(This,aBuffer,aBufferSize,aWroteSize)	\
    (This)->lpVtbl -> write(This,aBuffer,aBufferSize,aWroteSize)

#define IResource_seek(This,aPos,aFrom)	\
    (This)->lpVtbl -> seek(This,aPos,aFrom)

#define IResource_getPos(This,aPos)	\
    (This)->lpVtbl -> getPos(This,aPos)

#define IResource_getSize(This,aSize)	\
    (This)->lpVtbl -> getSize(This,aSize)

#define IResource_getCachedSize(This,aSize)	\
    (This)->lpVtbl -> getCachedSize(This,aSize)

#define IResource_getCacheProgress(This,aPercent)	\
    (This)->lpVtbl -> getCacheProgress(This,aPercent)

#define IResource_getStatus(This,aStatus)	\
    (This)->lpVtbl -> getStatus(This,aStatus)

#define IResource_setSize(This,aSize)	\
    (This)->lpVtbl -> setSize(This,aSize)

#define IResource_unpack(This,aPath,aWait)	\
    (This)->lpVtbl -> unpack(This,aPath,aWait)

#define IResource_getUpackProgress(This,aPercent)	\
    (This)->lpVtbl -> getUpackProgress(This,aPercent)

#define IResource_getURL(This,a_ppURL)	\
    (This)->lpVtbl -> getURL(This,a_ppURL)

#define IResource_unpackToMemory(This,aFileName,aSuccessful,aDataSize,aData)	\
    (This)->lpVtbl -> unpackToMemory(This,aFileName,aSuccessful,aDataSize,aData)

#define IResource_getDownloadedSize(This,aSize)	\
    (This)->lpVtbl -> getDownloadedSize(This,aSize)

#define IResource_download(This,aNotify)	\
    (This)->lpVtbl -> download(This,aNotify)

#define IResource_getDownloadProcess(This,aPercent)	\
    (This)->lpVtbl -> getDownloadProcess(This,aPercent)

#define IResource_unpack2(This,aPath,aNotify)	\
    (This)->lpVtbl -> unpack2(This,aPath,aNotify)

#define IResource_downloadToCache(This,aNotify)	\
    (This)->lpVtbl -> downloadToCache(This,aNotify)

#define IResource_unpackToCache(This,aPath,aNotify)	\
    (This)->lpVtbl -> unpackToCache(This,aPath,aNotify)

#define IResource_upload(This,aSrcRes,aNotify)	\
    (This)->lpVtbl -> upload(This,aSrcRes,aNotify)

#define IResource_getUploadedSize(This,aSize)	\
    (This)->lpVtbl -> getUploadedSize(This,aSize)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_close_Proxy( 
    IResource * This);


void __RPC_STUB IResource_close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_read_Proxy( 
    IResource * This,
    /* [size_is][out] */ BYTE *aBuffer,
    /* [in] */ DWORD aBufferSize,
    /* [out] */ DWORD *aReadSize);


void __RPC_STUB IResource_read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_write_Proxy( 
    IResource * This,
    /* [size_is][in] */ const BYTE *aBuffer,
    /* [in] */ DWORD aBufferSize,
    /* [out] */ DWORD *aWroteSize);


void __RPC_STUB IResource_write_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_seek_Proxy( 
    IResource * This,
    /* [in] */ DWORD aPos,
    /* [in] */ BYTE aFrom);


void __RPC_STUB IResource_seek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getPos_Proxy( 
    IResource * This,
    /* [out] */ DWORD *aPos);


void __RPC_STUB IResource_getPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getSize_Proxy( 
    IResource * This,
    /* [out] */ DWORD *aSize);


void __RPC_STUB IResource_getSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getCachedSize_Proxy( 
    IResource * This,
    /* [out] */ DWORD *aSize);


void __RPC_STUB IResource_getCachedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getCacheProgress_Proxy( 
    IResource * This,
    /* [out] */ BYTE *aPercent);


void __RPC_STUB IResource_getCacheProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getStatus_Proxy( 
    IResource * This,
    /* [out] */ DWORD *aStatus);


void __RPC_STUB IResource_getStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_setSize_Proxy( 
    IResource * This,
    /* [in] */ DWORD aSize);


void __RPC_STUB IResource_setSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_unpack_Proxy( 
    IResource * This,
    /* [in] */ BSTR aPath,
    /* [in] */ BOOL aWait);


void __RPC_STUB IResource_unpack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getUpackProgress_Proxy( 
    IResource * This,
    /* [out] */ BYTE *aPercent);


void __RPC_STUB IResource_getUpackProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getURL_Proxy( 
    IResource * This,
    BSTR *a_ppURL);


void __RPC_STUB IResource_getURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_unpackToMemory_Proxy( 
    IResource * This,
    /* [in] */ BSTR aFileName,
    /* [out] */ BOOL *aSuccessful,
    /* [out][in] */ DWORD *aDataSize,
    /* [size_is][out] */ BYTE **aData);


void __RPC_STUB IResource_unpackToMemory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getDownloadedSize_Proxy( 
    IResource * This,
    /* [out] */ DWORD *aSize);


void __RPC_STUB IResource_getDownloadedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_download_Proxy( 
    IResource * This,
    /* [in] */ IResourceNotify *aNotify);


void __RPC_STUB IResource_download_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getDownloadProcess_Proxy( 
    IResource * This,
    /* [out] */ BYTE *aPercent);


void __RPC_STUB IResource_getDownloadProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_unpack2_Proxy( 
    IResource * This,
    /* [in] */ BSTR aPath,
    /* [in] */ IResourceNotify *aNotify);


void __RPC_STUB IResource_unpack2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_downloadToCache_Proxy( 
    IResource * This,
    /* [in] */ IResourceNotify *aNotify);


void __RPC_STUB IResource_downloadToCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_unpackToCache_Proxy( 
    IResource * This,
    /* [in] */ BSTR aPath,
    /* [in] */ IResourceNotify *aNotify);


void __RPC_STUB IResource_unpackToCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_upload_Proxy( 
    IResource * This,
    /* [in] */ IResource *aSrcRes,
    /* [in] */ IResourceNotify *aNotify);


void __RPC_STUB IResource_upload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResource_getUploadedSize_Proxy( 
    IResource * This,
    /* [out] */ DWORD *aSize);


void __RPC_STUB IResource_getUploadedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResource_INTERFACE_DEFINED__ */


#ifndef __IResModuleMetadata_INTERFACE_DEFINED__
#define __IResModuleMetadata_INTERFACE_DEFINED__

/* interface IResModuleMetadata */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IResModuleMetadata;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("039B77A5-968F-4ee4-A1C4-43E76ED465D0")
    IResModuleMetadata : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getIdentifier( 
            /* [out] */ BSTR *aID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getTitle( 
            /* [out] */ BSTR *aTitle,
            /* [out][in] */ BSTR *aLanguage) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getLanguage( 
            /* [out] */ BSTR *aLanguage) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getDescription( 
            /* [out] */ BSTR *aDescription,
            /* [out][in] */ BSTR *aLanguage) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getMetadataString( 
            /* [in] */ BSTR aXPath,
            /* [out] */ BSTR *aStr) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getTaxonomy( 
            /* [in] */ BSTR aDiscipline,
            /* [out] */ BSTR *aTaxonomy) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getRelation( 
            /* [in] */ DWORD aRelN,
            /* [out] */ BSTR *aModuleID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResModuleMetadataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IResModuleMetadata * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IResModuleMetadata * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IResModuleMetadata * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getIdentifier )( 
            IResModuleMetadata * This,
            /* [out] */ BSTR *aID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getTitle )( 
            IResModuleMetadata * This,
            /* [out] */ BSTR *aTitle,
            /* [out][in] */ BSTR *aLanguage);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getLanguage )( 
            IResModuleMetadata * This,
            /* [out] */ BSTR *aLanguage);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getDescription )( 
            IResModuleMetadata * This,
            /* [out] */ BSTR *aDescription,
            /* [out][in] */ BSTR *aLanguage);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getMetadataString )( 
            IResModuleMetadata * This,
            /* [in] */ BSTR aXPath,
            /* [out] */ BSTR *aStr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getTaxonomy )( 
            IResModuleMetadata * This,
            /* [in] */ BSTR aDiscipline,
            /* [out] */ BSTR *aTaxonomy);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getRelation )( 
            IResModuleMetadata * This,
            /* [in] */ DWORD aRelN,
            /* [out] */ BSTR *aModuleID);
        
        END_INTERFACE
    } IResModuleMetadataVtbl;

    interface IResModuleMetadata
    {
        CONST_VTBL struct IResModuleMetadataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResModuleMetadata_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResModuleMetadata_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResModuleMetadata_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResModuleMetadata_getIdentifier(This,aID)	\
    (This)->lpVtbl -> getIdentifier(This,aID)

#define IResModuleMetadata_getTitle(This,aTitle,aLanguage)	\
    (This)->lpVtbl -> getTitle(This,aTitle,aLanguage)

#define IResModuleMetadata_getLanguage(This,aLanguage)	\
    (This)->lpVtbl -> getLanguage(This,aLanguage)

#define IResModuleMetadata_getDescription(This,aDescription,aLanguage)	\
    (This)->lpVtbl -> getDescription(This,aDescription,aLanguage)

#define IResModuleMetadata_getMetadataString(This,aXPath,aStr)	\
    (This)->lpVtbl -> getMetadataString(This,aXPath,aStr)

#define IResModuleMetadata_getTaxonomy(This,aDiscipline,aTaxonomy)	\
    (This)->lpVtbl -> getTaxonomy(This,aDiscipline,aTaxonomy)

#define IResModuleMetadata_getRelation(This,aRelN,aModuleID)	\
    (This)->lpVtbl -> getRelation(This,aRelN,aModuleID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModuleMetadata_getIdentifier_Proxy( 
    IResModuleMetadata * This,
    /* [out] */ BSTR *aID);


void __RPC_STUB IResModuleMetadata_getIdentifier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModuleMetadata_getTitle_Proxy( 
    IResModuleMetadata * This,
    /* [out] */ BSTR *aTitle,
    /* [out][in] */ BSTR *aLanguage);


void __RPC_STUB IResModuleMetadata_getTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModuleMetadata_getLanguage_Proxy( 
    IResModuleMetadata * This,
    /* [out] */ BSTR *aLanguage);


void __RPC_STUB IResModuleMetadata_getLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModuleMetadata_getDescription_Proxy( 
    IResModuleMetadata * This,
    /* [out] */ BSTR *aDescription,
    /* [out][in] */ BSTR *aLanguage);


void __RPC_STUB IResModuleMetadata_getDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModuleMetadata_getMetadataString_Proxy( 
    IResModuleMetadata * This,
    /* [in] */ BSTR aXPath,
    /* [out] */ BSTR *aStr);


void __RPC_STUB IResModuleMetadata_getMetadataString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModuleMetadata_getTaxonomy_Proxy( 
    IResModuleMetadata * This,
    /* [in] */ BSTR aDiscipline,
    /* [out] */ BSTR *aTaxonomy);


void __RPC_STUB IResModuleMetadata_getTaxonomy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModuleMetadata_getRelation_Proxy( 
    IResModuleMetadata * This,
    /* [in] */ DWORD aRelN,
    /* [out] */ BSTR *aModuleID);


void __RPC_STUB IResModuleMetadata_getRelation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResModuleMetadata_INTERFACE_DEFINED__ */


#ifndef __IResModule_INTERFACE_DEFINED__
#define __IResModule_INTERFACE_DEFINED__

/* interface IResModule */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IResModule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F7846177-528F-43c6-8D15-AD2C296E05EE")
    IResModule : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE close( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE openResource( 
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [out] */ IResource **aRes) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE resourceExists( 
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [out] */ DWORD *aStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getStatus( 
            /* [out] */ DWORD *aStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getSize( 
            /* [out] */ DWORD *aSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getDownloadSize( 
            /* [out] */ DWORD *aSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE download( 
            /* [in] */ BOOL aWait) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getDownloadProcess( 
            /* [out] */ BYTE *aPercent) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE unpack( 
            /* [in] */ BOOL aWait) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getUnpackProgress( 
            /* [out] */ BYTE *aPercent) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE pack( 
            /* [in] */ BOOL aWait) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getPackProgress( 
            /* [out] */ BYTE *aPercent) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getMetadata( 
            /* [out] */ IResModuleMetadata **aMetadata) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE registerModule( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE copyResource( 
            /* [in] */ BSTR aSrcPath,
            /* [in] */ IResModule *aDestModule,
            /* [in] */ BSTR aDestPath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getFullPath( 
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [out] */ BSTR *aFullPath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE copyFile( 
            /* [in] */ BSTR aSrcPath,
            /* [in] */ BSTR aDestPath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE extract( 
            /* [in] */ BSTR aSrcPath,
            /* [in] */ BSTR aDestSubPath,
            /* [out] */ BSTR *aFullPath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE downloadMetadata( 
            BOOL aWait) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getManifestSize( 
            /* [out] */ DWORD *aSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getManifestDownloadedSize( 
            /* [out] */ DWORD *aSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE pauseDownload( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE resumeDownload( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE removeResource( 
            /* [in] */ BSTR aPath,
            /* [out] */ BOOL *aSuccess) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE moveFile( 
            /* [in] */ BSTR aSrcPath,
            /* [in] */ BSTR aDestPath,
            /* [out] */ BOOL *aSuccess) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE removeFolder( 
            /* [in] */ BSTR aPath,
            /* [in] */ BOOL aYesToAll,
            /* [out] */ BOOL *aSuccess) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResModuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IResModule * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IResModule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IResModule * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *close )( 
            IResModule * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *openResource )( 
            IResModule * This,
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [out] */ IResource **aRes);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *resourceExists )( 
            IResModule * This,
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [out] */ DWORD *aStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getStatus )( 
            IResModule * This,
            /* [out] */ DWORD *aStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getSize )( 
            IResModule * This,
            /* [out] */ DWORD *aSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getDownloadSize )( 
            IResModule * This,
            /* [out] */ DWORD *aSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *download )( 
            IResModule * This,
            /* [in] */ BOOL aWait);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getDownloadProcess )( 
            IResModule * This,
            /* [out] */ BYTE *aPercent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *unpack )( 
            IResModule * This,
            /* [in] */ BOOL aWait);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getUnpackProgress )( 
            IResModule * This,
            /* [out] */ BYTE *aPercent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *pack )( 
            IResModule * This,
            /* [in] */ BOOL aWait);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getPackProgress )( 
            IResModule * This,
            /* [out] */ BYTE *aPercent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getMetadata )( 
            IResModule * This,
            /* [out] */ IResModuleMetadata **aMetadata);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *registerModule )( 
            IResModule * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *copyResource )( 
            IResModule * This,
            /* [in] */ BSTR aSrcPath,
            /* [in] */ IResModule *aDestModule,
            /* [in] */ BSTR aDestPath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getFullPath )( 
            IResModule * This,
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [out] */ BSTR *aFullPath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *copyFile )( 
            IResModule * This,
            /* [in] */ BSTR aSrcPath,
            /* [in] */ BSTR aDestPath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *extract )( 
            IResModule * This,
            /* [in] */ BSTR aSrcPath,
            /* [in] */ BSTR aDestSubPath,
            /* [out] */ BSTR *aFullPath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *downloadMetadata )( 
            IResModule * This,
            BOOL aWait);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getManifestSize )( 
            IResModule * This,
            /* [out] */ DWORD *aSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getManifestDownloadedSize )( 
            IResModule * This,
            /* [out] */ DWORD *aSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *pauseDownload )( 
            IResModule * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *resumeDownload )( 
            IResModule * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *removeResource )( 
            IResModule * This,
            /* [in] */ BSTR aPath,
            /* [out] */ BOOL *aSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *moveFile )( 
            IResModule * This,
            /* [in] */ BSTR aSrcPath,
            /* [in] */ BSTR aDestPath,
            /* [out] */ BOOL *aSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *removeFolder )( 
            IResModule * This,
            /* [in] */ BSTR aPath,
            /* [in] */ BOOL aYesToAll,
            /* [out] */ BOOL *aSuccess);
        
        END_INTERFACE
    } IResModuleVtbl;

    interface IResModule
    {
        CONST_VTBL struct IResModuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResModule_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResModule_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResModule_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResModule_close(This)	\
    (This)->lpVtbl -> close(This)

#define IResModule_openResource(This,aResPath,aFlags,aRes)	\
    (This)->lpVtbl -> openResource(This,aResPath,aFlags,aRes)

#define IResModule_resourceExists(This,aResPath,aFlags,aStatus)	\
    (This)->lpVtbl -> resourceExists(This,aResPath,aFlags,aStatus)

#define IResModule_getStatus(This,aStatus)	\
    (This)->lpVtbl -> getStatus(This,aStatus)

#define IResModule_getSize(This,aSize)	\
    (This)->lpVtbl -> getSize(This,aSize)

#define IResModule_getDownloadSize(This,aSize)	\
    (This)->lpVtbl -> getDownloadSize(This,aSize)

#define IResModule_download(This,aWait)	\
    (This)->lpVtbl -> download(This,aWait)

#define IResModule_getDownloadProcess(This,aPercent)	\
    (This)->lpVtbl -> getDownloadProcess(This,aPercent)

#define IResModule_unpack(This,aWait)	\
    (This)->lpVtbl -> unpack(This,aWait)

#define IResModule_getUnpackProgress(This,aPercent)	\
    (This)->lpVtbl -> getUnpackProgress(This,aPercent)

#define IResModule_pack(This,aWait)	\
    (This)->lpVtbl -> pack(This,aWait)

#define IResModule_getPackProgress(This,aPercent)	\
    (This)->lpVtbl -> getPackProgress(This,aPercent)

#define IResModule_getMetadata(This,aMetadata)	\
    (This)->lpVtbl -> getMetadata(This,aMetadata)

#define IResModule_registerModule(This)	\
    (This)->lpVtbl -> registerModule(This)

#define IResModule_copyResource(This,aSrcPath,aDestModule,aDestPath)	\
    (This)->lpVtbl -> copyResource(This,aSrcPath,aDestModule,aDestPath)

#define IResModule_getFullPath(This,aResPath,aFlags,aFullPath)	\
    (This)->lpVtbl -> getFullPath(This,aResPath,aFlags,aFullPath)

#define IResModule_copyFile(This,aSrcPath,aDestPath)	\
    (This)->lpVtbl -> copyFile(This,aSrcPath,aDestPath)

#define IResModule_extract(This,aSrcPath,aDestSubPath,aFullPath)	\
    (This)->lpVtbl -> extract(This,aSrcPath,aDestSubPath,aFullPath)

#define IResModule_downloadMetadata(This,aWait)	\
    (This)->lpVtbl -> downloadMetadata(This,aWait)

#define IResModule_getManifestSize(This,aSize)	\
    (This)->lpVtbl -> getManifestSize(This,aSize)

#define IResModule_getManifestDownloadedSize(This,aSize)	\
    (This)->lpVtbl -> getManifestDownloadedSize(This,aSize)

#define IResModule_pauseDownload(This)	\
    (This)->lpVtbl -> pauseDownload(This)

#define IResModule_resumeDownload(This)	\
    (This)->lpVtbl -> resumeDownload(This)

#define IResModule_removeResource(This,aPath,aSuccess)	\
    (This)->lpVtbl -> removeResource(This,aPath,aSuccess)

#define IResModule_moveFile(This,aSrcPath,aDestPath,aSuccess)	\
    (This)->lpVtbl -> moveFile(This,aSrcPath,aDestPath,aSuccess)

#define IResModule_removeFolder(This,aPath,aYesToAll,aSuccess)	\
    (This)->lpVtbl -> removeFolder(This,aPath,aYesToAll,aSuccess)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_close_Proxy( 
    IResModule * This);


void __RPC_STUB IResModule_close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_openResource_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aResPath,
    /* [in] */ DWORD aFlags,
    /* [out] */ IResource **aRes);


void __RPC_STUB IResModule_openResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_resourceExists_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aResPath,
    /* [in] */ DWORD aFlags,
    /* [out] */ DWORD *aStatus);


void __RPC_STUB IResModule_resourceExists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getStatus_Proxy( 
    IResModule * This,
    /* [out] */ DWORD *aStatus);


void __RPC_STUB IResModule_getStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getSize_Proxy( 
    IResModule * This,
    /* [out] */ DWORD *aSize);


void __RPC_STUB IResModule_getSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getDownloadSize_Proxy( 
    IResModule * This,
    /* [out] */ DWORD *aSize);


void __RPC_STUB IResModule_getDownloadSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_download_Proxy( 
    IResModule * This,
    /* [in] */ BOOL aWait);


void __RPC_STUB IResModule_download_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getDownloadProcess_Proxy( 
    IResModule * This,
    /* [out] */ BYTE *aPercent);


void __RPC_STUB IResModule_getDownloadProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_unpack_Proxy( 
    IResModule * This,
    /* [in] */ BOOL aWait);


void __RPC_STUB IResModule_unpack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getUnpackProgress_Proxy( 
    IResModule * This,
    /* [out] */ BYTE *aPercent);


void __RPC_STUB IResModule_getUnpackProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_pack_Proxy( 
    IResModule * This,
    /* [in] */ BOOL aWait);


void __RPC_STUB IResModule_pack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getPackProgress_Proxy( 
    IResModule * This,
    /* [out] */ BYTE *aPercent);


void __RPC_STUB IResModule_getPackProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getMetadata_Proxy( 
    IResModule * This,
    /* [out] */ IResModuleMetadata **aMetadata);


void __RPC_STUB IResModule_getMetadata_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_registerModule_Proxy( 
    IResModule * This);


void __RPC_STUB IResModule_registerModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_copyResource_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aSrcPath,
    /* [in] */ IResModule *aDestModule,
    /* [in] */ BSTR aDestPath);


void __RPC_STUB IResModule_copyResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getFullPath_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aResPath,
    /* [in] */ DWORD aFlags,
    /* [out] */ BSTR *aFullPath);


void __RPC_STUB IResModule_getFullPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_copyFile_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aSrcPath,
    /* [in] */ BSTR aDestPath);


void __RPC_STUB IResModule_copyFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_extract_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aSrcPath,
    /* [in] */ BSTR aDestSubPath,
    /* [out] */ BSTR *aFullPath);


void __RPC_STUB IResModule_extract_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_downloadMetadata_Proxy( 
    IResModule * This,
    BOOL aWait);


void __RPC_STUB IResModule_downloadMetadata_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getManifestSize_Proxy( 
    IResModule * This,
    /* [out] */ DWORD *aSize);


void __RPC_STUB IResModule_getManifestSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_getManifestDownloadedSize_Proxy( 
    IResModule * This,
    /* [out] */ DWORD *aSize);


void __RPC_STUB IResModule_getManifestDownloadedSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_pauseDownload_Proxy( 
    IResModule * This);


void __RPC_STUB IResModule_pauseDownload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_resumeDownload_Proxy( 
    IResModule * This);


void __RPC_STUB IResModule_resumeDownload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_removeResource_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aPath,
    /* [out] */ BOOL *aSuccess);


void __RPC_STUB IResModule_removeResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_moveFile_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aSrcPath,
    /* [in] */ BSTR aDestPath,
    /* [out] */ BOOL *aSuccess);


void __RPC_STUB IResModule_moveFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResModule_removeFolder_Proxy( 
    IResModule * This,
    /* [in] */ BSTR aPath,
    /* [in] */ BOOL aYesToAll,
    /* [out] */ BOOL *aSuccess);


void __RPC_STUB IResModule_removeFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResModule_INTERFACE_DEFINED__ */


#ifndef __IResMan_INTERFACE_DEFINED__
#define __IResMan_INTERFACE_DEFINED__

/* interface IResMan */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IResMan;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03E0B7C6-5965-49C6-A3DE-281CFD051B1F")
    IResMan : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE openResource( 
            /* [in] */ BSTR aResPath,
            /* [in] */ WORD aFlags,
            /* [out] */ IResource **aRes) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE openModule( 
            /* [in] */ BSTR aModuleID,
            /* [in] */ WORD aFlags,
            /* [out] */ IResModule **aRes) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE resourceExists( 
            /* [in] */ BSTR aResPath,
            /* [in] */ WORD aFlags,
            /* [out] */ DWORD *aStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getFullPath( 
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [out] */ BSTR *aFullPath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getRemoteBase( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setRemoteBase( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getLocalBase( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setLocalBase( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getCacheBase( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setCacheBase( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getRepositoryBase( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setRepositoryBase( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getUserBase( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setUserBase( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setupResMan( 
            /* [in] */ BSTR aRegKey) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setConnectType( 
            /* [in] */ int iType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE moduleExists( 
            /* [in] */ BSTR aModuleID,
            /* [in] */ WORD aFlags,
            /* [out] */ DWORD *aStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE writeStillOpened( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE openAsynchResource( 
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [in] */ IResourceNotify *aNotify,
            /* [out] */ IResource **aRes) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getErrorMessage( 
            /* [in] */ DWORD aError,
            /* [in] */ BSTR aLang,
            /* [out] */ BSTR *aMessage) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE importModule( 
            /* [in] */ BSTR aFileName,
            /* [out] */ DWORD *aErrors) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE openModuleByPath( 
            /* [in] */ BSTR aModulePath,
            /* [out] */ IResModule **aRes) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResManVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IResMan * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IResMan * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IResMan * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *openResource )( 
            IResMan * This,
            /* [in] */ BSTR aResPath,
            /* [in] */ WORD aFlags,
            /* [out] */ IResource **aRes);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *openModule )( 
            IResMan * This,
            /* [in] */ BSTR aModuleID,
            /* [in] */ WORD aFlags,
            /* [out] */ IResModule **aRes);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *resourceExists )( 
            IResMan * This,
            /* [in] */ BSTR aResPath,
            /* [in] */ WORD aFlags,
            /* [out] */ DWORD *aStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getFullPath )( 
            IResMan * This,
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [out] */ BSTR *aFullPath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getRemoteBase )( 
            IResMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setRemoteBase )( 
            IResMan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getLocalBase )( 
            IResMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setLocalBase )( 
            IResMan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getCacheBase )( 
            IResMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setCacheBase )( 
            IResMan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getRepositoryBase )( 
            IResMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setRepositoryBase )( 
            IResMan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getUserBase )( 
            IResMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setUserBase )( 
            IResMan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setupResMan )( 
            IResMan * This,
            /* [in] */ BSTR aRegKey);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setConnectType )( 
            IResMan * This,
            /* [in] */ int iType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *moduleExists )( 
            IResMan * This,
            /* [in] */ BSTR aModuleID,
            /* [in] */ WORD aFlags,
            /* [out] */ DWORD *aStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *writeStillOpened )( 
            IResMan * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *openAsynchResource )( 
            IResMan * This,
            /* [in] */ BSTR aResPath,
            /* [in] */ DWORD aFlags,
            /* [in] */ IResourceNotify *aNotify,
            /* [out] */ IResource **aRes);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getErrorMessage )( 
            IResMan * This,
            /* [in] */ DWORD aError,
            /* [in] */ BSTR aLang,
            /* [out] */ BSTR *aMessage);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *importModule )( 
            IResMan * This,
            /* [in] */ BSTR aFileName,
            /* [out] */ DWORD *aErrors);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *openModuleByPath )( 
            IResMan * This,
            /* [in] */ BSTR aModulePath,
            /* [out] */ IResModule **aRes);
        
        END_INTERFACE
    } IResManVtbl;

    interface IResMan
    {
        CONST_VTBL struct IResManVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResMan_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResMan_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResMan_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResMan_openResource(This,aResPath,aFlags,aRes)	\
    (This)->lpVtbl -> openResource(This,aResPath,aFlags,aRes)

#define IResMan_openModule(This,aModuleID,aFlags,aRes)	\
    (This)->lpVtbl -> openModule(This,aModuleID,aFlags,aRes)

#define IResMan_resourceExists(This,aResPath,aFlags,aStatus)	\
    (This)->lpVtbl -> resourceExists(This,aResPath,aFlags,aStatus)

#define IResMan_getFullPath(This,aResPath,aFlags,aFullPath)	\
    (This)->lpVtbl -> getFullPath(This,aResPath,aFlags,aFullPath)

#define IResMan_getRemoteBase(This,pVal)	\
    (This)->lpVtbl -> getRemoteBase(This,pVal)

#define IResMan_setRemoteBase(This,newVal)	\
    (This)->lpVtbl -> setRemoteBase(This,newVal)

#define IResMan_getLocalBase(This,pVal)	\
    (This)->lpVtbl -> getLocalBase(This,pVal)

#define IResMan_setLocalBase(This,newVal)	\
    (This)->lpVtbl -> setLocalBase(This,newVal)

#define IResMan_getCacheBase(This,pVal)	\
    (This)->lpVtbl -> getCacheBase(This,pVal)

#define IResMan_setCacheBase(This,newVal)	\
    (This)->lpVtbl -> setCacheBase(This,newVal)

#define IResMan_getRepositoryBase(This,pVal)	\
    (This)->lpVtbl -> getRepositoryBase(This,pVal)

#define IResMan_setRepositoryBase(This,newVal)	\
    (This)->lpVtbl -> setRepositoryBase(This,newVal)

#define IResMan_getUserBase(This,pVal)	\
    (This)->lpVtbl -> getUserBase(This,pVal)

#define IResMan_setUserBase(This,newVal)	\
    (This)->lpVtbl -> setUserBase(This,newVal)

#define IResMan_setupResMan(This,aRegKey)	\
    (This)->lpVtbl -> setupResMan(This,aRegKey)

#define IResMan_setConnectType(This,iType)	\
    (This)->lpVtbl -> setConnectType(This,iType)

#define IResMan_moduleExists(This,aModuleID,aFlags,aStatus)	\
    (This)->lpVtbl -> moduleExists(This,aModuleID,aFlags,aStatus)

#define IResMan_writeStillOpened(This)	\
    (This)->lpVtbl -> writeStillOpened(This)

#define IResMan_openAsynchResource(This,aResPath,aFlags,aNotify,aRes)	\
    (This)->lpVtbl -> openAsynchResource(This,aResPath,aFlags,aNotify,aRes)

#define IResMan_getErrorMessage(This,aError,aLang,aMessage)	\
    (This)->lpVtbl -> getErrorMessage(This,aError,aLang,aMessage)

#define IResMan_importModule(This,aFileName,aErrors)	\
    (This)->lpVtbl -> importModule(This,aFileName,aErrors)

#define IResMan_openModuleByPath(This,aModulePath,aRes)	\
    (This)->lpVtbl -> openModuleByPath(This,aModulePath,aRes)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_openResource_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aResPath,
    /* [in] */ WORD aFlags,
    /* [out] */ IResource **aRes);


void __RPC_STUB IResMan_openResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_openModule_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aModuleID,
    /* [in] */ WORD aFlags,
    /* [out] */ IResModule **aRes);


void __RPC_STUB IResMan_openModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_resourceExists_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aResPath,
    /* [in] */ WORD aFlags,
    /* [out] */ DWORD *aStatus);


void __RPC_STUB IResMan_resourceExists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_getFullPath_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aResPath,
    /* [in] */ DWORD aFlags,
    /* [out] */ BSTR *aFullPath);


void __RPC_STUB IResMan_getFullPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_getRemoteBase_Proxy( 
    IResMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IResMan_getRemoteBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_setRemoteBase_Proxy( 
    IResMan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IResMan_setRemoteBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_getLocalBase_Proxy( 
    IResMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IResMan_getLocalBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_setLocalBase_Proxy( 
    IResMan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IResMan_setLocalBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_getCacheBase_Proxy( 
    IResMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IResMan_getCacheBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_setCacheBase_Proxy( 
    IResMan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IResMan_setCacheBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_getRepositoryBase_Proxy( 
    IResMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IResMan_getRepositoryBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_setRepositoryBase_Proxy( 
    IResMan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IResMan_setRepositoryBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_getUserBase_Proxy( 
    IResMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IResMan_getUserBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_setUserBase_Proxy( 
    IResMan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IResMan_setUserBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_setupResMan_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aRegKey);


void __RPC_STUB IResMan_setupResMan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_setConnectType_Proxy( 
    IResMan * This,
    /* [in] */ int iType);


void __RPC_STUB IResMan_setConnectType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_moduleExists_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aModuleID,
    /* [in] */ WORD aFlags,
    /* [out] */ DWORD *aStatus);


void __RPC_STUB IResMan_moduleExists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_writeStillOpened_Proxy( 
    IResMan * This);


void __RPC_STUB IResMan_writeStillOpened_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_openAsynchResource_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aResPath,
    /* [in] */ DWORD aFlags,
    /* [in] */ IResourceNotify *aNotify,
    /* [out] */ IResource **aRes);


void __RPC_STUB IResMan_openAsynchResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_getErrorMessage_Proxy( 
    IResMan * This,
    /* [in] */ DWORD aError,
    /* [in] */ BSTR aLang,
    /* [out] */ BSTR *aMessage);


void __RPC_STUB IResMan_getErrorMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_importModule_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aFileName,
    /* [out] */ DWORD *aErrors);


void __RPC_STUB IResMan_importModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IResMan_openModuleByPath_Proxy( 
    IResMan * This,
    /* [in] */ BSTR aModulePath,
    /* [out] */ IResModule **aRes);


void __RPC_STUB IResMan_openModuleByPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResMan_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


