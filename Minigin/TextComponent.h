#pragma once

#include "BaseComponent.h"
#include "Font.h"
#include "Texture2D.h"
#include <memory>
#include <SDL_pixels.h>
#include <string>
#include "CustomDefs.h"

namespace dae
{
    class TextComponent final : public BaseComponent
    {
    public:
        TextComponent(GameObject* owner, const std::string& text, std::shared_ptr<Font> font);
        ~TextComponent() override = default;

        TextComponent(const TextComponent&) = delete;
        TextComponent(TextComponent&&) = delete;
        TextComponent& operator=(const TextComponent&) = delete;
        TextComponent& operator=(TextComponent&&) = delete;

        void Update(float deltaTime) override;
        void Render() const override;

        void SetText(const std::string& text);
        void SetColor(const SDL_Color& color) { m_color = color; m_needsUpdate = true; }
        Vector2 GetSize() const;

        std::shared_ptr<Texture2D> GetTexture() const;

    private:
        std::shared_ptr<Font> m_font;
        std::shared_ptr<Texture2D> m_textTexture;

        std::string m_text;
        Vector2 m_size{ 0, 0 };
        SDL_Color m_color{ 255, 255, 255, 255 };


        //Vector2 m_position{ 0, 0 };
        

        

        bool m_needsUpdate;

    };
}
