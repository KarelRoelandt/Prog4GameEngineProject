#include <stdexcept>
#include <chrono>
#include <thread>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Minigin.h"

#include <iostream>

#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h" // Include the header file for dae::Scene

SDL_Window* g_window{};

struct State
{
    // Add relevant state variables here
    // For example:
    double position;
    double velocity;

    // Interpolation function
    State Interpolate(const State& other, double alpha) const
	{
        State result;
        result.position = position * (1.0 - alpha) + other.position * alpha;
        result.velocity = velocity * (1.0 - alpha) + other.velocity * alpha;
        return result;
    }
};

void PrintSDLVersion()
{
    SDL_version version{};
    SDL_VERSION(&version);
    printf("We compiled against SDL version %u.%u.%u ...\n",
        version.major, version.minor, version.patch);

    SDL_GetVersion(&version);
    printf("We are linking against SDL version %u.%u.%u.\n",
        version.major, version.minor, version.patch);

    SDL_IMAGE_VERSION(&version);
    printf("We compiled against SDL_image version %u.%u.%u ...\n",
        version.major, version.minor, version.patch);

    version = *IMG_Linked_Version();
    printf("We are linking against SDL_image version %u.%u.%u.\n",
        version.major, version.minor, version.patch);

    SDL_TTF_VERSION(&version);
    printf("We compiled against SDL_ttf version %u.%u.%u ...\n",
        version.major, version.minor, version.patch);

    version = *TTF_Linked_Version();
    printf("We are linking against SDL_ttf version %u.%u.%u.\n",
        version.major, version.minor, version.patch);
}

dae::Minigin::Minigin(const std::string& dataPath)
{
    PrintSDLVersion();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
    }

    g_window = SDL_CreateWindow(
        "Programming 4 assignment - 2DAE19 Karel Roelandt",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,
        580,
        SDL_WINDOW_OPENGL
    );
    if (g_window == nullptr)
    {
        throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
    }

    Renderer::GetInstance().Init(g_window);

    ResourceManager::GetInstance().Init(dataPath);
}

dae::Minigin::~Minigin()
{
    Renderer::GetInstance().Destroy();
    SDL_DestroyWindow(g_window);
    g_window = nullptr;
    SDL_Quit();
}

void dae::Minigin::Initialize()
{
    // Additional initialization if needed
}

void dae::Minigin::Cleanup()
{
    // Additional cleanup if needed
}

double getCurrentTime()
{
    using namespace std::chrono;
    return duration_cast<duration<double>>(steady_clock::now().time_since_epoch()).count();
}

void dae::Minigin::Run(const std::function<void()>& load)
{
    load();

    auto& renderer = Renderer::GetInstance();
    auto& sceneManager = SceneManager::GetInstance();
    auto& input = InputManager::GetInstance();

    const int targetFPS = 60;
    const double targetFrameTime = 1.0 / targetFPS;

    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    bool doContinue = true;
    while (doContinue)
    {
        // Calculate delta time
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> duration = currentFrameTime - lastFrameTime;
        double deltaTime = duration.count();
        lastFrameTime = currentFrameTime;

        doContinue = input.ProcessInput();

        // Update the scene using variable deltaTime
        sceneManager.Update(static_cast<float>(deltaTime));

        // Render the current state
        renderer.Render();

        // Frame rate regulation
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> frameDuration = frameEndTime - currentFrameTime;
        double frameTime = frameDuration.count();
        if (frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(targetFrameTime - frameTime));
        }
    }
}
