#pragma once

#ifdef RESLOADER_EXPORTS
#define RESLOADER_API __declspec(dllexport)
#else
#define RESLOADER_API __declspec(dllimport)
#endif

#include "ILogWriter.h"

class RESLOADER_API CObjStorage
{
public:
	static CObjStorage* GetInstance();

	void SetResMan(res::resIResourceManager* resourceManager);
	void SetComMan(cm::cmICommunicationManager* communicationManager);
	void SetILogWriter(ILogWriter* alw);
	
	res::resIResourceManager* GetResMan();
	cm::cmICommunicationManager* GetComMan();
	ILogWriter* GetILogWriter();

private:
	res::resIResourceManager* m_pResourceManager;
	cm::cmICommunicationManager* m_pCommunicationManager;
	ILogWriter* m_lw;

public:
	CObjStorage();
	~CObjStorage();
private:
	CObjStorage(const CObjStorage&);
};