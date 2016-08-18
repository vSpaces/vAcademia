#pragma once

#include "oms_context.h"

using namespace cm;

class CRealityInfo :
	public cmIRealityInfo
{
public:
	CRealityInfo();
	CRealityInfo( const CRealityInfo& aRealityInfo);
	virtual ~CRealityInfo();

	virtual unsigned int GetRealityID() const;
	virtual unsigned int GetGuardFlags() const;
	virtual unsigned int GetParentRealityID() const;
	virtual unsigned int GetRealityDepth() const;
	virtual cmRealityType GetType() const;
	virtual long long GetTime() const;
	virtual const wchar_t* GetName() const;
	virtual void* GetData() const;
	virtual void SetData( void* pData);

	void SetRealityID( unsigned int aRealityID);
	void SetGuardFlags( unsigned int aGuardFlags);
	void SetDepth(unsigned int aRealityDepth);
	void SetParentRealityID( unsigned int aParentRealityID);
	void SetType( cmRealityType aType);
	void SetTime( time_t aTime);
	void SetName( const wchar_t* aName);
	void SetName( const wchar_t* aNameData, unsigned short aNameLength);

	CRealityInfo& operator=( const CRealityInfo& aRealityInfo);

private:
	void Assign( const CRealityInfo& aRealityInfo);

private:
	unsigned int nRealityID;
	unsigned int nGuardFlags;
	unsigned int nRealityDepth;
	unsigned int nParentRealityID;
	unsigned char nType;
	time_t nTime;
	wchar_t* sName;
	void* pData;
};
