#pragma once

#include <filesystem>
#include <iostream>
#include "Game.h"
#include "GameState.h"
#include "HighScoreState.h"
#include "Minigin.h"
#include "SceneManager.h"
#include "ServiceLocator.h"
#include <chrono>
#include <string>
#include "FPSComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TextComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputManager.h"
#include "PlayerCharacterComponent.h"
#include "RotationComponent.h"
#include "HealthComponent.h"
#include "HealthDisplay.h"
#include "ScoreComponent.h"
#include "ScoreDisplay.h"
#include "Observer.h"
#include "SoundService.h"

class GameplayState : public GameState
{
public:
    GameplayState() {}


    void Enter(Game* game) override
    {
        std::cout << "[\033[33mDebug\033[0m] Starting GameplayState::Enter\n";

        // Clear any existing input bindings
        dae::InputManager::GetInstance().ClearAllBindings();
        std::cout << "[\033[33mDebug\033[0m] Cleared all previous input bindings\n";

        // Create scene
        dae::SceneManager::GetInstance().CreateScene("Demo");

        // Play background music using EXACTLY the same pattern as the working sound effects
        try
        {
            auto soundService = ServiceLocator::GetSoundService();
            soundService->LoadMusic("Sound/MainTheme.mp3");
            soundService->PlayMusic("Sound/MainTheme.mp3", 32, true); // Loop the music
            std::cout << "[\033[33mDebug\033[0m] Playing background music\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "[\033[31mERROR\033[0m] Failed to play background music: " << e.what() << "\n";
        }

        Load();

        std::cout << "[\033[33mDebug\033[0m] GameplayState::Enter after loading scene\n";

        // Set up input handling for state transitions
        auto& inputManager = dae::InputManager::GetInstance();

        // Create a concrete command class implementation 
        class LeaveGameCommand : public dae::Command
        {
        public:
            explicit LeaveGameCommand(Game* gamePtr) : m_Game(gamePtr) {}

            void Execute() override
            {
                std::cout << "[\033[33mDebug\033[0m] LeaveGameCommand executed!\n";
                m_Game->ChangeState(std::make_shared<HighScoreState>());
            }
        private:
            Game* m_Game;
        };

        // Use the concrete implementation with SDLK_l
        inputManager.BindCommand(SDLK_l, dae::InputState::Down,
            std::make_shared<LeaveGameCommand>(game));

        std::cout << "[\033[33mDebug\033[0m] GameplayState::Enter completed\n";
    }

    // Rest of the class remains unchanged...
    void Update(Game* /*game*/, float /*deltaTime*/) override
    {
    }

    void Render(Game* /*game*/) override
    {
    }

    void Exit(Game* /*game*/) override
    {
        std::cout << "[\033[33mDebug\033[0m] Exiting Gameplay State\n";

        // Stop background music
        try
        {
            auto soundService = ServiceLocator::GetSoundService();
            soundService->StopMusic(); // Properly stop the music
        }
        catch (...)
        {
            std::cerr << "[\033[31mError\033[0m] Error stopping music\n";
        }


        // Clear input bindings specific to this state
        dae::InputManager::GetInstance().ClearAllBindings();
        std::cout << "[\033[33mDebug\033[0m] Cleared GameplayState input bindings\n";

        // Clean up gameplay scene here
        dae::SceneManager::GetInstance().DestroyScene("Demo");
    }

private:
    static void Load()
    {
        // Get the existing scene
        auto& scene = dae::SceneManager::GetInstance().GetScene("Demo");

        // Get screen dimensions
        float screenWidth = 1024; // Set your screen width here
        float screenHeight = 580; // Set your screen height here

        // Add background
        auto background = std::make_shared<dae::GameObject>();
        auto textureComponent = background->AddComponent<dae::TextureComponent>();
        textureComponent->SetTexture("Branding/background.tga");
        textureComponent->SetSize(screenWidth, screenHeight); // Set the size of the background texture
        auto transformComponent = background->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(0, 0);
        background->AddComponent<dae::RenderComponent>();
        scene.Add(background);

        // Add logo
        auto logo = std::make_shared<dae::GameObject>();
        textureComponent = logo->AddComponent<dae::TextureComponent>();
        textureComponent->SetTexture("logo.tga");
        transformComponent = logo->AddComponent<dae::TransformComponent>();
        auto logoSize = textureComponent->GetSize();
        transformComponent->SetPosition((screenWidth - logoSize.x) / 2, 180);
        logo->AddComponent<dae::RenderComponent>();
        scene.Add(logo);

        // Add text
        auto text = std::make_shared<dae::GameObject>();
        auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
        text->AddComponent<dae::TextComponent>("Programming 4 Assignment", font);
        transformComponent = text->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(272, 20);
        text->AddComponent<dae::RenderComponent>();
        scene.Add(text);

        // Add FPS text
        auto fpsText = std::make_shared<dae::GameObject>();
        fpsText->SetName("FPSText");
        auto fpsTextComponent = fpsText->AddComponent<dae::TextComponent>("0.0 FPS", font);
        fpsText->AddComponent<dae::FPSComponent>(fpsTextComponent);
        transformComponent = fpsText->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(10, 10);
        fpsText->AddComponent<dae::RenderComponent>();
        scene.Add(fpsText);


        auto bubbleAnchor = std::make_shared<dae::GameObject>();
        transformComponent = bubbleAnchor->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(250, 300);
        scene.Add(bubbleAnchor);


        // Add bubble
        auto bubble = std::make_shared<dae::GameObject>();
        bubble->SetParent(bubbleAnchor.get());
        textureComponent = bubble->AddComponent<dae::TextureComponent>();
        textureComponent->SetTexture("Sprites/Bubble.png");
        bubble->AddComponent<dae::TransformComponent>();
        bubble->AddComponent<dae::RotationComponent>(-450.0f, glm::vec2{ 0, 0 }, 40.f); // Add rotation component with speed, point, and radius
        bubble->AddComponent<dae::RenderComponent>();

        //scene.Add(bubble);



        // Add Child bubble
        auto childBubble = std::make_shared<dae::GameObject>();

        textureComponent = childBubble->AddComponent<dae::TextureComponent>();
        textureComponent->SetTexture("Sprites/Bubble.png");

        transformComponent = childBubble->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(0, 0);

        // Set parent-child relationship
        childBubble->SetParent(bubble.get());

        // Add rotation component with speed, point, and radius, using the parent's position as the center
        childBubble->AddComponent<dae::RotationComponent>(270.0f, glm::vec2{ 0, 0 }, 80.0f);

        childBubble->AddComponent<dae::RenderComponent>();

        //scene.Add(childBubble);

        // TEST adding / removing component child
        // Add Child bubble 2 to TEST removing child bubble
        auto childBubble2 = std::make_shared<dae::GameObject>();

        textureComponent = childBubble2->AddComponent<dae::TextureComponent>();
        textureComponent->SetTexture("Sprites/Bubble.png");

        transformComponent = childBubble2->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(0, 0);

        // Set parent-child relationship
        childBubble2->SetParent(bubble.get());
        childBubble2->SetParent(childBubble.get());
        //childBubble2->SetParent(nullptr);

        // Add rotation component with speed, point, and radius, using the parent's position as the center
        childBubble2->AddComponent<dae::RotationComponent>(180.0f, glm::vec2{ 0, 0 }, 80.0f);

        childBubble2->AddComponent<dae::RenderComponent>();

        // TEST removing component
        childBubble2->RemoveComponent<dae::RotationComponent>();

        scene.Add(childBubble2);

        // TEST removing child bubble
        scene.Remove(childBubble2);


        /*
        auto ImGuiObject = std::make_shared<dae::GameObject>();
        ImGuiObject->SetName("ImGuiObject");

        auto imguiComponent = ImGuiObject->AddComponent<dae::ImGuiComponent>();

        scene.Add(ImGuiObject);
        */

        // Add text
        auto fontPlayer = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 16);

        auto textPlayer1 = std::make_shared<dae::GameObject>();
        textPlayer1->AddComponent<dae::TextComponent>("Use WASD to move Bub (green), C to do damage, Z and X to add score.", fontPlayer);
        transformComponent = textPlayer1->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(10, 100);
        textPlayer1->AddComponent<dae::RenderComponent>();
        scene.Add(textPlayer1);

        auto textPlayer2 = std::make_shared<dae::GameObject>();
        textPlayer2->AddComponent<dae::TextComponent>("Use D-Pad to move Bob (blue), X to do damage, A and B to add score.", fontPlayer);
        transformComponent = textPlayer2->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(10, 120);
        textPlayer2->AddComponent<dae::RenderComponent>();
        scene.Add(textPlayer2);


        // Create player1 (keyboard-based)
        auto player1 = std::make_shared<dae::GameObject>();
        player1->SetName("Player1");
        // Add required components
        auto textureComp = player1->AddComponent<dae::TextureComponent>();
        textureComp->SetTexture("Sprites/Bub.png");
        auto transformComp = player1->AddComponent<dae::TransformComponent>();
        transformComp->SetPosition(200, 300);
        player1->AddComponent<dae::RenderComponent>();
        // Add the PlayerCharacterComponent
        auto player1Component = player1->AddComponent<dae::PlayerCharacterComponent>(100.0f);
        player1Component->BindInputs(true); // true = keyboard controls
        // Add the HealthComponent
        auto healthComponent = player1->AddComponent<dae::HealthComponent>(player1.get(), 3);
        auto scoreComponent = player1->AddComponent<dae::ScoreComponent>(player1.get(), 0);
        scene.Add(player1);

        // Create HealthDisplay GameObject
        auto healthDisplayObject1 = std::make_shared<dae::GameObject>();
        auto textComponent = healthDisplayObject1->AddComponent<dae::TextComponent>("# lives: 3", fontPlayer);
        auto textTransform = healthDisplayObject1->AddComponent<dae::TransformComponent>();
        textTransform->SetPosition(10, 150);
        healthDisplayObject1->AddComponent<dae::RenderComponent>();
        // Add HealthDisplay (Observer) as a component
        auto healthDisplay = healthDisplayObject1->AddComponent<dae::HealthDisplay>(healthDisplayObject1.get(), textComponent);
        healthComponent->AddObserver(healthDisplay.get());  // Pass the raw pointer
        scene.Add(healthDisplayObject1);

        // Create ScoreDisplay GameObject
        auto scoreDisplayObject1 = std::make_shared<dae::GameObject>();
        auto scoreTextComponent = scoreDisplayObject1->AddComponent<dae::TextComponent>("Score: 0", fontPlayer);
        auto scoreTextTransform = scoreDisplayObject1->AddComponent<dae::TransformComponent>();
        scoreTextTransform->SetPosition(10, 170); // Position it appropriately
        scoreDisplayObject1->AddComponent<dae::RenderComponent>();
        // Add ScoreDisplay (Observer) as a component
        auto scoreDisplay = scoreDisplayObject1->AddComponent<dae::ScoreDisplay>(scoreDisplayObject1.get(), scoreTextComponent);
        scoreComponent->AddObserver(scoreDisplay.get());  // Pass the raw pointer
        scene.Add(scoreDisplayObject1);


        // Create player2 (controller-based)
        auto player2 = std::make_shared<dae::GameObject>();
        player2->SetName("Player2");
        // Add required components
        auto textureComp2 = player2->AddComponent<dae::TextureComponent>();
        textureComp2->SetTexture("Sprites/Bob.png");
        auto transformComp2 = player2->AddComponent<dae::TransformComponent>();
        transformComp2->SetPosition(250, 300);
        player2->AddComponent<dae::RenderComponent>();
        // Add the PlayerCharacterComponent
        auto player2Component = player2->AddComponent<dae::PlayerCharacterComponent>(200.0f);
        player2Component->BindInputs(false, 0); // false = controller controls, 0 = first controller
        // Add the HealthComponent for player2
        auto healthComponent2 = player2->AddComponent<dae::HealthComponent>(player2.get(), 3);
        // Add the ScoreComponent for player2
        auto scoreComponent2 = player2->AddComponent<dae::ScoreComponent>(player2.get(), 0);
        scene.Add(player2);

        // Create HealthDisplay GameObject for player2
        auto healthDisplayObject2 = std::make_shared<dae::GameObject>();
        auto textComponent2 = healthDisplayObject2->AddComponent<dae::TextComponent>("# lives: 3", fontPlayer);
        auto textTransform2 = healthDisplayObject2->AddComponent<dae::TransformComponent>();
        textTransform2->SetPosition(10, 200); // Adjusted position for player2's health
        healthDisplayObject2->AddComponent<dae::RenderComponent>();
        // Add HealthDisplay (Observer) as a component
        auto healthDisplay2 = healthDisplayObject2->AddComponent<dae::HealthDisplay>(healthDisplayObject2.get(), textComponent2);
        healthComponent2->AddObserver(healthDisplay2.get());  // Pass the raw pointer
        scene.Add(healthDisplayObject2);

        // Create ScoreDisplay GameObject for player2
        auto scoreDisplayObject2 = std::make_shared<dae::GameObject>();
        auto scoreTextComponent2 = scoreDisplayObject2->AddComponent<dae::TextComponent>("Score: 0", fontPlayer);
        auto scoreTextTransform2 = scoreDisplayObject2->AddComponent<dae::TransformComponent>();
        scoreTextTransform2->SetPosition(10, 220); // Adjusted position for player2's score
        scoreDisplayObject2->AddComponent<dae::RenderComponent>();
        // Add ScoreDisplay (Observer) as a component
        auto scoreDisplay2 = scoreDisplayObject2->AddComponent<dae::ScoreDisplay>(scoreDisplayObject2.get(), scoreTextComponent2);
        scoreComponent2->AddObserver(scoreDisplay2.get());  // Pass the raw pointer
        scene.Add(scoreDisplayObject2);

    }


};
