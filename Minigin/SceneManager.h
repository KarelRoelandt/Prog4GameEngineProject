#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Singleton.h"

namespace dae
{
    class Scene;
    class SceneManager final : public Singleton<SceneManager>
    {
    public:
        SceneManager() = default;
        ~SceneManager() = default;

        SceneManager(const SceneManager&) = delete;
        SceneManager(SceneManager&&) noexcept = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager& operator=(SceneManager&&) noexcept = delete;

        Scene& CreateScene(const std::string& name);
        void DestroyScene(const std::string& name);
        Scene& GetScene(const std::string& name);

        void Update(float deltaTime);
        void Render();

    private:
        friend class Singleton<SceneManager>;
        std::vector<std::shared_ptr<Scene>> m_scenes;
    };
}