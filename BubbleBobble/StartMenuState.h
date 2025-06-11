#pragma once

#include "GameState.h"
#include "Game.h"
#include <iostream>
#include "GameplayState.h"

class StartMenuState : public GameState
{
public:
    StartMenuState() = default; // Add a default constructor

    void Enter(Game* /*game*/) override
    {
        std::cout << "Entering Start Menu State\n";
        // Load start menu assets here
    }

    void Update(Game* game, float deltaTime) override
    {
        
        std::cout << "Updating Start Menu State\n";
        std::cout << "Press 'P' to start the game...\n";
        char input;
        std::cin >> input;
        if (input == 'P' || input == 'p')
        {
        	game->ChangeState(std::make_shared<GameplayState>());
        }
        game->Update(deltaTime);
    }

    void Render(Game* game) override
    {
        game->Render(); // Explicitly mark the parameter as unused
        std::cout << "Rendering Start Menu Screen\n";
        // Render start menu UI here
    }

    void Exit(Game* /*game*/) override
    {
        std::cout << "Exiting Start Menu State\n";
        // Clean up start menu assets here
    }
};
