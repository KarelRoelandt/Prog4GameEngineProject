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
        TransformComponent(GameObject* owner);
        ~TransformComponent() override = default;

        TransformComponent(const TransformComponent& other) = delete;
        TransformComponent(TransformComponent&& other) noexcept = delete;
        TransformComponent& operator=(const TransformComponent& other) = delete;
        TransformComponent& operator=(TransformComponent&& other) noexcept = delete;

        void Update(float deltaTime) override;
        void Render() const override;

        void SetLocalPosition(const glm::vec3& position);
        const glm::vec3& GetLocalPosition() const;

        void SetLocalRotation(const glm::quat& rotation);
        const glm::quat& GetLocalRotation() const;

        void SetLocalScale(const glm::vec3& scale);
        const glm::vec3& GetLocalScale() const;

        const glm::mat4& GetWorldTransform() const;

        void SetPosition(float x, float y); // 2D convenience
        glm::vec2 GetPosition() const;   // 2D convenience

        void SetDirty(); // Public method to mark this transform and its children as dirty

        void PrintPositions() const;

    private:
        void UpdateWorldTransform();

        glm::vec3 m_LocalPosition;
        glm::quat m_LocalRotation;
        glm::vec3 m_LocalScale;
        glm::mat4 m_WorldTransform;
        bool m_IsDirty;
    };
}