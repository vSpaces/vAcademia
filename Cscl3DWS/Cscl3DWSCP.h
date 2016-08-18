 #ifndef _CSCL3DWSCP_H_
#define _CSCL3DWSCP_H_

#include "ATLCPImplMT.h"

#define PROXY_FIRE_EVENT_METHOD(NN,DSPID) \
	HRESULT Fire_##NN() \
	{ \
		CComVariant varResult; \
		T* pT = static_cast<T*>(this); \
		int nConnectionIndex; \
		int nConnections = m_vec.GetSize(); \
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++) \
		{ \
			CComPtr<IUnknown> sp; \
			sp.Attach (GetInterfaceAt(nConnectionIndex)); \
			IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p); \
			if (pDispatch != NULL) \
			{ \
				VariantClear(&varResult); \
				DISPPARAMS disp = { NULL, NULL, 0, 0 }; \
				pDispatch->Invoke(DSPID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL); \
			} \
		} \
		return varResult.scode; \
	}

template <class T>
class CProxy_IWS3DEvents : public IConnectionPointImplMT<T, &DIID__IWS3DEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
	PROXY_FIRE_EVENT_METHOD(OnLoaded,0x1)
	HRESULT Fire_OnLogEvent(BSTR name, VARIANT aEventID, VARIANT type, BSTR data)
	{
		CComVariant varResult;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		CComVariant* pvars = new CComVariant[4];
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			CComPtr<IUnknown> sp;
			sp.Attach (GetInterfaceAt(nConnectionIndex));

			IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
			if (pDispatch != NULL)
			{
				VariantClear(&varResult);
				pvars[3] = name;
				pvars[2] = aEventID;
				pvars[1] = &type;
				pvars[0] = data;
				DISPPARAMS disp = { pvars, NULL, 4, 0 };
				pDispatch->Invoke(0x2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
			}
		}
		delete[] pvars;
		return varResult.scode;
	
	}
	PROXY_FIRE_EVENT_METHOD(OnSceneLoaded,0x3)
	HRESULT Fire_OnCommand(BSTR cmd, BSTR par)
	{
		CComVariant varResult;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		CComVariant* pvars = new CComVariant[2];
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			CComPtr<IUnknown> sp;
			sp.Attach (GetInterfaceAt(nConnectionIndex));

			IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
			if (pDispatch != NULL)
			{
				VariantClear(&varResult);
				pvars[1] = cmd;
				pvars[0] = par;
				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				pDispatch->Invoke(0x4, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
			}
		}
		delete[] pvars;
		return varResult.scode;
	
	}
	PROXY_FIRE_EVENT_METHOD(OnInitialized,0x5)
	PROXY_FIRE_EVENT_METHOD(OnExecSysCommand,0x6)
	PROXY_FIRE_EVENT_METHOD(OnMinimize,0x7)
	PROXY_FIRE_EVENT_METHOD(OnExit,0x8)
	PROXY_FIRE_EVENT_METHOD(OnRepainted,0x9)
	PROXY_FIRE_EVENT_METHOD(OnLogInitialized,0x0A)

};
#endif