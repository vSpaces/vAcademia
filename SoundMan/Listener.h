// Listener.h: interface for the CListener
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_LISTENER_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_)
#define AFX_LISTENER_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_

#include "vector.h"

class CListener
{
public:
	CListener();
	~CListener();

public:
	float getVolumeFromDistance( const float coef, const float minDist, const float maxDist, const float attenunedDistCoef, const float dist);
	float getAngleOnHorinz( CalVector vec);
	float getAngleBetweenDirects( CalVector dir);
	void setDirect( CalVector dir);

protected:
	CalVector pos;
	CalVector dir;
};

#endif // !defined(AFX_LISTENER_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_)