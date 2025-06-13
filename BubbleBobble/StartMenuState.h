#pragma once

#include "BaseGameplayState.h"
#include <memory>

class StartMenuState final : public BaseGameplayState
{
public:
    StartMenuState();
    void Enter(Game* game) override;
    void Update(Game* game, float deltaTime) override;
    void Render(Game* game) override;
    void Exit(Game* game) override;
    void SetupPlayers(dae::Scene& scene) override;
    static void Load(dae::Scene& scene);
};