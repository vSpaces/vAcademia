#include "StdAfx.h"
#include "UpdatePacketHandler.h"

CUpdatePacketHandler::CUpdatePacketHandler()
{
}

CUpdatePacketHandler::~CUpdatePacketHandler()
{
}

void CUpdatePacketHandler::GettingsProperties(CPropertyList *aPropertyList, unsigned int propCount)
{
	while(propCount>0 && packetIn->NextProperty())
	{
		ATLASSERT( FALSE);
		unsigned short lenName=0;
		packetIn->GetPropertyNameLength(lenName);
		wchar_t *p;
		packetIn->GetPropertyName(p);
		wchar_t *propName = new wchar_t[lenName+1];
		wcsncpy(propName, p, lenName);
		propName[lenName]='\0';
		CPropertyValue *pVal;
		packetIn->GetPropertyValue(pVal);
		CPropertyValue *pValue = new CPropertyValue();
		*pValue=*pVal;
		aPropertyList->AddProperty(propName, pValue);
		if(pValue->type==UT_OBJECT)
		{
			unsigned int propCount = packetIn->GetPropertiesCount();
			pValue->propertyList = new CPropertyList(propCount);
			GettingsProperties(pValue->propertyList, propCount);
		}
		propCount--;
	}
}

void CUpdatePacketHandler::Handle(CUpdatePacketIn *aPacketIn)
{
	packetIn = aPacketIn;
	updatePacketContainer = new CUpdatePacketInContainer();
	unsigned short ownerLength=0;
	unsigned int flags=0, iRealityID=0;
	bool isConnected = false;
	packetIn->GetFlags(flags);
	packetIn->GetOwnerLength(ownerLength);
	wchar_t *p, *ownerData = new wchar_t[ownerLength];
	packetIn->GetOwnerData(p);
	wcsncpy(ownerData, p, ownerLength);
	packetIn->GetRealityID(iRealityID);
	packetIn->GetConnected(isConnected);
	updatePacketContainer->SetObjectID(packetIn->GetObjectID());
	updatePacketContainer->SetBornRealityID(packetIn->GetBornRealityID());
	updatePacketContainer->SetVersion(packetIn->GetVersion());
	updatePacketContainer->SetFlags(flags);
	updatePacketContainer->SetOwnerData(ownerData, ownerLength);
	packetIn->ResetToBeginReadingProperty();
	unsigned int propCount = packetIn->GetPropertiesCount();
	updatePacketContainer->CreateList(propCount);
	CPropertyList *propertyList;
	updatePacketContainer->GetProperties((syncIPropertyList*&)propertyList);
	GettingsProperties(propertyList, propCount);
}

bool CUpdatePacketHandler::GetContainer(syncIUpdatePacketIn* &aUpdatePacketContainer)
{
	aUpdatePacketContainer = updatePacketContainer;
	return true;
}