// HealthDisplay.h
#pragma once
#include "BaseComponent.h"
#include "Observer.h"
#include "GameObject.h"
#include "TextComponent.h"

namespace dae
{
    class HealthDisplay final : public BaseComponent, public Observer
    {
    public:
        HealthDisplay(GameObject* owner, GameObject* /*gameObject*/, std::shared_ptr<TextComponent> textComponent);
        virtual ~HealthDisplay() = default;

        void OnNotify(const GameObject* entity, int value) override;

        // Implement the abstract methods from BaseComponent
        void Update(float deltaTime) override;
        void Render() const override;

    private:
        std::shared_ptr<TextComponent> m_textComponent;
    };
}