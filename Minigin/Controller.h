#pragma once
#include<Xinput.h>

#include "imgui_internal.h"

class Command;

class Controller
{
public:
	void handleControllerInput();

	// lmethods to bind  commands

private:
	Command* buttonX_;
	Command* buttonY_;
};

inline void Controller::handleControllerInput()
{
	if (IsPressed(BUTTON_X)) return buttonX_;


}

class Controller
{
};


