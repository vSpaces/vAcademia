
#include "stdafx.h"
#include "GlobalSingletonStorage.h"
#include "GlobalInterfaceStorage.h"
#include "PlatformDependent.h"

CGlobalSingletonStorage* CGlobalSingletonStorage::GetInstance()
{
	static CGlobalSingletonStorage* obj = NULL;

	if (!obj)
	{
		obj = MP_NEW(CGlobalSingletonStorage);

//		CNEngine* eng = (CNEngine*)&obj->ne;

		/*if (!gI)
		{
			gI = CGlobalInterfaceStorage::GetInstance();
		}

		gI->SetLogWriter(&obj->lw);
		gI->SetNEngine(&obj->ne);
		gI->SetTextureManager(&obj->tm);
		gI->SetCamManager(&obj->cm);
		gI->SetFontManager(&obj->fm);
		gI->SetShaderManager(&obj->sm);
		gI->SetSpriteProcessor(&obj->sp);*/
	}

	return obj;
}

void CGlobalSingletonStorage::Destroy()
{
	//delete GetInstance();
}

CGlobalSingletonStorage::CGlobalSingletonStorage()
{
	
}

CGlobalSingletonStorage::CGlobalSingletonStorage(const CGlobalSingletonStorage&)
{

}

void CGlobalSingletonStorage::SetResLibrary(IResLibrary* reslib)
{
	rl = reslib;
}

void CGlobalSingletonStorage::SetLogWriter(ILogWriter* apLogWriter)
{
	lw.m_pLogWriter = apLogWriter;
}

CGlobalSingletonStorage& CGlobalSingletonStorage::operator=(const CGlobalSingletonStorage&)
{
#pragma warning(push)
#pragma warning(disable : 4239)
#pragma warning(disable : 4172)
	return CGlobalSingletonStorage::CGlobalSingletonStorage();
#pragma warning(pop)
}

CGlobalSingletonStorage::~CGlobalSingletonStorage()
{
	
}

