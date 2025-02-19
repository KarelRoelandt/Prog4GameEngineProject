#include "FPSComponent.h"
#include <chrono>
#include <iostream> // For debug output

namespace dae
{
    FPSComponent::FPSComponent(GameObject* owner, std::shared_ptr<TextComponent> textComponent)
        : BaseComponent(owner), m_TextComponent(std::move(textComponent)) // Store shared_ptr
    {
    }

    void FPSComponent::Update(float deltaTime)
    {
        m_FrameCount++;
        m_ElapsedTime += deltaTime;

        // Debug output to verify values
        std::cout << "DeltaTime: " << deltaTime << ", FrameCount: " << m_FrameCount << ", ElapsedTime: " << m_ElapsedTime << std::endl;

        if (m_ElapsedTime >= 0.1f) // Update FPS every second
        {
            float fps = m_FrameCount / m_ElapsedTime;
            std::string fpsString = std::to_string(fps);
            size_t dotPos = fpsString.find('.');
            if (dotPos != std::string::npos && dotPos + 2 < fpsString.size())
            {
                fpsString = fpsString.substr(0, dotPos + 2); // Keep one decimal place
            }
            m_TextComponent->SetText(fpsString + " FPS");

            // Debug output to verify FPS calculation
            std::cout << "FPS: " << fpsString << std::endl;

            m_FrameCount = 0;
            m_ElapsedTime = 0.0f;
        }
    }
}
