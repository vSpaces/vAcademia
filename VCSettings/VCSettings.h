

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Dec 09 09:50:14 2008
 */
/* Compiler settings for \Users\Hep\programming\Vu\Common\ComManIntr.idl:
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

#ifndef __VCSettings_h__
#define __VCSettings_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDataHandler_FWD_DEFINED__
#define __IDataHandler_FWD_DEFINED__
typedef interface IDataHandler IDataHandler;
#endif 	/* __IDataHandler_FWD_DEFINED__ */


#ifndef __IAsyncConnect_FWD_DEFINED__
#define __IAsyncConnect_FWD_DEFINED__
typedef interface IAsyncConnect IAsyncConnect;
#endif 	/* __IAsyncConnect_FWD_DEFINED__ */


#ifndef __IComMan_FWD_DEFINED__
#define __IComMan_FWD_DEFINED__
typedef interface IComMan IComMan;
#endif 	/* __IComMan_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IDataHandler_INTERFACE_DEFINED__
#define __IDataHandler_INTERFACE_DEFINED__

/* interface IDataHandler */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDataHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E70ACAEF-7324-41EE-8530-F1671F64DA25")
    IDataHandler : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE handleData( 
            /* [in] */ DWORD aDataSize,
            /* [size_is][in] */ BYTE *aData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDataHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDataHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDataHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDataHandler * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *handleData )( 
            IDataHandler * This,
            /* [in] */ DWORD aDataSize,
            /* [size_is][in] */ BYTE *aData);
        
        END_INTERFACE
    } IDataHandlerVtbl;

    interface IDataHandler
    {
        CONST_VTBL struct IDataHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDataHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDataHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDataHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDataHandler_handleData(This,aDataSize,aData)	\
    (This)->lpVtbl -> handleData(This,aDataSize,aData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDataHandler_handleData_Proxy( 
    IDataHandler * This,
    /* [in] */ DWORD aDataSize,
    /* [size_is][in] */ BYTE *aData);


void __RPC_STUB IDataHandler_handleData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDataHandler_INTERFACE_DEFINED__ */


#ifndef __IAsyncConnect_INTERFACE_DEFINED__
#define __IAsyncConnect_INTERFACE_DEFINED__

/* interface IAsyncConnect */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAsyncConnect;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("90FA9EAA-4DF7-4f71-B3E2-49EBB0D6AC55")
    IAsyncConnect : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE sendData( 
            /* [in] */ DWORD aDataSize,
            /* [size_is][in] */ BYTE *aData) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setDataHandler( 
            /* [in] */ IUnknown *aDataHandler) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getConnectName( 
            /* [out] */ BSTR *aName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAsyncConnectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAsyncConnect * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAsyncConnect * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAsyncConnect * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *sendData )( 
            IAsyncConnect * This,
            /* [in] */ DWORD aDataSize,
            /* [size_is][in] */ BYTE *aData);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setDataHandler )( 
            IAsyncConnect * This,
            /* [in] */ IUnknown *aDataHandler);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getConnectName )( 
            IAsyncConnect * This,
            /* [out] */ BSTR *aName);
        
        END_INTERFACE
    } IAsyncConnectVtbl;

    interface IAsyncConnect
    {
        CONST_VTBL struct IAsyncConnectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAsyncConnect_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAsyncConnect_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAsyncConnect_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAsyncConnect_sendData(This,aDataSize,aData)	\
    (This)->lpVtbl -> sendData(This,aDataSize,aData)

#define IAsyncConnect_setDataHandler(This,aDataHandler)	\
    (This)->lpVtbl -> setDataHandler(This,aDataHandler)

#define IAsyncConnect_getConnectName(This,aName)	\
    (This)->lpVtbl -> getConnectName(This,aName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAsyncConnect_sendData_Proxy( 
    IAsyncConnect * This,
    /* [in] */ DWORD aDataSize,
    /* [size_is][in] */ BYTE *aData);


void __RPC_STUB IAsyncConnect_sendData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAsyncConnect_setDataHandler_Proxy( 
    IAsyncConnect * This,
    /* [in] */ IUnknown *aDataHandler);


void __RPC_STUB IAsyncConnect_setDataHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAsyncConnect_getConnectName_Proxy( 
    IAsyncConnect * This,
    /* [out] */ BSTR *aName);


void __RPC_STUB IAsyncConnect_getConnectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAsyncConnect_INTERFACE_DEFINED__ */


#ifndef __IComMan_INTERFACE_DEFINED__
#define __IComMan_INTERFACE_DEFINED__

/* interface IComMan */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IComMan;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2D6E6F47-71A2-45EC-81D1-58B90004B17F")
    IComMan : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE sendQuery( 
            /* [in] */ BSTR aQuery,
            /* [in] */ BSTR aParams,
            /* [out] */ VARIANT *aResult) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE sendQueryPost( 
            /* [in] */ BSTR aQuery,
            /* [in] */ BSTR aParams,
            /* [out] */ VARIANT *aResult) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE sendQueryData( 
            /* [in] */ BSTR aQuery,
            /* [in] */ BSTR aParams,
            /* [out][in] */ ULONG *aDataSize,
            /* [size_is][out] */ BYTE **aData) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE openConnect( 
            /* [in] */ BSTR aConnectName,
            /* [in] */ IUnknown *aHandler,
            /* [out] */ IAsyncConnect **aConnect) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE closeConnect( 
            /* [out][in] */ IAsyncConnect **aConnect) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getSessionID( 
            /* [out] */ long *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setSessionID( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getServer( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setServer( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getQueryBase( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setQueryBase( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE logQueries( 
            /* [in] */ BSTR aLogID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getProxy( 
            /* [out] */ BYTE *aProxyUsing,
            /* [out] */ BSTR *aProxy,
            /* [out] */ BSTR *aProxyUser,
            /* [out] */ BSTR *aProxyPassword) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setProxy( 
            /* [in] */ BYTE aProxyUsing,
            /* [in] */ BSTR aProxy,
            /* [in] */ BSTR aProxyUser,
            /* [in] */ BSTR aProxyPassword
			/* [in] */ cmProxyType aProxyType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE login( 
            /* [in] */ BSTR aName,
            /* [in] */ BSTR aPassword,
            /* [out] */ int *aError) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setLoginData( 
            /* [in] */ DWORD aDataSize,
            /* [size_is][in] */ BYTE *aData,
            /* [out] */ int *aError) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE createMapServerSession( 
            /* [out][in] */ ULONG **ppClientSession) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE createSyncServerSession( 
            /* [out][in] */ ULONG **ppClientSession) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE destroyServerSession( 
            /* [out][in] */ ULONG **ppClientSession) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE createIComManager( 
            /* [in] */ ULONG *apContex) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getMaxCacheSize( 
            /* [out] */ DWORD *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setMaxCacheSize( 
            /* [in] */ DWORD newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getCacheBase( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setCacheBase( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getClientStringID( 
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getConnectionStatus( 
            /* [out] */ ULONG *pStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE createIResManager( 
            /* [in] */ ULONG *apContex) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComManVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IComMan * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IComMan * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IComMan * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *sendQuery )( 
            IComMan * This,
            /* [in] */ BSTR aQuery,
            /* [in] */ BSTR aParams,
            /* [out] */ VARIANT *aResult);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *sendQueryPost )( 
            IComMan * This,
            /* [in] */ BSTR aQuery,
            /* [in] */ BSTR aParams,
            /* [out] */ VARIANT *aResult);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *sendQueryData )( 
            IComMan * This,
            /* [in] */ BSTR aQuery,
            /* [in] */ BSTR aParams,
            /* [out][in] */ ULONG *aDataSize,
            /* [size_is][out] */ BYTE **aData);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *openConnect )( 
            IComMan * This,
            /* [in] */ BSTR aConnectName,
            /* [in] */ IUnknown *aHandler,
            /* [out] */ IAsyncConnect **aConnect);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *closeConnect )( 
            IComMan * This,
            /* [out][in] */ IAsyncConnect **aConnect);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getSessionID )( 
            IComMan * This,
            /* [out] */ long *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setSessionID )( 
            IComMan * This,
            /* [in] */ long newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getServer )( 
            IComMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setServer )( 
            IComMan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getQueryBase )( 
            IComMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setQueryBase )( 
            IComMan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *logQueries )( 
            IComMan * This,
            /* [in] */ BSTR aLogID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getProxy )( 
            IComMan * This,
            /* [out] */ BYTE *aProxyUsing,
            /* [out] */ BSTR *aProxy,
            /* [out] */ BSTR *aProxyUser,
            /* [out] */ BSTR *aProxyPassword);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setProxy )( 
            IComMan * This,
            /* [in] */ BYTE aProxyUsing,
            /* [in] */ BSTR aProxy,
            /* [in] */ BSTR aProxyUser,
            /* [in] */ BSTR aProxyPassword
			/* [in] */ cmProxyType aProxyType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *login )( 
            IComMan * This,
            /* [in] */ BSTR aName,
            /* [in] */ BSTR aPassword,
            /* [out] */ int *aError);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setLoginData )( 
            IComMan * This,
            /* [in] */ DWORD aDataSize,
            /* [size_is][in] */ BYTE *aData,
            /* [out] */ int *aError);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *createMapServerSession )( 
            IComMan * This,
            /* [out][in] */ ULONG **ppClientSession);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *createSyncServerSession )( 
            IComMan * This,
            /* [out][in] */ ULONG **ppClientSession);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *destroyServerSession )( 
            IComMan * This,
            /* [out][in] */ ULONG **ppClientSession);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *createIComManager )( 
            IComMan * This,
            /* [in] */ ULONG *apContex);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getMaxCacheSize )( 
            IComMan * This,
            /* [out] */ DWORD *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setMaxCacheSize )( 
            IComMan * This,
            /* [in] */ DWORD newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getCacheBase )( 
            IComMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setCacheBase )( 
            IComMan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getClientStringID )( 
            IComMan * This,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getConnectionStatus )( 
            IComMan * This,
            /* [out] */ ULONG *pStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *createIResManager )( 
            IComMan * This,
            /* [in] */ ULONG *apContex);
        
        END_INTERFACE
    } IComManVtbl;

    interface IComMan
    {
        CONST_VTBL struct IComManVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComMan_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComMan_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComMan_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComMan_sendQuery(This,aQuery,aParams,aResult)	\
    (This)->lpVtbl -> sendQuery(This,aQuery,aParams,aResult)

#define IComMan_sendQueryPost(This,aQuery,aParams,aResult)	\
    (This)->lpVtbl -> sendQueryPost(This,aQuery,aParams,aResult)

#define IComMan_sendQueryData(This,aQuery,aParams,aDataSize,aData)	\
    (This)->lpVtbl -> sendQueryData(This,aQuery,aParams,aDataSize,aData)

#define IComMan_openConnect(This,aConnectName,aHandler,aConnect)	\
    (This)->lpVtbl -> openConnect(This,aConnectName,aHandler,aConnect)

#define IComMan_closeConnect(This,aConnect)	\
    (This)->lpVtbl -> closeConnect(This,aConnect)

#define IComMan_getSessionID(This,pVal)	\
    (This)->lpVtbl -> getSessionID(This,pVal)

#define IComMan_setSessionID(This,newVal)	\
    (This)->lpVtbl -> setSessionID(This,newVal)

#define IComMan_getServer(This,pVal)	\
    (This)->lpVtbl -> getServer(This,pVal)

#define IComMan_setServer(This,newVal)	\
    (This)->lpVtbl -> setServer(This,newVal)

#define IComMan_getQueryBase(This,pVal)	\
    (This)->lpVtbl -> getQueryBase(This,pVal)

#define IComMan_setQueryBase(This,newVal)	\
    (This)->lpVtbl -> setQueryBase(This,newVal)

#define IComMan_logQueries(This,aLogID)	\
    (This)->lpVtbl -> logQueries(This,aLogID)

#define IComMan_getProxy(This,aProxyUsing,aProxy,aProxyUser,aProxyPassword)	\
    (This)->lpVtbl -> getProxy(This,aProxyUsing,aProxy,aProxyUser,aProxyPassword)

#define IComMan_setProxy(This,aProxyUsing,aProxy,aProxyUser,aProxyPassword,aProxyType)	\
    (This)->lpVtbl -> setProxy(This,aProxyUsing,aProxy,aProxyUser,aProxyPassword,aProxyType)

#define IComMan_login(This,aName,aPassword,aError)	\
    (This)->lpVtbl -> login(This,aName,aPassword,aError)

#define IComMan_setLoginData(This,aDataSize,aData,aError)	\
    (This)->lpVtbl -> setLoginData(This,aDataSize,aData,aError)

#define IComMan_createMapServerSession(This,ppClientSession)	\
    (This)->lpVtbl -> createMapServerSession(This,ppClientSession)

#define IComMan_createSyncServerSession(This,ppClientSession)	\
    (This)->lpVtbl -> createSyncServerSession(This,ppClientSession)

#define IComMan_destroyServerSession(This,ppClientSession)	\
    (This)->lpVtbl -> destroyServerSession(This,ppClientSession)

#define IComMan_createIComManager(This,apContex)	\
    (This)->lpVtbl -> createIComManager(This,apContex)

#define IComMan_getMaxCacheSize(This,pVal)	\
    (This)->lpVtbl -> getMaxCacheSize(This,pVal)

#define IComMan_setMaxCacheSize(This,newVal)	\
    (This)->lpVtbl -> setMaxCacheSize(This,newVal)

#define IComMan_getCacheBase(This,pVal)	\
    (This)->lpVtbl -> getCacheBase(This,pVal)

#define IComMan_setCacheBase(This,newVal)	\
    (This)->lpVtbl -> setCacheBase(This,newVal)

#define IComMan_getClientStringID(This,pVal)	\
    (This)->lpVtbl -> getClientStringID(This,pVal)

#define IComMan_getConnectionStatus(This,pStatus)	\
    (This)->lpVtbl -> getConnectionStatus(This,pStatus)

#define IComMan_createIResManager(This,apContex)	\
    (This)->lpVtbl -> createIResManager(This,apContex)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_sendQuery_Proxy( 
    IComMan * This,
    /* [in] */ BSTR aQuery,
    /* [in] */ BSTR aParams,
    /* [out] */ VARIANT *aResult);


void __RPC_STUB IComMan_sendQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_sendQueryPost_Proxy( 
    IComMan * This,
    /* [in] */ BSTR aQuery,
    /* [in] */ BSTR aParams,
    /* [out] */ VARIANT *aResult);


void __RPC_STUB IComMan_sendQueryPost_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_sendQueryData_Proxy( 
    IComMan * This,
    /* [in] */ BSTR aQuery,
    /* [in] */ BSTR aParams,
    /* [out][in] */ ULONG *aDataSize,
    /* [size_is][out] */ BYTE **aData);


void __RPC_STUB IComMan_sendQueryData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_openConnect_Proxy( 
    IComMan * This,
    /* [in] */ BSTR aConnectName,
    /* [in] */ IUnknown *aHandler,
    /* [out] */ IAsyncConnect **aConnect);


void __RPC_STUB IComMan_openConnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_closeConnect_Proxy( 
    IComMan * This,
    /* [out][in] */ IAsyncConnect **aConnect);


void __RPC_STUB IComMan_closeConnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_getSessionID_Proxy( 
    IComMan * This,
    /* [out] */ long *pVal);


void __RPC_STUB IComMan_getSessionID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_setSessionID_Proxy( 
    IComMan * This,
    /* [in] */ long newVal);


void __RPC_STUB IComMan_setSessionID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_getServer_Proxy( 
    IComMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IComMan_getServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_setServer_Proxy( 
    IComMan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IComMan_setServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_getQueryBase_Proxy( 
    IComMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IComMan_getQueryBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_setQueryBase_Proxy( 
    IComMan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IComMan_setQueryBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_logQueries_Proxy( 
    IComMan * This,
    /* [in] */ BSTR aLogID);


void __RPC_STUB IComMan_logQueries_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_getProxy_Proxy( 
    IComMan * This,
    /* [out] */ BYTE *aProxyUsing,
    /* [out] */ BSTR *aProxy,
    /* [out] */ BSTR *aProxyUser,
    /* [out] */ BSTR *aProxyPassword);


void __RPC_STUB IComMan_getProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_setProxy_Proxy( 
    IComMan * This,
    /* [in] */ BYTE aProxyUsing,
    /* [in] */ BSTR aProxy,
    /* [in] */ BSTR aProxyUser,
    /* [in] */ BSTR aProxyPassword,
	/* [in] */ cmProxyType aProxyType);


void __RPC_STUB IComMan_setProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase
	);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_login_Proxy( 
    IComMan * This,
    /* [in] */ BSTR aName,
    /* [in] */ BSTR aPassword,
    /* [out] */ int *aError);


void __RPC_STUB IComMan_login_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_setLoginData_Proxy( 
    IComMan * This,
    /* [in] */ DWORD aDataSize,
    /* [size_is][in] */ BYTE *aData,
    /* [out] */ int *aError);


void __RPC_STUB IComMan_setLoginData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_createMapServerSession_Proxy( 
    IComMan * This,
    /* [out][in] */ ULONG **ppClientSession);


void __RPC_STUB IComMan_createMapServerSession_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_createSyncServerSession_Proxy( 
    IComMan * This,
    /* [out][in] */ ULONG **ppClientSession);


void __RPC_STUB IComMan_createSyncServerSession_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_destroyServerSession_Proxy( 
    IComMan * This,
    /* [out][in] */ ULONG **ppClientSession);


void __RPC_STUB IComMan_destroyServerSession_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_createIComManager_Proxy( 
    IComMan * This,
    /* [in] */ ULONG *apContex);


void __RPC_STUB IComMan_createIComManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_getMaxCacheSize_Proxy( 
    IComMan * This,
    /* [out] */ DWORD *pVal);


void __RPC_STUB IComMan_getMaxCacheSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_setMaxCacheSize_Proxy( 
    IComMan * This,
    /* [in] */ DWORD newVal);


void __RPC_STUB IComMan_setMaxCacheSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_getCacheBase_Proxy( 
    IComMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IComMan_getCacheBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_setCacheBase_Proxy( 
    IComMan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IComMan_setCacheBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_getClientStringID_Proxy( 
    IComMan * This,
    /* [out] */ BSTR *pVal);


void __RPC_STUB IComMan_getClientStringID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_getConnectionStatus_Proxy( 
    IComMan * This,
    /* [out] */ ULONG *pStatus);


void __RPC_STUB IComMan_getConnectionStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IComMan_createIResManager_Proxy( 
    IComMan * This,
    /* [in] */ ULONG *apContex);


void __RPC_STUB IComMan_createIResManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComMan_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


