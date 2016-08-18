// Errors.h
#pragma once

namespace oms
{
	class omsContext;
}

class CSharingManager;
class CSharingSessionFactory;
class CSharingConnectionFactory;
class CDestinationsFactory;

typedef struct _SHARING_CONTEXT
{
	oms::omsContext*			gcontext;
	CSharingSessionFactory*		sessionsFactory;
	CSharingConnectionFactory*	connectionsFactory;
	CDestinationsFactory*		destinationsFactory;
	CSharingManager*				sharingMan;
} SHARING_CONTEXT;