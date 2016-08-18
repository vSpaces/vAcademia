#include "stdafx.h"
#include "SmokeOfFire.h"

const float BOUND_FADE = 0.1f;
const float EMMIT_RADIUS = 0.1f;
const float INITIAL_SIZE_MIN_MAX_K = 0.25f;

ParticleDynamic_SmokeOfFire::ParticleDynamic_SmokeOfFire(nrmParticles & particles) : 
	IParticlesDynamic(particles) 
{}

void ParticleDynamic_SmokeOfFire::dynamic(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)
{	
	CQuaternion rot(randf2(), randf2(), randf2(), 0.0f);
	desc.dir = lerp<CVector3D>(desc.dir, desc.dir * rot, particles.m_dirChangeFactor);
	desc.dir.Normalize();

	float newSpeed = lerp<float>(particles.m_speed.min, particles.m_speed.max, randf());
	desc.speed = lerp<float>(desc.speed, newSpeed, particles.m_speedChangeFactor);
	vert.pos += multToLocalSpeed(desc.dir, desc.speed) * t;

	vert.size = desc.sizeInitial + (particles.m_size.max - particles.m_size.min) * vert.pos.z;

	vert.opacity = lerp<float>(desc.opacityInitial, particles.m_opacity.min, vert.pos.z);
	float fade = BoundFade(vert.pos.x + 0.5f, BOUND_FADE) 
		* BoundFade(vert.pos.y + 0.5f, BOUND_FADE) 
		* BoundFade(vert.pos.z, BOUND_FADE);
	vert.opacity *= fade;
}

void ParticleDynamic_SmokeOfFire::dead(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)
{
	if (vert.pos.z > 1.0f || vert.pos.z < -1.0f
		|| abs(vert.pos.x) > 0.5f 
		|| abs(vert.pos.y) > 0.5f)
	{
		desc.isLive = false;		
		desc.wasLive = particles.m_countLived < particles.m_mustBeParticlesCount;
		vert.opacity = 0.0f;
		vert.size = 0.0f;
	}
}

void ParticleDynamic_SmokeOfFire::emit(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)
{
	vert.pos = CVector3D(
		lerp<float>(-EMMIT_RADIUS, EMMIT_RADIUS, randf()),
		lerp<float>(-EMMIT_RADIUS, EMMIT_RADIUS, randf()),
		0.0f);
	desc.dir = CVector3D(0, 0, 1);
	desc.speed = lerp<float>(particles.m_speed.min, particles.m_speed.max, randf());
	vert.size = desc.sizeInitial = lerp<float>(particles.m_size.min, particles.m_size.max, randf() * INITIAL_SIZE_MIN_MAX_K);
	desc.opacityInitial = particles.m_opacity.max;
	vert.opacity = 0.0f;
}

void ParticleDynamic_SmokeOfFire::alive(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)
{
	if (desc.wasLive || desc.time > 1.0f 
		|| (particles.m_countLived == 0 && particles.m_mustBeParticlesCount > 1)) 
	{
		desc.isLive = true;						
		desc.time = 0.0f;
	} else
	{
		if (desc.time > 0.0f) //обновляем отсчет времени до начала жизни
		{
			desc.time += localSpeedZ(desc.speed) * t;
		} else { //запускаем отсчет времени до начала жизни
			desc.time = randf();
			desc.speed = lerp<float>(particles.m_speed.min, particles.m_speed.max, randf());
		}
	}
}