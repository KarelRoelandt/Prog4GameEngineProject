#pragma once
#include <string>
#include <vector>
#include "Scene.h" // Forward declare or include if Scene is used directly

namespace dae
{
    class GameObject;
    class Scene;
}

class LevelParser
{
public:
    LevelParser() = default;

    bool LoadLevel(dae::Scene& scene, const std::string& filePath, bool isSinglePlayer);

private:
    void ParseBigTile(dae::Scene& scene, const std::string& lineData, bool isInvisible);
    void ParseSmallTile(dae::Scene& scene, const std::string& lineData, bool isFake);
    void ParsePlayer(dae::Scene& scene, const std::string& lineData, bool isSinglePlayer);
    void ParseZenChan(dae::Scene& scene, const std::string& lineData);

    std::vector<std::string> SplitString(const std::string& s, char delimiter);

    // Texture paths (relative to the "Data" directory)
    const std::string m_BigTileTexturePath = "Levels/1/Big.png";     // CHANGED: Removed "Data/" prefix
    const std::string m_SmallTileTexturePath = "Levels/1/Small.png"; // CHANGED: Removed "Data/" prefix
    // Note: Player and Enemy textures are hardcoded in their parse methods (e.g., "Sprites/Bub.png")
    // and are assumed to also be relative to the "Data" directory.

    int m_currentPlayerNumber = 1;
};