#pragma once

#include "BaseComponent.h"
#include <memory>
#include <string>

#include "CustomDefs.h"

namespace dae
{
    class Texture2D;

    class TextureComponent final : public BaseComponent
	{
    public:
        TextureComponent() = default;
        ~TextureComponent() override = default;

        TextureComponent(const TextureComponent&) = delete;
        TextureComponent(TextureComponent&&) = delete;
        TextureComponent& operator=(const TextureComponent&) = delete;
        TextureComponent& operator=(TextureComponent&&) = delete;

        void Update(float deltaTime) override;
        void Render() const override;

        void SetTexture(const std::string& filename);
        void SetPosition(float x, float y);
        void SetSize(float width, float height);
        Vector2 GetSize() const;

    private:
        std::shared_ptr<Texture2D> m_texture;
        dae::Vector2 m_position{ 0, 0 };
        Vector2 m_size{ 0, 0 };
    };
}