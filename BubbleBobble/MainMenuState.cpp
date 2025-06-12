#pragma once

#include <filesystem>
#include <iostream>
#include "Game.h"
#include "GameState.h"
#include "HighScoreState.h"
#include "Minigin.h"
#include "SceneManager.h"
#include "ServiceLocator.h"
#include <chrono>
#include <string>
#include "FPSComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TextComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputManager.h"
#include "PlayerCharacterComponent.h"
#include "RotationComponent.h"
#include "HealthComponent.h"
#include "HealthDisplay.h"
#include "ScoreComponent.h"
#include "ScoreDisplay.h"
#include "Observer.h"
#include "SoundService.h"

// In MainMenuState.cpp
void MainMenuState::HandleMenuSelection(int selection, Game* game)
{
    switch(selection) {
        case 0: // Single Player
            game->ChangeState(std::make_shared<SinglePlayerState>());
            break;
        case 1: // Two Player Co-op
            game->ChangeState(std::make_shared<TwoPlayerCoopState>());
            break;
        case 2: // Versus Mode
            game->ChangeState(std::make_shared<VersusState>());
            break;
        // Other options...
    }
}