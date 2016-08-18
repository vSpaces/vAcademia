// clefGuids.h: interface for the nrmPlugin class.
//
// $RCSfile: clefGuids.h,v $
// $Revision: 1.3 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2008/09/29 07:44:11 $
//              $Author: asmo1 $
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_clefGuids_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_clefGuids_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CLEFHDEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    N3DGUID name \
                    = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
/*
namespace clef
{
	CLEFHDEFINE_GUID(iid_iscriptcaller, 			0x3cff3d70, 0x5b2a, 0x4176, 0x8a, 0x50, 0x8b, 0xa5, 0x85, 0xef, 0x8c, 0x9c);
	CLEFHDEFINE_GUID(iid_irmpluginnotifications, 		0x31589478, 0xe655, 0x4f2a, 0x9d, 0x92, 0x60, 0x81, 0xad, 0x21, 0x14, 0x98);
	CLEFHDEFINE_GUID(iid_irmbaseplugin, 			0xf429c5e8, 0xcdf1, 0x46dc, 0xa2, 0x74, 0x78, 0x48, 0x59, 0x82, 0xdd, 0x41);
	CLEFHDEFINE_GUID(iid_irmpluginresponder, 		0x5a1e6b42, 0xb6f8, 0x4e2c, 0x89, 0x4d, 0x3e, 0xd3, 0x8e, 0xd9, 0x49, 0xda);
	CLEFHDEFINE_GUID(iid_ipropscontainer,			0x683fb741, 0xbd89, 0x4f7e, 0xba, 0x64, 0x74, 0xd6, 0x39, 0x5e, 0xa4, 0xaa);
	CLEFHDEFINE_GUID(iid_ianydatacontainer,			0xf663a693, 0xcd34, 0x47bc, 0xba, 0x56, 0xd,  0xad, 0x12, 0x3d, 0xfd, 0xf7);
}
*/
#endif // !defined(AFX_clefGuids_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)