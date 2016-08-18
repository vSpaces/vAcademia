
#pragma once

class IMessageObject
{
public:
	virtual int GetArgumentCount() = 0;
	virtual std::string GetNextString() = 0;
};