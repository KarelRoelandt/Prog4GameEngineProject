// Create the StartGameCommand using the same pattern as in PlayerCharacterComponent
class StartGameCommand : public dae::Command
{
public:
    explicit StartGameCommand(Game* gamePtr) : m_Game(gamePtr) {}

    void Execute() override
    {
        std::cout << "[\033[32mDebug\033[0m] StartGameCommand executed! Changing to GameplayState\n";
        m_Game->ChangeState(std::make_shared<GameplayState>());
    }
private:
    Game* m_Game;
};

// This is the pattern used in PlayerCharacterComponent
StartGameCommand* commandPtr = new StartGameCommand(game);
std::shared_ptr<dae::Command> sharedCommand(commandPtr, [](dae::Command*) {});
inputManager.BindCommand(SDLK_p, dae::InputState::Down, sharedCommand);