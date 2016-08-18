
#include "StdAfx.h"
#include "CSVFile.h"

CCSVFile::CCSVFile(std::string fileName):
	m_file(NULL),
	m_fileName(fileName)
{
}

void CCSVFile::Rewrite()
{
	if (0 == m_fileName.size())
	{
		return;
	}

	m_file = fopen(m_fileName.c_str(), "w");
}

void CCSVFile::WriteLn(CLogValue l1)
{
	std::string str = l1.GetData();
	str += ";";
	WriteLn(str);
}

void CCSVFile::WriteLn(CLogValue l1, CLogValue l2)
{
	CLogValue l(l1, ";", l2);
	WriteLn(l);
}

void CCSVFile::WriteLn(CLogValue l1, CLogValue l2, CLogValue l3)
{
	CLogValue l(l1, ";", l2);
	WriteLn(l, l3);
}

void CCSVFile::WriteLn(CLogValue l1, CLogValue l2, CLogValue l3, CLogValue l4)
{
	CLogValue l(l1, ";", l2);
	WriteLn(l, l3, l4);
}

void CCSVFile::WriteLn(CLogValue l1, CLogValue l2, CLogValue l3, CLogValue l4, CLogValue l5)
{
	CLogValue l(l1, ";", l2);
	WriteLn(l, l3, l4, l5);
}

void CCSVFile::WriteLn(CLogValue l1, CLogValue l2, CLogValue l3, CLogValue l4, CLogValue l5, CLogValue l6)
{
	CLogValue l(l1, ";", l2);
	WriteLn(l, l3, l4, l5, l6);
}

void CCSVFile::Close()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

void CCSVFile::WriteLn(std::string str)
{
	if (!m_file)
	{
		return;
	}

	fprintf(m_file, str.c_str());
	fprintf(m_file, "\n");
}

CCSVFile::~CCSVFile()
{
}