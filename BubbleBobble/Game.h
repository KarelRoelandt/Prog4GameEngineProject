	#pragma once

#include <memory>
#include "GameState.h"

class Game
{
public:
    void ChangeState(std::shared_ptr<GameState> newState)
    {
        if (m_CurrentState == newState)
        {
            return; // Avoid unnecessary state changes
        }

        if (m_CurrentState)
        {
            m_CurrentState->Exit(this); // Exit the current state if it exists
        }

        m_CurrentState = newState;

        if (m_CurrentState)
        {
            m_CurrentState->Enter(this); // Enter the new state
        }
    }


    void Update(float deltaTime)
    {
        if (m_CurrentState)
        {
            (void)this; // Explicitly mark 'this' as unused
            m_CurrentState->Update(this, deltaTime);
        }
    }

    void Render()
    {
        // If you have state-specific rendering, make sure it's called:
        if (m_CurrentState)
        {
            m_CurrentState->Render(this);
        }
    }

private:
    std::shared_ptr<GameState> m_CurrentState;
};
