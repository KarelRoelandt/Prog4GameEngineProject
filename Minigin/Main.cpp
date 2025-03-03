#include <SDL.h>
#include <chrono>
#include <string>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "FPSComponent.h"
#include "ImGuiComponent.h"
#include "RotationComponent.h"

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


    auto bubbleAchor = std::make_shared<dae::GameObject>();
    transformComponent = bubbleAchor->AddComponent<dae::TransformComponent>();
    transformComponent->SetPosition(250, 300);
    //scene.Add(bubbleAchor);


    // Add bubble
    auto bubble = std::make_shared<dae::GameObject>();

    textureComponent = bubble->AddComponent<dae::TextureComponent>();
    textureComponent->SetTexture("Sprites/Bubble.png");

    transformComponent = bubble->AddComponent<dae::TransformComponent>();
    //transformComponent->SetPosition(250, 300);

    bubble->SetParent(bubbleAchor.get());

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

    auto gameObject = std::make_shared<dae::GameObject>();
    gameObject->SetName("MyGameObject");

    auto imguiComponent = gameObject->AddComponent<dae::ImGuiComponent>();

    scene.Add(gameObject);


}

int main(int, char* [])
{
    dae::Minigin engine("../Data/");
    engine.Initialize();

    engine.Run(load);

    // Explicitly clear the scene to release all GameObject instances
    dae::SceneManager::GetInstance().DestroyScene("Demo");

    engine.Cleanup();
    return 0;
}
