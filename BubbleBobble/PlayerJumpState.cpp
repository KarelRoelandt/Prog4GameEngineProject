#include "PlayerJumpState.h"

#include <iostream>

#include "GameObject.h"

#include "ISoundService.h"
#include "ServiceLocator.h"


void PlayerJumpState::Enter(dae::GameObject* /*owner*/)
{
    std::cout << "[PlayerRunState] Enter\n";

    ServiceLocator::GetSoundService()->OutputSound("Player/Jump.wav", 32);

}

void PlayerJumpState::Update(dae::GameObject* /*owner*/, float /*deltaTime*/)
{
    // Optionally: handle run logic
}

void PlayerJumpState::Exit(dae::GameObject* /*owner*/)
{
    std::cout << "[PlayerRunState] Exit\n";
}