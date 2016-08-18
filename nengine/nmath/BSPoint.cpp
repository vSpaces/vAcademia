
#include "stdafx.h"
#include "BSPoint.h"

CBSCPoint::CBSCPoint(const CBSCPoint &right): 
		m_pos(right.m_pos),
        m_ctrlp1(right.m_ctrlp1),
        m_ctrlp2(right.m_ctrlp2),
		m_length(right.m_length),
		m_type(right.m_type)
{
}

CBSCPoint::CBSCPoint(CVector3D p, CVector3D c1, CVector3D c2)
{
	m_pos = p;
	m_ctrlp1 = c1;

	if (((FLT_MAX == c2.x) && (FLT_MAX == c2.y) && (FLT_MAX == c2.z)) || ((c2 - p) == (p - c1)))
	{
		m_type = CT_SMOOTH;
		m_ctrlp2 = p - (m_ctrlp1 - p);
	}
	else
	{
		m_type = CT_CORNER;
		m_ctrlp2 = c2;
	}
}

CBSCPoint& CBSCPoint::operator=(const CBSCPoint &right)
{
	m_length = right.m_length;
	m_pos = right.m_pos;
	m_ctrlp1 = right.m_ctrlp1;
	m_ctrlp2 = right.m_ctrlp2;
	return *this;
}

int CBSCPoint::operator==(const CBSCPoint &right)
{
	return !((m_pos != right.m_pos) || (m_ctrlp1 != right.m_ctrlp1) || (m_ctrlp2 != right.m_ctrlp2) || 
		(m_length != right.m_length));
}

int CBSCPoint::operator!=(const CBSCPoint &right)
{
	return ((m_pos != right.m_pos) || (m_ctrlp1 != right.m_ctrlp1) || (m_ctrlp2 != right.m_ctrlp2) || 
		(m_length != right.m_length));
}

CBSCPoint::~CBSCPoint()
{
}