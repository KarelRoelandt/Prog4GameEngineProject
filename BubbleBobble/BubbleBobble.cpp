#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Game.h"
#include "StartMenuState.h"
#include "Minigin.h"
#include "InputManager.h"

// Global pointer to the engine instance
dae::Minigin* g_Engine = nullptr;
	
int main(int, char* [])
{
    // Initialize engine once at the application level
    dae::Minigin engine("../Data/");
    engine.Initialize("");

    // Store engine reference in a globally accessible way
    g_Engine = &engine;

    Game game;

    // Start with the Start Menu State
    game.ChangeState(std::make_shared<StartMenuState>());

    // Run the engine with a simple initialization function, NOT A NESTED GAME LOOP
    engine.Run([&game]()
        {
            // Just set up any one-time initialization here
            std::cout << "[\033[32mDebug\033[0m] Engine initialization complete\n";
        });

    // Cleanup engine at the end
    engine.Cleanup();
    return 0;
}

