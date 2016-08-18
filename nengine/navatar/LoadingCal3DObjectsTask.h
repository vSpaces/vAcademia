
#pragma once

#include "CommonAvatarHeader.h"

#include <vector>
#include "ITask.h"
#include "FakeObject.h"

class CLoadingCal3DObjectsTask : public ITask
{
public:
	CLoadingCal3DObjectsTask();
	~CLoadingCal3DObjectsTask();

	void SetParams(CAvatarObject* avatarObject, C3DObject* obj3d, const std::string& path, const std::string& name, void* data, unsigned int size);
	
	STransitionalInfo* GetTransitionalInfo();

	std::string GetPath();

	std::string GetName();

	bool IsLoaded()const;

	void Start();

	void PrintInfo();

	void Destroy();
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	void FreeData();

	CAvatarObject* m_avatarObject;
	MP_STRING m_path;
	MP_STRING m_name;
	STransitionalInfo* m_info;

	bool m_isLoaded;	

	unsigned int m_size;
	void* m_data;
};