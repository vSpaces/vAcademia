// logfile.h: interface for the CalLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__56CB4C44_88FF_4D50_8857_AC74A817A620__INCLUDED_)
#define AFX_LOGFILE_H__56CB4C44_88FF_4D50_8857_AC74A817A620__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAL3D_API CalLogFile  
{
public:
	void write(LPCSTR str);
	void writeln(LPCSTR str);
	CalLogFile(LPCSTR filename);
	virtual ~CalLogFile();

protected:
	FILE*		handle;

};

#endif // !defined(AFX_LOGFILE_H__56CB4C44_88FF_4D50_8857_AC74A817A620__INCLUDED_)
