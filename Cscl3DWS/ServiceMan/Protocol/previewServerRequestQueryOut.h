#pragma once
#include "CommonPacketOut.h"

/************************************************************************/
/* Запрашивает адреса серверов для определенной локации
/************************************************************************/

using namespace Protocol;

class CPreviewServerRequestQueryOut : public CCommonPacketOut
{
public:
	CPreviewServerRequestQueryOut( unsigned int aiRecordID);
};