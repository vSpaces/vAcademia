
#pragma once

#include "StdAfx.h"
#include "CommonEngineHeader.h"
#include "CommonDefines.h"
#include <hash_map>
#include <vector>
#include <string>
#include <assert.h>

#pragma  warning (disable: 4786)

typedef MP_MAP<MP_WSTRING, int> LookupList;

template <class T>
class CManager
{
public:	
	CManager():
		MP_VECTOR_INIT(m_liveObjects),
		MP_VECTOR_INIT(m_objects),
		m_reservedMemoryBlock(NULL),
		m_reservedBlockCount(0),
		MP_MAP_INIT(m_map),
		m_count(0)
	{		
	}

	void ReserveMemory(const unsigned int count)
	{
		if (0 == count)
		{
			return;
		}

		m_objects.reserve(count);
		m_liveObjects.reserve(count);

		unsigned int size = count * sizeof(T);
		m_reservedMemoryBlock = MP_NEW_ARR(unsigned char, size);
 		m_reservedBlockCount = count;
	}

	bool IsObjectExists(const std::string& name)
	{
		USES_CONVERSION;
		std::wstring wname = A2W( name.c_str());
		return IsObjectExists( wname);
	}

	bool IsObjectExists(const std::wstring& name)
	{
		LookupList::const_iterator iter = m_map.find(MAKE_MP_WSTRING(name));

		return (iter != m_map.end());
	}

	// Return number of object in list
	// If object not found return -1
	virtual int GetObjectNumber(const std::string& name)const
	{
		USES_CONVERSION;
		std::wstring wname = A2W( name.c_str());
		return GetObjectNumber( wname);
	}

	virtual int GetObjectNumber(const std::wstring& name)const
	{
		LookupList::const_iterator iter = m_map.find(MAKE_MP_WSTRING(name));

		if (iter != m_map.end())
		{
			return (*iter).second;
		}
		else
		{
			return ERR_OBJECT_NOT_FOUND;
		}
	}

	// Return pointer to object
	T* GetObjectPointer(const unsigned int num)
	{
		if ((num >= 0) && (num < m_objects.size()))
		{
			return (m_objects[num]);
		}
		else
		{
			if (m_objects.size() >= 1)
			{
				return (m_objects[0]);
			}
			else
			{
				return NULL;
			}
		}
	}

	// Add object to list
	int AddObject(const std::string& name)
	{
		USES_CONVERSION;
		std::wstring wname = A2W( name.c_str());
		return AddObject( wname);
	}

	int AddObject(const std::wstring& name)
	{
		int id = GetObjectNumber(name);
		if (id != -1)
		{
			return id;
		}

		T* obj = AllocateObjectInMemory();

		obj->SetName(name);
		obj->SetID(m_count);
		m_objects.push_back(obj);
		m_liveObjects.push_back(obj);

		m_map.insert(LookupList::value_type(MAKE_MP_WSTRING(name), m_count));

		return (m_count++);
	}

	int RestoreObject(T* const obj)
	{
		if (!obj)
		{
			return -1;
		}

		int id = GetObjectNumber(obj->GetName());
		if (id != -1)
		{
			return id;
		}

		id = 0;
		std::vector<T*>::iterator it = m_objects.begin();
		std::vector<T*>::iterator itEnd = m_objects.end();

		for ( ; it != itEnd; it++, id++)
		if (*it == obj)
		{
				break;
		}

		if ((unsigned int)id < m_objects.size())
		{
			m_map.insert(LookupList::value_type(MAKE_MP_WSTRING(obj->GetName()), id));
			m_liveObjects.push_back(obj);
			obj->SetID(id);

			return id;
		}
		else
		{
			return -1;
		}
	}

	int ManageObject(T* const obj)
	{
		if (!obj)
		{
			return -1;
		}

		int id = GetObjectNumber(obj->GetName());
		if (id != -1)
		{
			return id;
		}
	
		m_objects.push_back(obj);	
		m_liveObjects.push_back(obj);
		m_map.insert(LookupList::value_type(MAKE_MP_WSTRING(obj->GetName()), m_count));
		obj->SetID(m_count);

		return (m_count++);
	}

	// Return count of objects
	int GetCount()const
	{
		return m_count;
	}

	void RenameObject(const unsigned int id, const std::string& name)
	{
		USES_CONVERSION;
		std::wstring wname = A2W( name.c_str());
		RenameObject( id, wname);
	}

	void RenameObject(const unsigned int id, const std::wstring& name)
	{
		if (id < m_objects.size())
		{
			LookupList::iterator it = m_map.find(MAKE_MP_WSTRING(m_objects[id]->GetName()));
			if (it != m_map.end())
			{
				m_map.erase(it);
			}
	
			m_objects[id]->SetName(name);
			m_map.insert(LookupList::value_type(MAKE_MP_WSTRING(name), id));
		}
	}

	void DeleteObject(const unsigned int id)
	{
		if (id < m_objects.size())
		{
			T* obj = m_objects[id];
			if (!obj->IsDeleted())
			{
				obj->MarkDeleted();	
			}

			LookupList::iterator it = m_map.find(MAKE_MP_WSTRING(obj->GetName()));
			if (it != m_map.end())
			{
				m_map.erase(it);
			}

			std::vector<T*>::iterator iter = m_liveObjects.begin();
			std::vector<T*>::iterator iterEnd = m_liveObjects.end();

			for ( ; iter != iterEnd; iter++)
			if (*iter == obj)
			{
				m_liveObjects.erase(iter);
				break;
			}
		}
	}

	void UnManageObject(const unsigned int id)
	{
		if (id < m_objects.size())
		{
			T* obj = m_objects[id];
			
			LookupList::iterator it = m_map.find(MAKE_MP_WSTRING(obj->GetName()));
			if (it != m_map.end())
			{
				m_map.erase(it);
			}

			std::vector<T*>::iterator iter = m_liveObjects.begin();
			std::vector<T*>::iterator iterEnd = m_liveObjects.end();

			for ( ; iter != iterEnd; iter++)
			if (*iter == obj)
			{
				m_liveObjects.erase(iter);
				break;
			}
		}
	}

	int GetLiveObjectsCount()
	{
		return m_liveObjects.size();
	}

	typename std::vector<T*>::iterator GetLiveObjectsBegin()
	{
		return m_liveObjects.begin();
	}

	typename std::vector<T*>::iterator GetLiveObjectsEnd()
	{
		return m_liveObjects.end();
	}

	void ClearAllLinks()
	{
		m_map.clear();
		m_objects.clear();
		m_liveObjects.clear();
		m_count = 0;		
	}

	~CManager()
	{
		ClearAll();
	}

private:
	// Clear all objects
	void ClearAll()
	{
		std::vector <T*>::iterator iter = m_objects.begin();
		std::vector <T*>::iterator end = m_objects.end();		

		for (unsigned int objectID = 0; iter != end; iter++, objectID++)
		if (!(*iter)->IsDeleted())
		{
			if (objectID < m_reservedBlockCount)
			{
				(*iter)->~T();
			}
			else
			{
				delete (*iter);
			}
		}

		ClearAllLinks();	

		if (m_reservedMemoryBlock)
		{
			delete[] m_reservedMemoryBlock;
		}
	}

	T* AllocateObjectInMemory()
	{
		if (m_objects.size() < m_reservedBlockCount)
		{
			unsigned char* ptr = m_reservedMemoryBlock;
			ptr += m_objects.size() * sizeof(T);
			// placement new, поэтому на профайлер менять не надо!
			return new (ptr) T();
		}

		return MP_NEW(T);
	}

	MP_VECTOR<T*> m_objects;
	MP_VECTOR<T*> m_liveObjects;

	unsigned char* m_reservedMemoryBlock;
	unsigned int m_reservedBlockCount;

	LookupList m_map;
	
	int m_count;
};
