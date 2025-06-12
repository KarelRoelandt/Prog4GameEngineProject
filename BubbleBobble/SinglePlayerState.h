// SinglePlayerState.h
#pragma once
#include "BaseGameplayState.h"

class SinglePlayerState : public BaseGameplayState
{
protected:
    // Override to only create Player 1
    void SetupPlayers(dae::Scene& scene) override;
    
    // Optionally override scene name
    std::string GetSceneName() const override { return "SinglePlayer"; }
};
