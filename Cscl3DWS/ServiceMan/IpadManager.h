#pragma once
#include "serviceman.h"
#include "Mutex.h"
using namespace rmml;

class CIPadManager : public service::IIPadManager
{
public:
	CIPadManager( omsContext *aContext);
	virtual ~CIPadManager();
	void StartSession(  wchar_t *aName, unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID, void *aImgObject);
	void DestroySession();
	/**/
	void OnIPadMessage( BYTE aType, BYTE* aData, int aDataSize);
	bool IsIPadObject( wchar_t *aName, unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID);
	void SendForceFullState();
	void SetIPadImageParam( const wchar_t *apSrc, float aProportion, float aScaleX, float aScaleY);
	void OnConnectRestored();
	void Logout();
	void UpdateIPadObjects();

protected:
	omsContext* pContext;
	unsigned int objectID;
	unsigned int bornRealityID;
	unsigned int realityID;
	bool bIPadConnected;
	int status;
	moIImage *pImgObject;
	unsigned int token;
	unsigned int packetID;
	CMutex mutex;
	CWComString src;
	float prop;
	CWComString serverIP;
	CWComString sObjName;
	float scaleFromAspectX;
	float scaleFromAspectY;

	void SetObjectParam( wchar_t *aName, unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID, moMedia *aImgObject);
	void OnIPadSessionStarted( unsigned int aToken);
	void OnIPadSessionStoped();
	void OnIPadSessionStatus( int aStatus);
	void OnIPadSessionBinState( unsigned char type, unsigned char *aData, unsigned int aDataSize);
	void IncPacketID();
	void GenerateToken();
	void SendFullState();
};

