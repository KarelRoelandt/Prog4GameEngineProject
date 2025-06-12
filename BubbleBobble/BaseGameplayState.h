#pragma once

#include "GameState.h"
#include "Minigin.h"
#include "SceneManager.h"
#include "Command.h" // Include the Command definition

// Forward declaration to avoid incomplete type errors
class Game;

class BaseGameplayState : public GameState
{
public:
    BaseGameplayState() = default;
    virtual ~BaseGameplayState() = default;

    // Common implementation of GameState methods
    void Enter(Game* game) override;
    void Update(Game* game, float deltaTime) override;
    void Render(Game* game) override;
    void Exit(Game* game) override;

protected:
    // Common functionality
    void SetupCommonUI(dae::Scene& scene);
    void PlayBackgroundMusic();
    void StopBackgroundMusic();

    // Pure virtual function that derived states must implement
    virtual void SetupPlayers(dae::Scene& scene) = 0;

    // Common scene name - could be customized per derived class if needed
    virtual std::string GetSceneName() const { return "Gameplay"; }

    // Implementation of LeaveGameCommand as a nested class
    class LeaveGameCommand : public dae::Command
    {
    public:
        explicit LeaveGameCommand(Game* gamePtr);
        void Execute() override;

    private:
        Game* m_Game;
    };
};