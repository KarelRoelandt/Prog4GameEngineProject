#include "ImGuiComponent.h"
#include "GameObject.h" // Include the full definition of GameObject

namespace dae
{
    ImGuiComponent::ImGuiComponent(GameObject* owner)
        : BaseComponent(owner)
    {
        // Initial calculation
        //m_TrashTheCache.CalculateEx1();
    }

    void ImGuiComponent::Render() const
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        m_TrashTheCache.RenderImGui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}