
#include "StdAfx.h"
#include "nrmCamera.h"
#include "OculusRift.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"

COculusRift::COculusRift():
	m_oculus(NULL),
	m_isCtrlFound(true)
{
}

void COculusRift::Update()
{	
}

void COculusRift::OnStart()
{
}

void COculusRift::OnStop()
{
}

IOculusRiftSupport* COculusRift::GetOculusObject(IOculusRiftSupport::InitError& initError)
{
	if (m_oculus)
	{
		return m_oculus;
	}

	unsigned int major, minor, build;
	g->ci.GetOSVersion(&major, &minor, &build);
	if (major < 6)
	{
		g->lw.WriteLn("Oculus Rift is not supported on Windows XP");
		return NULL;
	}

	g->lw.WriteLn("Checking Oculus Rift");

	std::wstring path = GetApplicationRootDirectory();
	path += L"oculus_rift_support.dll";
	HMODULE hDll = ::LoadLibraryW(path.c_str());

	if (hDll != NULL)
	{
		IOculusRiftSupport* (*CreateOculusRiftObject)(IOculusRiftSupport::InitError &initError);
		(FARPROC&)CreateOculusRiftObject = GetProcAddress(hDll, "CreateOculusRiftObject");
		if (CreateOculusRiftObject == NULL)
		{
			g->lw.WriteLn("Oculus Rift not found");
			FreeLibrary(hDll);	
			return NULL;
		}
		else
		{
			m_oculus = CreateOculusRiftObject(initError);
			if (m_oculus)
			{
				g->lw.WriteLn("Oculus Rift found");							
			}
			else
			{
				g->lw.WriteLn("Oculus Rift not found");
				FreeLibrary(hDll);	
				return NULL;
			}
		}
	}
	else
	{
		return NULL;
	}


	CHDRPostEffect* postEffect = dynamic_cast<CHDRPostEffect*>(g->scm.GetActiveScene()->GetPostEffect());
	if (!postEffect)
	{
		return NULL;
	}

	g->sp.CreateOculusDistortionMesh(m_oculus->GetDistortionMesh(), postEffect->GetOculusRiftShader());
	m_oculus->DestroyDistortionMesh();	

	return m_oculus;
}

COculusRift::~COculusRift()
{
	if (m_oculus) {
		delete m_oculus;
	}
}