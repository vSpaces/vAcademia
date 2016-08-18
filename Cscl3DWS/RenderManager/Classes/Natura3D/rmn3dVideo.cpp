// n3dVideo.cpp: implementation of the cn3dVideo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "rmn3dVideo.h"
#include "nrmObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define	VIDEOTIMEEFFECTOR	10000

cn3dVideo::cn3dVideo(nrmObject* pc)
{
	position = -1;
	prmObject = pc;
}

cn3dVideo::~cn3dVideo()
{

}

/*void cn3dVideo::post_update(DWORD time, LPVOID value, ibase* apivisibleobject)
{
	iplayable	*piplayable;
	GET_INTERFACE(iplayable, apivisibleobject, &piplayable);
	if( piplayable==NULL)	return;

	ivideo	*pivideo;
	GET_INTERFACE(ivideo, apivisibleobject, &pivideo);
	if( piplayable==NULL)	return;

	LONGLONG	cp;
	if( (cp=piplayable->get_current_ms()) != position)
	{
		position = cp;
		prmObject->GetMLMedia()->GetIContinuous()->SetCurPos(cp/VIDEOTIMEEFFECTOR);

	}
	if( pivideo->is_just_played())
	{
		piplayable->stop();
		prmObject->GetMLMedia()->GetIContinuous()->onPlayed();
	}
}

bool	cn3dVideo::check_support(ibase* apibase)
{
	void*	interfaces;
	if( !apibase)	return false;
	if( !apibase->query_interface(iid_iplayable, &interfaces))	return false;
	return true;
}

void	cn3dVideo::is_registered(ibase* apibase)
{
}



int		cn3dVideo::get_supported_interfaces(N3DGUID* pinterfaces, int maxCount)
{
	if( maxCount < 1)	return 0;
	pinterfaces[0] = iid_iplugupdatable;
}*/