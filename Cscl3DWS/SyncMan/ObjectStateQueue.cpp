#include "stdafx.h"
#include "ObjectStateQueue.h"
#include "mlSynchState.h"
#include "ILogWriter.h"

CObjectStateQueue::CObjectStateQueue()
:MP_LIST_INIT(queueObjectStates),
MP_MAP_INIT(objectStateHeap)
{
	InitializeCriticalSection(&locker);
}

CObjectStateQueue::~CObjectStateQueue()
{
	syncObjectState* state = NULL;
	while (Pop( state))
		DeleteObjectState( state);
	DeleteCriticalSection( &locker);
}

unsigned int  CObjectStateQueue::Push( syncObjectState* aObjectState)
{
	CCriticalSectionGuard guard( &locker);
	queueObjectStates.push_back( aObjectState);
	return queueObjectStates.size();
}

bool CObjectStateQueue::Pop( syncObjectState*& aObjectState)
{
	CCriticalSectionGuard guard( &locker);
	if (queueObjectStates.empty())
		return false;

	aObjectState = queueObjectStates.front();
	queueObjectStates.pop_front();
	return true;
}

inline bool IsObjectState(const syncObjectState* const aObjectState,
						  unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID)
{
	if (aObjectState->uiID != aObjectID)
		return false;
	if (aObjectState->uiBornRealityID != aBornRealityID)
		return false;
	if (aObjectState->uiRealityID != aRealityID)
		return false;
	return true;
}

bool CObjectStateQueue::NextObjectState( unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID, syncObjectState* &aObjectsStates)
{
	CCriticalSectionGuard guard( &locker);
	if (queueObjectStates.empty())
		return false;

	CQueueObjectStates::iterator it = queueObjectStates.begin();
	for (;  it != queueObjectStates.end(); it++)
	{
		syncObjectState* state = *it;
		if (IsObjectState( state, aObjectID, aBornRealityID, aRealityID))
		{
			aObjectsStates = state;
			queueObjectStates.erase( it);
			return true;
		}
	}
	return false;
}

bool CObjectStateQueue::IsEmpty()
{
	return queueObjectStates.empty();
}

syncObjectState* CObjectStateQueue::NewSyncObjectState()
{
	syncObjectState* state = MP_NEW( syncObjectState);
	AttachSyncObjectState( state);
	return state;
}

void CObjectStateQueue::AttachSyncObjectState(syncObjectState* aSyncObjectState)
{
	if (aSyncObjectState == NULL)
		return;
	CCriticalSectionGuard guard( &locker);
	objectStateHeap[ aSyncObjectState] = aSyncObjectState;
}

void CObjectStateQueue::DetachSyncObjectState(syncObjectState* aSyncObjectState)
{
	if (aSyncObjectState == NULL)
		return;
	CCriticalSectionGuard guard( &locker);
	CObjectStatesMap::iterator it = objectStateHeap.find( aSyncObjectState);
	if (it == objectStateHeap.end())
		return;
	objectStateHeap.erase( it);
}

void CObjectStateQueue::DeleteObjectStateImpl( syncObjectState*& aSyncObjectState)
{
	if (aSyncObjectState == NULL)
		return;
	CObjectStatesMap::iterator it = objectStateHeap.find( aSyncObjectState);
	if (it == objectStateHeap.end())
		return;
	objectStateHeap.erase( it);
	MP_DELETE( aSyncObjectState);
}

void CObjectStateQueue::DeleteObjectState( syncObjectState*& aSyncObjectState)
{
	if (aSyncObjectState == NULL)
		return;
	CCriticalSectionGuard guard( &locker);
	DeleteObjectStateImpl( aSyncObjectState);
}

inline bool NeedToDeleteState(const syncObjectState* const aObjectState,
						 unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID)
{
	if (aObjectState->uiID != aObjectID)
		return false;
	if (aObjectState->uiBornRealityID != aBornRealityID)
		return false;
	if (aObjectState->uiRealityID != aRealityID)
		return false;
	return true;
}

inline bool NeedToDeleteState(const syncObjectState* const aObjectState,
							  unsigned int aRealityID)
{
	if (aObjectState->uiRealityID == aRealityID)
		return false;
	return true;
}

void CObjectStateQueue::DeleteAllObjectStates( unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID)
{
	CCriticalSectionGuard guard( &locker);

	// ќчередь пуста?
	if (queueObjectStates.empty())
		return;  // ”ходим

	CQueueObjectStates::iterator it = queueObjectStates.begin();
	for (;  it != queueObjectStates.end();)
	{
		syncObjectState* state = *it;
		if (NeedToDeleteState( state, aObjectID, aBornRealityID, aRealityID))
		{
			DeleteObjectStateImpl( state);
			it = queueObjectStates.erase( it);
		}
		else
		{
			it++;
		}
	}
}

void CObjectStateQueue::DeleteObjectsExceptReality( unsigned int auRealityID)
{
	CCriticalSectionGuard guard( &locker);

	// ќчередь пуста?
	if (queueObjectStates.empty())
		return;  // ”ходим

	CQueueObjectStates::iterator it = queueObjectStates.begin();
	for (;  it != queueObjectStates.end();)
	{
		syncObjectState* state = *it;
		if (NeedToDeleteState( state, auRealityID))
		{

			DeleteObjectStateImpl( state);
			it = queueObjectStates.erase( it);
		}
		else
		{
			it++;
		}
	}
}

bool CObjectStateQueue::IsLeaked()
{
	bool leaked = (objectStateHeap.size() != queueObjectStates.size());
	return leaked;
}
