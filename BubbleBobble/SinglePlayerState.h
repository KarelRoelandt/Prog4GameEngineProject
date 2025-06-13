#pragma once

#include "BaseGameplayState.h" // Your actual BaseGameplayState
#include "LevelParser.h"       // For parsing and loading level data


class SinglePlayerState final : public BaseGameplayState
{
public:
    SinglePlayerState() = default;
    ~SinglePlayerState() override = default;

    SinglePlayerState(const SinglePlayerState& other) = delete;
    SinglePlayerState(SinglePlayerState&& other) = delete;
    SinglePlayerState& operator=(const SinglePlayerState& other) = delete;
    SinglePlayerState& operator=(SinglePlayerState&& other) = delete;

    // Overriding methods from GameState (via BaseGameplayState)
    void Enter(Game* game) override;
    void Update(Game* game, float deltaTime) override;
    void Render(Game* game) override; // Assuming BaseGameplayState has Render
    void Exit(Game* game) override;

private:
    // Implementing the pure virtual method from BaseGameplayState
    void SetupPlayers(dae::Scene& scene) override;

    LevelParser m_LevelParser; // Instance of the level parser
};