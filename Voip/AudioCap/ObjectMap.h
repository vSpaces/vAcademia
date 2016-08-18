#pragma once

//#ifndef ABSTRACT_OBJECT_INCLUDED
//	#include "abstractObject.h"
//#endif

template <class keyType, class objectType>
class CObjectMap : public PObject
{
public:
	CObjectMap(){};
	virtual ~CObjectMap(){};

	typedef std::map<keyType, objectType> objectMap;
	//typedef std::map<keyType, objectType *>::iterator objectIter;

public:
	// проверяет наличие ключа в списке
	BOOL hasObject( keyType key)
	{
		return m_objectMap.find( key) != m_objectMap.end();
	};

	// вставить объект с ключом 
	virtual BOOL insert( keyType key, objectType object)
	{
		m_objectMap.insert( std::pair<keyType, objectType>(key, object));
		return TRUE;
	};

	// обновить объект
	virtual BOOL update( objectType object)
	{		
		return TRUE;
	};

	// удалить объект по ключу
	virtual BOOL remove( keyType key, BOOL deleted = FALSE)
	{
		if ( deleted)
		{			
			const char *typeName = typeid( objectType).name();
			char *pdest = strchr( typeName, '*');
			if ( pdest != NULL)
			{
				objectType object = getObject( key);
				SAFE_DELETE( object);
			}
		}
		m_objectMap.erase( key);
		return TRUE;
	};

	// возврашает объект по ключу
	objectType getObject( keyType key)
	{
		if ( hasObject( key))
			return m_objectMap[ key];
		return (objectType)0;
	};

	// возвращает размер список объектов
	int getSize()
	{
		return m_objectMap.size();
	};

	// удаляет все объектов из списка
	void clear( BOOL bRemove = FALSE)
	{
		std::map<keyType, objectType>::iterator iter;
		if ( bRemove)
		{
			for ( iter = m_objectMap.begin(); iter != m_objectMap.end(); ++iter)
			{
				objectType object = ( objectType ) iter->second;			
				const char *typeName = typeid( objectType).name();
				char *pdest = strchr( typeName, '*');
				if ( pdest != NULL)
					SAFE_DELETE( object);
			}
		}
		m_objectMap.clear();
	};

protected:
	objectMap m_objectMap;
};
