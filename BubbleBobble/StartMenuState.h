#pragma once

#include "GameState.h"
#include "Game.h"
#include <iostream>
#include "GameplayState.h"
#include "InputManager.h"
#include "Minigin.h"
#include "SceneManager.h"

class StartMenuState : public GameState
{
public:
    StartMenuState()
    {
        std::cout << "[\033[33mDebug\033[0m] StartMenuState constructor\n";
    }

    void Enter(Game* game) override
    {
        std::cout << "[\033[33mDebug\033[0m] Entering Start Menu State\n";

        // FIRST: Clear all existing input bindings to prevent GameplayState keys from working
        dae::InputManager::GetInstance().ClearAllBindings();
        std::cout << "[\033[33mDebug\033[0m] Cleared all previous input bindings\n";

        // Create the start menu scene and load content
        auto& scene = dae::SceneManager::GetInstance().CreateScene("StartMenu");
        std::cout << "[\033[33mDebug\033[0m] Created StartMenu scene\n";

        // Load scene content
        Load(scene);

        // Only now set up input handling for start menu state
        auto& inputManager = dae::InputManager::GetInstance();

        class StartGameCommand : public dae::Command
        {
        public:
            explicit StartGameCommand(Game* gamePtr) : m_Game(gamePtr) {}

            void Execute() override
            {
                std::cout << "[\033[33mDebug\033[0m] StartGameCommand executed!\n";
                m_Game->ChangeState(std::make_shared<GameplayState>());
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

                // Create an SDL_QUIT event that will be processed by the engine
                SDL_Event quitEvent;
                quitEvent.type = SDL_QUIT;
                SDL_PushEvent(&quitEvent);
            }
        };

        // Only bind the specific keys needed for the start menu
        auto command = std::make_shared<StartGameCommand>(game);
        inputManager.BindCommand(SDLK_p, dae::InputState::Down, command);
        inputManager.BindCommand(SDLK_l, dae::InputState::Down, std::make_shared<QuitCommand>());

        std::cout << "[\033[33mDebug\033[0m] Bound quit game key (L)\n";
        std::cout << "[\033[33mDebug\033[0m] Bound only start menu keys (P)\n";
    }

    void Update(Game* /*game*/, float deltaTime) override
    {
        // Add debug timer to detect if Update is being called
        static float debugTimer = 0.0f;
        debugTimer += deltaTime;

        if (debugTimer >= 3.0f)
        {
            std::cout << "[\033[33mDebug\033[0m] StartMenuState::Update called (deltaTime: " << deltaTime << ")\n";

            // Test manual key state check
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            if (keystate[SDL_SCANCODE_P])
            {
                std::cout << "[\033[33mDebug\033[0m] P key is currently down (direct SDL check)\n";
            }

            debugTimer = 0.0f;
        }
    }

    void Render(Game* /*game*/) override
    {
        // Empty implementation
    }

    void Exit(Game* /*game*/) override
    {
        std::cout << "[\033[33mDebug\033[0m] Exiting Start Menu State\n";

        // Stop any playing background music
        try
        {
            auto soundService = ServiceLocator::GetSoundService();
            // Try to output the sound with volume 0 to effectively stop it
            // This is a workaround if there's no direct StopSound method
            soundService->StopMusic();
        }
        catch (...)
        {
            std::cerr << "[\033[31mERROR\033[0m] Error stopping menu music\n";
        }

        // Clear input bindings specific to this state
        dae::InputManager::GetInstance().ClearAllBindings();
        std::cout << "[\033[33mDebug\033[0m] Cleared StartMenuState input bindings\n";

        // Clean up scene
        dae::SceneManager::GetInstance().DestroyScene("StartMenu");
    }

    static void Load(dae::Scene& scene)
    {
        float screenWidth = 1024;
        float screenHeight = 580;

        auto background = std::make_shared<dae::GameObject>();
        auto textureComponent = background->AddComponent<dae::TextureComponent>();
        textureComponent->SetTexture("background.tga");
        textureComponent->SetSize(screenWidth, screenHeight);
        auto transformComponent = background->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(0, 0);
        background->AddComponent<dae::RenderComponent>();
        scene.Add(background);

        auto text = std::make_shared<dae::GameObject>();
        auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
        text->AddComponent<dae::TextComponent>("BUBBLE BOBBLE - START MENU", font);
        transformComponent = text->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(200, 200);
        text->AddComponent<dae::RenderComponent>();
        scene.Add(text);

        auto pressKeyText = std::make_shared<dae::GameObject>();
        auto smallFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
        pressKeyText->AddComponent<dae::TextComponent>("PRESS P TO START", smallFont);
        transformComponent = pressKeyText->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(200, 300);
        pressKeyText->AddComponent<dae::RenderComponent>();
        scene.Add(pressKeyText);

        // Add text to inform the user about the quit option
        auto quitText = std::make_shared<dae::GameObject>();
        quitText->AddComponent<dae::TextComponent>("Press L to Quit", smallFont);
        transformComponent = quitText->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(350.0f, 400.0f);
        quitText->AddComponent<dae::RenderComponent>();
        scene.Add(quitText);
    }
};