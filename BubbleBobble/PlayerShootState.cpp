#include "PlayerShootState.h"

#include "GameObject.h"
#include "PlayerCharacterComponent.h"
#include "AnimatorComponent.h"
#include "BoxCollisionComponent.h"
#include "BubbleComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "ServiceLocator.h"
#include "ISoundService.h"

void dae::PlayerShootState::Enter(GameObject* owner)
{
    auto currentScene = owner->GetComponent<dae::PlayerCharacterComponent>()->GetCurrentScene();
    if (!currentScene) return;

    // Play shoot animation
    if (auto anim = owner->GetComponent<AnimatorComponent>())
    {
	    anim->Play("Shoot", false);
    }
    
    std::string playerTexturePath;

   if (owner->GetName() == "Player1")
    {
        playerTexturePath = "PLayer/Bubby/";
    }
    else
    {
        playerTexturePath = "PLayer/Bobby/";
    }

    // Create bubble GameObject
    auto bubble = std::make_shared<GameObject>();
    bubble->SetName("Bubble");

    // Set bubble position to player's position
    glm::vec2 ownerPosition = owner->GetTransform()->GetPosition();
    bubble->GetTransform()->SetPosition(ownerPosition.x, ownerPosition.y);

    // Add components: Texture, Render, Movement, Collision, etc.
    bubble->AddComponent<dae::TextureComponent>()->SetTexture(playerTexturePath + "Bubble_Anim.png");
    bubble->GetComponent<dae::TextureComponent>()->SetRenderSize(48, 48);
    bubble->AddComponent<dae::RenderComponent>();
    bubble->AddComponent<dae::BubbleComponent>(200.f, owner->GetComponent<dae::PlayerCharacterComponent>()->GetFacingDirection(), 5.f);
    bubble->GetComponent<dae::BubbleComponent>()->SetStartPosition(ownerPosition);
    bubble->GetComponent<dae::BubbleComponent>()->SetMaxDistance(300.0f);
    bubble->GetComponent<dae::BubbleComponent>()->SetMaxUpDistance(200.0f);

    auto bubblesize = bubble->GetComponent<TextureComponent>()->GetRenderDestinationSize();
    bubble->AddComponent<dae::BoxCollisionComponent>(ownerPosition.x, ownerPosition.y, bubblesize.x - 4, bubblesize.x - 4, dae::ColliderTag::BUBBLE);

    auto bulletTexture = dae::ResourceManager::GetInstance().LoadTexture(playerTexturePath + "Bubble_Anim.png");
    auto animator = bubble->AddComponent<dae::AnimatorComponent>();
    animator->AddAnimationFromGrid("ShootBubble", bulletTexture, 48, 48, 3, 0.08f, true);
    animator->Play("ShootBubble");

    ServiceLocator::GetSoundService()->OutputSound("Player/Shoot.wav", 32);

    currentScene->Add(bubble);
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