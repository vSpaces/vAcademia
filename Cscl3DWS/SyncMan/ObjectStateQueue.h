#ifndef OBJECTSTATEQUEUE_H__
#define OBJECTSTATEQUEUE_H__

#include <list>
#include <map>

namespace rmml {
struct syncObjectState;
}
using namespace rmml;

class CObjectStateQueue
{
public:
	CObjectStateQueue();
	virtual ~CObjectStateQueue();

	// Очередь (перевый пришел - первый вышел) состояний объектов
	typedef std::vector<syncObjectState*> CObjectStatesVec;
	bool NextObjectState( unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID, syncObjectState* &aObjectsStates);
	unsigned int Push( syncObjectState* aObjectState);
	bool Pop( syncObjectState*& aObjectState);
	bool IsEmpty();
	bool IsLeaked();

	syncObjectState* NewSyncObjectState();
	void AttachSyncObjectState(syncObjectState* aSyncObjectState);
	void DetachSyncObjectState(syncObjectState* aSyncObjectState);
	void DeleteObjectState( syncObjectState*& aSyncObjectState);
	void DeleteAllObjectStates( unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID);

	void DeleteObjectsExceptReality( unsigned int auRealityID);

	void SetLogWriter( ILogWriter* aILogWriter)
	{
		m_ILogWriter = aILogWriter;
	}

private:
	void DeleteObjectStateImpl( syncObjectState*& aSyncObjectState);

private:
	// Критическая секция на потоково безопасное испозование объекта
	CRITICAL_SECTION locker;

	// Очередь (перевый пришел - первый вышел) состояний объектов
	typedef MP_LIST<syncObjectState*> CQueueObjectStates;
	CQueueObjectStates queueObjectStates;

	// Очередь (перевый пришел - первый вышел) состояний объектов
	typedef MP_MAP<syncObjectState*, syncObjectState*> CObjectStatesMap;
	CObjectStatesMap objectStateHeap;

	ILogWriter* m_ILogWriter;
};

#endif // OBJECTSTATEQUEUE_H__