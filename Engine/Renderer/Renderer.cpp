#include "pch.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Pipeline.h" 
#include "../Renderer/VertexBuffer.h" 
#include "../Renderer/Shader.h" 
#include "../Math/Vector2.h"
#include "../Renderer/Model.h"
#include "../Math/MathUtils.h"
#include "../Math/Rect.h"
#include "../Renderer/IndexBuffer.h"

using namespace nu;

bool Renderer::Initialize(int width, int height)
{
	SDL_Init(SDL_INIT_VIDEO);
	m_width = width;
	m_height = height;
	window = SDL_CreateWindow("SDL3 Project", width, height, 0);

	if (window == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}


	if (!TTF_Init()) {
		std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	SDL_GPUShaderFormat formats = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL;
	m_gpuDevice = SDL_CreateGPUDevice(formats, true, nullptr);
	if (!m_gpuDevice)
	{
		std::cerr << "Failed to create GPU Device: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	// claim the window for our modern GPU context
	SDL_ClaimWindowForGPUDevice(m_gpuDevice, window);

	// print out the driver being utilized (e.g., "vulkan" or "d3d12")
	std::cout << "GPU Driver Initialized: " << SDL_GetGPUDeviceDriver(m_gpuDevice) << std::endl;


	return true;


}

void Renderer::Shutdown() {
	TTF_Quit();
	if (renderer != nullptr) {
		SDL_DestroyRenderer(renderer);
	}
	if (window != nullptr) {
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
}


void Renderer::Clear() const {
	SDL_RenderClear(renderer);
}

void Renderer::Present() const{
	SDL_RenderPresent(renderer);
}

void Renderer::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Renderer::SetColor(float r, float g, float b, float a) const {
	SDL_SetRenderDrawColorFloat(renderer, r, g, b, a);
}


void Renderer::DrawPoint(float x, float y) const {
	SDL_RenderPoint(renderer, x, y);
}

void Renderer::DrawLine(float x1, float y1, float x2, float y2) const {
	SDL_RenderLine(renderer, x1, y1, x2, y2);
}


void Renderer::DrawFillRect(float x, float y, float w, float h) const{
	SDL_FRect rect{ x, y, w, h };
	SDL_RenderFillRect(renderer, &rect);
}


void Renderer::DrawRect(float x, float y, float w, float h) const {
	SDL_FRect rect{ x, y, w, h };
	SDL_RenderRect(renderer, &rect);
}


void Renderer::DrawTexture(const class Texture& texture, float x, float y, float angle, float scale, bool fliph, const Vector2& origin) const
{
	Vector2 size = texture.GetSize();

	float cameraX = (m_cameraEnabled) ? (m_camera.x - m_width * origin.x) : 0.0f;
	float cameraY = (m_cameraEnabled) ? (m_camera.y - m_height * origin.y): 0.0f;
	
	SDL_FRect destRect;
	destRect.w = size.x * scale;
	destRect.h = size.y * scale;

	destRect.x = (x - cameraX) - (destRect.w * origin.x);
	destRect.y = (y - cameraY) - (destRect.h * origin.y);


	// https://wiki.libsdl.org/SDL3/SDL_RenderTexture
	SDL_RenderTextureRotated(renderer, texture.m_texture, NULL, &destRect, angle, NULL, (fliph) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void nu::Renderer::DrawTexture(const Texture& texture, const Rect& source, float x, float y, float angle, float scale, bool fliph, const Vector2& origin) const
{

	float cameraX = (m_cameraEnabled) ? (m_camera.x - m_width * origin.x) : 0.0f;
	float cameraY = (m_cameraEnabled) ? (m_camera.y - m_height * origin.y) : 0.0f;

	SDL_FRect sourceRect;
	sourceRect.x = source.x;
	sourceRect.y = source.y;
	sourceRect.w = source.w;
	sourceRect.h = source.h;

	SDL_FRect destRect;
	destRect.w = source.w * scale;
	destRect.h = source.h * scale;

	destRect.x = (x - cameraX) - (destRect.w * origin.x);
	destRect.y = (y - cameraY) - (destRect.h * origin.y);


	SDL_SetTextureScaleMode(texture.m_texture, SDL_SCALEMODE_PIXELART);


	// https://wiki.libsdl.org/SDL3/SDL_RenderTexture
	SDL_RenderTextureRotated(renderer, texture.m_texture, &sourceRect, &destRect, angle, NULL, (fliph) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);



}

bool Renderer::BeginFrame()
{
	m_cmdBuffer = SDL_AcquireGPUCommandBuffer(m_gpuDevice);
	if (!m_cmdBuffer)
	{
		std::cerr << "Could not acquire command buffer: "
			<< SDL_GetError() << std::endl;
		return false;
	}

	SDL_GPUTexture* swapchainTexture = nullptr;
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(
		m_cmdBuffer, window, &swapchainTexture, nullptr, nullptr))
	{
		std::cerr << "Could not acquire swapchain texture: "
			<< SDL_GetError() << std::endl;
		SDL_CancelGPUCommandBuffer(m_cmdBuffer);
		m_cmdBuffer = nullptr;
		return false;
	}

	if (swapchainTexture == nullptr)
	{
		m_renderPass = nullptr;
		return true;
	}

	SDL_GPUColorTargetInfo colorTargetInfo{};
	colorTargetInfo.texture = swapchainTexture;
	colorTargetInfo.clear_color = SDL_FColor{ 0.5f, 0.5f, 0.5f, 1.0f };
	colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

	m_renderPass = SDL_BeginGPURenderPass(
		m_cmdBuffer, &colorTargetInfo, 1, nullptr);

	if (!m_renderPass)
	{
		std::cerr << "Could not begin render pass: "
			<< SDL_GetError() << std::endl;
		SDL_CancelGPUCommandBuffer(m_cmdBuffer);
		m_cmdBuffer = nullptr;
		return false;
	}

	return true;
}

bool Renderer::EndFrame() const
{
	if (m_renderPass != nullptr)
	{
		SDL_EndGPURenderPass(m_renderPass);
	}

	if (!SDL_SubmitGPUCommandBuffer(m_cmdBuffer))
	{
		std::cerr << "Could not submit command buffer: "
			<< SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

void Renderer::SetPipeline(const Pipeline& pipeline)
{
	// bind the pipeline to the current render pass
	// the pipeline sets the shaders and render settings used by the draw calls that follow
	SDL_BindGPUGraphicsPipeline(m_renderPass, pipeline.GetGPUPipeline());
}

void Renderer::SetVertexBuffer(const VertexBuffer& vertexBuffer)
{
	// describe which buffer to bind and where to start reading from
	SDL_GPUBufferBinding binding{.buffer = vertexBuffer.m_gpuBuffer, .offset = 0};
	SDL_BindGPUVertexBuffers(m_renderPass, 0, &binding, 1);
}

void nu::Renderer::SetIndexBuffer(const IndexBuffer& indexBuffer)
{
	SDL_GPUBufferBinding binding{ .buffer = indexBuffer.m_gpuBuffer, .offset = 0 };
	SDL_BindGPUIndexBuffer(m_renderPass, &binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
}

void nu::Renderer::SetVertexUniform(uint32_t slot, const void* data, uint32_t size)
{
	SDL_PushGPUVertexUniformData(m_cmdBuffer, slot, data, size);
}

void Renderer::Draw(uint32_t vertexCount)
{
	// draw using the currently bound pipeline and vertex buffer with SDL_DrawGPUPrimitives()
	// parameters:
	//   render pass    - the current render pass (m_renderPass)
	//   vertex count   - the number of vertices to draw (vertexCount)
	//   instance count - 1, draw one copy (more than 1 is used for instancing)
	//   first vertex   - 0, start at the first vertex in the buffer
	//   first instance - 0, start at the first instance

	SDL_DrawGPUPrimitives(m_renderPass, vertexCount, 1, 0, 0);
}

void nu::Renderer::DrawIndex(const IndexBuffer& indexBuffer)
{

	SDL_DrawGPUIndexedPrimitives(m_renderPass, indexBuffer.GetIndexCount(), 1, 0, 0, 0);
}




