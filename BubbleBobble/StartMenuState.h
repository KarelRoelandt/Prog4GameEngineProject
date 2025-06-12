#pragma once

#include "GameState.h"
#include "Game.h"
#include <iostream>
//#include "GameplayState.h"
#include "BaseGameplayState.h"
#include "SinglePlayerState.h"
#include "InputManager.h"
#include "Minigin.h"
#include "SceneManager.h"
#include "CustomDefs.h" // For dae::Vector2 if used explicitly

#include "Game.h" // Assuming Game class is used by BaseGameplayState or for context
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h" // Crucial for GetDataPath()
#include "ServiceLocator.h"

#include <filesystem>

#include "TextureComponent.h"
#include "SoundService.h"


class StartMenuState : public GameState
{
public:
    StartMenuState()
    {
        std::cout << "[\\033[33mDebug\\033[0m] StartMenuState constructor\\n";
    }

    void Enter(Game* game) override
    {
        std::cout << "[\\033[33mDebug\\033[0m] Entering Start Menu State\\n";

        dae::InputManager::GetInstance().ClearAllBindings();
        std::cout << "[\\033[33mDebug\\033[0m] Cleared all previous input bindings\\n";

        auto& scene = dae::SceneManager::GetInstance().CreateScene("StartMenu");
        std::cout << "[\\033[33mDebug\\033[0m] Created StartMenu scene\\n";

        Load(scene);

        auto& inputManager = dae::InputManager::GetInstance();

        class StartGameCommand : public dae::Command
        {
        public:
            explicit StartGameCommand(Game* gamePtr) : m_Game(gamePtr) {}

            void Execute() override
            {
                std::cout << "[\\033[33mDebug\\033[0m] StartGameCommand executed!\\n";
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
                std::cout << "[\\033[33mDebug\\033[0m] QuitCommand executed! Exiting application.\\n";
                SDL_Event quitEvent;
                quitEvent.type = SDL_QUIT;
                SDL_PushEvent(&quitEvent);
            }
        };

        auto command = std::make_shared<StartGameCommand>(game);
        inputManager.BindCommand(SDLK_p, dae::InputState::Down, command);
        inputManager.BindCommand(SDLK_l, dae::InputState::Down, std::make_shared<QuitCommand>());

        std::cout << "[\\033[33mDebug\\033[0m] Bound quit game key (L)\\n";
        std::cout << "[\\033[33mDebug\\033[0m] Bound only start menu keys (P)\\n";
    }

    void Update(Game* /*game*/, float deltaTime) override
    {
        static float debugTimer = 0.0f;
        debugTimer += deltaTime;

        if (debugTimer >= 3.0f)
        {
            std::cout << "[\\033[33mDebug\\033[0m] StartMenuState::Update called (deltaTime: " << deltaTime << ")\\n";
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            if (keystate[SDL_SCANCODE_P])
            {
                std::cout << "[\\033[33mDebug\\033[0m] P key is currently down (direct SDL check)\\n";
            }
            debugTimer = 0.0f;
        }
    }

    void Render(Game* /*game*/) override
    {
    }

    void Exit(Game* /*game*/) override
    {
        std::cout << "[\\033[33mDebug\\033[0m] Exiting Start Menu State\\n";
        try
        {
            auto soundService = ServiceLocator::GetSoundService();
            soundService->StopMusic();
        }
        catch (...)
        {
            std::cerr << "[\\033[31mERROR\\033[0m] Error stopping menu music\\n";
        }
        dae::InputManager::GetInstance().ClearAllBindings();
        std::cout << "[\\033[33mDebug\\033[0m] Cleared StartMenuState input bindings\\n";
        dae::SceneManager::GetInstance().DestroyScene("StartMenu");
    }

    static void Load(dae::Scene& scene)
    {
        float screenWidth = 1024;
        float screenHeight = 580;

        auto background = std::make_shared<dae::GameObject>();
        auto textureComponent = background->AddComponent<dae::TextureComponent>();
        textureComponent->SetTexture("Branding/background.tga");
        textureComponent->SetRenderSize(screenWidth, screenHeight); // MODIFIED
        auto transformComponent = background->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(0, 0);
        background->AddComponent<dae::RenderComponent>();
        scene.Add(background);

        auto splash = std::make_shared<dae::GameObject>();
        auto splashTextureComponent = splash->AddComponent<dae::TextureComponent>();
        splashTextureComponent->SetTexture("HUD/Logo.png");
        splashTextureComponent->SetRenderSize(screenWidth * .5f, screenHeight * .5f); // MODIFIED
        // Get the size *after* setting texture and render size if needed for positioning
        dae::Vector2 splashSize = splashTextureComponent->GetRenderDestinationSize(); // MODIFIED
        splash->GetTransform()->SetPosition(screenWidth / 2.f - splashSize.x / 2.f, screenHeight / 2.f - splashSize.y / 2.f); // MODIFIED
        splash->AddComponent<dae::RenderComponent>();
        scene.Add(splash);

        auto text = std::make_shared<dae::GameObject>();
        auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 36);
        text->AddComponent<dae::TextComponent>("BUBBLE BOBBLE - START MENU", font);
        // Re-assign transformComponent to the new GameObject's component
        transformComponent = text->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(200, 200);
        text->AddComponent<dae::RenderComponent>();
        scene.Add(text);

        auto pressKeyText = std::make_shared<dae::GameObject>();
        auto smallFont = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 24);
        pressKeyText->AddComponent<dae::TextComponent>("PRESS P TO START", smallFont);
        transformComponent = pressKeyText->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(200, 300);
        pressKeyText->AddComponent<dae::RenderComponent>();
        scene.Add(pressKeyText);

        auto quitText = std::make_shared<dae::GameObject>();
        quitText->AddComponent<dae::TextComponent>("Press L to Quit", smallFont);
        transformComponent = quitText->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(350.0f, 400.0f);
        quitText->AddComponent<dae::RenderComponent>();
        scene.Add(quitText);
    }
};
