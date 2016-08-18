
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "GlobalInterfaceStorage.h"
#include "EngineInit.h"

CGlobalInterfaceStorage* gI = NULL;
CGlobalSingletonStorage* g = NULL;

void InitEngine()
{
	gI = CGlobalInterfaceStorage::GetInstance();
	g = CGlobalSingletonStorage::GetInstance();
}