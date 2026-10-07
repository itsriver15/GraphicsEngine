#include "pch.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Texture.h"
#include "../Math/Vector2.h"
#include "../Renderer/Model.h"
#include "../Math/MathUtils.h"
#include "../Math/Rect.h"

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

void Renderer::DrawModel (const class Model& model, const struct Transform& transform) const
{
	
	for (auto mesh : model.GetMeshes()) {
		auto& points = mesh.GetPoints();
		SetColor(mesh.GetColor().r, mesh.GetColor().g, mesh.GetColor().b, 1.0f);


		for (int i = 0; i + 1 < mesh.GetPoints().size(); i++) {
			Vector2 v1 = points.at(i);
			Vector2 v2 = points.at(i + 1);

			v1 *= transform.scale;
			v2 *= transform.scale;

			v1 = v1.Rotate(transform.rotation * DegToRad);
			v2 = v2.Rotate(transform.rotation * DegToRad);

			v1 += transform.position;
			v2 += transform.position;
			DrawLine(v1.x, v1.y, v2.x, v2.y);
		}
	}

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
		std::cerr << "Could not acquire command buffer: " << SDL_GetError() << std::endl;
		return false;
	}


	SDL_GPUTexture* swapchainTexture = nullptr;
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(m_cmdBuffer, window, &swapchainTexture, nullptr, nullptr))
	{
		std::cerr << "Could not acquire swapchain texture: " << SDL_GetError() << std::endl;
		return false;
	}

	if (swapchainTexture != nullptr)
	{
		// configure the color target attachments (This handles clearing the screen)
		SDL_GPUColorTargetInfo color_target_info{};
		color_target_info.texture = swapchainTexture;
		color_target_info.clear_color = SDL_FColor{ 1.0f, 0.0f, 0.0f, 1.0f };
		color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
		color_target_info.store_op = SDL_GPU_STOREOP_STORE;

		m_renderPass = SDL_BeginGPURenderPass(m_cmdBuffer, &color_target_info, 1, nullptr);
		SDL_EndGPURenderPass(m_renderPass);
	}

	return true;
}

bool Renderer::EndFrame() const
{
	if (!SDL_SubmitGPUCommandBuffer(m_cmdBuffer))
	{
		std::cerr << "Could not submit command buffer: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}




