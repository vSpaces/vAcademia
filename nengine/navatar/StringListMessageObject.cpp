
#pragma once

#include "StdAfx.h"
#include "StringListMessageObject.h"

CStringListMessageObject::CStringListMessageObject(std::vector<std::string>& list, int argCount):
	MP_VECTOR_INIT(m_list),
	m_currentLineID(0),
	m_argCount(argCount)
{
	std::vector<std::string>::iterator it = list.begin();
	std::vector<std::string>::iterator itEnd = list.end();

	for ( ; it != itEnd; it++)
	{
		list.push_back(*it);
	}
}

int CStringListMessageObject::GetArgumentCount()
{
	return m_argCount;
}

std::string CStringListMessageObject::GetNextString()
{
	std::string tmp = m_list[m_currentLineID];
	m_currentLineID++;

	return tmp;
}

CStringListMessageObject::~CStringListMessageObject()
{
}