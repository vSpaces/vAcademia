#pragma once

typedef struct SUserInfo
{
	MP_WSTRING userName;
	MP_WSTRING firstName;
	MP_WSTRING lastName;
	int x, y;

public:
	SUserInfo():
	  MP_WSTRING_INIT(userName),
	  MP_WSTRING_INIT(firstName),
	  MP_WSTRING_INIT(lastName)
	{

	}
} _SUserInfo;

namespace SyncManProtocol
{
	class CGetUserListIn : public CCommonPacketIn
	{
	protected:
		unsigned int m_realityID;
		unsigned short m_userCount;

		SUserInfo* m_userList;

	public:
		CGetUserListIn( BYTE *aData, int aDataSize);
		~CGetUserListIn();

		bool Analyse();

		void CleanUp();

		const unsigned int GetRealityID()const;
		const unsigned short GetUserCount()const;
		const SUserInfo* GetUser(unsigned int id)const;
		SUserInfo* GetUserList()const;
	};
};