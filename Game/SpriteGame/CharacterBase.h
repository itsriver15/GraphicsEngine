#pragma once
#include "Framework/Actor.h"

class CharacterBase :public Actor {
public:
	enum class CharState{
		Move,
		Attack,
		Hit,
		Death
	};
public:
	void Read(const json::value_t& value) override;
	float GetHealth() const { return m_health; }
	

protected:
	CharState m_state = CharState::Move;
	float m_stateTimer{ 0.0f };
	float m_health{ 10.0f };

};