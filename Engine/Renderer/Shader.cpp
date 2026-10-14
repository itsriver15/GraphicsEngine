#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Core/File.h"

namespace nu
{
    Shader::~Shader()
    {
        if (m_gpuDevice && m_gpuShader)
        {
            SDL_ReleaseGPUShader(m_gpuDevice, m_gpuShader);
        }
    }

    bool Shader::Load(const std::string& filename, Renderer& renderer)
    {
        m_gpuDevice = renderer.GetGPUDevice();

        if (!m_gpuDevice)
        {
            std::cerr << "Error: GPU device is null." << std::endl;
            return false;
        }

        // Determine the shader stage from the filename.
        SDL_GPUShaderStage stage{};

        if (filename.find(".vert") != std::string::npos)
        {
            stage = SDL_GPU_SHADERSTAGE_VERTEX;
        }
        else if (filename.find(".frag") != std::string::npos)
        {
            stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
        }
        else
        {
            std::cerr << "Error: Shader filename must contain either "
                "'.vert' or '.frag'." << std::endl;
            return false;
        }

        // Determine the shader format supported by the GPU.
        SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
        std::string shaderFilename = filename;

        const char* entrypoint = "main";

        SDL_GPUShaderFormat backendFormats =
            SDL_GetGPUShaderFormats(m_gpuDevice);

        if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV)
        {
            shaderFilename += ".spv";
            format = SDL_GPU_SHADERFORMAT_SPIRV;
        }
        else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL)
        {
            shaderFilename += ".dxil";
            format = SDL_GPU_SHADERFORMAT_DXIL;
        }
        else
        {
            std::cerr << "Error: No supported shader formats found!"
                << std::endl;
            return false;
        }

        // Display the resolved shader path for debugging.
        namespace fs = std::filesystem;

        const auto path = fs::absolute(shaderFilename);

        std::cout << "Shader path: " << path.string() << std::endl;

        // Read the compiled shader file.
        std::vector<uint8_t> bytes = ReadBinaryFile(shaderFilename);

        if (bytes.empty())
        {
            std::cerr << "Error: Failed to read shader file: "
                << path.string() << std::endl;
            return false;
        }

        // Set up the shader creation information.
        SDL_GPUShaderCreateInfo shaderInfo{
            .code_size = bytes.size(),
            .code = bytes.data(),
            .entrypoint = entrypoint,
            .format = format,
            .stage = stage,
            .num_uniform_buffers = 1
        };

        // Create the GPU shader.
        m_gpuShader = SDL_CreateGPUShader(m_gpuDevice, &shaderInfo);

        if (!m_gpuShader)
        {
            std::cerr << "Error: Failed to create shader: "
                << SDL_GetError() << std::endl;
            return false;
        }

        std::cout << "Shader loaded successfully: "
            << path.string() << std::endl;

        return true;
    }
}
