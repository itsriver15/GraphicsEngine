#include "pch.h"
#include "../Framework/Scene.h"
#include "../Framework/Actor.h"
#include "../Core/Factory.h"
#include "../Renderer/Renderer.h"
#include "Components/CircleColliderComponent.h"


namespace nu {
	void Scene::AddActor(unique_ptr<Actor> actor){
		actor->m_scene = this;
		m_pendingActors.push_back(move(actor));

	}

	void Scene::RemoveAllActors(bool force)
	{
		if (force) {
			m_actors.clear();
		} else {
			erase_if(m_actors, [](auto& actor) { return !actor->GetPersistent();});
		}
	}
	bool Scene::Load(const std::string& sceneName)
	{
		json::document_t document;
		if (json::Load(sceneName, document))
		{
			if (JSON_HAS_NAME(document, "actors")) {
				for (auto& actorValue : document["actors"].GetArray()) {
					std::string typeName;
					JSON_READ_NAME(actorValue, "type", typeName);

					auto actor = Factory::Instance().Create<Actor>(typeName);


					if (!actor) {
						std::cout << "Could not create actor: " << typeName << std::endl;
						continue;
					}

					actor->Read(actorValue);

					bool prototype = false;
					JSON_READ(actorValue, prototype);

					if (prototype) {
						std::string name;
						JSON_READ(actorValue, name);

						Factory::Instance().RegisterPrototype<Actor>(name, std::move(actor));
					}
					else {
						std::string name;
						JSON_READ(actorValue, name);
						cout << "Adding Actor: " << name << endl;
						AddActor(std::move(actor));
					}
				}
		
			}
			
		}
		return false;
	}

	void Scene::Update(float dt) {
		//update actors
		for (auto& actor : m_actors) {
			actor->Update(dt);
		}
		//update collisions
		//UpdateCollisions();

		//remove destroyed actors
		erase_if(m_actors, [](auto& actor) {return actor->m_destroyed; });


		//add pending actors
		for (auto& actor : m_pendingActors) {
			actor->Start();
			m_actors.push_back(std::move(actor));
		}

		m_pendingActors.clear();
	}

	void Scene::Draw(const class Renderer& renderer) {
		for (auto& actor : m_actors) {
			if (actor) actor->Draw(renderer);
		}
	}

	void Scene::UpdateCollisions(){
		for (auto& actorA : m_actors) {
			for (auto& actorB : m_actors) {
				if (actorA == actorB || actorA->m_destroyed || actorB->m_destroyed) {
					continue;
				}

				auto colliderA = actorA->GetComponent<ColliderComponent>();
				auto colliderB = actorB->GetComponent<ColliderComponent>();

				if (!(colliderA && colliderB)) {
					continue;
				}
				if (colliderA->CheckCollision(*colliderB)) {
					actorA->OnCollision(actorB.get());
					actorB->OnCollision(actorA.get());
				}
			}
		}
	}
};