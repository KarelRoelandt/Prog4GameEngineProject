#pragma once
#include <string>
#include <vector>
#include <memory>

namespace dae
{
    class GameObject;

    class Scene final
    {
    public:
        explicit Scene(const std::string& name);
        ~Scene();

        Scene(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene& operator=(Scene&&) = delete;

        void Add(std::shared_ptr<GameObject> object);
        void Remove(std::shared_ptr<GameObject> object);
        void RemoveAll();

        void Update(float deltaTime);
        void Render() const;

        std::shared_ptr<GameObject> FindObjectByName(const std::string& name) const;
        const std::string& GetName() const { return m_name; }

    private:
        std::string m_name;
        std::vector<std::shared_ptr<GameObject>> m_objects;
        static unsigned int m_idCounter;
    };
}
