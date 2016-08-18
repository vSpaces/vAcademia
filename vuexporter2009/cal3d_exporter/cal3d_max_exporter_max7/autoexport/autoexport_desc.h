/**********************************************************************
 *<
	FILE:		    MeshSplit.h
	DESCRIPTION:	Mesh split-by-materials utility
	CREATED BY:		Steve Anderson
	HISTORY:		Created March 15, 2004

 *>	Copyright (c) 2004 Discreet, All Rights Reserved.
 **********************************************************************/

#include "Max.h"
#include "notify.h"
#include "resource.h"
#include "utilapi.h"
#include "istdplug.h"

#define MAX_AUTO_EXPORT_ID Class_ID(0x49833d83-0x999DD9, 0x404939d6)

class CMMLabTownAutoExportUtilityClassDesc : public ClassDesc 
{
	// member variables
protected:
	HINSTANCE m_hInstance;

public:
    int 			IsPublic();
    void *			Create(BOOL loading = FALSE);
    const TCHAR *	ClassName();
    SClass_ID		SuperClassID();
    Class_ID		ClassID();
    const TCHAR* 	Category();
	void SetInstance(HINSTANCE hInstance);
};