#pragma once
#include "Component.h"

namespace nu
{
	class CameraControllerComponent : public Component
	{
	public:
		CLASS_PROTOTYPE(CameraControllerComponent)

		void Update(float dt) override;

	private:

	};
}
