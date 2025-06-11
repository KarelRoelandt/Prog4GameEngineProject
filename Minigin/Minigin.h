#pragma once
#include <string>
#include <functional>

//#include "TextComponent.h"

namespace dae
{
	class Minigin
	{
	public:
		explicit Minigin(const std::string& dataPath);
		~Minigin();

		void Initialize(const std::string& dataPath);

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;

		void Cleanup();
		void Run(const std::function<void()>& load);

		

	private:

		bool isInitialized{};

	};
}
