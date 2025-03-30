#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Minigin.h"

#include <SDL.h>
#include <chrono>
#include <string>

#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "FPSComponent.h"

//#include "ImGuiComponent.h"

#include "RotationComponent.h"
#include "PlayerCharacterComponent.h"
#include "InputManager.h"

#include "HealthComponent.h"
#include "HealthDisplay.h"
#include "ScoreComponent.h"
#include "ScoreDisplay.h"


//#include <steam_api.h>
//#include <thread>

//#include "Achievements.h"
//#include "AchievementObserver.h"


void load()
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene("Demo");

    // Get screen dimensions
    float screenWidth = 1024; // Set your screen width here
    float screenHeight = 580; // Set your screen height here

    // Add background
    auto background = std::make_shared<dae::GameObject>();
    auto textureComponent = background->AddComponent<dae::TextureComponent>();
    textureComponent->SetTexture("background.tga");
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

int main(int, char* [])
{
    /*
    unsigned int n{ std::thread::hardware_concurrency() };
    std::cout << "Number of cores: " << n << std::endl;


    if (!SteamAPI_Init())
    {
        std::cerr << "Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed)." << std::endl;
        return 1;
    }
    else
        std::cout << "Successfully initialized steam." << std::endl;
	*/

    dae::Minigin engine("../Data/");
    engine.Initialize();

    engine.Run(load);

    // Explicitly clear the scene to release all GameObject instances
    dae::SceneManager::GetInstance().DestroyScene("Demo");

    engine.Cleanup();
    return 0;
}

