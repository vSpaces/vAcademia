#pragma once

#include <map>
#include "ObjectMapByName.h"
#include "SharingContext.h"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class CSharingConnection;

//////////////////////////////////////////////////////////////////////////
class DESKTOPMAN_API CSharingConnectionFactory
{
public:
	CSharingConnectionFactory(SHARING_CONTEXT* aContext);
	virtual ~CSharingConnectionFactory();

public:
	CSharingConnection*	GetConnectionByLocationName( LPCSTR alpcGroupName);
	//! Устанавливает требуемые параметры подключения к определенной комнате
	bool	SetSharingServerAddress(LPCSTR alpcGroupName
		, LPCSTR alpcServerAddress, unsigned int aiRecordPort);
	void	DestroyAllConnections();

private:
	// Play connections
	CObjectMapByName<CSharingConnection>		mapConnections;
	CCriticalSection	csFactory;

	SHARING_CONTEXT*	context;
};