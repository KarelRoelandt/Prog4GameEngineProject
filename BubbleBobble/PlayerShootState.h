#pragma once
#include "CharacterState.h"

namespace dae
{
    class PlayerShootState : public CharacterState
    {
    public:
        void Enter(GameObject* owner) override;
        void Update(GameObject* owner, float deltaTime) override;
        void Exit(GameObject* owner) override;
    };
}