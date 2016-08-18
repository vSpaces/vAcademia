
#pragma once

#include "mlRMML.h"
#include "mlSuperUser.h"

mlSuperUser::mlSuperUser():
	MP_VECTOR_INIT(m_superUsers)
{
	m_superUsers.push_back(MAKE_MP_WSTRING(L"nap"));
	m_superUsers.push_back(MAKE_MP_WSTRING(L"nap1"));
	m_superUsers.push_back(MAKE_MP_WSTRING(L"superadmin"));
}

bool mlSuperUser::IsCurrentUserSuperUser(const wchar_t* login)const
{
	MP_VECTOR<MP_WSTRING>::const_iterator it = m_superUsers.begin();
	MP_VECTOR<MP_WSTRING>::const_iterator itEnd = m_superUsers.end();

	for ( ; it != itEnd; it++)
	if (_wcsicmp((*it).c_str(), login) == 0)
	{
		return true;
	}

	return false;
}

mlSuperUser::~mlSuperUser()
{
}