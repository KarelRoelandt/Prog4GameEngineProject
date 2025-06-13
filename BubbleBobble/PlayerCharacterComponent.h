// PlayerCharacterComponent.h
#pragma once

#include <memory>
#include <glm.hpp>

#include "BaseComponent.h"

#include "SoundService.h"
#include "StateMachineComponent.h" // Add this include

class Event;  // Forward declaration of Event
class Observer;  // Forward declaration of Observer

namespace dae
{
	class BoxCollisionComponent;
	class GameObject;
    class TransformComponent;
    class InputManager;

    class PlayerCharacterComponent final : public BaseComponent
    {
    public:
        PlayerCharacterComponent(GameObject* owner, float speed);
        ~PlayerCharacterComponent() = default;

    	void Update(float deltaTime) override;
        void HandleCollisions(float);
        void Render() const override;

        void Move(float x, float y); // Method to be called by MoveCommand
        void StopMove(float x, float y);

        void Jump();

        //void UpdateDirection();

        // New method for binding inputs
        void BindInputs(bool isKeyboard, int controllerIdx = 0);

        // New method for doing damage
        void DoDamage(int amount);
        void AddScore(int points);

    private:
        float m_Speed;
        glm::vec2 m_Direction{ 0.0f, 0.0f };

        float m_VerticalVelocity{ 0.0f };
        bool m_IsOnGround{}; // Set to true when the player is on the ground
        const float m_JumpStrength{ 550.0f }; // Adjust as needed
        const float m_Gravity{ 991.0f };      // Adjust as needed
        BoxCollisionComponent* m_pPlayerCollider = nullptr;

        TransformComponent* m_pTransform{ nullptr };

        std::shared_ptr<ISoundService> m_pSoundService;

        dae::StateMachineComponent* m_pStateMachine{ nullptr }; // Add state machine pointer

        void EnsureStateMachine(); // Helper to cache state machine

    };


}

