#include "mlRMML.h"
#include "FileNameStorage.h"

CFileNameStorage* CFileNameStorage::GetInstance()
{
	static CFileNameStorage* obj = NULL;

	if (!obj)
	{
		obj = new CFileNameStorage();
	}

	return obj;
}

std::string CFileNameStorage::GetFileNameById(unsigned short id)
{
	if (id < m_fileNames.size())
	{
		return m_fileNames[id];
	}
	else
	{
		return "no_name";
	}
}

unsigned short CFileNameStorage::AddFileName(std::string fileName)
{
	MapFilesIDByName::iterator it = m_filesIDByName.find(MAKE_MP_STRING(fileName));

	if (it!= m_filesIDByName.end())
	{
		return (*it).second;
	}
	else
	{
		unsigned short id = m_fileNames.size(); 
		m_fileNames.push_back(MAKE_MP_STRING(fileName));
		m_filesIDByName.insert(MapFilesIDByName::value_type(MAKE_MP_STRING(fileName), id)); 
		return id;
	}

}

CFileNameStorage::CFileNameStorage():
	MP_MAP_INIT(m_filesIDByName), 
	MP_VECTOR_INIT(m_fileNames)
{
}

CFileNameStorage::~CFileNameStorage()
{

}