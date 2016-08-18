#include "stdafx.h"
#include ".\3dsoundfactor.h"

C3DSoundFactor::C3DSoundFactor()
{
	_attenutedCoef = 1.0f;
	_leftAttenutedCoef = 1.0f;
	_rightAttenutedCoef = 1.0f;
	_minDist = 750.0f;
	_maxDist = 6000.0f;
	_attenunedDistCoef = 750.0f;
	_avDir2youCoef = 0.7f;
	_angle_coef = 0.4f;
}

C3DSoundFactor::~C3DSoundFactor()
{
}

//////////////////////////////////////////////////////////////////////////

void C3DSoundFactor::calc( double adFactorDist, double adMinDist)
{
	if ( adFactorDist < 0.1)
		return;
	_maxDist = (float)adFactorDist;	
	if ( adMinDist > 0.0)
	{
		_minDist = (float)adMinDist;
		if ( _minDist > 0.5f * _maxDist)
			_minDist = 0.5f * _maxDist;
		else if ( _minDist < 0.1f * _maxDist)
			_minDist = 0.1f * _maxDist;
	}
	else
		_minDist = _attenunedDistCoef;
	
	_attenunedDistCoef = (_maxDist - _minDist) / 4.0f;
	if ( _attenunedDistCoef < 0.3f * _minDist)
		_attenunedDistCoef = 0.3f * _minDist;
}

//////////////////////////////////////////////////////////////////////////
