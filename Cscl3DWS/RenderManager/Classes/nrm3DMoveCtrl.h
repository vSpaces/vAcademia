// nrmMotion.h: interface for the nrmMotion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmMotion_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmMotion_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include <vector.h>

class nrm3DMoveCtrl
{
public:
	nrm3DMoveCtrl();
	virtual ~nrm3DMoveCtrl();

public:
	void update(int aiDelta);

private:
	CalVector		  m_vDestPos3D, m_vStartPos3D;
	int				  m_iDuration;
	float			  m_fCurTime;
	bool			  m_bCheckCollisions;
};

#endif // !defined(AFX_nrmMotion_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
