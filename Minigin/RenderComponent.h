#pragma once
#include "BaseComponent.h"
#include "TextureComponent.h"   // Needs the updated TextureComponent definition
#include "TransformComponent.h"
#include "Renderer.h"           // For dae::Renderer
#include "TextComponent.h"      // If you have a TextComponent that RenderComponent might render
#include "GameObject.h"
#include <iostream>             // For std::cout logging (optional)

// Ensure your Vector2 header is included (defined in CustomDefs.h for your project)
#include "CustomDefs.h" 

// GLM includes (used for objectPosition, and potentially by TransformComponent)
#include <glm.hpp>
#include <vec2.hpp> // For glm::vec2

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