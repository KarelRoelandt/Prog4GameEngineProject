#include "GameObject.h"
#include "RotationComponent.h"
#include "TransformComponent.h"

#include <trigonometric.hpp>

namespace dae
{
    RotationComponent::RotationComponent(GameObject* owner, float rotationSpeed, const glm::vec2& rotationPoint, float radius)
        : BaseComponent(owner), m_RotationSpeed(rotationSpeed), m_RotationPoint(rotationPoint), m_Radius(radius), m_InitialPositionSet(false)
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

        auto owner = GetOwner();
        if (owner)
        {
            auto transformComponent = owner->GetComponent<TransformComponent>();
            if (transformComponent)
            {
                glm::vec2 center;

                // Check if the object has a parent
                auto parent = owner->GetParent();
                if (parent)
                {
                    // Use parent's position as the center
                    auto parentTransform = parent->GetComponent<TransformComponent>();
                    if (parentTransform)
                    {
                        center = parentTransform->GetPosition();
                    }
                }
                else
                {
                    // Store the initial position if not already set
                    if (!m_InitialPositionSet)
                    {
                        m_InitialPosition = transformComponent->GetPosition();
                        m_InitialPositionSet = true;
                    }
                    center = m_InitialPosition;
                }

                glm::vec2 newPosition = center + glm::vec2(
                    m_Radius * cos(glm::radians(m_CurrentRotation)),
                    m_Radius * sin(glm::radians(m_CurrentRotation))
                );

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