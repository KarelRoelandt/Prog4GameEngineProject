#include "Scene.h"
#include "GameObject.h"

namespace dae
{
    unsigned int Scene::m_idCounter = 0;

    Scene::Scene(const std::string& name)
        : m_name(name)
    {
    }

    Scene::~Scene()
    {
        RemoveAll();
    }

    void Scene::Add(std::shared_ptr<GameObject> object)
    {
        object->SetScene(this);
        m_objects.push_back(object);
    }

    void Scene::Remove(std::shared_ptr<GameObject> object)
    {
        m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), object), m_objects.end());
    }

    void Scene::Remove(GameObject* object)
    {
        m_objects.erase(
            std::remove_if(m_objects.begin(), m_objects.end(),
                [object](const std::shared_ptr<GameObject>& ptr) { return ptr.get() == object; }),
            m_objects.end());
    }

    void Scene::QueueRemove(GameObject* object)
    {
        if (object && std::find(m_pendingRemoval.begin(), m_pendingRemoval.end(), object) == m_pendingRemoval.end())
            m_pendingRemoval.push_back(object);
    }

    void Scene::RemoveAll()
    {
        m_objects.clear();
    }

    void Scene::Update(float deltaTime)
    {
        for (auto& object : m_objects)
        {
            object->Update(deltaTime);
        }

        // Remove objects after update loop
        for (auto* obj : m_pendingRemoval)
        {
            Remove(obj);
        }
        m_pendingRemoval.clear();
    }

    void Scene::Render() const
    {
        for (const auto& object : m_objects)
        {
            object->Render();
        }
    }

    std::shared_ptr<GameObject> Scene::FindObjectByName(const std::string& name) const
    {
        for (const auto& object : m_objects)
        {
            if (object->GetName() == name)
            {
                return object;
            }
        }
        return nullptr;
    }
}
