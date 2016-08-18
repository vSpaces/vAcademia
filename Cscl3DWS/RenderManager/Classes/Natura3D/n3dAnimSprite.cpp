// n3dAnimSprite.cpp: implementation of the cn3dAnimSprite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "n3dAnimSprite.h"
#include "nrm3DObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cn3dAnimSprite::cn3dAnimSprite(nrmObject* pc)
{
	cFrame = -1;
	prmObject = pc;
}

cn3dAnimSprite::~cn3dAnimSprite()
{

}

void	cn3dAnimSprite::post_update(DWORD time, LPVOID value, ivisibleobject* apivisibleobject)
{
	iplayable	*piplayable;
	GET_INTERFACE(iplayable, apivisibleobject, &piplayable);
	if( piplayable==NULL)	return;
	if( piplayable->get_current_frame() != cFrame)
	{
		cFrame = piplayable->get_current_frame();
		prmObject->GetMLMedia()->GetIContinuous()->SetCurFrame(cFrame);
		prmObject->GetMLMedia()->GetIContinuous()->onEnterFrame();
		if( cFrame == piplayable->get_frames_count())
			prmObject->GetMLMedia()->GetIContinuous()->onPlayed();
	}	
}
