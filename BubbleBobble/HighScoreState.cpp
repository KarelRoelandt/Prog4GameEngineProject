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
#include <fstream>
#include <algorithm>
#include <sstream>
#include <filesystem>

// Define the high score file path
const std::string HighScoreState::HIGHSCORE_FILE = "Data/HighScores.txt";

void HighScoreState::Enter(Game* game)
{
    std::cout << "[\033[33mDebug\033[0m] Entering High Score State\n";

    EnsureHighScoreFileExists();

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

std::vector<HighScoreEntry> HighScoreState::LoadHighScores()
{
    std::vector<HighScoreEntry> scores;

    std::ifstream file(HIGHSCORE_FILE);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line) && scores.size() < MAX_SCORES)
        {
            std::stringstream ss(line);
            HighScoreEntry entry;

            // Format in file: NAME SCORE (e.g. "AAA 10000")
            if (ss >> entry.name >> entry.score)
            {
                scores.push_back(entry);
            }
        }
        file.close();
        std::cout << "[\033[33mDebug\033[0m] Loaded " << scores.size() << " high scores\n";
    }
    else
    {
        std::cout << "[\033[33mDebug\033[0m] High score file not found, using default scores\n";

        // Default scores if file can't be opened
        scores = {
            {"AAA", 10000},
            {"BBB", 9000},
            {"CCC", 8000},
            {"DDD", 7000},
            {"EEE", 6000}
        };
    }

    return scores;
}

void HighScoreState::SaveHighScore(const std::string& name, int score)
{
    // Load existing scores
    auto scores = LoadHighScores();

    // Add new score
    scores.push_back({ name, score });

    // Sort scores in descending order
    std::sort(scores.begin(), scores.end(),
        [](const HighScoreEntry& a, const HighScoreEntry& b)
        {
            return a.score > b.score;
        });

    // Keep only the top MAX_SCORES
    if (scores.size() > MAX_SCORES)
    {
        scores.resize(MAX_SCORES);
    }

    // Save back to file
    std::ofstream file(HIGHSCORE_FILE);
    if (file.is_open())
    {
        for (const auto& entry : scores)
        {
            file << entry.name << " " << entry.score << std::endl;
        }
        file.close();
        std::cout << "[\033[33mDebug\033[0m] Saved high scores to file\n";
    }
    else
    {
        std::cerr << "[\033[31mERROR\033[0m] Could not save high scores to file\n";
    }
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

    // Load high scores from file
    auto scores = LoadHighScores();

    // Add high scores from file
    auto smallFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);

    for (size_t i = 0; i < scores.size(); ++i)
    {
        // Create formatted score text (e.g. "1. AAA - 10000")
        std::stringstream scoreText;
        scoreText << (i + 1) << ". " << scores[i].name << " - " << scores[i].score;

        auto scoreObj = std::make_shared<dae::GameObject>();
        scoreObj->AddComponent<dae::TextComponent>(scoreText.str(), smallFont);
        transformComponent = scoreObj->AddComponent<dae::TransformComponent>();
        transformComponent->SetPosition(400.0f, 180.0f + static_cast<float>(i) * 40.0f);
        scoreObj->AddComponent<dae::RenderComponent>();
        scene.Add(scoreObj);
    }

    // Add instructions to return to menu
    auto instructions = std::make_shared<dae::GameObject>();
    instructions->AddComponent<dae::TextComponent>("Press ESC to return to main menu", smallFont);
    transformComponent = instructions->AddComponent<dae::TransformComponent>();
    transformComponent->SetPosition(350.0f, 450.0f);
    instructions->AddComponent<dae::RenderComponent>();
    scene.Add(instructions);
}

// Add this method to HighScoreState class
void HighScoreState::EnsureHighScoreFileExists()
{
    // Check if file exists first
    std::ifstream checkFile(HIGHSCORE_FILE);
    if (!checkFile.is_open())
    {
        std::cout << "[\033[33mDebug\033[0m] Creating initial high score file\n";

        // Create the directory if it doesn't exist
        std::filesystem::path filePath(HIGHSCORE_FILE);
        std::filesystem::create_directories(filePath.parent_path());

        // Create default high scores
        std::vector<HighScoreEntry> defaultScores = {
            {"AAA", 10000},
            {"BBB", 9000},
            {"CCC", 8000},
            {"DDD", 7000},
            {"EEE", 6000}
        };

        // Save to file
        std::ofstream file(HIGHSCORE_FILE);
        if (file.is_open())
        {
            for (const auto& entry : defaultScores)
            {
                file << entry.name << " " << entry.score << std::endl;
            }
            file.close();
            std::cout << "[\033[33mDebug\033[0m] Created initial high score file\n";
        }
        else
        {
            std::cerr << "[\033[31mERROR\033[0m] Failed to create high score file\n";
        }
    }
    else
    {
        checkFile.close();
    }
}