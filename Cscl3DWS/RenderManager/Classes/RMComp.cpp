// RMComp.cpp: implementation of the CRMCompLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RMComp.h"
#include "nrmViewport.h"
#include "nrmAudio.h"
#include "nrm3DScene.h"

CRMCompLib::CRMCompLib(CNRManager*	apRenderManager):
	MP_VECTOR_INIT(rmViewports),
	MP_VECTOR_INIT(rmScenes),
	MP_VECTOR_INIT(rmAudios)
{
	m_pRenderManager = apRenderManager;
}

CRMCompLib::~CRMCompLib()
{
	rmViewports.clear();	
	rmScenes.clear();
	rmAudios.clear();
}

/************************************************************************/
/* Main initialization procedure.
/* Initialize and bind:
/*	1. camera to viewports
/*		// ??
/************************************************************************/
void CRMCompLib::init()
{
	if( rmViewports.size() > 0)
	{
		for (unsigned int ivp = 0; ivp < rmViewports.size(); ivp++)
		{
			nrmViewport* viewport = (nrmViewport*)rmViewports[ivp];
			viewport->CameraChanged();
		}
	}
}

omsresult CRMCompLib::AddObject(mlMedia *apMLMedia)
{
	if( !apMLMedia)	return OMS_ERROR_FAILURE;
	moMedia*	apMOMedia = apMLMedia->GetSafeMO();
	
	if(apMLMedia->GetType()==MLMT_VIEWPORT){	rmViewports.push_back(apMOMedia);}
	else	if(apMLMedia->GetType()==MLMT_SCENE3D){	rmScenes.push_back(apMOMedia);}
	else	if(apMLMedia->GetType()==MLMT_AUDIO)
	{	
		rmAudios.push_back(apMOMedia);
		((nrmObject*)apMOMedia)->SetCompLib(this);
	}
	return	OMS_OK;
}

omsresult CRMCompLib::RemoveObject(mlMedia *apMLMedia)
{
	if (!apMLMedia)
	{
		return OMS_ERROR_ILLEGAL_VALUE;
	}

	if(apMLMedia->GetType()==MLMT_IMAGE){	return	OMS_ERROR_ILLEGAL_VALUE;}
	if(apMLMedia->GetType()==MLMT_MOTION){	return	OMS_ERROR_ILLEGAL_VALUE;}
	if(apMLMedia->GetType()==MLMT_VISEMES){	return	OMS_ERROR_ILLEGAL_VALUE;}
	if(apMLMedia->GetType()==MLMT_GROUP){	return	OMS_ERROR_ILLEGAL_VALUE;}
	
	moMedia*	pMO = apMLMedia->GetSafeMO();
	if( !pMO)	return OMS_OK;

	std::vector<moMedia*>*	pvec=NULL;
	switch(apMLMedia->GetType()) {
	case MLMT_VIEWPORT:
		pvec = &rmViewports;
		break;
	case MLMT_SCENE3D:
		pvec = &rmScenes;
		break;
	case MLMT_AUDIO:
		pvec = &rmAudios;
		break;
	}	

	if( pvec)
	{
		std::vector<moMedia*>::iterator	it;
		for (it=pvec->begin();it!=pvec->end();it++)
			if ((*it) == pMO)
			{
				pvec->erase(it);
				return OMS_OK;
			}
	}

	return OMS_ERROR_FAILURE;
}

void CRMCompLib::update(DWORD delta)
{
	std::vector<moMedia*>::iterator	it;
	for (it=rmAudios.begin();it!=rmAudios.end();it++)
		((nrmAudio*)(*it))->update(delta);

	for (it=rmScenes.begin();it!=rmScenes.end();it++)
		((nrm3DScene*)(*it))->update(delta);
}