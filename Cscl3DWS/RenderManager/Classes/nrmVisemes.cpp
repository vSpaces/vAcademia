// nrmVisemes.cpp: implementation of the nrmVisemes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmVisemes.h"

nrmVisemes::nrmVisemes(mlMedia* apMLMedia) : nrmObject(apMLMedia)
{
/*	piloadable = NULL;
	pivisemes = NULL;*/
	m_motion = NULL;
}

nrmVisemes::~nrmVisemes()
{
	/*if( piloadable)
	{
		ibase*	apibase = NULL;
		if( GET_INTERFACE(ibase, picontroller, &apibase))
			get_iobjslib()->destroy_object(apibase);
		piloadable = NULL;
	}*/
}

// загрузка нового ресурса
bool nrmVisemes::SrcChanged()
{
	/*USES_CONVERSION;
	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	if(!pwcSrc) return false;
	mlString sSRC=pwcSrc; 
	if(sSRC.empty()) return false;
	//
	piloadable = get_iobjslib()->create_visemes();
	GET_INTERFACE(ibase, piloadable, &pibase);
	ATLASSERT( pibase);
	if( piloadable->load(W2A(pwcSrc)))
	{
		GET_INTERFACE(ivisemes, piloadable, &pivisemes);
	}
	else
	{
		get_iobjslib()->destroy_object(pibase);
		piloadable = NULL;
		pibase = NULL;
		rm_trace("Can`t load visemes: %s\n", W2A(pwcSrc));
		return false;
	}*/
	return true;
}
