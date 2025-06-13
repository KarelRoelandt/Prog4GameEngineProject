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

StartMenuState::StartMenuState()
{
    std::cout << "[\033[33mDebug\033[0m] StartMenuState constructor\n";
}

void StartMenuState::Enter(Game* game)
{
    std::cout << "[\033[33mDebug\033[0m] Entering Start Menu State\n";
    dae::InputManager::GetInstance().ClearAllBindings();
    std::cout << "[\033[33mDebug\033[0m] Cleared all previous input bindings\n";

    auto& scene = dae::SceneManager::GetInstance().CreateScene("StartMenu");
    std::cout << "[\033[33mDebug\033[0m] Created StartMenu scene\n";

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
    // Add background using BaseGameplayState's background logic if needed
    // Optionally call SetupCommonUI(scene) if you want the same background as gameplay

    // Add splash/logo and menu text as before
    auto splash = std::make_shared<dae::GameObject>();
    auto splashTextureComponent = splash->AddComponent<dae::TextureComponent>();
    splashTextureComponent->SetTexture("HUD/Logo.png");
    splashTextureComponent->SetRenderSize(BaseGameplayState::SCREEN_WIDTH * .5f, BaseGameplayState::SCREEN_HEIGHT * .5f);
    dae::Vector2 splashSize = splashTextureComponent->GetRenderDestinationSize();
    splash->GetTransform()->SetPosition(BaseGameplayState::SCREEN_WIDTH / 2.f - splashSize.x / 2.f, BaseGameplayState::SCREEN_HEIGHT / 2.f - splashSize.y / 2.f);
    splash->AddComponent<dae::RenderComponent>();
    scene.Add(splash);

    auto text = std::make_shared<dae::GameObject>();
    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 36);
    text->AddComponent<dae::TextComponent>("BUBBLE BOBBLE - START MENU", font);
    text->GetTransform()->SetPosition(200, 200);
    text->AddComponent<dae::RenderComponent>();
    scene.Add(text);

    auto pressKeyText = std::make_shared<dae::GameObject>();
    auto smallFont = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 24);
    pressKeyText->AddComponent<dae::TextComponent>("PRESS P TO START", smallFont);
    pressKeyText->GetTransform()->SetPosition(200, 300);
    pressKeyText->AddComponent<dae::RenderComponent>();
    scene.Add(pressKeyText);

    auto quitText = std::make_shared<dae::GameObject>();
    quitText->AddComponent<dae::TextComponent>("Press L to Quit", smallFont);
    quitText->GetTransform()->SetPosition(350.0f, 400.0f);
    quitText->AddComponent<dae::RenderComponent>();
    scene.Add(quitText);
}