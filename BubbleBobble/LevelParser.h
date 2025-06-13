#pragma once
#include <string>
#include <vector>
#include <limits> // Required for std::numeric_limits

// Forward declaration for dae::Scene
namespace dae
{
    class Scene;
}

class LevelParser
{
public:
    LevelParser(); // Constructor

    // Loads the level from the given file path into the scene.
    bool LoadLevel(dae::Scene& scene, const std::string& filePath, bool isSinglePlayerGameMode);

    // Call this method to set the actual dimensions of dynamic game elements
    // like Player and ZenChan, if they differ from constructor defaults.
    void SetDynamicElementDimensions(float playerW, float playerH, float zenChanW, float zenChanH);

    // Setters for texture paths if they need to be configured externally
    void SetBigTileTexturePath(const std::string& path) { m_BigTileTexturePath = path; }
    void SetSmallTileTexturePath(const std::string& path) { m_SmallTileTexturePath = path; }
    void SetPlayerTexturePaths(const std::string& p1Path, const std::string& p2Path)
    {
        m_Player1TexturePath = p1Path;
        m_Player2TexturePath = p2Path;
    }
    void SetZenChanTexturePath(const std::string& path) { m_ZenChanTexturePath = path; }


private:
    // Pre-scans the level file to calculate the overall bounds of level elements
    // and determines the offset needed to center them within the defined game grid.
    void CalculateLevelOffsetAndBounds(const std::string& filePath);

    // Parsing methods for different elements
    void ParseBigTile(dae::Scene& scene, const std::string& lineData, bool isInvisible);
    void ParseSmallTile(dae::Scene& scene, const std::string& lineData, bool isFake);
    void ParsePlayer(dae::Scene& scene, const std::string& lineData, bool isSinglePlayerGameMode);
    void ParseZenChan(dae::Scene& scene, const std::string& lineData);

    // Utility to split strings
    static std::vector<std::string> SplitString(const std::string& s, char delimiter);

    int m_currentPlayerNumber; // Tracks player number for coop setup

    // Texture paths
    std::string m_BigTileTexturePath;
    std::string m_SmallTileTexturePath;
    std::string m_Player1TexturePath;
    std::string m_Player2TexturePath;
    std::string m_ZenChanTexturePath;

    // --- Dimensions for level bounds calculation ---
    // Tiles are fixed based on your plan
    const float m_bigTileWidth{ 48.0f };
    const float m_bigTileHeight{ 48.0f };
    const float m_smallTileWidth{ 24.0f };
    const float m_smallTileHeight{ 24.0f };
    // Player and ZenChan dimensions - can be updated by SetDynamicElementDimensions
    float m_playerWidth;
    float m_playerHeight;
    float m_zenChanWidth;
    float m_zenChanHeight;
    // --- End of Dimensions ---

    // Offset values to center the level
    float m_offsetX;
    float m_offsetY;

    // Game Area definitions based on your plan
    const float m_gameAreaWidth{ 960.0f };      // The width for game content centering
    const float m_gameAreaBaseHeight{ 840.0f }; // Total height of the game area including HUD
    const float m_hudHeight{ 48.0f };           // Height of the top HUD area
    const float m_gameGridHeight;             // Calculated: m_gameAreaBaseHeight - m_hudHeight
};