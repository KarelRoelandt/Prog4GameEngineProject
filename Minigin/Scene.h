#pragma once
#include <vector>
#include <memory>
#include <string>

#include "SceneManager.h"

namespace dae {
    class GameObject;

    class Scene final {
        friend Scene& SceneManager::CreateScene(const std::string& name);
    public:
        explicit Scene(const std::string& name);

        void Add(std::shared_ptr<GameObject> object);
        void Remove(std::shared_ptr<GameObject> object);
        void RemoveAll();
        void Update(float deltaTime);
        void Render() const;
        std::shared_ptr<GameObject> FindObjectByName(const std::string& name) const;
        const std::string& GetName() const; // Add this line

        ~Scene();
        Scene(const Scene& other) = delete;
        Scene(Scene&& other) = delete;
        Scene& operator=(const Scene& other) = delete;
        Scene& operator=(Scene&& other) = delete;

    private:
        std::string m_name;
        std::vector<std::shared_ptr<GameObject>> m_objects;
        static unsigned int m_idCounter;
    };
}
