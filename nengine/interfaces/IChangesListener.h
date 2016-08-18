
#pragma once

class IChangesListener
{
public:
	virtual void OnChanged(int eventID = 0) = 0;
};