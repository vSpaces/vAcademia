
#if !defined(AFX_rmIPlugin_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_rmIPlugin_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Plugin types
#define RMPT_RENDER		1	// perform render functions
#define RMPT_CREATEVO		2	// create visible object
#define RMPT_NEEDEXTERNAL	4	// SetExternal function must be called on initialization
#define RMPT_NOTIFICATIONS	8	// plugin support IPluginNotification interface

// Object type
#define RMPO_3DOBJECT		1	// applyed on 3DObject
#define RMPO_ALL		2	// applyed to all objects
#define RMPO_IMAGE		4	// applyed on Images

typedef struct _CLEFPLUGININFO 
{
	DWORD	dwSize;				// для определения версии
	DWORD	dwVersion;			// версия плагина
	DWORD	dwSupportedObjects;	// флаги поддерживаемых объектов
} CLEFPLUGININFO;


struct IPropertiesContainer;

struct IScriptCaller
{
	virtual bool Call(char* apProcName, char* aArgTypes, ...) = 0;
	virtual bool Call(char* apProcName, char* aArgTypes, va_list ptr) = 0;
};

struct IRMPluginNotifications
{
	virtual void	OnPropsChanged() = 0;			// Called on Any prop changed
	virtual void	OnPropChanged(char* apPropName) = 0;	// Called when prop <apPropName> is changed
};

struct IPlugin;

typedef	unsigned long clefresult;

#define	CLEF_ERROR		0xF0000000
#define	CLEF_UNKNOWNMESSAGE	1 | CLEF_ERROR

struct IRMBasePlugin
{
	virtual	void			FreeObject() = 0;
	virtual	void			GetInfo(CLEFPLUGININFO* apinfo) = 0;
	virtual	void			SetResponder(IScriptCaller* apirmpluginresponder) = 0;
	virtual	void			SetPropsContainer(IPropertiesContainer* apipropscontainer) = 0;
	virtual	unsigned long	ProcessMessage(int aiArgC, va_list aArgsV) = 0;
	virtual IPlugin*		GetPluginInterface() = 0;
};

#endif // !defined(AFX_rmIPlugin_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)