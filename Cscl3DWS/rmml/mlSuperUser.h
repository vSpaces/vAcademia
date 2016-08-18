#pragma once

class mlSuperUser
{
public:
	mlSuperUser();
	~mlSuperUser();

	bool IsCurrentUserSuperUser(const wchar_t* login)const;

private:
	MP_VECTOR<MP_WSTRING> m_superUsers;
};