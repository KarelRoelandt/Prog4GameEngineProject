#include "BubbleComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Scene.h"

void dae::BubbleComponent::Update(float deltaTime)
{
    m_Elapsed += deltaTime;

    // Check lifetime
    if (m_Elapsed >= m_Lifetime)
    {
        if (auto owner = GetOwner())
        {
            if (auto scene = owner->GetScene())
                scene->QueueRemove(owner);
        }
        return;
    }

    if (auto transform = GetOwner()->GetComponent<TransformComponent>())
    {
        auto pos = transform->GetPosition();

        if (m_State == BubbleState::Forward)
        {
            pos += m_Speed * m_Direction * deltaTime;
            transform->SetPosition(pos.x, pos.y);

            float traveled = glm::distance(m_StartPosition, pos);
            if (m_MaxDistance > 0.0f && traveled >= m_MaxDistance)
            {
                // Change direction to up
                m_State = BubbleState::Up;
                m_UpStartPosition = pos;
                m_Direction = glm::vec2(0.0f, -1.0f); // Upwards in screen space
            }
        }
        else if (m_State == BubbleState::Up)
        {
            pos += m_Speed * m_Direction * deltaTime;
            transform->SetPosition(pos.x, pos.y);

            float upTraveled = glm::distance(m_UpStartPosition, pos);
            if (m_MaxUpDistance > 0.0f && upTraveled >= m_MaxUpDistance)
            {
                if (auto owner = GetOwner())
                {
                    if (auto scene = owner->GetScene())
                        scene->QueueRemove(owner);
                }
                return;
            }
        }
    }
}