#pragma once

#include <vec2.hpp>
#include "BaseComponent.h"
#include "CustomDefs.h"

namespace dae
{
    class RotationComponent final : public BaseComponent
    {
    public:
        // Constructor with default values for rotation point and radius
        RotationComponent(GameObject* owner, float rotationSpeed, const glm::vec2& rotationPoint = glm::vec2(0.0f, 0.0f), float radius = 1.0f);
        ~RotationComponent() override = default;

        // Deleted copy and move constructors/assignments
        RotationComponent(const RotationComponent&) = delete;
        RotationComponent(RotationComponent&&) = delete;
        RotationComponent& operator=(const RotationComponent&) = delete;
        RotationComponent& operator=(RotationComponent&&) = delete;

        // Update method to be called every frame
        void Update(float deltaTime) override;

        // Render method (empty for now)
        void Render() const override {}

        // Setter and Getter methods for rotation speed, rotation point, and radius
        void SetRotationSpeed(float speed);
        float GetRotationSpeed() const;

        void SetRotationPoint(const glm::vec2& point);
        const glm::vec2& GetRotationPoint() const; // Return by const reference for efficiency

        void SetRadius(float radius);
        float GetRadius() const;

    private:
        float m_RotationSpeed; // Speed at which the object rotates
        float m_CurrentRotation{ 0.0f }; // Current rotation angle
        glm::vec2 m_RotationPoint; // Point around which the object rotates
        float m_Radius; // Distance from the rotation point
        bool m_InitialPositionSet; // Flag to check if the initial position is set
        glm::vec2 m_InitialPosition; // Initial position of the object
    };
}
