
#pragma once

#include "Assert.h"

template <class T>
T* zalloc(int size = -1)
{
	T* tmp = NULL;

	bool isArray = (size != -1);
	if (isArray)
	{
		tmp = new T[size];
		size = size * sizeof(T);
	}
	else
	{
		tmp = new T();
		size = sizeof(T);
	}

	if (tmp)
		memset((void*)tmp, 0, size);

	return tmp;
}

template <class T>
T* mp_realloc(unsigned char dllID, char* fileName, int line, char* funcName, int size = -1, unsigned char location = 0, void* ptr = NULL)
{
	T* tmp = NULL;

	bool isArray = (size != -1);
	if (isArray)
	{
		tmp = new T[size];
		size = size * sizeof(T);
	}
	else
	{
		tmp = new T();
		size = sizeof(T);
	}

	if (ptr != NULL)
	{
		size_t oldSize = _msize(ptr);

		memcpy((void*)tmp, ptr, oldSize < size ? oldSize : size);
		delete [] ptr;

		if (gMP)
		{
			gMP->DeleteInfo(dllID, ptr, funcName, MEMORY_TYPE_ARRAY, sizeof(T));
		}
		else
		{
			assert(false);
		}
	}

	if (gMP)
	{
		gMP->SaveInfo(dllID, tmp, size, fileName, (unsigned short)line, funcName, isArray, sizeof(T), location);
	}
	else
	{
		assert(false);
	}

	return tmp;
}

template <class T>
T* _realloc(int size = -1, void* ptr = NULL)
{
	T* tmp = NULL;

	bool isArray = (size != -1);
	if (isArray)
	{
		tmp = new T[size];
		size = size * sizeof(T);
	}
	else
	{
		tmp = new T();
		size = sizeof(T);
	}

	if (ptr != NULL)
	{
		size_t oldSize = _msize(ptr);
		memcpy((void*)tmp, ptr, (int)oldSize < size ? oldSize : size);
		delete [] ptr;
	}

	return tmp;
}


template <class T>
T* mp_alloc(unsigned char dllID, char* fileName, int line, char* funcName, int size = -1, unsigned char location = 0, bool isZeroMemory = false)
{
	T* tmp = NULL;

	bool isArray = (size != -1);
	if (isArray)
	{
		tmp = new T[size];
		size = size * sizeof(T);
	}
	else
	{
		tmp = new T();
		size = sizeof(T);
	}

	if (gMP)
	{
		gMP->SaveInfo(dllID, tmp, size, fileName, (unsigned short)line, funcName, isArray, sizeof(T), location);
	}
	else
	{
		assert(false);
	}

	if (isZeroMemory && tmp)
		memset((void*)tmp, 0, size);

	return tmp;
}

template <class T>
T* mp_fast_alloc_arr(unsigned char dllID, char* fileName, int line, char* funcName, int size, unsigned char location = 0)
{
	T* tmp = NULL;
	if (gBMC)
	{
		tmp = (T*)gBMC->GetMemory(size);
		if (tmp)
		{
			if (gMP)
			{
				gMP->SaveInfo(dllID, tmp, size, fileName, (unsigned short)line, funcName, MEMORY_TYPE_FAST_MALLOC, location);
			}
		}
		else
		{
			tmp = new T[size];

			if (gMP)
			{
				gMP->SaveInfo(dllID, tmp, size, fileName, (unsigned short)line, funcName, MEMORY_TYPE_FAST_MALLOC, location);
			}
		}
	}
	else
	{
		tmp = new T[size];

		if (gMP)
		{
			gMP->SaveInfo(dllID, tmp, size, fileName, (unsigned short)line, funcName, MEMORY_TYPE_FAST_MALLOC, location);
		}
	}

	return tmp;
}

template <class T>
T* fast_alloc_arr(int size)
{
	if (gBMC)
	{
		T* tmp = (T*)gBMC->GetMemory(size);
		if (tmp)
		{
			return tmp;
		}
	}
	T* tmp = new T[size];
	return tmp;
}

template <class T>
void mp_manage_var(unsigned char dllID, T* var, char* fileName, int line, char* funcName)
{
	if (gMP)
	{
		gMP->SaveInfo(dllID, var, sizeof(T), fileName, (unsigned short)line, funcName, MEMORY_TYPE_NOT_ARRAY, sizeof(T));
	}
	else
	{
		assert(false);
	}
}

