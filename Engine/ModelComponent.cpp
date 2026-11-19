#include "pch.h"
#include "ModelComponent.h"
#include "../Engine/Core/Factory.h"
#include "../Engine/Renderer/Uniforms.h"
#include "Framework/Actor.h"
#include "Resources/ResourceManager.h"
#include "Engine.h"
namespace nu {

	FACTORY_REGISTER(ModelComponent)

		void ModelComponent::Draw(const Renderer& renderer)
	{


	}

	void ModelComponent::Read(const json::value_t& value)
	{
	}

}
