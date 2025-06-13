// BubbleBobble/CharacterState.h
#pragma once

#include "StateMachineComponent.h"

class GameObject;

class CharacterState : public dae::IState
{
public:
    virtual ~CharacterState() override = default;
    virtual void Enter(dae::GameObject* owner) = 0;
    virtual void Update(dae::GameObject* owner, float deltaTime) = 0;
    virtual void Exit(dae::GameObject* owner) = 0;
    // Optionally, handle input or events
};