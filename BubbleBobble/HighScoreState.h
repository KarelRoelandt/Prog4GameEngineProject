#pragma once

#include "BaseGameplayState.h"
#include <vector>
#include <string>

struct HighScoreEntry
{
    std::string name;
    int score;
};

class HighScoreState final : public BaseGameplayState
{
public:
    static const std::string HIGHSCORE_FILE;
    static constexpr size_t MAX_SCORES = 5;

    void Enter(Game* game) override;
    void Update(Game* game, float deltaTime) override;
    void Render(Game* game) override;
    void Exit(Game* game) override;

    // Required by BaseGameplayState
    void SetupPlayers(dae::Scene& scene) override;

    static std::vector<HighScoreEntry> LoadHighScores();
    static void SaveHighScore(const std::string& name, int score);
    static void Load(class dae::Scene& scene);
    static void EnsureHighScoreFileExists();
};