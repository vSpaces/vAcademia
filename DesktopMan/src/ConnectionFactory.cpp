#include "StdAfx.h"
#include "..\include\ConnectionFactory.h"
#include "..\include\SharingConnection.h"
#include "ServiceMan.h"
#include "..\include\CSHelper.h"

CSharingConnectionFactory::CSharingConnectionFactory(SHARING_CONTEXT* aContext)
{
	context = aContext;
	csFactory.Init();
}

CSharingConnectionFactory::~CSharingConnectionFactory()
{
	csFactory.Term();
}

CSharingConnection*	CSharingConnectionFactory::GetConnectionByLocationName( LPCSTR alpcGroupName)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	CSharingConnection* connection = NULL;
	if (strcmp(alpcGroupName, ""))
	{	
		connection = mapConnections.GetObjectByName( alpcGroupName, context);
		if( !connection)
		{
			connection = mapConnections.CreateObjectByName( alpcGroupName, context);
			connection->SetConnectionName( alpcGroupName);
			context->gcontext->mpServiceMan->SendSharingServerRequest( alpcGroupName);
		}
	}
	else
	{
		connection = mapConnections.GetObjectByName( "serverConnection", context);
		if( !connection)
		{
			connection = mapConnections.CreateObjectByName( "serverConnection", context);
			connection->SetConnectionName( "serverConnection");
			context->gcontext->mpServiceMan->SendSharingServerRequest( "serverConnection");
		}
	}

	return connection;
}

bool	CSharingConnectionFactory::SetSharingServerAddress(LPCSTR alpcGroupName, LPCSTR alpcServerAddress, unsigned int aiRecordPort)
{
	if( !alpcGroupName)
		return false;
	if( *alpcGroupName == 0)
		return false;
	if( !alpcServerAddress)
		return false;
	if( *alpcServerAddress == 0)
		return false;
	if( aiRecordPort == 0)
		return false;

	csFactory.Lock();
	CSharingConnection* connection = mapConnections.GetObjectByName( alpcGroupName, context);
	ATLASSERT( connection);
	if( connection == NULL)
	{
		csFactory.Unlock();
		return false;
	}
	csFactory.Unlock();

	return connection->Connect( alpcServerAddress, aiRecordPort) != 0;
}

void	CSharingConnectionFactory::DestroyAllConnections()
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	mapConnections.DeleteAllObjects();
}