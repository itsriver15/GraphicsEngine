#pragma once
#include "Framework\Actor.h"

namespace nu {
	class PhysicsComponent;
	class SpriteAnimatorRendererComponent;
}


class PortalController : public Actor
{
public:
    CLASS_PROTOTYPE(PortalController)
    void Start() override;
    void Update(float dt) override;
    void OnCollision(Actor* other) override;

    void Read(const nu::json::value_t& value) override;

private:

    enum class State
    {
        Appearing,
        Idle,
        Closing
    };

    PhysicsComponent* m_physicsComponent = nullptr;
    SpriteAnimatorRendererComponent* m_rendererComponent = nullptr;

    State m_state = State::Idle;

    bool m_playerNearby = false;
};


