#pragma once

#include "BaseComponent.h"
#include "CustomDefs.h"      
#include <memory>
#include <string>
#include <SDL_rect.h>    
#include <SDL_render.h>     

#include <vec2.hpp>      

// Forward declarations
namespace dae
{
    class Texture2D;
    class ResourceManager;
}

namespace dae
{
    class TextureComponent final : public BaseComponent
    {
    public:
        explicit TextureComponent(GameObject* owner);
        ~TextureComponent() override = default;

        TextureComponent(const TextureComponent&) = delete;
        TextureComponent(TextureComponent&&) = delete;
        TextureComponent& operator=(const TextureComponent&) = delete;
        TextureComponent& operator=(TextureComponent&&) = delete;

        void Update(float deltaTime) override;
        void Render() const override;

        void SetTexture(const std::string& filename);
        std::shared_ptr<Texture2D> GetTexture() const;

        Vector2 GetTexturePixelSize() const;
        void SetRenderSize(float width, float height);
        Vector2 GetRenderDestinationSize() const;

        void SetSourceRect(int x, int y, int w, int h);
        void ClearSourceRect();
        bool IsUsingSourceRect() const;
        const SDL_Rect& GetSourceRect() const;

        SDL_Texture* GetSDLTexture() const; // This was line 52 in the previous version

    private:
        std::shared_ptr<Texture2D> m_texture;
        Vector2 m_renderSize;
        bool m_isRenderSizeSet;

        SDL_Rect m_SourceRect;
        bool m_UseSourceRect;
    };
}