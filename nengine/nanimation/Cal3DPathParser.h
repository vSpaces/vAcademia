#pragma once

#include <string>
#include <vector>

#define TYPE_UNKNOWN		0
#define TYPE_MANY_FILES		1
#define TYPE_SINGLE_FILE	2

typedef struct _SMaterialFileDesc
{
	int offset;
	int size;

	_SMaterialFileDesc(int _offset, int _size)
	{
		offset = _offset;
		size = _size;
	}

	_SMaterialFileDesc(const _SMaterialFileDesc& other)
	{
		offset = other.offset;
		size = other.size;
	}
} SMaterialFileDesc;

class CCal3DPathParser
{
public:
	CCal3DPathParser();
	~CCal3DPathParser();

	bool Parse(std::string& path, void* data, unsigned int size);

	bool GetMeshData(char*& data, int& size, bool& isNeedToDelete);
	bool GetSkeletonData(char*& data, int& size, bool& isNeedToDelete, std::string& path);
	bool GetXmlData(char*& data, int& size, bool& isNeedToDelete);
	
	bool GetMaterialData(std::string matName, int matIndex, char*& data, int& size, bool& isNeedToDelete);

	void ClearAll();

private:
	char* m_data;
	int m_size;

	int m_meshSize;
	int m_meshOffset;

	int m_skeletonSize;
	int m_skeletonOffset;

	int m_xmlSize;
	int m_xmlOffset;

	MP_VECTOR<SMaterialFileDesc> m_materials;
	MP_STRING m_path;
	MP_STRING m_dir;

	char m_type;
};