#pragma once
#include "Command.h"
#include "PlayerCharacterComponent.h"
#include <memory>

namespace dae
{
    class MoveCommand : public Command
    {
    public:
        MoveCommand(std::shared_ptr<PlayerCharacterComponent> character, float dx, float dy)
            : m_Character(character), m_DX(dx), m_DY(dy) {
        }

        void Execute() override
        {
            m_Character->Move(m_DX, m_DY);
        }

    private:
        std::shared_ptr<PlayerCharacterComponent> m_Character;
        float m_DX;
        float m_DY;
    };
}