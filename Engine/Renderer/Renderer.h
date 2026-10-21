#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_Image/SDL_image.h>
#include "Math/Transform.h"
#include "Math/Vector2.h"

using namespace nu;

namespace nu {
	class Renderer
	{
		friend class IndexBuffer;
		friend class Text;
		friend class Texture;
		friend class Shader;
		friend class VertexBuffer;
		

	public:
		bool Initialize(int, int);
		void Shutdown();

		void Clear() const;
		void Present() const;

		bool BeginFrame();
		bool EndFrame() const;

		void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) const;
		void SetColor(float r, float g, float b, float = 1.0f) const;


		void DrawPoint(float x, float y) const;
		void DrawLine(float x1, float y1, float x2, float y2) const;
		void DrawFillRect(float x, float y, float w, float h) const;
		void DrawRect(float x, float y, float w, float h) const;

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		void SetCamera(const Vector2& camera) { m_camera = camera; }

		void EnableCamera(bool enable = true) { m_cameraEnabled = enable; }
		//SDL GPU

		SDL_GPUDevice* GetGPUDevice() const { return m_gpuDevice; }
		SDL_Window* GetWindow() const { return window; }

		void CreateDepthBuffer();

		void SetPipeline(const class Pipeline& pipeline);
		void SetVertexBuffer(const class VertexBuffer& vertexBuffer);
		void SetIndexBuffer(const class IndexBuffer& indexBuffer);
		void SetVertexUniform(uint32_t slot, const void* data, uint32_t size);
		void Draw(uint32_t vertexCount);
		void SetFragmentTexture(uint32_t slot, const Texture& texture);
		void DrawIndex(const class IndexBuffer& indexBuffer);



	private:
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;


		Vector2 m_camera;
		bool m_cameraEnabled = true;

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUCommandBuffer* m_cmdBuffer = nullptr;
		SDL_GPURenderPass* m_renderPass = nullptr;
		SDL_GPUTexture* m_depthTexture = nullptr;

		int m_width = 0;
		int m_height = 0;
	};
}