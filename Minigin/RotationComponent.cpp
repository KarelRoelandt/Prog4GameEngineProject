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

        // Initialize newPosition to avoid potential uninitialized use
        glm::vec2 newPosition(0.0f, 0.0f);
        auto owner = GetOwner();
        if (owner)
        {
            auto parent = owner->GetParent();
            if (parent)
            {
                auto parentPos = parent->GetComponent<TransformComponent>()->GetPosition();
                newPosition = parentPos + glm::vec2(
                    m_Radius * cos(glm::radians(m_CurrentRotation)),
                    m_Radius * sin(glm::radians(m_CurrentRotation))
                );
            }
            else
            {
                auto transform = owner->GetComponent<TransformComponent>();
                if (transform)
                {
                    auto center = transform->GetPosition(); // Use object's own position
                    newPosition = center + glm::vec2(
                        m_Radius * cos(glm::radians(m_CurrentRotation)),
                        m_Radius * sin(glm::radians(m_CurrentRotation))
                    );
                }
            }

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
