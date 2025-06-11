#include "HighScoreState.h"
#include "StartMenuState.h" // Include it here to avoid circular dependency

void HighScoreState::Enter(Game* /*game*/)
{
    std::cout << "Entering High Score State\n";
    // Load high score assets here
}

void HighScoreState::Update(Game* game, float deltaTime)
{
	
    std::cout << "Updating High Score State\n";
    std::cout << "Press 'M' to return to the main menu...\n";
    char input;
    std::cin >> input;
    if (input == 'M' || input == 'm')
    {
        game->ChangeState(std::make_shared<StartMenuState>());
    }
    game->Update(deltaTime);
}

void HighScoreState::Render(Game* game)
{
	game->Render();
    std::cout << "Rendering High Score Screen\n";
    // Render high score UI here
}

void HighScoreState::Exit(Game* /*game*/)
{
    std::cout << "Exiting High Score State\n";
    // Clean up high score assets here
}
