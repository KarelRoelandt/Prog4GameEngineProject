#include <SDL.h>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include <iostream>
#include <SDL_image.h>
#include <stdexcept>

#include "Texture2D.h"
#include "Renderer.h"


dae::Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_texture);
}

glm::ivec2 dae::Texture2D::GetSize() const
{
	SDL_Rect dst;
	SDL_QueryTexture(GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	return { dst.w,dst.h };
}

SDL_Texture* dae::Texture2D::GetSDLTexture() const
{
	return m_texture;
}

dae::Texture2D::Texture2D(const std::string &fullPath)
{
	std::cout << "[Texture2D] Constructor: Attempting to IMG_LoadTexture with full path: '" << fullPath << "'" << std::endl;
	m_texture = IMG_LoadTexture(Renderer::GetInstance().GetSDLRenderer(), fullPath.c_str());
	if (m_texture == nullptr)
	{
		throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError() + " (Path attempted: '" + fullPath + "')");
	}
	m_texture = IMG_LoadTexture(Renderer::GetInstance().GetSDLRenderer(), fullPath.c_str());
	if (m_texture == nullptr)
		throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
}

dae::Texture2D::Texture2D(SDL_Texture* texture)	: m_texture{ texture } 
{
	assert(m_texture != nullptr);
}

