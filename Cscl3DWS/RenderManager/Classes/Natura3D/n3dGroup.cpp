// n3dGroup.cpp: implementation of the cn3dGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "n3dGroup.h"
#include "nrm3DGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cn3dGroup::cn3dGroup(nrm3DGroup* pc)
{
	prmGroup = pc;
}

cn3dGroup::~cn3dGroup()
{

}


/*void	cn3dGroup::post_update(DWORD time, LPVOID value, ibase* apivisibleobject)
{
	if( prmGroup)	prmGroup->update(time, value);
}

bool	cn3dGroup::check_support(ibase* apibase)
{
	void*	interfaces;
	if( !apibase)	return false;
	if( !apibase->query_interface(iid_ivisibleobject, &interfaces))	return false;
	return true;
}

void	cn3dGroup::is_registered(ibase* apibase)
{
}

int		cn3dGroup::get_supported_interfaces(N3DGUID* pinterfaces, int maxCount)
{
	if( maxCount < 1)	return 0;
	pinterfaces[0] = iid_iplugupdatable;
}*/