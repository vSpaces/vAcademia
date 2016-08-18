// SoundBuffer.cpp: implementation of the SDSoundBuffer and sound_buf classes
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Listener.h"

CListener::CListener()
{
	dir = CalVector( 0.0f, 1.0f, 0.0f);
	pos = CalVector( 0.0f, 0.0f, 0.0f);
}

CListener::~CListener()
{
}

float CListener::getVolumeFromDistance( const float coef, const float minDist, const float maxDist, const float attenunedDistCoef, const float dist)
{	
	float attenutedCoef = coef; 
	float attenutedDist = dist - minDist;
	if ( dist >= maxDist)
	{
		return 0.0f;
	}
	else if ( attenutedDist >= attenunedDistCoef)
	{
		float count = attenutedDist / attenunedDistCoef;
		while ( count > 1.0f)
		{
			attenutedCoef *= 0.5f;
			attenutedDist -= attenunedDistCoef;
			count--;
		}		
		attenutedCoef *= ( 1.0f - attenutedDist / ( 2.0f * attenunedDistCoef));
	}
	else if ( attenutedDist > 0.0f)
	{
		attenutedCoef *= ( 1.0f - attenutedDist / ( 2.0f * attenunedDistCoef));
	}
	return attenutedCoef;
}

float CListener::getAngleOnHorinz( CalVector avPos)
{
	CalVector avDir = avPos - pos;	
	CalVector dirXY = dir;
	CalVector avDirXY = avDir;
	dirXY.z = 0.0f;
	avDirXY.z = 0.0f;
	float angle = dirXY.getAngle( avDirXY);

	CalVector z = CalVector( 0.0f, 0.0f, 1.0f); //dirXY % avDirXY;
	CalVector dirXYZ = dirXY + CalVector( 0.0f, 0.0f, 1.0f);
	CalVector norm = dirXYZ % z;
	double arg = norm.dot( avDirXY) / ( norm.length() * avDirXY.length());
	if ( arg < 0.0f)
		angle = -angle;
	return angle;
	// вправо - угол положительный
}

float CListener::getAngleBetweenDirects( CalVector direct)
{
	return direct.getAngle( dir);
}

void CListener::setDirect( CalVector dir)
{
	this->dir = dir;
}