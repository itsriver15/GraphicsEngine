#include "PlayerController.h"
#include "Components/PhysicsComponent.h"
#include "Framework/Actor.h"
#include "Engine.h"
#include "Damager.h"
#include "Components/SpriteAnimatorRendererComponent.h"

using namespace nu;

FACTORY_REGISTER(PlayerController)

void PlayerController::Start()
{
    CharacterBase::Start();

    m_physicsComponent = GetComponent<PhysicsComponent>();
    assert(m_physicsComponent);

    m_rendererComponent = GetComponent<SpriteAnimatorRendererComponent>();
    assert(m_rendererComponent);

    m_rendererComponent->SetFlipH(true);

}

void PlayerController::Update(float dt)
{
    Vector2 velocity = m_physicsComponent->GetVelocity();

    switch (m_state) {
    case CharacterBase::CharState::Move:
    {
        float dir = 0;
        

        if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) {
            dir = -1.0f;
            m_rendererComponent->SetFlipH(false);
        }

        if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) {
            dir = +1.0f;
            m_rendererComponent->SetFlipH(true);
        }

        if (nu::Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_SPACE)
            && m_canJump)
        {
            Engine::Get().GetAudio().PlaySound("jump");
            velocity.y = -400.0f;
            m_canJump = false;
        }

        if (dir != 0) {
            velocity.x = dir * 100;
            m_rendererComponent->Play("run");
        }
        else {
            m_rendererComponent->Play("idle");
        }

        if (nu::Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_F)) {
            m_state = CharState::Attack;
            m_rendererComponent->Play("attack");

            auto damager =
                Factory::Instance().Create<Actor>("DamagerPrototype");

            Vector2 offset;

            if (m_rendererComponent->GetFlipH()) {
                offset = Vector2{ 40.0f, 0.0f };
            }
            else {
                offset = Vector2{ -40.0f, 0.0f };
            }

            damager->SetPosition(GetTransform().position + offset);
            damager->SetTag("PlayerDamager");
            m_scene->AddActor(move(damager));
        }
    }
    break;

    case CharacterBase::CharState::Attack:
    {
        if (m_rendererComponent->IsAnimationFinished()) {
            m_state = CharState::Move;
            m_rendererComponent->Play("idle");
        }
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

void PlayerController::OnCollision(nu::Actor* other)
{
    // Enemy attack
    if (EqualsIgnoreCase(other->GetTag(), "EnemyDamager")) {

        m_state = CharState::Hit;

        Engine::Get().GetAudio().PlaySound("hit");
        m_rendererComponent->Play("hit");

        Damager* damager = dynamic_cast<Damager*>(other);

        m_health -= damager->GetDamage();

        other->SetDestroyed();
    }

    // Tilemap / ground collision
    if (EqualsIgnoreCase(other->GetTag(), "ground") || EqualsIgnoreCase(other->GetTag(), "prop")) {
        m_canJump = true;
    }
}

void PlayerController::Read(const nu::json::value_t& value)
{
    CharacterBase::Read(value);
}