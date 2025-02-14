#include <stdexcept>
#include <chrono>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Minigin.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"

SDL_Window* g_window{};


struct State {
	// Add relevant state variables here
	// For example:
	double position;
	double velocity;

	// Interpolation function
	State Interpolate(const State& other, double alpha) const {
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

	SDL_TTF_VERSION(&version)
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

	const double dt = 0.01; // Fixed timestep
	double currentTime = getCurrentTime();
	double accumulator = 0.0;

	State previousState{};
	State currentState{};

	bool quit = false;
	SDL_Event e;

	while (!quit) {
		double newTime = getCurrentTime();
		double frameTime = newTime - currentTime;
		if (frameTime > 0.25) 
		{
			frameTime = 0.25; // Avoid spiral of death
		}
		currentTime = newTime;

		accumulator += frameTime;

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		input.ProcessInput();

		while (accumulator >= dt) {
			previousState = currentState;
			// Integrate the current state here
			// For example:
			currentState.position += currentState.velocity * dt;
			accumulator -= dt;
		}

		const double alpha = accumulator / dt;
		State interpolatedState = currentState.Interpolate(previousState, alpha);

		// Update the scene manager
		sceneManager.Update();

		// Render using the interpolated state
		renderer.Render();
	}
}

