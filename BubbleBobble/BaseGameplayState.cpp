// BaseGameplayState.cpp

#include <iostream>

#include "GameObject.h"
#include "Game.h" // Ensure Game is fully defined

#include "BaseGameplayState.h"
#include "HighScoreState.h"
#include "InputManager.h"
#include "RenderComponent.h"
#include "ServiceLocator.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "SoundService.h"

// LeaveGameCommand constructor
BaseGameplayState::LeaveGameCommand::LeaveGameCommand(Game* gamePtr)
    : m_Game(gamePtr)
{
}

// LeaveGameCommand Execute implementation
void BaseGameplayState::LeaveGameCommand::Execute()
{
    // Transition to HighScoreState when leaving the game
    m_Game->ChangeState(std::make_shared<HighScoreState>());
}

void BaseGameplayState::Enter(Game* game)
{
    // Common initialization
    std::cout << "[\033[33mDebug\033[0m] Starting BaseGameplayState::Enter\n";

    // Clear input bindings
    dae::InputManager::GetInstance().ClearAllBindings();

    // Create scene
    auto sceneName = GetSceneName();
    dae::SceneManager::GetInstance().CreateScene(sceneName);
    auto& scene = dae::SceneManager::GetInstance().GetScene(sceneName);

    // Play background music
    PlayBackgroundMusic();

    // Setup common UI elements
    SetupCommonUI(scene);

    // Let derived class setup player(s)
    SetupPlayers(scene);

    // Setup state transition commands
    auto& inputManager = dae::InputManager::GetInstance();
    inputManager.BindCommand(SDLK_l, dae::InputState::Down, std::make_shared<LeaveGameCommand>(game));

    std::cout << "[\033[33mDebug\033[0m] BaseGameplayState::Enter completed\n";
}

void BaseGameplayState::Update(Game* /*game*/, float deltaTime)
{
    // Update scene
    auto& sceneManager = dae::SceneManager::GetInstance();
    sceneManager.Update(deltaTime);
}

void BaseGameplayState::Render(Game* /*game*/)
{
    // Scene rendering is handled by the engine
    // No additional rendering needed here
}

void BaseGameplayState::Exit(Game* /*game*/)
{
    std::cout << "[\033[33mDebug\033[0m] BaseGameplayState::Exit\n";

    // Stop background music
    StopBackgroundMusic();

    // Scene cleanup will be handled by SceneManager
}

void BaseGameplayState::SetupCommonUI(dae::Scene& scene)
{
    // Add background, logo, FPS counter, instructions
    // This code comes from your current Load() function but only includes
    // the elements common to all game modes

    // Get screen dimensions
    float screenWidth = 1024;
    float screenHeight = 580;

    // Add background
    auto background = std::make_shared<dae::GameObject>();
    auto textureComponent = background->AddComponent<dae::TextureComponent>();
    textureComponent->SetTexture("background.tga");
    textureComponent->SetSize(screenWidth, screenHeight);
    auto transformComponent = background->AddComponent<dae::TransformComponent>();
    transformComponent->SetPosition(0, 0);
    background->AddComponent<dae::RenderComponent>();
    scene.Add(background);

    // Add logo and other common UI elements...
    // ...
}

void BaseGameplayState::PlayBackgroundMusic()
{
    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        soundService->LoadMusic("Sound/MainTheme.mp3");
        soundService->PlayMusic("Sound/MainTheme.mp3", 32, true);
    }
    catch (const std::exception& e)
    {
        std::cout << "[\033[31mERROR\033[0m] Failed to play background music: " << e.what() << "\n";
    }
}

void BaseGameplayState::StopBackgroundMusic()
{
    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        soundService->StopMusic();
    }
    catch (const std::exception& e)
    {
        std::cout << "[\033[31mERROR\033[0m] Failed to stop background music: " << e.what() << "\n";
    }
}