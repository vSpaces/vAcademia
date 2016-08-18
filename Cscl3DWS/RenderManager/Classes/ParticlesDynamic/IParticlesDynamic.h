#pragma once
#include "nrmParticles.h"

class IParticlesDynamic
{
public:
	IParticlesDynamic(nrmParticles & _particles) : particles(_particles) {}
	virtual ~IParticlesDynamic(){};

	virtual void dynamic(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)=0;
	virtual void dead(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)=0;
	virtual void emit(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)=0;
	virtual void alive(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)=0;

protected:
	nrmParticles & particles;

	static float randf() //от 0 до 1
	{
		return (float)rand() / RAND_MAX;
	}

	static float randf2() //от -1 до 1
	{
		return randf()*2.0f-1.0f;
	}

	template<typename T>
	static T lerp(T const& a, T const& b, float t)
	{
		return a + (b - a) * t;
	}

	float localSpeedZ(float s) const {
		float x, y, z;
		particles.m_obj3d->GetScale(&x, &y, &z);
		return s / z;
	}
	
	CVector3D multToLocalSpeed(CVector3D const& p, float s) const {
		CVector3D size = particles.m_obj3d->GetScale3D();
		return CVector3D(p.x * s / size.x, p.y * s / size.y, p.z * s / size.z);
	}

	static float BoundFade(float x, float border) {
		return BoundFade(x, border, border);
	}

	static float BoundFade(float x, float borderUp, float borderDown) {
		x = min(1.0f, max(0.0f, x));
		if (x < borderUp)
			return x / borderUp;
		if (x > 1.0f - borderDown)
			return (1.0f - x) / borderDown;
			//return 1.0f - (x - BOUND_FADE2) / BOUND_FADE;
		return 1.0f;
	}

private:
	IParticlesDynamic();
};

