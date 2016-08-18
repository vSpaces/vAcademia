//----------------------------------------------------------------------------//
// MMLabVirtualExport.h                                                       //

#ifndef MAX_VESPACE_EXPORT_H
#define MAX_VESPACE_EXPORT_H

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CMMLabVirtualExport : public SceneExport
{
// constructors/destructor
public:
	CMMLabVirtualExport();
	virtual ~CMMLabVirtualExport();

// interface functions	
public:
	const TCHAR *AuthorName();
	const TCHAR *CopyrightMessage();
	int DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options);
	const TCHAR *Ext(int n);
	int ExtCount();
	const TCHAR *LongDesc();
	const TCHAR *OtherMessage1();
	const TCHAR *OtherMessage2();
	const TCHAR *ShortDesc();
	void ShowAbout(HWND hWnd);
	unsigned int Version();
};

#endif

//----------------------------------------------------------------------------//
