#include "stdafx.h"
#include "SingleSmoke.h"

const float BOUND_FADE = 0.2f;
const float TIME_TO_APPEARANCE = 2.0f;

ParticleDynamic_SingleSmoke::ParticleDynamic_SingleSmoke(nrmParticles & particles) : 
	IParticlesDynamic(particles),
	m_countDying(0)
{}

void ParticleDynamic_SingleSmoke::dynamic(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)
{	
	CQuaternion rot(randf2(), randf2(), randf2(), 0.0f);
	desc.dir = lerp<CVector3D>(desc.dir, desc.dir * rot, particles.m_dirChangeFactor);
	desc.dir.Normalize();

	float newSpeed = lerp<float>(particles.m_speed.min, particles.m_speed.max, randf());
	desc.speed = lerp<float>(desc.speed, newSpeed, particles.m_speedChangeFactor);
	vert.pos += multToLocalSpeed(desc.dir, desc.speed) * t;

	vert.size = desc.sizeInitial;

	float opacityZ = min(1.0 - BOUND_FADE, vert.pos.z) / (1.0f - BOUND_FADE);
	vert.opacity = lerp<float>(desc.opacityInitial, particles.m_opacity.min, opacityZ);
	if (desc.state == State::APPEARANCE || desc.state == State::DYING) {
		desc.time += t/TIME_TO_APPEARANCE;
		if (desc.time > 1.0f) {
			if (desc.state == State::APPEARANCE) {
				desc.state = State::NONE;
			}
		} else {
			vert.opacity *= (desc.state == State::APPEARANCE) ? desc.time : 1.0f - desc.time;
		}
	}
	
	float fade = BoundFade(vert.pos.x + 0.5f, BOUND_FADE) 
		* BoundFade(vert.pos.y + 0.5f, BOUND_FADE) 
		* BoundFade(vert.pos.z, BOUND_FADE);
	vert.opacity *= fade;
}

void ParticleDynamic_SingleSmoke::dead(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)
{
	if (vert.pos.z > 1.0f
		|| vert.pos.z < 0.0f
		|| abs(vert.pos.x) > 0.5f 
		|| abs(vert.pos.y) > 0.5f
		|| (desc.state == State::DYING && desc.time > 1.0f))
	{
		desc.isLive = false;
		desc.state = State::NONE;
		desc.wasLive = particles.m_countLived < particles.m_mustBeParticlesCount;
		desc.sizeInitial = 0.0f;
		vert.opacity = 0.0f;
		vert.size = 0.0f;
		m_countDying = max(0, m_countDying - 1);
	} else {
		int mustDie = particles.m_countLived - particles.m_mustBeParticlesCount;
		if (desc.state == State::DYING) //обновляем отсчет времени до смерти
		{
			if (m_countDying > mustDie) {
				--m_countDying;
				desc.state = State::APPEARANCE;
			}
		} else {
			if (m_countDying < mustDie && desc.state == State::NONE) { //запускаем отсчет времени до смерти
				++m_countDying;
				desc.state = State::DYING;
				desc.time = 0.0f;
				desc.speed = lerp<float>(particles.m_speed.min, particles.m_speed.max, randf());
			}			
		}
	}
}

void ParticleDynamic_SingleSmoke::emit(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)
{
	vert.pos = CVector3D(randf2()/2.0f, randf2()/2.0f, randf());
	desc.dir = CVector3D(randf2(), randf2(), randf2());
	desc.dir.Normalize();

	desc.speed = lerp<float>(particles.m_speed.min, particles.m_speed.max, randf());
	vert.size = desc.sizeInitial = lerp<float>(particles.m_size.min, particles.m_size.max, randf());
	desc.opacityInitial = particles.m_opacity.max;
	vert.opacity = 0.0f;
}

void ParticleDynamic_SingleSmoke::alive(nrmParticles::ParticleDesc & desc, nrmParticles::ParticleVertex & vert, float t)
{
	if (desc.wasLive || (desc.state == State::BIRTH && desc.time > 1.0f))
	{
		desc.isLive = true;
		desc.state = State::APPEARANCE;
		desc.time = 0.0f;
	} else
	{
		if (desc.state == State::BIRTH) //обновляем отсчет времени до начала жизни
		{
			desc.time += localSpeedZ(desc.speed) * t;
		} else { //запускаем отсчет времени до начала жизни
			desc.state = State::BIRTH;
			desc.time = randf();
			desc.speed = lerp<float>(particles.m_speed.min, particles.m_speed.max, randf());
		}
	}
}