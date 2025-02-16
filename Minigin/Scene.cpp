#include "Scene.h"
#include "GameObject.h"

namespace dae {
    Scene::Scene(const std::string& name)
        : m_name(name) {
    }

    void Scene::Add(std::shared_ptr<GameObject> object) {
        m_objects.push_back(object);
    }

    void Scene::Remove(std::shared_ptr<GameObject> object) {
        m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), object), m_objects.end());
    }

    void Scene::RemoveAll() {
        m_objects.clear();
    }

    void Scene::Update(float deltaTime) {
        for (auto& object : m_objects) {
            object->Update(deltaTime);
        }
    }

    void Scene::Render() const {
        for (const auto& object : m_objects) {
            object->Render();
        }
    }

    std::shared_ptr<GameObject> Scene::FindObjectByName(const std::string& name) const {
        for (const auto& object : m_objects) {
            if (object->GetName() == name) {
                return object;
            }
        }
        return nullptr;
    }

    const std::string& Scene::GetName() const { // Add this method
        return m_name;
    }

    Scene::~Scene() = default;
}
