#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_Image/SDL_image.h>
#include "Math/Transform.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "Math/Vector2.h"

using namespace nu;

namespace nu {
	class Renderer
	{
		friend class Text;
		friend class Texture;

	public:
		bool Initialize(int, int);
		void Shutdown();

		void Clear() const;
		void Present() const;

		void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) const;
		void SetColor(float r, float g, float b, float = 1.0f) const;


		void DrawPoint(float x, float y) const;
		void DrawLine(float x1, float y1, float x2, float y2) const;
		void DrawFillRect(float x, float y, float w, float h) const;
		void DrawRect(float x, float y, float w, float h) const;


		void DrawModel(const class Model& model, const struct Transform& transform) const;

		void DrawTexture(const class Texture& texture, float x, float y, float angle = 0.0f, float scale = 1.0f, bool fliph = false, const Vector2& origin = Vector2{0.5f, 0.5f})const;
		void DrawTexture(const class Texture& texture, const struct Rect& source, float x, float y, float angle = 0.0f, float scale = 1.0f, bool fliph = false, const Vector2& origin = Vector2{ 0.5f, 0.5f }) const;


		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		void SetCamera(const Vector2& camera) { m_camera = camera; }

		void EnableCamera(bool enable = true) { m_cameraEnabled = enable; }

	private:
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;


		Vector2 m_camera;
		bool m_cameraEnabled = true;

		int m_width = 0;
		int m_height = 0;
	};
}