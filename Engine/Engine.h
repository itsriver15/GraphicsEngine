#pragma once


#include "../Engine/Core/Random.h"

//math
#include "../Engine/Math/Vector2.h"
#include "../Engine/Math/MathUtils.h"
#include "../Engine/Math/Transform.h"

//renderer
#include "../Engine/Renderer/Renderer.h"
#include "../Engine/Renderer/Mesh.h"
#include "../Engine/Renderer/Text.h"
#include "../Engine/Renderer/ParticleSystem.h"
#include "../Engine/Renderer/Texture.h"

//input
#include "../Engine/Input/Input.h"

//core
#include "../Engine/Core/GameTime.h"
#include "../Engine/Core/File.h"
#include "../Engine/Core/Factory.h"
#include "../Engine/Core/StringUtils.h"

//audio
#include "../Engine/Audio/Audio.h"

//serialization
#include "../Engine/Serialization/Json.h"

//framework
#include "../Engine/Framework/Object.h"

//resources
#include "../Engine/Resources/ResourceManager.h"

//physics
#include "../Engine/Physics/Physics.h"

//framework
#include "../Engine/Framework/Actor.h"
#include "../Engine/Framework/Scene.h"
#include "../Engine/Framework/Game.h"

#include <iostream>
#include <vector>

namespace nu {
	class Engine /*: public Singleton<Engine> */ {
	public:
		static Engine& Get() { static Engine engine; return engine; }

		bool Initialize();
		void Shutdown();

		void Update();

		Input& GetInput() { return m_input; }
		Renderer& GetRenderer() { return m_renderer; }

		Time& GetTime() { return m_time; }
		Audio& GetAudio() { return m_audio; }
		ParticleSystem& GetPS() { return m_particleSystem; }
		Physics& GetPhysics() { return m_physics; }

	private:
		Engine() = default;
	protected:
		Audio m_audio;
		Input m_input;
		Renderer m_renderer;
		Time m_time;
		ParticleSystem m_particleSystem;
		Physics m_physics;
	};
	extern Engine engine;
}


