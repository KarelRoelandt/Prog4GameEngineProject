#include "PlayerShootState.h"
#include "GameObject.h"
#include "PlayerCharacterComponent.h"
#include "AnimatorComponent.h"

void dae::PlayerShootState::Enter(GameObject* owner)
{
    // Play shoot animation
    if (auto anim = owner->GetComponent<AnimatorComponent>())
        anim->Play("Shoot", false);

    // Shoot bubble
    if (auto player = owner->GetComponent<PlayerCharacterComponent>())
        player->ShootBubble();
}

void dae::PlayerShootState::Update(GameObject* /*owner*/, float /*deltaTime*/)
{
    // Optionally, transition back to idle/run after animation is done
    // Or use a timer, or check if shoot animation is finished
}

void dae::PlayerShootState::Exit(GameObject* /*owner*/)
{
    // Any cleanup if needed
}