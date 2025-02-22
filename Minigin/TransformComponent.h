#pragma once
#include "BaseComponent.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <iostream> // For debugging

namespace dae
{
    class TransformComponent final : public BaseComponent
    {
    public:
        TransformComponent(GameObject* owner)
            : BaseComponent(owner), m_LocalPosition(0.0f, 0.0f, 0.0f), m_LocalRotation(glm::quat()), m_LocalScale(1.0f, 1.0f, 1.0f), m_WorldTransform(1.0f), m_IsDirty(true) {
        }
        ~TransformComponent() override = default;

        TransformComponent(const TransformComponent&) = delete;
        TransformComponent(TransformComponent&&) noexcept = delete;
        TransformComponent& operator=(const TransformComponent&) = delete;
        TransformComponent& operator=(TransformComponent&&) noexcept = delete;

        void Update(float /*deltaTime*/) override
        {
            if (m_IsDirty)
            {
                UpdateWorldTransform();
                m_IsDirty = false;
            }
        }

        void Render() const override {}

        void SetLocalPosition(const glm::vec3& position)
        {
            m_LocalPosition = position;
            m_IsDirty = true;
        }

        const glm::vec3& GetLocalPosition() const
        {
            return m_LocalPosition;
        }

        void SetLocalRotation(const glm::quat& rotation)
        {
            m_LocalRotation = rotation;
            m_IsDirty = true;
        }

        const glm::quat& GetLocalRotation() const
        {
            return m_LocalRotation;
        }

        void SetLocalScale(const glm::vec3& scale)
        {
            m_LocalScale = scale;
            m_IsDirty = true;
        }

        const glm::vec3& GetLocalScale() const
        {
            return m_LocalScale;
        }

        const glm::mat4& GetWorldTransform() const
        {
            return m_WorldTransform;
        }

        // Add the SetPosition method
        void SetPosition(float x, float y)
        {
            m_LocalPosition.x = x;
            m_LocalPosition.y = y;
            m_IsDirty = true;
            //std::cout << "SetPosition called with x: " << x << ", y: " << y << std::endl; // Debugging
        }

        // Add the GetPosition method
        glm::vec2 GetPosition() const
        {
            return { m_LocalPosition.x, m_LocalPosition.y };
        }

    private:
        void UpdateWorldTransform()
        {
            m_WorldTransform = glm::translate(glm::mat4(1.0f), m_LocalPosition) *
                glm::mat4_cast(m_LocalRotation) *
                glm::scale(glm::mat4(1.0f), m_LocalScale);

            auto owner = GetOwner();
            if (owner)
            {
                auto parent = owner->GetParent();
                if (parent)
                {
                    auto parentTransform = parent->GetComponent<TransformComponent>();
                    if (parentTransform)
                    {
                        m_WorldTransform = parentTransform->GetWorldTransform() * m_WorldTransform;
                    }
                }
            }
            //std::cout << "UpdateWorldTransform called. Local Position: (" << m_LocalPosition.x << ", " << m_LocalPosition.y << ", " << m_LocalPosition.z << ")" << std::endl; // Debugging
        }

        glm::vec3 m_LocalPosition;
        glm::quat m_LocalRotation;
        glm::vec3 m_LocalScale;
        glm::mat4 m_WorldTransform;
        bool m_IsDirty;
    };
}
