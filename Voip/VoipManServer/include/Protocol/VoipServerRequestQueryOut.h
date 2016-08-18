#pragma once
#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

/************************************************************************/
/* Запрашивает адреса серверов для определенной локации
/************************************************************************/
class CVoipServerRequestQueryOut : public CCommonVoipPacketOut
{
public:
	CVoipServerRequestQueryOut( LPCSTR alpcRoomName);
};