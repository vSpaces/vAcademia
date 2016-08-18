
#pragma once

#include "CommonEngineHeader.h"

#include <vector>
#include "ITask.h"
#include "IDynamicTexture.h"

class CUpdateDynamicTexturesTask : public ITask
{
public:
	CUpdateDynamicTexturesTask();
	~CUpdateDynamicTexturesTask();

	void AddDynamicTexture(IDynamicTexture* dynamicTexture);
	void Start();
	void PrintInfo();

	void Destroy() { assert(true); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	MP_VECTOR<IDynamicTexture *> m_dynamicTextures;
};