#include "SceneManager.h"
#include "Scene.h"
#include <stdexcept>

namespace dae {
    Scene& SceneManager::CreateScene(const std::string& name) {
        auto scene = std::make_shared<Scene>(name);
        m_scenes.push_back(scene);
        return *scene;
    }

    Scene& SceneManager::GetScene(const std::string& name) {
        for (const auto& scene : m_scenes) {
            if (scene->GetName() == name) {
                return *scene;
            }
        }
        throw std::runtime_error("Scene not found: " + name);
    }

    void SceneManager::Update(float deltaTime) {
        for (auto& scene : m_scenes) {
            scene->Update(deltaTime);
        }
    }

    void SceneManager::Render() {
        for (const auto& scene : m_scenes) {
            scene->Render();
        }
    }
}
