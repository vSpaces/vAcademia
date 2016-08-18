// ****************************************************************
// This is free software licensed under the NUnit license. You
// may obtain a copy of the license as well as information regarding
// copyright ownership at http://nunit.org/?p=license&r=2.4.
// ****************************************************************

#include "StdAfx.h"
#include "UpdatePacketHandlerTest.h"
#include "UpdatePacketHandler.h"

wchar_t* MB2WC(const char* apch){
	int iSLen=strlen(apch);
	wchar_t* wsAttrValue=new wchar_t[iSLen+1];
	mbstowcs(wsAttrValue, apch, iSLen);
	wsAttrValue[iSLen]=0;
	return wsAttrValue;
}

namespace SyncManProtocol 
{

	unsigned int idObj = 1000;
	unsigned int bornRealityID =444;
	unsigned short iVersion = 1;
	wchar_t *sOwnerValue = MB2WC("hep");
	unsigned int iFlags = 12345;

	const wchar_t *sPropNameDeep = MB2WC("hep777");
	int iPropValue = 10;
	bool bPropValue = true;
	double dPropValue = 10.10;
	wchar_t *sPropValue = MB2WC("hep_prop_value");
	int deepMax=1;
	CUpdatePacketHandler *handler;

	void UpdatePacketHandlerTest::AddPropertyObject(CUpdatePacketOut *packetOut, const wchar_t *sPropName, int deepMax)
	{
		packetOut->CreateObject(sPropName);
		if(deepMax>0)
			AddPropertyObject(packetOut, sPropName, deepMax-1);
		else
			packetOut->AddPropertyBool(sPropName, bPropValue);
		packetOut->AddPropertyDouble(sPropName, dPropValue);
		packetOut->CloseObject();
	}

	void UpdatePacketHandlerTest::Init() 
	{
		
	}

	void UpdatePacketHandlerTest::GetDeepProperties(syncIPropertyList *aPropertyList)
	{
		
		if(aPropertyList==NULL)
			Assert::IsTrue(false, "Список пустой");
		unsigned int count = aPropertyList->GetCount();
		for(int i=0;i<count;i++)
		{
			syncIPropertyInfo *propInfo = aPropertyList->GetProperty(i);
			if(propInfo==NULL)
				Assert::IsTrue(false, "Cвойство пустое");
			wchar_t *sName2;
			unsigned short sNameLen=0;
			propInfo->GetNameLength(sNameLen);
			sName2 = new wchar_t[sNameLen];
			wchar_t *p;
			propInfo->GetNameData(p);
			wcscpy(sName2, p);
			Assert::AreEqual(0, wcscmp(sPropNameDeep, sName2), "Имя свойства неверно");
			unsigned char type;
			propInfo->GetType(type);
			switch (type)
			{
				case 0:
				{
					bool bPropValue2;
					propInfo->GetValueBool(bPropValue2);
					Assert::AreEqual(bPropValue, bPropValue2, "Значение булевого свойства неверно");
					break;
				}
				case 1:
				{
					int iPropValue2;
					propInfo->GetValueInt(iPropValue2);
					Assert::AreEqual(iPropValue, iPropValue2, "Значение целого свойства неверно");
					break;
				}
				case 2:
				{
					double dPropValue2;
					propInfo->GetValueDouble(dPropValue2);
					Assert::LessOrEqual(dPropValue, dPropValue2, "Значение рационального свойства неверно");
					Assert::GreaterOrEqual(dPropValue, dPropValue2, "Значение рационального свойства неверно");
					break;
				}
				case 3:
				{
					wchar_t *sPropValue2;
					unsigned short sPropValue2Len=0;
					propInfo->GetValueStringLength(sPropValue2Len);
					sPropValue2 = new wchar_t[sPropValue2Len];
					wchar_t *p;
					propInfo->GetValueString(p);
					wcscpy(sPropValue2, p);
					//Assert::AreSame(sPropValue, sPropValue2, "Значение строкового свойства неверно");
					Assert::AreEqual(0, wcscmp(sPropValue, sPropValue2), "Значение строкового свойства неверно");
					break;
				}
				case 4:
				{
					syncIPropertyList *propertyList;
					if(propInfo->GetValueObject(propertyList))
						GetDeepProperties(propertyList);
				}
			}
		}
	}
	
	void UpdatePacketHandlerTest::Test() 
	{
		CUpdatePacketOut *packetOut = new CUpdatePacketOut(iVersion, idObj, bornRealityID);
		packetOut->BeginSysProperty();
		packetOut->AddOwner(sOwnerValue);
		packetOut->AddFlags(iFlags);
		packetOut->EndSysProperty();
		packetOut->BeginProperties();
		packetOut->AddPropertyString(sPropNameDeep, wcslen(sPropValue), sPropValue);
		AddPropertyObject(packetOut, sPropNameDeep, deepMax-1);
		packetOut->AddPropertyInt(sPropNameDeep, iPropValue);
		packetOut->EndProperty();
		CUpdatePacketIn *packetIn = new CUpdatePacketIn(packetOut->GetData(), packetOut->GetDataSize());
		packetIn->Analyse();
		handler = new CUpdatePacketHandler();
		handler->Handle(packetIn);
		syncIUpdatePacketIn *updatePacketContainer;
		handler->GetContainer(updatePacketContainer);
		unsigned int idObj2 = updatePacketContainer->GetObjectID();
		Assert::AreEqual(idObj, idObj2, "ID объекта неверный");
		unsigned int  bornRealityID2 = updatePacketContainer->GetBornRealityID();
		Assert::AreEqual(bornRealityID, bornRealityID2, "ID реальности неверный");
		unsigned short iVersion2 = updatePacketContainer->GetVersion();
		Assert::AreEqual(iVersion, iVersion2, "Версия неверна");
		unsigned int iFlags2=0;
		updatePacketContainer->GetFlags(iFlags2);
		Assert::AreEqual(iFlags, iFlags2, "Флаги неверный");
		wchar_t *sOwnerValue2;
		unsigned short sOwnerLen=0;
		updatePacketContainer->GetOwnerLength(sOwnerLen);
		sOwnerValue2 = new wchar_t[sOwnerLen+1];
		wchar_t *p;
		updatePacketContainer->GetOwnerData(p);
		wcscpy(sOwnerValue2, p);
		sOwnerValue2[sOwnerLen]='\0';
		Assert::AreEqual(0, wcscmp(sOwnerValue, sOwnerValue2), "Владелец неверный");

		syncIPropertyList *propertyList;
		updatePacketContainer->GetProperties(propertyList);
		GetDeepProperties(propertyList);
		delete packetOut;
		delete packetIn;
		updatePacketContainer->Release();
	}
}

