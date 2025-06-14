#pragma once
#include <vec2.hpp>

#include "BaseComponent.h"

namespace dae
{
    enum class BubbleState { Forward, Up };

    class BubbleComponent : public BaseComponent
    {
    public:
        BubbleComponent(GameObject* owner, float speed, glm::vec2 direction, float lifetime = 2.0f)
            : BaseComponent(owner), m_Speed(speed), m_Direction(direction), m_Lifetime(lifetime), m_Elapsed(0.0f)
        {
        }

        void Update(float deltaTime) override;
        void Render() const override {}

        void SetMaxDistance(float maxDistance) { m_MaxDistance = maxDistance; }
        void SetMaxUpDistance(float maxUpDistance) { m_MaxUpDistance = maxUpDistance; }
        void SetStartPosition(const glm::vec2& pos) { m_StartPosition = pos; }
        void SetDirection(const glm::vec2& dir) { m_Direction = dir; }

    private:
        float m_Speed;
        glm::vec2 m_Direction;
        float m_Lifetime;
        float m_Elapsed;

        float m_MaxDistance = 0.0f;
        float m_MaxUpDistance = 0.0f;
        glm::vec2 m_StartPosition{ 0.0f, 0.0f };
        glm::vec2 m_UpStartPosition{ 0.0f, 0.0f };
        BubbleState m_State = BubbleState::Forward;

    };
}