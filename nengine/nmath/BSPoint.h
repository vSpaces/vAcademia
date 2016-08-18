#pragma once

#pragma  warning (disable: 4702)

#include "Vector3D.h"
#include <float.h>

enum	BSCP_TYPE	{CT_CORNER, CT_SMOOTH};

class CBSCPoint 
{
public:
	CBSCPoint(const CBSCPoint &right);
	CBSCPoint(CVector3D p = CVector3D(0, 0, 0), CVector3D c1 = CVector3D(0, 0, 0), CVector3D c2 = CVector3D(FLT_MAX, FLT_MAX, FLT_MAX));
	~CBSCPoint();

	CBSCPoint & operator=(const CBSCPoint &right);
	int operator==(const CBSCPoint &right);
	int operator!=(const CBSCPoint &right);

	const CVector3D GetPosition()const;
	void SetPosition(CVector3D value);

	const CVector3D GetCtrlp1()const;
	void SetCtrlp1(CVector3D value);

	const CVector3D GetCtrlp2()const;
	void SetCtrlp2(CVector3D value);

	BSCP_TYPE m_type;
	float m_length;

private: 
	CVector3D m_pos;
	CVector3D m_ctrlp1;
	CVector3D m_ctrlp2;
};

inline const CVector3D CBSCPoint::GetPosition()const
{
	return m_pos;
}

__forceinline void CBSCPoint::SetPosition(CVector3D value)
{
	m_pos = value;
}

inline const CVector3D CBSCPoint::GetCtrlp1()const
{
	return m_ctrlp1;
}

__forceinline void CBSCPoint::SetCtrlp1(CVector3D value)
{
	m_ctrlp1 = value;
}

inline const CVector3D CBSCPoint::GetCtrlp2()const
{
	return m_ctrlp2;
}

__forceinline void CBSCPoint::SetCtrlp2(CVector3D value)
{
	m_ctrlp2 = value;
}
