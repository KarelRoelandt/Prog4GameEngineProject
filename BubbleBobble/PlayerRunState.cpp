// BubbleBobble/PlayerRunState.cpp
#include "PlayerRunState.h"
#include "GameObject.h"
#include <iostream>

void PlayerRunState::Enter(dae::GameObject* /*owner*/)
{
    std::cout << "[PlayerRunState] Enter\n";
    // Optionally: set animation, play sound, etc.
}

void PlayerRunState::Update(dae::GameObject* /*owner*/, float /*deltaTime*/)
{
    // Optionally: handle run logic
}

void PlayerRunState::Exit(dae::GameObject* /*owner*/)
{
    std::cout << "[PlayerRunState] Exit\n";
}