#include "FPSComponent.h"
#include <chrono>
#include <iostream> // For debug output
#include <sstream>  // For stringstream

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
        // std::cout << "DeltaTime: " << deltaTime << ", FrameCount: " << m_FrameCount << ", ElapsedTime: " << m_ElapsedTime << std::endl;

        if (m_ElapsedTime >= .25f) // Update FPS
        {
            float fps = m_FrameCount / m_ElapsedTime;
            std::stringstream fpsStream;
            fpsStream.precision(1);
            fpsStream << std::fixed << fps;
            m_TextComponent->SetText(fpsStream.str() + " FPS");

            // Debug output to verify FPS calculation
            // std::cout << "FPS: " << fpsStream.str() << std::endl;

            m_FrameCount = 0;
            m_ElapsedTime = 0.0f;
        }
    }
}