
#include "StdAfx.h"
#include "Video.h"
#include "VideoPlugin.h"

IVideo* CreateVideoObject()
{
	CVideo* video = new CVideo();

	return video;
}

void DeleteVideoObject(IVideo* video)
{
	if (video)
	{
		CVideo* _video = (CVideo*)video;
		delete _video;
	}
}