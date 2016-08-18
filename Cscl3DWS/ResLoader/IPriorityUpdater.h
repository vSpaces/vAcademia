
#pragma once

struct IAsynchResource;

class IPriorityUpdater
{
public:
	virtual void UpdateResourceLoadingPriority(IAsynchResource* asynchRes) = 0;
};