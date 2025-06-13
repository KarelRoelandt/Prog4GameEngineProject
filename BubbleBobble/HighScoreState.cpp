#include <fstream>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <iostream>

#include "Game.h"
#include "Scene.h"

#include "HighScoreState.h"
#include "StartMenuState.h"

#include "SceneManager.h"
#include "InputManager.h"
#include "ServiceLocator.h"
#include "ISoundService.h" 

#include "GameObject.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "TextureComponent.h"


const std::string HighScoreState::HIGHSCORE_FILE = "Data/HighScores.txt";


void HighScoreState::Enter(Game* game)
{
    EnsureHighScoreFileExists();

    std::cout << "[\\033[33mDebug\\033[0m] Entering High Score State\\n";
    BaseGameplayState::Enter(game);

    dae::InputManager::GetInstance().ClearAllBindings();
    std::cout << "[\\033[33mDebug\\033[0m] Cleared all previous input bindings\\n";

    auto& scene = dae::SceneManager::GetInstance().CreateScene("HighScore");

    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        if (soundService)
        {
            soundService->LoadMusic("Sound/MainTheme.mp3");
            soundService->PlayMusic("Sound/MainTheme.mp3", 32, true);
            std::cout << "[\\033[33mDebug\\033[0m] Playing background music\\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[\\033[31mERROR\\033[0m] Failed to play background music: " << e.what() << std::endl;
    }

    Load(scene);

    class ReturnToMenuCommand : public dae::Command
    {
    public:
        explicit ReturnToMenuCommand(Game* gamePtr) : m_Game(gamePtr) {}
        void Execute() override
        {
            std::cout << "[\\033[33mDebug\\033[0m] Returning to main menu\\n";
            m_Game->ChangeState(std::make_shared<StartMenuState>());
        }
    private:
        Game* m_Game;
    };

    auto& inputManager = dae::InputManager::GetInstance();
    inputManager.BindCommand(SDLK_ESCAPE, dae::InputState::Down,
        std::make_shared<ReturnToMenuCommand>(game));

    std::cout << "[\\033[33mDebug\\033[0m] High Score State setup complete\\n";
}

void HighScoreState::Update(Game* /*game*/, float /*deltaTime*/)
{
}

void HighScoreState::Render(Game* /*game*/)
{
}

void HighScoreState::Exit(Game* /*game*/)
{
    std::cout << "[\\033[33mDebug\\033[0m] Exiting High Score State\\n";
    try
    {
        auto soundService = ServiceLocator::GetSoundService();
        if (soundService)
        {
            soundService->StopMusic();
            std::cout << "[\\033[33mDebug\\033[0m] Stopped background music\\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[\\033[31mERROR\\033[0m] Error stopping music: " << e.what() << "\\n";
    }
    dae::InputManager::GetInstance().ClearAllBindings();
    dae::SceneManager::GetInstance().DestroyScene("HighScore");
}

void HighScoreState::SetupPlayers(dae::Scene& /*scene*/)
{
    // No players in high score state
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
            if (ss >> entry.name >> entry.score)
            {
                scores.push_back(entry);
            }
        }
        file.close();
        std::cout << "[\\033[33mDebug\\033[0m] Loaded " << scores.size() << " high scores\\n";
    }
    else
    {
        std::cout << "[\\033[33mDebug\\033[0m] High score file not found or could not be opened, using default scores\\n";
        scores = {
            {"AAA", 10000}, {"BBB", 9000}, {"CCC", 8000},
            {"DDD", 7000}, {"EEE", 6000}
        };
    }
    return scores;
}

void HighScoreState::SaveHighScore(const std::string& name, int score)
{
    auto scores = LoadHighScores();
    scores.push_back({ name, score });
    std::sort(scores.begin(), scores.end(),
        [](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });
    if (scores.size() > MAX_SCORES)
    {
        scores.resize(MAX_SCORES);
    }
    std::ofstream file(HIGHSCORE_FILE);
    if (file.is_open())
    {
        for (const auto& entry : scores)
        {
            file << entry.name << " " << entry.score << std::endl;
        }
        file.close();
        std::cout << "[\\033[33mDebug\\033[0m] Saved high scores to file\\n";
    }
    else
    {
        std::cerr << "[\\033[31mERROR\\033[0m] Could not save high scores to file: " << HIGHSCORE_FILE << "\\n";
    }
}

void HighScoreState::Load(dae::Scene& scene)
{
    // Center X for all text
    float centerX = BaseGameplayState::SCREEN_WIDTH / 2.0f;

    // Title
    auto title = std::make_shared<dae::GameObject>();
    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Pixel_NES.otf", 36);
    title->AddComponent<dae::TextComponent>("HIGH SCORE TABLE", font);
	title->GetComponent<dae::TextComponent>()->SetColor({ 255, 255, 0, 255 });
    title->GetTransform()->SetPosition(centerX - title->GetComponent<dae::TextComponent>()->GetSize().x / 2, 100.0f);
    title->AddComponent<dae::RenderComponent>();
    scene.Add(title);

    auto scores = LoadHighScores();
    auto smallFont = dae::ResourceManager::GetInstance().LoadFont("Fonts/Pixel_NES.otf", 24);

    for (size_t i = 0; i < scores.size(); ++i)
    {
        std::stringstream scoreText;
        scoreText << (i + 1) << ". " << scores[i].name << " - " << scores[i].score;
        auto scoreObj = std::make_shared<dae::GameObject>();
        scoreObj->AddComponent<dae::TextComponent>(scoreText.str(), smallFont);
        // Center each score line
        scoreObj->GetTransform()->SetPosition(centerX - 150.0f, 180.0f + static_cast<float>(i) * 40.0f); // Adjust -150 for visual centering
        scoreObj->AddComponent<dae::RenderComponent>();
        scene.Add(scoreObj);
    }

    auto instructions = std::make_shared<dae::GameObject>();
    instructions->AddComponent<dae::TextComponent>("Press ESC to return to main menu", smallFont);
    instructions->GetTransform()->SetPosition(centerX - 250.0f, 450.0f); // Adjust -250 for visual centering
    instructions->AddComponent<dae::RenderComponent>();
    scene.Add(instructions);
}

void HighScoreState::EnsureHighScoreFileExists()
{
    std::ifstream checkFile(HIGHSCORE_FILE);
    if (!checkFile.is_open())
    {
        std::cout << "[\\033[33mDebug\\033[0m] High score file does not exist or cannot be opened. Creating it now.\\n";
        std::filesystem::path filePath(HIGHSCORE_FILE);
        if (filePath.has_parent_path())
        {
            try
            {
                if (!std::filesystem::exists(filePath.parent_path()))
                {
                    std::filesystem::create_directories(filePath.parent_path());
                    std::cout << "[\\033[33mDebug\\033[0m] Created directory: " << filePath.parent_path().string() << "\\n";
                }
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                std::cerr << "[\\033[31mERROR\\033[0m] Filesystem error creating directory: " << e.what() << "\\n";
            }
        }
        std::vector<HighScoreEntry> defaultScores = {
            {"AAA", 10000}, {"BBB", 9000}, {"CCC", 8000},
            {"DDD", 7000}, {"EEE", 6000}
        };
        std::ofstream file(HIGHSCORE_FILE);
        if (file.is_open())
        {
            for (const auto& entry : defaultScores)
            {
                file << entry.name << " " << entry.score << std::endl;
            }
            file.close();
            std::cout << "[\\033[33mDebug\\033[0m] Created initial high score file: " << HIGHSCORE_FILE << "\\n";
        }
        else
        {
            std::cerr << "[\\033[31mERROR\\033[0m] Failed to create high score file: " << HIGHSCORE_FILE << "\\n";
        }
    }
    else
    {
        checkFile.close();
    }
}