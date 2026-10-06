#include "PortalController.h"
#include "Components/PhysicsComponent.h"
#include "Components/SpriteAnimatorRendererComponent.h"
#include "Engine.h"
#include "SpriteGame.h"

using namespace nu;

FACTORY_REGISTER(PortalController)

void PortalController::Start()
{
	Actor::Start();

	m_physicsComponent = GetComponent<PhysicsComponent>();
	assert(m_physicsComponent);

	m_rendererComponent = GetComponent<SpriteAnimatorRendererComponent>();
	assert(m_rendererComponent);

	m_rendererComponent->Play("idle");
}

void PortalController::Update(float dt)
{
	switch (m_state)
	{
	case State::Appearing:
		if (m_rendererComponent->IsAnimationFinished())
		{
			m_state = State::Idle;
			m_rendererComponent->Play("idle");
		}
		break;

	case State::Idle:
	{
		if (m_playerNearby &&
			Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_E))
		{
			m_state = State::Closing;
			m_rendererComponent->Play("close");
		}
	}
	break;

	case State::Closing:
	{
		if (m_rendererComponent->IsAnimationFinished())
		{
			auto game = dynamic_cast<SpriteGame*>(m_scene->GetGame());

			if (game)
			{
				game->NextLevel();
			}
		}
	}
	break;
	}

	Actor::Update(dt);
}

void PortalController::OnCollision(Actor* other)
{
	if (EqualsIgnoreCase(other->GetTag(), "Player"))
	{
		m_playerNearby = true;
	}
}

void PortalController::Read(const nu::json::value_t& value)
{
	Actor::Read(value);
}