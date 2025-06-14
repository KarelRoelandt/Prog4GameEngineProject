// BubbleBobble/PlayerRunState.cpp
#include "PlayerRunState.h"

#include <iostream>

#include "GameObject.h"
#include "AnimatorComponent.h"



void PlayerRunState::Enter(dae::GameObject* owner)
{
    std::cout << "[PlayerRunState] Enter\n";

    auto animator = owner->GetComponent<dae::AnimatorComponent>();
    if (animator)
    {
        if (!(animator->GetCurrentAnimation() == "Shoot" && animator->IsPlaying()))
        {
            animator->Play("Run");
        }
    }
}

void PlayerRunState::Update(dae::GameObject* owner, float /*deltaTime*/)
{
    auto animator = owner->GetComponent<dae::AnimatorComponent>();
    if (animator)
    {
        // If the current animation is "Shoot" and it just finished, play "Idle"
        if (animator->GetCurrentAnimation() == "Shoot" && !animator->IsPlaying())
        {
            animator->Play("Run");
        }
    }
}

void PlayerRunState::Exit(dae::GameObject* /*owner*/)
{
    std::cout << "[PlayerRunState] Exit\n";
}