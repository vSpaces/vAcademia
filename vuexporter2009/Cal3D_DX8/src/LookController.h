// LookController.h: interface for the CalLookController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOKCONTROLLER_H__6E3B7311_D330_4EB5_A294_B898F915FD2F__INCLUDED_)
#define AFX_LOOKCONTROLLER_H__6E3B7311_D330_4EB5_A294_B898F915FD2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "global.h"
#include "vector.h"
#include "quaternion.h"

class CalModel;
//
typedef struct _LOOKATPARAMS{
	int			adwDelay;
	int			adwTime;
	float		trTime;
	bool		bToCamera;
	bool		bToPoint;
	CalModel*	m_pModel;
	CalVector	camPos;	
	bool		live_eyes;
	float		m_LookPortion;

	D3DMATRIX*	m_SelfExtTrans;
	D3DMATRIX*	m_TrgExtTrans;
	_LOOKATPARAMS()
	{	adwDelay=0; trTime=0.0; bToCamera=false; bToPoint=false; 
		m_pModel=NULL; camPos=CalVector(0,0,0); adwTime=0; live_eyes=false;
		m_LookPortion = 1.0f;}
	void operator=(const _LOOKATPARAMS& lp)
	{	adwDelay=lp.adwDelay; trTime=lp.trTime; bToCamera=lp.bToCamera; bToPoint=lp.bToPoint; 
		m_pModel=lp.m_pModel; camPos=lp.camPos; adwTime=lp.adwTime; live_eyes=lp.live_eyes;
		m_LookPortion = lp.m_LookPortion;}
} LOOKATPARAMS;

class CalBone;

class CalLookController  
{
	//
	enum	TARGET_TYPE{	TT_POINT, TT_MODEL};			// type of target to look for
	enum	VIEW_VELOCITY{	VV_SLOW, VV_NORMAL, VV_FAST};	// velocity of the head rotation

public:
	void Enable( bool ab_Enable);
	bool m_bEnabled;
	bool Init(LOOKATPARAMS aparams);
	void SetTarget(CalModel* pTarget);
	void SetTarget(CalVector pTarget);
	void Update(float delay);
	CalLookController(CalModel* pModel);
	virtual ~CalLookController();

	CalModel*	m_LookTarget;
	CalVector	pntTarget;
	CalModel*	m_pModel;

	CalBone*	p_bHead;
	CalBone*	p_bNeck;
	CalBone*	p_bLook;
	CalBone*	p_bLeftEye;
	CalBone*	p_bRightEye;

	bool		live_eyes;

	TARGET_TYPE	type;

	int			iNeckID;
	int			iHeadID;
	int			iLookAtHeadID;

	int			iLeftEyeID, iRightEyeID;
	DWORD		state;

private:
	CalQuaternion	last_head_rotation;	// поворот головы на предыдущем кадре
	CalQuaternion	last_ly_rotation;	// поворот левого глаза на предыдущем кадре
	CalQuaternion	last_ry_rotation;	// поворот правого глаза на предыдущем кадре
	CalQuaternion	start_look_rotation;	// for portion look calculations

	LOOKATPARAMS	params;
};

#endif // !defined(AFX_LOOKCONTROLLER_H__6E3B7311_D330_4EB5_A294_B898F915FD2F__INCLUDED_)
