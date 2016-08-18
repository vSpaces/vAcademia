#include "mlRMML.h"	// for precompile headers
#include "ScriptedObjectManager.h"
#include <string>

#ifndef RMML_TESTS
	//#include "mlRMML.h"
#else
	#include "mlRMML-fake.h"
#endif

namespace rmml {

CScriptedObjectManager::CScriptedObjectManager(void):
	MP_MAP_INIT(movableRealityObjects),
	MP_MAP_INIT(oneRealityObjects),
	MP_MAP_INIT(allObjectMap)
{
}

CScriptedObjectManager::~CScriptedObjectManager(void)
{
}

unsigned __int64 GetComplexID( unsigned int id1,  unsigned int id2)
{
	unsigned __int64 complexID = id2;
	complexID <<= 32;
	complexID += id1;
	return complexID;
}

void CScriptedObjectManager::JoinScriptedObject( std::string& aJoinedObject)
{
	/*
	CMovableRealityObjectMap::const_iterator itScripted = movableRealityObjects.begin();
	for ( ;  itScripted != movableRealityObjects.end();  itScripted++)
	{
		mlRMMLElement* pElem = itScripted->second;
		if (!itScripted->second->IsAvatar())
			continue;
		aJoinedObject += cLPCSTR( itScripted->second->GetName());
		aJoinedObject += " (";
		AppendPointer(aJoinedObject, pElem);
		aJoinedObject += "), ";
	}
	*/
}

void CScriptedObjectManager::RegisterObject( mlRMMLElement* apMLEl)
{
	if (apMLEl == NULL)
		return;

	UnregisterObject( apMLEl);

	CObjectIdentifiers objectIdentifiers;
	objectIdentifiers.objectID = apMLEl->GetID();
	objectIdentifiers.bornRealityID = apMLEl->GetBornRealityID();
	allObjectMap[ apMLEl] = objectIdentifiers;

	if (apMLEl->IsAvatar())
	{
		unsigned __int64 complexID = GetComplexID( apMLEl->GetID(), apMLEl->GetBornRealityID());
		movableRealityObjects[ complexID] = apMLEl;
	}
	else
		oneRealityObjects[ apMLEl->GetID()] = apMLEl;
}

void CScriptedObjectManager::ChangeObjectRealityAndBornReality( mlRMMLElement* apMLEl, unsigned int auRealityID, unsigned int auBornRealityID)
{
	// ѕри переходе из циклической записи в такую же обычную - bornReality совпадают, иначе надо еще реальности сравнивать
	unsigned int currentReality = apMLEl->GetRealityID();
	apMLEl->RealityChanged( auRealityID);

	//исправление ошибки #913 - добавлены проверки на currentBornReality != auBornRealityID
	unsigned int currentBornReality = apMLEl->GetBornRealityID();
	apMLEl->SetBornRealityID( auBornRealityID);
	RegisterObject( apMLEl);
	if (currentBornReality != auBornRealityID || currentReality != auRealityID)
	{
		apMLEl->ResetSynch();
		apMLEl->ResetSynchedChildren();
	}
}

void CScriptedObjectManager::UnregisterObject( mlRMMLElement* apMLEl)
{
	CAllObjectMap::iterator it = allObjectMap.find( apMLEl);
	if (it == allObjectMap.end())
		return;

	CObjectIdentifiers objectIdentifiers = it->second;
	unsigned __int64 complexID = GetComplexID( objectIdentifiers.objectID, objectIdentifiers.bornRealityID);
	movableRealityObjects.erase( complexID);
	oneRealityObjects.erase( objectIdentifiers.objectID);

	allObjectMap.erase( it);
}

mlRMMLElement* CScriptedObjectManager::GetObject(unsigned int auObjID, unsigned int auBornRealityID, bool aAccurately)
{
	mlRMMLElement* pMLEl = NULL;
	unsigned __int64 complexID = GetComplexID( auObjID, auBornRealityID);
	CMovableRealityObjectMap::const_iterator mi = movableRealityObjects.find( complexID);
	if (mi != movableRealityObjects.end())
		pMLEl = mi->second;
	else
		pMLEl = GetOneRealityObject(auObjID);

	if (pMLEl == NULL)
		return NULL;

	CAllObjectMap::iterator it = allObjectMap.find( pMLEl);
	if (it == allObjectMap.end())
		return NULL;

	CObjectIdentifiers findingIdentifiers( auObjID, auBornRealityID);
	CObjectIdentifiers registerIdentifiers = it->second;
	CObjectIdentifiers realIdentifiers( pMLEl->GetID(), pMLEl->GetBornRealityID());

	if (realIdentifiers != findingIdentifiers)
	{
		// »справление #2429
		if (aAccurately)
			pMLEl = NULL;
		//  онец исправлени€ #2429
	}
	else if (realIdentifiers != registerIdentifiers)
		RegisterObject( pMLEl);
	
	return pMLEl;
}

mlRMMLElement* CScriptedObjectManager::GetOneRealityObject(unsigned int auObjID)
{
	mlRMMLElement* pMLEl = NULL;
	COneRealityObjectMap::const_iterator it = oneRealityObjects.find( auObjID);
	if (it != oneRealityObjects.end())
		pMLEl = it->second;

	if (pMLEl == NULL)
		return NULL;
	if (pMLEl->GetID() != auObjID)
	{
		RegisterObject( pMLEl);
		pMLEl = NULL;
	}
	return pMLEl;
}

void CScriptedObjectManager::GetObjects( ObjectsVector& anObjects)
{
	anObjects.reserve( movableRealityObjects.size() + oneRealityObjects.size());

	CMovableRealityObjectMap::const_iterator itMovable = movableRealityObjects.begin();
	for ( ;  itMovable != movableRealityObjects.end();  itMovable++)
	{
		mlRMMLElement* pMLEl = itMovable->second;
		anObjects.push_back( pMLEl);
	}

	COneRealityObjectMap::const_iterator itOneReality = oneRealityObjects.begin();
	for ( ;  itOneReality != oneRealityObjects.end();  itOneReality++)
	{
		mlRMMLElement* pMLEl = itOneReality->second;
		anObjects.push_back( pMLEl);
	}
}

void CScriptedObjectManager::GetNoAvatarSyncObjects( ObjectsVector& anObjects)
{
	anObjects.reserve( movableRealityObjects.size() + oneRealityObjects.size());

	CMovableRealityObjectMap::const_iterator itMovable = movableRealityObjects.begin();
	for ( ;  itMovable != movableRealityObjects.end();  itMovable++)
	{
		mlRMMLElement* pMLEl = itMovable->second;
		if( !pMLEl->IsSynchronized() || pMLEl->GetID() >= OBJ_ID_AVATAR)
			continue;
		anObjects.push_back( pMLEl);
	}

	COneRealityObjectMap::const_iterator itOneReality = oneRealityObjects.begin();
	for ( ;  itOneReality != oneRealityObjects.end();  itOneReality++)
	{
		mlRMMLElement* pMLEl = itOneReality->second;
		if( !pMLEl->IsSynchronized() || pMLEl->GetID() >= OBJ_ID_AVATAR)
			continue;
		anObjects.push_back( pMLEl);
	}
}

// ¬озвращает живых аватаров. »справление #84
void CScriptedObjectManager::GetRealAvatars( ObjectsVector& anObjects )
{
	anObjects.reserve( movableRealityObjects.size() + anObjects.size());

	CMovableRealityObjectMap::const_iterator itMovable = movableRealityObjects.begin();
	for ( ;  itMovable != movableRealityObjects.end();  itMovable++)
	{
		mlRMMLElement* pMLEl = itMovable->second;
		if (pMLEl->GetID() >= OBJ_ID_AVATAR && pMLEl->GetBornRealityID() == 0)
			anObjects.push_back( pMLEl);
	}
}

// ”дал€ет записанных аватаров, которые могли остатьс€ при выходе из записи
void CScriptedObjectManager::GetRecordedAvatars( ObjectsVector& anObjects )
{
	anObjects.reserve( movableRealityObjects.size() + anObjects.size());

	CMovableRealityObjectMap::const_iterator itMovable = movableRealityObjects.begin();
	for ( ;  itMovable != movableRealityObjects.end();  itMovable++)
	{
		mlRMMLElement* pMLEl = itMovable->second;
		if (pMLEl->GetID() >= OBJ_ID_AVATAR && pMLEl->GetBornRealityID() != 0)
			anObjects.push_back( pMLEl);
	}
}
}