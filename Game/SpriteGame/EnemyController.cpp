#include "EnemyController.h"
#include "Components/PhysicsComponent.h"
#include "Framework/Actor.h"
#include "Engine.h"
#include "Damager.h"
#include "Components/SpriteAnimatorRendererComponent.h"
#include "SpriteGame.h"

using namespace nu;
FACTORY_REGISTER(EnemyController)

void EnemyController::Start()
{
	CharacterBase::Start();

	m_physicsComponent = GetComponent<nu::PhysicsComponent>();
	assert(m_physicsComponent);
	m_rendererComponent = GetComponent<nu::SpriteAnimatorRendererComponent>();
	assert(m_rendererComponent);
}

void EnemyController::Update(float dt)
{
	
	Vector2 velocity = m_physicsComponent->GetVelocity();
	switch (m_state) {
		case CharacterBase::CharState::Move:
		{
			float dir = 0;
			auto player = m_scene->GetActorByName("PlayerPrototype");
			if (player) {
				Vector2 position = GetTransform().position;
				Vector2 playerPosition = player->GetTransform().position;
				Vector2 direction = playerPosition - position;

				m_rendererComponent->SetFlipH(playerPosition.x < position.x);

				if (playerPosition.x < position.x) {
					dir = -1.0f;
				}
				else {
					dir = 1.0f;
				}


				if (dir != 0.0f) {
					velocity.x = dir * 35.0f;
					m_rendererComponent->Play("run");
				}
				else {
					m_rendererComponent->Play("idle");
				}

				if (direction.Length() < 100.0f) {
					m_state = CharState::Attack;
					m_hasAttacked = false;
					m_rendererComponent->Play("attack");

					
				}

			}
			else {
				m_rendererComponent->Play("idle");
			}

		}
		break;
		case CharacterBase::CharState::Attack:
			if (m_hasAttacked == false && m_rendererComponent->GetFrame() == 3) {
				m_hasAttacked = true;
				auto damager = Factory::Instance().Create<Damager>("DamagerPrototype");
				damager->SetDamage(3.0f);
				damager->SetPosition(GetTransform().position);
				damager->SetScale(3.0f);
				damager->SetTag("EnemyDamager");
				m_scene->AddActor(std::move(damager));
			}
			if (m_rendererComponent->IsAnimationFinished()) {
				m_state = CharState::Move;
				m_rendererComponent->Play("idle");
			}
		break;
		case CharacterBase::CharState::Hit:
			if (m_rendererComponent->IsAnimationFinished()) {
				if (m_health > 0) {
					m_state = CharState::Move;
					m_rendererComponent->Play("idle");
				}
				else {
					m_state = CharState::Death;
				}
			}
		break;
		case CharacterBase::CharState::Death:
			SetDestroyed();
		break;
	}


	m_physicsComponent->SetVelocity(velocity);

	CharacterBase::Update(dt);
}

void EnemyController::OnCollision(nu::Actor* other)
{
	
	if (EqualsIgnoreCase(other->GetTag(), "PlayerDamager")) {
	
		//hit enemy
		m_state = CharState::Hit;
		Engine::Get().GetAudio().PlaySound("hit");
		m_rendererComponent->Play("hit");

		Damager* damager = dynamic_cast<Damager*>(other);

		m_health -= damager->GetDamage();

		if (m_health <= 0) {
			auto game = dynamic_cast<SpriteGame*>(m_scene->GetGame());

			if (game) {
				game->AddPoints(100);
			}

		}

		//remove damager
		other->SetDestroyed();
	}
}

void EnemyController::Read(const nu::json::value_t& value)
{
	CharacterBase::Read(value);
}
