// VersusState.h
#pragma once
#include "BaseGameplayState.h"

class VersusState : public BaseGameplayState
{
protected:
    void SetupPlayers(dae::Scene& scene) override;
    std::string GetSceneName() const override { return "Versus"; }
};