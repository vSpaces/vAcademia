#include "stdafx.h"
#include "ReceiveSyncDataIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;


CReceiveSyncDataIn::~CReceiveSyncDataIn(){
	if(owner!=NULL){
		delete(owner);owner=NULL;
	}
	if(props==NULL)
		return;
	for (int i=0; i<propCount; i++)
	{
		delete(props[i].name); props[i].name = NULL;
		delete(props[i].value); props[i].value = NULL;
	}
	delete props; 
}


bool CReceiveSyncDataIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, iObjectID))
		return false;
	if(!memparse_object(pData, idx, idBornRealityID))
		return false;
	if(!memparse_object(pData, idx, iVersion))
		return false;
	if(!AnalyseSysProperties(idx))
		return false;
	if(!AnalyseProperties(idx))
		return false;
	return EndOfData( idx);
}

bool CReceiveSyncDataIn::AnalyseSysProperties(int &idx)
{
	flagsIsSet = false;
	ownerIsSet = false;
	zoneIDIsSet = false;
	BYTE sysPropCount = 0;
	if(!memparse_object(pData, idx, sysPropCount))
		return false;
	if(sysPropCount==0)
		return true;
	for (int i=0;i<sysPropCount;i++)
	{
		BYTE propType = 0;
		if(!memparse_object(pData, idx, propType))
			return false;
		switch (propType) 
		{
		case 0:{  // Флаги
			if(!memparse_object(pData, idx, flags))
				return false;
			flagsIsSet = true;
			break;
		}
		case 1:{ // Владелец
			if(!memparse_string(pData, idx, &owner))
				return false;
			ownerIsSet = true;
			break;
		}
		case 2:{  // Зона
			if(!memparse_object(pData, idx, iZoneID))
				return false;
			zoneIDIsSet = true;
			break;
		}
		case 3:{  // Зона
			if(!memparse_object(pData, idx, iRealityID))
				return false;
			realityIDIsSet = true;
			break;
		}
		default:
			return false;
		}
	}
	return true;
}

bool CReceiveSyncDataIn::AnalyseProperties(int &idx)
{
	propCount = 0;
	if(!memparse_object(pData, idx, propCount))
		return false;
	if(propCount==0)
		return true;
	props = new CProps[propCount];
	for (int i = 0;i<propCount;i++)
	{
		if(!memparse_string(pData, idx, &props[i].name))
			return false;
		props[i].nameLen = strlen((char*)props[i].name);
		props[i].valueLen = memparse_bytes(pData, idx, &props[i].value);
		if(!props[i].valueLen)
			return false;
	}
	return true;
}


unsigned int CReceiveSyncDataIn::GetObjectID()
{
	return iObjectID;
}

unsigned int CReceiveSyncDataIn::GetBornRealityID()
{
	return idBornRealityID;
}

unsigned short CReceiveSyncDataIn::GetVersion()
{
	return iVersion;
}

bool CReceiveSyncDataIn::GetFlags(unsigned int &aValue)
{
	if (flagsIsSet)
		aValue = flags;
	return flagsIsSet;
}

wchar_t *CReceiveSyncDataIn::GetOwner(bool &aownerIsSet)
{
	aownerIsSet = ownerIsSet;
	if (ownerIsSet){
		return owner;
	}
	return NULL;
}

unsigned int CReceiveSyncDataIn::GetZoneID(bool &azoneIDIsSet)
{
	azoneIDIsSet = zoneIDIsSet;
	if (zoneIDIsSet)
		return iZoneID;
	return 0;
}

short CReceiveSyncDataIn::GetPropertiesCount()
{
	return propCount;
}

unsigned char *CReceiveSyncDataIn::GetPropertyName( int anIndex, unsigned short *len)
{
	if(anIndex>propCount)
		return NULL;
	*len = props[anIndex].nameLen;
	return props[anIndex].name;
}

BYTE *CReceiveSyncDataIn::GetPropertyValue(int anIndex, unsigned short *len)
{
	if(anIndex>propCount)
		return NULL;
	*len = props[anIndex].valueLen;
	return props[anIndex].value;
}