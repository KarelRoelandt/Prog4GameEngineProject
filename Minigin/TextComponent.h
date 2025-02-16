#pragma once

#include "BaseComponent.h"
#include "Font.h"
#include "Texture2D.h"
#include <memory>
#include <string>

#include "CustomDefs.h"

namespace dae
{
    class TextComponent final : public BaseComponent
	{
    public:
        TextComponent(const std::string& text, std::shared_ptr<Font> font);
        ~TextComponent() override = default;

        TextComponent(const TextComponent&) = delete;
        TextComponent(TextComponent&&) = delete;
        TextComponent& operator=(const TextComponent&) = delete;
        TextComponent& operator=(TextComponent&&) = delete;

        void Update(float deltaTime) override;
        void Render() const override;

        void SetText(const std::string& text);
        void SetPosition(float x, float y);
        Vector2 GetSize() const;

    private:
        bool m_needsUpdate;
        std::string m_text;
        Vector2 m_position{ 0, 0 };
        Vector2 m_size{ 0, 0 };
        std::shared_ptr<Font> m_font;
        std::shared_ptr<Texture2D> m_textTexture;
    };
}
