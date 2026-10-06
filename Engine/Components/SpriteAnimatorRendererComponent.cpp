#include "pch.h"
#include "SpriteAnimatorRendererComponent.h"
#include "Engine.h"
#include "Math/MathUtils.h"
#include "Renderer/TextureFrames.h"

namespace nu {
	FACTORY_REGISTER(SpriteAnimatorRendererComponent)

	void SpriteAnimatorRendererComponent::Update(float dt)
	{
		if (!m_currentAnimation.textureFrames) { return; }
	
		m_frameTimer += dt;
		float frameTime = 1.0f / m_currentAnimation.framesPerSecond;

		//increase frame while frame timer is greater than frame time
		while (m_frameTimer >= frameTime) {
			m_currentFrame++;
			if (m_currentAnimation.loop) {
				//loop frames
				m_currentFrame = m_currentFrame % m_currentAnimation.textureFrames->GetTotalFrames();
			} else {
				//stop on last frame
				m_currentFrame = Clamp(0u, m_currentAnimation.textureFrames->GetTotalFrames() - 1, m_currentFrame);
			}
			

			m_frameTimer -= frameTime;

		}
		m_sourceRect = m_currentAnimation.textureFrames->GetFrameRect(m_currentFrame);
	}
	void SpriteAnimatorRendererComponent::Start()
	{
		if (!m_defaultAnimationName.empty()) {
			Play(m_defaultAnimationName);
		} else {
			auto iter = m_spriteAnimations.begin();
			string name = iter->first;
			Play(name);
		}
	}
	void SpriteAnimatorRendererComponent::Read(const json::value_t& value)
	{
		SpriteRendererComponent::Read(value);

		JSON_READ_NAME(value, "default_animation", m_defaultAnimationName);

		if (JSON_HAS(value, animations) && JSON_GET(value, animations).IsArray())
		{
			for (auto& animatorValue : JSON_GET(value, animations).GetArray())
			{
				SpriteAnimation spriteAnimation;

				std::string textureAnimationName;

				JSON_READ_NAME(animatorValue, "name", spriteAnimation.name);

				std::string texture_frames;
				JSON_READ_REQ(animatorValue, texture_frames);

				if (!texture_frames.empty())
				{
					spriteAnimation.textureFrames = Resources().Get<TextureFrames>(texture_frames, Engine::Get().GetRenderer());
					if (!spriteAnimation.textureFrames)
					{
						std::cerr << "Could not load texture frames: " << texture_frames << std::endl;
					}
				}

				JSON_READ_NAME_REQ(animatorValue, "frames_per_second", spriteAnimation.framesPerSecond);
				JSON_READ_NAME(animatorValue, "loop", spriteAnimation.loop);

				m_spriteAnimations[ToLower(spriteAnimation.name)] = spriteAnimation;
			}
		}

	}

	void SpriteAnimatorRendererComponent::Play(const string& name)
	{
		if (EqualsIgnoreCase(name, m_currentAnimation.name)) {
			return;
		}



		auto iter = m_spriteAnimations.find(ToLower(name));

		if (iter == m_spriteAnimations.end())
		{
			cerr << "Could not find animation: " << name << endl;
			return;
		}

		m_currentAnimation = iter->second;
		m_currentFrame = 0;
		m_frameTimer = 0.0f;

		m_texture = m_currentAnimation.textureFrames->GetTexture();

		m_sourceRect = m_currentAnimation.textureFrames->GetFrameRect(m_currentFrame);
	}

	bool SpriteAnimatorRendererComponent::IsAnimationFinished() const
	{
		if (!m_currentAnimation.textureFrames)
			return false;

		if (m_currentAnimation.loop)
			return false;

		return m_currentFrame >= m_currentAnimation.textureFrames->GetTotalFrames() - 1;
	}
}
