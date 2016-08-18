#include "stdafx.h"
#include "PipeProtocol/CommonPipePacketIn.h"


CCommonPipePacketIn::CCommonPipePacketIn( BYTE *aData, int aDataSize) 
{
	MP_NEW_V2(data, CDataStorage, aData, aDataSize);	
}

CCommonPipePacketIn::~CCommonPipePacketIn()
{
	MP_DELETE( data);
}

bool CCommonPipePacketIn::Analyse()
{
	data->Reset();
	if(!data->Read( id))
		return false;
	return true;
}

int	CCommonPipePacketIn::GetID()
{
	return id;
}

bool CCommonPipePacketIn::EndOfData()
{
	if( data->GetRealDataSize() != data->GetSize())
		return false;
	return true;
}

/*CDataStorage *CCommonPipePacketIn::GetDataStorage()
{
	return data;
}*/