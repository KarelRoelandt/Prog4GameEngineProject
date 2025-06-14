#pragma once

#include <glm.hpp>
#include <vec2.hpp>
#include <iostream>

#include "GameObject.h"

#include "BaseComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "TextComponent.h"

#include "Renderer.h"

#include "CustomDefs.h" 

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
            if (!owner)
            {
                return;
            }

            glm::vec2 objectPosition{};
            auto transformComponent = owner->GetComponent<TransformComponent>();
            if (transformComponent)
            {
                objectPosition = transformComponent->GetPosition(); // Assuming this returns glm::vec2
            }

            // --- Texture Rendering Part ---
            if (owner->HasComponent<TextureComponent>())
            {
                auto textureComponent = owner->GetComponent<TextureComponent>();
                if (textureComponent)
                {
                    auto textureSharedPtr = textureComponent->GetTexture();
                    if (textureSharedPtr)
                    {
                        // Use GetRenderDestinationSize() here
                        dae::Vector2 renderSizeDae = textureComponent->GetRenderDestinationSize();
                        float destWidth = renderSizeDae.x;
                        float destHeight = renderSizeDae.y;

                        if (textureComponent->IsUsingSourceRect())
                        {
                            const SDL_Rect& srcRect = textureComponent->GetSourceRect();
                            Renderer::GetInstance().RenderTexture(*textureSharedPtr, objectPosition.x, objectPosition.y, destWidth, destHeight, &srcRect, textureComponent->GetFlipX());
                        }
                        else
                        {
                            Renderer::GetInstance().RenderTexture(*textureSharedPtr, objectPosition.x, objectPosition.y, destWidth, destHeight, nullptr, textureComponent->GetFlipX());
                        }
                    }
                }
            }

            // --- Text Rendering Part (if applicable) ---
            if (owner->HasComponent<TextComponent>())
            {
                auto textComponent = owner->GetComponent<TextComponent>();
                if (textComponent)
                {
                    auto textureSharedPtr = textComponent->GetTexture();
                    if (textureSharedPtr)
                    {
                        // Assuming TextComponent's texture is rendered at its native size or TextComponent handles its own sizing.
                        // If TextComponent needs scaling like TextureComponent, this part might need adjustment.
                        // The current Renderer::RenderTexture overload used here implies native size rendering at the given position.
                        Renderer::GetInstance().RenderTexture(*textureSharedPtr, objectPosition.x, objectPosition.y);
                    }
                }
            }
        }
    };
}