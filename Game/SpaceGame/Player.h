#pragma once
#include "Framework/Actor.h"

struct PlayerDesc : public nu::ActorDesc {
	float speed = 0.0f;


};
class Player : public nu::Actor {

public:
	Player() = default;
	Player(const PlayerDesc& playerDesc) : Actor{ playerDesc }{
	
		m_speed = playerDesc.speed;
	}

	CLASS_PROTOTYPE(Player)

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
	float m_speed = 0.0f;
};