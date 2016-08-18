
#include "StdAfx.h"
#include "StringFunctions.h"
#include "Cal3DPathParser.h"

CCal3DPathParser::CCal3DPathParser():
	MP_VECTOR_INIT(m_materials),
	MP_STRING_INIT(m_path),
	MP_STRING_INIT(m_dir),
	m_type(TYPE_UNKNOWN),
	m_skeletonOffset(0),
	m_skeletonSize(0),
	m_meshOffset(0),
	m_xmlOffset(0),
	m_meshSize(0),
	m_xmlSize(0),
	m_data(NULL),
	m_size(0)
{
}

bool CCal3DPathParser::Parse(std::string& path, void* data, unsigned int size)
{
	m_type = TYPE_UNKNOWN;
	m_path = path;

	int pos = path.find_last_of(".");
	if (-1 == pos)
	{
		return false;
	}

	int pos1 = path.find_last_of("\\");
	int pos2 = path.find_last_of("/");
	int posDir = (pos1 > pos2) ? pos1 : pos2;

	if (-1 == posDir)
	{
		return false;
	}

	m_dir = path.substr(0, posDir + 1);
	std::string ext = path.substr(pos + 1, path.length() - pos - 1);
	ext = StringToLower(ext);

	if ("xml" == ext)
	{
		ifile* file = g->rl->GetResMemFile(path.c_str());
		bool isFound = (file != NULL);
		g->rl->DeleteResFile(file);
		m_type = TYPE_MANY_FILES;
		return isFound;
	}
	else if ("pck" == ext)
	{
		m_type = TYPE_SINGLE_FILE;
		m_data = NULL;

		if (!data)
		{
			ifile* file = g->rl->GetResMemFile(path.c_str());
			if (file)
			{
				m_size = file->get_file_size();
				m_data = MP_NEW_ARR(char, m_size);			
				file->read((byte*)m_data, m_size);			
				g->rl->DeleteResFile(file);
			}
		}
		else
		{
			m_size = size;
			m_data = MP_NEW_ARR(char, m_size);			
			memcpy(m_data, data, size);			
		}

		if (m_data)
		{
			m_meshSize = *(int*)m_data;
			m_skeletonSize = *(((int*)m_data) + 1);
			m_xmlSize = *(((int*)m_data) + 2);
			int matCount = *(((int*)m_data) + 3);
			/*int blendCount = *(((int*)m_data) + 4);*/
			m_meshOffset = 20;
			m_skeletonOffset = m_meshOffset + m_meshSize;
			m_xmlOffset = m_skeletonOffset + m_skeletonSize;
			int materialOffset = m_xmlOffset + m_xmlSize;
			for (int i = 0; i < matCount; i++)
			{
				int materialSize = *(int*)(((char*)m_data) + materialOffset);
				materialOffset += 4;

				SMaterialFileDesc tmp(materialOffset, materialSize);
				m_materials.push_back(tmp);

				materialOffset += materialSize;
			}
		}
		else
		{
			return false;
		}

		return true;
	}

	return false;
}

bool CCal3DPathParser::GetMeshData(char*& data, int& size, bool& isNeedToDelete)
{
	isNeedToDelete = false;

	if (TYPE_MANY_FILES == m_type)
	{
		std::string path = m_dir;
		path += "mesh.cmf";
		ifile* file = g->rl->GetResMemFile(path.c_str());
		if (file)
		{
			size = file->get_file_size();
			data = MP_NEW_ARR(char, size + 1);
			data[size] = 0;
			file->read((byte*)data, size);			
			g->rl->DeleteResFile(file);
			isNeedToDelete = true;
		}
		else
		{
			return false;
		}
	}
	else if (TYPE_SINGLE_FILE == m_type)
	{
		if (0 == m_meshSize)
		{
			return false;
		}

		data = (((char*)m_data) + m_meshOffset);
		size = m_meshSize;
	}

	return true;
}

bool CCal3DPathParser::GetSkeletonData(char*& data, int& size, bool& isNeedToDelete, std::string& path)
{
	isNeedToDelete = false;

	if (TYPE_MANY_FILES == m_type)
	{		
		ifile* file = g->rl->GetResMemFile(path.c_str());
		if (file)
		{
			size = file->get_file_size();
			data = MP_NEW_ARR(char, size + 1);
			data[size] = 0;
			file->read((byte*)data, size);			
			g->rl->DeleteResFile(file);
			isNeedToDelete = true;
		}
		else
		{
			return false;
		}
	}
	else if (TYPE_SINGLE_FILE == m_type)
	{
		if (0 == m_skeletonSize)
		{
			return false;
		}

		data = (((char*)m_data) + m_skeletonOffset);
		size = m_skeletonSize;
	}

	return true;
}

bool CCal3DPathParser::GetXmlData(char*& data, int& size, bool& isNeedToDelete)
{
	isNeedToDelete = false;

	if (TYPE_MANY_FILES == m_type)
	{
		ifile* file = g->rl->GetResMemFile(m_path.c_str());
		if (file)
		{
			size = file->get_file_size();
			data = MP_NEW_ARR(char, size + 1);
			data[size] = 0;
			file->read((byte*)data, size);			
			g->rl->DeleteResFile(file);
			isNeedToDelete = true;
		}
		else
		{
			return false;
		}
	}
	else if (TYPE_SINGLE_FILE == m_type)
	{
		if (0 == m_xmlSize)
		{
			return false;
		}

		char* data2 = (((char*)m_data) + m_xmlOffset);
		size = m_xmlSize;	
		data = MP_NEW_ARR(char, size + 1);
		data[size] = 0;

		memcpy(data, data2, size);
		isNeedToDelete = true;
	}
		
	return true;	
}
	
bool CCal3DPathParser::GetMaterialData(std::string matName, int matIndex, char*& data, int& size, bool& isNeedToDelete)
{
	isNeedToDelete = false;

	if (TYPE_MANY_FILES == m_type)
	{
		ifile* file = g->rl->GetResMemFile(matName.c_str());
		if (file)
		{
			size = file->get_file_size();
			data = MP_NEW_ARR(char, size + 1);
			data[size] = 0;
			file->read((byte*)data, size);			
			g->rl->DeleteResFile(file);
			isNeedToDelete = true;
		}
		else
		{
			return false;
		}
	}
	else if (TYPE_SINGLE_FILE == m_type)
	{
		if ((unsigned int)matIndex >= m_materials.size())
		{
			return false;
		}

		data = (((char*)m_data) + m_materials[matIndex].offset);
		size = m_materials[matIndex].size;		
	}

	return true;
} 

void CCal3DPathParser::ClearAll()
{
	if (m_data)
	{
		MP_DELETE_ARR(m_data);
		m_data = NULL;
		m_size = 0;
	}

	m_meshSize = 0;
	m_meshOffset = 0;

	m_skeletonSize = 0;
	m_skeletonOffset = 0;

	m_xmlSize = 0;
	m_xmlOffset = 0;

	m_materials.clear();
	
	m_type = TYPE_UNKNOWN;
}

CCal3DPathParser::~CCal3DPathParser()
{
}