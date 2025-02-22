#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>
#include "BaseComponent.h"

namespace dae
{
    class GameObject final
    {
    public:
        GameObject() = default;
        ~GameObject() = default;

        void Update(float deltaTime);
        void Render() const;

        template <typename T, typename... Args>
        std::shared_ptr<T> AddComponent(Args&&... args)
        {
            auto component = std::make_shared<T>(this, std::forward<Args>(args)...);
            m_Components[typeid(T)] = component;
            return component;
        }

        template <typename T>
        void RemoveComponent()
        {
            m_Components.erase(typeid(T));
        }

        template <typename T>
        std::shared_ptr<T> GetComponent() const
        {
            auto it = m_Components.find(typeid(T));
            if (it != m_Components.end())
            {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr;
        }

        template <typename T>
        bool HasComponent() const
        {
            return m_Components.find(typeid(T)) != m_Components.end();
        }

        void SetName(const std::string& name);
        const std::string& GetName() const;

        // Add the declarations for GetParent and SetParent
        GameObject* GetParent() const;
        void SetParent(GameObject* parent);

    private:
        std::unordered_map<std::type_index, std::shared_ptr<BaseComponent>> m_Components;
        std::string m_Name;
        GameObject* m_Parent = nullptr; // Declare m_Parent
    };
}
