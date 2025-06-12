#pragma once
#include "GameState.h"
#include "Scene.h"
#include <vector>
#include <string>

struct HighScoreEntry
{
    std::string name;
    int score;
};

class HighScoreState : public GameState
{
public:
    void Enter(Game* game) override;
    void Update(Game* game, float deltaTime) override;
    void Render(Game* game) override;
    void Exit(Game* game) override;

    // Load scene content
    void Load(dae::Scene& scene);

    void EnsureHighScoreFileExists();

    // High score file operations
    static std::vector<HighScoreEntry> LoadHighScores();
    static void SaveHighScore(const std::string& name, int score);

private:
    static const std::string HIGHSCORE_FILE;
    static const int MAX_SCORES = 5;
};