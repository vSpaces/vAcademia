#pragma once

struct ifile;
#include "iasynchresource.h"
#include "iasynchresourcehandler.h"

struct IAsynchResourceManager;
struct IAsyncResourcesLoadingStat;

struct IResLibrary
{
	// Ansi versions
	virtual	ifile*	GetResFile(const char* fileName) = 0;
	virtual	ifile*	GetResFileToWrite(const char* fileName) = 0;
	virtual	ifile*	GetResMemFile(const char* fileName) = 0;
	//virtual	IAsynchResource* GetAsynchResource() = 0;

	// Unicode versions
	virtual	ifile*	GetResFile(const wchar_t* fileName) = 0;
	virtual	ifile*	GetResFileToWrite(const wchar_t* fileName) = 0;
	virtual	ifile*	GetResMemFile(const wchar_t* fileName) = 0;
	virtual	IAsynchResource* GetAsynchResource() = 0;

	virtual void	DeleteResFile(ifile* pf) = 0;
	virtual void	DeleteAsynchResource(IAsynchResource* piasynchresource) = 0;
	virtual IAsyncResourcesLoadingStat*	GetResloadingStat() = 0;
	virtual IAsynchResourceManager*	GetAsynchResMan() = 0;
	virtual void setNotSaveToCache(bool abNotSaveCache = false) = 0;
	virtual bool LoadXml( void *apDoc, const std::string &aFileName) = 0;
	virtual bool LoadXml( void *apDoc, const std::wstring &aFileName) = 0;
};