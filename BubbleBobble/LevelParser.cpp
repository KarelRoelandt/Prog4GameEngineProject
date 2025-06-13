#include "LevelParser.h"

#include <fstream>
#include <sstream>
#include <iostream>    // For debug output
#include <algorithm>   // For std::min/max
#include <limits>      // For std::numeric_limits

#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h" // Still needed for GetTransform()
#include "TextureComponent.h"
#include "RenderComponent.h"
#include "PlayerCharacterComponent.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "TextComponent.h" 
#include "HealthDisplay.h"
#include "ScoreDisplay.h"
#include "ResourceManager.h"
#include "BoxCollisionComponent.h"
#include "PlayerCommands.h"

LevelParser::LevelParser()
    : m_currentPlayerNumber(1)
    , m_BigTileTexturePath("Levels/1/Big.png")
    , m_SmallTileTexturePath("Levels/1/Small.png")
    , m_Player1TexturePath("Sprites/Bub.png")
    , m_Player2TexturePath("Sprites/Bob.png")
    , m_ZenChanTexturePath("Enemies/ZenChan/Run_Anim.png")
{

    m_offsetX = (m_windowWidth - m_gameAreaWidth) / 2;          // WINDOWS % factor niet in meegerekend
    m_offsetY = (m_windowHeight - m_gameAreaHeight) / 2;

    std::cout << "[LevelParser] Initialized with screen dimensions: "
        << BaseGameplayState::SCREEN_WIDTH << "x" << BaseGameplayState::SCREEN_HEIGHT << "." << "\n";
    std::cout << "[LevelParser] Game Area Width for centering: " << m_gameAreaWidth << "\n";
    std::cout << "[LevelParser] HUD Height (m_hudHeight from .h): " << m_hudHeight << "\n";
    std::cout << "[LevelParser] Game Grid Height (for content below HUD): " << m_gameGridHeight
        << " (calculated from m_gameAreaBaseHeight: " << m_gameAreaHeight
        << " and m_hudHeight: " << m_hudHeight << "px)" << "\n";
}

void LevelParser::SetDynamicElementDimensions(float playerW, float playerH, float zenChanW, float zenChanH)
{
    m_playerWidth = playerW;
    m_playerHeight = playerH;
    m_zenChanWidth = zenChanW;
    m_zenChanHeight = zenChanH;
    std::cout << "[LevelParser] Dynamic element dimensions updated." << "\n";
}


bool LevelParser::LoadLevel(dae::Scene& scene, const std::string& filePath, bool isSinglePlayerGameMode)
{
    m_currentPlayerNumber = 1;

    std::ifstream levelFile(filePath);
    if (!levelFile.is_open())
    {
        std::cerr << "[LevelParser] Error: Could not open level file: " << filePath << "\n";
        return false;
    }
    std::string line;
    std::cout << "[LevelParser] Loading level: " << filePath << " with global offset (" << m_offsetX << "," << m_offsetY << ")" << "\n";
    while (std::getline(levelFile, line))
    {
        if (line.empty() || line.rfind("//", 0) == 0) continue;
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos)
        {
            std::cerr << "[LevelParser] Warning: Malformed line (no '='): " << line << "\n";
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
            std::cout << "[LevelParser] Unknown object type: " << type << "\n";
        }
    }
    levelFile.close();
    std::cout << "[LevelParser] Level loading complete." << "\n";
    return true;
}

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

void LevelParser::ParseBigTile(dae::Scene& scene, const std::string& lineData, bool isInvisible)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 2) { std::cerr << "[LevelParser] Error: Malformed BigTile data: " << lineData << "\n"; return; }
    float x_file = std::stof(parts[0]);
    float y_file = std::stof(parts[1]);
    auto tile = std::make_shared<dae::GameObject>();
    tile->SetName(isInvisible ? "BigTileInvis" : "BigTile");

    float worldX = x_file + m_offsetX;
    float worldY = y_file + m_offsetY;
    tile->GetTransform()->SetPosition(worldX, worldY);

    if (!isInvisible)
    {
        auto texture = tile->AddComponent<dae::TextureComponent>();
        texture->SetTexture(m_BigTileTexturePath);
        texture->SetRenderSize(m_bigTileWidth, m_bigTileHeight);
        tile->AddComponent<dae::RenderComponent>();
    }

    tile->AddComponent<dae::BoxCollisionComponent>(worldX, worldY, m_bigTileWidth, m_bigTileHeight, dae::ColliderTag::BIG_TILE);
    std::cout << "[LevelParser] Added BIG_TILE BoxCollisionComponent to " << tile->GetName() << " at (" << worldX << "," << worldY << ")" << "\n";

    scene.Add(tile);
}

void LevelParser::ParseSmallTile(dae::Scene& scene, const std::string& lineData, bool isFake)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 2) { std::cerr << "[LevelParser] Error: Malformed SmallTile data: " << lineData << "\n"; return; }
    float x_file = std::stof(parts[0]);
    float y_file = std::stof(parts[1]);
    auto tile = std::make_shared<dae::GameObject>();
    tile->SetName(isFake ? "FakeTile" : "Tile");

    float worldX = x_file + m_offsetX;
    // Uses the m_offsetY calculated from raw file Y bounds, THEN adds m_smallTileHeight
    float worldY = y_file + m_offsetY + m_smallTileHeight;
    tile->GetTransform()->SetPosition(worldX, worldY);

    auto texture = tile->AddComponent<dae::TextureComponent>(); // Fake tiles also have texture in your current setup
    texture->SetTexture(m_SmallTileTexturePath);
    texture->SetRenderSize(m_smallTileWidth, m_smallTileHeight);
    tile->AddComponent<dae::RenderComponent>();

    if (!isFake)
    {
        tile->AddComponent<dae::BoxCollisionComponent>(worldX, worldY, m_smallTileWidth, m_smallTileHeight, dae::ColliderTag::SMALL_TILE);
        std::cout << "[LevelParser] Added SMALL_TILE BoxCollisionComponent to " << tile->GetName() << " at (" << worldX << "," << worldY << ")" << "\n";
    }
    scene.Add(tile);
}

void LevelParser::ParsePlayer(dae::Scene& scene, const std::string& lineData, bool isSinglePlayerGameMode)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 3) { std::cerr << "[LevelParser] Error: Malformed Player data: " << lineData << "\n"; return; }
    float x_file = std::stof(parts[0]);
    float y_file = std::stof(parts[1]);
    bool isKeyboard = (parts[2] == "true" || parts[2] == "True");

    if (isSinglePlayerGameMode && m_currentPlayerNumber > 1) return;

    auto player = std::make_shared<dae::GameObject>();
    player->SetName("Player" + std::to_string(m_currentPlayerNumber));

    float worldX = x_file + m_offsetX;
    float worldY = y_file + m_offsetY;
    player->GetTransform()->SetPosition(worldX, worldY);

    auto textureComp = player->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture(m_currentPlayerNumber == 1 ? m_Player1TexturePath : m_Player2TexturePath);
    textureComp->SetRenderSize(m_playerWidth, m_playerHeight);
    player->AddComponent<dae::RenderComponent>();
    player->AddComponent<dae::BoxCollisionComponent>(worldX, worldY, m_playerWidth - 4, m_playerHeight - 0, dae::ColliderTag::PLAYER);
    // std::cout << "[LevelParser] Added PLAYER BoxCollisionComponent to " << player->GetName() << " at (" << worldX << "," << worldY << ")" << "\n";
    auto playerComponent = player->AddComponent<dae::PlayerCharacterComponent>(100.0f);
    playerComponent->BindInputs(isKeyboard, m_currentPlayerNumber);
    playerComponent->SetCurrentScene(&scene);
    auto healthComponent = player->AddComponent<dae::HealthComponent>(player.get(), 3);
    auto scoreComponent = player->AddComponent<dae::ScoreComponent>(player.get(), 0);
   

    scene.Add(player);

    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 16);
    if (!font) { std::cerr << "[LevelParser] Error: Could not load font for player UI." << "\n"; }
    else
    {
        float uiYPos1_lives = 10.f;
        float uiYPos2_score = 28.f;
        auto healthDisplayObject = std::make_shared<dae::GameObject>();
        healthDisplayObject->SetName(player->GetName() + "HealthDisplay");
        healthDisplayObject->GetTransform()->SetPosition(m_currentPlayerNumber == 1 ? 10.f : m_gameAreaWidth - 150.f, uiYPos1_lives);
        auto healthText = healthDisplayObject->AddComponent<dae::TextComponent>("Lives: 3", font);
        healthDisplayObject->AddComponent<dae::RenderComponent>();
        auto healthDisplay = healthDisplayObject->AddComponent<dae::HealthDisplay>(healthDisplayObject.get(), healthText);
        healthComponent->AddObserver(healthDisplay.get());
        scene.Add(healthDisplayObject);

        auto scoreDisplayObject = std::make_shared<dae::GameObject>();
        scoreDisplayObject->SetName(player->GetName() + "ScoreDisplay");
        scoreDisplayObject->GetTransform()->SetPosition(m_currentPlayerNumber == 1 ? 10.f : m_gameAreaWidth - 150.f, uiYPos2_score);
        auto scoreText = scoreDisplayObject->AddComponent<dae::TextComponent>("Score: 0", font);
        scoreDisplayObject->AddComponent<dae::RenderComponent>();
        auto scoreDisplay = scoreDisplayObject->AddComponent<dae::ScoreDisplay>(scoreDisplayObject.get(), scoreText);
        scoreComponent->AddObserver(scoreDisplay.get());
        scene.Add(scoreDisplayObject);
    }
    m_currentPlayerNumber++;
}

void LevelParser::ParseZenChan(dae::Scene& scene, const std::string& lineData)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 2) { std::cerr << "[LevelParser] Error: Malformed ZenChan data: " << lineData << "\n"; return; }
    float x_file = std::stof(parts[0]);
    float y_file = std::stof(parts[1]);
    auto enemy = std::make_shared<dae::GameObject>();
    enemy->SetName("ZenChan");

    float worldX = x_file + m_offsetX;
    float worldY = y_file + m_offsetY;
    enemy->GetTransform()->SetPosition(worldX, worldY);

    auto textureComp = enemy->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture(m_ZenChanTexturePath);
    textureComp->SetRenderSize(m_zenChanWidth, m_zenChanHeight);
    enemy->AddComponent<dae::RenderComponent>();

    
    enemy->AddComponent<dae::BoxCollisionComponent>(worldX, worldY, m_zenChanWidth, m_zenChanHeight, dae::ColliderTag::ENEMY); // Assuming you add ENEMY to ColliderTag
    // std::cout << "[LevelParser] Added ENEMY BoxCollisionComponent to " << enemy->GetName() << " at (" << worldX << "," << worldY << ")" << "\n";

    scene.Add(enemy);
}
