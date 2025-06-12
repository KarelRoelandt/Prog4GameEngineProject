#include "Renderer.h"
#include "SceneManager.h" // Your Renderer.cpp includes this
#include "Texture2D.h"
#include <stdexcept> // For std::runtime_error
#include <cstring>   // For strcmp

// Your GetOpenGLDriverIndex function
static int GetOpenGLDriverIndex()
{
    auto openglIndex = -1;
    const auto driverCount = SDL_GetNumRenderDrivers();
    for (auto i = 0; i < driverCount; i++)
    {
        SDL_RendererInfo info;
        if (!SDL_GetRenderDriverInfo(i, &info))
            if (!strcmp(info.name, "opengl")) // Potential warning: strcmp result not used safely
                openglIndex = i;
    }
    return openglIndex;
}

namespace dae
{
    void Renderer::Init(SDL_Window* window)
    {
        m_window = window;
        // Using your GetOpenGLDriverIndex()
        m_renderer = SDL_CreateRenderer(window, GetOpenGLDriverIndex(), SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (m_renderer == nullptr)
        {
            throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
        }
        // Your ImGui commented out code was here
    }

    void Renderer::Render() const
    {
        const auto& color = GetBackgroundColor();
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(m_renderer);

        SceneManager::GetInstance().Render(); // Your existing call

        // Your ImGui commented out code was here
        SDL_RenderPresent(m_renderer);
    }

    void Renderer::Destroy()
    {
        // Your ImGui commented out code was here
        if (m_renderer != nullptr)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
    }

    void Renderer::RenderTexture(const Texture2D& texture, const float x, const float y) const
    {
        SDL_Rect dst{};
        dst.x = static_cast<int>(x);
        dst.y = static_cast<int>(y);
        SDL_QueryTexture(texture.GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
        SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
    }

    void Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
    {
        SDL_Rect dst{};
        dst.x = static_cast<int>(x);
        dst.y = static_cast<int>(y);
        dst.w = static_cast<int>(width);
        dst.h = static_cast<int>(height);
        SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
    }

    // ADD THIS NEW IMPLEMENTATION
    void Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height, const SDL_Rect* srcRect) const
    {
        SDL_Rect dstRect{};
        dstRect.x = static_cast<int>(x);
        dstRect.y = static_cast<int>(y);
        dstRect.w = static_cast<int>(width);
        dstRect.h = static_cast<int>(height);

        // Use SDL_RenderCopyEx if you need rotation or flipping, otherwise SDL_RenderCopy is fine.
        // For basic source rectangle functionality, SDL_RenderCopy is sufficient.
        SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), srcRect, &dstRect);
    }

    SDL_Renderer* Renderer::GetSDLRenderer() const
    {
        return m_renderer;
    }
} // namespace dae