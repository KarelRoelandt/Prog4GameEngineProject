#pragma once

#include <SDL_rect.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "BaseComponent.h"
#include "TextureComponent.h"
#include "GameObject.h"

namespace dae
{
    struct Animation
    {
        std::string name;
        std::vector<SDL_Rect> frames;
        float frameTime = 0.1f; // Time per frame in seconds
        bool loop = true;
        std::shared_ptr<Texture2D> texture;
    };

    class AnimatorComponent final : public BaseComponent
    {
    public:
        explicit AnimatorComponent(GameObject* owner);
        ~AnimatorComponent() override = default;

        void Update(float deltaTime) override;
        void Render() const override {}

        void AddAnimation(const Animation& animation);
        void Play(const std::string& name, bool resetIfPlaying = false);
        void Stop();
        bool IsPlaying() const { return m_IsPlaying; }
        const std::string& GetCurrentAnimation() const { return m_CurrentAnimation; }

        void SetFlip(bool flip);

        void AddAnimationFromGrid(
	        const std::string& name,
	        std::shared_ptr<Texture2D> texture,
	        int frameWidth,
	        int frameHeight,
	        int frameCount,
	        float frameTime,
	        bool loop,
	        int startX = 0, int startY = 0
        );

    private:
        void ApplyFrameToTexture();

        std::unordered_map<std::string, Animation> m_Animations;
        std::string m_CurrentAnimation;
        size_t m_CurrentFrame = 0;
        float m_Elapsed = 0.0f;
        bool m_IsPlaying = false;

        bool m_Flip = false;

        TextureComponent* m_pTextureComponent = nullptr;
    };
}
