#pragma once
#include "SpriteRendererComponent.h"
#include "Resources/ResourceManager.h"

namespace nu {
	class SpriteAnimationRendererComponent : public SpriteRendererComponent {
	public:
		CLASS_PROTOTYPE(SpriteAnimationRendererComponent)

	void Update(float dt) override;
	void Start() override;

	void Read(const json::value_t& value) override;

	private:
		string m_textureFramesName;

		float m_framesPerSecond = 1.0f;
		bool m_loop = true;
		
		unsigned int m_currentFrame;

		float m_frameTimer = 0;


		res_t<class TextureFrames> m_textureFrames;
	};
}
