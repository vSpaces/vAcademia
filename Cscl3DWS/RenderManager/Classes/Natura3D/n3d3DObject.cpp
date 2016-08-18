// n3d3DObject.cpp: implementation of the cn3DObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "n3d3DObject.h"
#include "nrm3DObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cn3DObject::cn3DObject(nrm3DObject* pc)
{
	prmObject = pc;
}

cn3DObject::~cn3DObject()
{

}

//void cn3DObject::update(DWORD	dwTime, void* data)
//{
//	C3DObject::update(dwTime, data);
//	if( prmObject)	prmObject->update(dwTime, data);
//}

/*void	cn3DObject::post_update(DWORD time, LPVOID value, ibase* apivisibleobject)
{
	if( prmObject)	prmObject->update(time, value);
}

bool	cn3DObject::check_support(ibase* apibase)
{
	void*	interfaces;
	if( !apibase)	return false;
	if( !apibase->query_interface(iid_ivisibleobject, &interfaces))	return false;
	return true;
}

void	cn3DObject::is_registered(ibase* apibase)
{
}

int		cn3DObject::get_supported_interfaces(N3DGUID* pinterfaces, int maxCount)
{
	if( maxCount < 1)	return 0;
	pinterfaces[0] = iid_iplugupdatable;
}*/