#pragma once

// Асинхронный task на разбор больших списков объектов с сервера объектов

#include <vector>
#include "ITask.h"
#include "ObjectsDescIn.h"
#include "IObjectReceivedHandler.h"

class CGetObjectsEndTask : public ITask
{
public:
	CGetObjectsEndTask();
	~CGetObjectsEndTask();

	void Start();
	void PrintInfo();

	void SetParams(char* unpackBuffer, unsigned long unpackedSize, unsigned int aRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, bool abFilterPacketsByReality, IObjectReceivedHandler* objectHandler, bool isStaticObjectsOnly, bool aOld_server); 

	MapManagerProtocol::CObjectsDescIn* GetObjectsPacket();

	IObjectReceivedHandler* GetReceivedHandler();

	vecServerZoneObjects GetZonesInfo();

	void Destroy() { assert(false); }
	void SelfDestroy();

	bool IsStaticObjectsOnly()
	{
		return m_isStaticObjectsOnly;
	}

	bool IsOptionalTask() { return false; }

private:
	void FreeUnpackBuffer();

	MapManagerProtocol::CObjectsDescIn* m_objectsPacket;
	unsigned int m_realityID;
	bool m_abFilterPacketsByReality;
	vecServerZoneObjects m_zonesInfo;
	char* m_unpackBuffer;
	IObjectReceivedHandler* m_objectHandler;
	bool m_isResultProcessed;
	bool m_isStaticObjectsOnly;
};