// Minigin/StateMachineComponent.h
#pragma once
#include <memory>
#include "BaseComponent.h"

namespace dae
{

    class IState
    {
    public:
        virtual ~IState() = default;
        virtual void Enter(dae::GameObject* owner) = 0;
        virtual void Update(dae::GameObject* owner, float deltaTime) = 0;
        virtual void Exit(dae::GameObject* owner) = 0;
    };

    class StateMachineComponent : public BaseComponent
    {
    public:
        using BaseComponent::BaseComponent; // Inherit constructor

        void Update(float deltaTime) override
        {
            if (m_CurrentState)
                m_CurrentState->Update(GetOwner(), deltaTime);
        }

        void Render() const override
        {
            // State machines typically do not render anything.
        }

        void ChangeState(std::unique_ptr<IState> newState)
        {
            if (m_CurrentState)
                m_CurrentState->Exit(GetOwner());
            m_CurrentState = std::move(newState);
            if (m_CurrentState)
                m_CurrentState->Enter(GetOwner());
        }

        IState* GetCurrentState() const
        {
            return m_CurrentState.get();
        }

    private:
        std::unique_ptr<IState> m_CurrentState;
    };

} // namespace dae