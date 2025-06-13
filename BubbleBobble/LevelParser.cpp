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
#include "PlayerRunState.h"
#include "ScoreDisplay.h"
#include "ResourceManager.h"
#include "StateMachineComponent.h"

#include "PlayerIdleState.h"


LevelParser::LevelParser()
    : m_currentPlayerNumber(1)
    , m_BigTileTexturePath("Levels/1/Big.png")
    , m_SmallTileTexturePath("Levels/1/Small.png")
    , m_Player1TexturePath("Player/Bubby/Run_Anim.png")
    , m_Player2TexturePath("Player/Bobby/Run_Anim.png")
    , m_ZenChanTexturePath("Enemies/ZenChan/Run_Anim.png")
{
    m_offsetX = (m_windowWidth - m_gameAreaWidth) / 2;          // WINDOWS % factor niet in meegerekend
	m_offsetY = (m_windowHeight - m_gameAreaHeight) / 2;
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
    // Uses the m_offsetY calculated from raw file Y bounds
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
    // Uses the m_offsetY calculated from raw file Y bounds, THEN adds m_smallTileHeight
    tile->GetTransform()->SetPosition(x_file + m_offsetX, y_file + m_offsetY + m_smallTileHeight);
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
    auto textureComp = player->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture(m_currentPlayerNumber == 1 ? m_Player1TexturePath : m_Player2TexturePath);
    textureComp->SetRenderSize(m_playerWidth, m_playerHeight);

    player->GetTransform()->SetPosition(x_file + m_offsetX, y_file + m_offsetY + 24);

    player->AddComponent<dae::RenderComponent>();

    player->AddComponent<dae::PlayerCharacterComponent>(100.0f)->BindInputs(isKeyboard, m_currentPlayerNumber);
    
    auto healthComponent = player->AddComponent<dae::HealthComponent>(player.get(), 3);
    auto scoreComponent = player->AddComponent<dae::ScoreComponent>(player.get(), 0);

    auto stateMachine = player->AddComponent<dae::StateMachineComponent>();
    stateMachine->ChangeState(std::make_unique<PlayerIdleState>());

    scene.Add(player);


    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 24);

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
    
    enemy->GetTransform()->SetPosition(x_file + m_offsetX, y_file + m_offsetY + 96 );

    auto textureComp = enemy->AddComponent<dae::TextureComponent>();
    textureComp->SetTexture(m_ZenChanTexturePath);
    textureComp->SetRenderSize(m_zenChanWidth, m_zenChanHeight);
    enemy->AddComponent<dae::RenderComponent>();
    scene.Add(enemy);
}