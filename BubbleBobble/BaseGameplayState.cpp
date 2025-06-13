// BaseGameplayState.cpp

#include <iostream>

#include "Game.h"
#include "Scene.h"

#include "GameObject.h"

#include "BaseGameplayState.h"
#include "HighScoreState.h"

#include "InputManager.h"
#include "ServiceLocator.h"
#include "SoundService.h"
#include "SceneManager.h" 

#include "RenderComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"


BaseGameplayState::LeaveGameCommand::LeaveGameCommand(Game* gamePtr)
    : m_Game(gamePtr)
{
}

void BaseGameplayState::LeaveGameCommand::Execute()
{
    m_Game->ChangeState(std::make_shared<HighScoreState>());
}

void BaseGameplayState::Enter(Game* game)
{
    std::cout << "[\\033[33mDebug\\033[0m] Starting BaseGameplayState::Enter\\n";
    dae::InputManager::GetInstance().ClearAllBindings();

    auto sceneName = GetSceneName();
    dae::SceneManager::GetInstance().CreateScene(sceneName);
    auto& scene = dae::SceneManager::GetInstance().GetScene(sceneName);

    PlayBackgroundMusic();
    SetupCommonUI(scene);
    SetupPlayers(scene);

    auto& inputManager = dae::InputManager::GetInstance();
    inputManager.BindCommand(SDLK_l, dae::InputState::Down, std::make_shared<LeaveGameCommand>(game));

    std::cout << "[\\033[33mDebug\\033[0m] BaseGameplayState::Enter completed\\n";
}

void BaseGameplayState::Update(Game* /*game*/, float deltaTime)
{
    auto& sceneManager = dae::SceneManager::GetInstance();
    sceneManager.Update(deltaTime);
}

void BaseGameplayState::Render(Game* /*game*/)
{
}

void BaseGameplayState::Exit(Game* /*game*/)
{
    std::cout << "[\\033[33mDebug\\033[0m] BaseGameplayState::Exit\\n";
    StopBackgroundMusic();
    // If derived states need to clean up their specific scene:
    // dae::SceneManager::GetInstance().DestroyScene(GetSceneName()); 
}

void BaseGameplayState::SetupCommonUI(dae::Scene& scene)
{
    auto background = std::make_shared<dae::GameObject>();
    auto textureComponent = background->AddComponent<dae::TextureComponent>();
    textureComponent->SetTexture("Branding/background.tga");
    textureComponent->SetRenderSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    //auto transformComponent = background->AddComponent<dae::TransformComponent>();
    background->GetTransform()->SetPosition(0, 0);
    background->AddComponent<dae::RenderComponent>();
    scene.Add(background);
}

void BaseGameplayState::PlayBackgroundMusic()
{
    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        if (soundService)
        {
            soundService->LoadMusic("Sound/MainTheme.mp3");
            soundService->PlayMusic("Sound/MainTheme.mp3", 32, true);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[\\033[31mERROR\\033[0m] Failed to play background music: " << e.what() << "\\n"; // Corrected
    }
}

void BaseGameplayState::StopBackgroundMusic()
{
    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        if (soundService)
        {
            soundService->StopMusic();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[\\033[31mERROR\\033[0m] Failed to stop background music: " << e.what() << "\\n"; // Corrected
    }
}