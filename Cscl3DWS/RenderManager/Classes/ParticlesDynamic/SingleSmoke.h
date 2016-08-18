#pragma once
#include "IParticlesDynamic.h"

class ParticleDynamic_SingleSmoke : public IParticlesDynamic {
public:
	ParticleDynamic_SingleSmoke(nrmParticles & particles);

	virtual void dynamic(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t);
	virtual void dead(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t);
	virtual void emit(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t);
	virtual void alive(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t);

private:
	enum State {
		NONE = 0,
		BIRTH,
		APPEARANCE,
		DYING
	};

	int m_countDying;
};