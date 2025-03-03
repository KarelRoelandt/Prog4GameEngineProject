#pragma once
#include <numeric>
#include "BaseComponent.h"
#include "imgui.h"
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include "ThrashTheCache.h"

namespace dae
{
    class GameObject; // Forward declaration

    class ImGuiComponent : public BaseComponent
    {
    public:
        ImGuiComponent(GameObject* owner);

        void Update(float /*deltaTime*/) override {}
        void Render() const override;

    private:
        ThrashTheCache m_TrashTheCache;
    };
}