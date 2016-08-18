#include "stdafx.h"
#include "PipeProtocol/CommonPipePacketOut.h"

CCommonPipePacketOut::CCommonPipePacketOut( byte aID)
{
    MP_NEW_V(data, CDataStorage, 4);
	data->Add(aID);
}

CCommonPipePacketOut::~CCommonPipePacketOut()
{
	data = NULL;
	//MP_DELETE( data);
}

CDataStorage *CCommonPipePacketOut::GetDataStorage()
{
	return data;
}