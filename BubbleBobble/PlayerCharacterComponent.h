// PlayerCharacterComponent.h
#pragma once

#include "BaseComponent.h"
#include <glm.hpp>
#include <memory>
#include "Command.h"

#include "SoundService.h"
#include "StateMachineComponent.h" // Add this include
#include "PlayerRunState.h"        // Add this include

class Event;  // Forward declaration of Event
class Observer;  // Forward declaration of Observer

namespace dae
{
    class GameObject;
    class TransformComponent;
    class InputManager;

    class PlayerCharacterComponent final : public BaseComponent
    {
    public:
        PlayerCharacterComponent(GameObject* owner, float speed);
        ~PlayerCharacterComponent() = default;
        void Update(float deltaTime) override;
        void Render() const override;
        void Move(float x, float y); // Method to be called by MoveCommand
        void StopMove(float x, float y);

        void UpdateDirection();

        // New method for binding inputs
        void BindInputs(bool isKeyboard, int controllerIdx = 0);

        // New method for doing damage
        void DoDamage(int amount);

        void AddScore(int points);

    private:
        float m_Speed;
        bool m_MovingLeft, m_MovingRight, m_MovingUp, m_MovingDown;
        glm::vec2 m_Direction{ 0.0f, 0.0f };
        TransformComponent* m_pTransform{ nullptr };

        std::shared_ptr<ISoundService> m_pSoundService;

        dae::StateMachineComponent* m_pStateMachine{ nullptr }; // Add state machine pointer

        void EnsureStateMachine(); // Helper to cache state machine

    };


    // Command class for movement
    class MoveCommand : public Command
    {
    public:
        MoveCommand(std::shared_ptr<PlayerCharacterComponent> player, float x, float y)
            : m_pPlayer(player), m_DirectionX(x), m_DirectionY(y) {
        }
        void Execute() override
        {
            if (m_pPlayer)
            {
                m_pPlayer->Move(m_DirectionX, m_DirectionY);
            }
        }
    private:
        std::shared_ptr<PlayerCharacterComponent> m_pPlayer;
        float m_DirectionX;
        float m_DirectionY;
    };

    // New StopMoveCommand
    class StopMoveCommand : public Command
    {
    public:
        StopMoveCommand(std::shared_ptr<PlayerCharacterComponent> player, float x, float y)
            : m_pPlayer(player), m_DirectionX(x), m_DirectionY(y) {
        }
        void Execute() override
        {
            if (m_pPlayer)
            {
                m_pPlayer->StopMove(m_DirectionX, m_DirectionY);
            }
        }
    private:
        std::shared_ptr<PlayerCharacterComponent> m_pPlayer;
        float m_DirectionX;
        float m_DirectionY;
    };


    // Command class for causing damage
    class DamageCommand : public Command
    {
    public:
        DamageCommand(std::shared_ptr<PlayerCharacterComponent> player, int damageAmount)
            : m_pPlayer(player), m_DamageAmount(damageAmount) {
        }

        void Execute() override
        {
            if (m_pPlayer)
            {
                m_pPlayer->DoDamage(m_DamageAmount);
            }
        }

    private:
        std::shared_ptr<PlayerCharacterComponent> m_pPlayer;
        int m_DamageAmount;
    };

    class ScoreCommand : public Command
    {
    public:
        ScoreCommand(std::shared_ptr<PlayerCharacterComponent> player, int points)
            : m_pPlayer(player), m_Points(points) {
        }

        void Execute() override
        {
            if (m_pPlayer)
            {
                m_pPlayer->AddScore(m_Points);
            }
        }

    private:
        std::shared_ptr<PlayerCharacterComponent> m_pPlayer;
        int m_Points;
    };

}

