#include "pch.h"
#include "SpriteAnimationRendererComponent.h"
#include "Renderer/TextureFrames.h"
#include "Engine.h"
#include "Math/MathUtils.h"

namespace nu {

	FACTORY_REGISTER(SpriteAnimationRendererComponent)
	
	void SpriteAnimationRendererComponent::Update(float dt)
	{
		if (!m_textureFrames) {
			std::cerr << "Texture frames not set!" << std::endl;
			return;
		}

		m_frameTimer += dt;
		float frameTime = 1.0f / m_framesPerSecond;

		//increase frame while frame timer is greater than frame time
		while (m_frameTimer >= frameTime) {
			m_currentFrame++;
			if (m_loop) {
				//loop frames
				m_currentFrame = m_currentFrame % m_textureFrames->GetTotalFrames();
			}
			else {
				//stop on last frame
				m_currentFrame = Clamp(0u, m_textureFrames->GetTotalFrames() - 1, m_currentFrame);
			}

			m_frameTimer -= frameTime;
		}
		m_sourceRect = m_textureFrames->GetFrameRect(m_currentFrame);
	}

	void SpriteAnimationRendererComponent::Start()
	{
		if (!m_textureFramesName.empty()) {
			m_textureFrames = Resources().Get<TextureFrames>(m_textureFramesName, Engine::Get().GetRenderer());

			if (m_textureFrames) {
				m_sourceRect = m_textureFrames->GetFrameRect(0);
				m_size = Vector2{ m_sourceRect.w, m_sourceRect.h };
				m_texture = m_textureFrames->GetTexture();
			}

		} else {
			cerr << "Could not load texture frames: " << m_textureFramesName << endl;
		}
	}


	void SpriteAnimationRendererComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		JSON_READ_NAME_REQ(value, "frames_per_second", m_framesPerSecond);
		JSON_READ_NAME(value, "loop", m_loop);
		JSON_READ_NAME_REQ(value, "texture_frames", m_textureFramesName);



	}
}
