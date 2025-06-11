		#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif


#include "Game.h"
#include "StartMenuState.h"
#include "Minigin.h"

int main(int, char* [])
{
    Game game;

    // Start with the Start Menu State
    game.ChangeState(std::make_shared<StartMenuState>());

    // Main game loop
    bool isRunning = true;
    while (isRunning)
    {
        float deltaTime = 0.016f; // Example fixed delta time
        game.Update(deltaTime);
        game.Render();
    }

    return 0;
}


//int main(int, char* [])
//{
//    /*
//    unsigned int n{ std::thread::hardware_concurrency() };
//    std::cout << "Number of cores: " << n << std::endl;
//
//
//    if (!SteamAPI_Init())
//    {
//        std::cerr << "Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed)." << std::endl;
//        return 1;
//    }
//    else
//        std::cout << "Successfully initialized steam." << std::endl;
//	*/
//

//
//    engine.Run(load);
//
//    // Explicitly clear the scene to release all GameObject instances
//    dae::SceneManager::GetInstance().DestroyScene("Demo");
//
//    engine.Cleanup();
//    return 0;
//}

