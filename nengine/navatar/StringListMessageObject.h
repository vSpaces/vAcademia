
#pragma once

#include "CommonAvatarHeader.h"

#include "MessageObject.h"
#include <vector>

class CStringListMessageObject : public IMessageObject
{
public:
	CStringListMessageObject(std::vector<std::string>& list, int argCount);
	~CStringListMessageObject();

	int GetArgumentCount();
	std::string GetNextString();

private:
	MP_VECTOR<MP_STRING> m_list;
	int m_currentLineID;
	int m_argCount;
};