#include "TextureComponent.h"
#include "ResourceManager.h" // For ResourceManager::GetInstance()
#include "Texture2D.h"       // For Texture2D class methods
#include "GameObject.h"      // For GetOwner()->GetName() potentially, though not used in this version

#include <iostream>          // For std::cerr, std::cout (optional logging)
// glm::vec2.hpp is included in TextureComponent.h for glm::ivec2

namespace dae
{
    TextureComponent::TextureComponent(GameObject* owner)
        : BaseComponent(owner),
        m_texture(nullptr),
        m_renderSize(0.f, 0.f),
        m_isRenderSizeSet(false),
        m_SourceRect({ 0, 0, 0, 0 }),
        m_UseSourceRect(false)
    {
    }

    void TextureComponent::Update(float /*deltaTime*/)
    {
        // No update logic needed
    }

    void TextureComponent::Render() const
    {
        // Does not render directly
    }

    void TextureComponent::SetTexture(const std::string& filename)
    {
        // Make sure to include ResourceManager.h for this line
        m_texture = ResourceManager::GetInstance().LoadTexture(filename);
        m_UseSourceRect = false;
        m_isRenderSizeSet = false;

        if (m_texture)
        {
            m_renderSize = GetTexturePixelSize();
            // std::cout << "[TextureComponent::SetTexture] Texture '" << filename << "' loaded. Default render size: "
            //           << m_renderSize.x << "x" << m_renderSize.y << std::endl;
        }
        else
        {
            m_renderSize = { 0.f, 0.f };
            if (!filename.empty())
            {
                std::cerr << "[TextureComponent::SetTexture] Warning: Failed to load texture: " << filename << std::endl;
            }
        }
    }

    std::shared_ptr<Texture2D> TextureComponent::GetTexture() const
    {
        return m_texture;
    }

    Vector2 TextureComponent::GetTexturePixelSize() const
    {
        if (m_texture)
        {
            glm::ivec2 nativeSize = m_texture->GetSize(); // Uses GetSize() from your Texture2D
            return { static_cast<float>(nativeSize.x), static_cast<float>(nativeSize.y) };
        }
        return { 0.f, 0.f };
    }

    void TextureComponent::SetRenderSize(float width, float height)
    {
        m_renderSize = { width, height };
        m_isRenderSizeSet = true;
    }

    Vector2 TextureComponent::GetRenderDestinationSize() const
    {
        if (m_isRenderSizeSet)
        {
            return m_renderSize;
        }
        if (m_UseSourceRect)
        {
            return { static_cast<float>(m_SourceRect.w), static_cast<float>(m_SourceRect.h) };
        }
        if (m_texture)
        {
            return GetTexturePixelSize();
        }
        return { 0.f, 0.f };
    }

    void TextureComponent::SetSourceRect(int x, int y, int w, int h)
    {
        m_SourceRect = { x, y, w, h };
        m_UseSourceRect = true;
    }

    void TextureComponent::ClearSourceRect()
    {
        m_UseSourceRect = false;
    }

    bool TextureComponent::IsUsingSourceRect() const
    {
        return m_UseSourceRect;
    }

    const SDL_Rect& TextureComponent::GetSourceRect() const
    {
        return m_SourceRect;
    }

    SDL_Texture* TextureComponent::GetSDLTexture() const
    {
        if (m_texture)
        {
            return m_texture->GetSDLTexture(); // Uses GetSDLTexture() from your Texture2D
        }
        return nullptr;
    }
}