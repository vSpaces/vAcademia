#pragma once

#include "CommonRenderManagerHeader.h"

// базовый класс для объекта, которому другие объекты
// отдают ссылки на себя или на связанные объекты
class CLinkCollector
{
public:
	CLinkCollector();
	~CLinkCollector();

	void AddLink(void* pLink);
	void DeleteLink(void* pLink);
	std::vector<void*>& GetLinks();

private:
	MP_VECTOR<void*> m_links;
};