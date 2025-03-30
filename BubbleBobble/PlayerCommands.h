#pragma once
#include "Command.h"
#include "PlayerCharacterComponent.h"
#include <memory>

namespace dae
{
    class MoveCommand : public Command
    {
    public:
        MoveCommand(std::shared_ptr<PlayerCharacterComponent> player, float x, float y)
            : m_Player(player), m_X(x), m_Y(y) {
        }

        void Execute() override
        {
            if (auto player = m_Player.lock())
            {
                player->Move(m_X, m_Y);
            }
        }

    private:
        std::weak_ptr<PlayerCharacterComponent> m_Player;
        float m_X, m_Y;
    };

    class StopMoveCommand : public Command
    {
    public:
        StopMoveCommand(std::shared_ptr<PlayerCharacterComponent> player, float x, float y)
            : m_Player(player), m_X(x), m_Y(y) {
        }

        void Execute() override
        {
            if (auto player = m_Player.lock())
            {
                player->StopMove(m_X, m_Y);
            }
        }

    private:
        std::weak_ptr<PlayerCharacterComponent> m_Player;
        float m_X, m_Y;
    };
}