#pragma once
#include "CharacterBase.h"


namespace nu {
	class PhysicsComponent;
	class SpriteAnimatorRendererComponent;
}
class FlyingEnemyController : public CharacterBase
{

	public:
		CLASS_PROTOTYPE(FlyingEnemyController)
		void Start() override;
		void Update(float dt) override;
		void OnCollision(nu::Actor* other) override;
		void Read(const nu::json::value_t& value) override;

	protected:
		PhysicsComponent* m_physicsComponent{ nullptr };
		SpriteAnimatorRendererComponent* m_rendererComponent{ nullptr };

};


