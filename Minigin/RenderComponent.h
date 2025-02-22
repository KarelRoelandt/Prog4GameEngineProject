#pragma once
#include "BaseComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "Renderer.h"

namespace dae
{
    class RenderComponent final : public BaseComponent
    {
    public:
        RenderComponent(GameObject* owner) : BaseComponent(owner) {}
        ~RenderComponent() override = default;

        RenderComponent(const RenderComponent&) = delete;
        RenderComponent(RenderComponent&&) noexcept = delete;
        RenderComponent& operator=(const RenderComponent&) = delete;
        RenderComponent& operator=(RenderComponent&&) noexcept = delete;

        void Update(float /*deltaTime*/) override {}
        void Render() const override
        {
            auto owner = GetOwner();
            glm::vec2 objectPosition{};
            if (owner->HasComponent<TransformComponent>())
            {
                auto transformComponent = owner->GetComponent<TransformComponent>();
                objectPosition = transformComponent->GetPosition();
            }

            if (owner->HasComponent<TextureComponent>())
            {
                auto textureComponent = owner->GetComponent<TextureComponent>();
                const auto& texture = textureComponent->GetTexture();
                if (texture)
                {
                    Renderer::GetInstance().RenderTexture(*texture, objectPosition.x, objectPosition.y, textureComponent->GetSize().x, textureComponent->GetSize().y);
                }
            }

            if (owner->HasComponent<TextComponent>())
            {
                auto textComponent = owner->GetComponent<TextComponent>();
                const auto& texture = textComponent->GetTexture();
                if (texture)
                {
                    Renderer::GetInstance().RenderTexture(*texture, objectPosition.x, objectPosition.y);
                }
            }
        }
    };
}
