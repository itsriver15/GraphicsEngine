#include "pch.h"
#include "../Renderer/ParticleSystem.h"
#include "../Renderer/Renderer.h"

namespace nu {
	bool ParticleSystem::Initialize(size_t size)
	{
		// reserve # particles in container
		m_particles.resize(size);

		return true;
	}

	void ParticleSystem::Shutdown()
	{
		// clear all particle
		m_particles.clear();
	}

	void ParticleSystem::Update(float dt)
	{
		// update active particles
		for (auto& particle : m_particles)
		{
			if (particle.active == false) continue;

			// reduce particle.lifespan by subtracting delta time
			particle.lifespan -= dt;

			// set active if (particle.lifespan > 0);
			particle.active = (particle.lifespan > 0);

			// update position with velocity (multiply by dt)
			
			particle.position += particle.velocity * dt;
		}
	}

	void ParticleSystem::Draw(const Renderer& renderer)
	{
		// draw all active particles
		for (auto& particle : m_particles)
		{
			if (particle.active)
			{
				// set particle color and draw point at current position
				renderer.SetColor(particle.color.r, particle.color.g, particle.color.b);
				renderer.DrawTexture(*particle.texture, particle.position.x, particle.position.y, 0.0f, 0.3f, false);
			}
		}
	}

	void ParticleSystem::AddParticle(const Particle& particle)
	{
		// get free particle
		Particle* freeParticle = GetFreeParticle();
		// check if free particle is not nullptr
		if (freeParticle)
		{
			// set free particle object with particle
			*freeParticle = particle;
			// set particle active
			freeParticle->active = true;
		}
	}

	Particle* ParticleSystem::GetFreeParticle()
	{
		// find and return inactive particle 
		for (auto& particle : m_particles)
		{
			// return pointer to inactive particle
			if (!particle.active)
				return &particle;
		}

		// no free particles, return nullptr
		return nullptr;
	}
}