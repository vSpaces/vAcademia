
#include "StdAfx.h"
#include "res.h"
#include "crmResLibrary.h"
#include "AsynchResource.h"
#include "ObjStorage.h"
#include "crmResourceFile.h"
#include "AsynchResourceManager.h"
#include "../nengine/include/TinyXML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace res;

crmResLibrary::crmResLibrary():
	mpIResourceManager(NULL)
{
	bNotSaveCache = false;
}

crmResLibrary::~crmResLibrary()
{
	Release();
	//_CrtDumpMemoryLeaks();
}

void crmResLibrary::Release()
{
	if (GetAsynchResManNoCreation())
	{
		CAsynchResourceManager::ReleaseInstance();
	}
}

void crmResLibrary::setNotSaveToCache(bool abNotSaveCache)
{
	bNotSaveCache = abNotSaveCache;
}

ifile* crmResLibrary::GetResFile(const char* fileName)
{	
	USES_CONVERSION;
	return GetResFile( A2W( fileName));
}

ifile*	crmResLibrary::GetResFile(const wchar_t* fileName)
{
	MP_NEW_P( resFile, crmResourceFile, this);

	if( resFile->Open(fileName, RES_LOCAL))
		return resFile;

	if( resFile->Open(fileName))
		return resFile;

	MP_DELETE( resFile);
	return NULL;
}

ifile* crmResLibrary::GetResFileToWrite(const char* fileName)
{	
	USES_CONVERSION;
	return GetResFileToWrite( A2W(fileName));
}

ifile*	crmResLibrary::GetResFileToWrite(const wchar_t* fileName)
{
	wchar_t*	pwcSrcFull = mpIResourceManager->GetFullPath(fileName);

	MP_NEW_P( resFile, crmResourceFile, this);
	if (resFile->Open( pwcSrcFull, RES_TO_WRITE|RES_CREATE))
	{
		return resFile;
	}

	MP_DELETE( resFile);

	return NULL;
}


ifile* crmResLibrary::GetResMemFile(const char* fileName)
{	
	USES_CONVERSION;
	return GetResMemFile( A2W( fileName));
}

ifile*	crmResLibrary::GetResMemFile(const wchar_t* fileName)
{
	MP_NEW_P( resFile, crmMemResourceFile, this);
	if( resFile->Open(fileName))
		return resFile;
	MP_DELETE( resFile);
	return NULL;
}

/*IAsynchResource* crmResLibrary::GetAsynchResource()
{
	MP_NEW_P2( file, CAsynchResource, this, bNotSaveCache);
	return (IAsynchResource*)file;
}*/

IAsynchResource* crmResLibrary::GetAsynchResource()
{
	MP_NEW_P2( file, CAsynchResource, this, bNotSaveCache);
	return (IAsynchResource*)file;
}

void crmResLibrary::DeleteResFile(ifile* pf)
{			
	if (pf != NULL)
	{
		pf->release();
	}	
}

void crmResLibrary::DeleteAsynchResource(IAsynchResource* piasynchresource)
{
	CAsynchResource *pasynchresource = (CAsynchResource*)piasynchresource;
	MP_DELETE( pasynchresource);
}

void	crmResLibrary::set_resource_manager(resIResourceManager* apIResourceManager)
{
	mpIResourceManager = apIResourceManager;
	mpIResourceManager->SetILogWriter(CObjStorage::GetInstance()->GetILogWriter());
	CObjStorage::GetInstance()->SetResMan(apIResourceManager);
	/*mpIResourceManager = apIResourceManager;
	((CAsynchResourceManager*)GetAsynchResMan())->SetResManager(apIResourceManager);*/
}

void crmResLibrary::set_communication_manager(cm::cmICommunicationManager* apComManager)
{
	CObjStorage::GetInstance()->SetComMan(apComManager);
}

IAsynchResourceManager*	crmResLibrary::GetAsynchResMan()
{
	return (IAsynchResourceManager*)CAsynchResourceManager::GetInstance();
}

IAsyncResourcesLoadingStat*	crmResLibrary::GetResloadingStat()
{
	return (IAsyncResourcesLoadingStat*)CAsynchResourceManager::GetInstance();
}

IAsynchResourceManager*	crmResLibrary::GetAsynchResManNoCreation()
{
	return (IAsynchResourceManager*)CAsynchResourceManager::GetInstanceNoCreation();
}

bool crmResLibrary::LoadXml( void *apDoc, const std::string &aFileName)
{
	USES_CONVERSION;
	std::wstring wFileName = A2W( aFileName.c_str());
	return LoadXml( apDoc, wFileName);
}

bool crmResLibrary::LoadXml( void *apDoc, const std::wstring &aFileName)
{
	bool isLoaded = false;
	//TiXmlDocument doc( W2A(fileName.c_str()));
	ifile* file = GetResMemFile( aFileName.c_str());
	if (file)
	{
		unsigned char* data = MP_NEW_ARR(unsigned char,file->get_file_size() + 1 );//new unsigned char[file->get_file_size() + 1];
		data[file->get_file_size()] = 0;
		file->read(data, file->get_file_size());
		((TiXmlDocument*)apDoc)->Parse((const char*)data);
		isLoaded = true;
		DeleteResFile(file);

		MP_DELETE_ARR(data);//delete[] data;
	}
	return isLoaded;
}