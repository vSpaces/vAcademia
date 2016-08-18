
#include "StdAfx.h"
#include "Kinect.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"

CKinect::CKinect():
	m_kinect(NULL)
{
}

IKinect* CKinect::GetKinectObject(C3DObject* mainObject)
{
	if (!mainObject)
	{
		return NULL;
	}

	if (!mainObject->GetAnimation())
	{
		return NULL;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)mainObject->GetAnimation();

	if (m_kinect)
	{
		sao->SetKinectObject(m_kinect);
		return m_kinect;
	}

	std::wstring path = GetApplicationRootDirectory();
	path += L"kinect_plugin.dll";

	//при установке своего аватара поднимаем кинект и ссылку на кинект-объект даем скелетному объекту у своего аватара

	HMODULE hDll = ::LoadLibraryW(path.c_str());

	if (hDll == NULL)
	{		
		g->lw.WriteLn("kinect_plugin.dll not found");
	}
	else
	{
		IKinect* (*CreateKinectObject)();
		(FARPROC&)CreateKinectObject = GetProcAddress(hDll, "CreateKinectObject");
		if( CreateKinectObject == NULL)
		{
			FreeLibrary(hDll);
			g->lw.WriteLn("Plugin doesn`t implement main function [CreateKinectObject] \n");
		}
		else
		{
			IKinect* kinect = CreateKinectObject();
			if (kinect->Init())
			{				
				if (sao)
				{
					sao->SetKinectObject(kinect);								
					m_kinect = kinect;
					return kinect;
				}
				else
				{
					FreeLibrary(hDll);
				}
			}
			else
			{
				FreeLibrary(hDll);
			}
		}
	}

	return NULL;
}

CKinect::~CKinect()
{
}
