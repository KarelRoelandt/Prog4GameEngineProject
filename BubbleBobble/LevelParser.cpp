#include "LevelParser.h"
#include <fstream>
#include <sstream>
#include <iostream>    // For debug output
#include <algorithm>   // For std::min/max
#include <limits>      // For std::numeric_limits

#include "GameObject.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "RenderComponent.h"
#include "PlayerCharacterComponent.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "HealthDisplay.h"
#include "ScoreDisplay.h"
#include "ResourceManager.h" 
#include "TextComponent.h" 
#include "Scene.h" // Make sure dae::Scene is fully defined

// Assuming Game.h provides Game::kWindowWidth and Game::kWindowHeight
// If not, these might need to be passed in or accessed differently.
// For now, LevelParser uses its own m_gameAreaWidth/Height for internal layout.
// #include "Game.h" 

LevelParser::LevelParser()
    : m_currentPlayerNumber(1)
    , m_BigTileTexturePath("Levels/1/Big.png")       // ** EXAMPLE - Update if needed **
    , m_SmallTileTexturePath("Levels/1/Small.png")   // ** EXAMPLE - Update if needed **
    , m_Player1TexturePath("Sprites/Bub.png")               // ** EXAMPLE - Update if needed **
    , m_Player2TexturePath("Sprites/Bob.png")               // ** EXAMPLE - Update if needed **
    , m_ZenChanTexturePath("Enemies/ZenChan/Run_Anim.png")  // ** EXAMPLE - Update if needed **
    // Default dynamic element dimensions - ** UPDATE THESE or use SetDynamicElementDimensions **
    , m_playerWidth(48.0f)        // ** PLACEHOLDER - UPDATE **
    , m_playerHeight(48.0f)       // ** PLACEHOLDER - UPDATE **
    , m_zenChanWidth(48.0f)       // ** PLACEHOLDER - UPDATE **
    , m_zenChanHeight(48.0f)      // ** PLACEHOLDER - UPDATE **
    , m_offsetX(0.0f)
    , m_offsetY(0.0f)
    , m_gameGridHeight(m_gameAreaBaseHeight - m_hudHeight) // Should be 792.0f
{
    std::cout << "[LevelParser] Initialized with specified tile sizes." << std::endl;
    std::cout << "[LevelParser] Game Grid Area for centering: " << m_gameAreaWidth << "x" << m_gameGridHeight << " (below " << m_hudHeight << "px HUD)" << std::endl;
    std::cout << "[LevelParser] ** IMPORTANT: Update Player/ZenChan dimensions using SetDynamicElementDimensions() if defaults are incorrect! **" << std::endl;
}

void LevelParser::SetDynamicElementDimensions(float playerW, float playerH, float zenChanW, float zenChanH)
{
    m_playerWidth = playerW;
    m_playerHeight = playerH;
    m_zenChanWidth = zenChanW;
    m_zenChanHeight = zenChanH;
    std::cout << "[LevelParser] Dynamic element dimensions updated: Player(" << playerW << "x" << playerH
        << "), ZenChan(" << zenChanW << "x" << zenChanH << ")" << std::endl;
}


void LevelParser::CalculateLevelOffsetAndBounds(const std::string& filePath)
{
    std::ifstream levelFile(filePath);
    if (!levelFile.is_open())
    {
        std::cerr << "[LevelParser] Error (Pre-scan): Could not open level file: " << filePath << std::endl;
        m_offsetX = 0; // Default to no offset if file can't be read
        m_offsetY = m_hudHeight; // At least position below HUD
        return;
    }

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    std::string line;
    bool isEmptyLevel = true;

    while (std::getline(levelFile, line))
    {
        if (line.empty() || line.rfind("//", 0) == 0) continue;

        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) continue;

        std::string type = line.substr(0, equalsPos);
        std::string data = line.substr(equalsPos + 1);
        auto parts = SplitString(data, ',');

        float x = 0, y = 0;
        float currentWidth = 0, currentHeight = 0;

        if (type == "BigTile" || type == "BigTileInvis")
        {
            if (parts.size() != 2) continue;
            x = std::stof(parts[0]);
            y = std::stof(parts[1]);
            currentWidth = m_bigTileWidth;
            currentHeight = m_bigTileHeight;
            isEmptyLevel = false;
        }
        else if (type == "Tile" || type == "FakeTile")
        {
            if (parts.size() != 2) continue;
            x = std::stof(parts[0]);
            y = std::stof(parts[1]);
            currentWidth = m_smallTileWidth;
            currentHeight = m_smallTileHeight;
            isEmptyLevel = false;
        }
        else if (type == "Player")
        {
            if (parts.size() != 3) continue;
            x = std::stof(parts[0]);
            y = std::stof(parts[1]);
            currentWidth = m_playerWidth;
            currentHeight = m_playerHeight;
            isEmptyLevel = false;
        }
        else if (type == "ZenChan")
        {
            if (parts.size() != 2) continue;
            x = std::stof(parts[0]);
            y = std::stof(parts[1]);
            currentWidth = m_zenChanWidth;
            currentHeight = m_zenChanHeight;
            isEmptyLevel = false;
        }
        else
        {
            continue;
        }

        minX = std::min(minX, x);
        minY = std::min(minY, y);
        maxX = std::max(maxX, x + currentWidth);
        maxY = std::max(maxY, y + currentHeight);
    }
    levelFile.close();

    if (isEmptyLevel)
    {
        minX = 0; minY = 0; maxX = 0; maxY = 0; // Avoid issues with numeric_limits if no elements
        std::cout << "[LevelParser] Warning: Level file appears empty or has no recognized elements for bounds calculation." << std::endl;
    }

    float levelWidth = (isEmptyLevel) ? 0 : (maxX - minX);
    float levelHeight = (isEmptyLevel) ? 0 : (maxY - minY);

    m_offsetX = (m_gameAreaWidth - levelWidth) / 2.0f - minX;
    m_offsetY = m_hudHeight + (m_gameGridHeight - levelHeight) / 2.0f - minY;

    std::cout << "[LevelParser] Scanned Level Elements Bounds: Min(" << minX << "," << minY << ") Max(" << maxX << "," << maxY << ")" << std::endl;
    std::cout << "[LevelParser] Scanned Level Elements Dimensions: " << levelWidth << "x" << levelHeight << std::endl;
    std::cout << "[LevelParser] Calculated Final Offset for elements (includes HUD shift): (" << m_offsetX << ", " << m_offsetY << ")" << std::endl;
}


bool LevelParser::LoadLevel(dae::Scene& scene, const std::string& filePath, bool isSinglePlayerGameMode)
{
    m_currentPlayerNumber = 1;

    CalculateLevelOffsetAndBounds(filePath);

    std::ifstream levelFile(filePath);
    if (!levelFile.is_open())
    {
        std::cerr << "[LevelParser] Error: Could not open level file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::cout << "[LevelParser] Loading level: " << filePath << " with offset (" << m_offsetX << "," << m_offsetY << ")" << std::endl;

    while (std::getline(levelFile, line))
    {
        if (line.empty() || line.rfind("//", 0) == 0) continue;

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
    if (parts.size() != 2) { /* Error */ return; }
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);

    auto tile = std::make_shared<dae::GameObject>();
    tile->SetName(isInvisible ? "BigTileInvis" : "BigTile");
    auto transform = tile->AddComponent<dae::TransformComponent>();
    transform->SetPosition(x + m_offsetX, y + m_offsetY);

    if (!isInvisible)
    {
        auto texture = tile->AddComponent<dae::TextureComponent>();
        texture->SetTexture(m_BigTileTexturePath);
        texture->SetRenderSize(m_bigTileWidth, m_bigTileHeight);
        tile->AddComponent<dae::RenderComponent>();
    }
    scene.Add(tile);
}

void LevelParser::ParseSmallTile(dae::Scene& scene, const std::string& lineData, bool isFake)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 2) { /* Error */ return; }
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);

    auto tile = std::make_shared<dae::GameObject>();
    tile->SetName(isFake ? "FakeTile" : "Tile");
    auto transform = tile->AddComponent<dae::TransformComponent>();
    transform->SetPosition(x + m_offsetX, y + m_offsetY);

    auto texture = tile->AddComponent<dae::TextureComponent>();
    texture->SetTexture(m_SmallTileTexturePath);
    texture->SetRenderSize(m_smallTileWidth, m_smallTileHeight);
    tile->AddComponent<dae::RenderComponent>();
    scene.Add(tile);
}

void LevelParser::ParsePlayer(dae::Scene& scene, const std::string& lineData, bool isSinglePlayerGameMode)
{
    auto parts = SplitString(lineData, ',');
    if (parts.size() != 3) { /* Error */ return; }
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);
    bool isKeyboard = (parts[2] == "true" || parts[2] == "True");

    if (isSinglePlayerGameMode && m_currentPlayerNumber > 1) return;

    auto player = std::make_shared<dae::GameObject>();
    player->SetName("Player" + std::to_string(m_currentPlayerNumber));

    auto textureComp = player->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture(m_currentPlayerNumber == 1 ? m_Player1TexturePath : m_Player2TexturePath);
    textureComp->SetRenderSize(m_playerWidth, m_playerHeight);

    auto transformComp = player->AddComponent<dae::TransformComponent>();
    transformComp->SetPosition(x + m_offsetX, y + m_offsetY);
    player->AddComponent<dae::RenderComponent>();

    auto playerComponent = player->AddComponent<dae::PlayerCharacterComponent>(100.0f);
    playerComponent->BindInputs(isKeyboard, m_currentPlayerNumber);

    auto healthComponent = player->AddComponent<dae::HealthComponent>(player.get(), 3);
    auto scoreComponent = player->AddComponent<dae::ScoreComponent>(player.get(), 0);

    scene.Add(player);
    // Debug output for player position
    // std::cout << "[LevelParser] Added " << player->GetName() << " at final pos (" << (x + m_offsetX) << "," << (y + m_offsetY) << ")" << std::endl;


    // --- UI Elements - Positioned within HUD area (top 48px) ---
    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 16); // Smaller font for HUD
    if (!font) { std::cerr << "[LevelParser] Error: Could not load font for player UI." << std::endl; }
    else
    {
        float uiYPos1 = 10.f; // Y position for first line of UI text
        float uiYPos2 = 28.f; // Y position for second line of UI text (16px font + 2px padding)

        // Health Display
        auto healthDisplayObject = std::make_shared<dae::GameObject>();
        healthDisplayObject->SetName(player->GetName() + "HealthDisplay");
        auto healthText = healthDisplayObject->AddComponent<dae::TextComponent>("Lives: 3", font);
        // Position P1 on left, P2 on right, within HUD area
        healthDisplayObject->GetTransform()->SetPosition(m_currentPlayerNumber == 1 ? 10.f : m_gameAreaWidth - 150.f, uiYPos1);
        healthDisplayObject->AddComponent<dae::RenderComponent>();
        auto healthDisplay = healthDisplayObject->AddComponent<dae::HealthDisplay>(healthDisplayObject.get(), healthText);
        healthComponent->AddObserver(healthDisplay.get());
        scene.Add(healthDisplayObject);

        // Score Display
        auto scoreDisplayObject = std::make_shared<dae::GameObject>();
        scoreDisplayObject->SetName(player->GetName() + "ScoreDisplay");
        auto scoreText = scoreDisplayObject->AddComponent<dae::TextComponent>("Score: 0", font);
        scoreDisplayObject->GetTransform()->SetPosition(m_currentPlayerNumber == 1 ? 10.f : m_gameAreaWidth - 150.f, uiYPos2);
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
    if (parts.size() != 2) { /* Error */ return; }
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);

    auto enemy = std::make_shared<dae::GameObject>();
    enemy->SetName("ZenChan");
    auto transform = enemy->AddComponent<dae::TransformComponent>();
    transform->SetPosition(x + m_offsetX, y + m_offsetY);

    auto textureComp = enemy->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture(m_ZenChanTexturePath);
    textureComp->SetRenderSize(m_zenChanWidth, m_zenChanHeight);
    enemy->AddComponent<dae::RenderComponent>();
    scene.Add(enemy);
}