// HealthDisplay.cpp
#include "HealthDisplay.h"
#include <string>

namespace dae
{
    HealthDisplay::HealthDisplay(GameObject* owner, GameObject* /*gameObject*/, std::shared_ptr<TextComponent> textComponent)
        : BaseComponent(owner), m_textComponent(textComponent)
    {
    }

    void HealthDisplay::OnNotify(const GameObject* /*entity*/, int value)
    {
        // Update the text display with the new health value
        m_textComponent->SetText("# lives: " + std::to_string(value));
    }

    // Implementation of abstract methods
    void HealthDisplay::Update(float /*deltaTime*/)
    {
        // The display doesn't need per-frame updates
        // But we must implement this to satisfy the abstract base class
    }

    void HealthDisplay::Render() const
    {
        // The actual rendering is handled by the TextComponent
        // But we must implement this to satisfy the abstract base class
    }
}