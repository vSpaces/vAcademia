// n3dVideo.h: interface for the Ccn3dVideo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_cn3dVideo_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
#define AFX_cn3dVideo_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
class nrmObject;

//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class cn3dVideo /*:	public ibaseplugin,
					public iplugupdatable*/
{
public:
	cn3dVideo(nrmObject* pc = NULL);
	virtual ~cn3dVideo();

public:
	/*IMPLEMENT_RELEASE
	BEGIN_INTERFACE_IMPLEMENTATION
		IMPLEMENT_INTERFACE(ibaseplugin)
		IMPLEMENT_INTERFACE(iplugupdatable)
	END_INTERFACE_IMPLEMENTATION
	
	// реализация natura3d::ibaseplugin
public:
	bool	check_support(ibase* apibase);
	void	is_registered(ibase* apibase);
	int		get_supported_interfaces(N3DGUID* pinterfaces, int maxCount);
	
	// реализация natura3d::in3dplvisible1
public:
	void	pre_update(DWORD time, LPVOID value, ibase* apivisibleobject, bool& bhandled){ bhandled=false;};
	void	post_update(DWORD time, LPVOID value, ibase* apivisibleobject);*/

protected:
	nrmObject*	prmObject;
	LONGLONG position;
};

#endif // !defined(AFX_cn3dVideo_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
