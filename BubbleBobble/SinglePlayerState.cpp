#include "SinglePlayerState.h"
#include "Game.h" // Assuming Game class is used by BaseGameplayState or for context
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h" // Crucial for GetDataPath()
#include "LevelParser.h"     // For m_LevelParser
// Potentially InputManager.h if any direct input handling was planned here, though usually handled by commands

#include <iostream> // For std::cout and std::cerr

void SinglePlayerState::Enter(Game* game)
{
    std::cout << "[SinglePlayerState] Enter called." << std::endl;
    BaseGameplayState::Enter(game); // This should call SetupPlayers via the virtual mechanism
    // and set up the "Gameplay" scene.
    std::cout << "[SinglePlayerState] BaseGameplayState::Enter finished." << std::endl;
}

void SinglePlayerState::Exit(Game* /*game*/)
{
    std::cout << "[SinglePlayerState] Exit called." << std::endl;
    // Perform any cleanup specific to the single-player state if needed.
    // For example, if this state added specific observers or event listeners, remove them.
    // If the scene is entirely owned by this state and not managed globally for gameplay,
    // it might be cleared or reset here, but usually, SceneManager handles scene transitions.
}

void SinglePlayerState::Update(Game* /*game*/, float /*deltaTime*/)
{
    // The active scene (presumably "Gameplay" set up by BaseGameplayState)
    // will be updated by the global SceneManager in the main game loop.
    // If there's any logic specific to SinglePlayerState that needs to run every frame
    // and isn't tied to a GameObject in the scene, it would go here.
    // For now, assuming SceneManager handles the active scene's update.
    // dae::SceneManager::GetInstance().Update(deltaTime); // This is usually called from the main game loop directly
}

void SinglePlayerState::Render(Game* /*game*/)
{
    // Similar to Update, rendering is typically handled by the global SceneManager
    // rendering the active scene from the main game loop.
    // dae::SceneManager::GetInstance().Render(); // This is usually called from the main game loop directly
}

void SinglePlayerState::SetupPlayers(dae::Scene& scene)
{
    std::cout << "[SinglePlayerState::SetupPlayers] Initializing level for scene: " << scene.GetName() << std::endl;

    // 1. Load UI Font and create instruction text
    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 16);
    if (!font)
    {
        std::cerr << "[SinglePlayerState::SetupPlayers] CRITICAL ERROR: Failed to load font 'Fonts/Lingua.otf'." << std::endl;
    }
    else
    {
        auto textPlayer1 = std::make_shared<dae::GameObject>();
        textPlayer1->SetName("InstructionsP1_SP_State"); // Unique name
        textPlayer1->AddComponent<dae::TextComponent>("Use WASD to move Bub, C to attack.", font);

        // Ensure TransformComponent is added (or retrieved if already exists)
        auto transformComp = textPlayer1->GetComponent<dae::TransformComponent>();
        if (!transformComp)
        {
            transformComp = textPlayer1->AddComponent<dae::TransformComponent>();
        }
        transformComp->SetPosition(10.f, 60.f); // Example position

        // Ensure RenderComponent is added
        if (!textPlayer1->GetComponent<dae::RenderComponent>())
        {
            textPlayer1->AddComponent<dae::RenderComponent>();
        }

        scene.Add(textPlayer1);
        std::cout << "[SinglePlayerState::SetupPlayers] Added instruction text." << std::endl;
    }

    // 2. Construct the level file path using ResourceManager's data path
    std::string baseDataPath = dae::ResourceManager::GetInstance().GetDataPath().string();
    std::cout << "[SinglePlayerState::SetupPlayers] ResourceManager::GetDataPath() returned: '" << baseDataPath << "'" << std::endl;

    // Ensure baseDataPath ends with a slash if it doesn't already.
    // This is important for correct path joining.
    if (!baseDataPath.empty() && baseDataPath.back() != '/' && baseDataPath.back() != '\\')
    {
        baseDataPath += '/';
        std::cout << "[SinglePlayerState::SetupPlayers] Added trailing slash to baseDataPath. Now: '" << baseDataPath << "'" << std::endl;
    }

    std::string relativeLevelPath = "Levels/1/Data.txt"; // Path relative to the data root
    std::string levelFilePath = baseDataPath + relativeLevelPath;

    std::cout << "[SinglePlayerState::SetupPlayers] Attempting to load level using LevelParser with dynamic path: '" << levelFilePath << "'" << std::endl;

    // 3. Load the level using LevelParser
    bool loadResult = m_LevelParser.LoadLevel(scene, levelFilePath, true); // true for single-player
    if (!loadResult)
    {
        std::cerr << "[SinglePlayerState::SetupPlayers] CRITICAL ERROR: LevelParser::LoadLevel failed for file: '" << levelFilePath << "'" << std::endl;
    }
    else
    {
        std::cout << "[SinglePlayerState::SetupPlayers] LevelParser::LoadLevel succeeded for file: '" << levelFilePath << "'" << std::endl;
    }

    std::cout << "[SinglePlayerState::SetupPlayers] Finished." << std::endl;
}