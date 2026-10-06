#include "pch.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Renderer.h"

#include <SDL3_image/SDL_image.h>
#include <iostream>


using namespace nu;

    Texture::~Texture()
    {
        // if texture exists, destroy texture
        if (m_texture != nullptr) SDL_DestroyTexture(m_texture);
    }

    bool Texture::Load(const std::string& filename, Renderer& renderer)
    {
        // load image onto surface
        SDL_Surface* surface = IMG_Load(filename.c_str());
        if (surface == nullptr)
        {
            std::cerr << "Could not load image: " << filename << std::endl;
            return false;
        }

        // create texture from surface, texture is a friend class of renderer
        m_texture = SDL_CreateTextureFromSurface(renderer.renderer, surface);
        // once texture is created, surface can be freed up
        SDL_DestroySurface(surface);
        if (m_texture == nullptr)
        {
            std::cerr << "Could not create texture: " << filename << std::endl;
            std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
            return false;
        }
		//cache size of texture
        SDL_GetTextureSize(m_texture, &m_size.x, &m_size.y);

        return true;
    }

