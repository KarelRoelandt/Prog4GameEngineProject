#include "TextComponent.h"
#include "Renderer.h"
#include <SDL_ttf.h>
#include <stdexcept>

namespace dae
{
    TextComponent::TextComponent(GameObject* owner, const std::string& text, std::shared_ptr<Font> font)
        : BaseComponent(owner), m_text(text), m_font(std::move(font)), m_needsUpdate(true)
    {
    }

    void TextComponent::Update(float deltaTime)
    {
        (void)deltaTime; // Mark deltaTime as intentionally unused

        if (m_needsUpdate)
        {
            const SDL_Color color = { 255, 255, 255 }; // White color
            const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), color);
            if (surf == nullptr)
            {
                throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
            }
            auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
            if (texture == nullptr)
            {
                throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
            }
            SDL_FreeSurface(surf);
            m_textTexture = std::make_shared<Texture2D>(texture);
            m_size = { static_cast<float>(surf->w), static_cast<float>(surf->h) };
            m_needsUpdate = false;
        }
    }

    void TextComponent::Render() const
    {
    }

    void TextComponent::SetText(const std::string& text)
    {
        m_text = text;
        m_needsUpdate = true;
    }

    Vector2 TextComponent::GetSize() const
    {
        return m_size;
    }

    std::shared_ptr<Texture2D> TextComponent::GetTexture() const
    {
        return m_textTexture;
    }
}
