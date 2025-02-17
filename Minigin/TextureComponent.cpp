#include "TextureComponent.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"

namespace dae
{
    void TextureComponent::Update(float /*deltaTime*/)
    {
        // No update logic for texture component
    }

    void TextureComponent::Render() const
    {
    }

    void TextureComponent::SetTexture(const std::string& filename)
    {
        m_texture = ResourceManager::GetInstance().LoadTexture(filename);
        if (m_texture) 
        {
            auto size = m_texture->GetSize();
            m_size = { static_cast<float>(size.x), static_cast<float>(size.y) };
        }
    }

    void TextureComponent::SetSize(float width, float height)
    {
        m_size = { width, height };
    }

    Vector2 TextureComponent::GetSize() const
    {
        return m_size;
    }

    std::shared_ptr<Texture2D> TextureComponent::GetTexture() const 
    {
        return m_texture;
    }
}
