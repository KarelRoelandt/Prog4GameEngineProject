#pragma once
#include "BaseComponent.h"
#include "Observer.h"
#include "GameObject.h"

namespace dae
{
    class HealthComponent final : public BaseComponent, public Subject
    {
    public:
        HealthComponent(GameObject* owner, GameObject* /*gameObject*/, int maxHealth = 3);
        ~HealthComponent() override = default;

        void TakeDamage(int amount);
        void Heal(int amount);
        int GetHealth() const { return m_health; }

        // Implement the abstract methods from BaseComponent
        void Update(float deltaTime) override;
        void Render() const override;

    private:
        int m_health;
        int m_maxHealth;
    };
}