#pragma once

#include "CharacterState.h"

class PlayerJumpState : public CharacterState
{
public:
    void Enter(dae::GameObject* owner) override;
    void Update(dae::GameObject* owner, float deltaTime) override;
    void Exit(dae::GameObject* owner) override;
};