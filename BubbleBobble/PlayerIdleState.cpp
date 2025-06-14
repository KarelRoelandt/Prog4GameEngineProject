// BubbleBobble/PlayerRunState.cpp
#include "PlayerIdleState.h"
#include "GameObject.h"
#include <iostream>

#include "AnimatorComponent.h"

void PlayerIdleState::Enter(dae::GameObject* owner)
{
    std::cout << "[PlayerIdleState] Enter\n";
    // Optionally: set animation, play sound, etc.
    auto animator = owner->GetComponent<dae::AnimatorComponent>();
    if (!(animator->GetCurrentAnimation() == "Shoot" && animator->IsPlaying()))
    {
        animator->Play("Idle");
    }

}

void PlayerIdleState::Update(dae::GameObject* owner, float /*deltaTime*/)
{
    auto animator = owner->GetComponent<dae::AnimatorComponent>();
    if (animator)
    {
        // If the current animation is "Shoot" and it just finished, play "Idle"
        if (animator->GetCurrentAnimation() == "Shoot" && !animator->IsPlaying())
        {
            animator->Play("Idle");
        }
    }
}

void PlayerIdleState::Exit(dae::GameObject* /*owner*/)
{
    std::cout << "[PlayerIdleState] Exit\n";
}