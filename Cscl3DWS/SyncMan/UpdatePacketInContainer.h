#pragma once
#include "SyncProtocol.h"
#include "PropertyInfo.h"
#include "PropertyList.h"

class CUpdatePacketInContainer : public syncIUpdatePacketIn
{
public:
	CUpdatePacketInContainer();
	virtual ~CUpdatePacketInContainer();
	void SetObjectID(unsigned int aObjID);
	void SetBornRealityID(unsigned int aBornRealityID);
	void SetVersion(unsigned short aVersion);
	void SetFlags(unsigned int aFlags);
	void SetOwnerData(wchar_t *aOwnerData, unsigned short aOwnerLength);
	void CreateList(unsigned short aPropCount);

private:
	unsigned int objID;
	unsigned int bornRealityID;
	unsigned short version;
	unsigned int flags;
	bool flagsIsSet;

	wchar_t *ownerData;
	unsigned short ownerLength;
	bool ownerIsSet;

	unsigned int iZoneID;
	bool zoneIDIsSet;

	unsigned int iRealityID;
	bool realityIDIsSet;

	bool isConnected;
	bool connectedIsSet;
	CPropertyList *propertyList;
public:
	virtual unsigned int GetObjectID();
	virtual unsigned int GetBornRealityID();
	virtual unsigned short GetVersion();
	virtual bool GetFlags(unsigned int &aValue);
	virtual bool GetOwnerLength(unsigned short &aValue);
	virtual bool GetOwnerData(wchar_t *&aValue);
	virtual bool GetZoneID(unsigned int &aiZoneID);
	virtual bool GetRealityID(unsigned int &aiRealityID);
	virtual bool GetConnected(bool &aIsConnected);
	virtual bool GetProperties(syncIPropertyList* &aPropertyList);
	virtual bool Release();                                 
};