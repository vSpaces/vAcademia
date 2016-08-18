#pragma once

#include "resloader.h"
#include "n3dasynchresource.h"

using namespace natura3d;

typedef std::map<iasynchresource*, int> ResList;

class RESLOADER_API CResourceLoader
{
public:
	CResourceLoader();
	~CResourceLoader();
	
	void SendResourcePriority(int priority, iasynchresource* res);

private:
	ResList m_resList;	
};