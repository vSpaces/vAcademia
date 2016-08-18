#pragma once

struct IMoniker;

class CInputDevice
{
public:
	CInputDevice( LPCTSTR sName, IMoniker* pMoniker);
	virtual ~CInputDevice();

public:
	IMoniker *GetMoniker();
	LPCTSTR GetName();

protected:
	IMoniker *m_pMoniker;
	CComString m_sName;
};

/*
class CPin
{
public:
	CPin();
	~CPin();

	HRESULT init( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum);

	BOOL isPinByName( TCHAR *sPinName, TCHAR *sRequiredPinName);

	HRESULT make();

protected:
	IPin *m_pPin;
	IAMAudioInputMixer *m_pPinMixer;
	BOOL bOwn;
};
//*/