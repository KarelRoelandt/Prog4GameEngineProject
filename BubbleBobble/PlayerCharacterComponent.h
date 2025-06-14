#pragma once
#include "BaseComponent.h"
#include "vec2.hpp"
#include <memory>

#include "InputManager.h" 
#include "Controller.h"   

// Forward declarations
namespace dae
{
    class GameObject;
    class TransformComponent;
    class BoxCollisionComponent;
    class StateMachineComponent;
    class Scene;
}

class ISoundService;

namespace dae
{
    class PlayerCharacterComponent : public BaseComponent
    {
    public:
        PlayerCharacterComponent(GameObject* owner, float speed);
        ~PlayerCharacterComponent() override = default;

        PlayerCharacterComponent(const PlayerCharacterComponent& other) = delete;
        PlayerCharacterComponent(PlayerCharacterComponent&& other) noexcept = delete;
        PlayerCharacterComponent& operator=(const PlayerCharacterComponent& other) = delete;
        PlayerCharacterComponent& operator=(PlayerCharacterComponent&& other) noexcept = delete;

        void Update(float deltaTime) override;
        void Render() const override;

        void SetCurrentScene(Scene* scene);
        Scene* GetCurrentScene() const { return m_pCurrentScene; }

        void Move(float x, float y = 0.0f);
        void StopMove(float x, float y = 0.0f);
        void Jump();
        void ShootBubble();
        void DoDamage(int amount);
        void AddScore(int points);

        glm::vec2 GetDirection() const { return m_Direction; }
        glm::vec2 GetFacingDirection() const { return m_FacingDirection; }

        void BindInputs(bool isKeyboard, int playerNumberForInput);

        bool IsOnGround() const { return m_IsOnGround; }

    protected:
        void HandleCollisions(float deltaTime);
        void EnsureStateMachine();

        float m_Speed;
        glm::vec2 m_Direction;
        float m_VerticalVelocity;
        bool m_IsOnGround;
        float m_JumpStrength;
        float m_Gravity;

        glm::vec2 m_FacingDirection{ 1.0f, 0.0f }; // Default facing right

        TransformComponent* m_pTransform;
        BoxCollisionComponent* m_pPlayerCollider;
        StateMachineComponent* m_pStateMachine;
        ISoundService* m_pSoundService; // Using global ISoundService*
        Scene* m_pCurrentScene;
    };
}