#pragma once
#include <memory>
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <string>
#include "BaseComponent.h"

namespace dae
{
    class Texture2D;

    class GameObject final
    {
    public:
        GameObject() = default;
        virtual ~GameObject() = default;

        GameObject(const GameObject&) = delete;
        GameObject(GameObject&&) noexcept = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject& operator=(GameObject&&) noexcept = delete;

        virtual void Update(float deltaTime);
        virtual void Render() const;

        template <typename T, typename... Args>
        T* AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<BaseComponent, T>::value, "T must inherit from BaseComponent");
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            component->SetOwner(this);
            T* componentPtr = component.get();
            m_Components[typeid(T)] = std::move(component);
            return componentPtr;
        }

        template <typename T>
        void RemoveComponent()
        {
            m_Components.erase(typeid(T));
        }

        template <typename T>
        T* GetComponent() const
        {
            auto it = m_Components.find(typeid(T));
            if (it != m_Components.end())
            {
                return dynamic_cast<T*>(it->second.get());
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

    private:
        std::unordered_map<std::type_index, std::unique_ptr<BaseComponent>> m_Components;
        std::string m_Name;
    };
}
