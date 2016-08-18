#pragma once

#include "..\nengine\ncommon\Thread.h"
#include <map>
#include "Tlhelp32.h"
#include <string>

typedef unsigned int socklen_t;
#include "../curl/include/curl/curl.h"

class CLoadUpdater;

struct ThreadInfoForVersion
{
	std::string serverIP;
	std::string curVersion;
	std::string buildType;
	CLoadUpdater* sender;
	ThreadInfoForVersion( std::string _serverIP, std::string curVersion, std::string buildType);	
};

class CLoadUpdater
{
public:
	void SendVersion();
	void SetErrorCode(int errorCode);
private:
	std::string GetPropertyFromPlayerINI(std::string section, std::string key);

private:
	CThread m_threadWorldServer;
	int m_errorCode;
};

