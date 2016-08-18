// logfile.cpp: implementation of the CalLogFile class.
//
//////////////////////////////////////////////////////////////////////
#include "global.h"
#include "logfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CalLogFile::CalLogFile(LPCSTR filename)
{
	handle = 0;
	handle = fopen( filename, "w");
}

CalLogFile::~CalLogFile()
{
	if( !handle)	return;
	fclose(handle);
}

void CalLogFile::write(LPCSTR str)
{
	if( !handle)	return;
	fwrite(str, strlen(str), 1, handle);
}

void CalLogFile::writeln(LPCSTR str)
{
	if( !handle)	return;
	fwrite(str, strlen(str), 1, handle);
	fwrite("\n", 1, 1, handle);
}

