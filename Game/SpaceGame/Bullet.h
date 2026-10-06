#pragma once
#include "Framework/Actor.h"
struct BulletDesc : public nu::ActorDesc {
	float speed = 0.0f;
};
class Bullet : public nu::Actor {
public:
	Bullet() = default;
	Bullet(const BulletDesc& bulletDesc) : Actor{ bulletDesc } {
		m_speed = bulletDesc.speed;
	}

	CLASS_PROTOTYPE(Bullet)
	
	void Update(float dt) override;

	void SetSpeed(float speed) {
		m_speed = speed;
	}

	float GetSpeed() const {
		return m_speed;
	}

	virtual void OnCollision(Actor* other) override;


	float GetSpeed() { return m_speed; }


	virtual void Read(const json::value_t& value) override;

private:
	float m_speed = 400.0f;
};