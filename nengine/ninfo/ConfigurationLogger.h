
#pragma once

#include "ILogWriter.h"

class CConfigurationLogger
{
public:
	CConfigurationLogger();
	~CConfigurationLogger();

	void LogAll(ILogWriter* lw);
	void LogProcesses(ILogWriter* lw);

private:

};