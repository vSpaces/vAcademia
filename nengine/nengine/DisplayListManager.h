
#pragma once

#include "Manager.h"
#include "DisplayList.h"

class CDisplayListManager : public CManager<CDisplayList>
{
friend class CGlobalSingletonStorage;

public:
	int CreateDisplayList(std::string name);

	void CallDisplayList(int num);
	
	void BindDisplayList(int num);
	void UnbindDisplayList();

private:
	CDisplayListManager();
	CDisplayListManager(const CDisplayListManager&);
	CDisplayListManager& operator=(const CDisplayListManager&);
	~CDisplayListManager();

	int m_lastDisplayListNum;
};