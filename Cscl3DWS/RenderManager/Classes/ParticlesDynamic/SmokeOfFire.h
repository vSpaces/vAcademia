#pragma once
#include "IParticlesDynamic.h"

class ParticleDynamic_SmokeOfFire : public IParticlesDynamic {
public:
	ParticleDynamic_SmokeOfFire(nrmParticles & particles);

	virtual void dynamic(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t);
	virtual void dead(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t);
	virtual void emit(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t);
	virtual void alive(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t);
};