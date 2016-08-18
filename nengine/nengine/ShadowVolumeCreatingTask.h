
#pragma once

#include <vector>
#include "ITask.h"

class CShadowVolumeCreatingTask : public ITask
{
public:
	CShadowVolumeCreatingTask();
	~CShadowVolumeCreatingTask();

	void SetShadowObject(void* shadowObject);
	void Start();
	void PrintInfo();

	void Destroy() { assert(true); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	__forceinline bool IsFacesNeighbours(int fs1[3], int fs2[3])
	{
		int count = 0;

		for (int k = 0; k < 3; k++)
		for (int i = 0; i < 3; i++)
		if (fs1[k] == fs2[i])	
		{
			m_sovp[count++] = k;
		}

		return (count == 2);
	}

	void* m_fs;
	int m_faceCount;
	int m_sovp[9];
};