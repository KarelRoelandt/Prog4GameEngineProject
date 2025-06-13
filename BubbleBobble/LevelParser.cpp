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
#include "BaseGameplayState.h" // For SCREEN_WIDTH and SCREEN_HEIGHT

LevelParser::LevelParser()
    : m_currentPlayerNumber(1)
    , m_BigTileTexturePath("Levels/1/Big.png")
    , m_SmallTileTexturePath("Levels/1/Small.png")
    , m_Player1TexturePath("Sprites/Bub.png")
    , m_Player2TexturePath("Sprites/Bob.png")
    , m_ZenChanTexturePath("Enemies/ZenChan/Run_Anim.png")
    , m_playerWidth(48.0f)
    , m_playerHeight(48.0f)
    , m_zenChanWidth(48.0f)
    , m_zenChanHeight(48.0f)
    , m_offsetX(0.0f)
    , m_offsetY(0.0f)
    , m_gameAreaWidth(BaseGameplayState::SCREEN_WIDTH)
    , m_gameAreaBaseHeight(BaseGameplayState::SCREEN_HEIGHT)
    , m_gameGridHeight(m_gameAreaBaseHeight - m_hudHeight) // m_hudHeight from .h (48.0f)
{
    std::cout << "[LevelParser] Initialized with screen dimensions: "
        << BaseGameplayState::SCREEN_WIDTH << "x" << BaseGameplayState::SCREEN_HEIGHT << "." << "\n";
    std::cout << "[LevelParser] Game Area Width for centering: " << m_gameAreaWidth << "\n";
    std::cout << "[LevelParser] HUD Height (m_hudHeight from .h): " << m_hudHeight << "\n";
    std::cout << "[LevelParser] Game Grid Height (for content below HUD): " << m_gameGridHeight
        << " (calculated from m_gameAreaBaseHeight: " << m_gameAreaBaseHeight
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

void LevelParser::CalculateLevelOffsetAndBounds(const std::string& filePath)
{
    std::ifstream levelFile(filePath);
    if (!levelFile.is_open())
    {
        std::cerr << "[LevelParser] Error (Pre-scan): Could not open level file: " << filePath << "\n";
        m_offsetX = 0;
        m_offsetY = m_hudHeight + (m_gameGridHeight / 2.0f); // Basic fallback
        return;
    }

    float min_file_X = std::numeric_limits<float>::max();
    float max_file_X = std::numeric_limits<float>::lowest();
    float min_effective_render_Y = std::numeric_limits<float>::max();
    float max_effective_render_Y = std::numeric_limits<float>::lowest();

    bool isEmptyLevel = true;
    std::string line;

    while (std::getline(levelFile, line))
    {
        if (line.empty() || line.rfind("//", 0) == 0) continue;
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) continue;

        std::string type = line.substr(0, equalsPos);
        std::string data = line.substr(equalsPos + 1);
        auto parts = SplitString(data, ',');
        float x_coord = 0, y_coord = 0;
        float currentWidth = 0, currentHeight = 0;

        if (type == "BigTile" || type == "BigTileInvis")
        {
            if (parts.size() != 2) continue;
            x_coord = std::stof(parts[0]); y_coord = std::stof(parts[1]);
            currentWidth = m_bigTileWidth; currentHeight = m_bigTileHeight;
            isEmptyLevel = false;
        }
        else if (type == "Tile" || type == "FakeTile")
        {
            if (parts.size() != 2) continue;
            x_coord = std::stof(parts[0]); y_coord = std::stof(parts[1]);
            currentWidth = m_smallTileWidth; currentHeight = m_smallTileHeight;
            isEmptyLevel = false;
        }
        else if (type == "Player")
        {
            if (parts.size() != 3) continue;
            x_coord = std::stof(parts[0]); y_coord = std::stof(parts[1]);
            currentWidth = m_playerWidth; currentHeight = m_playerHeight;
            isEmptyLevel = false;
        }
        else if (type == "ZenChan")
        {
            if (parts.size() != 2) continue;
            x_coord = std::stof(parts[0]); y_coord = std::stof(parts[1]);
            currentWidth = m_zenChanWidth; currentHeight = m_zenChanHeight;
            isEmptyLevel = false;
        }
        else
        {
            continue;
        }

        min_file_X = std::min(min_file_X, x_coord);
        max_file_X = std::max(max_file_X, x_coord + currentWidth);

        float effective_y_render_start = y_coord;
        if (type == "Tile" || type == "FakeTile")
        {
            effective_y_render_start += m_smallTileHeight;
        }

        min_effective_render_Y = std::min(min_effective_render_Y, effective_y_render_start);
        max_effective_render_Y = std::max(max_effective_render_Y, effective_y_render_start + currentHeight);
    }
    levelFile.close();

    float level_file_width = 0;
    if (isEmptyLevel)
    {
        min_file_X = 0; max_file_X = 0;
        min_effective_render_Y = 0; max_effective_render_Y = 0;
        std::cout << "[LevelParser] Warning: Level file empty or no recognized elements for bounds." << "\n";
    }
    else
    {
        level_file_width = max_file_X - min_file_X;
    }

    float effective_level_render_height = max_effective_render_Y - min_effective_render_Y;
    if (isEmptyLevel) effective_level_render_height = 0;

    m_offsetX = (m_gameAreaWidth - level_file_width) / 2.0f - min_file_X;
    m_offsetY = m_hudHeight + (m_gameGridHeight - effective_level_render_height) / 2.0f - min_effective_render_Y;

    std::cout << "[LevelParser] --- Centering Calculation (Effective Render Bounds) ---" << "\n";
    std::cout << "[LevelParser] Screen Width (m_gameAreaWidth): " << m_gameAreaWidth << "\n";
    std::cout << "[LevelParser] Screen Height (m_gameAreaBaseHeight): " << m_gameAreaBaseHeight << "\n";
    std::cout << "[LevelParser] HUD Height (m_hudHeight): " << m_hudHeight << "\n";
    std::cout << "[LevelParser] Game Grid Display Height (m_gameGridHeight): " << m_gameGridHeight << "\n";
    std::cout << "[LevelParser] Level File Width (level_file_width): " << level_file_width << "\n";
    std::cout << "[LevelParser] Effective Level Render Height (for centering): " << effective_level_render_height << "\n";
    std::cout << "[LevelParser] Min File X (min_file_X): " << min_file_X << "\n";
    std::cout << "[LevelParser] Min Effective Render Y (min_effective_render_Y): " << min_effective_render_Y << "\n";
    std::cout << "[LevelParser] Calculated Global Offset (m_offsetX, m_offsetY): (" << m_offsetX << ", " << m_offsetY << ")" << "\n";
}

bool LevelParser::LoadLevel(dae::Scene& scene, const std::string& filePath, bool isSinglePlayerGameMode)
{
    m_currentPlayerNumber = 1;
    CalculateLevelOffsetAndBounds(filePath);
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
    tile->GetTransform()->SetPosition(x_file + m_offsetX, y_file + m_offsetY);
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
    if (parts.size() != 2) { std::cerr << "[LevelParser] Error: Malformed SmallTile data: " << lineData << "\n"; return; }
    float x_file = std::stof(parts[0]);
    float y_file = std::stof(parts[1]);
    auto tile = std::make_shared<dae::GameObject>();
    tile->SetName(isFake ? "FakeTile" : "Tile");
    // Effective render start for SmallTile is (y_file + m_smallTileHeight). Apply m_offsetY to this.
    tile->GetTransform()->SetPosition(x_file + m_offsetX, (y_file + m_smallTileHeight) + m_offsetY);
    auto texture = tile->AddComponent<dae::TextureComponent>();
    texture->SetTexture(m_SmallTileTexturePath);
    texture->SetRenderSize(m_smallTileWidth, m_smallTileHeight);
    tile->AddComponent<dae::RenderComponent>();
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
    player->GetTransform()->SetPosition(x_file + m_offsetX, y_file + m_offsetY);

    auto textureComp = player->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture(m_currentPlayerNumber == 1 ? m_Player1TexturePath : m_Player2TexturePath);
    textureComp->SetRenderSize(m_playerWidth, m_playerHeight);
    player->AddComponent<dae::RenderComponent>();
    auto playerComponent = player->AddComponent<dae::PlayerCharacterComponent>(100.0f);
    playerComponent->BindInputs(isKeyboard, m_currentPlayerNumber);
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
    enemy->GetTransform()->SetPosition(x_file + m_offsetX, y_file + m_offsetY);
    auto textureComp = enemy->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture(m_ZenChanTexturePath);
    textureComp->SetRenderSize(m_zenChanWidth, m_zenChanHeight);
    enemy->AddComponent<dae::RenderComponent>();
    scene.Add(enemy);
}