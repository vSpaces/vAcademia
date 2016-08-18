// VoipManClientTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "filesys.h"

HMODULE	hVoipModule;
std::auto_ptr<oms::omsContext>	pOMSContext;

HMODULE LoadDLL( LPCSTR alpcRelPath, LPCSTR alpcName)
{
	CComString sModuleDirectory;
	GetModuleFileName( NULL, sModuleDirectory.GetBufferSetLength(MAX_PATH), MAX_PATH);

	CComString sCurrentDirectory;
	GetCurrentDirectory( MAX_PATH, sCurrentDirectory.GetBufferSetLength(MAX_PATH));

	SECFileSystem fs;
	SetCurrentDirectory( fs.GetPath(sModuleDirectory) + alpcRelPath);
	HMODULE handle = ::LoadLibrary( alpcName);
	SetCurrentDirectory( sCurrentDirectory);
	return handle;
}

void FreeLibrary(unsigned long auHModule){
	::FreeLibrary((HMODULE)auHModule);
}

void* GetProcAddress(unsigned long auHModule, const char* apcProcName){
	if(auHModule == 0)
		return NULL;
	return ::GetProcAddress((HMODULE)auHModule,apcProcName);
}

void CreateVoipManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;
	ATLASSERT( !context->mpVoipMan);
	if( context->mpVoipMan)
		return;

#ifdef DEBUG
#define	VOIP_MANAGER_DLL	"VoipManClient.dll"
#else
#define	VOIP_MANAGER_DLL	"VoipManClient.dll"
#endif

	if( !hVoipModule)
		hVoipModule = LoadDLL( "voip\\", VOIP_MANAGER_DLL);

	if( hVoipModule)
	{
		omsresult (*lpCreateVoipManagerClient)( oms::omsContext* aContext);
		(FARPROC&)lpCreateVoipManagerClient = (FARPROC)GetProcAddress( (unsigned long)hVoipModule, "CreateVoipManagerClient");
		if( lpCreateVoipManagerClient)
			lpCreateVoipManagerClient( context);
		if( !context->mpVoipMan)
		{
			FreeLibrary((unsigned long)hVoipModule);
			hVoipModule = 0;
		}
	}
}

void DestroyVoipManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;

	if( !hVoipModule)
		return;

	omsresult (*lpDestroyVoipManager)( oms::omsContext* aContext);
	(FARPROC&)lpDestroyVoipManager = (FARPROC)GetProcAddress( (unsigned long)hVoipModule, "DestroyVoipManagerClient");
	if( lpDestroyVoipManager)
		lpDestroyVoipManager( context);
	FreeLibrary( (unsigned long)hVoipModule);
}

int _tmain(int argc, _TCHAR* argv[])
{
	pOMSContext = std::auto_ptr<oms::omsContext>(new oms::omsContext);
	CreateVoipManagerImpl( pOMSContext.get());
	getchar();
	DestroyVoipManagerImpl( pOMSContext.get());
	return 0;
}

