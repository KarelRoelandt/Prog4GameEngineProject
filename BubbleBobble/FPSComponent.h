#pragma once

#include "BaseComponent.h"
#include "TextComponent.h"
#include <string>
#include <memory>

namespace dae
{
    class FPSComponent final : public BaseComponent
    {
    public:
        FPSComponent(GameObject* owner, std::shared_ptr<TextComponent> textComponent);
        ~FPSComponent() override = default;

        FPSComponent(const FPSComponent&) = delete;
        FPSComponent(FPSComponent&&) = delete;
        FPSComponent& operator=(const FPSComponent&) = delete;
        FPSComponent& operator=(FPSComponent&&) = delete;

        void Update(float deltaTime) override;
        void Render() const override {}

    private:
        std::shared_ptr<TextComponent> m_TextComponent; // Change to std::shared_ptr
        int m_FrameCount{ 0 };
        float m_ElapsedTime{ 0.0f };
    };
}
