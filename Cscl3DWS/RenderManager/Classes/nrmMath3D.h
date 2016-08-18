// nrmMath3D.h: interface for the nrmMath3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRMMATH3D_H__11B50C75_36FB_465A_AB7A_8A437279D3EF__INCLUDED_)
#define AFX_NRMMATH3D_H__11B50C75_36FB_465A_AB7A_8A437279D3EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace rmml;

class nrmMath3D  : public rm::IMath3DRMML
{
public:
	nrmMath3D();
	virtual ~nrmMath3D();

public:	//IMath3D interface implementation
	void	quatMult(ml3DRotation &rot1, ml3DRotation &rot2, ml3DRotation &rotRes);
	void	quatBlend(ml3DRotation &rot1, ml3DRotation &rot2, double k, ml3DRotation &rotRes);
	void	vec3MultQuat(ml3DPosition &pos, ml3DRotation &rot, ml3DPosition &posRes);
};

#endif // !defined(AFX_NRMMATH3D_H__11B50C75_36FB_465A_AB7A_8A437279D3EF__INCLUDED_)
