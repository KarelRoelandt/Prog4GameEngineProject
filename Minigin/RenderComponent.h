#pragma once
#include "BaseComponent.h"
//#include "ImGuiComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "Renderer.h"
#include "TextComponent.h"

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
            if (!owner) return; // Safety check for null owner

            glm::vec2 objectPosition{};
            if (owner->HasComponent<TransformComponent>())
            {
                auto transformComponent = owner->GetComponent<TransformComponent>();
                if (transformComponent) // Ensure component exists
                    objectPosition = transformComponent->GetPosition();
            }

            if (owner->HasComponent<TextureComponent>())
            {
                auto textureComponent = owner->GetComponent<TextureComponent>();
                if (textureComponent) // Ensure component exists
                {
                    auto texture = textureComponent->GetTexture(); // Value not reference
                    if (texture)
                    {
                        Renderer::GetInstance().RenderTexture(*texture, objectPosition.x, objectPosition.y, textureComponent->GetSize().x, textureComponent->GetSize().y);
                    }
                }
            }

            if (owner->HasComponent<TextComponent>())
            {
                auto textComponent = owner->GetComponent<TextComponent>();
                if (textComponent) // Ensure component exists
                {
                    auto texture = textComponent->GetTexture(); // Value not reference
                    if (texture)
                    {
                        Renderer::GetInstance().RenderTexture(*texture, objectPosition.x, objectPosition.y);
                    }
                }
            }

            //if (owner->HasComponent<ImGuiComponent>())
            //{
            //    owner->Render();
            //}
        }
    };
}
