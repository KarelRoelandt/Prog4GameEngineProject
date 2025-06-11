#pragma once
#include <memory>

class Game;

class GameState
{
public:
    virtual ~GameState() = default;

    virtual void Enter(Game* game) = 0;  // Called when entering the state
    virtual void Update(Game* game, float deltaTime) = 0;  // Called every frame
    virtual void Render(Game* game) = 0;  // Called to render the state
    virtual void Exit(Game* game) = 0;  // Called when exiting the state
};

