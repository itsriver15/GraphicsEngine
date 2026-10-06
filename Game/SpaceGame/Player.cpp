#include "Engine.h"
#include "../Engine/Resources/ResourceManager.h"
#include "SpaceGame.h"
#include "Bullet.h"
#include "Player.h"
#include "Components/PhysicsComponent.h"


#include <memory>

using namespace nu;

FACTORY_REGISTER(Player)

void Player::Update(float dt){
    float thrust = 0.0f;
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)) thrust = m_speed;
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) thrust = -m_speed;

    float rotate = 0.0f;
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) rotate = -180.0f;
    if (nu::Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) rotate = +180.0f;

    SetRotation(m_transform.rotation + rotate * dt);

    auto physicsComponent = GetComponent<nu::PhysicsComponent>();
    if (physicsComponent) {
        Vector2 forward{ 1.0f, 0.0f };
        Vector2 force = forward.Rotate(m_transform.rotation * DegToRad) * thrust;

        physicsComponent->ApplyForce(force);
        physicsComponent->ApplyTorque(rotate);

        Vector2 position = physicsComponent->GetPosition();

        Engine::Get().GetRenderer().SetCamera(position);
    }



    nu::Vector2 forward{ 1.0f,0.0f };
    nu::Vector2 velocity = forward.Rotate((DegToRad * m_transform.rotation)) * thrust;

    AddVelocity(velocity * dt);
 
     //particle system
    if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)) {
        nu::Particle particle;
        particle.position = m_transform.position;
        particle.color = { 1.0f, 1.0f, 1.0f };
        particle.lifespan = RandomFloat(0.5f, 1.5f);
        particle.velocity = { RandomFloat(-200.0f, 200.0f), RandomFloat(-200.0f, 200.0f) };
        particle.texture = Resources().Get<Texture>("textures/particle.png", Engine::Get().GetRenderer());

   
        Engine::Get().GetPS().AddParticle(particle);
    }

    //fire
    if (nu::Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_SPACE)) {
        auto actor = Factory::Instance().Create<Actor>("BulletPrototype");
        Transform transform{ {m_transform.position}, m_transform.rotation, 1.0f };
        actor->SetTransform(transform);

        m_scene->AddActor(move(actor));
    };

    //movement
    if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_F))
    {
        SetPosition(Vector2{RandomFloat(0.0f, 1280.0f),RandomFloat(0.0f, 1024.0f)});
    }
    if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_LSHIFT))
    {
       SetSpeed(3000.0f);
    }
    else if (Engine::Get().GetInput().GetKeyReleased(SDL_SCANCODE_LSHIFT))
    {
       SetSpeed(1000.0f);// original speed
    }
    if (Engine::Get().GetInput().GetKeyPressed(SDL_SCANCODE_R))
    {
        SetRotation(m_transform.rotation + 180.0f);
    }

    Actor::Update(dt);
}

void Player::OnCollision(Actor* other) {

    Color color1 = { 1.0f, 1.0f, 1.0f };
    Color color2 = { 0.0f, 1.0f, 1.0f };
    Color color3 = { 1.0f, 0.0f, 1.0f };

 

    if (other->GetTag() == "Enemy") {

        SetDestroyed();
        ((SpaceGame*)m_scene->GetGame())->OnPlayerDead();

        for (int i = 0; i < 100; i++)
        {
            nu::Particle particle;
            particle.position = m_transform.position;
            int randomInt = RandomInt(0, 3);
            switch (randomInt) {
            case 0:
                particle.color = color1;
                break;
            case 1:
                particle.color = color2;
                break;
            case 2:
                particle.color = color3;
                break;
            default:
                particle.color = color1;
                break;
            }
            particle.lifespan = nu::RandomFloat(0.5f, 2.0f);
            particle.velocity = { nu::RandomFloat(-600.0f, 600.0f), nu::RandomFloat(-600.0f, 600.0f) };

            particle.texture = Resources().Get<Texture>("textures/particle.png", Engine::Get().GetRenderer());

            nu::Engine::Get().GetPS().AddParticle(particle);
        }
    }
    
}

void Player::Read(const json::value_t& value)
{
    Actor::Read(value);

    JSON_READ_NAME(value, "speed", m_speed);

}
