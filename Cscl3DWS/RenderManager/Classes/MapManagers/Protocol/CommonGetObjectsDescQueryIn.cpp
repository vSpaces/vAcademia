#include "stdafx.h"
#include "CommonGetObjectsDescQueryIn.h"
#include "GlobalSingletonStorage.h"
//#include "qpsort.h"

#define SWITCH_OFF_PATHES_INTERVAL	100
#define MAP_OBJECT_PROTOCOL = 1

using namespace MapManagerProtocol;

CCommonGetObjectsDescQueryIn::CCommonGetObjectsDescQueryIn(BYTE *aData, int aDataSize, bool aOld_server) :
	CCommonPacketInForMapManager(aData, aDataSize),
	MP_VECTOR_INIT(vObjectPos),
	old_server( aOld_server),
	protocolID( 0)
{
};

bool CCommonGetObjectsDescQueryIn::GetZoneObjectInfo(short index, ZONE_OBJECT_INFO& info1, bool bGetData)
{
	USES_CONVERSION;
	if (index>-1)
	{
		if(vObjectPos.size()<=(unsigned int)index)
			return false;
		__ObjectPos objectPos = vObjectPos.at(index);
		idx = objectPos.iPosObject;		
	}
	else if (index < -1)
		return false;

	unsigned int sqID=0;
	if(!memparse_object(pData, idx, sqID, bGetData)) 
		return false;
	info1.zoneID = ZoneID(sqID);

	if(!memparse_object(pData, idx, info1.complexObjectID.bornRealityID, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.complexObjectID.objectID, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.creatingTime, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.translation.x, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.translation.y, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.translation.z, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.boundsVec.x, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.boundsVec.y, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.boundsVec.z, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.rotation.x, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.rotation.y, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.rotation.z, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.rotation.w, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.scale.x, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.scale.y, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.scale.z, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.type, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.level, bGetData)) 
		return false;

	if (info1.level >=SWITCH_OFF_PATHES_INTERVAL)
	{
		info1.isNeedToSwitchOffPathes = true;
		info1.level = info1.level%SWITCH_OFF_PATHES_INTERVAL;
	}
	else
	{
		info1.isNeedToSwitchOffPathes = false;
	}

	if(!memparse_object(pData, idx, info1.lod, bGetData)) 
		return false;
	if(!memparse_stringLarge(pData, idx, info1.resPath, bGetData)) 
		return false;
	if(!memparse_stringLarge(pData, idx, info1.className, bGetData)) 
		return false;
	if(!memparse_stringLarge(pData, idx, info1.objectName, bGetData)) 
		return false;
	std::string referenceName;
	if(!memparse_stringLarge(pData, idx, referenceName, bGetData)) 
		return false;
	if( protocolID == 0)
	{
		CComString params;
		if(!memparse_stringLarge(pData, idx, params, bGetData)) 
			return false;
		if (!params.IsEmpty())
		{
			info1.SetParams( A2W(params.GetBuffer()));
		}
	}
	else
	{
		CWComString params;
		if(!memparse_unicodeStringLarge(pData, idx, params, bGetData)) 
			return false;
		if (!params.IsEmpty())
		{
			info1.SetParams( params.GetBuffer());
		}
	}
	if(!memparse_object(pData, idx, info1.groupID, bGetData)) 
		return false;
	if(!memparse_object(pData, idx, info1.version, bGetData)) 
		return false;
	info1.isLogicalObjExists = 0;
	if(!memparse_object(pData, idx, info1.isLogicalObjExists, bGetData)) 
		return false;

	// Проверяем наличие секции синхронизируемых свойств
	unsigned char uSynchStateNext = 0;
	if(!memparse_object(pData, idx, uSynchStateNext)) 
		return false;
	if (uSynchStateNext == 0)
		return true;

	// Обрабатываем синхронизируемые свойства
	unsigned int uSynchDataSize = 0;
	memparse_object(pData, idx, uSynchDataSize);
	if (idx + uSynchDataSize > iDataSize)
		return false;

	if (bGetData)
		info1.SetSynchData( pData + idx, uSynchDataSize);
	idx += uSynchDataSize;

	return true;
}

bool CCommonGetObjectsDescQueryIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	idx = 0;

	if( !old_server)
	{
		if(!memparse_object(pData, idx, protocolID)) 
			return false;
	}

	if(!memparse_object(pData, idx, prevRealityID)) 
		return false;

	if(!memparse_object(pData, idx, currentRealityID)) 
		return false;

	if(!memparse_object(pData, idx, currentZoneID)) 
		return false;

	if(!memparse_object(pData, idx, objectCount)) 
		return false;

	for (int i = 0; i < objectCount; i++)
	{
		if(!AnalyseObject())
			return false;
	}
	if (!EndOfData(idx))
		return false;

	return true;
}

bool CCommonGetObjectsDescQueryIn::AnalyseObject()
{
	unsigned char isExist;
	if(!memparse_object(pData, idx, isExist)) 
		return false;

	int iPosObject = idx;
	__ObjectPos objectPos;
	if (isExist == 0)
	{
		unsigned int notFoundObjectID;
		if(!memparse_object(pData, idx, notFoundObjectID, false)) 
			return false;
		objectPos.bExist = false;
	}
	else
	{
		ZONE_OBJECT_INFO info1;
		if (!GetZoneObjectInfo(-1, info1, false))
			return false;
		objectPos.bExist = true;
	}
	objectPos.iPosObject = iPosObject;
	vObjectPos.push_back(objectPos); 
	return true;
}

bool CCommonGetObjectsDescQueryIn::IsObjectExist(short index, unsigned int &notFoundObjectID)
{
	__ObjectPos objectPos = vObjectPos.at(index);
	if (objectPos.bExist == 0)
	{
		memparse_object(pData, objectPos.iPosObject, notFoundObjectID); 
	}
	return objectPos.bExist;
}

unsigned int CCommonGetObjectsDescQueryIn::GetPrevRealityID()
{
	return prevRealityID;
}

unsigned int CCommonGetObjectsDescQueryIn::GetCurrentRealityID()
{
	return currentRealityID;
}

ZoneIdentifier CCommonGetObjectsDescQueryIn::GetCurrentZoneID()
{
	return currentZoneID;
}

unsigned short CCommonGetObjectsDescQueryIn::GetObjectCount(){
	return objectCount;
}
