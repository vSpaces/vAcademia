#pragma once

#include "RotationPack.h"
#include "3DObject.h"
#include "Manager.h"

class C3DObjectManager : public CManager<C3DObject>
{
friend class CGlobalSingletonStorage;

public:
	void LoadWorld(std::wstring fileName);
	void SaveWorld(std::wstring fileName);

	void AddObject3D(C3DObject* obj, CLODGroup* lodGroup, float x, float y, float z,
		CRotationPack* rotation, bool hasPhysics);
	int AddObject3D(CLODGroup* lodGroup, float x, float y, float z,
		CRotationPack* rotation, bool hasPhysics);

	void GetWorldSize(float* x1, float* y1, float* z1, float* x2, float* y2, float* z2);

private:
	float m_minX, m_maxX, m_minY, m_maxY, m_minZ, m_maxZ;

	C3DObjectManager(void);
	C3DObjectManager(const C3DObjectManager&);
	C3DObjectManager& operator=(const C3DObjectManager&);
	~C3DObjectManager(void);
};