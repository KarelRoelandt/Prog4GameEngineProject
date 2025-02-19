#include "RotationComponent.h"
#include "GameObject.h" // Include the GameObject header file

#include <trigonometric.hpp>

#include "TransformComponent.h"

namespace dae
{
    RotationComponent::RotationComponent(GameObject* owner, float rotationSpeed, const glm::vec2& rotationPoint, float radius)
        : BaseComponent(owner), m_RotationSpeed(rotationSpeed), m_RotationPoint(rotationPoint), m_Radius(radius)
    {
    }

    void RotationComponent::Update(float deltaTime)
    {
        // Calculate the change in rotation based on speed and delta time
        m_CurrentRotation += m_RotationSpeed * deltaTime;

        // If rotation goes beyond 360 degrees, reset it to 0 to prevent overflow
        if (m_CurrentRotation >= 360.0f)
        {
            m_CurrentRotation -= 360.0f;
        }

        // Calculate the new position after applying the rotation
        // (This can be used for updating the transform if needed)
        glm::vec2 newPosition = m_RotationPoint + glm::vec2(
            m_Radius * cos(glm::radians(m_CurrentRotation)),
            m_Radius * sin(glm::radians(m_CurrentRotation))
        );

        // Set the new position or update other necessary components here
        auto owner = GetOwner();
        if (owner)
        {
            auto transformComponent = owner->GetComponent<TransformComponent>();
            if (transformComponent)
            {
                transformComponent->SetPosition(newPosition.x, newPosition.y);
            }
        }
    }

    void RotationComponent::SetRotationSpeed(float speed)
    {
        m_RotationSpeed = speed;
    }

    float RotationComponent::GetRotationSpeed() const
    {
        return m_RotationSpeed;
    }

    void RotationComponent::SetRotationPoint(const glm::vec2& point)
    {
        m_RotationPoint = point;
    }

    const glm::vec2& RotationComponent::GetRotationPoint() const
    {
        return m_RotationPoint;
    }

    void RotationComponent::SetRadius(float radius)
    {
        m_Radius = radius;
    }

    float RotationComponent::GetRadius() const
    {
        return m_Radius;
    }
}
