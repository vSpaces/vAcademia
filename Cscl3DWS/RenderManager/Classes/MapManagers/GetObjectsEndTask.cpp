
#include "StdAfx.h"
#include "GetObjectsEndTask.h"
#include "GlobalSingletonStorage.h"
#include "MapManagerZone.h"

CGetObjectsEndTask::CGetObjectsEndTask():
	m_objectsPacket(NULL),
	m_unpackBuffer(NULL),
	m_objectHandler(NULL),
	MP_VECTOR_INIT(m_zonesInfo),
	m_isResultProcessed(false),
	m_isStaticObjectsOnly(false)
{
}

void CGetObjectsEndTask::PrintInfo()
{
	g->lw.WriteLn("GetObjectsEndTask ");
}

void CGetObjectsEndTask::SetParams(char* unpackBuffer, unsigned long unpackedSize,  unsigned int aRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, bool abFilterPacketsByReality, IObjectReceivedHandler* objectHandler, bool isStaticObjectsOnly, bool aOld_server)
{
	MP_NEW_V3(m_objectsPacket, MapManagerProtocol::CObjectsDescIn, (BYTE*)unpackBuffer ,unpackedSize, aOld_server);

	m_isStaticObjectsOnly = isStaticObjectsOnly;
	m_unpackBuffer = unpackBuffer;
	m_realityID = aRealityID;
	m_zonesInfo = zonesInfo;
	m_abFilterPacketsByReality = abFilterPacketsByReality;

	if (objectHandler)
		m_objectHandler = objectHandler;
}


void CGetObjectsEndTask::Start()
{
	if (m_objectsPacket == NULL)
	{
		return;
	}

	if(!m_objectsPacket->Analyse())
	{
		ATLASSERT( false);
		g->lw.WriteLn("[ANALYSE FALSE] CObjectsDescIn");
		FreeUnpackBuffer();
		return;
	}

	if (m_abFilterPacketsByReality && (unsigned int)gRM->mapMan->GetCurrentRealityID() != m_objectsPacket->GetCurrentRealityID())
	{
		FreeUnpackBuffer();
		return;
	}

	unsigned short objCount = m_objectsPacket->GetObjectCount();
	m_zonesInfo.reserve(objCount);
	for (unsigned short i = 0; i < objCount; i++)
	{
		unsigned int notFoundObjectID;
		if (!m_objectsPacket->IsObjectExist(i, notFoundObjectID))
			continue;

		ZONE_OBJECT_INFO* info1 = MP_NEW(ZONE_OBJECT_INFO);

		bool isParsedSuccessfully = m_objectsPacket->GetZoneObjectInfo(i, *info1);

		if (!isParsedSuccessfully)
		{
			MP_DELETE(info1);

			vecServerZoneObjects::iterator it = m_zonesInfo.begin();
			vecServerZoneObjects::iterator itEnd = m_zonesInfo.end();

			for ( ; it != itEnd; it++)
			{
				MP_DELETE(*it);
			}

			m_zonesInfo.clear();
			FreeUnpackBuffer();
			return;
		}

		// ? наверное уже можно проверять, является ли объект синхронизируемым
		if (info1->complexObjectID.bornRealityID > 0 && info1->type != vespace::VE_LOCATIONBOUNDS && info1->type != vespace::VE_TEMPLOCATIONBOUNDS && info1->type != vespace::VE_COMMINICATIONAREABOUNDS)
		{
			std::string suffix = StringFormat("__%d_reality__", info1->complexObjectID.bornRealityID);
			info1->objectName += suffix;			
		}
		info1->complexObjectID.realityID = m_realityID;
		info1->createImmediately = false;

		m_zonesInfo.push_back(info1);
	}

	FreeUnpackBuffer();
}

vecServerZoneObjects CGetObjectsEndTask::GetZonesInfo()
{
	m_isResultProcessed = true;
	return m_zonesInfo;
}

MapManagerProtocol::CObjectsDescIn* CGetObjectsEndTask::GetObjectsPacket()
{
	return m_objectsPacket;
}

IObjectReceivedHandler* CGetObjectsEndTask::GetReceivedHandler()
{
	return m_objectHandler;
}

void CGetObjectsEndTask::FreeUnpackBuffer()
{
	if (m_unpackBuffer)
	{
		MP_DELETE_ARR(m_unpackBuffer);
		m_unpackBuffer = NULL;
	}
}

void CGetObjectsEndTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CGetObjectsEndTask::~CGetObjectsEndTask()
{
	assert(m_isResultProcessed);

	if (!m_isResultProcessed)
	{
		vecServerZoneObjects::iterator it = m_zonesInfo.begin();
		vecServerZoneObjects::iterator itEnd = m_zonesInfo.end();

		for ( ; it != itEnd; it++)
		{
			MP_DELETE(*it);
		}
	}

	if (m_objectsPacket)
	{
		MP_DELETE(m_objectsPacket);
	}

	FreeUnpackBuffer();
}