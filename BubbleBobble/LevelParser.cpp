#include "LevelParser.h"
#include <fstream>
#include <sstream>
#include <iostream> // For debug output

#include "GameObject.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "RenderComponent.h"
// #include "ColliderComponent.h" // Temporarily commented out
#include "PlayerCharacterComponent.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "HealthDisplay.h"
#include "ScoreDisplay.h"
#include "ResourceManager.h" 
#include "TextComponent.h" 

// Definition for LevelParser::LoadLevel
bool LevelParser::LoadLevel(dae::Scene& scene, const std::string& filePath, bool isSinglePlayerGameMode)
{
    std::ifstream levelFile(filePath);
    if (!levelFile.is_open())
    {
        std::cerr << "[LevelParser] Error: Could not open level file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::cout << "[LevelParser] Loading level: " << filePath << std::endl;

    m_currentPlayerNumber = 1;

    while (std::getline(levelFile, line))
    {
        if (line.empty() || line.rfind("//", 0) == 0)
        {
            continue;
        }

        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos)
        {
            std::cerr << "[LevelParser] Warning: Malformed line (no '='): " << line << std::endl;
            continue;
        }

        std::string type = line.substr(0, equalsPos);
        std::string data = line.substr(equalsPos + 1);

        if (type == "BigTile") ParseBigTile(scene, data, false);
        else if (type == "BigTileInvis") ParseBigTile(scene, data, true);
        else if (type == "Tile") ParseSmallTile(scene, data, false);
        else if (type == "FakeTile") ParseSmallTile(scene, data, true);
        else if (type == "Player") ParsePlayer(scene, data, isSinglePlayerGameMode);
        else if (type == "ZenChan") ParseZenChan(scene, data);
        else
        {
            std::cout << "[LevelParser] Unknown object type: " << type << std::endl;
        }
    }

    levelFile.close();
    std::cout << "[LevelParser] Level loading complete." << std::endl;
    return true;
}

// Definition for LevelParser::SplitString
std::vector<std::string> LevelParser::SplitString(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// Definition for LevelParser::ParseBigTile
void LevelParser::ParseBigTile(dae::Scene& scene, const std::string& lineData, bool isInvisible)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 2)
    {
        std::cerr << "[LevelParser] Error: Malformed BigTile data: " << lineData << std::endl;
        return;
    }
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);

    auto tile = std::make_shared<dae::GameObject>();
    tile->SetName(isInvisible ? "BigTileInvis" : "BigTile");

    auto transform = tile->AddComponent<dae::TransformComponent>();
    transform->SetPosition(x, y);

    if (!isInvisible)
    {
        auto texture = tile->AddComponent<dae::TextureComponent>();
        texture->SetTexture(m_BigTileTexturePath);
        tile->AddComponent<dae::RenderComponent>();
    }

    scene.Add(tile);
}

// Definition for LevelParser::ParseSmallTile
void LevelParser::ParseSmallTile(dae::Scene& scene, const std::string& lineData, bool isFake)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 2)
    {
        std::cerr << "[LevelParser] Error: Malformed SmallTile data: " << lineData << std::endl;
        return;
    }
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);

    auto tile = std::make_shared<dae::GameObject>();
    tile->SetName(isFake ? "FakeTile" : "Tile");

    auto transform = tile->AddComponent<dae::TransformComponent>();
    transform->SetPosition(x, y);

    auto texture = tile->AddComponent<dae::TextureComponent>();
    texture->SetTexture(m_SmallTileTexturePath);
    tile->AddComponent<dae::RenderComponent>();

    scene.Add(tile);
}

// Definition for LevelParser::ParsePlayer
void LevelParser::ParsePlayer(dae::Scene& scene, const std::string& lineData, bool isSinglePlayerGameMode)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 3)
    {
        std::cerr << "[LevelParser] Error: Malformed Player data: " << lineData << std::endl;
        return;
    }
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);
    bool isKeyboard = (parts[2] == "true" || parts[2] == "True");

    if (isSinglePlayerGameMode && m_currentPlayerNumber > 1)
    {
        return;
    }

    auto player = std::make_shared<dae::GameObject>();
    player->SetName("Player" + std::to_string(m_currentPlayerNumber));

    auto textureComp = player->AddComponent<dae::TextureComponent>();
    if (m_currentPlayerNumber == 1) textureComp->SetTexture("Sprites/Bub.png");
    else textureComp->SetTexture("Sprites/Bob.png");

    auto transformComp = player->AddComponent<dae::TransformComponent>();
    transformComp->SetPosition(x, y);
    player->AddComponent<dae::RenderComponent>();

    auto playerComponent = player->AddComponent<dae::PlayerCharacterComponent>(100.0f);
    playerComponent->BindInputs(isKeyboard, m_currentPlayerNumber);

    auto healthComponent = player->AddComponent<dae::HealthComponent>(player.get(), 3);
    auto scoreComponent = player->AddComponent<dae::ScoreComponent>(player.get(), 0);

    scene.Add(player);
    std::cout << "[LevelParser] Added " << player->GetName() << " at " << x << "," << y << " (Keyboard: " << isKeyboard << ")" << std::endl;

    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 16);
    if (!font)
    {
        std::cerr << "[LevelParser] Error: Could not load font for player UI." << std::endl;
    }
    else
    {
        auto healthDisplayObject = std::make_shared<dae::GameObject>();
        healthDisplayObject->SetName(player->GetName() + "HealthDisplay");
        auto healthText = healthDisplayObject->AddComponent<dae::TextComponent>("Lives: 3", font);
        //auto healthTransform = healthDisplayObject->AddComponent<dae::TransformComponent>();
        healthDisplayObject->GetTransform()->SetPosition(m_currentPlayerNumber == 1 ? 10.f : 500.f, 10.f);
        healthDisplayObject->AddComponent<dae::RenderComponent>();
        // Pass the std::shared_ptr<dae::TextComponent> directly
        auto healthDisplay = healthDisplayObject->AddComponent<dae::HealthDisplay>(healthDisplayObject.get(), healthText); // CHANGED HERE
        healthComponent->AddObserver(healthDisplay.get());
        scene.Add(healthDisplayObject);

        auto scoreDisplayObject = std::make_shared<dae::GameObject>();
        scoreDisplayObject->SetName(player->GetName() + "ScoreDisplay");
        auto scoreText = scoreDisplayObject->AddComponent<dae::TextComponent>("Score: 0", font);
        //auto scoreTransform = scoreDisplayObject->AddComponent<dae::TransformComponent>();
        scoreDisplayObject->GetTransform()->SetPosition(m_currentPlayerNumber == 1 ? 10.f : 500.f, 30.f);
        scoreDisplayObject->AddComponent<dae::RenderComponent>();
        // Pass the std::shared_ptr<dae::TextComponent> directly
        auto scoreDisplay = scoreDisplayObject->AddComponent<dae::ScoreDisplay>(scoreDisplayObject.get(), scoreText); // CHANGED HERE
        scoreComponent->AddObserver(scoreDisplay.get());
        scene.Add(scoreDisplayObject);
    }
    m_currentPlayerNumber++;
}

// Definition for LevelParser::ParseZenChan
void LevelParser::ParseZenChan(dae::Scene& scene, const std::string& lineData)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 2)
    {
        std::cerr << "[LevelParser] Error: Malformed ZenChan data: " << lineData << std::endl;
        return;
    }
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);

    auto enemy = std::make_shared<dae::GameObject>();
    enemy->SetName("ZenChan");

    auto transform = enemy->AddComponent<dae::TransformComponent>();
    transform->SetPosition(x, y);

    auto textureComp = enemy->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture("Enemies/ZenChan/Run_Anim.png");
    enemy->AddComponent<dae::RenderComponent>();

    scene.Add(enemy);
    std::cout << "[LevelParser] Added ZenChan at " << x << "," << y << std::endl;
}