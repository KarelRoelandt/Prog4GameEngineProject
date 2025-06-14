#include "StartMenuState.h"
#include "Game.h"
#include "SinglePlayerState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "ServiceLocator.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TextureComponent.h"
#include "ISoundService.h"
#include "Scene.h"
#include <SDL.h>
#include <iostream>
#include <SDL_mixer.h>

StartMenuState::StartMenuState()
{
    std::cout << "[\033[33mDebug\033[0m] StartMenuState constructor\n";
}

void StartMenuState::Enter(Game* game)
{
    std::cout << "[\033[33mDebug\033[0m] Entering Start Menu State\n";
    BaseGameplayState::Enter(game);

    dae::InputManager::GetInstance().ClearAllBindings();
    std::cout << "[\033[33mDebug\033[0m] Cleared all previous input bindings\n";

    auto& scene = dae::SceneManager::GetInstance().CreateScene("StartMenu");
    std::cout << "[\033[33mDebug\033[0m] Created StartMenu scene\n";

    ServiceLocator::GetSoundService()->LoadSound("Player/Jump.wav");
    ServiceLocator::GetSoundService()->LoadSound("Player/Shoot.wav");
    ServiceLocator::GetSoundService()->LoadSound("Enemies/Death.wav");

    Load(scene);

    auto& inputManager = dae::InputManager::GetInstance();

    class StartGameCommand : public dae::Command
    {
    public:
        explicit StartGameCommand(Game* gamePtr) : m_Game(gamePtr) {}
        void Execute() override
        {
            std::cout << "[\033[33mDebug\033[0m] StartGameCommand executed!\n";
            m_Game->ChangeState(std::make_shared<SinglePlayerState>());
        }
    private:
        Game* m_Game;
    };

    class QuitCommand : public dae::Command
    {
    public:
        QuitCommand() = default;
        void Execute() override
        {
            std::cout << "[\033[33mDebug\033[0m] QuitCommand executed! Exiting application.\n";
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
        }
    };

    auto command = std::make_shared<StartGameCommand>(game);
    inputManager.BindCommand(SDLK_p, dae::InputState::Down, command);
    inputManager.BindCommand(SDLK_l, dae::InputState::Down, std::make_shared<QuitCommand>());

    std::cout << "[\033[33mDebug\033[0m] Bound quit game key (L)\n";
    std::cout << "[\033[33mDebug\033[0m] Bound only start menu keys (P)\n";
}

void StartMenuState::Update(Game*, float deltaTime)
{
    static float debugTimer = 0.0f;
    debugTimer += deltaTime;
    if (debugTimer >= 3.0f)
    {
        std::cout << "[\033[33mDebug\033[0m] StartMenuState::Update called (deltaTime: " << deltaTime << ")\n";
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_P])
        {
            std::cout << "[\033[33mDebug\033[0m] P key is currently down (direct SDL check)\n";
        }
        debugTimer = 0.0f;
    }
}

void StartMenuState::Render(Game*)
{
    // Render handled by scene manager
}

void StartMenuState::Exit(Game*)
{
    std::cout << "[\033[33mDebug\033[0m] Exiting Start Menu State\n";
    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        soundService->StopMusic();
    }
    catch (...)
    {
        std::cerr << "[\033[31mERROR\033[0m] Error stopping menu music\n";
    }
    dae::InputManager::GetInstance().ClearAllBindings();
    std::cout << "[\033[33mDebug\033[0m] Cleared StartMenuState input bindings\n";
    dae::SceneManager::GetInstance().DestroyScene("StartMenu");
}

void StartMenuState::SetupPlayers(dae::Scene&)
{
    // No players in the start menu
}

void StartMenuState::Load(dae::Scene& scene)
{
    auto font24Pixel_NES = dae::ResourceManager::GetInstance().LoadFont("Fonts/Pixel_NES.otf", 24);

    auto splash = std::make_shared<dae::GameObject>();
    splash->AddComponent<dae::TextureComponent>()->SetTexture("HUD/Logo.png");
    dae::Vector2 splashSize = splash->GetComponent<dae::TextureComponent>()->GetRenderDestinationSize();
    splash->GetTransform()->SetPosition(BaseGameplayState::SCREEN_WIDTH / 2.f - splashSize.x / 2.f, BaseGameplayState::
																SCREEN_HEIGHT / 2.f - splashSize.y / 2.f);
    splash->AddComponent<dae::RenderComponent>();
    scene.Add(splash);

    auto pressKeyText = std::make_shared<dae::GameObject>();
    pressKeyText->AddComponent<dae::TextComponent>("Press P To Start", font24Pixel_NES);
    dae::Vector2 pressKeyTextSize = pressKeyText->GetComponent<dae::TextComponent>()->GetSize();
    pressKeyText->GetTransform()->SetPosition(BaseGameplayState::SCREEN_WIDTH / 2.f - pressKeyTextSize.x / 2.f, BaseGameplayState::
																	SCREEN_HEIGHT - 200);
    pressKeyText->AddComponent<dae::RenderComponent>();
    scene.Add(pressKeyText);

    auto quitText = std::make_shared<dae::GameObject>();
    quitText->AddComponent<dae::TextComponent>("Press L to Quit", font24Pixel_NES);
    dae::Vector2 quitTextSize = quitText->GetComponent<dae::TextComponent>()->GetSize();
    quitText->GetTransform()->SetPosition(BaseGameplayState::SCREEN_WIDTH / 2.f - quitTextSize.x / 2.f, BaseGameplayState::
																	SCREEN_HEIGHT - 170);
    quitText->GetComponent<dae::TextComponent>()->SetColor(SDL_Color(255,0,0,255));
    quitText->AddComponent<dae::RenderComponent>();
    scene.Add(quitText);

}