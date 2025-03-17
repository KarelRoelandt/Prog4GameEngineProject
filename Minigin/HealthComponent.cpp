// HealthComponent.cpp
#include "HealthComponent.h"

namespace dae
{
    HealthComponent::HealthComponent(GameObject* owner, GameObject* /*gameObject*/, int maxHealth)
        : BaseComponent(owner), m_maxHealth(maxHealth), m_health(maxHealth)
    {
    }

    void HealthComponent::TakeDamage(int amount)
    {
        m_health = std::max(0, m_health - amount);
        NotifyObservers(GetOwner(), m_health);
    }

    void HealthComponent::Heal(int amount)
    {
        m_health = std::min(m_maxHealth, m_health + amount);
        NotifyObservers(GetOwner(), m_health);
    }

    // Implementation of abstract methods
    void HealthComponent::Update(float /*deltaTime*/)
    {
        // Health component doesn't need any per-frame updates
        // But we must implement this to satisfy the abstract base class
    }

    void HealthComponent::Render() const
    {
        // Health component doesn't need rendering
        // But we must implement this to satisfy the abstract base class
    }
}