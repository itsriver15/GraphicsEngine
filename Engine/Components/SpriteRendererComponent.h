#pragma once
#include "RendererComponent.h"
#include "../Resources/ResourceManager.h"
#include "../Renderer/Texture.h"
#include "Math/Rect.h"

namespace nu {
	
	class SpriteRendererComponent : public RendererComponent {

	public:
		CLASS_PROTOTYPE(SpriteRendererComponent)

		void Draw(const Renderer& renderer) override;

		virtual void Read(const json::value_t& value) override;

		void Start() override;
		
		bool GetFlipH() { return m_fliph; }
		void SetFlipH(bool flip = false) { m_fliph = flip; }





	protected:
		res_t<Texture> m_texture;
		string m_textureName;
		Rect m_sourceRect;
		Vector2 m_origin{ 0.5f };
		Vector2 m_size{ 0.0f };
		bool m_fliph = false;



	};
}