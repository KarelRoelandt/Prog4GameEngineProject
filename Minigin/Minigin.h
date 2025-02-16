#pragma once
#include <string>
#include <functional>

#include "TextComponent.h"

namespace dae
{
	class Minigin
	{
	public:
		explicit Minigin(const std::string& dataPath);
		~Minigin();

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;

		void Initialize();
		void Cleanup();
		void Run(const std::function<void()>& load);

	private:
		std::shared_ptr<TextComponent> fpsTextComponent;

	};
}
