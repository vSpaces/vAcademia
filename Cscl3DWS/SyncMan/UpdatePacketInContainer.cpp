#include "StdAfx.h"
#include "updatepacketincontainer.h"

CUpdatePacketInContainer::CUpdatePacketInContainer()
{
	objID=0;
	bornRealityID=0;
	flags=0;
	iZoneID = 0xFFFFFFFF;
	iRealityID = 0;
	isConnected = false;
	propertyList = NULL;
}

CUpdatePacketInContainer::~CUpdatePacketInContainer()
{
}

void CUpdatePacketInContainer::SetObjectID(unsigned int aObjID)
{
	objID = aObjID;
}

void CUpdatePacketInContainer::SetBornRealityID(unsigned int aBornRealityID)
{
	bornRealityID = aBornRealityID;
}

void CUpdatePacketInContainer::SetVersion(unsigned short aVersion)
{
	version = aVersion;
}

void CUpdatePacketInContainer::SetFlags(unsigned int aFlags)
{
	flags = aFlags;
}

void CUpdatePacketInContainer::SetOwnerData(wchar_t *aOwnerData, unsigned short aOwnerLength)
{
	ownerData = aOwnerData;
	ownerLength = aOwnerLength;
}

void CUpdatePacketInContainer::CreateList(unsigned short aPropCount)
{
	MP_NEW_V( propertyList, CPropertyList, aPropCount);
}

unsigned int CUpdatePacketInContainer::GetObjectID()
{
	return objID;
}

unsigned int CUpdatePacketInContainer::GetBornRealityID()
{
	return bornRealityID;
}

unsigned short CUpdatePacketInContainer::GetVersion()
{
	return version;
}

bool CUpdatePacketInContainer::GetFlags(unsigned int &aValue)
{
	if (flagsIsSet)
		aValue = flags;
	return flagsIsSet;
}

bool CUpdatePacketInContainer::GetOwnerLength(unsigned short &aValue)
{
	if(ownerIsSet)
		aValue = ownerLength;
	return ownerIsSet;
}

bool CUpdatePacketInContainer::GetOwnerData(wchar_t *&aValue)
{
	if(ownerIsSet)
		aValue = ownerData;
	return ownerIsSet;
}

bool CUpdatePacketInContainer::GetZoneID(unsigned int &aiZoneID)
{
	if(zoneIDIsSet)
		aiZoneID = iZoneID;
	return zoneIDIsSet;
}

bool CUpdatePacketInContainer::GetRealityID(unsigned int &aiRealityID)
{
	if(realityIDIsSet)
		aiRealityID = iRealityID;
	return realityIDIsSet;
}

bool CUpdatePacketInContainer::GetConnected(bool &aIsConnected)
{
	if(connectedIsSet)
		aIsConnected = isConnected;
	return connectedIsSet;
}

bool CUpdatePacketInContainer::GetProperties(syncIPropertyList* &aPropertyList)
{
	aPropertyList = propertyList;
	return true;
}

bool CUpdatePacketInContainer::Release()
{
	MP_DELETE( propertyList);
	return true;
}

