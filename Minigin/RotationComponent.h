#pragma once
#include <vec2.hpp>

#include "BaseComponent.h"
#include "CustomDefs.h"

namespace dae
{
    class RotationComponent final : public BaseComponent
    {
    public:
        RotationComponent(GameObject* owner, float rotationSpeed, const glm::vec2& center, float radius);
        ~RotationComponent() override = default;

        RotationComponent(const RotationComponent&) = delete;
        RotationComponent(RotationComponent&&) = delete;
        RotationComponent& operator=(const RotationComponent&) = delete;
        RotationComponent& operator=(RotationComponent&&) = delete;

        void Update(float deltaTime) override;
        void Render() const override {}

        void SetRotationSpeed(float speed) { m_RotationSpeed = speed; }
        float GetRotationSpeed() const { return m_RotationSpeed; }

        void SetCenter(const glm::vec2& center) { m_Center = center; }
        glm::vec2 GetCenter() const { return m_Center; }

        void SetRadius(float radius) { m_Radius = radius; }
        float GetRadius() const { return m_Radius; }

    private:
        float m_RotationSpeed;
        float m_CurrentRotation;
        glm::vec2 m_Center;
        float m_Radius;
    };
}
