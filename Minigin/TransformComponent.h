#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>

#include "BaseComponent.h"

namespace dae { class GameObject; }


namespace dae
{
    class TransformComponent final : public BaseComponent
    {
    public:
        TransformComponent(GameObject* owner)
            : BaseComponent(owner), m_LocalPosition(0.0f, 0.0f, 0.0f), m_LocalRotation(glm::quat()),
            m_LocalScale(1.0f, 1.0f, 1.0f), m_WorldTransform(1.0f), m_IsDirty(true)
        {
        }

        ~TransformComponent() override = default;

        void Update(float /*deltaTime*/) override
        {
            if (m_IsDirty)
            {
                UpdateWorldTransform();
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
            if (m_IsDirty)
            {
                const_cast<TransformComponent*>(this)->UpdateWorldTransform();
            }
            return m_WorldTransform;
        }

        void SetPosition(float x, float y)
        {
            m_LocalPosition.x = x;
            m_LocalPosition.y = y;
            m_IsDirty = true;
        }

        glm::vec2 GetPosition() const
        {
            return glm::vec2(m_LocalPosition.x, m_LocalPosition.y);
        }

        void PrintPositions() const
        {
            std::cout << "Local Position: (" << m_LocalPosition.x << ", " << m_LocalPosition.y << ", " << m_LocalPosition.z << ")\n";
            const glm::mat4& worldTransform = GetWorldTransform();
            glm::vec3 worldPosition = glm::vec3(worldTransform[3]);
            std::cout << "World Position: (" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << ")\n";
        }

    private:
        void UpdateWorldTransform(); // Declaration only - implementation in .cpp file

        glm::vec3 m_LocalPosition;
        glm::quat m_LocalRotation;
        glm::vec3 m_LocalScale;
        glm::mat4 m_WorldTransform;

        bool m_IsDirty;
    };
}