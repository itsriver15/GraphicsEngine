#include "FlyingEnemyController.h"
#include "Components/PhysicsComponent.h"
#include "Framework/Actor.h"
#include "Engine.h"
#include "Damager.h"
#include "Components/SpriteAnimatorRendererComponent.h"

using namespace nu;
FACTORY_REGISTER(FlyingEnemyController)

void FlyingEnemyController::Start()
{
	CharacterBase::Start();

	m_physicsComponent = GetComponent<nu::PhysicsComponent>();
	assert(m_physicsComponent);
	m_rendererComponent = GetComponent<nu::SpriteAnimatorRendererComponent>();
	assert(m_rendererComponent);
}

void FlyingEnemyController::Update(float dt)
{
	Vector2 velocity = m_physicsComponent->GetVelocity();

	switch (m_state) {
	case CharacterBase::CharState::Move:
	{
		auto player = m_scene->GetActorByName("PlayerPrototype");
		if (player) {
			Vector2 position = GetTransform().position;
			Vector2 playerPosition = player->GetTransform().position;

			Vector2 direction = playerPosition - position;

			if (direction.Length() < 20.0f) {
				m_state = CharState::Attack;
			}
			m_rendererComponent->SetFlipH(playerPosition.x < position.x);
			m_physicsComponent->ApplyForce(direction.Normalized() * 400.0f);
		}
	}
		break;
	case CharacterBase::CharState::Attack:
		break;
	case CharacterBase::CharState::Hit:
		if (m_rendererComponent->IsAnimationFinished()) {
			m_state = CharState::Move;
			m_rendererComponent->Play("idle");
		}
		break;
	case CharacterBase::CharState::Death:
		break;

	}


	CharacterBase::Update(dt);
}

void FlyingEnemyController::OnCollision(nu::Actor* other)
{
	if (EqualsIgnoreCase(other->GetTag(), "PlayerDamager")) {
		m_state = CharState::Hit;
		m_rendererComponent->Play("hit");
		Damager* damager = dynamic_cast<Damager*>(other);
		if (damager)
			m_health -= damager->GetDamage();
		if (m_health <= 0) {
			SetDestroyed();
		}
	}
}

void FlyingEnemyController::Read(const nu::json::value_t& value)
{
	CharacterBase::Read(value);
}
