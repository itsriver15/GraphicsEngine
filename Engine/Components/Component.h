#pragma once
#include "../Framework/Object.h"

namespace nu {

	class Actor;

	class Component : public Object {
	public:
		CLASS_PROTOTYPE(Component)

		Component() = default;
		~Component() = default;

		virtual void Start() {}
		virtual void Destroyed() {}


		virtual void Update(float dt) {}

		Actor* GetOwner() const { return m_owner; }
		void SetOwner(Actor* owner) { m_owner = owner; }

	protected:
		Actor* m_owner = nullptr;

	};
}