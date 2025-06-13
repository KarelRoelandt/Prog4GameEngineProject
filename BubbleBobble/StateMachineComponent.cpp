#include "StateMachineComponent.h"

namespace dae
{
    StateMachineComponent::StateMachineComponent(GameObject* owner)
        : BaseComponent(owner)
        , m_CurrentState(nullptr)
    {
        // Optionally set default state here if needed
        // m_CurrentState = std::make_unique<PlayerIdleState>();
    }

    void StateMachineComponent::Update(float deltaTime)
    {
        if (m_CurrentState)
            m_CurrentState->Update(GetOwner(), deltaTime);
    }

    void StateMachineComponent::ChangeState(std::unique_ptr<IState> newState)
    {
        if (m_CurrentState)
            m_CurrentState->Exit(GetOwner());
        m_CurrentState = std::move(newState);
        if (m_CurrentState)
            m_CurrentState->Enter(GetOwner());
    }

    IState* StateMachineComponent::GetCurrentState() const
    {
        return m_CurrentState.get();
    }
}