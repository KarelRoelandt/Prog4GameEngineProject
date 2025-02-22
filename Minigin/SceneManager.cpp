#include "SceneManager.h"
#include "Scene.h"
#include <stdexcept>
#include <algorithm>

namespace dae
{
    Scene& SceneManager::CreateScene(const std::string& name)
    {
        auto scene = std::make_shared<Scene>(name);
        m_scenes.push_back(scene);
        return *scene;
    }

    void SceneManager::DestroyScene(const std::string& name)
    {
        auto it = std::remove_if(m_scenes.begin(), m_scenes.end(), [&name](const std::shared_ptr<Scene>& scene) {
            return scene->GetName() == name;
            });

        if (it != m_scenes.end())
        {
            m_scenes.erase(it, m_scenes.end());
        }
    }

    Scene& SceneManager::GetScene(const std::string& name)
    {
        for (const auto& scene : m_scenes)
        {
            if (scene->GetName() == name)
            {
                return *scene;
            }
        }
        throw std::runtime_error("Scene not found: " + name);
    }

    void SceneManager::Update(float deltaTime)
    {
        for (auto& scene : m_scenes)
        {
            scene->Update(deltaTime);
        }
    }

    void SceneManager::Render()
    {
        for (const auto& scene : m_scenes)
        {
            scene->Render();
        }
    }
}
