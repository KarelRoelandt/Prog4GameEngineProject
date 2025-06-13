#include "AnimatorComponent.h"
#include "TextureComponent.h"
#include "GameObject.h"

namespace dae
{
    AnimatorComponent::AnimatorComponent(GameObject* owner)
        : BaseComponent(owner)
    {
        if (owner)
        {
            auto tex = owner->GetComponent<TextureComponent>();
            if (tex) m_pTextureComponent = tex.get();
        }
    }

    void AnimatorComponent::AddAnimation(const Animation& animation)
    {
        m_Animations[animation.name] = animation;
    }

    void AnimatorComponent::Play(const std::string& name, bool resetIfPlaying)
    {
        if (m_CurrentAnimation == name && !resetIfPlaying && m_IsPlaying)
            return;

        auto it = m_Animations.find(name);
        if (it != m_Animations.end())
        {
            m_CurrentAnimation = name;
            m_CurrentFrame = 0;
            m_Elapsed = 0.0f;
            m_IsPlaying = true;

            // Set the texture in TextureComponent to the animation's texture
            if (m_pTextureComponent && it->second.texture)
                m_pTextureComponent->SetTexture(it->second.texture);

            ApplyFrameToTexture();
        }
    }

    void AnimatorComponent::Stop()
    {
        m_IsPlaying = false;
    }

    void AnimatorComponent::Update(float deltaTime)
    {
        if (!m_IsPlaying || m_CurrentAnimation.empty()) return;

        auto it = m_Animations.find(m_CurrentAnimation);
        if (it == m_Animations.end()) return;

        const Animation& anim = it->second;
        m_Elapsed += deltaTime;

        if (m_Elapsed >= anim.frameTime)
        {
            m_Elapsed -= anim.frameTime;
            m_CurrentFrame++;
            if (m_CurrentFrame >= anim.frames.size())
            {
                if (anim.loop)
                    m_CurrentFrame = 0;
                else
                {
                    m_CurrentFrame = anim.frames.size() - 1;
                    m_IsPlaying = false;
                }
            }
            ApplyFrameToTexture();
        }
    }

    void AnimatorComponent::SetFlip(bool flip)
    {
        m_Flip = flip;
        if (m_pTextureComponent)
            m_pTextureComponent->SetFlipX(flip);
    }

    void AnimatorComponent::AddAnimationFromGrid(
        const std::string& name,
        std::shared_ptr<Texture2D> texture,
        int frameWidth,
        int frameHeight,
        int frameCount,
        float frameTime,
        bool loop,
        int startX,
        int startY
    )
    {
        std::vector<SDL_Rect> frames;
        if (!m_pTextureComponent) return;
        Vector2 texSize = m_pTextureComponent->GetTexturePixelSize();

        int columns = static_cast<int>(texSize.x) / frameWidth;
        int rows = static_cast<int>(texSize.y) / frameHeight;
        int count = 0;

        for (int y = 0; y < rows && count < frameCount; ++y)
        {
            for (int x = 0; x < columns && count < frameCount; ++x)
            {
                SDL_Rect rect;
                rect.x = startX + x * frameWidth;
                rect.y = startY + y * frameHeight;
                rect.w = frameWidth;
                rect.h = frameHeight;
                frames.push_back(rect);
                ++count;
            }
        }

        Animation anim;
        anim.name = name;
        anim.frames = frames;
        anim.frameTime = frameTime;
        anim.loop = loop;
        anim.texture = texture; // Store the texture
        AddAnimation(anim);
    }

    void AnimatorComponent::ApplyFrameToTexture()
    {
        if (!m_pTextureComponent) return;
        auto it = m_Animations.find(m_CurrentAnimation);
        if (it == m_Animations.end()) return;
        const Animation& anim = it->second;
        if (m_CurrentFrame < anim.frames.size())
            m_pTextureComponent->SetSourceRect(anim.frames[m_CurrentFrame].x, anim.frames[m_CurrentFrame].y, anim.frames[m_CurrentFrame].w, anim.frames[m_CurrentFrame].h);
    }
}