#include <iostream> 

#include "SinglePlayerState.h"

#include "Game.h" // Assuming Game class is used by BaseGameplayState or for context
#include "Scene.h"

#include "SceneManager.h"

#include "GameObject.h"

#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"

#include "ResourceManager.h"
#include "LevelParser.h"

// Potentially InputManager.h if any direct input handling was planned here, though usually handled by commands


void SinglePlayerState::Enter(Game* game)
{
    std::cout << "[SinglePlayerState] Enter called." << "\n";
    BaseGameplayState::Enter(game); // This should call SetupPlayers via the virtual mechanism
    // and set up the "Gameplay" scene.
    std::cout << "[SinglePlayerState] BaseGameplayState::Enter finished." << "\n";
}

void SinglePlayerState::Exit(Game* /*game*/)
{
    std::cout << "[SinglePlayerState] Exit called." << "\n";
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
    std::cout << "[SinglePlayerState::SetupPlayers] Initializing level for scene: " << scene.GetName() << "\n";

    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Pixel_NES.otf", 24);
    
    auto textPlayer1 = std::make_shared<dae::GameObject>();
    textPlayer1->AddComponent<dae::TextComponent>("Use WASD to move Bub, C to attack.", font);
    textPlayer1->GetTransform()->SetPosition(10.f, 60.f); // Example position
	textPlayer1->AddComponent<dae::RenderComponent>();
    scene.Add(textPlayer1);

    std::string baseDataPath = dae::ResourceManager::GetInstance().GetDataPath().string();
    std::cout << "[SinglePlayerState::SetupPlayers] ResourceManager::GetDataPath() returned: '" << baseDataPath << "'" << "\n";

    std::string relativeLevelPath = "Levels/1/Data.txt";
    std::string levelFilePath = baseDataPath + relativeLevelPath;

    std::cout << "[SinglePlayerState::SetupPlayers] Attempting to load level using LevelParser with dynamic path: '" << levelFilePath << "'" << "\n";

    // Load the level using LevelParser
    bool loadResult = m_LevelParser.LoadLevel(scene, levelFilePath, true);
    if (!loadResult)
    {
        std::cerr << "[SinglePlayerState::SetupPlayers] CRITICAL ERROR: LevelParser::LoadLevel failed for file: '" << levelFilePath << "'" << "\n";
    }
    else
    {
        std::cout << "[SinglePlayerState::SetupPlayers] LevelParser::LoadLevel succeeded for file: '" << levelFilePath << "'" << "\n";
    }

    std::cout << "[SinglePlayerState::SetupPlayers] Finished." << "\n";
}