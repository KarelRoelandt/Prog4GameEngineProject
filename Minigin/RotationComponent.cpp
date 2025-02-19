#include "RotationComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <glm.hpp> // Ensure glm functions are available

namespace dae
{
    RotationComponent::RotationComponent(GameObject* owner, float rotationSpeed, const glm::vec2& center, float radius)
        : BaseComponent(owner), m_RotationSpeed(rotationSpeed), m_CurrentRotation(0.0f), m_Center(center), m_Radius(radius)
    {
    }

    void RotationComponent::Update(float deltaTime)
    {
        m_CurrentRotation += m_RotationSpeed * deltaTime;

        // Keep rotation within [0, 360)
        if (m_CurrentRotation >= 360.0f)
        {
            m_CurrentRotation -= 360.0f;
        }
        else if (m_CurrentRotation < 0.0f)
        {
            m_CurrentRotation += 360.0f;
        }

        auto transform = GetOwner()->GetComponent<TransformComponent>();
        if (!transform) return;

        float radian = glm::radians(m_CurrentRotation);
        float x = m_Center.x + m_Radius * cos(radian);
        float y = m_Center.y + m_Radius * sin(radian);

        transform->SetPosition(x, y);
        transform->SetRotation(m_CurrentRotation);
    }
}
