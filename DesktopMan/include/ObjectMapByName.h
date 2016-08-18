#pragma once

#include <map>
#include "ComString.h"
#include "SharingContext.h"


template <class T>
class CObjectMapByName
{
public:
	CObjectMapByName()
	{
	}

	~CObjectMapByName()
	{
	}

	T*	GetObjectByName( LPCSTR alpcName, SHARING_CONTEXT* context)
	{
		if( !alpcName)
			return NULL;
		if( *alpcName == 0)
			return NULL;
		CComString	sObjectName( alpcName);
		if( mapObjects.find(sObjectName) != mapObjects.end())
			return mapObjects[ sObjectName];

		return NULL;
	}

	T*	CreateObjectByName( LPCSTR alpcName, SHARING_CONTEXT* context)
	{
		if( !alpcName)
			return NULL;
		if( *alpcName == 0)
			return NULL;
		CComString	sObjectName( alpcName);
		if( mapObjects.find(sObjectName) != mapObjects.end())
			return mapObjects[ sObjectName];

		T*	session = NULL;
		MP_NEW_V(session, T, context);
		mapObjects[ sObjectName] = session;
		return session;
	}

	void	DeleteObjectByName( LPCSTR alpcName)
	{
		T*	session = mapObjects[ alpcName];
		mapObjects.erase( mapObjects.find( alpcName));
		SAFE_DELETE( session);
	}

	void	DeleteObject( T* apObject)
	{
		std::map<CComString, T*>::iterator it = mapObjects.begin(), end = mapObjects.end();
		for (; it!=mapObjects.end(); it++)
		{
			if( it->second == apObject)
			{
				mapObjects.erase( it);
				SAFE_DELETE( apObject);
				return;
			}
		}
	}

	void	DeleteAllObjects()
	{
		while( !mapObjects.empty())
		{
			T* session = mapObjects.begin()->second;
			mapObjects.erase( mapObjects.begin());
			SAFE_DELETE( session);
		}
	}

	//without delete!
	void	EraseObjectByName( LPCSTR alpcName)
	{
		T*	session = mapObjects[ alpcName];
		mapObjects.erase( mapObjects.find( alpcName));
	}
	//without delete!
	void	EraseObject( T* apObject)
	{
		std::map<CComString, T*>::iterator it = mapObjects.begin(), end = mapObjects.end();
		for (; it!=mapObjects.end(); it++)
		{
			if( it->second == apObject)
			{
				mapObjects.erase( it);
				return;
			}
		}
	}
	//without delete!
	void	EraseAllObjects()
	{
		while( !mapObjects.empty())
		{
			T* session = mapObjects.begin()->second;
			mapObjects.erase( mapObjects.begin());
		}
	}


	T* GetFirst()
	{
		if( mapObjects.size() == 0)
			return NULL;

		iter = mapObjects.begin();
		return iter->second;
	}

	T* GetNext()
	{
		iter++;
		if( iter == mapObjects.end())
			return NULL;

		return iter->second;
	}

private:
	std::map<CComString, T*>	mapObjects;
	typename std::map<CComString, T*>::iterator iter;
};