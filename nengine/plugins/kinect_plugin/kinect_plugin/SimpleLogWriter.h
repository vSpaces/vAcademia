
#pragma once

#include <string>

class CSimpleLogWriter
{
public:
	CSimpleLogWriter();
	~CSimpleLogWriter();

	void Write(std::string s);
	void Write(std::string s, int a);

private:
	std::string m_logPath;
};