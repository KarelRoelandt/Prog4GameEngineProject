#include "ScoreDisplay.h"
#include <string>

namespace dae
{
    ScoreDisplay::ScoreDisplay(GameObject* owner, std::shared_ptr<TextComponent> textComponent)
        : BaseComponent(owner), m_textComponent(textComponent)
    {
    }

    void ScoreDisplay::OnNotify(const GameObject* /*entity*/, int value)
    {
        // Update the text display with the new score value
        m_textComponent->SetText("Score: " + std::to_string(value));
    }

    // Implementation of abstract methods
    void ScoreDisplay::Update(float /*deltaTime*/)
    {
        // The display doesn't need per-frame updates
        // But we must implement this to satisfy the abstract base class
    }

    void ScoreDisplay::Render() const
    {
        // The actual rendering is handled by the TextComponent
        // But we must implement this to satisfy the abstract base class
    }
}