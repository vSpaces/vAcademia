
#pragma once

#include "../CommonRenderManagerHeader.h"

#include "../../nengine/nengine/3DObject.h"

class CActiveObjects
{
public:
	CActiveObjects();
	~CActiveObjects();
	
	void Deactivate();
	std::vector <C3DObject*> GetObjects();
	unsigned int Count();
	void AddObject(C3DObject* obj);
	C3DObject* GetObject(int index);

	bool IsActive(C3DObject* obj);
	void Deactivate(C3DObject* obj);

	C3DObject* GetNextTeleportObject();

private:
	unsigned int m_teleportIndex;

	MP_VECTOR<C3DObject*> m_activeObjects;
	CRITICAL_SECTION m_cs;

	void Clear();
	void Erase(int index);

};