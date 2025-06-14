#pragma once
#include <vec2.hpp>
#include <cmath> // For std::abs
#include <memory> // For std::shared_ptr

#include "BaseComponent.h"
#include "GameObject.h"
#include "BoxCollisionComponent.h"
#include "AnimatorComponent.h"
#include "TransformComponent.h"

namespace dae
{
    class ZenChanMovementComponent final : public BaseComponent
    {
    public:
        // Constructor with owner parameter as required by your engine
        ZenChanMovementComponent(GameObject* owner, float movementSpeed = 50.0f, float gravity = 200.0f)
            : BaseComponent(owner),
            m_MovementSpeed(movementSpeed),
            m_Gravity(gravity),
            m_VerticalVelocity(0.0f),
            m_IsOnGround(false),
            m_pTransform(nullptr),
            m_pCollisionComponent(nullptr),
            m_pAnimator(nullptr)
        {
        }

        void Update(float deltaTime) override;
        void Render() const override {}

        void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
        void SetGravity(float gravity) { m_Gravity = gravity; }
        void SetMaxFallSpeed(float maxFallSpeed) { m_MaxFallSpeed = maxFallSpeed; }
        void SetPatrolRange(float range) { m_PatrolRange = range; }

    private:
        void ApplyGravity(float deltaTime);
        void HandleCollisions();
        void UpdateMovement(float deltaTime);
        void CheckGroundBeneath(); // Check if there's ground under ZenChan

        enum class MovementState
        {
            IDLE,
            WALKING_LEFT,
            WALKING_RIGHT,
            FALLING
        };

        float m_MovementSpeed{ 50.0f };
        float m_Gravity{ 200.0f };
        float m_VerticalVelocity{ 0.0f };
        float m_MaxFallSpeed{ 300.0f };
        float m_PatrolRange{ 960.0f };

        MovementState m_CurrentState{ MovementState::IDLE };
        float m_MovementDirection{ 1.0f }; // 1 for right, -1 for left
        float m_InitialX{ 0.0f };
        bool m_IsOnGround{ false };
        bool m_IsGoingThroughGap{ false }; // New flag to track gap falling
        bool m_InitialXSet{ false };

        // Raw pointers to components for efficiency
        TransformComponent* m_pTransform{ nullptr };
        BoxCollisionComponent* m_pCollisionComponent{ nullptr };
        AnimatorComponent* m_pAnimator{ nullptr };

        float m_ElapsedTime{ 0.0f };
        float m_DirectionChangeInterval{ 2.0f };
    };
}