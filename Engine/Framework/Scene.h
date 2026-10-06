#pragma once
#include "../Framework/Actor.h"

#include <memory>
#include <vector>
#include <string>


using namespace std;

namespace nu {
	class Actor;
	class Scene {
	public:

		void AddActor(unique_ptr<Actor> actor);
		void RemoveAllActors(bool force = false);

		bool Load(const std::string& sceneName);

		void Update(float dt);
		void Draw(const class Renderer& renderer);

		template<typename T = Actor>
		T* GetActorByName(const string& name);

		void SetGame(class Game* game) {
			m_game = game;
		}

		class Game* GetGame() { return m_game; }

		string GetName() { return m_name; }
		void SetName(const string& name) { m_name = name; }


	private:
		void UpdateCollisions();


	private:
		vector<unique_ptr<Actor>> m_actors;
		vector<unique_ptr<Actor>> m_pendingActors;
		string m_name;

		class Game* m_game = nullptr;
	};

	template<typename T>
	inline T* Scene::GetActorByName(const string& name) {
		for (auto& actor : m_actors) {
			T* actorT = dynamic_cast<T*>(actor.get());
			if (actorT && actorT->m_name == name) return actorT;
		}
		return nullptr;
	}
}