#pragma once

class CFileNameStorage
{
public:
	static CFileNameStorage* GetInstance();

	std::string GetFileNameById(unsigned short id);
	unsigned short AddFileName(std::string fileName);

private:

	CFileNameStorage();
	~CFileNameStorage();

	typedef MP_MAP<MP_STRING, unsigned short> MapFilesIDByName;
	MapFilesIDByName m_filesIDByName;
	MP_VECTOR<MP_STRING> m_fileNames;

};