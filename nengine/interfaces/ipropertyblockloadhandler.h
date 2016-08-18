
#pragma once

#include <string>

class IPropertyBlockLoadHandler
{
public:
	virtual void OnPropertyBlockLoaded(const void* data, const unsigned int size, const std::string& fileName, const unsigned int modelID) = 0;
};