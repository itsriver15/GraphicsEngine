#pragma once
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Renderer/Texture.h"
#include "../Resources/ResourceManager.h"

#include <vector>
#include <memory>

using namespace std;

namespace nu {

	struct Particle
	{
		bool active{ false };
		float lifespan{ 1 };

		Vector2 position{ 0, 0 };
		Vector2 velocity{ 0, 0 };
		Color color{ 0, 0, 0 };
		res_t<Texture> texture = nullptr;
	};

	class ParticleSystem {

	public:
		ParticleSystem() = default;

		bool Initialize(size_t poolSize = 1000);
		void Shutdown();

		void Update(float dt);
		void Draw(const class Renderer& renderer);

		void AddParticle(const Particle& particle);

	private:
		Particle* GetFreeParticle();

	private:
		// store particles in particle pool
		vector<Particle> m_particles;

	};
}
