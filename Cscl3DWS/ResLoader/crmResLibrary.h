// crmResLibrary.h: interface for the crmResLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_crmResLibrary_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_)
#define AFX_crmResLibrary_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class resIResource;
#include "crmResourceFile.h"
#include "crmMemResourceFile.h"
//#include "AsynchResource.h"

//////////////////////////////////////////////////////////////////////////

#ifdef RESLOADER_EXPORTS
#define RESLOADER_API __declspec(dllexport)
#else
#define RESLOADER_API __declspec(dllimport)
#endif

#include "ifile.h"
#include "ireslibrary.h"
	
//////////////////////////////////////////////////////////////////////////
//class crmResLibrary  : public CResLibrary
class RESLOADER_API crmResLibrary  : public IResLibrary
{
	res::resIResourceManager* mpIResourceManager;
	friend class crmMemResourceFile;
	friend class crmResourceFile;
	friend class CAsynchResource;
//#ifdef _DEBUG
//	std::vector<CAsynchResource*> mvOpenedAResources; // вектор для отлова потерянных ресурсов
//	void register_resource(CAsynchResource* apRes);
//	void unregister_resource(CAsynchResource* apRes);
//	void find_lost_resources();
//#endif
public:
	crmResLibrary();
	virtual ~crmResLibrary();

	// реализация natura3d::ireslibrary
public:
	// Ansi versions
	virtual	ifile* GetResFile(const char* fileName);
	virtual	ifile* GetResFileToWrite(const char* fileName);
	virtual	ifile* GetResMemFile(const char* fileName);
	//IAsynchResource* GetAsynchResource();

	// Unicode versions
	virtual	ifile*	GetResFile(const wchar_t* fileName);
	virtual	ifile*	GetResFileToWrite(const wchar_t* fileName);
	virtual	ifile*	GetResMemFile(const wchar_t* fileName);
	IAsynchResource* GetAsynchResource();

	virtual void DeleteResFile(ifile* pf);
	void DeleteAsynchResource(IAsynchResource* piasynchresource);
	IAsyncResourcesLoadingStat*	GetResloadingStat();
	IAsynchResourceManager*	GetAsynchResMan();
	IAsynchResourceManager*	GetAsynchResManNoCreation();	
	
	void setNotSaveToCache(bool abNotSaveCache = false);
	bool LoadXml( void *apDoc, const std::string &aFileName);
	bool LoadXml( void *apDoc, const std::wstring &aFileName);

public:
	void	set_resource_manager(res::resIResourceManager* apIResourceManager);
	void	set_communication_manager(cm::cmICommunicationManager* apComManager);

private:
	void Release();

private:
	bool bNotSaveCache;
};

#endif // !defined(AFX_crmResLibrary_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_)
