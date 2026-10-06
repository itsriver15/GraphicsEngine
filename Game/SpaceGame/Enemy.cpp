#include "Enemy.h"
#include "Player.h"
#include "SpaceGame.h"
#include "Engine.h" 
#include "Components/PhysicsComponent.h"

FACTORY_REGISTER(Enemy)

void Enemy::Update(float dt) {
	Player* player = m_scene->GetActorByName<Player>("PlayerPrototype");
	if (player) {
		auto physicsComponent = GetComponent<nu::PhysicsComponent>();
		if (physicsComponent) {
			Vector2 forward{ 1.0f, 0.0f };
			Vector2 force = forward.Rotate(m_transform.rotation * DegToRad) * m_speed;

			physicsComponent->ApplyForce(force);

			Vector2 direction = player->GetTransform().position - m_transform.position;
			float rotation = direction.Angle();
			physicsComponent->SetRotation(rotation * DegToRad);

			Vector2 position = physicsComponent->GetPosition();

			position.x = Wrap(0.0f, 1280.0f, position.x);
			position.y = Wrap(0.0f, 1024.0f, position.y);

			physicsComponent->SetPosition(position);
		}

		Actor::Update(dt);


	}
}

void Enemy::OnCollision(Actor* other) {

	if (other->GetTag() == "Bullet") {
		SetDestroyed();
		other->SetDestroyed();
		cout << "Destroyed" << endl;
		if (m_scene && m_scene->GetGame())
		{
			((SpaceGame*)m_scene->GetGame())->AddPoints(100);
		}

		Engine::Get().GetAudio().PlaySound("explosion");

		// create particle explosion
		for (int i = 0; i < 100; i++)
		{
			nu::Particle particle;
			particle.position = m_transform.position;
			particle.color = { 1.0f, 0.0f, 0.0f };
			particle.lifespan = nu::RandomFloat(0.5f, 2.0f);
			particle.velocity = { nu::RandomFloat(-600.0f, 600.0f), nu::RandomFloat(-600.0f, 600.0f) };
			particle.texture = Resources().Get<Texture>("textures/particle.png", Engine::Get().GetRenderer());

			nu::Engine::Get().GetPS().AddParticle(particle);
		}
	}
}

void Enemy::Read(const json::value_t& value)
{
	Actor::Read(value);

	JSON_READ_NAME(value, "speed", m_speed);
	JSON_READ_NAME(value, "points", m_points);
	JSON_READ_NAME(value, "health", m_health);

}