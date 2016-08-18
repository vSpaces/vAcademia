
#include "stdafx.h"
#include "FlashNP.h"
#include <assert.h>
#include "flash.h"
#include "npruntime_impl.h"

static const char* const errorStrings[] = {
	"No errors occurred.", // 0 NPERR_NO_ERROR
		"Error with no specific error code occurred.", // 1 NPERR_GENERIC_ERROR
		"Invalid instance passed to the plug-in.", // 2 NPERR_INVALID_INSTANCE_ERROR
		"Function table invalid.", // 3 NPERR_INVALID_FUNCTABLE_ERROR
		"Loading of plug-in failed.", // 4 NPERR_MODULE_LOAD_FAILED_ERROR
		"Memory allocation failed.", // 5 NPERR_OUT_OF_MEMORY_ERROR
		"Plug-in missing or invalid.", // 6 NPERR_INVALID_PLUGIN_ERROR
		"Plug-in directory missing or invalid.", // 7 NPERR_INVALID_PLUGIN_DIR_ERROR
		"Versions of plug-in and Communicator do not match.", // 8 NPERR_INCOMPATIBLE_VERSION_ERROR
		"Parameter missing or invalid.", // 9 NPERR_INVALID_PARAM
		"URL missing or invalid.", // 10 NPERR_INVALID_URL
		"File missing or invalid.", // 11 NPERR_FILE_NOT_FOUND
		"Stream contains no data.", // 12 NPERR_NO_DATA
		"Seekable stream expected.", // 13 NPERR_STREAM_NOT_SEEKABLE
		"Unknown error code"
};

CFlashNP gFlashNP;

CFlashNP::CFlashNP():
m_module(NULL),
miMajorFlashVersion(0),
m_NPP_Shutdown(NULL)
{
	LoadFlashPlugin();
}

CFlashNP::~CFlashNP(){
	if(m_module != NULL){
		/*try
		{*/
#ifdef BUILD_M
			if( false)
#endif
				if(m_NPP_Shutdown != NULL)
					m_NPP_Shutdown();	// тут падает в дебаге
		/*}
		catch (...)
		{
		}*/
		::FreeLibrary(m_module);
	}
}

int CFlashNP::LoadFlashPlugin(){
	if(m_module != NULL)
		return miMajorFlashVersion;
	m_module = NULL;
	// узнать где лежит Flash-плугин
	// HKEY_LOCAL_MACHINE\SOFTWARE\Macromedia\FlashPlayerPlugin   Path
	// C:\WINDOWS\system32\Macromed\Flash
	std::string sFlashPath;

	char pcFlashPath[MAX_PATH * 2 + 2] = "";
	HKEY hKey = NULL;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Macromedia\\FlashPlayerPlugin", 0, KEY_READ, &hKey) == ERROR_SUCCESS && hKey != 0){
		DWORD dwType = REG_SZ;
		DWORD dwSize = 0;
		if(RegQueryValueEx( hKey, "Path", 0, &dwType, NULL, &dwSize) == ERROR_SUCCESS && dwSize > 0){
			assert(dwSize < MAX_PATH * 2);
			if(RegQueryValueEx( hKey, "Path", 0, &dwType, (LPBYTE)pcFlashPath, &dwSize) == ERROR_SUCCESS && dwSize > 0){
				sFlashPath = pcFlashPath;
			}
		}
		RegCloseKey(hKey);
	}
	if(sFlashPath.empty()){
		// взять системный путь
		UINT uiSize = GetSystemDirectory(pcFlashPath, MAX_PATH*2);
		if(uiSize > 0)
			sFlashPath = pcFlashPath;
		if(sFlashPath[sFlashPath.length()-1] != '\\')
			sFlashPath += '\\';
		sFlashPath += "Macromed\\Flash";
	}
	if(sFlashPath.empty())
		return 0;
	if(sFlashPath[sFlashPath.length()-1] != '\\')
		sFlashPath += '\\';
	sFlashPath += "NPSWF32.dll";

	// узнать из реестра версию Flash-плугина (из HKEY_LOCAL_MACHINE\SOFTWARE\Macromedia\FlashPlayer\  CurrentVersion)
	std::string sFlashVersion;
	miMajorFlashVersion = 0; // неизвестная версия
	hKey = NULL;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Macromedia\\FlashPlayer", 0, KEY_READ, &hKey) == ERROR_SUCCESS && hKey != 0){
		DWORD dwType = REG_SZ;
		DWORD dwSize = 0;
		if(RegQueryValueEx( hKey, "CurrentVersion", 0, &dwType, NULL, &dwSize) == ERROR_SUCCESS && dwSize > 0){
			char* pcFlashVersion = new char[dwSize * 2];
			if(RegQueryValueEx( hKey, "CurrentVersion", 0, &dwType, (LPBYTE)pcFlashVersion, &dwSize) == ERROR_SUCCESS && dwSize > 0){
				sFlashVersion = pcFlashVersion; // 10,0,22,87
			}
			delete[] pcFlashVersion;
		}
		RegCloseKey(hKey);
		if(!sFlashVersion.empty()){
			std::string sMajorVer = sFlashVersion;
			std::string::size_type pos = sFlashVersion.find(',');
			if(pos != std::string::npos){
				sMajorVer = sFlashVersion.substr(0, pos);
			}
			miMajorFlashVersion = atoi(sMajorVer.c_str());
		}
	}
	m_module = ::LoadLibrary(sFlashPath.c_str());
	if(m_module == NULL){
		return -1; // dll-ка по какой-то причине не загрузилась (см. GetLastError)
	}

	NP_GetEntryPointsFuncPtr NP_GetEntryPoints = 0;
	NP_InitializeFuncPtr NP_Initialize = 0;
	NPError npErr;

	NP_Initialize = (NP_InitializeFuncPtr)GetProcAddress(m_module, "NP_Initialize");
	NP_GetEntryPoints = (NP_GetEntryPointsFuncPtr)GetProcAddress(m_module, "NP_GetEntryPoints");
	m_NPP_Shutdown = (NPP_ShutdownProcPtr)GetProcAddress(m_module, "NP_Shutdown");

	if (!NP_Initialize || !NP_GetEntryPoints || !m_NPP_Shutdown)
		return -2; // какая-то кривая dll-ка

	memset(&m_pluginFuncs, 0, sizeof(m_pluginFuncs));
	m_pluginFuncs.size = sizeof(m_pluginFuncs);

	npErr = NP_GetEntryPoints(&m_pluginFuncs);
	LOG_NPERROR(npErr);
	if (npErr != NPERR_NO_ERROR){
		::FreeLibrary(m_module);
		m_module = NULL;
		return -2; // какая-то кривая dll-ка
	}

	memset(&m_browserFuncs, 0, sizeof(m_browserFuncs));
for(int ii = 0; ii < sizeof(m_browserFuncs) / 4; ii++){
((int*)&m_browserFuncs)[ii] = ii;
}
	m_browserFuncs.size = sizeof (m_browserFuncs);
	m_browserFuncs.version = NP_VERSION_MINOR;

///*
	m_browserFuncs.geturl = NPN_GetURL;
	m_browserFuncs.posturl = NPN_PostURL;
	m_browserFuncs.requestread = NPN_RequestRead;
	m_browserFuncs.newstream = NPN_NewStream;
	m_browserFuncs.write = NPN_Write;
	m_browserFuncs.destroystream = NPN_DestroyStream;
	m_browserFuncs.status = NPN_Status;
	m_browserFuncs.uagent = NPN_UserAgent;
	m_browserFuncs.memalloc = NPN_MemAlloc;
	m_browserFuncs.memfree = NPN_MemFree;
	m_browserFuncs.memflush = NPN_MemFlush;
	m_browserFuncs.reloadplugins = NPN_ReloadPlugins;
	m_browserFuncs.geturlnotify = NPN_GetURLNotify;
	m_browserFuncs.posturlnotify = NPN_PostURLNotify;
	m_browserFuncs.getvalue = NPN_GetValue;
	m_browserFuncs.setvalue = NPN_SetValue;
	m_browserFuncs.invalidaterect = NPN_InvalidateRect;
	m_browserFuncs.invalidateregion = NPN_InvalidateRegion;
	m_browserFuncs.forceredraw = NPN_ForceRedraw;
	m_browserFuncs.getJavaEnv = NPN_GetJavaEnv;
	m_browserFuncs.getJavaPeer = NPN_GetJavaPeer;
	m_browserFuncs.pushpopupsenabledstate = NPN_PushPopupsEnabledState;
	m_browserFuncs.poppopupsenabledstate = NPN_PopPopupsEnabledState;
	m_browserFuncs.pluginthreadasynccall = NPN_PluginThreadAsyncCall;

	m_browserFuncs.releasevariantvalue = _NPN_ReleaseVariantValue;
	m_browserFuncs.getstringidentifier = _NPN_GetStringIdentifier;
	m_browserFuncs.getstringidentifiers = _NPN_GetStringIdentifiers;
	m_browserFuncs.getintidentifier = _NPN_GetIntIdentifier;
	m_browserFuncs.identifierisstring = _NPN_IdentifierIsString;
	m_browserFuncs.utf8fromidentifier = _NPN_UTF8FromIdentifier;
	m_browserFuncs.intfromidentifier = _NPN_IntFromIdentifier;
	m_browserFuncs.createobject = _NPN_CreateObject;
	m_browserFuncs.retainobject = _NPN_RetainObject;
	m_browserFuncs.releaseobject = _NPN_ReleaseObject;
	m_browserFuncs.invoke = _NPN_Invoke;
	m_browserFuncs.invokeDefault = _NPN_InvokeDefault;
	m_browserFuncs.evaluate = _NPN_Evaluate;
	m_browserFuncs.getproperty = _NPN_GetProperty;
	m_browserFuncs.setproperty = _NPN_SetProperty;
	m_browserFuncs.removeproperty = _NPN_RemoveProperty;
	m_browserFuncs.hasproperty = _NPN_HasProperty;
	m_browserFuncs.hasmethod = _NPN_HasMethod;
	m_browserFuncs.setexception = _NPN_SetException;
	m_browserFuncs.enumerate = _NPN_Enumerate;
	m_browserFuncs.construct = _NPN_Construct;

	npErr = NP_Initialize(&m_browserFuncs);
	LOG_NPERROR(npErr);

	if (npErr != NPERR_NO_ERROR){
		::FreeLibrary(m_module);
		m_module = NULL;
		return -3; // какая-то ошибка при инициализации
	}

//	m_pluginFuncs.setvalue("WMODE", "transparent");

	// ??
	return miMajorFlashVersion;
}

IFlash* CFlashNP::CreateInstance(IFlashCallBack* apCallBack, HWND ahwndParentWindow, IResLibrary* apResLibrary){
	if(m_module == NULL || m_pluginFuncs.newp == NULL)
		return NULL;
	CFlash* pFlash = new CFlash(apCallBack, ahwndParentWindow, apResLibrary);
	NPMIMEType pluginType = "application/x-shockwave-flash";
	pFlash->m_instanceStruct.pdata = NULL;
	pFlash->m_instanceStruct.ndata = pFlash;
//	std::string sSrcFile = "C:/vu2008/ui/k01_2.swf";
//	std::string sSrc = "file:///C:/vu2008/ui/k01_2.swf"; // "C:/vu2008/ui/k01_2.swf";
//	char* argn[] = {"WMODE", "HEIGHT", "WIDTH", "SRC"};
//	char* argv[] = {"transparent", "100", "100", (char*)sSrc.c_str()}; // "file:///C:/vu2008/ui/k01_2.swf"
//	char* argn[] = {"WMODE", "HEIGHT", "WIDTH"};
//	char* argv[] = {"transparent", "100", "100"};
	char* argn[] = {"allowScriptAccess", "HEIGHT", "WIDTH", "SRC", "WMODE", "swliveconnect"};
//	<param name="allowScriptAccess" value="sameDomain" />
//always | never | samedomain
	char* argv[] = {"always", "100", "100", "", "transparent", "true"}; // (char*)sSrc.c_str() //  opaque
//	typedef NPError (*NPP_NewProcPtr)(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc, char* argn[], char* argv[], NPSavedData* saved);
	NPError npErr = m_pluginFuncs.newp(pluginType, pFlash->m_instance, NP_EMBED, 6, argn, argv, NULL);
	if (npErr != NPERR_NO_ERROR || pFlash->m_isWindowed){ 
		delete pFlash;
		return NULL;
	}
//	m_pluginFuncs.setvalue
/*	
	NPStream stream;
	memset(&stream, 0, sizeof(stream));
	stream.url = sSrc.c_str();
	FILE* f = fopen(sSrcFile.c_str(), "r");
	fseek(f, 0L, SEEK_END);
	fpos_t iFSize = 0;
	fgetpos(f, &iFSize);
	fseek(f, 0L, SEEK_SET);
	stream.end = (int32)iFSize;
	char* buf = new char[iFSize + 1];
	fread(buf, 1, iFSize, f);
//	typedef NPError (*NPP_NewStreamProcPtr)(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
	uint16 stype = NP_NORMAL;
	npErr = m_pluginFuncs.newstream(pFlash->m_instance, pluginType, &stream, false, &stype);
//	NP_NORMAL
//	m_pluginFuncs.asfile(pFlash->m_instance, &stream, sSrc.c_str());
	int32 iReadySize = m_pluginFuncs.writeready(pFlash->m_instance, &stream);
	//typedef int32 (*NPP_WriteProcPtr)(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
	int32 iRet = m_pluginFuncs.write(pFlash->m_instance, &stream, 0, iFSize, buf);
	int hh=0;
	m_pluginFuncs.destroystream(pFlash->m_instance, &stream, NPRES_DONE);
	delete[] buf;

	fclose(f);
*/
//NPObject* object = NULL;
//npErr = gFlashNP.m_pluginFuncs.getvalue(pFlash->m_instance, NPPVpluginScriptableNPObject, &object);
//if(npErr != NPERR_NO_ERROR || object == NULL)
//return NULL;

// отрисовка
//typedef int16 (*NPP_HandleEventProcPtr)(NPP instance, void* event);
//NPEvent npEvent;
/*
typedef struct _NPEvent
{
	uint16   event;
	uint32   wParam;
	uint32   lParam;
} NPEvent;
*/
//gFlashNP.m_pluginFuncs.event(npEvent);

	return pFlash;
}

void CFlashNP::DestroyInstance(IFlash* apFlash){
	if (apFlash != NULL){
		CFlash* pFlash = (CFlash*)apFlash;
		m_pluginFuncs.destroy(pFlash->m_instance, NULL);
		delete pFlash;
	}
}

static CFlash* flashObjForInstance(NPP instance)
{
	if (instance && instance->ndata)
		return static_cast<CFlash*>(instance->ndata);
//	return PluginView::currentPluginView();
	return NULL;
}

void* NPN_MemAlloc(uint32 size)
{
	return malloc(size);
}

void NPN_MemFree(void* ptr)
{
	free(ptr);
}

uint32 NPN_MemFlush(uint32 size)
{
	// Do nothing
	return 0;
}

void NPN_ReloadPlugins(NPBool reloadPages)
{
//	refreshPlugins(reloadPages);
}

NPError NPN_RequestRead(NPStream* stream, NPByteRange* rangeList)
{
	return NPERR_STREAM_NOT_SEEKABLE;
}

NPError NPN_GetURLNotify(NPP instance, const char* url, const char* target, void* notifyData)
{
	return flashObjForInstance(instance)->getURLNotify(url, target, notifyData);
}

NPError NPN_GetURL(NPP instance, const char* url, const char* target)
{
	return flashObjForInstance(instance)->getURL(url, target);
}

NPError NPN_PostURLNotify(NPP instance, const char* url, const char* target, uint32 len, const char* buf, NPBool file, void* notifyData)
{
	return flashObjForInstance(instance)->postURLNotify(url, target, len, buf, file, notifyData);
}

NPError NPN_PostURL(NPP instance, const char* url, const char* target, uint32 len, const char* buf, NPBool file)
{
	return flashObjForInstance(instance)->postURL(url, target, len, buf, file);
}

NPError NPN_NewStream(NPP instance, NPMIMEType type, const char* target, NPStream** stream)
{
	return flashObjForInstance(instance)->newStream(type, target, stream);
}

int32 NPN_Write(NPP instance, NPStream* stream, int32 len, void* buffer)
{
	return flashObjForInstance(instance)->write(stream, len, buffer);
}

NPError NPN_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
	return flashObjForInstance(instance)->destroyStream(stream, reason);
}

const char* NPN_UserAgent(NPP instance)
{
	CFlash* flash = flashObjForInstance(instance);

	if (!flash)
		return CFlash::userAgentStatic();

	return flash->userAgent();
}

void NPN_Status(NPP instance, const char* message)
{
	flashObjForInstance(instance)->status(message);
}

void NPN_InvalidateRect(NPP instance, NPRect* invalidRect)
{
	flashObjForInstance(instance)->invalidateRect(invalidRect);
}

void NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
	flashObjForInstance(instance)->invalidateRegion(invalidRegion);
}

void NPN_ForceRedraw(NPP instance)
{
	flashObjForInstance(instance)->forceRedraw();
}

NPError NPN_GetValue(NPP instance, NPNVariable variable, void* value)
{
	CFlash* flash = flashObjForInstance(instance);

	if (!flash)
		return CFlash::getValueStatic(variable, value);

	return flashObjForInstance(instance)->getValue(variable, value);
}

NPError NPN_SetValue(NPP instance, NPPVariable variable, void* value)
{
	return flashObjForInstance(instance)->setValue(variable, value);
}

void* NPN_GetJavaEnv()
{
	// Unsupported
	return 0;
}

void* NPN_GetJavaPeer(NPP instance)
{
	// Unsupported
	return 0;
}

void NPN_PushPopupsEnabledState(NPP instance, NPBool enabled)
{
//	flashObjForInstance(instance)->pushPopupsEnabledState(enabled);
}

void NPN_PopPopupsEnabledState(NPP instance)
{
//	flashObjForInstance(instance)->popPopupsEnabledState();
}

void NPN_PluginThreadAsyncCall(NPP instance, void (*func) (void *), void *userData)
{
//	PluginMainThreadScheduler::scheduler().scheduleCall(instance, func, userData);
}
