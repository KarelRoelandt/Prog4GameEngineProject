// SinglePlayerState.cpp
#include "SinglePlayerState.h"

// Add all necessary component includes at the top
#include "GameObject.h"
#include "TextComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "HealthComponent.h"
#include "PlayerCharacterComponent.h"
#include "ScoreComponent.h"
#include "HealthDisplay.h"
#include "ScoreDisplay.h"
#include "Scene.h"

void SinglePlayerState::SetupPlayers(dae::Scene& scene)
{
    // Create only Player1
    auto fontPlayer = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 16);

    // Add instructions for Player1 only
    auto textPlayer1 = std::make_shared<dae::GameObject>();
    textPlayer1->AddComponent<dae::TextComponent>("Use WASD to move Bub (green), C to do damage, Z and X to add score.", fontPlayer);
    auto transformComponent = textPlayer1->AddComponent<dae::TransformComponent>();
    transformComponent->SetPosition(10, 100);
    textPlayer1->AddComponent<dae::RenderComponent>();
    scene.Add(textPlayer1);

    // Create player1 (keyboard-based)
    auto player1 = std::make_shared<dae::GameObject>();
    player1->SetName("Player1");
    // Add components
    auto textureComp = player1->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture("Sprites/Bub.png");
    auto transformComp = player1->AddComponent<dae::TransformComponent>();
    transformComp->SetPosition(200, 300);
    player1->AddComponent<dae::RenderComponent>();
    // Add player control
    auto player1Component = player1->AddComponent<dae::PlayerCharacterComponent>(100.0f);
    player1Component->BindInputs(true); // true = keyboard controls
    // Add health and score
    auto healthComponent = player1->AddComponent<dae::HealthComponent>(player1.get(), 3);
    auto scoreComponent = player1->AddComponent<dae::ScoreComponent>(player1.get(), 0);
    scene.Add(player1);

    // Add UI for health and score
    auto healthDisplayObject = std::make_shared<dae::GameObject>();
    auto textComponent = healthDisplayObject->AddComponent<dae::TextComponent>("# lives: 3", fontPlayer);
    auto healthTextTransform = healthDisplayObject->AddComponent<dae::TransformComponent>();
    healthTextTransform->SetPosition(10, 150);
    healthDisplayObject->AddComponent<dae::RenderComponent>();
    // Add HealthDisplay (Observer) as a component
    auto healthDisplay = healthDisplayObject->AddComponent<dae::HealthDisplay>(healthDisplayObject.get(), textComponent);
    healthComponent->AddObserver(healthDisplay.get());  // Pass the raw pointer
    scene.Add(healthDisplayObject);

    // Create ScoreDisplay GameObject
    auto scoreDisplayObject = std::make_shared<dae::GameObject>();
    auto scoreTextComponent = scoreDisplayObject->AddComponent<dae::TextComponent>("Score: 0", fontPlayer);
    auto scoreTextTransform = scoreDisplayObject->AddComponent<dae::TransformComponent>();
    scoreTextTransform->SetPosition(10, 170); // Position it appropriately
    scoreDisplayObject->AddComponent<dae::RenderComponent>();
    // Add ScoreDisplay (Observer) as a component
    auto scoreDisplay = scoreDisplayObject->AddComponent<dae::ScoreDisplay>(scoreDisplayObject.get(), scoreTextComponent);
    scoreComponent->AddObserver(scoreDisplay.get());  // Pass the raw pointer
    scene.Add(scoreDisplayObject);
}