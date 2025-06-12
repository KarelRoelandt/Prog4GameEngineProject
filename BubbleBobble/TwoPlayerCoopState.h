// TwoPlayerCoopState.h
#pragma once
#include "BaseGameplayState.h"

class TwoPlayerCoopState : public BaseGameplayState
{
protected:
    void SetupPlayers(dae::Scene& scene) override;
    std::string GetSceneName() const override { return "TwoPlayerCoop"; }
};