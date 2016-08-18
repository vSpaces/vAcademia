
#pragma once

#include <string>
#include "LogValue.h"

class CCSVFile
{
public:
	CCSVFile(std::string fileName);
	~CCSVFile();

	void Rewrite();
	void WriteLn(CLogValue l1);
	void WriteLn(CLogValue l1, CLogValue l2);
	void WriteLn(CLogValue l1, CLogValue l2, CLogValue l3);
	void WriteLn(CLogValue l1, CLogValue l2, CLogValue l3, CLogValue l4);
	void WriteLn(CLogValue l1, CLogValue l2, CLogValue l3, CLogValue l4, CLogValue l5);
	void WriteLn(CLogValue l1, CLogValue l2, CLogValue l3, CLogValue l4, CLogValue l5, CLogValue l6);
	void Close();

private:
	void WriteLn(std::string str);

	FILE* m_file;
	std::string m_fileName;
};