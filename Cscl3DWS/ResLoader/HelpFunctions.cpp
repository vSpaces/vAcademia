#include "StdAfx.h"
#include "HelpFunctions.h"

CComString	CorrectFilename(LPCSTR fileName)
{
	if (!fileName)	
	{
		return "";
	}

	CComString ret( fileName);
	ret.Replace("\\\\", "\\");
	ret.Replace("//", "/");
	ret.MakeLower();
	ret.Replace("http:/", "http://");
	ret.Replace("ftp:/", "ftp://");

	return ret;
}