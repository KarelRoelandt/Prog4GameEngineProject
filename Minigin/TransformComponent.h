#pragma once
#include "BaseComponent.h"
#include "CustomDefs.h"

namespace dae
{
    class TransformComponent final : public BaseComponent
    {
    public:
        TransformComponent() = default;
        ~TransformComponent() override = default;

        TransformComponent(const TransformComponent&) = delete;
        TransformComponent(TransformComponent&&) noexcept = delete;
        TransformComponent& operator=(const TransformComponent&) = delete;
        TransformComponent& operator=(TransformComponent&&) noexcept = delete;

        void Update(float deltaTime) override {}
        void Render() const override {}

        void SetPosition(float x, float y)
        {
            m_position = { x, y };
        }

        Vector2 GetPosition() const
        {
            return m_position;
        }

        void SetRotation(float angle)
        {
            m_rotation = angle;
        }

        float GetRotation() const
        {
            return m_rotation;
        }

        void SetScale(float x, float y)
        {
            m_scale = { x, y };
        }

        Vector2 GetScale() const
        {
            return m_scale;
        }

    private:
        Vector2 m_position{ 0, 0 };
        float m_rotation{ 0 };
        Vector2 m_scale{ 1, 1 };
    };
}