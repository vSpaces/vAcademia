// RMComp.h: interface for the CRMCompLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RMCOMP_H__9345053A_98C9_42E0_BA9D_2BE6C85F2751__INCLUDED_)
#define AFX_RMCOMP_H__9345053A_98C9_42E0_BA9D_2BE6C85F2751__INCLUDED_

#include "../CommonRenderManagerHeader.h"

using	namespace rmml;
class CNRManager;

class CRMCompLib  
{
	CNRManager*	m_pRenderManager;
public:
	CRMCompLib(CNRManager*	apRenderManager);
	virtual ~CRMCompLib();
	
public:
	void update(DWORD delta);
	omsresult AddObject(mlMedia* apMLMedia);
	omsresult RemoveObject(mlMedia* apMLMedia);
	void init();

private:
	MP_VECTOR<moMedia*>	rmViewports;	
	MP_VECTOR<moMedia*>	rmScenes;
	MP_VECTOR<moMedia*>	rmAudios;
};

#endif // !defined(AFX_RMCOMP_H__9345053A_98C9_42E0_BA9D_2BE6C85F2751__INCLUDED_)


