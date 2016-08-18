#pragma once

struct proxyConfig
{
	int			useUser;
	int			isUsed;
	int			version;
	CComString	proxy;
	CComString	user;
	CComString	password;
};

struct proxyConfigForExternal
{
	int			useUser;
	int			isUsed;
	int			version;
	CComString*	proxy;
	CComString*	user;
	CComString*	password;
};