#pragma once
#include <map>
#include <string>
#include "SpriteAnimationRendererComponent.h"
#include "Resources/ResourceManager.h"

namespace nu {
	class SpriteAnimatorRendererComponent : public SpriteRendererComponent {
	public:
		struct SpriteAnimation {
			string name;
			float framesPerSecond = 10;
			bool loop = true;
			res_t<class TextureFrames> textureFrames;
		};
	public:
		CLASS_PROTOTYPE(SpriteAnimatorRendererComponent)

		void Update(float dt) override;
		void Start() override;
		void Read(const json::value_t& value) override;

		void Play(const string& name);
		bool IsPaused() { return m_paused; }
		void SetPaused(bool paused = false) { m_paused = paused; }
		void Pause() { m_paused = true; }

		unsigned int GetFrame() const { return m_currentFrame; }
		const string& GetAnimationName()const { return m_currentAnimation.name; }

		bool IsAnimationFinished() const;




	private:
		unsigned int m_currentFrame = 0;
		float m_frameTimer = 0.0f;
		bool m_paused = false;
		string m_defaultAnimationName;
		SpriteAnimation m_currentAnimation;
		map<string, SpriteAnimation> m_spriteAnimations;


	};
}
