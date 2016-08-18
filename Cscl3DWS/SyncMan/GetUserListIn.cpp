#include "stdafx.h"
#include "GetUserListIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CGetUserListIn::CGetUserListIn(BYTE *apData, int apDataSize):
	CCommonPacketIn( apData, apDataSize),
	m_userList(NULL),
	m_realityID(0),
	m_userCount(0)	
{
}

bool CGetUserListIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
	{
		return false;
	}

	CleanUp();

	int idx=0;
	if (!memparse_object(pData, idx, m_realityID))
	{
		return false;
	}
	if (!memparse_object(pData, idx, m_userCount))
	{
		return false;	
	}

	m_userList = MP_NEW_ARR( SUserInfo, m_userCount);

	for (int i = 0; i < m_userCount; i++)
	{
		if (!memparse_string(pData, idx, m_userList[i].userName))
		{
			return false;
		}
		if (!memparse_string(pData, idx, m_userList[i].firstName))
		{
			return false;
		}
		if (!memparse_string(pData, idx, m_userList[i].lastName))
		{
			return false;
		}
		if (!memparse_object(pData, idx, m_userList[i].x))
		{
			return false;
		}
		if (!memparse_object(pData, idx, m_userList[i].y))
		{
			return false;
		}
	}

	return EndOfData( idx);
}

const unsigned int CGetUserListIn::GetRealityID()const
{
	return m_realityID;
}

SUserInfo* CGetUserListIn::GetUserList()const
{
	return m_userList;
}

const unsigned short CGetUserListIn::GetUserCount()const
{
	return m_userCount;
}

const SUserInfo* CGetUserListIn::GetUser(unsigned int id)const
{
	if (id < m_userCount)
	{
		return &m_userList[id];
	}
	else
	{
		return NULL;
	}
}

void CGetUserListIn::CleanUp()
{
	if (m_userList != NULL)
	{
		MP_DELETE_ARR( m_userList);
		m_userList = NULL;
		m_userCount = 0;
	}
}

CGetUserListIn::~CGetUserListIn()
{
	CleanUp();
}