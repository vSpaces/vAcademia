#pragma once

class C3DSoundFactor
{
public:
	C3DSoundFactor();
	virtual ~C3DSoundFactor();

public:
	void calc( double adFactorDist, double adMinDist);

public:
	// коэффициент падения уровня громкости звука
	float _attenutedCoef;
	// коэффициент падения уровня громкости звука
	float _leftAttenutedCoef;
	// коэффициент падения уровня громкости звука
	float _rightAttenutedCoef;
	// минимальное расстояние, от 0 до minDist амплитуда звука не меняется
	float _minDist;
	// расстояние
	float _maxDist;
	// расстояние, на которое амплитуда звука падает в два раза
	float _attenunedDistCoef;
	// коэффициент падения уровня громкости звука, если аватара расположены спиной друг другу
	float _avDir2youCoef;
	// угловой коэффиицент для снижения резкости звука между "панами" наушниками
	float _angle_coef;
};
