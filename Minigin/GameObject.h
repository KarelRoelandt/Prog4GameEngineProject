#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <type_traits> // For std::is_same_v
#include <iostream>

#include "BaseComponent.h"
#include "TransformComponent.h"

namespace dae
{
    // ANSI escape codes for coloring output (consider moving to a common utility header)
    const std::string ANSI_COLOR_RESET_GAMEOBJECT_H = "\033[0m";
    const std::string ANSI_COLOR_YELLOW_ORANGE_GAMEOBJECT_H = "\033[33m";
    const std::string WARNING_PREFIX_GAMEOBJECT_H = ANSI_COLOR_RESET_GAMEOBJECT_H + "[" + ANSI_COLOR_YELLOW_ORANGE_GAMEOBJECT_H + "WARNING" + ANSI_COLOR_RESET_GAMEOBJECT_H + "] ";


    class GameObject final
    {
    public:
        GameObject(); // Constructor will add TransformComponent by default
        ~GameObject();

        GameObject(const GameObject& other) = delete;
        GameObject(GameObject&& other) noexcept = delete;
        GameObject& operator=(const GameObject& other) = delete;
        GameObject& operator=(GameObject&& other) noexcept = delete;

        void Update(float deltaTime);
        void Render() const;

        template <typename T, typename... Args>
        std::shared_ptr<T> AddComponent(Args&&... args)
        {
            // Prevent adding multiple TransformComponents
            if constexpr (std::is_same_v<T, dae::TransformComponent>)
            {
                if (HasComponent<dae::TransformComponent>())
                {
                    std::cout << dae::WARNING_PREFIX_GAMEOBJECT_H << "GameObject::AddComponent - GameObject '" << m_Name << "' already has a TransformComponent. Returning existing one." << std::endl;
                    return GetComponent<dae::TransformComponent>();
                }
            }

            auto component = std::make_shared<T>(this, std::forward<Args>(args)...);
            m_Components[typeid(T)] = component;
            return component;
        }

        template <typename T>
        void RemoveComponent()
        {
            if constexpr (std::is_same_v<T, dae::TransformComponent>)
            {
                std::cout << dae::WARNING_PREFIX_GAMEOBJECT_H << "GameObject::RemoveComponent - Removing TransformComponent from GameObject '" << m_Name << "'. This is generally not recommended as it's auto-added and core to GameObject functionality." << std::endl;
            }
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

        GameObject* GetParent() const;
        // keepWorldPosition: if true, object maintains its world space transform.
        //                     if false, object's local transform is reset relative to new parent.
        void SetParent(GameObject* newParent, bool keepWorldPosition = true);

        const std::vector<GameObject*>& GetChildren() const;

        // Utility to quickly get transform, as it's always expected to be there
        // Returns a raw pointer as components are managed by shared_ptr internally.
        dae::TransformComponent* GetTransform() const;


    private:
        bool IsDescendant(GameObject* potentialDescendant) const;

        std::string m_Name;
        GameObject* m_Parent = nullptr;
        std::vector<GameObject*> m_Children; // Raw pointers to children

        // Components stored by their type_index for quick lookup
        std::unordered_map<std::type_index, std::shared_ptr<BaseComponent>> m_Components;
    };
}