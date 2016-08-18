//----------------------------------------------------------------------------//
// MMLabVirtualExportDesc.h                                                   //

#ifndef MAX_VESPACE_EXPORT_DESC_H
#define MAX_VESPACE_EXPORT_DESC_H

//----------------------------------------------------------------------------//
// Defines                                                                    //
//----------------------------------------------------------------------------//

#define MAX_VESPACE_EXPORT_ID Class_ID(0x50944e93-0x00AEEA, 0x515a40e6)

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//
 
class CMMLabVirtualExportDesc : public ClassDesc2
{
// member variables
protected:
	HINSTANCE m_hInstance;

// constructors/destructor
public:
	CMMLabVirtualExportDesc();
	virtual ~CMMLabVirtualExportDesc();

// member functions	
public:
	void SetInstance(HINSTANCE hInstance);

// interface functions	
public:
	const TCHAR *Category();
	Class_ID ClassID();
	const TCHAR *ClassName();
	void *Create(BOOL loading = FALSE);
	HINSTANCE HInstance();
	const TCHAR *InternalName(); 
	int IsPublic();
	SClass_ID SuperClassID();
};

#endif

//----------------------------------------------------------------------------//
