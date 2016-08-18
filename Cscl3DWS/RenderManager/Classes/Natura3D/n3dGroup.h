// n3dGroup.h: interface for the cn3DGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_cn3DGroup_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
#define AFX_cn3DGroup_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_

class nrm3DGroup;

class cn3dGroup 
{
public:
	cn3dGroup( nrm3DGroup * pc = NULL);
	virtual ~cn3dGroup();

protected:
	nrm3DGroup*	prmGroup;
};

#endif // !defined(AFX_cn3DGroup_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
