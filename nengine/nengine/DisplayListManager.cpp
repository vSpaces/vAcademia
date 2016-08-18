
#include "stdafx.h"
#include "DisplayListManager.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CDisplayListManager::CDisplayListManager(void)
{
	ReserveMemory(10);
	m_lastDisplayListNum = 10000;
}

CDisplayListManager::~CDisplayListManager(void)
{
}

void CDisplayListManager::CallDisplayList(int num)
{
	CDisplayList* dl = GetObjectPointer(num);

	g->stp.PrepareForRender();
	GLFUNC(glCallList)(dl->GetGLNumber());
}

int CDisplayListManager::CreateDisplayList(std::string name)
{
	int num = AddObject(name);
	
	CDisplayList* dl = GetObjectPointer(num);

	dl->SetGLNumber(m_lastDisplayListNum++);

	return num;
}

void CDisplayListManager::BindDisplayList(int num)
{
	CDisplayList* dl = GetObjectPointer(num);

	GLFUNC(glNewList)(dl->GetGLNumber(), GL_COMPILE);
}

void CDisplayListManager::UnbindDisplayList()
{
	GLFUNC(glEndList)();
}