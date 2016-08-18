
#pragma once

#include "MessageObject.h"

class CVAListMessageObject : public IMessageObject
{
public:
	CVAListMessageObject(va_list list, int argCount);
	~CVAListMessageObject();

	int GetArgumentCount();
	std::string GetNextString();

private:
	va_list m_list;
	int m_argCount;
};