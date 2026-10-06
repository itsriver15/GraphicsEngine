#pragma once
#include "Framework/Actor.h"
struct EnemyDesc : public nu::ActorDesc {
	int points = 0;
	float health = 0;
	float speed = 0.0f;
};
class Enemy : public nu::Actor {
public:
	Enemy() = default;
	Enemy(const EnemyDesc& enemyDesc) : Actor{ enemyDesc } {	
		m_speed = enemyDesc.speed;
		m_health = enemyDesc.health;
		m_points = enemyDesc.points;
	}

	CLASS_PROTOTYPE(Enemy)

	void Update(float dt) override;
	void SetSpeed(float speed) {
		m_speed = speed;
	}

	float GetSpeed() const {
		return m_speed;
	}

	void OnCollision(Actor* other) override;


	virtual void Read(const json::value_t& value) override;

private:
	int m_points = 100;
	float m_health = 1.0f;
	float m_speed = 800.0f;
};