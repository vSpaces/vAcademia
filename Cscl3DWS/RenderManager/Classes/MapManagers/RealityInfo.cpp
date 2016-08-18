#include "stdafx.h"
#include "realityinfo.h"

const wchar_t* sNull = L"";

CRealityInfo::CRealityInfo()
{
	sName = NULL;
	pData = NULL;
	nGuardFlags = 0;
}

CRealityInfo::CRealityInfo(const CRealityInfo& aRealityInfo)
{
	sName = NULL;
	pData = NULL;
	Assign(aRealityInfo);
}

CRealityInfo::~CRealityInfo()
{
	SetName(0, NULL);
}

unsigned int CRealityInfo::GetRealityID() const
{
	return nRealityID;
}

unsigned int CRealityInfo::GetGuardFlags() const
{
	return nGuardFlags;
}

unsigned int CRealityInfo::GetRealityDepth() const
{
	return nRealityDepth;
}

unsigned int CRealityInfo::GetParentRealityID() const
{
	return nParentRealityID;
}

cmRealityType CRealityInfo::GetType() const
{
	return (cmRealityType) nType;
}

long long CRealityInfo::GetTime() const
{
	return nTime;
}

const wchar_t* CRealityInfo::GetName() const
{
	if (sName == NULL)
		return sNull;
	return sName;
}

void* CRealityInfo::GetData() const
{
	return pData;
}

void CRealityInfo::SetData(void* aData)
{
	pData = aData;
}

void CRealityInfo::SetRealityID(unsigned int aRealityID)
{
	nRealityID = aRealityID;
}

void CRealityInfo::SetGuardFlags( unsigned int aGuardFlags)
{
	nGuardFlags = aGuardFlags;
}

void CRealityInfo::SetDepth(unsigned int aRealityDepth)
{
	nRealityDepth = aRealityDepth;
}
void CRealityInfo::SetParentRealityID(unsigned int aParentRealityID)
{
	nParentRealityID = aParentRealityID;
}

void CRealityInfo::SetType(cmRealityType aType)
{
	nType = (unsigned char)aType;
}

void CRealityInfo::SetTime(time_t aTime)
{
	nTime = aTime;
}

void CRealityInfo::SetName(const wchar_t* aName)
{
	if (aName != NULL)
	{
		SetName(aName, (unsigned short)wcslen(aName));
	}
	else
	{
		SetName(NULL, 0);
	}
}

void CRealityInfo::SetName(const wchar_t* aNameData,  unsigned short aNameLength)
{
	if (sName != NULL)
	{
		MP_DELETE_ARR(sName);
	}
	sName = NULL;
	if (aNameLength == 0 || aNameData == NULL)
	{
		return;
	}
	sName = MP_NEW_ARR(wchar_t, aNameLength + 1);
	memcpy(sName, aNameData, aNameLength * sizeof(wchar_t));
	sName[ aNameLength] = 0;
}

CRealityInfo& CRealityInfo::operator=(const CRealityInfo& aRealityInfo)
{
	Assign(aRealityInfo);
	return *this;
}

void CRealityInfo::Assign(const CRealityInfo& aRealityInfo)
{
	SetRealityID(aRealityInfo.GetRealityID());
	SetParentRealityID(aRealityInfo.GetParentRealityID());
	SetDepth(aRealityInfo.GetRealityDepth());
	SetType(aRealityInfo.GetType());
	SetTime((time_t)aRealityInfo.GetTime());
	SetName(aRealityInfo.GetName());
	SetData(aRealityInfo.GetData());
	SetGuardFlags(aRealityInfo.GetGuardFlags());
}