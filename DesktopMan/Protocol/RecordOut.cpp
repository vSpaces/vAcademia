#include "StdAfx.h"
#include ".\RecordOut.h"

using namespace SharingManProtocol;

RecordOut::RecordOut(unsigned short sessionID, unsigned int aPlayRecordID, unsigned int aRecordID)
{
	GetDataBuffer().reset();
	GetDataBuffer().add<bool>( true);
	GetDataBuffer().add<unsigned short>( sessionID);
	GetDataBuffer().add<unsigned int>( aRecordID);
	GetDataBuffer().add<bool>( true);
}
RecordOut::RecordOut(unsigned short sessionID, unsigned int aRecordID, LPCSTR alpFileName)
{
	GetDataBuffer().reset();
	GetDataBuffer().add<bool>( true);
	GetDataBuffer().add<unsigned short>( sessionID);
	GetDataBuffer().add<unsigned int>( aRecordID);
	GetDataBuffer().add<bool>( false);
	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpFileName));
}
RecordOut::RecordOut(unsigned short sessionID)
{
	GetDataBuffer().reset();
	GetDataBuffer().add<bool>( false);
	GetDataBuffer().add<unsigned short>( sessionID);
}

RecordOut::~RecordOut(void)
{
}
