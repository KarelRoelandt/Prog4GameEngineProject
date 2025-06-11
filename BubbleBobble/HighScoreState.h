#pragma once

#include "GameState.h"
#include "Game.h"
#include <iostream>
#include <memory>

// Forward declare StartMenuState instead of including it
class StartMenuState;

class HighScoreState : public GameState
{
public:
    void Enter(Game* game) override;
    void Update(Game* game, float deltaTime) override;
    void Render(Game* game) override;
    void Exit(Game* game) override;
};
