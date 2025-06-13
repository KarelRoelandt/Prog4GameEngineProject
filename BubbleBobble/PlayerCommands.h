#pragma once

#include <memory>

#include "Command.h"
#include "PlayerShootState.h"

class PlayerCharacterComponent;


namespace dae
{
    class MoveCommand : public Command
    {
    public:
        MoveCommand(std::shared_ptr<PlayerCharacterComponent> player, float x, float y)
            : m_pPlayer(player), m_DirectionX(x), m_DirectionY(y)
        {
        }
        void Execute() override
        {
            //std::cout << "MoveCommand executed\n";
            if (m_pPlayer)
                m_pPlayer->Move(m_DirectionX, m_DirectionY);
        }
    private:
        std::shared_ptr<PlayerCharacterComponent> m_pPlayer;
        float m_DirectionX;
        float m_DirectionY;
    };

    class StopMoveCommand : public Command
    {
    public:
        StopMoveCommand(std::shared_ptr<PlayerCharacterComponent> player, float x, float y)
            : m_pPlayer(player), m_DirectionX(x), m_DirectionY(y)
        {
        }
        void Execute() override
        {
            if (m_pPlayer)
                m_pPlayer->StopMove(m_DirectionX, m_DirectionY);
        }
    private:
        std::shared_ptr<PlayerCharacterComponent> m_pPlayer;
        float m_DirectionX;
        float m_DirectionY;
    };

    class JumpCommand : public Command
    {
    public:
        JumpCommand(std::shared_ptr<PlayerCharacterComponent> player)
            : m_pPlayer(player)
        {
        }
        void Execute() override
        {
            if (m_pPlayer)
                m_pPlayer->Jump();
        }
    private:
        std::shared_ptr<PlayerCharacterComponent> m_pPlayer;
    };


    // Command class for causing damage
    class DamageCommand : public Command
    {
    public:
        DamageCommand(std::shared_ptr<PlayerCharacterComponent> player, int damageAmount)
            : m_pPlayer(player), m_DamageAmount(damageAmount)
        {
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
            : m_pPlayer(player), m_Points(points)
        {
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

    class ShootCommand : public Command
    {
    public:
        ShootCommand(std::shared_ptr<PlayerCharacterComponent> player) : m_pPlayer(player) {}
        void Execute() override
        {
            if (!m_pPlayer) return;
            auto owner = m_pPlayer->GetOwner();
            if (!owner) return;
            auto stateMachine = owner->GetComponent<StateMachineComponent>();
            if (stateMachine)
                stateMachine->ChangeState(std::make_unique<PlayerShootState>());
        }
    private:
        std::shared_ptr<PlayerCharacterComponent> m_pPlayer;
    };

}