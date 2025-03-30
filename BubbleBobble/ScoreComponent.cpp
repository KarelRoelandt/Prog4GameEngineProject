// ScoreComponent.cpp
#include "ScoreComponent.h"
namespace dae
{
    ScoreComponent::ScoreComponent(GameObject* owner, GameObject* /*gameObject*/, int initialScore)
        : BaseComponent(owner), m_score(initialScore)
    {
    }

    void ScoreComponent::AddScore(int amount)
    {
        m_score += amount;
        NotifyObservers(GetOwner(), m_score);
    }

    void ScoreComponent::ResetScore()
    {
        m_score = 0;
        NotifyObservers(GetOwner(), m_score);
    }

    // Implementation of abstract methods
    void ScoreComponent::Update(float /*deltaTime*/)
    {
        // Score component doesn't need any per-frame updates
        // But we must implement this to satisfy the abstract base class
    }

    void ScoreComponent::Render() const
    {
        // Score component doesn't need rendering
        // But we must implement this to satisfy the abstract base class
    }
}