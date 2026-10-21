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

	CreateDepthBuffer();

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

	if (swapchainTexture != nullptr)
	{
	

	SDL_GPUColorTargetInfo colorTargetInfo{};
	colorTargetInfo.texture = swapchainTexture;
	colorTargetInfo.clear_color = SDL_FColor{ 0.5f, 0.5f, 0.5f, 1.0f };
	colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

	// configure the depth target (This handles the depth sorting)
	SDL_GPUDepthStencilTargetInfo depthTargetInfo = {};
	depthTargetInfo.texture = m_depthTexture;
	depthTargetInfo.clear_depth = 1.0f;
	depthTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	depthTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
	depthTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
	depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

	m_renderPass = SDL_BeginGPURenderPass(
		m_cmdBuffer, &colorTargetInfo, 1, &depthTargetInfo);

	if (!m_renderPass)
	{
		std::cerr << "Could not begin render pass: "
			<< SDL_GetError() << std::endl;
		SDL_CancelGPUCommandBuffer(m_cmdBuffer);
		m_cmdBuffer = nullptr;
		return false;
	}
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

void nu::Renderer::CreateDepthBuffer()
{
	SDL_GPUTextureCreateInfo depthInfo{
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
		.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
		.width = (uint32_t)m_width,
		.height = (uint32_t)m_height,
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.sample_count = SDL_GPU_SAMPLECOUNT_1,
	};


	m_depthTexture = SDL_CreateGPUTexture(m_gpuDevice, &depthInfo);
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

void Renderer::SetFragmentTexture(uint32_t slot, const Texture& texture)
{
	SDL_GPUTextureSamplerBinding binding
	{
		.texture = texture.m_gpuTexture,
		.sampler = texture.m_gpuSampler
	};

	SDL_BindGPUFragmentSamplers(m_renderPass, slot, &binding, 1);
}


