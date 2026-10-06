#include "Bullet.h"
#include "Math/MathUtils.h"
#include "Core/Factory.h"
#include "Components/PhysicsComponent.h"

FACTORY_REGISTER(Bullet)

void Bullet::Update(float dt) {
	auto physicsComponent = GetComponent<nu::PhysicsComponent>();
	if (physicsComponent) {

		nu::Vector2 forward{ 1.0f, 0.0f };
		nu::Vector2 force = forward.Rotate(m_transform.rotation * nu::DegToRad) * m_speed;

		physicsComponent->SetVelocity(force);

		Vector2 position = physicsComponent->GetPosition();

		position.x = Wrap(0.0f, 1280.0f, position.x);
		position.y = Wrap(0.0f, 1024.0f, position.y);

		physicsComponent->SetPosition(position);
	}


	Actor::Update(dt);
}

void Bullet::OnCollision(Actor* other) {
	
	if (GetTag() == "PlayerBullet") {
		if (other->GetName() == "Enemy") {
			other->SetDestroyed();
			SetDestroyed();
		}
	}
	else if (GetTag() == "bullet") {
		if (other->GetName() == "Player") {
			other->SetDestroyed();
			SetDestroyed();
		}
	}
	
	
}

void Bullet::Read(const json::value_t& value) {

	Actor::Read(value);

	JSON_READ_NAME(value, "speed", m_speed);

}