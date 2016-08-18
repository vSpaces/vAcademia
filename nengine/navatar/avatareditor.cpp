
#include "stdafx.h"
#include "FakeObject.h"
#include "common.h"
#include "plugbase.h"

CPlugBase* plugin = NULL;
CRITICAL_SECTION csInit;

bool CreateAvatarEditorInstance(IScriptCaller* rmpluginresponder, IRMBasePlugin** rmbaseplugin)
{
	if (!plugin)
	{
		plugin = MP_NEW(CPlugBase);
		InitializeCriticalSection(&csInit);
	}

	if (plugin->is_registered_already(rmpluginresponder))
	{
		return false;
	}

	EnterCriticalSection(&csInit);
	*rmbaseplugin = MP_NEW(CAvatarObject);
	plugin->add_plugin(rmpluginresponder, *rmbaseplugin);
	LeaveCriticalSection(&csInit);

	return true;
}

void DestroyAvatarEditorInstance(IScriptCaller* rmpluginresponder)
{
	if (plugin->is_registered_already(rmpluginresponder))
	{
		plugin->destroy_instance(rmpluginresponder);
	}
}