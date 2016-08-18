// oculus_rift_support.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Oculus.h"

IOculusRiftSupport* CreateOculusRiftObject(IOculusRiftSupport::InitError& initError)
{
	COculus* oculus = new COculus();
	if (oculus->Start(initError))
	{
		return oculus;
	}
	else
	{
		delete oculus;
		return NULL;
	}
}
