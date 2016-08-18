#include "StdAfx.h"
#include ".\deleterecord.h"

using namespace SharingManProtocol;

CDeleteRecord::CDeleteRecord(unsigned int aRecordID)
{
	GetDataBuffer().reset();

	GetDataBuffer().add<unsigned int>( aRecordID);
}