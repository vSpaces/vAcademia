
#pragma once

class ICommandPerformer
{
public:
	virtual void HandleSetParameter(std::string partPath, std::string commandID, std::string commandParam, std::string commandValue) = 0;
	virtual void ApplyDefaultSettings() = 0;
};