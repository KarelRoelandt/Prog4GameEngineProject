#include "HighScoreState.h"
#include "StartMenuState.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "ServiceLocator.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "TextureComponent.h"

void HighScoreState::Enter(Game* game)
{
    std::cout << "[\033[33mDebug\033[0m] Entering High Score State\n";

    // Clear any existing input bindings
    dae::InputManager::GetInstance().ClearAllBindings();
    std::cout << "[\033[33mDebug\033[0m] Cleared all previous input bindings\n";

    // Create scene
    auto& scene = dae::SceneManager::GetInstance().CreateScene("HighScore");

    // Play background music using the same pattern as in GameplayState
    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        soundService->LoadMusic("Data/Sound/MainTheme.mp3");
        soundService->PlayMusic("Data/Sound/MainTheme.mp3", 32, true); // Loop the music
        std::cout << "[\033[33mDebug\033[0m] Playing background music\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "[\033[31mERROR\033[0m] Failed to play background music: " << e.what() << std::endl;
    }

    // Load scene content
    Load(scene);

    // Bind ESC key to return to main menu
    class ReturnToMenuCommand : public dae::Command
    {
    public:
        explicit ReturnToMenuCommand(Game* gamePtr) : m_Game(gamePtr) {}

        void Execute() override
        {
            std::cout << "[\033[33mDebug\033[0m] Returning to main menu\n";
            m_Game->ChangeState(std::make_shared<StartMenuState>());
        }
    private:
        Game* m_Game;
    };

    // Bind the ESC key
    auto& inputManager = dae::InputManager::GetInstance();
    inputManager.BindCommand(SDLK_ESCAPE, dae::InputState::Down,
        std::make_shared<ReturnToMenuCommand>(game));

    std::cout << "[\033[33mDebug\033[0m] High Score State setup complete\n";
}

void HighScoreState::Update(Game* /*game*/, float /*deltaTime*/)
{
    // Empty implementation - could add score animation or effects later
}

void HighScoreState::Render(Game* /*game*/)
{
    // Empty implementation - rendering handled by engine
}

void HighScoreState::Exit(Game* /*game*/)
{
    std::cout << "[\033[33mDebug\033[0m] Exiting High Score State\n";

    // Stop background music
    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        soundService->StopMusic();
        std::cout << "[\033[33mDebug\033[0m] Stopped background music\n";
    }
    catch (...)
    {
        std::cerr << "[\033[31mERROR\033[0m] Error stopping music\n";
    }

    // Clean up
    dae::InputManager::GetInstance().ClearAllBindings();
    dae::SceneManager::GetInstance().DestroyScene("HighScore");
}

void HighScoreState::Load(dae::Scene& scene)
{
    // Screen dimensions
    float screenWidth = 1024.0f;
    float screenHeight = 580.0f;

    // Add background
    auto background = std::make_shared<dae::GameObject>();
    auto textureComponent = background->AddComponent<dae::TextureComponent>();
    textureComponent->SetTexture("background.tga");
    textureComponent->SetSize(screenWidth, screenHeight);
    auto transformComponent = background->AddComponent<dae::TransformComponent>();
    transformComponent->SetPosition(0.0f, 0.0f);
    background->AddComponent<dae::RenderComponent>();
    scene.Add(background);

    // Add title
    auto title = std::make_shared<dae::GameObject>();
    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
    title->AddComponent<dae::TextComponent>("HIGH SCORES", font);
    transformComponent = title->AddComponent<dae::TransformComponent>();
    transformComponent->SetPosition(400.0f, 100.0f);
    title->AddComponent<dae::RenderComponent>();
    scene.Add(title);

    // Add placeholder high scores
    auto smallFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);

    // List of placeholder scores
    const char* scores[] = {
        "1. AAA - 10000",
        "2. BBB - 9000",
        "3. CCC - 8000",
        "4. DDD - 7000",
        "5. EEE - 6000"
    };

    // Add each score as a text component
    for (int i = 0; i < 5; ++i)
    {
        auto scoreText = std::make_shared<dae::GameObject>();
        scoreText->AddComponent<dae::TextComponent>(scores[i], smallFont);
        transformComponent = scoreText->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(400.0f, 180.0f + static_cast<float>(i) * 40.0f);
        scoreText->AddComponent<dae::RenderComponent>();
        scene.Add(scoreText);
    }

    // Add instructions to return to menu
    auto instructions = std::make_shared<dae::GameObject>();
    instructions->AddComponent<dae::TextComponent>("Press ESC to return to main menu", smallFont);
    transformComponent = instructions->AddComponent<dae::TransformComponent>();
    transformComponent->SetPosition(350.0f, 450.0f);
    instructions->AddComponent<dae::RenderComponent>();
    scene.Add(instructions);
}